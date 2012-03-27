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
  static const int RPM_HISTORY = 1;
  int historyIndex;
  int rpmHistory[RPM_HISTORY];
  unsigned long lastRevTime;

public:
  HallSensor(int, int);
  void addRevolution();
  void noRevolution();
  int getRPM();
};
