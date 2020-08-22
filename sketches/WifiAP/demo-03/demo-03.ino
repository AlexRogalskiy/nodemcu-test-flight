#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServerSecure.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>

#ifndef APSSID
#define APSSID "Konio"
#define APPSK  "trust123s"
#endif

#define GPIO_D1  5

// Indicate which digital I/Os should be displayed on the chart.
// From GPIO16 to GPIO0, a '1' means the corresponding GPIO will be shown
// e.g. 0b11111000000111111
unsigned int gpioMask;

/* Set these to your desired credentials. */
const char* ssid = APSSID;
const char* password = APPSK;
const char* host = "automata";
const char* softAP_ssid = "Pantera";

/* Don't set this wifi credentials. They are configurated at runtime and stored on EEPROM */
char client_ssid[33] = "";
char client_password[65] = "";

/* Soft AP network parameters */
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

const char* www_username = "admin";
const char* www_password = "esp8266";
// allows you to set the realm of authentication Default:"Login Required"
const char* www_realm = "Custom Auth Realm";
// the Content of the HTML response in case of Unautherized Access Default:empty
String authFailResponse = "Authentication Failed";

static const char TEXT_PLAIN[] PROGMEM = "text/plain";
static const char TEXT_JSON[] PROGMEM = "text/json";
static const char TEXT_HTML[] PROGMEM = "text/html";

/** Should I connect to WLAN asap? */
boolean connect = false;

BearSSL::ESP8266WebServerSecure server(443);

static const char serverCert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDSzCCAjMCCQD2ahcfZAwXxDANBgkqhkiG9w0BAQsFADCBiTELMAkGA1UEBhMC
VVMxEzARBgNVBAgMCkNhbGlmb3JuaWExFjAUBgNVBAcMDU9yYW5nZSBDb3VudHkx
EDAOBgNVBAoMB1ByaXZhZG8xGjAYBgNVBAMMEXNlcnZlci56bGFiZWwuY29tMR8w
HQYJKoZIhvcNAQkBFhBlYXJsZUB6bGFiZWwuY29tMB4XDTE4MDMwNjA1NDg0NFoX
DTE5MDMwNjA1NDg0NFowRTELMAkGA1UEBhMCQVUxEzARBgNVBAgMClNvbWUtU3Rh
dGUxITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDCCASIwDQYJKoZI
hvcNAQEBBQADggEPADCCAQoCggEBAPVKBwbZ+KDSl40YCDkP6y8Sv4iNGvEOZg8Y
X7sGvf/xZH7UiCBWPFIRpNmDSaZ3yjsmFqm6sLiYSGSdrBCFqdt9NTp2r7hga6Sj
oASSZY4B9pf+GblDy5m10KDx90BFKXdPMCLT+o76Nx9PpCvw13A848wHNG3bpBgI
t+w/vJCX3bkRn8yEYAU6GdMbYe7v446hX3kY5UmgeJFr9xz1kq6AzYrMt/UHhNzO
S+QckJaY0OGWvmTNspY3xCbbFtIDkCdBS8CZAw+itnofvnWWKQEXlt6otPh5njwy
+O1t/Q+Z7OMDYQaH02IQx3188/kW3FzOY32knER1uzjmRO+jhA8CAwEAATANBgkq
hkiG9w0BAQsFAAOCAQEAnDrROGRETB0woIcI1+acY1yRq4yAcH2/hdq2MoM+DCyM
E8CJaOznGR9ND0ImWpTZqomHOUkOBpvu7u315blQZcLbL1LfHJGRTCHVhvVrcyEb
fWTnRtAQdlirUm/obwXIitoz64VSbIVzcqqfg9C6ZREB9JbEX98/9Wp2gVY+31oC
JfUvYadSYxh3nblvA4OL+iEZiW8NE3hbW6WPXxvS7Euge0uWMPc4uEcnsE0ZVG3m
+TGimzSdeWDvGBRWZHXczC2zD4aoE5vrl+GD2i++c6yjL/otHfYyUpzUfbI2hMAA
5tAF1D5vAAwA8nfPysumlLsIjohJZo4lgnhB++AlOg==
-----END CERTIFICATE-----
)EOF";

