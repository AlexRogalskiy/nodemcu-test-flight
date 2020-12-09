/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

// Import required libraries
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <FS.h>
#include <Wire.h>
#include <RCSwitch.h>

#ifndef APSSID
#define APSSID "Konio5"
#define APPSK  "trustico"
#endif

/* Set these to your desired credentials. */
const char* ssid = APSSID;
const char* password = APPSK;
const char* host = "automata";

/* Soft AP network parameters */
IPAddress apIP(192, 168, 5, 1);
IPAddress netMsk(255, 255, 255, 0);

// Set LED GPIO
const int ledPin = 13;
// Stores LED state
String ledState;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

RCSwitch mySwitch = RCSwitch();

// Replaces placeholder with LED state value
String processor(const String& var) {
  Serial.println(var);
  if (var == "STATE") {
    if (digitalRead(ledPin)) {
      ledState = "OFF";
    }
    else {
      ledState = "ON";
    }
    Serial.print(ledState);
    return ledState;
  }
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(74880);
  Serial.setDebugOutput(true);
  delay(10);

  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);

  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(ssid, password);

  IPAddress localIp = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(localIp);

  // MDNS INIT
  if (MDNS.begin(host)) {
    MDNS.addService("http", "tcp", 80);
    Serial.print(F("Open http://"));
    Serial.print(host);
    Serial.printf("Ready! Open http://%s.local in your browser\n", host);
  }

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(ledPin, LOW);
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(ledPin, HIGH);
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Start server
  server.begin();
}

void loop() {
  if (mySwitch.available()) {
    Serial.println("Receiving data from remote device");
    output(mySwitch.getReceivedValue(), mySwitch.getReceivedBitlength(), mySwitch.getReceivedDelay(), mySwitch.getReceivedRawdata(), mySwitch.getReceivedProtocol());
    mySwitch.resetAvailable();
  }

  //MDNS.update();
}
