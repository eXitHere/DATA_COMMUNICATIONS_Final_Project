//https://github.com/dwblair/arduino_crc8 DOWNLOAD THIS LIB FOR CRC8


#include<Arduino.h>
#include"protocol_lib.h"


ProtocolControl::ProtocolControl(String srcName)
{
  this->srcName = srcName;
  this->STARTFLAG = "o";
  this->allReceiving = "";
  this->ackNo = "0";
}
ProtocolControl::~ProtocolControl()
{
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
    data += "~";//type with ASCII 126
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
  String receiver = "B";
  String textData = "";

  if (Serial.available())//Read data from serial
  {
    this->ackNo = "0";//init ackNo
    this->allReceiving = "";//clear receiver
    textData = Serial.readStringUntil('\n');//read data from serial
  }
  bool transmitted = false;

  while (textData.length() > 0)
  {
    transmitted = true;

    String splittedData = textData.substring(0, 2);
    String framedData = "";
    textData = textData.substring(2);

    if (textData.length() > 2)
      framedData = this->makeDataFrame(splittedData, frameNo, "1", receiver);//String textData, String frameNo, String ENDFLAG, String destName
    else
      framedData = this->makeDataFrame(splittedData, frameNo, "0", receiver);//Last frame

    for (int i = 0; i < framedData.length(); i++)
    {
      tx.sendFM(framedData[i]);//FM transmission
    }
    bool okAck = false;

    while (!okAck)
    {
      String ackFrame = "";

      long tooLong = millis();
      while (ackFrame.length() < 6)//receive and construct frame
      {
        int temp = rx.receiveFM();//wait for 40ms return -1
        if (temp == -1 || millis() - tooLong >= 120)
        {
          ackFrame = "";
          break;
        }
        ackFrame += char(temp);
        tooLong = millis();
      }
      if (ackFrame.equals(""))
      {
        continue;
      }

      long current = millis();
      if (!this->approveAckFrame(ackFrame)) //bad ack or no ack
      {
        //Wait for TIMEOUT
        if (millis() - current >= TIMEOUT)
        {
          //timeout
          for (int i = 0; i < framedData.length(); i++)
          {
            tx.sendFM(framedData[i]);//FM retransmission
          }
          current = millis();
        }
      }
      else
      {
        okAck = true;
      }
    }
  }

  if (transmitted == true)
  {
    frameNo = "0";
    textData = "";
    Serial.println("-----End of Trasmission-----");
  }
}

void ProtocolControl::receiver()
{
  bool corrupt = false, resend = false;
  char endFlag = '1';
  bool okFrame = false;
  
  while (!okFrame)//TODO: WAIT FOR INCOMING DATA
  {
    String receivedFrame = "";
    long tooLong = millis();
    while (receivedFrame.length() < 8)//receive and construct frame
    {
      int temp = rx.receiveFM();//wait for 40ms return -1
      if (temp == -1 || millis() - tooLong >= 120)
      {
        receivedFrame = "";
        break;
      }
      receivedFrame += char(temp);
      tooLong = millis();
    }
    if(receivedFrame.equals(""))
    {
      continue;
    }

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
          String resAckFrame = this->makeAckFrame(ackNo, "0", "A");//TODO: CHANGE VALUE

          for (int i = 0; i < resAckFrame.length(); i++)
          {
            tx.sendFM(resAckFrame[i]);//FM Response Ack
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
        String resAckFrame = this->makeAckFrame(ackNo, "0", "A");//TODO: CHANGE VALUE
        resend = !resend;
        for (int i = 0; i < resAckFrame.length(); i++)
        {
          tx.sendFM(resAckFrame[i]);//FM Resend Response Ack
        }

      }
    }
  }
  if (endFlag == '0' && !corrupt && !resend)
  {
    ackNo = "0";
    allReceiving = "";
    Serial.print("\n");
    Serial.print("-------End Of Receiving----------\n\n");
  }
}
