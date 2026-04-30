#include <Arduino.h>
#include "motor.h"
#include "config.h"

void processCommand(String line) {
  line.trim();

  if (line == "status") {
    printStatus();
    return;
  }

  if (line == "home") {
    homeAllMotors();
    Serial.println("HOMED");
    return;
  }

  int colon = line.indexOf(':');
  if (colon > 0) {
    int motorIdx = line.substring(0, colon).toInt();
    long steps = line.substring(colon + 1).toInt();
    if (motorIdx >= 0 && motorIdx < NUM_MOTORS) {
      moveMotor(motorIdx, steps);
      Serial.print("M");
      Serial.print(motorIdx);
      Serial.print(" target now ");
      Serial.println(motors[motorIdx].targetPosition);
    }
  }
}

void readSerial() {
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
}

void setup() {
  Serial.begin(115200);
  initMotors();
  while (!Serial) { delay(10); }
  Serial.println("READY");
}

void loop() {
  readSerial();
  serviceAllMotors();
}