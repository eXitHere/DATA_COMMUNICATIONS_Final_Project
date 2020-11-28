#include <Adafruit_MCP4725.h>
#include <Wire.h>

#include "protocol_lib.h"

ProtocolControl* protocol;

void setup()
{
  Serial.begin(115200);
  protocol = new ProtocolControl("A", "B", 90.0);
}

void loop()
{
  //protocol->wrapper();
  protocol->w_wrapper();
}
