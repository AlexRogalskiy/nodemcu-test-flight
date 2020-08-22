/** Load WLAN credentials from EEPROM */
void loadCredentials() {
  EEPROM.begin(512);
  EEPROM.get(0, client_ssid);
  EEPROM.get(0 + sizeof(client_ssid), client_password);
  char ok[2 + 1];
  EEPROM.get(0 + sizeof(client_ssid) + sizeof(client_password), ok);
  EEPROM.end();
  if (String(ok) != String("OK")) {
    client_ssid[0] = 0;
    client_password[0] = 0;
  }
  Serial.println("Recovered credentials:");
  Serial.println(client_ssid);
  Serial.println(strlen(client_password) > 0 ? "********" : "<no password>");
}

/** Store WLAN credentials to EEPROM */
void saveCredentials() {
  EEPROM.begin(512);
  EEPROM.put(0, client_ssid);
  EEPROM.put(0 + sizeof(client_ssid), client_password);
  char ok[2 + 1] = "OK";
  EEPROM.put(0 + sizeof(client_ssid) + sizeof(client_password), ok);
  EEPROM.commit();
  EEPROM.end();
}
