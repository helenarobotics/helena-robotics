/****
 * I2C Sensor
 * This program increments a couple counters and responds to I2C
 * requests to fetch their values.  In this setup, the Arduino acts as a
 * slave on the I2C bus.
 ****/
#include <Wire.h>
#include <stdarg.h>

//
// The I2C Slave code
//

// I2C constants
const int I2C_ADDRESS = 0xC2;
const int COUNTER_REGISTER = 0x00;

// The counters
const int NUM_COUNTERS = 2;
int counter[NUM_COUNTERS];

// What counter value is being requested?
int requestedCounter = 0;

// We send at most 6 bytes of data
byte sendData[6];

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting I2C Sensor...");

  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));

  // Initialize the counter values to random numbers
  for (int i = 0; i < NUM_COUNTERS; i++) {
    counter[i] = random(500);
  }
  
  // By default, assume the first counter
  requestedCounter = 1;

  // Setup I2C
  Wire.begin(I2C_ADDRESS >> 1);

  // onReceive is called to provide the parameters for the request, and
  // onRequest is called when to send out the response to the master.
  Wire.onReceive(wireReceive);
  Wire.onRequest(wireRequest);
}

// Print buffer
char prtBuff[128];
int timeCounter = 0;
void loop() 
{
  for (int i = 0; i < NUM_COUNTERS; i++) {
     // Increment the counter by a random number between 1 and 3.
     counter[i] += random(2) + 1;
  }

  // Roughly once/5 secs print out the counters
  if (timeCounter++ % 50 == 0) {
    for (int i = 0; i < NUM_COUNTERS; i++) {
      sprintf(prtBuff, "Counter[%d] value: %d", i, counter[i]);
      Serial.println(prtBuff);
    }
    Serial.println("");
  }
  delay(100);
}

// Determine which sensor is being requested.
void wireReceive(int d)
{
  Serial.println("wireReceive");
  if (d > 0) {
    // Grab the 'register'
    byte reg = Wire.read();

    switch (reg) {
    case COUNTER_REGISTER:
      // The first (and only) parameter is the counter to send.
      if (Wire.available())
        requestedCounter = Wire.read();
      break;

    default:
      break;
    }

    // Ignore any remaining request data
    while (Wire.available() > 0)
      Wire.read();
  }
}

// Send the response to the master
void wireRequest()
{
  Serial.println("wireRequest");

  int sendLen = 0;
  if (requestedCounter >= 0 && requestedCounter < NUM_COUNTERS) {
    switch (requestedCounter) {
    case 0:
      // Send both counters!
      sendLen += storeLEShort(counter[0], sendData, sendLen);
      sendLen += storeLEShort(counter[1], sendData, sendLen);
      break;

    default:
      sendLen += storeLEShort(counter[requestedCounter - 1], sendData, sendLen);
      break;
    }
  }
  Wire.write(sendData, sendLen);
}

// Store the data as two bytes in little endian format
int storeLEShort(int data, byte *buff, int offset)
{
  buff[offset] = (data & 0xFF);
  buff[offset + 1] = ((data >> 8) & 0xFF);
  return 2;
}
