/* Copyright Augusto Ciuffoletti 2019 */
//#include <Arduino.h>
//#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h> // https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266HTTPClient
#include <WiFiClientSecureBearSSL.h> // https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/bearssl-client-secure-class.html
#include <ArduinoJson.h>
#include "oled.h"
#include "secret.h"
#include "pubkey.h"

#define PUMP D5
#define STITCH_URL "eu-west-1.aws.webhooks.mongodb-stitch.com/api/client/v2.0/app/manage-jjtug/service/plantControl/incoming_webhook"

char *plant="test";
//char *plant="bouganville"; 
//char *plant="balcone"; 
//char *plant="benjamin";


// Local configuration
const time_t minWait=30;   // minimum wait time (in sec) (default 60)
//const time_t maxWait=600;
const time_t maxWait=1800;  // maximum wait time (in sec) (default 3595)
char oled_buffer[16]; 

WiFiClient client;
HTTPClient https;

//const char *mongoDBapi="tZNAI3OdkoqPEA3QIa2yMzXtWZ8tAeY3";
// see https://github.com/esp8266/Arduino/blob/master/doc/esp8266wifi/bearssl-client-secure-class.rst
// Fingerprint for api.mlab.com
//const uint8_t fingerprint[20] = {0x16, 0x6E, 0xA6, 0xE7, 0xF3, 0x7A, 0x41, 0x20, 0xC7, 0xF8, 0x7E, 0x6D, 0x85, 0xE9, 0x2E, 0x44, 0xF0, 0xF2, 0xBF, 0x8A};
// Fingerprint for mongDB webhook
// const uint8_t fingerprint[20] = {0x73, 0x5D, 0x6B, 0xA2, 0xF7, 0xED, 0x7C, 0x72, 0x74, 0xAC, 0xA3, 0xF5, 0x67, 0xF0, 0x56, 0x6B, 0x68, 0x3B, 0x4B, 0x47};

// This structure is downloaded from mongoDB, indexed on the "plant" string
struct {
  time_t timeToWait;  // time before next activation (in seconds) (TO DO)
  time_t duration;    // duration of watering (in seconds)
  int imMax;          // maximum pump current (in mA)
  int imMin;          // minimum pump current (in mA)
  int TZ;             // location timezone
} softState;

// This structure is uploaded to mongoDB
struct {
  float current;              // Sensed pump current
  boolean emptyTank;       // Tank empty flag
} report;

