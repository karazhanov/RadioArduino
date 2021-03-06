#ifndef __MY_LCD_H__
  #define __MY_LCD_H__

  #include "MyDht.h"
  #include <Adafruit_GFX.h>
  #include <MCUFRIEND_kbv.h>
  #include <TouchScreen.h>
  #include <Adafruit_Sensor.h>

  #define MINPRESSURE 250
  #define MAXPRESSURE 500
  #define TS_MINX 152
  #define TS_MINY 145
  #define TS_MAXX 927
  #define TS_MAXY 968
  // Assign human-readable names to some common 16-bit color values:
  #define BLACK           0x0000
  #define BLUE            0x055E
  #define RED             0xF206
  #define GREEN           0x8E09
  #define WHITE           0xFFFF

  #define Orientation 3    //PORTRAIT
  const int XP=7, XM=A1, YP=A2, YM=6; //ID=0x9341
  const int TS_LEFT=538, TS_RT=570, TS_TOP=638, TS_BOT=680;

#define VOL_UP 1
#define VOL_DOWN 2
#define CH_UP 3
#define CH_DOWN 4
#define AUTO 5
#define NONE -1

#define touchDelay 500

extern "C" {
  typedef void (*onTouch)(int button);
}

typedef struct TOUCH_INFO {
  uint16_t xpos = -1;
  uint16_t ypos = -1;
  bool pressed = false;
};

class MyLCD {
  private:
    void drawRadioUI();
    TOUCH_INFO getPressedInfo();
    bool checkPress(Adafruit_GFX_Button &button, TOUCH_INFO &touch, int keyCode);

    MCUFRIEND_kbv tft;       // hard-wired for UNO shields anyway.
    TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
    Point tp;
    uint16_t currentFreq = -1;
    unsigned long timing = 0;

    onTouch _onTouch = NULL;
    Adafruit_GFX_Button chanelUp, chanelDown, volumeUp, volumeDown, autoSearch;
  public:
    void init();
    void updateDHT(MyDht *dht);
    void updateFreq(uint16_t, char*);
    void detectTouch();
    void attachOnTouch(onTouch newFunction) {
      _onTouch = newFunction;
    }
};

#endif
