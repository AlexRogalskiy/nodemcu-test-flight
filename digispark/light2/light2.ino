*  15.08.2019
  Digispark ATtiny85, buzzer, LED`s, TEMT6000 light sensors

  Датчики освещенности 1 - 3 подключаются к портам 2,4,3 соответственно
  Пищалка подключается к порту 0
  Светодиод подключается к порту 1
*/

//Если с датчика освещенности поступит значение ниже установленного порога, то плата будет считать датчик затемненным
#define sensor1_ON_value 25 //Откорректировать значение для установки порога "отсутствия света" для 1 датчика
#define sensor2_ON_value 25 //Откорректировать значение для установки порога "отсутствия света" для 2 датчика
#define sensor3_ON_value 25 //Откорректировать значение для установки порога "отсутствия света" для 3 датчика

//Если с датчика освещенности поступит значение выше установленного порога, то плата будет считать датчик вышедшим из тени
#define sensor1_OFF_value 30 //Откорректировать значение для установки порога "отсутствия света" для 1 датчика
#define sensor2_OFF_value 30 //Откорректировать значение для установки порога "отсутствия света" для 2 датчика
#define sensor3_OFF_value 30 //Откорректировать значение для установки порога "отсутствия света" для 3 датчика

#include <avr/power.h>

int buzzerpin = PB0;
int ledpin = PB1;
boolean curstate = false;


void setup()
{
//  pinMode(buzzerpin, OUTPUT );
//  digitalWrite(buzzerpin, LOW );
  pinMode(ledpin, OUTPUT );

  //Уменьшим тактовую частоту Ардуино в 16 раз при помощи делителя
  clock_prescale_set(clock_div_16);
}

void loop()
{
  if (analogRead(A1) < sensor1_ON_value)// && analogRead(A3) < sensor3value)
  {
  if (analogRead(A2) < sensor2_ON_value)
  {
  if (analogRead(A3) < sensor3_ON_value)
  curstate = 1;
  }
  }
  if (analogRead(A1) > sensor1_OFF_value || analogRead(A2) > sensor2_OFF_value || analogRead(A3) > sensor3_OFF_value)
  curstate = 0;

  action(curstate);

  delay(2.5);
}

void action(boolean state)
{
  if (state == 1)
  {
  digitalWrite(buzzerpin, HIGH );
  digitalWrite(ledpin, HIGH );
  }
  else if (state == 0)
  {
  digitalWrite(buzzerpin, LOW );
  digitalWrite(ledpin, LOW );
  }
}
