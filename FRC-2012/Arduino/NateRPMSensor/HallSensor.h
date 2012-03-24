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
  static const short RPM_HISTORY = 5;
  short rpmHistory[RPM_HISTORY];
  short historyIndex;
  unsigned long lastRevTime;

public:
  HallSensor(int, int);
  void addRevolution();
  int getRPM();
};
