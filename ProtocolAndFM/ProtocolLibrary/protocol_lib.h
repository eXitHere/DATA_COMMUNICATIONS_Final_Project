#ifndef PROTOCOL_LIB_H
#define PROTOCOL_LIB_H

#include <Arduino.h>

#include "FM_RX.h"
#include "FM_TX.h"


class ProtocolControl
{
public:
  ProtocolControl(String);
  ~ProtocolControl();

  String makeDataFrame(String textData, String frameNo, String ENDFLAG = "X", String destName = "B");
  bool approveDataFrame(String);

  String makeAckFrame(String ackNo, String ENDFLAG = "X", String destName = "B");
  bool approveAckFrame(String);

  void transmitter();
  void receiver();


private:
  String srcName;
  String ackNo;
  String allReceiving;
  String STARTFLAG;
  const int TIMEOUT = 2000;
  const int BACKOFF = 7;

  FM_RX rx = FM_RX(87.5);//TODO: CHANGE FREQ
  FM_TX tx = FM_TX();
};

#endif PROTOCOL_LIB_H
