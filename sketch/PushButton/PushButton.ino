/*
 * ESP8266 Wi-Fi Button
 * Rui Santos 
 * Complete Project Details https://randomnerdtutorials.com
 */
 
#include <ESP8266WiFi.h>

// Replace with your SSID and Password
const char* ssid     = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// Replace with your unique IFTTT URL resource
const char* resource = "REPLACE_WITH_YOUR_IFTTT_URL_RESOURCE";

// How your resource variable should look like, but with your own API KEY (that API KEY below is just an example):
//const char* resource = "/trigger/button_pressed/with/key/nAZjOphL3d-ZO4N3k64-1A7gTlNSrxMJdmqy";

// Maker Webhooks IFTTT
const char* server = "maker.ifttt.com";

void setup() {
  Serial.begin(115200); 

  initWifi();
  makeIFTTTRequest();

  // Deep sleep mode until RESET pin is connected to a LOW signal (pushbutton is pressed)
  ESP.deepSleep(0);
}

void loop() {
  // sleeping so wont get here
}

// Establish a Wi-Fi connection with your router
void initWifi() {
  Serial.print("Connecting to: "); 
  Serial.print(ssid);
  WiFi.begin(ssid, password);  

  int timeout = 10 * 4; // 10 seconds
  while(WiFi.status() != WL_CONNECTED  && (timeout-- > 0)) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");

  if(WiFi.status() != WL_CONNECTED) {
     Serial.println("Failed to connect, going back to sleep");
  }

  Serial.print("WiFi connected in: "); 
  Serial.print(millis());
  Serial.print(", IP address: "); 
  Serial.println(WiFi.localIP());
}

// Make an HTTP request to the IFTTT web service
void makeIFTTTRequest() {
  Serial.print("Connecting to "); 
  Serial.print(server);
  
  WiFiClient client;
  int retries = 5;
  while(!!!client.connect(server, 80) && (retries-- > 0)) {
    Seri