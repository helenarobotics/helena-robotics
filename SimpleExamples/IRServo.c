// Ignore warnings about unreferenced functions
#pragma systemFile

// The below allocations allow starting the same task multiple times with
// different data.

// The _LEN hack only works since we're not assigning the numbers
typedef enum { IR_LEFT, IR_RIGHT, IRSERVO_LEN } IRServoEnum;
IRServoEnum servoNum = IR_LEFT;

// What direction is the servo moving
typedef enum { SWEEP_LEFT, SWEEP_RIGHT } SweepDirection;

// Local configuration for the specific sensor/servo.
typedef struct {
    // Incoming information
    TServoIndex irServo;
    tSensors irSensor;
    int irServoCenterPos;
    int irSensorOffsetPos;
    SweepDirection initialSweepDirection;

    // Outgoing information
    bool acquired;
    float irHeading;
} IRServoStruct;

// Keep track of individual servo/senor pair's data
IRServoStruct irServoData[IRSERVO_LEN];

// Public forward declarations
IRServoEnum initializeIRServo(TServoIndex servo, int servoCenterPos,
                              tSensors sensor, int sensorOffsetPos);
bool irAcquired(IRServoEnum selector);
void forceReacquire(IRServoEnum selector);
float irHeading(IRServoEnum selector);

// Internal method declarations
task _leftIrTask();
task _rightIrTask();
void _irTaskDetails(IRServoStruct irss);

IRServoEnum initializeIRServo(TServoIndex servo, int servoCenterPos,
                              tSensors sensor, int sensorOffsetPos) {
    // Ensure we only allow the number allocated
    if (servoNum >= IRSERVO_LEN) {
        PlaySound(soundException);
        return IRSERVO_LEN + 1;
    }

    irServoData[servoNum].irServo = servo;
    irServoData[servoNum].irServoCenterPos = servoCenterPos;
    irServoData[servoNum].irSensor = sensor;
    irServoData[servoNum].irSensorOffsetPos = sensorOffsetPos;

    // You can't start the same task name more than once, so we have to
    // define two separate tasks, even if we can differentiate between
    // them with global variables.
    switch(servoNum) {
    case IR_LEFT:
        irServoData[servoNum].initialSweepDirection = SWEEP_LEFT;
        StartTask(_leftIrTask);
        break;

    case IR_RIGHT:
        irServoData[servoNum].initialSweepDirection = SWEEP_RIGHT;
        StartTask(_rightIrTask);
        break;
    }
    return servoNum++;
}

bool irAcquired(IRServoEnum selector) {
    if (selector >= IR_LEFT && selector < IRSERVO_LEN) {
        return irServoData[selector].acquired;
    }
    return false;
}

void forceReacquire(IRServoEnum selector) {
    if (selector >= IR_LEFT && selector < IRSERVO_LEN) {
        irServoData[selector].acquired = false;
    }
}

float irHeading(IRServoEnum selector) {
    if (irAcquired(selector))
        return irServoData[selector].irHeading;
    else
        return 0.0;
}

// Using two tasks due to limitations in RobotC
task _leftIrTask() {
    _irTaskDetails(irServoData[IR_LEFT]);
}

task _rightIrTask() {
    _irTaskDetails(irServoData[IR_RIGHT]);
}

// It's approximately 180 degrees for the entire servo span, so each
// servo tick is ~0.72 degrees.
const float SERVO_POS_TO_DEGREES = 180.0 / 255.0;

// HiTechnic IR sensor constants
const int IR_NOMATCH_SECTION = 0;
const int IR_RIGHT_SECTION = 4;
const int IR_CENTER_SECTION = 5;
const int IR_LEFT_SECTION = 6;

