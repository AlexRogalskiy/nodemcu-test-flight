#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <RH_NRF24.h>
#include <SPI.h>
//#include "nRF24L01.h"
#include "RF24.h"

const char* ssid = "Pantera";
const char* password = "YEzpe93r";

int32_t data[5];

int gatewayID = EEPROM.read(0);

RF24 radio(99, 15);
RH_NRF24 nrf24(2, 4); // use this for NodeMCU Amica/AdaFruit Huzzah ESP8266 Feather

void setup()
{
  Serial.begin(74880);
  Serial.print("Receiver Started, ID: ");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  Serial.print(gatewayID);
  Serial.println();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");

  nrf24.init();
  nrf24.setChannel(3);
  nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm);

  radio.begin();
  radio.setAutoAck(true);
  radio.setChannel(100);
  radio.setDataRate(RF24_2MBPS);   // скорость обмена RF24_2MBPS, RF24_1MBPS, RF24_250KBPS - максимальная чувствительность на 250 кбитс, NRF24L01 без плюса 250 не умеет
  radio.setPALevel(RF24_PA_MAX);    // уровень мощности RF24_PA_MIN -18dBm, RF24_PA_LOW -12dBm, RF24_PA_HIGH -6dBM, RF24_PA_MAX 0dBm
  radio.openReadingPipe(1, 0xAABBCCDD11LL);
  radio.startListening();
}

void loop()
{
  if (radio.available()) {
    radio.read( &data, sizeof(data));
    Serial.println("Device ID: ");
    Serial.print(data[0]);
  }

  if (nrf24.available())
  {
    // Should be a message for us now
    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (nrf24.recv(buf, &len))
    {

      // Send a reply
      uint8_t sdata[] = "Data Received.";
      nrf24.send(sdata, sizeof(sdata));
      nrf24.waitPacketSent();

      int deviceID = buf[0];

      Serial.println("--- Data retrieved from device ---");
      Serial.print("Device ID: ");
      Serial.print(deviceID);
    }
  }
  else
  {
    Serial.println("No New Message");
  }
  delay(2000);
}
