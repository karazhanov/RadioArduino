#include <Arduino.h>
#include <Wire.h>

#include "RDA5807M.h"

// initialize all internals.
bool RDA5807M::init(receiveServicenNameFunction receiveServiceName, receiveTextFunction receiveText, receiveTimeFunction receiveTime) {
  bool result = false; // no chip found yet.
  rds = new RDSParser(receiveServiceName, receiveText, receiveTime);
  rds->init();
  Wire.begin();
  Wire.beginTransmission(I2C_INDX);
  result = Wire.endTransmission();
  if (result == 0) {
    result = true;
    // initialize all registers
    registers[RADIO_REG_CHIPID] = 0x5804;  // 00 id
    registers[1] = 0x0000;  // 01 not used
    registers[RADIO_REG_CTRL] = (RADIO_REG_CTRL_RESET | RADIO_REG_CTRL_ENABLE);
    setBand(RADIO_BAND_FM);
    registers[RADIO_REG_R4] = RADIO_REG_R4_EM50;//  0x1800;  // 04 DE ? SOFTMUTE
    registers[RADIO_REG_VOL] = 0x9081; // 0x81D1;  // 0x82D1 / INT_MODE, SEEKTH=0110,????, Volume=1
    registers[6] = 0x0000;
    registers[7] = 0x0000;
    registers[8] = 0x0000;
    registers[9] = 0x0000;

    // reset the chip
    _saveRegisters();

    registers[RADIO_REG_CTRL] = RADIO_REG_CTRL_ENABLE;
    _saveRegister(RADIO_REG_CTRL);
  }  // if
  return(result);
} // init()

// switch the power off
void RDA5807M::term() {
  setVolume(0);
  registers[RADIO_REG_CTRL] = 0x0000;   // all bits off
  _saveRegisters();
} // term

// ----- Volume control -----
void RDA5807M::setVolume(uint8_t newVolume) {
  _volume = newVolume;
  newVolume &= RADIO_REG_VOL_VOL;
  registers[RADIO_REG_VOL] &= (~RADIO_REG_VOL_VOL);
  registers[RADIO_REG_VOL] |= newVolume;
  _saveRegister(RADIO_REG_VOL);
} // setVolume()
uint8_t RDA5807M::getVolume() {
  return(_volume);
} // getVolume()

void RDA5807M::setBassBoost(bool switchOn) {
  _bassBoost = switchOn;
  uint16_t regCtrl = registers[RADIO_REG_CTRL];
  if (switchOn)
  regCtrl |= RADIO_REG_CTRL_BASS;
  else
  regCtrl &= (~RADIO_REG_CTRL_BASS);
  registers[RADIO_REG_CTRL] = regCtrl;
  _saveRegister(RADIO_REG_CTRL);
} // setBassBoost()
bool RDA5807M::getBassBoost() {
  return(_bassBoost);
} // getBassBoost()

// Mono / Stereo
void RDA5807M::setMono(bool switchOn) {
  _mono = switchOn;
  registers[RADIO_REG_CTRL] &= (~RADIO_REG_CTRL_SEEK);
  if (switchOn) {
    registers[RADIO_REG_CTRL] |= RADIO_REG_CTRL_MONO;
  }
  else {
    registers[RADIO_REG_CTRL] &= ~RADIO_REG_CTRL_MONO;
  }
  _saveRegister(RADIO_REG_CTRL);
} // setMono
bool RDA5807M::getMono() {
  return(_mono);
} // getMono()

// Switch mute mode.
void RDA5807M::setMute(bool switchOn) {
  _mute = switchOn;
  if (switchOn) {
    // now don't unmute
    registers[RADIO_REG_CTRL] &= (~RADIO_REG_CTRL_UNMUTE);
  }
  else {
    // now unmute
    registers[RADIO_REG_CTRL] |= RADIO_REG_CTRL_UNMUTE;
  } // if
  _saveRegister(RADIO_REG_CTRL);
} // setMute()
bool RDA5807M::getMute() {
  return(_mute);
} // getMute()

// Switch softmute mode.
void RDA5807M::setSoftMute(bool switchOn) {
  _softMute = switchOn;
  if (switchOn) {
    registers[RADIO_REG_R4] |= (RADIO_REG_R4_SOFTMUTE);
  }
  else {
    registers[RADIO_REG_R4] &= (~RADIO_REG_R4_SOFTMUTE);
  } // if
  _saveRegister(RADIO_REG_R4);
} // setSoftMute()
bool RDA5807M::getSoftMute() {
  return(_softMute);
} // getSoftMute()

