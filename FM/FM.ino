#include "FM_RX.h"
#include "FM_TX.h"

FM_RX* receiver;
FM_TX* transmitter;

void setup()
{
  Serial.begin(115200);
  receiver = new FM_RX(87.7);
  transmitter = new FM_TX();

  Serial.flush();
}

void loop()
{
  transmittion();
  receive();
}

void receive() {
  uint16_t data = receiver->receiveFM();
  if (data != -1)
    Serial.print((char)data);
}

void transmittion() {
  if (Serial.available() > 0)
  {
    int counter = 0;
    char inData[30];
    String inp = Serial.readString();
    //inp += "\n";
    for (int i = 0; i < inp.length(); i++)
    {
      inData[i] = inp[i];
      counter++;
    }

    for ( int i = 0; i < counter; ++i ) {
      transmitter->sendFM(inData[i]);
    }
  }
}
