// What direction is the servo moving
typedef enum { SWEEP_LEFT, SWEEP_RIGHT } SweepDirection;

// HiTechnic IR sensor constants
const int IR_NOMATCH_SECTION = 0;
const int IR_RIGHT_SECTION = 4;
const int IR_CENTER_SECTION = 5;
const int IR_LEFT_SECTION = 6;

TServoIndex irServo;
tSensors irSensor;

void initializeIRServo(TServoIndex _irServo, tSensors _irSensor,
                       SweepDirection direction) {
    // Keep track of the servo and sensor
    irServo = _irServo;
    irSensor = _irSensor;

    // How many positions for every update (~20ms)
    servoChangeRate[irServo] = 10;

    // Initialize the servo postion.
    servoStartPos = 0;
    if (direction == SWEEP_LEFT)
        servoPos = 255;
    servo[irServo] = servoStartPos;

    // Give the servo time to get to the start position.  Worst case
    // scenario is the servo is at the opposite extreme (255 pos), and
    // because we move ~10 positions every 20ms (see above), moving 260
    // positions is 520ms, so go with 550ms to be safe.
    wait1Msec(550ms);
}

int findBeaconPosition() {

    // Determine which way to sweep the servo based on the start position
    int servoPos = servo[irServo];
    int servoChange = 1;
    int servoEndPos = 255;
    if (servoPos > 0) {
        servoChange = -1;
        servoEndPos = 0;
    }

    // Go find the IR beacon and return the servo position when found.
    bool found = false;
    int prevIrSection = SensorValue[irSensor];
    do {
        // Did we find the sensor?
        int currIrSection = SensorValue[irss.irSensor];
        // Determine if we found a center transition spot
        if (currIrSection == IR_CENTER_SECTION &&
            abs(currIrSection - prevIrSection) == 1) {
            found = true;
        } else if (servoPos == servoEndPos) {
            // Not really found but we've done all we can.  Set the
            // servoPos to an invalid value to let the caller know we
            // didn't find it.
            found = true;
            servoPos = -1;
        } else {
            // Move the servo to the next position and keep track of the
            // old sensor reading.
            prevIrSection = currIrSection;
            servoPos += servoChange;
            servo[irServo] = servoPos;

            // Delay to allow the servo to move (twice as long as it
            // should take for 1pos).  At most, this will require us to
            // wait ~1 second for a sweep of the entire servo range.
            wait1Msec(4);
        }
    } while (!found);

    // We've found it, or we've swept the entire servo value.  Return
    // the servo back to the 0 position and return the found position to
    // the caller.
    servo[irServo] = 0;

    return servoPos;
}
