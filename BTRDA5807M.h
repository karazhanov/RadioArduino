/*
 * BTRDA5807M.h
 *
 *  Created on: 2018. júl. 5.
 *      Author: BT-Soft
 *
 * Eredeti ötlet:
 * @see: https://github.com/mathertel/Radio
 *
 */
#ifndef BTRDA5807M_H_
#define BTRDA5807M_H_

#include <Arduino.h>

//-------------------------------------------------------------------------------------------------------------------------
// I2C-Address szekvenciális eléréshez
#define I2C_SEQ  		0x10

// I2C-Address indexelt eléréshez
#define I2C_INDX  		0x11


#define RADIO_MAX_RSSI 		0b111111	/* , 63d, 0x3F, az RSSI logaritmikus érték! */
#define RADIO_MAX_VOLUME  	15

//Frekvencia határok
const uint16_t RDA5807M_BAND_LO[4] PROGMEM = { 8700, 7600, 7600, 6500 };
const uint16_t RDA5807M_BAND_HI[4] PROGMEM = { 10800, 9100, 10800, 7600 };
const uint8_t RDA5807M_BAND_SPACING[4] PROGMEM = { 100, 200, 50, 25 };

// ----- type definitions -----

/// Band datatype.
/// The BANDs a receiver probably can implement.
enum RADIO_BAND {
	RADIO_BAND_FM = 0, 			// 00 = 87-108 MHz (US/Europe)
	RADIO_BAND_FM_JAPAN = 1, 	// 01 = 76-91 MHz (Japan).
	RADIO_BAND_FM_WORLD = 2, 	// 10 = 76-108 MHz (world wide)
	RADIO_BAND_FM_EAST = 3, 	// 11 = 65-76 MHz East Europe or 50-65MHz
};

/// A structure that contains information about the radio features from the chip.
typedef struct RADIO_INFO {
	uint16_t currentFreq; 	// Aktuális vételi frekvencia
	uint8_t rssi;  			// Radio Station Strength Information.
	bool rds;      			// RDS information is available.
	bool tuned;    			// A stable frequency is tuned.
	//--
	bool stereo;   			// Stereo audio is available
	uint8_t volume;			// Aktuális hangerő
	bool bassBoost;			//bassBoost
	bool softMute;
};

/// callback function for passing RDS data.
extern "C" {
typedef void (*receiveRDSFunction)(uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4);
}

class BT_RDA5807M {
public:

	bool _debugEnabled = true;

	BT_RDA5807M();

	bool init();
	void term();

// ----- Audio features -----

	void setVolume(uint8_t newVolume);
	uint8_t getVolume(void) {
		return _volume;
	}
	void setBassBoost(bool switchOn);bool getBassBoost(void) {
		return _bassBoost;
	}
	void setMono(bool switchOn);
	void setMute(bool switchOn);
	void setSoftMute(bool switchOn);    // Set the soft mute mode (mute on low signals) on or off.
	bool getSoftMute(void) {
		return _softMute;
	}

// ----- Receiver features -----
	void setBand(RADIO_BAND newBand);
	void setFrequency(uint16_t newF);
	void setBandFrequency(RADIO_BAND newBand, uint16_t newFreq) {
		setBand(newBand);
		setFrequency(newFreq);
	}

	uint16_t getFrequency(void);

	void seekUp(bool toNextSender = true);   // start seek mode upwards
	void seekDown(bool toNextSender = true); // start seek mode downwards

// ----- combined status functions -----

	void getRadioInfo(RADIO_INFO *info); // Retrieve some information about the current radio function of the chip.

	uint16_t getFrequencyStep(void) {
		return _freqSteps;
	}
	uint16_t getMaxFrequency(void) {
		return _freqHigh;
	}
	uint16_t getMinFrequency(void) {
		return _freqLow;
	}

	void checkRDS(void);
	void clearRDS() {
		if (_sendRDS) {
			_sendRDS(0x0000, 0x0000, 0x0000, 0x0000);
		}
	}

	// send valid and good data to the RDS processor via newFunction
	// remember the RDS function
	void attachReceiveRDS(receiveRDSFunction newFunction) {
		_sendRDS = newFunction;
	}

private:

	uint8_t _volume = 0;    			// Last set volume level.
	RADIO_BAND _band = RADIO_BAND_FM;   // Last set band.
	uint16_t _currentFreq = 10330;   	// Last set frequency.

	uint16_t _freqLow = RDA5807M_BAND_LO[RADIO_BAND_FM];    // Lowest frequency of the current selected band.
	uint16_t _freqHigh = RDA5807M_BAND_HI[RADIO_BAND_FM];   // Highest frequency of the current selected band.
	uint16_t _freqSteps = RDA5807M_BAND_SPACING[RADIO_BAND_FM];  // Resolution of the tuner.

	bool _bassBoost;bool _softMute;

// ----- local variables

	uint16_t reg02;
	uint16_t reg03;
	uint16_t reg04;
	uint16_t reg05;
	uint16_t reg06;
	uint16_t reg07;

	uint16_t reg0A;
	uint16_t reg0B;
	uint16_t reg0C;
	uint16_t reg0D;
	uint16_t reg0E;
	uint16_t reg0F;

// ----- low level communication to the chip using I2C bus

	void _readRegisters(); // read all status & data registers
	void _saveRegisters();     // Save writable registers back to the chip
	void _saveRegister(uint8_t regAddresss, uint16_t value); // Save one register back to the chip

	void _write16(uint16_t val);        // Write 16 Bit Value on I2C-Bus
	uint16_t _read16(void);

	receiveRDSFunction _sendRDS = NULL; // Registered RDS Function that is called on new available data.

};

#endif /* BTRDA5807M_H_ */
