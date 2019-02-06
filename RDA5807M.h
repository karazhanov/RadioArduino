#ifndef RDA5807M_h
#define RDA5807M_h

#include <Arduino.h>
#include <Wire.h>

#include "radio.h"

enum RADIO_BAND {
  RADIO_BAND_NONE = 0, ///< No band selected.
  RADIO_BAND_FM = 1, ///< FM band 87.5 � 108 MHz (USA, Europe) selected.
  RADIO_BAND_FMWORLD = 2, ///< FM band 76 � 108 MHz (Japan, Worldwide) selected.
  RADIO_BAND_AM = 3, ///< AM band selected.
  RADIO_BAND_KW = 4, ///< KW band selected.
  RADIO_BAND_MAX = 4  ///< Maximal band enumeration value.
};
/// Frequency data type.
/// Only 16 bits are used for any frequency value (not the real one)
typedef uint16_t RADIO_FREQ;
/// A structure that contains information about the radio features from the chip.
typedef struct RADIO_INFO {
  bool active;   ///< receiving is active.
  uint8_t rssi;  ///< Radio Station Strength Information.
  uint8_t snr;   ///< Signal Noise Ratio.
  bool rds;      ///< RDS information is available.
  bool tuned;    ///< A stable frequency is tuned.
  bool mono;     ///< Mono mode is on.
  bool stereo;   ///< Stereo audio is available
};
/// a structure that contains information about the audio features
typedef struct AUDIO_INFO {
  uint8_t volume;
  bool mute;
  bool softmute;
  bool bassBoost;
};
// ----- Register Definitions -----
// this chip only supports FM mode
#define FREQ_STEPS 10
#define RADIO_REG_CHIPID  0x00
#define RADIO_REG_CTRL    0x02
#define RADIO_REG_CTRL_OUTPUT 0x8000
#define RADIO_REG_CTRL_UNMUTE 0x4000
#define RADIO_REG_CTRL_MONO   0x2000
#define RADIO_REG_CTRL_BASS   0x1000
#define RADIO_REG_CTRL_SEEKUP 0x0200
#define RADIO_REG_CTRL_SEEK   0x0100
#define RADIO_REG_CTRL_RDS    0x0008
#define RADIO_REG_CTRL_NEW    0x0004
#define RADIO_REG_CTRL_RESET  0x0002
#define RADIO_REG_CTRL_ENABLE 0x0001
#define RADIO_REG_CHAN    0x03
#define RADIO_REG_CHAN_SPACE     0x0003
#define RADIO_REG_CHAN_SPACE_100 0x0000
#define RADIO_REG_CHAN_BAND      0x000C
#define RADIO_REG_CHAN_BAND_FM      0x0000
#define RADIO_REG_CHAN_BAND_FMWORLD 0x0008
#define RADIO_REG_CHAN_TUNE   0x0010
//      RADIO_REG_CHAN_TEST   0x0020
#define RADIO_REG_CHAN_NR     0x7FC0
#define RADIO_REG_R4    0x04
#define RADIO_REG_R4_EM50   0x0800
//      RADIO_REG_R4_RES   0x0400
#define RADIO_REG_R4_SOFTMUTE   0x0200
#define RADIO_REG_R4_AFC   0x0100
#define RADIO_REG_VOL     0x05
#define RADIO_REG_VOL_VOL   0x000F
#define RADIO_REG_RA      0x0A
#define RADIO_REG_RA_RDS       0x8000
#define RADIO_REG_RA_RDSBLOCK  0x0800
#define RADIO_REG_RA_STEREO    0x0400
#define RADIO_REG_RA_NR        0x03FF
#define RADIO_REG_RB          0x0B
#define RADIO_REG_RB_FMTRUE   0x0100
#define RADIO_REG_RB_FMREADY  0x0080
#define RADIO_REG_RDSA   0x0C
#define RADIO_REG_RDSB   0x0D
#define RADIO_REG_RDSC   0x0E
#define RADIO_REG_RDSD   0x0F
// I2C-Address RDA Chip for sequential  Access
#define I2C_SEQ  0x10
// I2C-Address RDA Chip for Index  Access
#define I2C_INDX  0x11

