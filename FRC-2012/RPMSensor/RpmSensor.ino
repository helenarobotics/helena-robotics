/****
RPM Sensor
This program uses a reed switch or hall sensor to measure the RPM
of a wheel (or any rotating object) and sends the data over an I2C
Connection. Arduio acts as a slave on the I2C
****/

#include <Wire.h>

class HallSensor {
    int interrupt;
    int port;
    int output_port;
    double last_time;
    int count;
    int rpm;
    void updateRPM();
  public:
    HallSensor(int, int, int);
    void countRevolution();
    int getRPM();
    void sendRPM();
    void printRPM();
};

HallSensor::HallSensor(int tmp_port, int tmp_interrupt, int tmp_output){
  interrupt = tmp_interrupt;
  port = tmp_port;
  output_port = tmp_output;
  
  count = 0;
  rpm = 0;
  last_time = micros();
  
  pinMode(port, INPUT);
  pinMode(output_port, OUTPUT);
}

void HallSensor::updateRPM(){
  double diff_time = micros() - last_time;
  
  //check if timer has overflowen
  if(diff_time <= 0){
    return;
  }
  
  // rpm = rotations / time in micro-seconds * 1000000 ms/s * 60 sec
  int new_rpm = (int)( (double)count / diff_time * 60000000);
  
  // Update the rpm using a bias so to smooth it out
  /// learning rate = .8
  rpm = new_rpm * .80 + rpm * .20;
  
  //reset count
  count = 0;
  last_time = micros();
  printRPM();
}

void HallSensor::countRevolution(){
  count++;
  if (count > 5){
    updateRPM();
  }
  
  //toggle output led
  if (digitalRead(output_port) == LOW)
    digitalWrite(output_port, HIGH);
  else
    digitalWrite(output_port, LOW);
    

}

int HallSensor::getRPM(){
  return rpm;
}

void HallSensor::sendRPM(){
  byte data [2];
  data[0] = rpm & 0x0F;
  data[1] = rpm >> 4;
  Wire.write(data, 2);
}

void HallSensor::printRPM(){
  Serial.print("RPM ");
  Serial.print(port);
  Serial.print(" value: ");
  Serial.print(rpm);
  Serial.print("\n");
}

const int I2C_ADDRESS = 0xC4;
const int MAX_COUNT = 5;

int sensor_request;

HallSensor hs1(2,0,4);
HallSensor hs2(3,0,5);

void setup(){
  Serial.begin(9600);
  Serial.print("Starting RPM Sensor...\n");

  sensor_request = 1;
  
  // Setup I2C
  Wire.begin(I2C_ADDRESS);
  Wire.onRequest(wireSend);
  Wire.onReceive(wireReceive);
  
  // Connect HallSensors to interrupts
  attachInterrupt(0, updateHs1, RISING);
  attachInterrupt(1, updateHs2, RISING);
}

void updateHs1(){
  hs1.countRevolution();
}

void updateHs2(){
  hs2.countRevolution();
}

void loop(){
  
  
  delay(10);
}

void wireReceive(int d
){
  while(Wire.available()){
    switch(Wire.read()){
      case 1:
        sensor_request = 1;
        break;
      case 2:
        sensor_request = 2;
        break;
    }
  }
}

void wireSend(){
  switch(sensor_request){
     case 1:
       hs1.sendRPM();
       break;
     case 2:
       hs2.sendRPM();
       break;
  }
}

