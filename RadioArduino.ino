#include "MyLCD.h"
#include "MyDht.h"
#include "RDA5807M.h"

MyDht dht;
MyLCD lcd;

int button = NONE;

RDA5807M radio;
RADIO_INFO radioInfo;
uint16_t freqStep;
uint16_t maxFreq;
uint16_t minFreq;
char tmpBuff[12];

char rdsServiceName[8 + 2];
char rdsText[64 + 2];
char rdsTime[6];

void setup(void) {
  Serial.begin(9600);
  dht.init();
  lcd.init();
  lcd.attachOnTouch(onTouchScreen);

  radio.init(rdsDisplayServiceName, rdsDisplayText, rdsDisplayTime);
	radio.setBand(RADIO_BAND_FM);
  radio.setFrequency(10700);
  radio.setVolume(5);

  delay(1000);
  radio.formatFrequency(tmpBuff, 12);
  lcd.updateFreq(radio.getFrequency(), tmpBuff); 
}

void loop() {
    lcd.detectTouch();
    dht.update();
//    radioInfoLoop();
    lcd.updateDHT(&dht);

    radio.getRadioInfo(&radioInfo);
    if( radioInfo.tuned) {
      radio.formatFrequency(tmpBuff, 12);
      lcd.updateFreq(radio.getFrequency(), tmpBuff);
    }
}

void onTouchScreen(int buttonId) {
  if(button != buttonId) {
    button = buttonId;
    switch (buttonId) {
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
    button = NONE;
  }
}

void rdsDisplayServiceName(char *name) {
  Serial.print(F("RDS-service: "));
  strncpy(rdsServiceName, name, sizeof(rdsServiceName));
  Serial.println(name);
}

void rdsDisplayText(char *text) {
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
  long f = radio.getFrequency();
  sprintf(tmpBuff, " %5d MHz |", f);
  Serial.print(tmpBuff);
  sprintf(tmpBuff, " %3d.%2d MHz", f / 100, f % 100);
  Serial.print(tmpBuff);
  sprintf(tmpBuff, "| tuned=%1d", radioInfo.tuned?1:0);
  Serial.print(tmpBuff);
  sprintf(tmpBuff, "| rds=%1d", radioInfo.rds?1:0);
  Serial.print(tmpBuff);
  sprintf(tmpBuff, "| rssi=%d", radioInfo.rssi);
  Serial.print(tmpBuff);
  sprintf(tmpBuff, "| snr=%d", radioInfo.snr);
  Serial.println(tmpBuff);
  
  
//  if (radioInfo.tuned) {
//    if(radioInfo.rds) {
//       Serial.println(F("RDS "));
//    }
//		Serial.print(F("rssi "));
//    uint8_t rssi = map(radioInfo.rssi, 0, 0b111111, 0, 10);
//  	sprintf(tmpBuff, " %2d", radioInfo.rssi);
//  	Serial.print(tmpBuff);
//    sprintf(tmpBuff, " %3d.%1d", radioInfo.currentFreq / 100, (radioInfo.currentFreq % 100) / 10);
//  	Serial.print(tmpBuff);
//  	Serial.println(F(" MHz"));
//}
}
