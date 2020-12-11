#include  <SPI.h>

#include "nRF24L01.h"
#include "RF24.h"
int msg[1];
RF24 radio(4, 15);

const byte rxAddr[6] = "00001";

void setup()
{
  Serial.begin(9600);
  Serial.println("Start....");
  radio.begin();
  radio.setRetries(15, 15);
  radio.openWritingPipe(rxAddr);
  
  radio.stopListening();
}

char text[] = "Hello Sanki I'm Here !!";
int counter = 0;
void loop()
{
  if (counter <10) {
    text[0] = '0';
    text[1] = char(counter + 97);
    counter++;
  } else {
    counter = 0;
  }
  
  Serial.print("Sending.....");
  radio.write(&text, sizeof(text));
  Serial.print("OK : ");
  Serial.println(text[1]);
  delay(500);
}
