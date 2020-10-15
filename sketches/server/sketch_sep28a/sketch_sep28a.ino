/*
  ESP8266 Blink by Simon Peter
  Blink the blue LED on the ESP-01 module
  This example code is in the public domain
  The blue LED on the ESP-01 module is connected to GPIO1
  (which is also the TXD pin; so we cannot use Serial.print() at the same time)
  Note that this sketch uses LED_BUILTIN to find the pin with the internal LED
*/

#include <ESP8266WiFi.h>
//#include <PubSubClient.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <ArduinoJson.h>

const char* ssid = "Pantera";
const char* password = "YEzpe93r";
const char* mqtt_server = "1.1.1.1";
const int mqtt_port = 1883;

String path = "home/sensors/";

WiFiClient espClient;
//PubSubClient client(espClient);
Adafruit_BMP280 bme; // I2C

unsigned long last_measurement = 0;

void reconnect() {
  // Loop until we're reconnected
  
  /*while (!client.connected()) {
    // Create a random client ID
    String clientId = "ESP8266Sensor-";
    clientId += String(random(0xffff), HEX);
        
    Serial.printf("Attempting MQTT connection to %s:%d from my ip\n", mqtt_server, mqtt_port);
    Serial.println(WiFi.localIP());

    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
         
      // Wait 5 seconds before retrying
      delay(5000);
    }*/
  //}
}


void setup() {
  Serial.begin(74880);
  Serial.setDebugOutput(true);
  delay(10);
  Serial.println("Setup( )");
  byte count = 0;
  if (!bme.begin(0x76, 0x60)) {  // This requires this patch https://github.com/adafruit/Adafruit_BMP280_Library/pull/10
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
  }

  //Wire.begin();
  for (byte i = 1; i < 120; i++)
  {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
      {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      count++;
      delay (1);  // maybe unneeded?
      } // end of good response
  } // end of for loop
  Serial.println ("Done.");
  Serial.print ("Found ");
  Serial.print (count, DEC);
  Serial.println (" device(s).");
  
  WiFi.persistent(false);
  WiFi.setOutputPower(0);
  WiFi.begin(ssid, password);
  delay(1000);

  byte i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    i++;
    Serial.printf("Trying to connect to %s with pass %s\n", ssid, password);
   
    if (i > 10) {
      Serial.println("Resetting wifi");
      WiFi.mode(WIFI_OFF);   // this is a temporary line, to be removed after SDK update to 1.5.4
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);
      i = 0;
      Serial.println("wifi reset done.");
    }
  }

  Serial.print(F("My IP is: "));
  Serial.println(WiFi.localIP());

  path += WiFi.macAddress();
  randomSeed(micros());
  //client.setServer(mqtt_server, mqtt_port);
}

// the loop function runs over and over again forever
void loop() {
  //if (!client.connected()) {
  //  reconnect();
  //}
  //client.loop();

  if (last_measurement + 60 * 1000 < millis()) {

    float temp = bme.readTemperature();
    float pressure = bme.readPressure();
  
    //StaticJsonBuffer<200> jsonBuffer;
    
    //JsonObject& root = jsonBuffer.createObject();
   // root["temp"] = temp;
   // root["pressure"] = double_with_n_digits(pressure * 0.01f, 6); // Hectopascals
    String sensorpath = path + "/bmp280";
    String output;
    //root.printTo(output);
    Serial.println(output);
    Serial.println("Pressure in Hectopascals, Temperature in Celcius");
    //client.publish(sensorpath.c_str(), output.c_str());
    last_measurement = millis();
  }
  delay(1000);
}
