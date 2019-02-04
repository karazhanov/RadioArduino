#include "MyLCD.h"

MyLCD::MyLCD() {}

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

  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  //  vol+
  tft.fillRect(4, 130, 90, 40, GREEN); // x, y, w, h, r, color
  tft.setCursor(20, 145);
  tft.print(F("Vol +"));
  //  vol-
  tft.fillRect(4, 190, 90, 40, RED); // x, y, w, h, r, color
  tft.setCursor(20, 205);
  tft.print(F("Vol -"));
  //  ch+
  tft.fillRect(100, 130, 90, 40, GREEN); // x, y, w, h, r, color
  tft.setCursor(120, 145);
  tft.print(F("CH +"));
  //  ch-
  tft.fillRect(100, 190, 90, 40, RED); // x, y, w, h, r, color
  tft.setCursor(120, 205);
  tft.print(F("CH -"));
  // auto
  tft.setTextSize(3);
  tft.fillRect(200, 130, 115, 100, BLUE); // x, y, w, h, r, color
  tft.setCursor(220, 145);
  tft.print(F("AUTO"));
  tft.setCursor(220, 190);
  tft.print(F("TUNE"));
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

void MyLCD::detectTouch() {
     uint16_t xpos, ypos;  //screen coordinates
     tp = ts.getPoint();   //tp.x, tp.y are ADC values
     pinMode(XM, OUTPUT);
     pinMode(YP, OUTPUT);

     if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE) {
        ypos = map(tp.x, TS_MINX, TS_MAXX, 0, tft.height());
        xpos = map(tp.y, TS_MINY, TS_MAXY, tft.width(), 0);
       // ch+
       if(pointInRect(xpos, ypos, 100, 130, 90, 40)) {
          if (_onTouch) {
      			_onTouch(CH_UP);
      		}
          return;
       }
       //  ch-
       if(pointInRect(xpos, ypos, 100, 190, 90, 40)) {
          if (_onTouch) {
      			_onTouch(CH_DOWN);
      		}
          return;
       }
       // vol+
       if(pointInRect(xpos, ypos, 4, 130, 90, 40)) {
          if (_onTouch) {
      			_onTouch(VOL_UP);
      		}
          return;
       }
       // vol-
       if(pointInRect(xpos, ypos, 4, 190, 90, 40)) {
          if (_onTouch) {
      			_onTouch(VOL_DOWN);
      		}
          return;
       }
       // auto
       if(pointInRect(xpos, ypos, 200, 130, 115, 100)) {
          if (_onTouch) {
      			_onTouch(AUTO);
      		}
          return;
       }
   }
   if (_onTouch) {
       _onTouch(NONE);
   }
}

bool MyLCD::pointInRect(uint16_t xpos, uint16_t ypos, uint16_t x0, uint16_t y0, uint16_t w, uint16_t h) {
   return xpos >= x0 &&
           xpos <= (x0 + w) &&
           ypos >= y0 &&
           ypos <= (y0 + h);
}
