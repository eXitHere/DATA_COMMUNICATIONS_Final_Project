#include"ServoController.h"
#define pin_horizontal_Servo 11
#define pin_vertical_Servo   10
void setup() {
  // INITIAL
  init_serve(pin_horizontal_Servo, pin_vertical_Servo);
}

void loop() {
  if(Serial.available()) {
    String tmp = Serial.readString();
    if(isCommandToServo(tmp)) {
      // L035040 => L , H=35, V=40
      // IF Edit degree 
      // L = Left
      // R = Right
      // M = Middle
    }
  }
}
