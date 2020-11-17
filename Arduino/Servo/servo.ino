#include <Servo.h>

Servo vertical_Servo;  // create servo object to control a servo
Servo horizontal_Servo;

#define R_DEGREE 138
#define M_DEGREE 87
#define L_DEGREE 37

void setup() {
  horizontal_Servo.attach(5);
  vertical_Servo.attach(3);

  // INITIAL
  horizontal_Servo.write(87);
  vertical_Servo.write(93);
  Serial.begin(115200);
}

int idx_v = 0;
int idx_h = 0;
void loop() {
  if(Serial.available()) {
    char temp = Serial.read();
    int degree_hori = horizontal_Servo.read();
    if(temp == 'R') {
      if(degree_hori < R_DEGREE) {
        for(int i = degree_hori ; i < R_DEGREE ; i++ ) {
          horizontal_Servo.write(i);
          delay(5);
        }
      }
      else {
        for(int i = degree_hori ; i > R_DEGREE ; i-- ) {
          horizontal_Servo.write(i);
          delay(5);
        }
      }
      vertical_Servo.write(90);
    }
    else if(temp =='M') {
      if(degree_hori < M_DEGREE) {
        for(int i = degree_hori ; i < M_DEGREE ; i++ ) {
          horizontal_Servo.write(i);
          delay(5);
        }
      }
      else {
        for(int i = degree_hori ; i > M_DEGREE ; i-- ) {
          horizontal_Servo.write(i);
          delay(5);
        }
      }
      vertical_Servo.write(88);
    }
    else if(temp =='L') {
      if(degree_hori < L_DEGREE) {
        for(int i = degree_hori ; i < L_DEGREE ; i++ ) {
          horizontal_Servo.write(i);
          delay(5);
        }
      }
      else {
        for(int i = degree_hori ; i > L_DEGREE ; i-- ) {
          horizontal_Servo.write(i);
          delay(5);
        }
      }
      vertical_Servo.write(93);
    }
    else if(temp == 'U' and idx_v < 180) {
      horizontal_Servo.write(idx_v++);
      Serial.print("Vertical ");
      Serial.println(idx_v);
    }
    else if(temp == 'I' and idx_v > 0) {
      horizontal_Servo.write(idx_v--);
      Serial.print("Vertical ");
      Serial.println(idx_v);
    }
    else if(temp == 'J' and idx_h < 180) {
      vertical_Servo.write(idx_h++);
      Serial.print("Horizontal ");
      Serial.println(idx_h);
    }
    else if(temp == 'K' and idx_h > 0) {
      vertical_Servo.write(idx_h--);
      Serial.print("Horizontal ");
      Serial.println(idx_h);
    }
  }
}
