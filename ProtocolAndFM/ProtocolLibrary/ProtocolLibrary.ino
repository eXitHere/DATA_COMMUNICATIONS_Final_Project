#include "protocol_lib.h"

ProtocolControl* protocol;

void setup()
{
  Serial.begin(115200);
  protocol = new ProtocolControl("A", "B", 87.7);
}

void loop()
{
  protocol->stopAndWaitARQWrapper();
}
