#include "MyLCD.h"
#include "MyDht.h"
#include "BTRDA5807M.h"
#include "RDSParser.h"

MyDht dht;
MyLCD lcd;

BT_RDA5807M radio;
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
	// radio.setFrequency(config.configVars.currentFrequency);
	// radio.setVolume(config.configVars.volume);
	// radio.setBassBoost(config.configVars.bassBoost);
	// radio.setSoftMute(config.configVars.softMute);

	radio.attachReceiveRDS(rdsProcess);
	rds.init();
	rds.attachServicenNameCallback(rdsDisplayServiceName);
	rds.attachTextCallback(rdsDisplayText);
	rds.attachTimeCallback(rdsDisplayTime);
  radio.seekUp(true);
}

void loop() {
    dht.update();
    lcd.detectTouch();
    lcd.updateDHT(&dht);
    lcd.updateFreq(radioInfo.currentFreq);
    radioInfoLoop();
}

void onTouchScreen(int buttonId) {
//   Serial.print("BUT = ");
//   Serial.println(buttonId);
}

void rdsProcess(uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4) {
  rds.processData(block1, block2, block3, block4);
}

void rdsDisplayServiceName(char *name) {
  Serial.print(F("RDS-service: "));
  Serial.println(name);
  strncpy(rdsServiceName, name, sizeof(rdsServiceName));
}

void rdsDisplayText(char *text) {
  #ifdef SERIAL_DEBUG
    Serial.print(F("RDS-text: '"));
    Serial.print(text);
    Serial.println(F("'"));
  #endif
    if (strlen(text) == 0) {
      return;
    }
  strncpy(rdsText, text, sizeof(rdsText));
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
  if (radioInfo.tuned) {
		Serial.println(F("RDS"));
    uint8_t rssi = map(radioInfo.rssi, 0, RADIO_MAX_RSSI, 0, 10);
  	sprintf(tmpBuff, "%2d", radioInfo.rssi);
  	Serial.println(tmpBuff);
    sprintf(tmpBuff, "%3d.%1d", radioInfo.currentFreq / 100, (radioInfo.currentFreq % 100) / 10);
  	Serial.print(tmpBuff);
  	Serial.println(F(" MHz"));
}}
