/****
 * Hall Sensor
 * This program uses a reed switch or hall sensor to measure the RPM
 * of a wheel (or any rotating object).
 ****/
class HallSensor {
private:
    int sensorPort;
    int ledPort;

    int currRpm;
    unsigned long lastRevTime;

public:
    HallSensor(int, int);
    void addRevolution();
    void noRevolution();
    int getRPM();
};