static const char serverKey[] PROGMEM =  R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEA9UoHBtn4oNKXjRgIOQ/rLxK/iI0a8Q5mDxhfuwa9//FkftSI
IFY8UhGk2YNJpnfKOyYWqbqwuJhIZJ2sEIWp2301OnavuGBrpKOgBJJljgH2l/4Z
uUPLmbXQoPH3QEUpd08wItP6jvo3H0+kK/DXcDzjzAc0bdukGAi37D+8kJfduRGf
zIRgBToZ0xth7u/jjqFfeRjlSaB4kWv3HPWSroDNisy39QeE3M5L5ByQlpjQ4Za+
ZM2yljfEJtsW0gOQJ0FLwJkDD6K2eh++dZYpAReW3qi0+HmePDL47W39D5ns4wNh
BofTYhDHfXzz+RbcXM5jfaScRHW7OOZE76OEDwIDAQABAoIBAQDKov5NFbNFQNR8
djcM1O7Is6dRaqiwLeH4ZH1pZ3d9QnFwKanPdQ5eCj9yhfhJMrr5xEyCqT0nMn7T
yEIGYDXjontfsf8WxWkH2TjvrfWBrHOIOx4LJEvFzyLsYxiMmtZXvy6YByD+Dw2M
q2GH/24rRdI2klkozIOyazluTXU8yOsSGxHr/aOa9/sZISgLmaGOOuKI/3Zqjdhr
eHeSqoQFt3xXa8jw01YubQUDw/4cv9rk2ytTdAoQUimiKtgtjsggpP1LTq4xcuqN
d4jWhTcnorWpbD2cVLxrEbnSR3VuBCJEZv5axg5ZPxLEnlcId8vMtvTRb5nzzszn
geYUWDPhAoGBAPyKVNqqwQl44oIeiuRM2FYenMt4voVaz3ExJX2JysrG0jtCPv+Y
84R6Cv3nfITz3EZDWp5sW3OwoGr77lF7Tv9tD6BptEmgBeuca3SHIdhG2MR+tLyx
/tkIAarxQcTGsZaSqra3gXOJCMz9h2P5dxpdU+0yeMmOEnAqgQ8qtNBfAoGBAPim
RAtnrd0WSlCgqVGYFCvDh1kD5QTNbZc+1PcBHbVV45EmJ2fLXnlDeplIZJdYxmzu
DMOxZBYgfeLY9exje00eZJNSj/csjJQqiRftrbvYY7m5njX1kM5K8x4HlynQTDkg
rtKO0YZJxxmjRTbFGMegh1SLlFLRIMtehNhOgipRAoGBAPnEEpJGCS9GGLfaX0HW
YqwiEK8Il12q57mqgsq7ag7NPwWOymHesxHV5mMh/Dw+NyBi4xAGWRh9mtrUmeqK
iyICik773Gxo0RIqnPgd4jJWN3N3YWeynzulOIkJnSNx5BforOCTc3uCD2s2YB5X
jx1LKoNQxLeLRN8cmpIWicf/AoGBANjRSsZTKwV9WWIDJoHyxav/vPb+8WYFp8lZ
zaRxQbGM6nn4NiZI7OF62N3uhWB/1c7IqTK/bVHqFTuJCrCNcsgld3gLZ2QWYaMV
kCPgaj1BjHw4AmB0+EcajfKilcqtSroJ6MfMJ6IclVOizkjbByeTsE4lxDmPCDSt
/9MKanBxAoGAY9xo741Pn9WUxDyRplww606ccdNf/ksHWNc/Y2B5SPwxxSnIq8nO
j01SmsCUYVFAgZVOTiiycakjYLzxlc6p8BxSVqy6LlJqn95N8OXoQ+bkwUux/ekg
gz5JWYhbD6c38khSzJb0pNXCo3EuYAVa36kDM96k1BtWuhRS10Q1VXk=
-----END RSA PRIVATE KEY-----
)EOF";

