//#include <EEPROM.h>
#include <SPI.h>
//#include <RH_NRF24.h>
#include "nRF24L01.h"
#include "RF24.h"

char msg[6] = "hello";
//RF24 radio(2, 4);
//RF24 radio(12, 14, 26, 25, 27);
//RF24 radio(9, 10);  // CE_PIN, CSN_PIN
RF24 radio(2, 4);
const uint64_t pipe = 0xAABBCCDD11LL;

int32_t data[5];    // на данный момент используется int32_t чтобы иметь возможность получить большие числа счетчика передач, для тестирования продолжительности работы от батарейки,
// потом можно обычный int
// data 0 - данные АЦП или пересчитанное напряжение питания c тремя знаками(в милливольтах, на стороне приемника переменная float деленная на 1000
// data 1 - счетчик отправленных пакетов, сбрасывается при снятии питания
// data 2 - P атмсомферное в мм, можно умножить на 10 если нужны десятые, тогда на стороне приемника делить на 10 и float
// data 3 - Т температура умноженное на 100 на стороне приемника float делить на 100
// data 4 - H влажность умноженная на 100

//RH_NRF24 nrf24(2, 4); // use this for NodeMCU Amica/AdaFruit Huzzah ESP8266 Feather
//RH_NRF24 nrf24(8, 7); // use this with Arduino UNO/Nano

//int deviceID = EEPROM.read(0);

void setup()
{
  Serial.begin(74880);
  Serial.setDebugOutput(true);
  delay(10);

  //  if (!nrf24.init())
  //  {
  //    Serial.println("init failed");
  //  }
  //  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  //  if (!nrf24.setChannel(3))
  //  {
  //    Serial.println("setChannel failed");
  //  }
  //  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm)) {
  //    Serial.println("setRF failed");
  //  }
  //  Serial.println("Transmitter started");

  radio.begin();
  radio.setAutoAck(true);                    // подтверждение приема, true - включено, false - отключено
  radio.setRetries(2, 1);                 // количество попыток передачи если включен режим автоподтверждения приема, первая цифра - задержка между передачами, 1 = 250 мкс, вторая - количество попыток передачи
  radio.setChannel(100);                     // номер канала, можно перед выбором просканировать эфир чтобы выяснить менее зашумленные каналы(использовать скетч scanner из библ. RF24)
  radio.setPayloadSize(7);
  radio.setDataRate(RF24_2MBPS);        // скорость обмена RF24_2MBPS, RF24_1MBPS, RF24_250KBPS - максимальная чувствительность на 250 кбитс
  radio.setPALevel(RF24_PA_MAX);       // уровень мощности RF24_PA_MIN -18dBm, RF24_PA_LOW -12dBm, RF24_PA_HIGH -6dBM, RF24_PA_MAX 0dBm
  //radio.openWritingPipe(0xAABBCCDD11LL);    // уникальный номер передатчика, на приемнике должен быть такой же
  radio.openWritingPipe(pipe);
  //radio.powerDown();
}

void loop()
{
  Serial.println("Sending to gateway");
  //  uint8_t data[4];

  //  data[0] = deviceID;

  //  Serial.println("------------- Measurements -------------");
  //  Serial.print(", ID: ");
  //  Serial.print(data[0]);
  //  Serial.println();

  //nrf24.send(data, sizeof(data));
  //nrf24.waitPacketSent();

  // Now wait for a reply
  //uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
  //uint8_t len = sizeof(buf);

  //  if (nrf24.waitAvailableTimeout(1000))
  //  {
  //    // Should be a reply message for us now
  //    if (nrf24.recv(buf, &len))
  //    {
  //      Serial.print("got reply: ");
  //      Serial.println((char*)buf);
  //    }
  //    else
  //    {
  //      Serial.println("recv failed");
  //    }
  //  }
  //  else
  //  {
  //    Serial.println("No reply.");
  //  }

  sendMessage();
  delay(5000);
}

void sendMessage() {
  int32_t data[5];
  data[0] = 100;
  data[1] = 1;
  data[2] = 2;
  data[3] = 3;
  data[3] = 4;

  //radio.powerUp();
  //radio.write(&data, sizeof(data));
  Serial.println(msg);
  radio.write(msg, 6);
  //radio.powerDown();
}
