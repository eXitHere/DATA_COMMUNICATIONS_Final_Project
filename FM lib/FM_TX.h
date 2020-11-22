#include <Arduino.h>
#include <Wire.h>
#include<Adafruit_MCP4725.h>
#include<Adafruit_ADS1015.h>

#define NUM_SAMPLE 8
#define NUM_FREQ 4
#define FREQ_DIFF 100
#define DEF_FREQ 1700

class FM_TX
{
  public:
    FM_TX();
    void sendFM(char data);

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