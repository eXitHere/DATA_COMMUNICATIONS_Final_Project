#include"ServoController.h"
#define R_ADDRESS 0
#define R_T_ADDRESS 1
#define M_ADDRESS 2
#define M_T_ADDRESS 3
#define L_ADDRESS 4
#define L_T_ADDRESS 5
int R_DEGREE = 0;
int R_T_DEGREE = 0;
int M_DEGREE = 0;
int M_T_DEGREE = 0;
int L_DEGREE = 0;
int L_T_DEGREE = 0;
Servo vertical_Servo;
Servo horizontal_Servo;

void writeServoTo(int ser, int newPos) {
  /*
     ser
     0 = horizontal_Servo
     1 = vertical_Servo
  */
  int now_degree = 0;
  if (ser == 0) {
    now_degree = horizontal_Servo.read();
    if (now_degree < newPos) {
      for (int i = now_degree ; i < newPos ; i++ ) {
        horizontal_Servo.write(i);
        delay(5);
      }
    }
    else {
      for (int i = now_degree ; i > newPos ; i-- ) {
        horizontal_Servo.write(i);
        delay(5);
      }
    }
  }
  else if (ser == 1) {
    now_degree = vertical_Servo.read();
    if (now_degree < newPos) {
      for (int i = now_degree ; i < newPos ; i++ ) {
        vertical_Servo.write(i);
        delay(5);
      }
    }
    else {
      for (int i = now_degree ; i > newPos ; i-- ) {
        vertical_Servo.write(i);
        delay(5);
      }
    }
  }

}


void moveTo(char degree) {
  writeServoTo(1, 130);
  switch (degree) {
    case 'L':
      writeServoTo(0, L_DEGREE);
      writeServoTo(1, L_T_DEGREE);
      //Serial.println("L command");
      break;
    case 'M':
      writeServoTo(0, M_DEGREE);
      writeServoTo(1, M_T_DEGREE);
      //Serial.println("M command");
      break;
    case 'R':
      writeServoTo(0, R_DEGREE);
      writeServoTo(1, R_T_DEGREE);
      //Serial.println("R command");
      break;
    case 'C':
      horizontal_Servo.write(90);
      vertical_Servo.write(45);
      break;
  }
}

int valueFromString(char x0, char x1, char x2) {
  return (x0 - '0') * 100 + (x1 - '0') * 10 + (x2 - '0') * 1;
}

void updateValue() {
  /*for (int i = 0; i < 6; i++) {
    Serial.print(i);
    Serial.print(" ");
    Serial.println(readValue(i));
    }*/
  R_DEGREE = readValue(R_ADDRESS);
  R_T_DEGREE = readValue(R_T_ADDRESS);
  L_DEGREE = readValue(L_ADDRESS);
  L_T_DEGREE = readValue(L_T_ADDRESS);
  M_DEGREE = readValue(M_ADDRESS);
  M_T_DEGREE = readValue(M_T_ADDRESS);
}

bool isCommandToServo(String command) {
  int haveNewLine = command[command.length() - 1] == '\n' ? 0 : 1;
  if (command.length() == 8 - haveNewLine) {
    //Serial.println("edit");
    saveValue(command[0], valueFromString(command[1], command[2], command[3]), valueFromString(command[4], command[5], command[6]));
    updateValue();
    return true;
  }
  else if (command.length() == 2 - haveNewLine) {
    moveTo(command[0]);
    return true;
  }
  return false;
}

void saveValue(char degree, int h, int v) {
  switch (degree) {
    case 'L':
      writeValue(L_ADDRESS, h);
      writeValue(L_T_ADDRESS, v);
      break;
    case 'M':
      writeValue(M_ADDRESS, h);
      writeValue(M_T_ADDRESS, v);
      break;
    case 'R':
      writeValue(R_ADDRESS, h);
      writeValue(R_T_ADDRESS, v);
      break;
  }
}

int readValue(int address) {
  return EEPROM.read(address);
}

void writeValue(int address, int val) {
  EEPROM.write(address, val);
}

void init_serve(int pin_horizontal_Servo, int pin_vertical_Servo) {
  horizontal_Servo.attach(pin_horizontal_Servo);
  vertical_Servo.attach(pin_vertical_Servo);
  horizontal_Servo.write(90);
  vertical_Servo.write(45);
  updateValue();
  Serial.begin(115200);
  /*Serial.println("Hello from arduino");
    for (int i = 0; i < 6; i++) {
    Serial.print(i);
    Serial.print(" ");
    Serial.println(readValue(i));
    }*/
}
