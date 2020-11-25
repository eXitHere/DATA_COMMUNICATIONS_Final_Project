#include "CRC8.h"

CRC8 crc8;
uint8_t checksum;
void testCrc();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  crc8.begin();

}

uint8_t getUintCrc(String str)
{
  int str_len = str.length() + 1; // calculate length of message (with one extra character for the null terminator)
  unsigned char char_array[str_len]; // prepare a character array (the buffer)
  str.toCharArray(char_array, str_len); // copy it over
  checksum = crc8.get_crc8(char_array, str_len);

  return checksum;
}



void loop() 
{
  String x = "nax5";//string with crc added
  
  String init = x.substring(0, x.length()-1);
  uint8_t checksum = getUintCrc(init);
  Serial.println(init +", "+String(checksum) + " in char: " + String(char(checksum)));
  Serial.println("---------------------");
  
  String withCrc = init + String(char(checksum));
  withCrc = withCrc.substring(0,3);
  Serial.println(withCrc + " check: " + String(getUintCrc(withCrc)));
  Serial.println("---------------------");
  
  String y ="nax";
  checksum = getUintCrc(y);
  Serial.println(y +", "+String(checksum) + " in char: " + String(char(checksum)));
  Serial.println("---------------------------------------------------------");
 delay(2000); 
}


void testCrc()
{
  // original message 
  String str = "nax", CRCED = "nax5";
  
  // generate the checksum
  int str_len = str.length() + 1; // calculate length of message (with one extra character for the null terminator)
  unsigned char char_array[str_len]; // prepare a character array (the buffer)
  str.toCharArray(char_array, str_len); // copy it over
  checksum = crc8.get_crc8(char_array, str_len);
  uint8_t checksum2 = getUintCrc(str);

  //combine message and checksum with separating character
  String checksum_str = String(checksum);
  String checksum2_str = String(checksum2);

  Serial.println("Original message:");
  Serial.println(str);
  Serial.println("CRC8 checksum:" + String(checksum));
  Serial.println(checksum_str + " " + checksum2_str+"\nSolve");

  str += char(checksum);
  
  str_len = str.length() + 1; // calculate length of message (with one extra character for the null terminator)
  str.toCharArray(char_array, str_len); // copy it over
  checksum = crc8.get_crc8(char_array, str_len);
  checksum2 = getUintCrc(str);
  checksum_str = String(checksum);
  checksum2_str = String(checksum2);

  Serial.println("CRCed Message:");
  Serial.println(CRCED);
  Serial.println("CRC8 checksum:" + String(checksum));
  Serial.println(checksum_str + " " + checksum2_str);
  delay(1000);
}
