#include <SPI.h>

#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(2, 4);
const byte rxAddr[6] = "00001";

void setup()
{
  //while (!Serial);
  Serial.begin(9600);
  
  radio.begin();
  radio.openReadingPipe(0, rxAddr);
  
  radio.startListening();
}

void loop()
{
  if (radio.available())
  {
    char text[32] = {0};
    radio.read(&text, sizeof(text));
    Serial.print("Received : ");
    Serial.println(text);
  } else {
    Serial.println("No Singal");
    delay(500);
  }
}
