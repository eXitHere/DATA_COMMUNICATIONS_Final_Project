#include <Adafruit_MCP4725.h>

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>

#define NUM_SAMPLE 4
#define NUM_FREQ 4
#define FREQ_DIFF 100
#define DEF_FREQ 2500

class FM_TX
{
public:
  FM_TX();
  void sendFM(char data);

  void sendFM_noDelay(char data);

private:
  int delay0;
  float S[NUM_SAMPLE];
  float S_DAC[NUM_SAMPLE];
  int freq[NUM_FREQ];
  int freqDelay[NUM_FREQ];

  Adafruit_MCP4725 dac;

  void setVoltage(uint16_t vol);
  void transmit(char in);
};
