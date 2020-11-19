#include "protocol_lib.h"

void setup()
{
  Serial.begin(9600);
  Serial.println(addTwo(5,6));
}

void loop()
{
}