// ----- Band and frequency control methods -----
// tune to new band.
void RDA5807M::setBand(RADIO_BAND newBand) {
  uint16_t r;
  _band = newBand;
  if (newBand == RADIO_BAND_FM) {
    _freqLow = 8700;
    _freqHigh = 10800;
    _freqSteps = 10;

  }
  else if (newBand == RADIO_BAND_FMWORLD) {
    _freqLow = 7600;
    _freqHigh = 10800;
    _freqSteps = 10;
  }
  if (newBand == RADIO_BAND_FM)
  r = RADIO_REG_CHAN_BAND_FM;
  else if (newBand == RADIO_BAND_FMWORLD)
  r = RADIO_REG_CHAN_BAND_FMWORLD;
  registers[RADIO_REG_CHAN] = (r | RADIO_REG_CHAN_SPACE_100);
} // setBand()
RADIO_BAND RDA5807M::getBand()         { return(_band); }

RADIO_FREQ RDA5807M::getMinFrequency() { return(_freqLow); }
RADIO_FREQ RDA5807M::getMaxFrequency() { return(_freqHigh); }
RADIO_FREQ RDA5807M::getFrequencyStep(){ return(_freqSteps); }

void RDA5807M::setBandFrequency(RADIO_BAND newBand, RADIO_FREQ newFreq) {
  setBand(newBand);
  setFrequency(newFreq);
} // setBandFrequency()

// retrieve the real frequency from the chip after automatic tuning.
RADIO_FREQ RDA5807M::getFrequency() {
  // check register A
  Wire.requestFrom (I2C_SEQ, 2);
  registers[RADIO_REG_RA] = _read16();
  Wire.endTransmission();
  uint16_t ch = registers[RADIO_REG_RA] & RADIO_REG_RA_NR;
  _freq = _freqLow + (ch * 10);  // assume 100 kHz spacing
  return (_freq);
}  // getFrequency

void RDA5807M::setFrequency(RADIO_FREQ newF) {
  _freq = newF;
  uint16_t newChannel;
  uint16_t regChannel = registers[RADIO_REG_CHAN] & (RADIO_REG_CHAN_SPACE | RADIO_REG_CHAN_BAND);
  if (newF < _freqLow) newF = _freqLow;
  if (newF > _freqHigh) newF = _freqHigh;
  newChannel = (newF - _freqLow) / 10;
  regChannel += RADIO_REG_CHAN_TUNE; // enable tuning
  regChannel |= newChannel << 6;
  // enable output and unmute
  registers[RADIO_REG_CTRL] |= RADIO_REG_CTRL_OUTPUT | RADIO_REG_CTRL_UNMUTE | RADIO_REG_CTRL_RDS | RADIO_REG_CTRL_ENABLE; //  | RADIO_REG_CTRL_NEW
  _saveRegister(RADIO_REG_CTRL);
  registers[RADIO_REG_CHAN] = regChannel;
  _saveRegister(RADIO_REG_CHAN);
  // adjust Volume
  _saveRegister(RADIO_REG_VOL);
} // setFrequency()

// start seek mode upwards
void RDA5807M::seekUp(bool toNextSender) {
  // start seek mode
  registers[RADIO_REG_CTRL] |= RADIO_REG_CTRL_SEEKUP;
  registers[RADIO_REG_CTRL] |= RADIO_REG_CTRL_SEEK;
  _saveRegister(RADIO_REG_CTRL);
  if (! toNextSender) {
    // stop scanning right now
    registers[RADIO_REG_CTRL] &= (~RADIO_REG_CTRL_SEEK);
    _saveRegister(RADIO_REG_CTRL);
  } // if
} // seekUp()

// start seek mode downwards
void RDA5807M::seekDown(bool toNextSender) {
  registers[RADIO_REG_CTRL] &= (~RADIO_REG_CTRL_SEEKUP);
  registers[RADIO_REG_CTRL] |= RADIO_REG_CTRL_SEEK;
  _saveRegister(RADIO_REG_CTRL);

  if (! toNextSender) {
    // stop scanning right now
    registers[RADIO_REG_CTRL] &= (~RADIO_REG_CTRL_SEEK);
    _saveRegister(RADIO_REG_CTRL);
  } // if
} // seekDown()

// Load all status registers from to the chip
// registers 0A through 0F
// using the sequential read access mode.
void RDA5807M::_readRegisters() {
  Wire.requestFrom (I2C_SEQ, (6 * 2) );
  for (int i = 0; i < 6; i++) {
    registers[0xA+i] = _read16();
  }
  Wire.endTransmission();
}

// Save writable registers back to the chip
// The registers 02 through 06, containing the configuration
// using the sequential write access mode.
void RDA5807M::_saveRegisters() {
  Wire.beginTransmission(I2C_SEQ);
  for (int i = 2; i <= 6; i++)
  _write16(registers[i]);
  Wire.endTransmission();
} // _saveRegisters

// Save one register back to the chip
void RDA5807M::_saveRegister(byte regNr) {
  Wire.beginTransmission(I2C_INDX);
  Wire.write(regNr);
  _write16(registers[regNr]);
  Wire.endTransmission();
} // _saveRegister

