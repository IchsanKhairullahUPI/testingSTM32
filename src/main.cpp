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

  if (line.startsWith("pulse:")) {
    int colon1 = line.indexOf(':');
    int colon2 = line.indexOf(':', colon1 + 1);
    if (colon2 > 0) {
      int motorIdx = line.substring(colon1+1, colon2).toInt();
      long count = line.substring(colon2+1).toInt();
      if (motorIdx >= 0 && motorIdx < NUM_MOTORS) {
        Motor &m = motors[motorIdx];
        Serial.print("Pulsing M");
        Serial.print(motorIdx);
        Serial.print(" ");
        Serial.print(count);
        Serial.println(" times (blocking)...");
        digitalWrite(m.dirPin, LOW);
        delayMicroseconds(50);
        for (long i = 0; i < count; i++) {
          digitalWrite(m.pulPin, HIGH);
          delayMicroseconds(200);
          digitalWrite(m.pulPin, LOW);
          delayMicroseconds(200);
        }
        Serial.println("DONE");
      }
    }
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