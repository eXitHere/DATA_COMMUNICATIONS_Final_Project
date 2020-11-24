#include "CRC8.h"

CRC8 crc8;
uint8_t checksum;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  crc8.begin();

}

void loop() {


  // original message 
  String str = "oBA0Da";

  // generate the checksum
  int str_len = str.length() + 1; // calculate length of message (with one extra character for the null terminator)
  unsigned char char_array[str_len]; // prepare a character array (the buffer)
  str.toCharArray(char_array, str_len); // copy it over
  checksum = crc8.get_crc8(char_array, str_len);

  //combine message and checksum with separating character
  String checksum_str = String(checksum);

  String new_str = str + String(char(checksum));
  int new_str_len = new_str.length() + 1; // calculate length of message (with one extra character for the null terminator)
  unsigned char new_char_array[new_str_len]; // prepare a character array (the buffer)
  new_str.toCharArray(new_char_array, new_str_len); // copy it over
  uint8_t new_checksum = crc8.get_crc8(new_char_array, new_str_len);
  String new_checksum_str = String(new_checksum);

  Serial.println("Original message:");
  Serial.println(str);
  Serial.println("CRC8 checksum:" + String(checksum));
  Serial.println(checksum_str);
  Serial.println("CRCed:");
  Serial.println(new_str);
  Serial.println("CRC8 checksum:" + String(new_checksum));
  Serial.println(new_checksum_str);
  delay(1000);
}
