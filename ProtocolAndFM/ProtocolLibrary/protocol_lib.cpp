//https://github.com/dwblair/arduino_crc8 DOWNLOAD THIS LIB FOR CRC8


#include<Arduino.h>
#include"protocol_lib.h"

ProtocolControl::ProtocolControl(String srcName, String destName, float freq)
{
  this->srcName = srcName;
  this->destName = destName;
  this->STARTFLAG = "o";
  this->allReceiving = "";
  this->ackNo = "0";
  this->rx = new FM_RX(freq);
  this->tx = new FM_TX();
  Serial.println("Init completed");
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
  String receiver = this->destName;
  String textData = "";
  int backoff = 0;

  if (Serial.available())//Read data from serial
  {
    this->ackNo = "0";//init ackNo
    this->allReceiving = "";//clear receiver
    textData = Serial.readStringUntil('\n');//read data from serial
    Serial.println(textData);

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
      Serial.println(framedData);

      for (int i = 0; i < framedData.length(); i++)
      {
        tx->sendFM(framedData[i]);//FM transmission
      }
      

      long current = millis();
      bool okAck = false;
      while (!okAck)
      {
        Serial.println("ACK Not Okay: Wait for ACK");
        String ackFrame = "";

        long tooLong = millis();

        while (ackFrame.length() < 6)//receive and construct frame
        {
          int temp = rx->receiveFM();//wait for 40ms return -1 if nothing
          Serial.println(temp);
          if (temp == -1 || millis() - tooLong >= 80)
          {
            ackFrame = "";
            break;
          }
          else
          {
            ackFrame += char(temp);
            tooLong = millis();
          }
        }
        if (ackFrame.equals("") && millis() - current < TIMEOUT)
        {
          continue;
        }


        if (backoff > this->BACKOFF)
        {
          Serial.println("Maximum Back Off: Stop trasmitting");
          return;
        }

        if (!this->approveAckFrame(ackFrame)) //bad ack or no ack
        {
          //Wait for TIMEOUT
          if (millis() - current >= TIMEOUT)
          {
            Serial.println("Timeout: Resend");
            backoff++;
            //timeout
            for (int i = 0; i < framedData.length(); i++)
            {
              tx->sendFM(framedData[i]);//FM retransmission
            }
            current = millis();
          }
        }
        else
        {
          Serial.println("Good Ack: Next Frame");
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

}

void ProtocolControl::receiver()
{
  bool corrupt = false, resend = false;
  bool okFrame = false;
  char endFlag = '1';
  const long TOOLONG = 2000;

  int temp = rx->receiveFM();

  if (temp != -1)//Wait for incoming data
  {
    Serial.println("Start Receiving");
    //Serial.print(temp);
    long timer = millis();

    while (endFlag != '0' && millis() - timer <= TOOLONG)
    {
      String receivedFrame = "";
      long tooLong = millis();
      while (receivedFrame.length() < 8)//receive and construct frame
      {
        int temp = rx->receiveFM();//wait for 40ms return -1
        if (temp == -1 || millis() - tooLong >= 80)
        {
          receivedFrame = "";
          break;
        }
        receivedFrame += char(temp);
        tooLong = millis();
      }
      if (receivedFrame.equals(""))
      {
        continue;
      }

      if (this->approveDataFrame(receivedFrame))
      {
        char dest = receivedFrame[1];
        if (dest == this->srcName[0])
        {
          char frameNumber = receivedFrame[3];
          if (frameNumber == this->ackNo[0])
          {
            this->allReceiving += receivedFrame.substring(4, 6);
            Serial.println("OK: " + allReceiving);
            endFlag = receivedFrame[5];
            ackNo == "0" ? ackNo = "1" : ackNo = "0";
            String resAckFrame = this->makeAckFrame(ackNo, "0", destName);//TODO: CHANGE VALUE
            Serial.println(resAckFrame);
            for (int i = 0; i < resAckFrame.length(); i++)
            {
              tx->sendFM(resAckFrame[i]);//FM Response Ack
            }
            timer = millis();
          }
          else
          {
            Serial.println("Discard Old Frame");
            String resAckFrame = this->makeAckFrame(ackNo, "0", this->destName);//TODO: CHANGE VALUE
            resend = !resend;
            for (int i = 0; i < resAckFrame.length(); i++)
            {
              tx->sendFM(resAckFrame[i]);//FM Resend Response Ack
            }
          }
        }
      }
      else
      {
        Serial.println("Corrupted Frame");
        corrupt = !corrupt;
      }
    }
    
    if (endFlag == '0' && !corrupt && !resend)
    {
      Serial.println(this->allReceiving);
      ackNo = "0";
      this->allReceiving = "";
      Serial.print("-------End Of Receiving----------\n\n");
    }
    else if (millis() - timer > TOOLONG)
    {
      Serial.println("Too Long, Stop Receiving: " + this->allReceiving);
      ackNo = "0";
      this->allReceiving = "";

    }
  }
}
