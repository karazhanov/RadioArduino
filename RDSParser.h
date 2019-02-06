#ifndef __RDSPARSER_H__
#define __RDSPARSER_H__

#include <Arduino.h>

/// callback function for passing a ServicenName
extern "C" {
  typedef void(*receiveServicenNameFunction)(char *name);
  typedef void(*receiveTextFunction)(char *name);
  typedef void(*receiveTimeFunction)(uint8_t hour, uint8_t minute);
}

/// Library for parsing RDS data values and extracting information.
class RDSParser
{
public:
  RDSParser(receiveServicenNameFunction receiveServiceName, receiveTextFunction receiveText, receiveTimeFunction receiveTime) {
    memset(this, 0, sizeof(RDSParser));
    _sendServiceName = receiveServiceName;
  	_sendText = receiveText;
  	_sendTime = receiveTime;
  }; ///< create a new object from this class.
  /// Initialize internal variables before starting or after a change to another channel.
  void init();
  /// Pass all available RDS data through this function.
  void processData(uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4);
private:
  // ----- actual RDS values
  uint8_t rdsGroupType, rdsTP, rdsPTY;
  uint8_t _textAB, _last_textAB, _lastTextIDX;

  // Program Service Name
  char _PSName1[10]; // including trailing '\00' character.
  char _PSName2[10]; // including trailing '\00' character.
  char programServiceName[10];    // found station name or empty. Is max. 8 character long.

  receiveServicenNameFunction _sendServiceName; ///< Registered ServiceName function.
  receiveTimeFunction _sendTime; ///< Registered Time function.
  receiveTextFunction _sendText;

  uint16_t _lastRDSMinutes; ///< last RDS time send to callback.

  char _RDSText[64 + 2];

}; //RDSParser

#endif //__RDSPARSER_H__
