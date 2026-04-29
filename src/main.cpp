#include <Arduino.h>

#define DIR_PIN PB1
#define PUL_PIN PB0

const long pulsesPerRev = 6400;
int pulseDelay = 200;

void pulseSteps(long steps, bool clockwise) {
  digitalWrite(DIR_PIN, clockwise ? LOW : HIGH);
  delayMicroseconds(50);  // give DIR time to settle before pulses
  for (long i = 0; i < steps; i++) {
    digitalWrite(PUL_PIN, HIGH);
    delayMicroseconds(pulseDelay);
    digitalWrite(PUL_PIN, LOW);
    delayMicroseconds(pulseDelay);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(PUL_PIN, OUTPUT);
  digitalWrite(DIR_PIN, LOW);
  digitalWrite(PUL_PIN, LOW);
  while (!Serial) { delay(10); }
  Serial.println("READY");
}

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    long steps = line.toInt();
    if (steps != 0) {
      bool clockwise = (steps > 0);
      long absSteps = abs(steps);
      Serial.print("moving ");
      Serial.print(steps);
      Serial.println(" pulses...");
      pulseSteps(absSteps, clockwise);
      Serial.println("DONE");
    }
  }
}