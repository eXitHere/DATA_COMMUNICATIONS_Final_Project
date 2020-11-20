#ifndef PROTOCOL_LIB_H
#define PROTOCOL_LIB_H

#include<Arduino.h>

int addTwo(int,int);//use for testing this import


String makeDataFrame(String, int);
bool approveDataFrame(String);

String makeAckFrame(String, int);
bool approveAckFrame(String);


#endif PROTOCOL_LIB_H
