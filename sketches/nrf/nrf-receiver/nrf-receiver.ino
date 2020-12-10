//#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
//#include <RH_NRF24.h>
#include "nRF24L01.h"
#include "RF24.h"

char msg[6];
//RF24 radio(7, 8);
//RF24 radio(2, 15);
RF24 radio(99, 4);
//RF24 radio(2, 15);
//RF24 radio(99, 15); // CE, CSN, 99 - это заглушка, чтобы освободить один пин, если передача не предполлагается то можно CE просто подтянуть резистором к питанию, иначе нужно указать вывод
//RH_NRF24 nrf24(2, 4); // use this for NodeMCU Amica/AdaFruit Huzzah ESP8266 Feather
const uint64_t pipe = 0xAABBCCDD11LL;

const char* ssid = "Pantera";
const char* password = "YEzpe93r";

int32_t data[5];

//int gatewayID = EEPROM.read(0);

void setup()
{
  Serial.begin(74880);
  Serial.print("Receiver Started, ID: ");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  //Serial.print(gatewayID);
  Serial.println();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");

  //  nrf24.init();
  //  nrf24.setChannel(3);
  //  nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm);

  radio.begin();
  radio.setAutoAck(true);
  radio.setChannel(100);
  radio.setPayloadSize(7);
  radio.setDataRate(RF24_2MBPS);   // скорость обмена RF24_2MBPS, RF24_1MBPS, RF24_250KBPS - максимальная чувствительность на 250 кбитс, NRF24L01 без плюса 250 не умеет
  radio.setPALevel(RF24_PA_MAX);    // уровень мощности RF24_PA_MIN -18dBm, RF24_PA_LOW -12dBm, RF24_PA_HIGH -6dBM, RF24_PA_MAX 0dBm
  //radio.openReadingPipe(1, 0xAABBCCDD11LL);
  radio.openReadingPipe(1, pipe);
  radio.startListening();
}

void loop()
{
  Serial.println("Receiving data <<< ");
  if (radio.available()) {
    //radio.read(&data, sizeof(data));
    radio.read(msg, 6);
    Serial.println("Device ID: ");
    //Serial.print(data[0]);
    //Serial.print(data[1]);
    Serial.println(msg);
  }
  else
  {
    Serial.println("No New Message");
  }

  //  if (nrf24.available())
  //  {
  //    // Should be a message for us now
  //    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
  //    uint8_t len = sizeof(buf);
  //    if (nrf24.recv(buf, &len))
  //    {
  //
  //      // Send a reply
  //      uint8_t sdata[] = "Data Received.";
  //      nrf24.send(sdata, sizeof(sdata));
  //      nrf24.waitPacketSent();
  //
  //      int deviceID = buf[0];
  //
  //      Serial.println("--- Data retrieved from device ---");
  //      Serial.print("Device ID: ");
  //      Serial.print(deviceID);
  //    }
  //  }
  //  else
  //  {
  //    Serial.println("No New Message");
  //  }
  delay(2000);
}