int joinAP(int retry){
  Serial.print("Connecting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(MYSSID, MYPASSWD);
  while ( ( WiFi.status() != WL_CONNECTED ) && ((retry--) > 0) )
  {
    delay(500);
    Serial.print(".");
  }
  if ( WiFi.status() != WL_CONNECTED ) {
    Serial.println("Can't connect to AP");
    return 1;
  }
  Serial.println();
  Serial.print("Connected as ");
  Serial.println(WiFi.localIP());
  digitalWrite(LED_BUILTIN,LOW);
  return 0;
}

int disconnectAP(){
  WiFi.disconnect();
  while (WiFi.status() == WL_CONNECTED) delay(1000);
  digitalWrite(LED_BUILTIN,HIGH);
}

int parseState(String json) {
// From https://arduinojson.org/v6/assistant/
  const size_t capacity = JSON_OBJECT_SIZE(5) + 50;
//  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(4) + 40;
  DynamicJsonDocument doc(capacity);
  Serial.print(https.getString());
  deserializeJson(doc, json);//https.getString());
//  Serial.println(" deserialized");
  softState.timeToWait = atol(doc["ttw"]); //
  softState.duration = atol(doc["D"]); // 24
  softState.imMax = atoi(doc["imMax"]); // 2
  softState.imMin = atoi(doc["imMin"]); // 2
  softState.TZ = atoi(doc["TZ"]);
//    Serial.println("Extracted");
}

// returns 0 if successful, or -1 in case of failure
// Next state is in SoftState
int sendReport() {
  char endpoint[200];
  BearSSL::WiFiClientSecure client;
//  BearSSL::PublicKey key(pubkey);
//  client.setKnownKey(&key);
//  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
//  client->setFingerprint(fingerprint);
  client.setInsecure();
  sprintf(endpoint,"https://%s/report_secure?plant=%s&Im=%d&emptyTank=%d&secret=%s",
    STITCH_URL,
    plant,
    (int)round(report.current),
    report.emptyTank,
    SECRET);
  if (https.begin(client, endpoint)) {
//    Serial.println(endpoint);
    int x=https.GET();
    if (x == 200) {
      Serial.println("Report successful.");
      parseState(https.getString());
      return 0;
    } else {
      Serial.println("Problem reporting. HTTP error code " + String(x));
      return 1;
    }
    https.end();
  }
  return 1;
}

int download() {
  char endpoint[200];
  BearSSL::WiFiClientSecure client;
//  BearSSL::PublicKey key(pubkey);
//  client.setKnownKey(&key);
//  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
// client->setFingerprint(fingerprint);
  client.setInsecure();
// endpoint for mongoDB webhook
  sprintf(endpoint,"https://%s/download?plant=%s&secret=%s",STITCH_URL,plant,SECRET);
//  Serial.println(endpoint);
  if (https.begin(client, endpoint)) {
    int x=https.GET();
    if(x == 200) {
      Serial.println("MongoDB softState read successful.");
      parseState(https.getString());
      return 0;
    } else {
      Serial.println("Problem reading softState. HTTP error code " + String(x));
      return 1;
    }
  }
  https.end();
  return 1;
}

int deepSleep(time_t timeToWait) {
    if ( timeToWait > minWait ) {
        if ( timeToWait > (maxWait+minWait) ) {
          Serial.print("Max wait (deep sleep): "); Serial.print(maxWait); Serial.println(" seconds");
          ESP.deepSleep(maxWait*1E6); //deepSleep is in microseconds
        }
        else {
          timeToWait-=minWait;  // necessarily > 0
          Serial.print("Short wait (deep sleep): "); Serial.print(timeToWait); Serial.println(" seconds");
          ESP.deepSleep(timeToWait*1E6);
        }
      } else {
        Serial.print("It's final countdown: "); Serial.print(timeToWait); Serial.println(" seconds");
        delay(timeToWait*1E3); // active wait when TimeToWait<minWait
        return 0;
      }
}

#define RELAY D5     // Digital pin for relay control
#define VCC 3300.0   // mV
/*
 * La pompa viene attivata per l'intervallo di tempo indicato (timer).
 * In serie alla pompa, dal lato della terra, si inserisce una
 * resistenza da 1 Ohm. Il capo della resistenza che non è a terra
 * viene portato su un input analogico (A0) e quindi il valore su
 * A0 corrisponde all'assorbimento del motore in mA. Un 
 * assorbimento anomalo indica serbatoio vuoto, distacco del tubo,
 * fori di irrigazione tappati. Se il valore sul sensore è inferiore
 * ad un valore di soglia dato, la funzione termina con segnalazione
 * di errore.
 * 
 * timer: tempo di funzionamento (in secondi)
 * check: puntatore ad una variabile di tipo int. In ingresso
 *        viene riportato il valore di soglia per interrompere
 *        il funzionamento del motore. Se il valore al sensore
 *        è inferiore alla soglia il motore viene disattivato dopo
 *        due periodi di campionamento (20 secondi) e si 
 *        restituisce il valore 1.
 *        Altrimenti viene portato a termine il ciclo di
 *        funzionamento e si restituisce nella variabile puntata da
 *        check il valore medio del sensore.
 * return: 0 se Ok, altrimenti 1
 */
int pump(unsigned long timer, float *average) {
  int fail=0;
  int Dt=1000; // loop period in ms
  unsigned long n=0;
  double current;
  *average=0;
// Begin motor on period
  digitalWrite(PUMP, HIGH);
  for (n=0; n<(timer*1000)/Dt; n++ ) {
    current=(analogRead(A0))*(VCC/1024.0);
    if (n>(5000/Dt)) {                               // Non considera nella media i primi 5 sec di funzionamento (transitori)
      *average-=(*average-current)/(n+1-(5000/Dt));
    } else {
      *average=current;
    }
    Serial.println(current);
    Serial.println(*average);
    if (current > softState.imMin && current < softState.imMax) {
//      digitalWrite(LED_BUILTIN,HIGH);
      fail=0;
    } else {
      if ( fail>3 ) {
      digitalWrite(PUMP, LOW);
      Serial.println("Serbatoio vuoto!");
      return 1;
      }
      fail++;
    }
    delay(Dt);
  }
  digitalWrite(RELAY, LOW);    // turn the LED off by making the voltage LOW
//  digitalWrite(LED_BUILTIN,HIGH);
  return 0;
}

void setup() {
  long int timeToWait;
  delay(1000);
  pinMode(PUMP, OUTPUT);
  oled_init();
  digitalWrite(PUMP, LOW);
  Serial.begin(115200);
  Serial.println("\n========\nDEEP_SLEEP_stitch (7115cf8)");
  Serial.print("Plant: ");Serial.println(plant);
  oled_display(0,plant);
  oled_display(1,"Connecting");
  if ( joinAP(30) ) ESP.deepSleep(10*1E6); // deep sleep 10 seconds
  oled_display(1,"Download");
  while ( download() ) {
    Serial.println("Retry state download"); 
    delay(1000);
    }
  Serial.print("timeToWait = "); Serial.println(softState.timeToWait);
  Serial.print("duration = "); Serial.println(softState.duration);
  Serial.print("imMax = "); Serial.println(softState.imMax);
  Serial.print("imMin = "); Serial.println(softState.imMin);
  Serial.print("TZ = "); Serial.println(softState.TZ);
    
  sprintf(oled_buffer,"ttw=%05d",softState.timeToWait);
  oled_display(2,oled_buffer);
  sprintf(oled_buffer,"D=%05d",softState.duration);
  oled_display(3,oled_buffer);
  oled_display(1,"Sleeping");
  while ( deepSleep(softState.timeToWait) == 0 ) {
    // enters only when time for action
    // ==== ACTION ====
    oled_display(2,"Running");
    Serial.println("RUNNING ACTION NOW");
    pump(softState.duration, &(report.current));
    while ( sendReport() ) {
      Serial.println("report retry"); 
      delay(1000);
    }
    disconnectAP();
    Serial.println(softState.timeToWait);
    sprintf(oled_buffer,"ttw=%05d",softState.timeToWait);
    oled_display(2,oled_buffer);
    oled_display(1,"Sleeping");
  } 
}

void loop() {
  Serial.println("Error");
  delay(1000);
}