////////////////////////////////
// Handlers to return HTTP codes

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

void handleAdmin() {
  if (!server.authenticate(www_username, www_password))
      //Basic Auth Method with Custom realm and Failure Response
      //return server.requestAuthentication(BASIC_AUTH, www_realm, authFailResponse);
      //Digest Auth Method with realm="Login Required" and empty Failure Response
      //return server.requestAuthentication(DIGEST_AUTH);
      //Digest Auth Method with Custom realm and empty Failure Response
      //return server.requestAuthentication(DIGEST_AUTH, www_realm);
      //Digest Auth Method with Custom realm and Failure Response
    {
      return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
    }
    server.send(200, "text/plain", "Login OK");
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
//
///** Handle root or redirect to captive portal */
//void handleCaptivePortal() {
//  if (captivePortal()) { // If caprive portal redirect instead of displaying the page.
//    return;
//  }
//  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
//  server.sendHeader("Pragma", "no-cache");
//  server.sendHeader("Expires", "-1");
//
//  String Page;
//  Page += F(
//            "<!DOCTYPE html><html lang='en'><head>"
//            "<meta name='viewport' content='width=device-width'>"
//            "<title>CaptivePortal</title></head><body>"
//            "<h1>HELLO WORLD!!</h1>");
//  if (server.client().localIP() == apIP) {
//    Page += String(F("<p>You are connected through the soft AP: ")) + softAP_ssid + F("</p>");
//  } else {
//    Page += String(F("<p>You are connected through the wifi network: ")) + ssid + F("</p>");
//  }
//  Page += F(
//            "<p>You may want to <a href='/wifi'>config the wifi connection</a>.</p>"
//            "</body></html>");
//
//  server.send(200, "text/html", Page);
//}
//
///** Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
//boolean captivePortal() {
//  if (!isIp(server.hostHeader()) && server.hostHeader() != (String(host) + ".local")) {
//    Serial.println("Request redirected to captive portal");
//    server.sendHeader("Location", String("http://") + toStringIp(server.client().localIP()), true);
//    server.send(302, "text/plain", "");   // Empty content inhibits Content-length header so we have to close the socket ourselves.
//    server.client().stop(); // Stop is needed because we sent no content length
//    return true;
//  }
//  return false;
//}
//
///** Wifi config page handler */
//void handleWifi() {
//  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
//  server.sendHeader("Pragma", "no-cache");
//  server.sendHeader("Expires", "-1");
//
//  String Page;
//  Page += F(
//            "<!DOCTYPE html><html lang='en'><head>"
//            "<meta name='viewport' content='width=device-width'>"
//            "<title>CaptivePortal</title></head><body>"
//            "<h1>Wifi config</h1>");
//  if (server.client().localIP() == apIP) {
//    Page += String(F("<p>You are connected through the soft AP: ")) + softAP_ssid + F("</p>");
//  } else {
//    Page += String(F("<p>You are connected through the wifi network: ")) + ssid + F("</p>");
//  }
//  Page +=
//    String(F(
//             "\r\n<br />"
//             "<table><tr><th align='left'>SoftAP config</th></tr>"
//             "<tr><td>SSID ")) +
//    String(softAP_ssid) +
//    F("</td></tr>"
//      "<tr><td>IP ") +
//    toStringIp(WiFi.softAPIP()) +
//    F("</td></tr>"
//      "</table>"
//      "\r\n<br />"
//      "<table><tr><th align='left'>WLAN config</th></tr>"
//      "<tr><td>SSID ") +
//    String(ssid) +
//    F("</td></tr>"
//      "<tr><td>IP ") +
//    toStringIp(WiFi.localIP()) +
//    F("</td></tr>"
//      "</table>"
//      "\r\n<br />"
//      "<table><tr><th align='left'>WLAN list (refresh if any missing)</th></tr>");
//  Serial.println("scan start");
//  int n = WiFi.scanNetworks();
//  Serial.println("scan done");
//  if (n > 0) {
//    for (int i = 0; i < n; i++) {
//      Page += String(F("\r\n<tr><td>SSID ")) + WiFi.SSID(i) + ((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? F(" ") : F(" *")) + F(" (") + WiFi.RSSI(i) + F(")</td></tr>");
//    }
//  } else {
//    Page += F("<tr><td>No WLAN found</td></tr>");
//  }
//  Page += F(
//            "</table>"
//            "\r\n<br /><form method='POST' action='wifisave'><h4>Connect to network:</h4>"
//            "<input type='text' placeholder='network' name='n'/>"
//            "<br /><input type='password' placeholder='password' name='p'/>"
//            "<br /><input type='submit' value='Connect/Disconnect'/></form>"
//            "<p>You may want to <a href='/'>return to the home page</a>.</p>"
//            "</body></html>");
//  server.send(200, "text/html", Page);
//  server.client().stop(); // Stop is needed because we sent no content length
//}
//
///** Handle the WLAN save form and redirect to WLAN config page again */
//void handleWifiSave() {
//  Serial.println("wifi save");
//  server.arg("n").toCharArray(client_ssid, sizeof(client_ssid) - 1);
//  server.arg("p").toCharArray(client_password, sizeof(client_password) - 1);
//  
//  server.sendHeader("Location", "wifi", true);
//  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
//  server.sendHeader("Pragma", "no-cache");
//  server.sendHeader("Expires", "-1");
//  
//  server.send(302, "text/plain", "");    // Empty content inhibits Content-length header so we have to close the socket ourselves.
//  server.client().stop(); // Stop is needed because we sent no content length
//  saveCredentials();
//  connect = strlen(ssid) > 0; // Request WLAN connect with new credentials if there is a SSID
//}
////////////////////////////////
// Utils to return HTTP codes

void replyOK() {
  server.send(200, FPSTR(TEXT_PLAIN), "");
}

void replyOKWithMsg(String msg) {
  server.send(200, FPSTR(TEXT_PLAIN), msg);
}

void replyNotFound(String msg) {
  server.send(404, FPSTR(TEXT_PLAIN), msg);
}

void replyBadRequest(String msg) {
  Serial.println(msg);
  server.send(400, FPSTR(TEXT_PLAIN), msg + "\r\n");
}

void replyServerError(String msg) {
  Serial.println(msg);
  server.send(500, FPSTR(TEXT_PLAIN), msg + "\r\n");
}

//This function redirects home
void redirect(){
  String url = "https://" + toStringIp(WiFi.localIP());
  Serial.println("Redirect called. Redirecting to " + url);
  server.sendHeader("Location", url, true);
  Serial.println("Header sent.");
  server.send( 302, "text/plain", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  Serial.println("Empty page sent.");
  server.client().stop(); // Stop is needed because we sent no content length
  Serial.println("Client stopped.");
}
//
//void connectWifi() {
//  Serial.println("Connecting as wifi client...");
//  WiFi.disconnect();
//  WiFi.begin(ssid, password);
//  int connRes = WiFi.waitForConnectResult();
//  Serial.print("connRes: ");
//  Serial.println(connRes);
//}
////////////////////////////////

// Return default GPIO mask, that is all I/Os except SD card ones
unsigned int defaultMask() {
  unsigned int mask = 0b11111111111111111;
  for (auto pin = 0; pin <= 16; pin++) {
    if (isFlashInterfacePin(pin)) {
      mask &= ~(1 << pin);
    }
  }
  return mask;
}

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
    MDNS.addService("https", "tcp", 443);
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
  server.on("/graph", HTTP_GET, handleGraph);
  server.on("/admin", HTTP_GET, handleAdmin);

//  server.on("/wifi", handleCaptivePortal);
//  server.on("/wifi/test", handleWifi);
//  server.on("/wifi/save", handleWifiSave);
  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("HTTP server started");

  connect = strlen(ssid) > 0; // Request WLAN connect if there is a SSID
}

void loop() {
  server.handleClient();
  MDNS.update();
  gpioMask = defaultMask();
}
