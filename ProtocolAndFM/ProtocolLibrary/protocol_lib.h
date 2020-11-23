#ifndef PROTOCOL_LIB_H
#define PROTOCOL_LIB_H

#include <Arduino.h>

#include "FM_RX.h"
#include "FM_TX.h"


class ProtocolControl
{
public:
  ProtocolControl(String srcName,String destName,float freq);
  ~ProtocolControl();

  String makeDataFrame(String textData, String frameNo, String ENDFLAG, String destName);
  bool approveDataFrame(String);

  String makeAckFrame(String ackNo, String ENDFLAG, String destName);
  bool approveAckFrame(String);

  void transmitter();
  void receiver();


private:
  String srcName, destName;
  String ackNo;
  String allReceiving;
  String STARTFLAG;
  const int TIMEOUT = 2000;
  const int BACKOFF = 7;

  FM_RX* rx;
  FM_TX* tx;
};

#endif PROTOCOL_LIB_H
