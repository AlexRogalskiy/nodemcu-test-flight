// This is code for battery powered ESP8266 with DHT21 senzor on port D4 (IO2) using blynk cloud solution for data representation
// As deepsleep is used you need to connect D0 (IO16) to RST by 1kohm resistor so device can wake up from deepsleep
// Version 2.0
// by Miro Smola, smolam@gmail.com
#define BLYNK_PRINT Serial                        // you can comment out, its for terminal window only
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

ADC_MODE(ADC_VCC);                                // enabled measure of 3.3 V level on A0 port but disables that port for ussage

#define DHTPIN 2                                  // IO2 is port D4 where data pin of DHT senzor is connected
#define DHTTYPE DHT21                             // I use DHT21, change to other DHT senzor if you use other one
DHT dht(DHTPIN, DHTTYPE);                         // Enabled DHT

char auth[] = "code"; // FILL in your BLYNK code 

char ssid[] = "ssid";                             // your wifi SSID
char pass[] = "passwd";                        // your wifi password

void setup()
{
  Serial.begin(9600);                             // start serial console for debugging purpose
  Serial.println("started");                      // send to serial control messsage
  WiFi.begin(ssid, pass);                         // start wifi connection
  for (int i=0; i <= 100; i++) {                  // Start of 10 seconds connection to wifi phase. If your wifi needs more time to connect to, change this number to secods * 10
    if (WiFi.status() != WL_CONNECTED) {          // Check if wifi is in not connected status
      delay(100);                                 // wait 0.1 s - this is reason for multiplying by 10 the time in for cycle
      Serial.print(".");                          // writes dots to serial console if wifi is not connected yet
      if ( i > 95 ) {                             // I wait here for 9,5 second to get wifi connection, set your timer by rewriting timer with seconds * 10 if your wifi conenction needs more time
        Serial.println("FORCE SLEEP");            // send to serial console control message
        ESP.deepSleep(600000000);                 // put device to 10 minutes sleep, adjust if other sleep time is reqired is wifi fails to conenct
        delay(200);                               // recommended to use with deepsleep
      }
    }
    else {i = 500;}                               // ones connected to wifi the for cycle is ended by incresing i over high limit
    }
  Blynk.config(auth);                             // set blynk authentification string
  Serial.println("connecting to blynk");          // send to serial control messsage
  Blynk.connect();                                // start connection to Blynk server, this command has 30 secons timeout
  Serial.println("end of setup");                 // send to serial control messsage
}

void sendTemperature()                            // temperature measuring function
{
  float h = dht.readHumidity();                   // reads humidity from senzor and save to h
  float t = dht.readTemperature();                // reads temperature from senzor and save to t
  float power = (ESP.getVcc());                   // reads voltage on 3.3 V level
  Serial.println("measure");                      // send to serial results of measurings
  Serial.print(t);            
  Serial.print(" C, ");
  Serial.print(h);
  Serial.println(" %");
  Serial.println(power);
  Blynk.virtualWrite(6, power);
  if (isnan(h) or isnan(t)) {                     // checks if readings from sensors were obtained
    // cant read sensor
  }
  else {
  Blynk.virtualWrite(0, t);                       // send to Blynk virtual pin 0 temperature value
  Blynk.virtualWrite(1, h);                       // send to Blynk virtual pin 1 humidity value
  }
  Serial.println("success sleep");                // send to serial control messsage
  ESP.deepSleep(600000000);                       // put device to 10 minutes sleep, adjust if other sleep time is reqired between measurements
  delay(200);                                     // recommended to use with deepsleep
}

void loop()
{
  Blynk.run();                                    // run blynk
  sendTemperature();                              // calls measure temperature function
}

