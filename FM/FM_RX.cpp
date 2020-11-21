#include "FM_RX.h"

FM_RX::FM_RX(float freq)
{
  // put your setup code here, to run once:
  sbi(ADCSRA, ADPS2);
  cbi(ADCSRA, ADPS1);
  cbi(ADCSRA, ADPS0);

  Wire.begin();
  radio.setFrequency(freq);

}

int FM_RX::receiveFM()
{
  int prev = 0;
  int count = 0;

  uint16_t data = 0;
  uint16_t bit_check = 0;

  bool check_baud = false;

  uint32_t baud_begin = micros();

  while (micros() - baud_begin < 40000)
  {
    int tmp = analogRead(A2);

      if ( tmp == 1 and prev == 0 and !check_amp ) // check amplitude
      {
        check_amp = true; // is first max amplitude in that baud
        if ( !check_baud )
        {
          baud_begin = micros();
          bit_check++;
        }
      }
    
      if (tmp == 1 and check_baud) {
        if (micros() - baud_begin > 19400 ) // full baud
        {
          int dt = ((int(floor((count - 2) / 3.0))) & 3);
          uint16_t last = dt << (bit_check * 2);  // shift data
          data |= last;                                                               // add two new bits in data
          baud_check++;
          if (baud_check == 4) // 8 bits
          {
            Serial.print((char)data);
            if ( data != 0)
              return data;
            data = 0;
            baud_check = 0;
            bit_check = -1;
          }
          baud_begin = micros();
          check_baud = false;
          count = 0;
        }
      }
    
      if (tmp == 1 and prev == 0 and check_amp) {
        count++;
        //Serial.println(tmp);
        check_baud = true;
        check_amp = false;
      }
      prev = tmp;
    }
  }
  return -1;
}

int8_t FM_RX::isPeek(uint16_t val)
{
  if (val <= 600)
    return 1;
  else
    return 0;
}
