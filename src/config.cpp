#include "config.h"

const int NUM_MOTORS = 2; // ubah angka sesuai jumlah stepper

Motor motors[] = {
  // pulPin, dirPin, currentPos, targetPos, lastEdgeMicros, halfPeriodMicros, pulseState
  //{PA0, PA1, 0, 0, 0, 200, false},   // J1
  //{PA2, PA3, 0, 0, 0, 200, false},   // J2
  //{PA4, PA5, 0, 0, 0, 200, false},   // J3
  {PA0, PA1, 0, 0, 0, 200, false},   // J4
  {PB0, PB1, 0, 0, 0, 200, false},  // J5
  //{PB11, PB12, 0, 0, 0, 200, false}, // J6
};