#ifndef __RADIO_h__
#define __RADIO_h__

#include <Arduino.h>
#include <Wire.h>

extern "C" {
  typedef void(*receiveRDSFunction)(uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4);
}


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

// ----- common RADIO class definition -----

/// Library to control radio chips in general. This library acts as a base library for the chip specific implementations.
class RADIO {
public:
  const uint8_t MAXVOLUME = 15; ///< max volume level for all radio implementations.
  RADIO(); ///< create a new object from this class.
  virtual bool   init();  ///< initialize library and the chip.
  virtual void   term();  ///< terminate all radio functions.
  // ----- Audio features -----
  virtual void    setVolume(uint8_t newVolume); ///< Control the volume output of the radio chip in the range 0..15.
  virtual uint8_t getVolume();                  ///< Retrieve the current output volume in the range 0..15.
  virtual void    setMute(bool switchOn);       ///< Control the mute mode of the radio chip.
  virtual bool    getMute();                    ///< Retrieve the current mute mode setting.
  virtual void    setSoftMute(bool switchOn);   ///< Control the softmute mode (mute on low signals) of the radio chip.
  virtual bool    getSoftMute();                ///< Retrieve the current soft mute mode setting.
  virtual void    setBassBoost(bool switchOn);  ///< Control the bass boost mode of the radio chip.
  virtual bool    getBassBoost();               ///< Retrieve the current bass boost mode setting.

  // ----- Receiver features -----
  virtual RADIO_FREQ getMinFrequency();     ///< Get the minimum frequency of the current selected band.
  virtual RADIO_FREQ getMaxFrequency();     ///< Get the maximum frequency of the current selected band.
  virtual RADIO_FREQ getFrequencyStep();    ///< Get resolution of the current selected band.

  virtual void       setBand(RADIO_BAND newBand);   ///< Set the current band.
  virtual RADIO_BAND getBand();                     ///< Retrieve the current band setting.

  virtual void       setFrequency(RADIO_FREQ newF); ///< Start using the new frequency for receiving.
  virtual RADIO_FREQ getFrequency(void);            ///< Retrieve the current tuned frequency.

  virtual void       setBandFrequency(RADIO_BAND newBand, RADIO_FREQ newFreq); ///< Set Band and Frequency in one call.

  virtual void       seekUp(bool toNextSender = true);   ///< Start a seek upwards from the current frequency.
  virtual void       seekDown(bool toNextSender = true); ///< Start a seek downwards from the current frequency.

  virtual void       setMono(bool switchOn);   ///< Control the mono mode of the radio chip.
  virtual bool       getMono();                ///< Retrieve the current mono mode setting.

  // ----- combined status functions -----
  virtual void getRadioInfo(RADIO_INFO *info); ///< Retrieve some information about the current radio function of the chip.
  virtual void getAudioInfo(AUDIO_INFO *info); ///< Retrieve some information about the current audio function of the chip.
  // ----- Supporting RDS for FM bands -----
  virtual void checkRDS(); ///< Check if RDS Data is available and good.
  virtual void clearRDS(); ///< Clear RDS data in the attached RDS Receiver by sending 0,0,0,0.
  virtual void attachReceiveRDS(receiveRDSFunction newFunction); ///< Register a RDS processor function.
  // ----- Utilitys -----
  /// Format the current frequency for display and printing.
  virtual void formatFrequency(char *s, uint8_t length);

protected:

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

  receiveRDSFunction _sendRDS; ///< Registered RDS Function that is called on new available data.

  void _printHex4(uint16_t val); ///> Prints a register as 4 character hexadecimal code with leading zeros.

private:
  void int16_to_s(char *s, uint16_t val); ///< Converts a int16 number to a string, similar to itoa, but using the format "00000".

}; // class RADIO
#endif
