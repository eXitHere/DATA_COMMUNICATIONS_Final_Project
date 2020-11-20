//https://github.com/dwblair/arduino_crc8 DOWNLOAD THIS LIB FOR CRC8


#include<Arduino.h>
#include"protocol_lib.h"

ProtocolControl::ProtocolControl(String srcName, String startFlag)
{
  this->srcName = srcName;
  this->STARTFLAG = startFlag;
  this->allReceiving = "";
  this->ackNo = "0";
}
ProtocolControl::~ProtocolControl()
{
}

int ProtocolControl::addTwo(int a, int b)
{
  return a + b;
}


String ProtocolControl::makeDataFrame(String textData, String frameNo, String ENDFLAG, String destName)
{
  /*
    User must shift the data manually!

    This code will generate a frame of 2 bytes/frame
    Will return a frame of data in this format
    START,dest,src,frameNo,data,data,Check,END

  */
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
bool ProtocolControl::approveDataFrame(String frame)//TODO: CHANGE TO CRC
{
  /*
    Analyze checker and return boolean true if the data is correct
  */
  if (frame.length() != 8)
  {
    return false;
  }

  int sum = 0;
  for (int i = 0; i < 6; i++)
  {
    sum += int(frame[i]);
  }

  if (sum % 2 == int(frame[6] - '0'))
  {
    return true;
  }
  else
  {
    return false;
  }
}

String ProtocolControl::makeAckFrame(String ackNo, String ENDFLAG, String destName)
{
  /*
    User must shift the data manually!

    This code will generate am acknowledge frame
    Will return a frame in this format
    START,dest,src,frameNo,Check,END
  */
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

bool ProtocolControl::approveAckFrame(String frame)//TODO: CHANGE TO CRC
{
  /*
    Analyze checker and return boolean true if the ack is correct
  */
  if (frame.length() != 6)
  {
    return false;
  }

  int sum = 0;
  for (int i = 0; i < 4; i++)
  {
    sum += int(frame[i]);
  }

  if (sum % 2 == int(frame[4] - '0'))
  {
    return true;
  }
  else
  {
    return false;
  }
}

void ProtocolControl::transmitter()
{
  String frameNo = "0";
  char receiver;
  String textData = "";
  int completeTrasmission = 0;

  if (Serial.available())
  {
    ackNo = '0';//init ackNo
    allReceiving = "";//clear receiver
    //Serial.print("Receiver is: ");
    while (!Serial.available());//wait for input
    receiver = char(Serial.read());//choose receiver
    Serial.println(receiver);
    while (!Serial.available());//wait for input
    textData = Serial.readStringUntil('\n');
    Serial.println(textData);
  }

  while (textData.length() > 0)
  {
    String splittedData = textData.substring(0, 2);
    String framedData = this->makeDataFrame(splittedData, frameNo, "o", "B");//String textData, String frameNo, String ENDFLAG, String destName
    textData = textData.substring(2);//TODO: ADD IDENTIFY END OF TRANSMISSION TO FRAME: (textData.length() <= 0) ? toSend += "0" : toSend += "1";//end of data? (8 bits)

    for (int i = 0; i < framedData.length(); i++)
    {
      //TODO: SEND DATA WITH FSK
    }


    //TODO: RECEIVE ACK FRAME FROM FM
    String ackFrame = "TEMPACKFRAME";
    while (!this->approveAckFrame(ackFrame))
    {
      //Wait for ACK and TIMEOUT
      long current = millis();
      while (!true)//TODO: WHILE NOTHING ARRIVE
      {
        if (millis() - current >= TIMEOUT)
        {
          //TIMEOUT
          for (int i = 0; i < framedData.length(); i++)
          {
            //TODO: RETRASMISSION WITH FSK
          }
          current = millis();
        }
      }
    }
  }

  frameNo = "0";
  textData = "";
  completeTrasmission = 0;
  Serial.println("-----End of Trasmission-----");
}


String ProtocolControl::decodeFM(String receiving)
{
  return receiving;
}

void ProtocolControl::receiver()
{
  bool corrupt = false, resend=false;
  char endFlag='1';
  while (true)//TODO: WHILE INCOMING DATA EXIST
  {
    String receivedFrame = decodeFM("TEMPDATA");//TODO: RECEIVE DATA FROM ANALOG AND DECODE

    char dest = receivedFrame[1];
    if (dest == this->srcName[0])
    {
      char frameNumber = receivedFrame[3];
      if (frameNumber == ackNo[0])
      {
        if (this->approveDataFrame(receivedFrame))
        {
          endFlag = receivedFrame[5];
          ackNo == "0" ? ackNo = "1" : ackNo = "0";
          String resAckFrame = this->makeAckFrame(ackNo, "x", "B");//TODO: CHANGE VALUE

          for (int i = 0; i < resAckFrame.length(); i++)
          {
            //TODO: SEND ACK FRAME WITH FSK
          }
        }
        else
        {
          Serial.println("Corrupt Frame");
          corrupt = !corrupt;
        }
      }
      else
      {
        Serial.println("Discard Old Frame");
        String resAckFrame = this->makeAckFrame(ackNo, "x", "B");//TODO: CHANGE VALUE
        resend = !resend;
        for (int i = 0; i < resAckFrame.length(); i++)
        {
          //TODO: SEND ACK FRAME WITH FSK
        }
        
      }
    }
  }
  if(endFlag == '0' && !corrupt && !resend)
  {
    ackNo = "0";
      allReceiving = "";
      Serial.print("\n");
      Serial.print("-------End Of Receiving----------\n\n");
  }
}
