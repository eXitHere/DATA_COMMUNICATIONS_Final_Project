#include "protocol_lib.h"

ProtocolControl protocol("A", "o");

void setup()
{
  Serial.begin(9600);
  Serial.println(protocol.addTwo(5, 6));
}

void loop()
{
}
