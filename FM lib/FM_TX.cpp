#include "FM_TX.h"

FM_TX::FM_TX()
{
  Wire.begin();
  dac.begin(0x64);
  for (int i = 0; i < NUM_FREQ; i++)
  {
    freq[i] = (i * 2 + 5) * FREQ_DIFF;
    freqDelay[i] = ((1000000 / freq[i] - 1000000 / DEF_FREQ) / NUM_SAMPLE);
  }

  for (int i = 0; i < NUM_SAMPLE; i++)
  {
    S[i] = sin(DEG_TO_RAD * 360.0 / NUM_SAMPLE * i);
    S_DAC[i] = S[i] * 2047.5 + 2047.5;
  }

  setVoltage(2047);
}

void FM_TX::setVoltage(uint16_t vol)
{
  dac.setVoltage(vol, false);
}

void FM_TX::sendFM(char in)
{
  transmit(in);
  setVoltage(2047);
  delay(500);
}

void FM_TX::transmit(char in)
{
  int input[4];
  for( int i = 0; i < NUM_FREQ; i++)
    input[i] = (in >> (i * 2)) & B0011;

  for (int k = 0; k <= 3; k++)
  {
    for (int cycle = freq[input[k]] / FREQ_DIFF; cycle > 0; cycle--)
    {
      for (int sample = 0; sample < NUM_SAMPLE; sample++)
      {
        setVoltage(S_DAC[sample]);
        delayMicroseconds(freqDelay[input[k]]);
      }
    }
  }
}
