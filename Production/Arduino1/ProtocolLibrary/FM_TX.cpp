#include "FM_TX.h"

FM_TX::FM_TX()
{
  Wire.begin();
  dac.begin(0x64);
  for (int i = 0; i < NUM_FREQ; i++)
  {
    freq[i] = ((i + 1) * 5) * FREQ_DIFF;
    freqDelay[i] = ( 1000000 / freq[i] + ( (2 - i) * 200 )) / NUM_SAMPLE ;
    Serial.print(freq[i]);
    Serial.print(" ");
    Serial.println(freqDelay[i]);
  }

  for (int i = 0; i < NUM_SAMPLE; i++)
  {
    S[i] = sin(DEG_TO_RAD * 360.0 / NUM_SAMPLE * i);
    S_DAC[i] = S[i] * 2047.5 + 2047.5;
    Serial.print(S[i]);
    Serial.print(" ");
    Serial.println(S_DAC[i]);
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
  delay(30);
}

void FM_TX::transmit(char in)
{
  int input[8];
  for( int i = 0; i < 8; i++) 
  {
    input[i] = (in >> i) & B0001;
  }

  for (int k = 0; k < 8; k++)
  {
    for (int cycle = freq[input[k]] / FREQ_DIFF; cycle > 0; cycle--)
    {
      for (int sample = NUM_SAMPLE - 1; sample >= 0; sample--)
      {
        setVoltage(S_DAC[sample]);
        delayMicroseconds(freqDelay[input[k]]);
      }
    }
    delayMicroseconds((1- input[k]) * 4000 + 1250);
  }
}

void FM_TX::sendFM_noDelay(char data)
{
  transmit(data);
  setVoltage(2047);
  //delay(30);
}