// write a register value using 2 bytes into the Wire.
void RDA5807M::_write16(uint16_t val) {
  Wire.write(val >> 8); Wire.write(val & 0xFF);
} // _write16

// read a register value using 2 bytes in a row
uint16_t RDA5807M::_read16(void) {
  uint8_t hiByte = Wire.read();
  uint8_t loByte = Wire.read();
  return(256*hiByte + loByte);
} // _read16

void RDA5807M::getAudioInfo(AUDIO_INFO *info) {
  // set everything to false and 0.
  memset(info, 0, sizeof(AUDIO_INFO));
  // use current settings
  info->volume = _volume;
  info->mute = _mute;
  info->softmute = _softMute;
  info->bassBoost = _bassBoost;
} // getAudioInfo()

void RDA5807M::clearRDS() {
  rds->processData(0, 0, 0, 0);
} // clearRDS()

void RDA5807M::checkRDS() {
    // check register A
    Wire.requestFrom (I2C_SEQ, 2);
    registers[RADIO_REG_RA] = _read16();
    Wire.endTransmission();

    if (registers[RADIO_REG_RA] & RADIO_REG_RA_RDS) {
      // check for new RDS data available
      uint16_t newData;
      bool result = false;
      Wire.beginTransmission(I2C_INDX);                // Device 0x11 for random access
      Wire.write(RADIO_REG_RDSA);                   // Start at Register 0x0C
      Wire.endTransmission(0);                         // restart condition
      Wire.requestFrom(I2C_INDX, 8, 1);                  // Retransmit device address with READ, followed by 8 bytes
      newData = _read16();
      if (newData != registers[RADIO_REG_RDSA]) { registers[RADIO_REG_RDSA] = newData; result = true; }
      newData = _read16();
      if (newData != registers[RADIO_REG_RDSB]) { registers[RADIO_REG_RDSB] = newData; result = true; }
      newData = _read16();
      if (newData != registers[RADIO_REG_RDSC]) { registers[RADIO_REG_RDSC] = newData; result = true; }
      newData = _read16();
      if (newData != registers[RADIO_REG_RDSD]) { registers[RADIO_REG_RDSD] = newData; result = true; }
      Wire.endTransmission();
      if (result) {
        // new data in the registers
        // send to RDS decoder
        rds->processData(registers[RADIO_REG_RDSA], registers[RADIO_REG_RDSB], registers[RADIO_REG_RDSC], registers[RADIO_REG_RDSD]);
      } // if
    } // if
}

/// Retrieve all the information related to the current radio receiving situation.
void RDA5807M::getRadioInfo(RADIO_INFO *info) {
  memset(info, 0, sizeof(RADIO_INFO));
  info->mono = _mono;
  // read data from registers A .. F of the chip into class memory
  _readRegisters();
  info->active = true; // ???
  if (registers[RADIO_REG_RA] & RADIO_REG_RA_STEREO) info->stereo = true;
  if (registers[RADIO_REG_RA] & RADIO_REG_RA_RDS) info->rds = true;
  info->rssi = registers[RADIO_REG_RB] >> 10;
  if (registers[RADIO_REG_RB] & RADIO_REG_RB_FMTRUE) info->tuned = true;
  if (registers[RADIO_REG_CTRL] & RADIO_REG_CTRL_MONO) info->mono = true;
} // getRadioInfo()
// ----- internal functions -----
void RDA5807M::int16_to_s(char *s, uint16_t val) {
  uint8_t n = 5;
  while (n > 0) {
    n--;
    if ((n == 4) || (val > 0)) {
      s[n] = '0' + (val % 10);
      val = val / 10;
    }
    else {
      s[n] = ' ';
    }
  } // while
} // int16_to_s()
/// Prints a register as 4 character hexadecimal code with leading zeros.
void RDA5807M::_printHex4(uint16_t val) {
  if (val <= 0x000F) Serial.print('0');     // if less 2 Digit
  if (val <= 0x00FF) Serial.print('0');     // if less 3 Digit
  if (val <= 0x0FFF) Serial.print('0');     // if less 4 Digit
  Serial.print(val, HEX);
  Serial.print(' ');
} // _printHex4

void RDA5807M::formatFrequency(char *s, uint8_t length) {
  RADIO_BAND b = getBand();
  RADIO_FREQ f = getFrequency();
  if ((s != NULL) && (length > 10)) {
    *s = '\0';
    if ((b == RADIO_BAND_FM) || (b == RADIO_BAND_FMWORLD)) {
      // " ff.ff MHz" or "fff.ff MHz"
      int16_to_s(s, (uint16_t)f);
      s[5] = s[4]; s[4] = s[3]; s[3] = '.';
      strcpy(s+6, " MHz");
    } // if
  } // if
} // formatFrequency()
// The End.
