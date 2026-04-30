#pragma once
#include <Arduino.h>

struct Motor {
  uint8_t pulPin;
  uint8_t dirPin;
  long currentPosition;
  long targetPosition;
  unsigned long lastEdgeMicros;
  unsigned long halfPeriodMicros;
  bool pulseState;
};

void initMotors();
void serviceMotor(Motor &m);
void serviceAllMotors();
void homeAllMotors();
void moveMotor(int motorIdx, long stepsDelta);
void printStatus();