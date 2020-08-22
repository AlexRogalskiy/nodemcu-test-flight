#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>

#ifndef APSSID
#define APSSID "Konio"
#define APPSK  "trust123s"
#endif

#define GPIO_D1 5

// Indicate which digital I/Os should be displayed on the chart.
// From GPIO16 to GPIO0, a '1' means the corresponding GPIO will be shown
// e.g. 0b11111000000111111
unsigned int gpioMask;

/* Set these to your desired credentials. */
const char* ssid = APSSID;
const char* password = APPSK;
const char* host = "automata";
const char* softAP_ssid = "Pantera";

static const char TEXT_PLAIN[] PROGMEM = "text/plain";
static const char TEXT_JSON[] PROGMEM = "text/json";
static const char TEXT_HTML[] PROGMEM = "text/html";

/* Soft AP network parameters */
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

ESP8266WebServer server(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/
void handleRoot() {
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");

  // Match the request
  int val = digitalRead(GPIO_D1);
  
  String Page;
  Page += F(
            "<!DOCTYPE html><html lang='en'><head>"
            "<meta name='viewport' content='width=device-width'>"
            "<title>Captive Portal</title>"
            "</head><body>"
            "<h1>You are CONNECTED</h1>");
  Page += String(F("<p>GPIO is now: ")) + ((val) ? F("high") : F("low")) + F("</p>");
  Page += String(F("<br><br>Click <a href='https://")) + toStringIp(WiFi.localIP()) + F("/on'>here</a> to switch LIGHT on");
  Page += String(F("<br>or <a href='https://")) + toStringIp(WiFi.localIP()) + F("/off'>here</a> to switch LIGHT off");
  Page += String(F("</body></html>"));

  server.send(200, "text/html", Page);
}

void handleLightOff() {
  digitalWrite(GPIO_D1, 1);
  redirect();
//  server.send(200, "text/html", "<h1>You are OFF</h1>");
}

void handleLightOn() {
  digitalWrite(GPIO_D1, 0);
    redirect();
//  server.send(200, "text/html", "<h1>You are ON</h1>");
}

void handleLogo() {
  static const uint8_t gif[] PROGMEM = {
    0x47, 0x49, 0x46, 0x38, 0x37, 0x61, 0x10, 0x00, 0x10, 0x00, 0x80, 0x01,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x2c, 0x00, 0x00, 0x00, 0x00,
    0x10, 0x00, 0x10, 0x00, 0x00, 0x02, 0x19, 0x8c, 0x8f, 0xa9, 0xcb, 0x9d,
    0x00, 0x5f, 0x74, 0xb4, 0x56, 0xb0, 0xb0, 0xd2, 0xf2, 0x35, 0x1e, 0x4c,
    0x0c, 0x24, 0x5a, 0xe6, 0x89, 0xa6, 0x4d, 0x01, 0x00, 0x3b
  };
  char gif_colored[sizeof(gif)];
  memcpy_P(gif_colored, gif, sizeof(gif));
  // Set the background to a random set of colors
  gif_colored[16] = millis() % 256;
  gif_colored[17] = millis() % 256;
  gif_colored[18] = millis() % 256;
  server.send(200, "image/gif", gif_colored, sizeof(gif_colored));
}

void handleGpio() {
  String json;
  json.reserve(88);
  json = "{\"time\":";
  json += millis();
  json += ", \"heap\":";
  json += ESP.getFreeHeap();
  json += ", \"analog\":";
  json += analogRead(A0);
  json += ", \"gpioMask\":";
  json += gpioMask;
  json += ", \"gpioData\":";
  json += (uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16));
  json += "}";
  server.send(200, "text/json", json);
}

void handleGraph() {
  String out;
  out.reserve(2600);
  char temp[70];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  int y = rand() % 130;
  for (int x = 10; x < 390; x += 10) {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    y = y2;
  }
  out += "</g>\n</svg>\n";

  server.send(200, "image/svg+xml", out);
}

/** Handle root or redirect to captive portal */
void handleCaptivePortal() {
  if (captivePortal()) { // If caprive portal redirect instead of displaying the page.
    return;
  }
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");

  String Page;
  Page += F(
            "<!DOCTYPE html><html lang='en'><head>"
            "<meta name='viewport' content='width=device-width'>"
            "<title>CaptivePortal</title></head><body>"
            "<h1>HELLO WORLD!!</h1>");
  if (server.client().localIP() == apIP) {
    Page += String(F("<p>You are connected through the soft AP: ")) + softAP_ssid + F("</p>");
  } else {
    Page += String(F("<p>You are connected through the wifi network: ")) + ssid + F("</p>");
  }
  Page += F(
            "<p>You may want to <a href='/wifi'>config the wifi connection</a>.</p>"
            "</body></html>");

  server.send(200, "text/html", Page);
}

/** Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
boolean captivePortal() {
  if (!isIp(server.hostHeader()) && server.hostHeader() != (String(host) + ".local")) {
    Serial.println("Request redirected to captive portal");
    server.sendHeader("Location", String("http://") + toStringIp(server.client().localIP()), true);
    server.send(302, "text/plain", "");   // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server.client().stop(); // Stop is needed because we sent no content length
    return true;
  }
  return false;
}

void handleNotFound() {
  String message = "Resource Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

////////////////////////////////
//This function redirects home
void redirect(){
  String url = "https://" + WiFi.localIP().toString();
  Serial.println("Redirect called. Redirecting to " + url);
  server.sendHeader("Location", url, true);
  Serial.println("Header sent.");
  server.send( 302, "text/plain", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  Serial.println("Empty page sent.");
  server.client().stop(); // Stop is needed because we sent no content length
  Serial.println("Client stopped.");
}

// Return default GPIO mask, that is all I/Os except SD card ones
unsigned int defaultMask() {
  unsigned int mask = 0b11111111111111111;
  for (auto pin = 0; pin <= 16; pin++) {
    if (isFlashInterfacePin(pin)) {
      mask &= ~(1 << pin);
    }
  }
  return mask;
}////////////////////////////////

void setup() {
  Serial.begin(74880);
  Serial.setDebugOutput(true);
  delay(10);

  // Подготовка GPIO
  pinMode(GPIO_D1, OUTPUT);
  digitalWrite(GPIO_D1, 1);

  Serial.println();
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

  server.getServer().setRSACert(new BearSSL::X509List(serverCert), new BearSSL::PrivateKey(serverKey));

  server.on("/", HTTP_GET, handleRoot);
  server.on("/on", HTTP_GET, handleLightOn);
  server.on("/off", HTTP_GET, handleLightOff);
  server.on("/logo", HTTP_GET, handleLogo);
  server.on("/gpio", HTTP_GET, handleGpio);
  server.on("/graph", HTTP_GET, handleGraph);s
  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
    MDNS.update();
    gpioMask = defaultMask();
}
