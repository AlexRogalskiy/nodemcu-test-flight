#include <ESP8266WiFi.h>

int state = LOW;
int LED = LED_BUILTIN;
char on = LOW;
char off = HIGH;

const char* ssid = "Vishal";
const char* password = "vvishal123";

WiFiServer server(80);

void setup()
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, off);

  Serial.print("Connecting");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");  
  server.begin();  // Starts the Server
  Serial.println("Server started");

  Serial.print("IP Address of network: "); // Prints IP address on Serial Monitor
  Serial.println(WiFi.localIP());
  Serial.print("Copy and paste the following URL: https://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop()
{
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }
  Serial.println("Waiting for new client");
  while(!client.available())
  {
    delay(1);
  }

  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  
  if(request.indexOf("/LEDON") != -1)
  {
    digitalWrite(LED, on); // Turn ON LED
    state = on;
  }
  if(request.indexOf("/LEDOFF") != -1)
  {
    digitalWrite(LED, off); // Turn OFF LED
    state = off;
  }

/*------------------HTML Page Creation---------------------*/

  client.println("HTTP/1.1 200 OK"); // standalone web server with an ESP8266
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");

  client.println("<head>");
  client.println("<title>LED TOGGLE</title>");
  client.println("<style type=\"text/css\">");
  client.println("body{");
  client.println("background-color:#000000;");
  client.println("color:#ffffff;");
  client.println("font-family: Arial, Helvetica, sans-serif;");
  client.println("font-size:30px;");
  client.println("line-height:1.6em;");
  client.println("margin:0;");
  client.println("}");
  client.println(".container{");
  client.println("width:80%;");
  client.println("margin:auto;");
  client.println("overflow:hidden;");
  client.println("text-align:center;");
  client.println("}");
  client.println("#main-header {");
  client.println("background-color:coral;");
  client.println("color:#fff;");
  client.println("}");
  client.println(".button1{");
  client.println("border:1px #fff solid;");
  if(state == on){
  client.println("background-color:red;");
  }
  else{
  client.println("background-color:#F75D59;");
  }
  client.println("color:#fff;");
  client.println("padding:60px 60px;");
  client.println("margin-top:10px;");
  client.println("border-radius:50%;");
  client.println("font-size:50px;");
  client.println("}");
  client.println(".button2{");
  client.println("border:1px #fff solid;");
  if(state == off){
  client.println("background-color:red;");
  }
  else{
  client.println("background-color:#F75D59;");
  }
  client.println("color:#fff;");
  client.println("padding:60px 50px;");
  client.println("margin-top:10px;");
  client.println("border-radius:50%;");
  client.println("font-size:50px;");
  client.println("}");
  client.println(".LED{");
  client.println("border:1px #fff solid;");
  if(state == off){
  client.println("background-color:#000;");
  }
  else{
  client.println("background-color:#FDD017;");
  }
  client.println("color:#fff;");
  client.println("padding:80px 80px;");
  client.println("margin-top:10px;");
  client.println("border-radius:50%;");
  client.println("}");
  client.println("@media(max-width:600px){");
  client.println("#main-header{");
  client.println("width:100%;");
  client.println("}");
  client.println(".container{");
  client.println("width:auto;");
  client.println("}");
  client.println(".LED{");
  client.println("width:auto;");
  client.println("}");
  client.println("</style>");
  client.println("</head>");
  client.print("<body>");
  client.println("<header id=\"main-header\">");
  client.println("<div class=\"container\">");
  client.println("<h1>LED TOGGLE</h1>");
  client.println("<h4>by Vishal Soni</h4>");
  client.println("</div>");
  client.println("</header>");
  client.println("<div class=\"container\">");
  client.println("<button class=\"LED\"></button>");
  client.println("</div>");
  client.println("<div class=\"container\">");
  client.println("<a href=\"/LEDON\"\"><button class=\"button1\">ON</button></a>");
  client.println("<a href=\"/LEDOFF\"\"><button class=\"button2\">OFF</button></a>");
  client.println("</div>");
  client.print("</body>");
  client.println("</html>");

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}
