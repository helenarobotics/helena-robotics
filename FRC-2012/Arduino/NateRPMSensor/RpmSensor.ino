/****
 * RPM Sensor
 * This program uses the HallSensor code (which measures the RPM of a
 * wheel or other rotating object).  The Arduino acts as a slave on the
 * I2C bus and sends the data to any master that requests the data.
 ****/

#include <Wire.h>
#include "HallSensor.h"

// Define to enable debugging
// #defined DEBUG 1

//
// The I2C Slave code
//

// I2C constants
const int I2C_ADDRESS = 0xCA;
const int SENSOR_REGISTER = 0x00;

// Sensor information
int requestedSensor;
HallSensor *hs1, *hs2;

// We send at most 6 bytes of data
byte sendData[6];

// Keep track of time
double prevTime;

void setup() {
#if defined(DEBUG)
  Serial.begin(115200);
  Serial.println("Starting RPM Sensor...");
#endif

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

  // Remember the time
  prevTime = micros();
}

// These should be HallSensor methods, but attachInterrupt doesn't
// allow such things done easily.
void hs1Interrupt() {
  hs1->addRevolution();
}

void hs2Interrupt() {
  hs2->addRevolution();
}

int loopCounter = 0;
void loop() {
  // Calculate how much time has elapsed, and pass it to
  // the Hall Sensor to calculate RPM.
  double currTime = micros();
  hs1->calculateRPM(currTime - prevTime);
  hs1->calculateRPM(currTime - prevTime);
  double diffTime = currTime - prevTime;
  prevTime = currTime;

#if defined(DEBUG)
  // Roughly once/sec print out the calculated RPM
  if ((++loopCounter % 10) == 0) {
    Serial.print("DT");
    Serial.println(diffTime);
    Serial.print("RPM 1 value: ");
    Serial.println(hs1->getRPM());
    Serial.print("RPM 2 value: ");
    Serial.println(hs2->getRPM());
  }
#endif
  delay(25);
}

// Determine which sensor is being requested.
void wireReceive(int d) {
  if (d > 0) {
    // Grab the 'register'!
    byte reg = Wire.read();

    for (int i = 1; i < d; i++) {
      if (reg == SENSOR_REGISTER) {
        // Request for SENSOR data
        switch (i) {
        case 1:
          requestedSensor = Wire.read();
          break;

        default:
          // The sensor register only needs a single value, so if more
          // request data was provided throw it away.
          Wire.read();
          break;
        }
      } else {
        // Unknown register, just read and throw away!
        Wire.read();
      }
    }
  }
}

// Send the response to the master
void wireSend() {
  int sendLen = 0;
  switch (requestedSensor) {
  case 0:
    sendLen += storeLEShort(hs1->getRPM(), sendData, sendLen);
    sendLen += storeLEShort(hs2->getRPM(), sendData, sendLen);
    break;

  case 1:
    sendLen += storeLEShort(hs1->getRPM(), sendData, sendLen);
    break;

  case 2:
    sendLen += storeLEShort(hs2->getRPM(), sendData, sendLen);
    break;

  default:
    // Nothing to send
    break;
  }
  Wire.write(sendData, sendLen);
}

// Store the data as two bytes in little endian format
int storeLEShort(int data, byte *buff, int offset) {
  buff[offset] = (data & 0xFF);
  buff[offset + 1] = ((data >> 8) & 0xFF);
  return 2;
}
