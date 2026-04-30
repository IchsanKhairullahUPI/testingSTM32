#include <Arduino.h>

struct Motor {
  uint8_t pulPin;
  uint8_t dirPin;
  long currentPosition;
  long targetPosition;
  unsigned long lastEdgeMicros;
  unsigned long halfPeriodMicros;  // microseconds per half-pulse
  bool pulseState;
};

const int NUM_MOTORS = 1;
Motor motors[NUM_MOTORS] = {
  {PB0, PB1, 0, 0, 0, 200, false}  // J5: PUL=PB0, DIR=PB1, 200us half-period
};

void serviceMotor(Motor &m) {
  if (m.currentPosition == m.targetPosition) return;

  unsigned long now = micros();
  if (now - m.lastEdgeMicros < m.halfPeriodMicros) return;

  // Time to toggle
  bool dirHigh = (m.targetPosition > m.currentPosition);
  digitalWrite(m.dirPin, dirHigh ? LOW : HIGH);

  m.pulseState = !m.pulseState;
  digitalWrite(m.pulPin, m.pulseState ? HIGH : LOW);
  m.lastEdgeMicros = now;

  // Count a step on the falling edge (HIGH→LOW transition)
  if (!m.pulseState) {
    m.currentPosition += dirHigh ? 1 : -1;
  }
}

void processCommand(String line) {
  line.trim();

  if (line == "status") {
    for (int i = 0; i < NUM_MOTORS; i++) {
      Serial.print("M");
      Serial.print(i);
      Serial.print(" pos=");
      Serial.print(motors[i].currentPosition);
      Serial.print(" target=");
      Serial.println(motors[i].targetPosition);
    }
  }
  else if (line == "home") {
    for (int i = 0; i < NUM_MOTORS; i++) {
      motors[i].currentPosition = 0;
      motors[i].targetPosition = 0;
    }
    Serial.println("HOMED");
  }
  else {
    int colon = line.indexOf(':');
    if (colon > 0) {
      int motorIdx = line.substring(0, colon).toInt();
      long steps = line.substring(colon + 1).toInt();
      if (motorIdx >= 0 && motorIdx < NUM_MOTORS) {
        motors[motorIdx].targetPosition += steps;
        Serial.print("M");
        Serial.print(motorIdx);
        Serial.print(" target now ");
        Serial.println(motors[motorIdx].targetPosition);
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < NUM_MOTORS; i++) {
    pinMode(motors[i].pulPin, OUTPUT);
    pinMode(motors[i].dirPin, OUTPUT);
    digitalWrite(motors[i].pulPin, LOW);
    digitalWrite(motors[i].dirPin, LOW);
  }
  while (!Serial) { delay(10); }
  Serial.println("READY");
}

void loop() {
  static String inputBuffer = "";
  static unsigned long lastByteMillis = 0;
  const unsigned long LINE_TIMEOUT_MS = 100;

  while (Serial.available()) {
    char c = Serial.read();
    lastByteMillis = millis();
    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) {
        processCommand(inputBuffer);
        inputBuffer = "";
      }
    } else {
      inputBuffer += c;
    }
  }

  if (inputBuffer.length() > 0 && (millis() - lastByteMillis > LINE_TIMEOUT_MS)) {
    processCommand(inputBuffer);
    inputBuffer = "";
  }

  for (int i = 0; i < NUM_MOTORS; i++) {
    serviceMotor(motors[i]);
  }
}