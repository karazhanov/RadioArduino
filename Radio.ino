#include "MyLCD.h"
#include "MyDht.h"
#include "RDA5807M.h"
#include "RDSParser.h"

MyDht dht;
MyLCD lcd;

int button = NONE;

RDA5807M radio;
RADIO_INFO radioInfo;
uint16_t freqStep;
uint16_t maxFreq;
uint16_t minFreq;
char tmpBuff[10];

RDSParser rds;
char rdsServiceName[8 + 2];
char rdsText[64 + 2];
char rdsTime[6];
#define RDS_SCROLL_POSITION_BEGIN -13
int8_t rdsTextScrollPosition = RDS_SCROLL_POSITION_BEGIN;

void setup(void) {
  Serial.begin(9600);
  dht.init();
  lcd.init();
  lcd.attachOnTouch(onTouchScreen);

  radio.init();
	radio.setBand(RADIO_BAND_FM);
  radio.setFrequency(10700);
	radio.attachReceiveRDS(rdsProcess);

 radio.setVolume(5);

delay(1000);

	rds.init();

	rds.attachServicenNameCallback(rdsDisplayServiceName);
	rds.attachTextCallback(rdsDisplayText);
	rds.attachTimeCallback(rdsDisplayTime);
//  lcd.updateFreq(radioInfo.currentFreq);
  Serial.println("debugStatus()");
  radio.debugStatus();
}

void loop() {
    dht.update();
//    radioInfoLoop();
    radio.checkRDS();
     lcd.detectTouch();
     lcd.updateDHT(&dht);
//    lcd.updateFreq(radioInfo.currentFreq);

}

void onTouchScreen(int buttonId) {
  if(button != buttonId) {
    button = buttonId;
    switch (button) {
      case CH_UP:
        Serial.println("CH +");
        radio.seekUp(true);
        break;
      case CH_DOWN:
        Serial.println("CH -");
        radio.seekDown(true);
        break;
      case VOL_UP:
        Serial.println("VOL +");
        break;
      case VOL_DOWN:
        Serial.println("VOL -");
        break;
      case AUTO:
        Serial.println("AUTO");
        break;
       default:;
    }
  }
}

void rdsProcess(uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4) {
  rds.processData(block1, block2, block3, block4);
}

void rdsDisplayServiceName(char *name) {
  Serial.print(F("RDS-service: "));
  strncpy(rdsServiceName, name, sizeof(rdsServiceName));
  Serial.println(name);
}

void rdsDisplayText(char *text) {
//    Serial.print(F("RDS-text: '"));
//    Serial.print(text);
//    Serial.println(F("'"));
    if (strlen(text) == 0) {
      return;
    }
  strncpy(rdsText, text, sizeof(rdsText));
  Serial.print("RDS = ");
  Serial.println(rdsText);
}

void rdsDisplayTime(uint8_t hour, uint8_t minute) {
  Serial.print(F("RDS-time hour: "));
  Serial.print(hour);
  Serial.print(F(", minute: "));
  Serial.println(minute);
  uint8_t hour1 = hour / 10;
  uint8_t hour2 = hour % 10;
  uint8_t minute1 = minute / 10;
  uint8_t minute2 = minute % 10;
  if (hour1 > 2 || hour2 > 9) {
    return;
  }
  if (hour1 == 2 && hour2 > 3) {
    return;
  }
  if (minute1 > 5 || minute2 > 9) {
    return;
  }
  sprintf(rdsTime, "%01d%01d:%01d%01d", hour1, hour2, minute1, minute2);
}

void radioInfoLoop() {
  radio.getRadioInfo(&radioInfo);
  if (radioInfo.tuned) {
    if(radioInfo.rds) {
       Serial.print(F("RDS "));
    } else {
      Serial.print(F("NO RDS "));
    }
//		Serial.print(F("rssi "));
//    uint8_t rssi = map(radioInfo.rssi, 0, 0b111111, 0, 10);
//  	sprintf(tmpBuff, " %2d", radioInfo.rssi);
//  	Serial.print(tmpBuff);
//    sprintf(tmpBuff, " %3d.%1d", radioInfo.currentFreq / 100, (radioInfo.currentFreq % 100) / 10);
//  	Serial.print(tmpBuff);
//  	Serial.println(F(" MHz"));
}}
