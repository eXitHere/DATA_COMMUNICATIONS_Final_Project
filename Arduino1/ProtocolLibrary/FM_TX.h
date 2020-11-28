#include <Adafruit_MCP4725.h>

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>

#define NUM_SAMPLE 4
#define NUM_FREQ 2
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
  uint16_t S[NUM_SAMPLE];
  uint16_t S_DAC[NUM_SAMPLE];
  uint16_t freq[NUM_FREQ];
  uint16_t freqDelay[NUM_FREQ];

  Adafruit_MCP4725 dac;

  void setVoltage(uint16_t vol);
  void transmit(char in);
};
