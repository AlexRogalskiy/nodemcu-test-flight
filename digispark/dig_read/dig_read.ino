int sensorValue = 0;

void setup() {
    //All pins are capable of digital input.
    pinMode(0, INPUT); //0 is P0, 1 is P1, 2 is P2, etc. - unlike the analog inputs, for digital inputs the pin number matches.
}

void loop() {
    sensorValue = digitalRead(1); //Returns HIGH or LOW (true or false / 1 or 0).
}
