#include <ESP8266WiFi.h>

// Имя и пароль вашей сети WiFi
const char* ssid = "Konio5";
const char* password = "trustico";

// Создаем сервер и порт для прослушки 80

WiFiServer server(80);

void setup() {
  Serial.begin(74880);
  delay(10);

  // Подготовка GPIO
  pinMode(5, OUTPUT);
  digitalWrite(5, 1);
  pinMode(4, OUTPUT);
  digitalWrite(4, 1);
  pinMode(0, OUTPUT);
  digitalWrite(0, 1);
  pinMode(2, OUTPUT);
  digitalWrite(2, 1);

  // Подключаемся к сети WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // Ожидание подключения
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Запуск сервера
  server.begin();
  Serial.println("Server started");

  // Вывод полученного IP адреса
  Serial.println(WiFi.localIP());
}

void loop() {

  // Проверка подключения
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Ожидание данных
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  // Чтение первой строки запроса
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Работа с GPIO
  if (req.indexOf("/1/0") != -1)
    digitalWrite(5, 0);
  else if (req.indexOf("/1/1") != -1)
    digitalWrite(5, 1);
  else if (req.indexOf("/2/0") != -1)
    digitalWrite(4, 0);
  else if (req.indexOf("/2/1") != -1)
    digitalWrite(4, 1);
  else if (req.indexOf("/3/0") != -1)
    digitalWrite(0, 0);
  else if (req.indexOf("/3/1") != -1)
    digitalWrite(0, 1);
  else if (req.indexOf("/4/0") != -1)
    digitalWrite(2, 0);
  else if (req.indexOf("/4/1") != -1)
    digitalWrite(2, 1);
  else if (req.indexOf("/5") != -1) {
    Serial.println("TEST OK");
    String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nTest OK. Uptime: ";

    // Подстчет UpTime
    int Sec = (millis() / 1000UL) % 60;
    int Min = ((millis() / 1000UL) / 60UL) % 60;
    int Hours = ((millis() / 1000UL) / 3600UL) % 24;
    int Day = ((millis() / 1000UL) / 3600UL / 24UL);
    s += Day;
    s += "d ";
    s += Hours;
    s += ":";
    s += Min;
    s += ":";
    s += Sec;
    s += "</html>\n";
    client.print(s);
    client.stop();
    return;
  }
  else
    // Если неверный запрос написать об ошибке
  {
    Serial.println("invalid request");
    String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nInvalid request";
    s += "</html>\n";
    client.print(s);
    client.stop();
    return;
  }

  client.flush();



  // Формирование ответа
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO set OK";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");
}
