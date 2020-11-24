#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); //pin2 Rx, pin3 Tx

void setup(void)
{
  Serial.begin(9600);
  mySerial.begin(9600);
}

void loop()
{
  if(Serial.available()) {
    mySerial.write(Serial.read());
  }
  if(mySerial.available()) {
    Serial.print(mySerial.read());
  }
}
