#include "protocol_lib.h"

ProtocolControl* protocol;

void setup()
{
  Serial.begin(115200);
  protocol = new ProtocolControl("A");
}

void loop()
{
  protocol->transmitter();
  protocol->receiver();
}
