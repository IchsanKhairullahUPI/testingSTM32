#include <Arduino.h>
#include <AccelStepper.h>

#define PUL_PIN PB0
#define DIR_PIN PB1

AccelStepper stepper(AccelStepper::DRIVER, PUL_PIN, DIR_PIN);

void setup() {
  Serial.begin(115200);
  pinMode(PUL_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  stepper.setMaxSpeed(2000);
  stepper.setAcceleration(1000);
  while (!Serial) { delay(10); }
  Serial.println("READY");
}

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();

    if (line == "status") {
      Serial.print("pos=");
      Serial.println(stepper.currentPosition());
      return;
    }

    if (line == "home") {
      stepper.setCurrentPosition(0);
      Serial.println("HOMED");
      return;
    }

    long steps = line.toInt();
    if (steps != 0) {
      Serial.print("moving ");
      Serial.print(steps);
      Serial.println(" steps...");
      stepper.move(steps);
      while (stepper.distanceToGo() != 0) stepper.run();
      Serial.println("DONE");
    }
  }
}