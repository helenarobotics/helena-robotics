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

  count = 0;
  rpm = 0;
}

void HallSensor::addRevolution() {
  // toggle output led
  digitalWrite(ledPort, !digitalRead(ledPort));
  count++;
}

void HallSensor::calculateRPM(double diffTime) {
  // XXX - Handle overflow?
  if (diffTime > 0) {
    // rpm = (rotations / utime) * 1000 * 1000 * 60 [usecs in a minute]
    int new_rpm = (int)((double)count / diffTime * 1000000 * 60);

    // Update the rpm using a bias to smooth it out (learning rate = 0.01)
    rpm = rpm * 0.90 + new_rpm * 0.1;
  }
  // Disable interrupts while we change the count so that we don't end
  // up with corrupted count values.
  cli();
  count = 0;
  sei();
}

int HallSensor::getRPM() {
  return rpm;
}