/// Library to control the RDA5807M radio chip.
class RDA5807M {
  public:
    // ----- RDA5807M specific implementations -----
    const uint8_t MAXVOLUME = 15;   ///< max volume level for radio implementations.

  RDA5807M(receiveServicenNameFunction receiveServiceName, receiveTextFunction receiveText, receiveTimeFunction receiveTime) {
    memset(this, 0, sizeof(RDA5807M));
    rds = new RDSParser(receiveServiceName, receiveText, receiveTime);
  };

  bool   init();
  void   term();

  // ----- Audio features -----

  void    setVolume(uint8_t newVolume)
  uint8_t getVolume();                  ///< Retrieve the current output volume in the range 0..15.
  void    setBassBoost(bool switchOn);
  bool    getBassBoost();               ///< Retrieve the current bass boost mode setting.;
  void    setMono(bool switchOn);
  bool    getMono();                ///< Retrieve the current mono mode setting.
  void    setMute(bool switchOn);
  bool    getMute();                    ///< Retrieve the current mute mode setting.
  void    setSoftMute(bool switchOn);    ///< Set the soft mute mode (mute on low signals) on or off.
  bool    getSoftMute();                ///< Retrieve the current soft mute mode setting.

  // ----- Receiver features -----
  void       setBand(RADIO_BAND newBand);
  RADIO_BAND getBand();                     ///< Retrieve the current band setting.
  void       setFrequency(RADIO_FREQ newF);
  RADIO_FREQ getFrequency(void);

  void       setBandFrequency(RADIO_BAND newBand, RADIO_FREQ newFreq); ///< Set Band and Frequency in one call.

  void    seekUp(bool toNextSender = true);   // start seek mode upwards
  void    seekDown(bool toNextSender = true); // start seek mode downwards

  // ----- Supporting RDS for RADIO_BAND_FM and RADIO_BAND_FMWORLD
  void    checkRDS();
  void    clearRDS(); ///< Clear RDS data in the attached RDS Receiver by sending 0,0,0,0.

  // ----- Receiver features -----
  RADIO_FREQ getMinFrequency();     ///< Get the minimum frequency of the current selected band.
  RADIO_FREQ getMaxFrequency();     ///< Get the maximum frequency of the current selected band.
  RADIO_FREQ getFrequencyStep();    ///< Get resolution of the current selected band.

  // ----- combined status functions -----
  void getRadioInfo(RADIO_INFO *info); ///< Retrieve some information about the current radio function of the chip.
  void getAudioInfo(AUDIO_INFO *info); ///< Retrieve some information about the current audio function of the chip.

  // ----- Utilitys -----
  /// Format the current frequency for display and printing.
  void formatFrequency(char *s, uint8_t length);

   private:
   RDSParser *rds;
  // ----- local variables
  uint16_t registers[16];  // memory representation of the registers

  // ----- low level communication to the chip using I2C bus

  void     _readRegisters(); // read all status & data registers
  void     _saveRegisters();     // Save writable registers back to the chip
  void     _saveRegister(byte regNr); // Save one register back to the chip

  void     _write16(uint16_t val);        // Write 16 Bit Value on I2C-Bus
  uint16_t _read16(void);

  uint8_t _volume;    ///< Last set volume level.
  bool    _bassBoost; ///< Last set bass Boost effect.
  bool    _mono;      ///< Last set mono effect.
  bool    _mute;      ///< Last set mute effect.
  bool    _softMute;  ///< Last set softMute effect.

  RADIO_BAND _band;   ///< Last set band.
  RADIO_FREQ _freq;   ///< Last set frequency.

  RADIO_FREQ _freqLow;    ///< Lowest frequency of the current selected band.
  RADIO_FREQ _freqHigh;   ///< Highest frequency of the current selected band.
  RADIO_FREQ _freqSteps;  ///< Resulution of the tuner.

  void _printHex4(uint16_t val); ///> Prints a register as 4 character hexadecimal code with leading zeros.
  void int16_to_s(char *s, uint16_t val); ///< Converts a int16 number to a string, similar to itoa, but using the format "00000".
};

#endif
