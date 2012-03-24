/****
 * RPM Sensor
 * This program uses a reed switch or hall sensor to measure the RPM
 * of a wheel (or any rotating object).
 ****/

#include "HallSensor.h"
#include <Arduino.h>

HallSensor::HallSensor(int _sensorPort, int _ledPort) {
  sensorPort = _sensorPort;
  ledPort = _ledPort;

  pinMode(sensorPort, INPUT);
  pinMode(ledPort, OUTPUT);

  lastRevTime = 0;
  rpm = 0;
}

void HallSensor::addRevolution() {
  // toggle output led
  digitalWrite(ledPort, !digitalRead(ledPort));

  // Calculate the RPM
  unsigned long currRevTime = micros();
  if (lastRevTime > 0) {
    double diffTime = currRevTime - lastRevTime;
    // The instantaneous RPM is the time to make one revolution:
    // RPM = 60 secs/min * 1000 ms/sec * 1000 us/ms * 1 rev / us
    int newRpm = (int)(60.0 * 1000.0 * 1000.0 / diffTime);

    // Avoid overflow from spurious electrical values.
    if (newRpm > 0)
        rpm = newRpm;
  }
  lastRevTime = currRevTime;
}

void HallSensor::noRevolution() {
  if (lastRevTime > 0) {
    double diffTime = micros() - lastRevTime;
    if (diffTime > (1000.0 * 1000.0)) {
      // No revolution in over a second, so assume the RPM is zero.
      rpm = 0;
      lastRevTime = 0;
    }
  }
}

int HallSensor::getRPM() {
  return rpm;
}
