#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define GPIO_D1_pin 5

MDNSResponder mdns;

const char* ssid = "Pantera";
const char* password = "YEzpe93r";

ESP8266WebServer server(80);

String webPage = "";

void setup(void) {
  webPage += F(
            "<!DOCTYPE html><html lang='en'><head>"
            "<meta name='viewport' content='width=device-width'>"
            "<title>Captive Portal</title>"
            "</head><body>"
            "<h1>You are CONNECTED</h1>");

  // подготавливаем GPIO-контакты:
  pinMode(GPIO_D1_pin, OUTPUT);
  digitalWrite(GPIO_D1_pin, LOW);

  delay(1000);
  Serial.begin(74880);
  Serial.setDebugOutput(true);
  WiFi.begin(ssid, password);
  Serial.println("");

  // ждем соединения:
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");  //  "Подключились к "
  Serial.println(ssid);
  Serial.print("IP address: ");  //  "IP - адрес: "
  Serial.println(WiFi.localIP());

  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

  server.on(" / ", []() {
    server.send(200, "text / html", webPage);
  });
  server.on(" / on", []() {
    server.send(200, "text / html", webPage);
    digitalWrite(GPIO_D1_pin, HIGH);
  });
  server.on(" / off", []() {
    server.send(200, "text / html", webPage);
    digitalWrite(GPIO_D1_pin, LOW);
  });
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
