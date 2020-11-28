#ifndef PROTOCOL_LIB_H
#define PROTOCOL_LIB_H

#include <Arduino.h>

#include "FM_RX.h"
#include "FM_TX.h"
#include "CRC8.h"

class ProtocolControl
{
public:
  ProtocolControl(String srcName, String destName, float freq);
  ~ProtocolControl();

  String makeDataFrame(String textData, String frameNo, String ENDFLAG, String destName);
  bool approveDataFrame(String);

  String makeAckFrame(String ackNo, String ENDFLAG, String destName);
  bool approveAckFrame(String);

  void wrapper();
  void transmitter();
  void receiver();

  void w_wrapper(string inp);
  void w_transmitter(string inp);
  void w_receiver();

private:
  String srcName, destName;
  String ackNo;
  String allReceiving;
  String STARTFLAG;
  const int TIMEOUT = 2000;
  const int BACKOFF = 7;

  FM_RX *rx;
  FM_TX *tx;
  CRC8 *crc8;
};

#endif PROTOCOL_LIB_H
