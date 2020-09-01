// This is code for ESP8266 with DHT22 senzor on port D4 (IO2) using blynk cloud solution for data representation with OLED wemos D1 mini shield.
// Version 1.0
// by Miro Smola, smolam@gmail.com
#define BLYNK_PRINT Serial  // you can comment out, its for terminal window only
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include <DHT.h>
#include <Wire.h>
#include <SFE_MicroOLED.h>

#define DHTPIN 2                                            // port where DTH22 is connected (D4 is IO2 on wemos D1 mini)
#define DHTTYPE DHT22                                       // I use DHT22, change to other DHT senzor if you use other one
DHT dht(DHTPIN, DHTTYPE);                                   // enabled DHT sensor

char auth[] = "auth";                                       // Your Blynk authentication string

char ssid[] = "ssid";                                       // SSID (name) of your wifi
char pass[] = "password";                                   // your wifi password

SimpleTimer timer;                                          // enable simple timer

#define PIN_RESET 255                                       // Code for OLED inicialization
#define DC_JUMPER 0
MicroOLED oled(PIN_RESET, DC_JUMPER);  

void setup()
{
  Serial.begin(9600);                                       // Enabled serial debugging output
  Blynk.begin(auth, ssid, pass);                            // Connects to wifi and Blynk services

  oled.begin();                                             // start oled display
  oled.clear(PAGE);                                         // clears oled cash
  oled.clear(ALL);
  oled.setFontType(0);                                      // set small font
  oled.setCursor(0,40);                                     // set pen coordinates
  oled.print("bronka");                                     // puts name of senzor/room at the bottom of display to cache
  oled.display();                                           // display whats in cache
  
  timer.setInterval(10000L, sendTemperature);               // every 10 seconds run function sendTemperature
  
}

void sendTemperature()                                      // function send temperature
{
  float h = dht.readHumidity();                             // reads humidity from senzor and save to h
  float t = dht.readTemperature();                          // reads temperature from senzor and save to t
  
  if (isnan(h) or isnan(t)) {                               // checks if readings from sensors were obtained
    // cant read sensor
  }
  else {
  Blynk.virtualWrite(1, t);                                 // send to Blynk virtual pin 1 temperature value
  Blynk.virtualWrite(3, h);                                 // send to Blynk virtual pin 3 humidity value
  oled.setFontType(2);                                      // set big font type 2
  oled.setCursor(0,0);                                      // set pen coordinates fot temperature readings
  oled.print(t);                                            // puts puts temperature value to display cache
  oled.setCursor(0,21);                                     // set pen coordinates fot humidity readings
  oled.print(h);                                            // puts puts humidity value to display cache
  oled.display();                                           // display whats in cache
  }  
}

void loop()
{
  Blynk.run();                                              // run blynk
  timer.run();                                              // run timer
}

