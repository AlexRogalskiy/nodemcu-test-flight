//Wifi ultrasonic reading using nodemcu
//made by Dhananjay 
//cantact adhanu99.blogspot.com

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
 
// Replace with your network credentials
const char* ssid = "DHANANJAYA";
const char* password = "7887631681";
const int trigPin = D0;
const int echoPin = D1;
// defines variables
long duration;
int distance;
 
ESP8266WebServer server(80);   //instantiate server at port 80 (http port)
 
String page = "";
double data; 
void setup(void){
 
 pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT);
  
  delay(1000);
  Serial.begin(115200);
  WiFi.begin(ssid, password); //begin WiFi connection
  Serial.println("");
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/", [](){
    page = "<h4>adhanu99.blogspot.com</h4><h1>Sensor to Node MCU Web Server</h1><h3>ultrasonic distance:</h3> <h1>"+String(data)+"</14><head><META HTTP-EQUIV=\"refresh\" CONTENT=\"1\"></head>"; 
    server.send(200, "text/html", page);
    
  });
  
  server.begin();
  Serial.println("Web server started!");
}
 
void loop(void){
  data = distance;

// Clears the trigPin
digitalWrite(trigPin, LOW);
delayMicroseconds(2);
// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);
// Calculating the distance
distance= duration*0.034/2;
// Prints the distance on the Serial Monitor
Serial.print("Distance: ");
Serial.println(distance);
delay(100);
  server.handleClient();
}