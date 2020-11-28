#ifndef MY_LIBRARY_H
#define MY_LIBRARY_H
#include <Servo.h>
#include <EEPROM.h>
#include <Arduino.h>
void init_serve(int pin_horizontal_Servo, int pin_vertical_Servo);
int readValue(int address);
void writeValue(int address, int val);
void saveValue(char degree, int h, int t);
bool isCommandToServo(String command);
int valueFromString(char x0, char x1, char x2);
void moveTo(char degree);
void writeServoTo(int ser, int newPos);
String getSettingAll();
#endif
