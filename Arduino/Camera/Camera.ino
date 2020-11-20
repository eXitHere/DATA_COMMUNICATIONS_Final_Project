#include "CameraController.h"

/*
 * StringPgm(PSTR("RDY"));
 * 
 */

void setup() {
  initCamera(0x80 | 3);
}


void loop() {
  capture(120, 120);
}
