//Test this frame design with Serial first
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11);  // rx, tx

char myName;
char ackNo = '0';
String allReceiving = "";
const String STARTFLAG = "O";

void flushRx(int);     //clear all data from selected serial
void receiver();    //operate as receiving end
void transmitter(); //operate as trasmitting end

void setup()
{
  mySerial.begin(57600);
  Serial.begin(115200);
  delay(200);
  Serial.println("Enter device's name as a single character: ");
  while (!Serial.available());

  myName = Serial.read();
  Serial.println(myName);
}

void loop()
{
  trasmitter();
  receiver();
  flushRx(0);
}


void flushRx(int mode)
{
  if (mode == 0)
  {
    while (mySerial.available())
    {
      String tmp = mySerial.readString();
    }
  }
  else
  {
    while (Serial.available())
    {
      String tmp = Serial.readString();
    }
  }
}

void receiver()
{
}
void trasmitter()
{
  int frameNo = 0;
  char receiver;
  String textData = "";
  int completeTrasmission = 0;

  if (Serial.available())
  {
    ackNo = '0';//init ackNo
    allReceiving = "";//clear receiver
    Serial.print("Receiver is: ");
    while (!Serial.available());//wait for input
    receiver = char(Serial.read());//choose receiver
    Serial.println(receiver);
    flushRx(1);
    while (!Serial.available());//wait for input
    textData = Serial.readStringUntil('\n');
    Serial.println(textData);
  }

  while (textData.length() > 0)
  {
    if (textData.length() > 0)
    {
      String toSend = "";
      String data = textData.substring(0, 10); //Real project will be used 10 bits instead 10 chars
      toSend += STARTFLAG;
      toSend += receiver;
      toSend += myName;
      toSend += frameNo;
      if (data.length() < 10) //add padding
      {
        for (int i = data.length(); i < 10; i ++)
        {
          data += '~';
        }
      }
      toSend += data;

      int sum = 0;
      for (int i = 0; i < toSend.length(); i++) {
        sum += int(toSend[i]);
      }
      (sum % 2 == 0) ? toSend += '1' : toSend += '0';

      textData = textData.substring(10);
      (textData.length() <= 0) ? toSend += "00" : toSend += "11";//end of data?
      //Send data
      Serial.print("Send frame : ");
      Serial.println(frameNo);
      Serial.print("Data       : ");
      Serial.println(data);
      Serial.println(toSend);



      for (int i = 0; i < toSend.length(); i++)
      {
        mySerial.write(toSend[i]);
        flushRx(0);
      }

      
      int goodACK = 0;
      while (goodACK != 1) 
      {
        //Wait for ACK and TIMEOUT
        long current = millis();
        while (!mySerial.available())
        {
          if (millis() - current >= 3000)
          {
            Serial.println("Timeout");
            Serial.print("Retransmit frame: ");
            Serial.println(frameNo);
            Serial.print("Data       : ");
            for (int i = 0; i < toSend.length(); i++) {
              mySerial.write(toSend[i]);
              flushRx(0);
            }
            Serial.println(data);
            Serial.println();
            current = millis();
          }
        }

        //Read ACK
        //Change Bit Size in production
        Serial.println();
        String receiveText = mySerial.readStringUntil('\n');
        Serial.println(receiveText);
        /*
        Serial.println("Receive frame");
        Serial.print("Header    : ");
        Serial.println(receiveText.substring(0, 20));
        Serial.print("ACK No.   : ");
        Serial.println(receiveText[20]);
        Serial.print("Checking  : ");
        Serial.println(receiveText[21]);
        Serial.print("End flag  :");
        Serial.println(receiveText[22]);
        */
        sum = 0;
        for (int i = 0; i < 7; i++)
        {
          sum += int(receiveText[i]);
        }
        if (sum % 2 == (receiveText[7] - '0'))
        {
          Serial.println("Receive ACK");
          frameNo = (frameNo + 1) % 2;
          goodACK = 1;
        }
        else
        {
          Serial.println("Corrupted ACK");
        }
      }


    }
    else
    {
      Serial.print("\nSend Text : ");
      while (!Serial.available());
      textData = Serial.readStringUntil('\n');
      Serial.println(textData);
    }
    completeTrasmission = 1;
  }
  if (completeTrasmission == 1)
  {
    frameNo = 0;
    textData = "";
    completeTrasmission = 0;
    completeTrasmission = 0;
    Serial.println("-----End of Trasmission-----");
  }

}
