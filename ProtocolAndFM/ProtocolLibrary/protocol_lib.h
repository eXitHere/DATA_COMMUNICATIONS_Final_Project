#ifndef PROTOCOL_LIB_H
#define PROTOCOL_LIB_H

#include <Arduino.h>
class ProtocolControl
{
public:
  ProtocolControl(String);
  ~ProtocolControl();

  int addTwo(int, int); //use for testing this import

  String makeDataFrame(String textData, String frameNo, String ENDFLAG = "X", String destName = "B");
  bool approveDataFrame(String);

  String makeAckFrame(String ackNo, String ENDFLAG = "X", String destName = "B");
  bool approveAckFrame(String);

  void transmitter();
  void receiver();

  String decodeFM(String);

private:
  String srcName;
  String ackNo;
  String allReceiving;
  String STARTFLAG;
  const int TIMEOUT = 2000;
};

#endif PROTOCOL_LIB_H
