/*
  Example for different sending methods
   https://github.com/sui77/rc-switch/
*/
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(74880);
  Serial.setDebugOutput(true);
  delay(10);

  // Transmitter is connected to Arduino Pin #0
  mySwitch.enableTransmit(0);  // Optional set pulse length.
  // mySwitch.setPulseLength(320);

  // Optional set protocol (default is 1, will work for most outlets)
  // mySwitch.setProtocol(2);

  // Optional set number of transmission repetitions.
  // mySwitch.setRepeatTransmit(15);
}

void loop() {
  /* Same switch as above, but using decimal code */
  //if (mySwitch.available()) {
    mySwitch.send(1394007, 24);
    Serial.println("Sending data to remote device: 1394007");
    delay(2000);
    mySwitch.send(1394006, 24);
    Serial.println("Sending data to remote device: 1394006");
    delay(2000);
  //}
}
