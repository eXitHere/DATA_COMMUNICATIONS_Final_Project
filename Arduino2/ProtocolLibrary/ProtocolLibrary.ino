#include "protocol_lib.h"
#include"ServoController.h"
#define pin_horizontal_Servo 11
#define pin_vertical_Servo   10
ProtocolControl* protocol;

void setup()
{
  Serial.begin(115200);
  init_serve(pin_horizontal_Servo, pin_vertical_Servo);
  protocol = new ProtocolControl("B", "A", 87.7);
}

void loop()
{
  //protocol->wrapper();
  if (Serial.available()) {
    String tmp = Serial.readStringUntil('\n');
    if (isCommandToServo(tmp)) {
      // L035040 => L , H=35, V=40
      // IF Edit degree
      // L = Left
      // R = Right
      // M = Middle
      // C = Center | Set ready
    }
    else {
      protocol->w_wrapper(tmp);
    }
    Serial.flush();
  }
}
