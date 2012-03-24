/****
 * Hall Sensor
 * This program uses a reed switch or hall sensor to measure the RPM
 * of a wheel (or any rotating object).
 ****/
class HallSensor {
private:
  int sensorPort;
  int ledPort;

  // Code to calculate and keep track of RPM
  unsigned long lastRevTime;
  int rpm;

public:
  HallSensor(int, int);
  void addRevolution();
  void noRevolution();
  int getRPM();
};
