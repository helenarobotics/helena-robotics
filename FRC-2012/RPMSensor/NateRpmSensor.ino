/****
 * RPM Sensor
 * This program uses a reed switch or hall sensor to measure the RPM
 * of a wheel (or any rotating object) and sends the data over an I2C
 * Connection. The Arduino acts as a slave on the I2C bus.
 ****/

#include <Wire.h>

const int MAX_COUNT = 5;
class HallSensor {
  int sensorPort;
  int ledPort;
  double last_time;
  int count;
  int rpm;
  void updateRPM();
public:
  HallSensor(int, int);
  void countRevolution();
  int getRPM();
};

HallSensor::HallSensor(int _sensorPort, int _ledPort) {
  sensorPort = _sensorPort;
  ledPort = _ledPort;

  pinMode(sensorPort, INPUT);
  pinMode(ledPort, OUTPUT);

  count = 0;
  rpm = 0;
  last_time = micros();
}

void HallSensor::countRevolution() {
  // toggle output led
  digitalWrite(ledPort, !digitalRead(ledPort));

  if (++count > MAX_COUNT) {
    // make sure timer hasn't overflown
    double diff_time = micros() - last_time;
    if (diff_time > 0) {
      // rpm = rotations / time in micro-seconds * 1000000 ms/s * 60 sec
      int new_rpm = (int)((double)count / diff_time * 1000000 * 60);

      // Update the rpm using a bias to smooth it out (learning rate = .8)
      rpm = new_rpm * .8 + rpm * .2;
    }
    count = 0;
    last_time = micros();
  }
}

int HallSensor::getRPM() {
  return rpm;
}

const int I2C_ADDRESS = 0xC4;
const String EOL = "\r\n";

int requestedSensor;
HallSensor *hs1, *hs2;

void setup() {
  Serial.begin(9600);
  Serial.print("Starting RPM Sensor..." + EOL);

  // By default, assume the first sensor
  requestedSensor = 1;

  // Setup I2C
  Wire.begin(I2C_ADDRESS >> 1);

  // onReceive is called to provide the parameters for the request, and
  // onRequest is called when to send out the response to the master.
  Wire.onReceive(wireReceive);
  Wire.onRequest(wireSend);

  hs1 = new HallSensor(2, 4);
  hs2 = new HallSensor(3, 5);

  // Connect HallSensor to interrupts
  attachInterrupt(0, hs1Interrupt, RISING);
  attachInterrupt(1, hs2Interrupt, RISING);
}

// These should be HallSensor methods, but attachInterrupt doesn't
// allow such things done easily.
void hs1Interrupt() {
     hs1->countRevolution();
}

void hs2Interrupt() {
     hs2->countRevolution();
}

// Roughly once/sec print out the calculated RPM
int counter = 0;
void loop() {
  counter++;
  if (counter % 10 == 0) {
    Serial.print("RPM 1 value:" + hs1->getRPM() + EOL);
    Serial.print("RPM 2 value:" + hs2->getRPM() + EOL);
  }
  delay(100);
}

// Determine which sensor is being requested.
void wireReceive(int d) {
  while (Wire.available()) {
    switch (Wire.read()) {
    case 1:
      requestedSensor = 1;
      break;

    case 2:
      requestedSensor = 2;
      break;
    }
  }
}

// Send the response to the master
void wireSend() {
  int rpm = 0;
  switch (requestedSensor) {
  case 1:
    rpm = hs1->getRPM();
    break;
  case 2:
    rpm = hs2->getRPM();
    break;
  }
  byte data[2];
  data[0] = rpm & 0xFF;
  data[1] = (rpm >> 8) & 0xFF;
  Wire.write(data, 2);
}

