#include "motor.h"
#include "config.h"

void initMotors() {
  for (int i = 0; i < NUM_MOTORS; i++) {
    pinMode(motors[i].pulPin, OUTPUT);
    pinMode(motors[i].dirPin, OUTPUT);
    digitalWrite(motors[i].pulPin, LOW);
    digitalWrite(motors[i].dirPin, LOW);
  }
}

void serviceMotor(Motor &m) {
  if (m.currentPosition == m.targetPosition) return;

  unsigned long now = micros();
  if (now - m.lastEdgeMicros < m.halfPeriodMicros) return;

  bool dirHigh = (m.targetPosition > m.currentPosition);
  digitalWrite(m.dirPin, dirHigh ? LOW : HIGH);

  m.pulseState = !m.pulseState;
  digitalWrite(m.pulPin, m.pulseState ? HIGH : LOW);
  m.lastEdgeMicros = now;

  if (!m.pulseState) {
    m.currentPosition += dirHigh ? 1 : -1;
  }
}

void serviceAllMotors() {
  for (int i = 0; i < NUM_MOTORS; i++) {
    serviceMotor(motors[i]);
  }
}

void homeAllMotors() {
  for (int i = 0; i < NUM_MOTORS; i++) {
    motors[i].currentPosition = 0;
    motors[i].targetPosition = 0;
  }
}

void moveMotor(int motorIdx, long stepsDelta) {
  if (motorIdx < 0 || motorIdx >= NUM_MOTORS) return;
  motors[motorIdx].targetPosition += stepsDelta;
}

void printStatus() {
  for (int i = 0; i < NUM_MOTORS; i++) {
    Serial.print("M");
    Serial.print(i);
    Serial.print(" pos=");
    Serial.print(motors[i].currentPosition);
    Serial.print(" target=");
    Serial.println(motors[i].targetPosition);
  }
}