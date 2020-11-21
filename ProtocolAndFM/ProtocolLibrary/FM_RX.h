#include <Arduino.h>
#include <Wire.h>
#include <TEA5767Radio.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

class FM_RX
{
  public:
    FM_RX(float freq);
    int receiveFM();

  private:
    TEA5767Radio radio = TEA5767Radio();

    int8_t isPeek(uint16_t);
};
