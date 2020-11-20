//https://github.com/dwblair/arduino_crc8 DOWNLOAD THIS LIB FOR CRC8


#include<Arduino.h>


int addTwo(int a, int b)
{
  return a + b;
}


String makeDataFrame(String textData, String frameNo)
{
  /*
  User must shift the data manually!

  This code will generate a frame of 2 bytes/frame
  Will return a frame of data in this format
  START,dest,src,frameNo,data,data,Check,END

  */
  const String STARTFLAG = "X";//CUSTOM THIS
  const String ENDFLAG = "X";//CUSTOM THIS
  const String srcName = "A", destName = "B";//CUSTOM THIS
  String toSend = "";//Frame will be store here
  //Header
  toSend += STARTFLAG;
  toSend += destName;
  toSend += srcName;
  toSend += frameNo;

  //Body
  String data = textData;
  while (data.length() < 2) //Add padding: DIFF FROM OLD CODE
  {
    data += "~";//ASCII 126
  }
  toSend += data;

  //Trailer
  int sum = 0;
  for (int i = 0; i < toSend.length(); i++)
  {
    sum += int(toSend[i]);
  }
  (sum % 2 == 0) ? toSend += "1" : toSend += "0";// parity bit-ish
  toSend += ENDFLAG;

  return toSend;
}
bool approveDataFrame(String data)//TODO: CHANGE TO CRC
{
  /*
  Analyze checker and return boolean true if the data is correct
  */
    if(data.length() != 8)
  {
    return false;
  }
  
  int sum = 0;
  for(int i=0;i<6;i++)
  {
    sum += int(data[i]);
  }

  if(sum%2 == (data[6]-'0'))
  {
    return true;
  }
  else
  {
    return false;
  }
}

String makeAckFrame(int ackNo)
{
  /*
  User must shift the data manually!

  This code will generate am acknowledge frame
  Will return a frame in this format
  START,dest,src,frameNo,Check,END
  */
  const String STARTFLAG = "X";//CUSTOM THIS
  const String ENDFLAG = "X";//CUSTOM THIS
  const String srcName = "A", destName = "B";//CUSTOM THIS
  String toSend = "";//Frame will be store here
  //Header
  toSend += STARTFLAG;
  toSend += destName;
  toSend += srcName;

  //Ack Number
  toSend += ackNo;

  //Trailer
  int sum = 0;
  for (int i = 0; i < toSend.length(); i++)
  {
    sum += int(toSend[i]);
  }
  (sum % 2 == 0) ? toSend += "1" : toSend += "0";// parity bit-ish
  toSend += ENDFLAG;

  return toSend;
}

bool approveAckFrame(String data)//TODO: CHANGE TO CRC
{
  /*
  Analyze checker and return boolean true if the ack is correct
  */
  if(data.length() != 6)
  {
    return false;
  }
  
  int sum = 0;
  for(int i=0;i<4;i++)
  {
    sum += int(data[i]);
  }

  if(sum%2 == (data[4]-'0'))
  {
    return true;
  }
  else
  {
    return false;
  }
}
