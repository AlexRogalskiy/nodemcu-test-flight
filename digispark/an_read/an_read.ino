int sensorValue = 0;

void setup() {
    //You need not set pin mode for analogRead - though if you have set the pin to
    //output and later want to read from it then you need to set pinMode(0,INPUT);
    //where 0 is the physical pin number not the analog input number.
    //
    //See below for the proper pinMode statement to go with each analog read.
}

void loop() {
    // The analog pins are referenced by their analog port number, not their pin
    //number and are as follows:

    sensorValue = analogRead(1); //Read P2
    //To set to input: pinMode(2, INPUT);
    //THIS IS P2, P2 is analog input 1, so when you are using analog read, you refer to it as 1.

    //sensorValue = analogRead(2); //Read P4
    //To set to input: pinMode(4, INPUT);
    //THIS IS P4, P4 is analog input 2, so when you are using analog read, you refer to it as 2.

    //sensorValue = analogRead(3); //Read P3
    //To set to input: pinMode(3, INPUT);
    //THIS IS P3, P3 is analog input 3, so when you are using analog read, you refer to it as 3.

    //sensorValue = analogRead(0); //Read P5
    //To set to input: pinMode(5, INPUT);
    //THIS IS P5, P5 is analog input 0, so when you are using analog read, you refer to it as 0.
}
