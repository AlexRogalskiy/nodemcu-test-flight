#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <RH_NRF24.h>
#include <SPI.h>

const char* ssid = "Pantera";
const char* password = "YEzpe93r";

int gatewayID = EEPROM.read(0);

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
}

void loop()
{
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
