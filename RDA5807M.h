#ifndef RDA5807M_h
#define RDA5807M_h

#include <Arduino.h>
#include <Wire.h>

#include "radio.h"

// ----- library definition -----

/// Library to control the RDA5807M radio chip.
class RDA5807M : public RADIO {
  public:
    // ----- RDA5807M specific implementations -----
    const uint8_t MAXVOLUME = 15;   ///< max volume level for radio implementations.

  RDA5807M();
  
  bool   init();
  void   term();
  
  // ----- Audio features -----
  
  void   setVolume(uint8_t newVolume);
  void   setBassBoost(bool switchOn);
  void   setMono(bool switchOn);
  void   setMute(bool switchOn);
  void   setSoftMute(bool switchOn);    ///< Set the soft mute mode (mute on low signals) on or off.

  // ----- Receiver features -----
  void   setBand(RADIO_BAND newBand);
  void   setFrequency(RADIO_FREQ newF);
  RADIO_FREQ getFrequency(void);

  void    seekUp(bool toNextSender = true);   // start seek mode upwards
  void    seekDown(bool toNextSender = true); // start seek mode downwards
  
  // ----- Supporting RDS for RADIO_BAND_FM and RADIO_BAND_FMWORLD

  void    checkRDS();

  // ----- combined status functions -----

  virtual void getRadioInfo(RADIO_INFO *info); ///< Retrieve some information about the current radio function of the chip.

  // ----- Supporting RDS for RADIO_BAND_FM and RADIO_BAND_FMWORLD

   private:
  // ----- local variables
  uint16_t registers[16];  // memory representation of the registers

  // ----- low level communication to the chip using I2C bus

  void     _readRegisters(); // read all status & data registers
  void     _saveRegisters();     // Save writable registers back to the chip
  void     _saveRegister(byte regNr); // Save one register back to the chip
  
  void     _write16(uint16_t val);        // Write 16 Bit Value on I2C-Bus
  uint16_t _read16(void);
};

#endif
