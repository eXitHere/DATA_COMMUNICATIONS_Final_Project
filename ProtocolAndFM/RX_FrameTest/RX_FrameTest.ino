#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11);

char ownerName;
char ackNo = '0';
String allReceiving = "";

void setup()
{
  mySerial.begin(57600);
  Serial.begin(115200);
  delay(200);
  Serial.println("Enter ID :");
  
  while (!Serial.available());
  
  ownerName = Serial.read();
  Serial.print("My ID is : ");
  Serial.println(ownerName);
}

void flushRx()
{
  while (mySerial.available())
  {
    String tmp = mySerial.readString();
  }
}


void receiving()
{
  //can't handle buffer overflowing
  //find way to reset ackNo
  while (mySerial.available()) {
    String incomingText = mySerial.readString();
    Serial.print("Receiving incoming frame : ");
    Serial.println(incomingText);
    bool corrup = false, resend = false;
    

    char open_flag = incomingText[0], reciever = incomingText[1], sender = incomingText[2],
    frame_number = incomingText[3], check = incomingText[6], end_flag = incomingText[7];//extract data

    char open_flag[0], reciever = incomingText[1], sender = incomingText[2],frame_number = incomingText[3], check = incomingText[14], end_flag = incomingText[15];//extract data


    if (reciever == ownerName)//this frame is for me
    {
      if (frame_number == ackNo)
      {
//        Serial.print("Receive frame\nHeader : ");
//        Serial.print(open_flag);
//        Serial.println(reciever);
//        Serial.print("Frame No. : ");
//        Serial.println(frame_number);
        String data = "";

        for (int i = 4; i <= 5; i++)

        for (int i = 4; i <= 13; i++)

        {
          if (char(incomingText[i]) != '~')
            data += char(incomingText[i]);
        }
//        Serial.println("Data : " + data);
//        Serial.print("Checking : ");
//        Serial.println(check);

        int bitChecker = 0;

        for (int i = 0; i <= 5; i++)

        for (int i = 0; i <= 13; i++)

        {
           bitChecker += int(incomingText[i]);
        }
        
        if ((bitChecker % 2 == 0 && check == '1') || (bitChecker % 2 == 1 && check == '0'))
        {
          
          Serial.println("Received");
          Serial.print("All data : ");
          allReceiving += data;
          Serial.println(allReceiving);

          ackNo == '0' ? ackNo = '1' : ackNo = '0';
          Serial.print("\nSend ACK ");
          Serial.println(ackNo);
          String responseAck = "";
          responseAck += open_flag;
          responseAck += sender;
          responseAck += reciever;
          responseAck += ackNo;

          char resCheck = '0';
          int sum = 0;
          for (int i = 0; i < responseAck.length(); i++)
          {
            for (int j = 0; j < 8; j++)
            {
              int x = bitRead(responseAck[i], j);
              if (x == 1)
                sum++;
            }
          }
          
          if (sum % 2 == 0)
            resCheck = '1';

          responseAck += resCheck;
          responseAck += "0";

          Serial.print("Response Ack : ");
          Serial.println(responseAck);

          //send ack
          for (int i = 0; i < responseAck.length(); i++)
          {
            mySerial.write(responseAck[i]);
          }
          
        }
        else
        {
          Serial.println("Corrupted frame");
          corrup = !corrup;
        }
      }
      else
      {
        Serial.println("Discarding old frame");
        Serial.print("\nRe-send ACK");
        Serial.println(ackNo);

        // make frame resend ack
        String responseAck = "";
        responseAck += open_flag;
        responseAck += sender;
        responseAck += reciever;
        responseAck += ackNo;

        char resCheck = '0';
        int sum = 0;
        for (int i = 0; i < responseAck.length(); i++)
        {
          for (int j = 0; j < 8; j++)
          {
            int x = bitRead(responseAck[i], j);
            if (x == 1)
              sum++;
          }
        }
        if (sum % 2 == 0)
          resCheck = '1';

        responseAck += resCheck;
        responseAck += "0";
        
        //resend ack
        for (int i = 0; i < responseAck.length(); i++)
        {
          mySerial.write(responseAck[i]);
        }
        resend = !resend;
      }
      
    }
    else
    {
      Serial.println("Not for me!");
      Serial.println(incomingText);
    }

    flushRx();
    if ( end_flag == '0' and !corrup and !resend ) {
      ackNo = '0';
      allReceiving = "";
      Serial.print("Transmittion finished\n");
      Serial.print("----------------------------------\n\n");
    }
  }
}

void loop()
{
  receiving();
}
