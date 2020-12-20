#include <Wire.h>
#include <Adafruit_PN532.h>

#define PN532_IRQ (2)
#define PN532_RESET (3)
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

void setup(void) {
  Serial.begin(115200);
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1);
  }
  nfc.SAMConfig();
}

void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success) {
    Serial.println("Found an ISO14443A card");
    Serial.print(" UID Length: "); Serial.print(uidLength, DEC); Serial.println(" bytes");
    Serial.print(" UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");
    if (uidLength == 7) {
      uint8_t data[32];
      Serial.println("Seems to be an NTAG2xx tag (7 byte UID)");
      for (uint8_t i = 0; i < 42; i++) {
        success = nfc.ntag2xx_ReadPage(i, data);
        Serial.print("PAGE ");
        if (i < 10) {
          Serial.print("0");
          Serial.print(i);
        }
        else {
          Serial.print(i);
        }
        Serial.print(": ");
        if (success) {
          nfc.PrintHexChar(data, 4);
        }
        else {
          Serial.println("Unable to read the requested page!");
        }
      }
    }
    else
    {
      Serial.println("This doesn't seem to be an NTAG203 tag (UUID length != 7 bytes)!");
    }
    Serial.println("\n\nSend a character to scan another tag!");
    Serial.flush();
    while (!Serial.available());
    while (Serial.available()) {
      Serial.read();
    }
    Serial.flush();
  }
}
