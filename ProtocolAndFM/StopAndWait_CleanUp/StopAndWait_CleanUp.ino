//Test this frame design with Serial first
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11);  // rx, tx

char myName;
char ackNo = '0';
String allReceiving = "";
const String STARTFLAG = "1011";

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
    while (!Serial.available());//wait for input
    receiver = char(Serial.read());//choose receiver
    Serial.print("Receiver is: ");
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

      //ACK timeout
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
      //TODO: Check Corrupted ACK
      String receiveText = mySerial.readStringUntil('\n');
      Serial.println("Receive frame");
      Serial.print("Header    : ");
      Serial.println(receiveText.substring(1, 3));
      Serial.print("ACK No.   : ");
      Serial.println(receiveText[3]);
      frameNo = receiveText[3] - '0';
      Serial.print("Checking  : ");
      Serial.println(receiveText[receiveText.length() - 1]);
      Serial.println("Received\n");

      //Send data
      Serial.print("Send frame : ");
      Serial.println(frameNo);
      Serial.print("Data       : ");
      Serial.println(data);
      Serial.println();

      //change value for next trasmission
      (frameNo == 0) ? frameNo = 1 : frameNo = 0;  //toggle frame number
      textData = textData.substring(10);
      (textData.length() <= 0) ? toSend += '0' : toSend += '1';//end of data?




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
