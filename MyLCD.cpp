#include "MyLCD.h"

void MyLCD::init() {
  tft.reset();
  tft.begin(tft.readID());
  tft.setRotation(Orientation);
  drawRadioUI();
}

void MyLCD::drawRadioUI() {
  tft.fillScreen(BLACK);

  tft.fillRect(0, 0, 320, 240, BLACK); // x, y, w, h, r, color
  // radio info
  tft.fillRect(4, 4, 188, 108, GREEN); // x, y, w, h, r, color
  tft.fillRect(6, 6, 184, 104, BLACK); // x, y, w, h, r, color
  // temperature
  tft.fillRect(200, 4, 116, 46, GREEN); // x, y, w, h, r, color
  tft.fillRect(202, 6, 112, 42, BLACK); // x, y, w, h, r, color
  // humidity
  tft.fillRect(200, 65, 116, 46, GREEN); // x, y, w, h, r, color
  tft.fillRect(202, 67, 112, 42, BLACK); // x, y, w, h, r, color

   volumeUp.initButtonUL(&tft,  4, 130, 90, 40, GREEN, GREEN, WHITE, "Vol +", 2);
   volumeUp.drawButton();
   volumeDown.initButtonUL(&tft,  4, 190, 90, 40, RED, RED, WHITE, "Vol -", 2);
   volumeDown.drawButton();
   chanelUp.initButtonUL(&tft,  100, 130, 90, 40, GREEN, GREEN, WHITE, "CH +", 2);
   chanelUp.drawButton();
   chanelDown.initButtonUL(&tft,  100, 190, 90, 40, RED, RED, WHITE, "CH -", 2);
   chanelDown.drawButton();
   autoSearch.initButtonUL(&tft,  200, 130, 115, 100, BLUE, BLUE, WHITE, "AUTO\TUNE", 2);
   autoSearch.drawButton();

  // tft.setTextSize(2);
  // tft.setTextColor(WHITE);
  // //  vol+
  // tft.fillRect(4, 130, 90, 40, GREEN); // x, y, w, h, r, color
  // tft.setCursor(20, 145);
  // tft.print(F("Vol +"));
  // //  vol-
  // tft.fillRect(4, 190, 90, 40, RED); // x, y, w, h, r, color
  // tft.setCursor(20, 205);
  // tft.print(F("Vol -"));
  // //  ch+
  // tft.fillRect(100, 130, 90, 40, GREEN); // x, y, w, h, r, color
  // tft.setCursor(120, 145);
  // tft.print(F("CH +"));
  // //  ch-
  // tft.fillRect(100, 190, 90, 40, RED); // x, y, w, h, r, color
  // tft.setCursor(120, 205);
  // tft.print(F("CH -"));
  // // auto
  // tft.setTextSize(3);
  // tft.fillRect(200, 130, 115, 100, BLUE); // x, y, w, h, r, color
  // tft.setCursor(220, 145);
  // tft.print(F("AUTO"));
  // tft.setCursor(220, 190);
  // tft.print(F("TUNE"));
}

void MyLCD::updateDHT(MyDht *dht) {
  tft.setTextColor(GREEN);
  tft.setTextSize(2);
  if(dht->isTemperatureChanged()) {
    tft.fillRect(202, 6, 112, 42, BLACK);
    tft.setCursor(215, 17);
    tft.print(dht->temperature());
    tft.setCursor(290, 17);
    tft.print(F("C"));
  }
  if(dht->isHumidityChanged()) {
    tft.fillRect(202, 67, 112, 42, BLACK);
    tft.setCursor(215, 79);
    tft.print(dht->humidity());
    tft.setCursor(290, 79);
    tft.print(F("%"));
  }
  dht->dropChanges();
}

void MyLCD::updateFreq(uint16_t freq) {
  if(freq != currentFreq) {
     tft.fillRect(6, 6, 184, 104, BLACK);
     tft.setTextColor(GREEN);
     tft.setTextSize(2);
     tft.setCursor(20, 17);
     char tmpBuff[10];
     sprintf(tmpBuff, "%3d.%1d MHz", freq / 100, (freq % 100) / 10);
     tft.print(tmpBuff);
     currentFreq = freq;
  }
}

TOUCH_INFO MyLCD::getPressedInfo() {
  Point tp = ts.getPoint();   //tp.x, tp.y are ADC values
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  digitalWrite(YP, HIGH);   //because TFT control pins
  digitalWrite(XM, HIGH);
  TOUCH_INFO info;
  if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE) {
    info.pressed = true;
    info.ypos = map(tp.x, TS_MINX, TS_MAXX, 0, tft.height());
    info.xpos = map(tp.y, TS_MINY, TS_MAXY, tft.width(), 0);
  }
  return info;
}

bool MyLCD::checkPress(Adafruit_GFX_Button &button, TOUCH_INFO &touch, int keyCode) {
  button.press(touch.pressed && button.contains(touch.xpos, touch.ypos));
  if (button.justPressed()) {
    button.drawButton(true);
    if (_onTouch) {
 			_onTouch(keyCode);
 		}
    timing = millis();
    return true;
  }
  if (button.justReleased()) {
      button.drawButton();
  }
}

void MyLCD::detectTouch() {
  if (millis() - timing > touchDelay) {
   TOUCH_INFO touch = getPressedInfo();
//   chanelUp.press(touch.pressed && chanelUp.contains(touch.xpos, touch.ypos));
//   chanelDown.press(touch.pressed && chanelDown.contains(touch.xpos, touch.ypos));

//   volumeUp.press(touch.pressed && volumeUp.contains(touch.xpos, touch.ypos));
//   volumeDown.press(touch.pressed && volumeDown.contains(touch.xpos, touch.ypos));
//   autoSearch.press(touch.pressed && autoSearch.contains(touch.xpos, touch.ypos));


   checkPress(chanelUp, touch, CH_UP);
 }
}
