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
  this->crc8 = new CRC8();
  this->crc8->begin();
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

  //Trailer: CRC
  int str_len = toSend.length() + 1;
  unsigned char char_array[str_len]; // prepare a character array (the buffer)
  toSend.toCharArray(char_array, str_len); // copy it over
  uint8_t checksum = crc8->get_crc8(char_array, str_len);
  toSend += char(checksum);
  /*
    int sum = 0;
    for (int i = 0; i < toSend.length(); i++)
    {
    sum += int(toSend[i]);
    }
    (sum % 2 == 0) ? toSend += "1" : toSend += "0";// parity bit-ish
  */
  toSend += ENDFLAG;

  return toSend;
}
bool ProtocolControl::approveDataFrame(String frame)//TODO: CHANGE TO CRC
{
  /*
    Analyze checker and return boolean true if the data is correct
  */
  Serial.println("Checking " + String(frame));
  if (frame.length() != 8)
  {
    Serial.println("Bad Size");
    return false;
  }

  String toCheck = frame.substring(0, 6);
  int str_len = toCheck.length() + 1; // calculate length of message (with one extra character for the null terminator)
  unsigned char char_array[str_len]; // prepare a character array (the buffer)
  toCheck.toCharArray(char_array, str_len); // copy it over
  uint8_t checksum = crc8->get_crc8(char_array, str_len);

  if (char(checksum) == frame[6])
  {
    return true;
  }
  else
  {
    Serial.println("Bad Check");
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

  //Trailer: CRC
  int str_len = toSend.length() + 1;
  unsigned char char_array[str_len]; // prepare a character array (the buffer)
  toSend.toCharArray(char_array, str_len); // copy it over
  uint8_t checksum = crc8->get_crc8(char_array, str_len);
  toSend += char(checksum);
  /*
    int sum = 0;
    for (int i = 0; i < toSend.length(); i++)
    {
    sum += int(toSend[i]);
    }
    (sum % 2 == 0) ? toSend += "1" : toSend += "0";// parity bit-ish
  */
  toSend += ENDFLAG;

  return toSend;
}

bool ProtocolControl::approveAckFrame(String frame)//TODO: CHANGE TO CRC
{
  /*
    Analyze checker and return boolean true if the ack is correct
  */
  Serial.println("Checking " + String(frame));
  if (frame.length() != 6)
  {
    Serial.println("Bad Size");
    return false;
  }

  String toCheck = frame.substring(0, 4);
  int str_len = toCheck.length() + 1; // calculate length of message (with one extra character for the null terminator)
  unsigned char char_array[str_len]; // prepare a character array (the buffer)
  toCheck.toCharArray(char_array, str_len); // copy it over
  uint8_t checksum = crc8->get_crc8(char_array, str_len);

  if (char(checksum) == frame[4])
  {
    return true;
  }
  else
  {
    Serial.println("Bad Check");
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
    Serial.flush();

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
      delay(150);


      long current = millis();
      bool okAck = false;
      while (!okAck)
      {
        Serial.println("ACK Not Okay: Wait for ACK");
        String ackFrame = "";

        long tooLong = millis();

        while (ackFrame.length() < 6)//receive and construct frame
        {
          ackFrame = rx->receiveFM();
        }
        
        /*if (ackFrame.equals("") && millis() - current < TIMEOUT)
        {
          continue;
        }*/


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
          frameNo == "0" ? frameNo = "1" : frameNo = "0";
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
  static const long TOOLONG = 10000;
  long timer = millis();

  String receivedFrame = "";
  //uint16_t temp = rx->receiveFM();
  receivedFrame = rx->receiveFM();

  if (!receivedFrame.equals(""))//Wait for incoming data then start receiving
  {

    //Serial.println("Start Receiving");
    //Serial.println(String(char(temp)) + " " + String(temp));
    timer = millis();

    while (millis() - timer <= TOOLONG)
    {
      /*
        long tooLong = millis();
        while (receivedFrame.length() < 8)//receive and construct frame
        {
        int temp = -1;
        while (temp == -1)
        {
          temp = rx->receiveFM();//wait for 40ms return -1 for bad data
        }
        //Serial.println("After loop temp: " + String(char(temp)));

        if (temp != -1)
        {
          Serial.println("Found this: " + String(char(temp)));
          receivedFrame += char(temp);
          tooLong = millis();
        }
        else if (temp == -1)
        {
          Serial.println("Bad Data " + String(temp));
          receivedFrame = "";
          break;
        }
        else if ( millis() - tooLong >= 120)
        {
          Serial.println("Waiting too Long " + String(millis() - tooLong));
          receivedFrame = "";
          break;
        }

        }
        if (receivedFrame.equals(""))
        {
        continue;
        }*/

      //Serial.println(receivedFrame);


      //String DUMMYFRAME = "oAB0xy" + String(char(206)) + "0";//testing
      bool x = this->approveDataFrame(receivedFrame);//testing
      //receivedFrame = DUMMYFRAME;//testing
      Serial.println(x);
      if (x)
      {
        char dest = receivedFrame[1];
        //Serial.println(String(dest) + " " + this->srcName[0]);
        if (dest == this->srcName[0])
        {
          char frameNumber = receivedFrame[3];
          //Serial.println(String(frameNumber) + " " + String(this->ackNo[0]));
          if (frameNumber == this->ackNo[0])
          {
            for (int i = 4; i < 6; i++)
            {
              if (receivedFrame[i] != '~')
              {
                this->allReceiving += receivedFrame[i];
              }
            }
            Serial.println("OK: " + allReceiving);
            endFlag = receivedFrame[7];
            ackNo == "0" ? ackNo = "1" : ackNo = "0";
            String resAckFrame = this->makeAckFrame(ackNo, "0", destName);//TODO: CHANGE VALUE
            Serial.println("ACK FRame: " + String(resAckFrame));
            for (int i = 0; i < resAckFrame.length(); i++)
            {
              tx->sendFM(resAckFrame[i]);//FM Response Ack
            }

            if (endFlag = '0')
            {
              break;
            }
          }
          else
          {
            Serial.println("Discard Old Frame: Resend ACK");
            String resAckFrame = this->makeAckFrame(ackNo, "0", this->destName);//TODO: CHANGE VALUE
            resend = !resend;
            for (int i = 0; i < resAckFrame.length(); i++)
            {
              tx->sendFM(resAckFrame[i]);//FM Resend Response Ack
            }
          }
        }
        timer = millis();
      }
      else
      {
        Serial.println("Corrupted Frame: " + String(receivedFrame));
        corrupt = !corrupt;
        receivedFrame = rx->receiveFM();
      }
      
    }

    if (millis() - timer > TOOLONG)
    {
      Serial.println("Too Long, Stop Receiving: " + this->allReceiving);
      ackNo = "0";
      this->allReceiving = "";
    }
    else if (endFlag == '0' && !corrupt && !resend)
    {
      Serial.println(this->allReceiving);
      ackNo = "0";
      this->allReceiving = "";
      Serial.print("-------End Of Receiving----------\n\n");
    }

  }
}

void ProtocolControl::stopAndWaitARQWrapper()
{
  this->transmitter();
  this->receiver();
}


/*
  //TODO: CODE THIS AS FALLBACK
  void ProtocolControl::stopAndWaitWrapper()
  {
  this->sw_transmitter();
  this->sw_receiver();
  }
  void ProtocolControl::sw_transmitter()
  {
  String frameNo = "0";
  String receiver = this->destName;
  String textData = "";

  if (Serial.available())//Read data from serial
  {
    this->ackNo = "0";//init ackNo
    this->allReceiving = "";//clear receiver
    textData = Serial.readStringUntil('\n');//read data from serial
    Serial.println(textData);
    Serial.flush();

    while (textData.length() > 0)
    {
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

      int temp;
      do
      {
        temp = rx->receiveFM();
      }while(temp == -1);

    }
  }


  }
  void ProtocolControl::sw_receiver()
  {

  }
*/
