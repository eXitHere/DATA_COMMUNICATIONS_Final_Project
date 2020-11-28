#include "CameraController.h"

/*
 * StringPgm(PSTR("*RDY*"));
 * 
 */

void setup() {
  initCamera(0x80 | 3);
  //initCamera(12);
}


void loop() {
  capture(160, 120);
}
