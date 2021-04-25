#include <avr/power.h>

int buzzerpin = PB2;
int ledpin = PB3;
int ledpin2 = PB4;

boolean curstate = false;

void setup()
{
  pinMode(buzzerpin, OUTPUT);
  digitalWrite(buzzerpin, LOW);
  
  pinMode(ledpin, INPUT);
  pinMode(ledpin2, INPUT);

  // Уменьшим тактовую частоту Ардуино в 16 раз при помощи делителя
  clock_prescale_set(clock_div_16);
}

void loop()
{
  if (digitalRead(ledpin) == 1 && digitalRead(ledpin2) == 1)
  {
    curstate = 0;
  }

  if (digitalRead(ledpin) == 0 && digitalRead(ledpin2) == 0)
  {
    curstate = 1;
  }
  
  action(curstate);

  delay(2.5);
}

void action(boolean state)
{
  if (state == 1)
  {
    digitalWrite(buzzerpin, HIGH);
  }
  if (state == 0)
  {
    digitalWrite(buzzerpin, LOW);
  }
}
