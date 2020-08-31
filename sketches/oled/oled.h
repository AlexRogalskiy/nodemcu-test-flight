
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(128,48,&Wire,OLED_RESET);
char oled[6][11];

int oled_init() {
  if (! display.begin(SSD1306_SWITCHCAPVCC, 0x3C) ) {
    Serial.println("Oled initialization failed");
    return 0;
  }
//  display.display();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  for (int i=0; i<6; i++)
    for (int j=0; j<10; j++)
      oled[i][j]='.';
  display.display();
  return 1;
}

void oled_display(int r, const char *msg) {
  int i=strlen(msg);
  display.clearDisplay();
  if (i>9) i=9;
  while (i>=0) {
    *(oled[r]+i)=*(msg+i);
    i--;
  }
  for (i=0; i<6; i++) {
    display.setCursor(32,i*8);
    display.print(oled[i]);
  }
  display.display();
}
