void setup() {
    //All pins are capable of Digital output, though P5 is 3 V at HIGH instead of 5 V
    pinMode(0, OUTPUT); //0 is P0, 1 is P1, 2 is P2, etc. - unlike the analog inputs, for digital outputs the pin number matches.
}

void loop() {
    digitalWrite(0,HIGH); //Turn the pin HIGH (5 V)
    delay(1000);
    digitalWrite(0,LOW); //Turn the pin LOW (GND)
    delay(1000);
}
