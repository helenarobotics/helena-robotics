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

  // Keep track of the instantaneous RPM calculations to aid in getting
  // a better/smoother RPM value.
  for (int i = 0; i < RPM_HISTORY; i++)
    rpmHistory[i] = 0;
  historyIndex = 0;

  // Not really true, but good enough for now.
  lastRevTime = 0;
}

void HallSensor::addRevolution() {
  // toggle output led
  digitalWrite(ledPort, !digitalRead(ledPort));

  // Calculate the RPM
  unsigned long currRevTime = micros();
  if (lastRevTime > 0) {
    double diffTime = currRevTime - lastRevTime;

    // RPM = 60 secs/min * 1000 ms/sec * 1000 us/ms * 1 rev / us
    // The instantaneous RPM is the time to make one revolution:
    // * 60 seconds/min * 1000 ms/sec * 1000 us/ms * 1 (rev) / diffTime (in us)
    if (diffTime > 0) {
      short newRpm = (short)(60.0 * 1000.0 * 1000.0 / diffTime);

      // Keep track of the RPM history!
      rpmHistory[historyIndex] = newRpm;
      historyIndex++;
      if (historyIndex >= RPM_HISTORY)
        historyIndex = 0;
    }
  }
  lastRevTime = currRevTime;
}

// Move the calculation to zero if it's been more than 2 seconds since
// we last measured anything
void HallSensor::noRevolution() {
  if (lastRevTime > 0) {
    double diffTime = micros() - lastRevTime;
    if (diffTime > 2 * 1000 * 1000) {
      // Zero out all the calculations
      for (int i = 0; i < RPM_HISTORY; i++)
        rpmHistory[i] = 0;
      historyIndex = 0;
    }
  }
}

int HallSensor::getRPM() {
  // Return the average of all the calculations
  int total = 0;
  for (int i = 0; i < RPM_HISTORY; i++)
    total += rpmHistory[i];
    
  return total / RPM_HISTORY;
}
