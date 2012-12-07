#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTServo,  none)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Sensor, S3,     irSensor,       sensorHiTechnicIRSeeker1200)
#pragma config(Motor,  mtr_S1_C1_1,     motorD,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C1_2,     motorE,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,     motorF,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_2,     motorG,        tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S1_C3_1,    irServo,              tServoStandard)
#pragma config(Servo,  srvo_S1_C3_2,    servo2,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// Servo/sensor pair driver
#include "IRServo.c"

// Servo constants
const int SERVO_CENTER_POS = 125;

// The IR section we are using is offset this many servo ticks from center
const int SERVO_IRSECTION_POS_OFFSET = 15;

// Access to IRServos
IRServoEnum irServoIndex;

// Forward declaration
void showDisplay();

void initializeRobot() {
    // Initialize the left/right servo/sensor blocks.
    irServoIndex = initializeIRServo(
        irServo, SERVO_CENTER_POS, irSensor, SERVO_IRSECTION_POS_OFFSET);
}

task main() {
    initializeRobot();

// Not used here, but if so would be setup here
//    waitForStart();

    startIRServo(irServoIndex);

    while (true) {
        // Display all the information on the screen
        showDisplay();
        EndTimeSlice();
    }
}

void showDisplay() {
    // IR beacons
    if (irAcquired(irServoIndex))
        nxtDisplayString(1, "IR=%3.1f", irHeading(irServoIndex));
    else
        nxtDisplayClearTextLine(1);
}
