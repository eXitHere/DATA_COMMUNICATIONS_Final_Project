#include <Arduino.h>
#include <Wire.h>
#include <TEA5767.h>

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
    TEA5767 radio;

    int8_t isPeek(uint16_t);
};
