void setup()
{
    //Set Pins 0 and 1 as outputs.
    //Some Digisparks have a built-in LED on pin 0, while some have it on
    //pin 1. This way, we can all Digisparks.
    pinMode(0, OUTPUT);
    pinMode(1, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
}
void loop()
{
    //Set the LED pins to HIGH. This gives power to the LED and turns it on
    digitalWrite(0, HIGH);
    digitalWrite(1, HIGH);
    digitalWrite(2, HIGH);
    digitalWrite(3, HIGH);
    //Wait for a second
    delay(1000);
    //Set the LED pins to LOW. This turns it off
    digitalWrite(0, LOW);
    digitalWrite(1, LOW);
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    //Wait for a second
    delay(1000);
}
