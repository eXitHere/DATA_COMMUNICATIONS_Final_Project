#include "FM_RX.h"
#include "FM_TX.h"

FM_RX* receiver;
FM_TX* transmitter;

void setup()
{
  Serial.begin(115200);
  receiver = new FM_RX(107.5);
  transmitter = new FM_TX();

  Serial.flush();
}

void loop()
{
  if (Serial.available() > 0)
  {
    int counter = 0;
    char inData[15];
    String inp = Serial.readString();
    inp += "\n";
    for (int i = 0; i < inp.length(); i++)
    {
      inData[i] = inp[i];
      counter++;
    }

    for ( int i = 0; i < counter - 1; ++i ) {
      transmitter->sendFM(inData[i]);
    }
  }
}
