#include"ServoController.h"
#define pin_horizontal_Servo 11
#define pin_vertical_Servo   10
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); //pin2 Rx, pin3 Tx
void setup() {
  // INITIAL
  init_serve(pin_horizontal_Servo, pin_vertical_Servo);
  mySerial.begin(115200);
}

void loop() {
  if (Serial.available()) {
    String tmp = Serial.readString();
    if (isCommandToServo(tmp)) {
      // L035040 => L , H=35, V=40
      // IF Edit degree
      // L = Left
      // R = Right
      // M = Middle
    }
  }
  if (mySerial.available()) {
    Serial.print(char(mySerial.read()));
  }
}