void _irTaskDetails(IRServoStruct irss) {
    // How many positions for every update (~20ms)
    servoChangeRate[irss.irServo] = 10;

    // Initialize things
    irss.acquired = false;
    servo[irss.irServo] = irss.irServoCenterPos;
    SweepDirection sweepDirection = irss.initialSweepDirection;

    // Wait a bit to ensure the servos have time to get to their location
    wait1Msec(1000);

    // Choose the initial center transition section.  Due to servo
    // limitations, we may have to switch to the LEFT/RIGHT section to
    // avoid the endpoints.
    int irTriggerSection = IR_RIGHT_SECTION;

    // Start 'er up!
    int prevIrSection = SensorValue[irss.irSensor];
    int servoPos = servo[irss.irServo];
    while (true) {
        // Did we find the sensor?
        int currIrSection = SensorValue[irss.irSensor];
        if (currIrSection == IR_NOMATCH_SECTION) {
            irss.acquired = false;

            // No sensor found, so sweep back and forth looking for it.
            if (sweepDirection == SWEEP_LEFT) {
                servoPos--;
                if (servoPos <= 0) {
                    sweepDirection = SWEEP_RIGHT;
                    servoPos = 0;
                }
            } else {
                servoPos++;
                if (servoPos >= 255) {
                    sweepDirection = SWEEP_LEFT;
                    servoPos = 255;
                }
            }
        } else if (prevIrSection != 0 && abs(currIrSection - prevIrSection) <= 1) {
            // The above line filters out messed up values that occur
            // when the signal gets too great.  The servo isn't going
            // fast enough to skip an entire section, so if that occurs,
            // we've got issues.

            // Treat this like a line-follower with the transition
            // between the CENTER section and LEFT/RIGHT section as the
            // line to follow.

            // First, check if we're at the transition.  If so,
            // calculate the angle to the IR beacon.  Note, the sensor
            // apparently has a different transition from left->right as
            // it does right->left, so we calculate the angle in the
            // same direction everytime, otherwise we end up with
            // different angles.  Either that, or we are going too fast
            // and the sensor can't respond fast enough.
            if (currIrSection == IR_CENTER_SECTION && prevIrSection == irTriggerSection) {
                // Calculate the heading since this is the transition.
                // We must take into account the ir section offset from
                // center.
                int irPos = servoPos;
                if (irTriggerSection == IR_RIGHT_SECTION)
                    irPos += irss.irSensorOffsetPos;
                else
                    irPos -= irss.irSensorOffsetPos;

                // Calculate the heading to the IR beacon
                float currHeading = (irPos - irss.irServoCenterPos) * SERVO_POS_TO_DEGREES;

                // Add a little hysteresis to avoid changing the heading
                // too quickly since noise may cause bogus errors.
                irss.irHeading = 0.5 * irss.irHeading + 0.5 * currHeading;
                irss.acquired = true;

                // Ensure we aren't too close to the edge and need to
                // switch the trigger section.
                if (servoPos < 64 && irTriggerSection == IR_RIGHT_SECTION)
                    irTriggerSection = IR_LEFT_SECTION;
                else if (servoPos > 192 && irTriggerSection == IR_LEFT_SECTION)
                    irTriggerSection = IR_RIGHT_SECTION;
            }
            // Try to stay centered on the transition
            if (irTriggerSection == IR_RIGHT_SECTION) {
                if (currIrSection <= IR_RIGHT_SECTION)
                    servoPos++;
                else
                    servoPos--;
            } else {
                if (currIrSection >= IR_LEFT_SECTION)
                    servoPos--;
                else
                    servoPos++;
            }
            // If we are too close to the edge we may not be able to
            // find the line properly, so ensure the servoPos is in
            // bounds.
            if (servoPos < 0)
                servoPos = 0;
            else if (servoPos > 255)
                servoPos = 255;
        } else
            irss.acquired = false;

        // Move the servo and keep track of the old readings
        servo[irss.irServo] = servoPos;
        prevIrSection = currIrSection;

        // How long to delay between movement of the servo ticks.  If
        // we're close to the center section, slow down the scan if
        // we're tracking to get more accurate readings.
        int servoDelayTime = 4;  // Twice as long as it should take for 1pos
        if (abs(IR_CENTER_SECTION - currIrSection) <= 1)
            servoDelayTime *= 2;
        wait1Msec(servoDelayTime);
    }
}