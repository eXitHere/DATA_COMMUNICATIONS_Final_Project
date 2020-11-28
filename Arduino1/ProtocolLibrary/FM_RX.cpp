#include "FM_RX.h"

FM_RX::FM_RX(float freq)
{
  // put your setup code here, to run once:
  sbi(ADCSRA, ADPS2);
  cbi(ADCSRA, ADPS1);
  cbi(ADCSRA, ADPS0);

  Wire.begin();
  radio = new TEA5767(freq);
  radio->init();
  radio->set_frequency(freq);

}

int FM_RX::receiveFM()
{
  int prev = 0;
  int count = 0;

  uint16_t data = 0;
  uint16_t bit_check = 0;

  bool check_baud = false;
  bool check_amp = false;

  uint32_t baud_begin = micros();

  while (micros() - baud_begin < 40000)
  {
    int tmp = isPeek(analogRead(A2));

    if ( tmp == 1 and prev == 0 and !check_amp ) // check amplitude
    {
      check_amp = true; // is first max amplitude in that baud
      if ( !check_baud )
      {
        baud_begin = micros();
      }
    }

    if (tmp == 0 and check_baud) {
      if (micros() - baud_begin > 9800 )
      {
        int dt = ((int(floor((count) / 5.0) - 1)) & 1);
        uint16_t last = dt << (bit_check);
        data |= last;

        bit_check++;

        //          Serial.print(count);
        //          Serial.print(" : ");
        //          Serial.print(dt);
        //          Serial.print(" , ");
        if (bit_check == 8) // 8 bits
        {
          //            Serial.print("\nAlphabets :\t");
          //            Serial.print((char)data);
          if ( data != 0)
            return data;
          //            Serial.println("\t" + String(char(data)) + "\n");
          //            all_data += String(char(data));
          data = 0;
          bit_check = 0;
        }
        check_baud = false;
        count = 0;
      }
    }

    if (tmp == 0 and prev == 1 and check_amp) {
      count++;
      //Serial.println(tmp);
      check_baud = true;
      check_amp = false;
    }
    prev = tmp;
  }
  return -1;
}

int8_t FM_RX::isPeek(uint16_t val)
{
  if (val <= 200)
  //if (val >= 430)//Using 600 with amplifier
    return 1;
  else
    return 0;
}


String FM_RX::receiveStringFM(int maxLength)//Return data string (Empty string if nothing arrive)
{
  int prev = 0;
  int count = 0;

  uint16_t data = 0;
  uint16_t bit_check = 0;

  bool check_baud = false;
  bool check_amp = false;

  uint32_t baud_begin = micros();

  String message = "";

  while (micros() - baud_begin < 40000)
  {
    int tmp = isPeek(analogRead(A2));

    if ( tmp == 1 and prev == 0 and !check_amp ) // check amplitude
    {
      check_amp = true; // is first max amplitude in that baud
      if ( !check_baud )
      {
        baud_begin = micros();
      }
    }

    if (tmp == 0 and check_baud) {
      if (micros() - baud_begin > 9800 )
      {

        int dt = ((int(floor((count) / 5.0)) - 1) & 1);
        uint16_t last = dt << (bit_check);
        data |= last;

        bit_check++;

        //  Serial.print(count);
        //          Serial.print(" : ");
        //          Serial.print(dt);
        //          Serial.print(" , ");
        if (bit_check == 8) // 8 bits
        {
          if (data != 0)
            message += char(data);
          if (message.length() == maxLength)
            break;
          data = 0;
          bit_check = 0;
        }
        check_baud = false;
        count = 0;
      }
    }

    if (tmp == 0 and prev == 1 and check_amp) {
      count++;
      //Serial.println(tmp);
      check_baud = true;
      check_amp = false;
    }
    prev = tmp;
  }
  return message;
}
