#ifndef MY_LIBRARY_H
#define MY_LIBRARY_H
#include<Arduino.h>
void error_led(void);
void twiStart(void);
void twiWriteByte(uint8_t DATA, uint8_t type);
void twiAddr(uint8_t addr, uint8_t typeTWI);
void wrReg(uint8_t reg, uint8_t dat);
static uint8_t twiRd(uint8_t nack);
uint8_t rdReg(uint8_t reg);
void wrSensorRegs8_8(const struct regval_list reglist[]);
void setColor(void);
void setRes(void);
void camInit(void);
void arduinoUnoInut(void);
void StringPgm(const char * str);
static void captureImg(uint16_t wg, uint16_t hg);
void initCamera(int reg);
void capture(int width_, int height_);
#endif
