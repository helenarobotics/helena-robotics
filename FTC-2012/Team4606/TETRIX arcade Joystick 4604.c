#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTServo,  none)
#pragma config(Sensor, S1,     ,                    sensorI2CMuxController)
#pragma config(Motor,  mtr_S1_C1_1,     leftDrive,    tmotorNormal, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C1_2,     rightDrive,     tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,     conveyorMotor, tmotorNormal, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C2_2,     armMotor,      tmotorNormal, openLoop)
#pragma config(Servo,  srvo_S1_C3_1,    clawServo,            tServoStandard)
#pragma config(Servo,  srvo_S1_C3_2,    conveyorDumpServo,    tServoStandard)
#pragma config(Servo,  srvo_S1_C3_3,    bowlingBallServo,     tServoStandard)
#pragma config(Servo,  srvo_S1_C3_4,    servo4,               tServoStandard)
#pragma config(Servo,  srvo_S1_C3_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*--------------------------------------------------------------------------------------------------------*\
|*                                                                                                        *|
|*                                    - Tetrix Quiet  Tank Drive -                                        *|
|*                                          ROBOTC on Tetrix                                              *|
|*                                                                                                        *|
|*  This program allows you to drive a robot via remote control using the ROBOTC Debugger.                *|
|*  This particular method uses "Tank Drive" where each side is controlled individually like a tank.      *|
|*  This program also ignores low values that would cause your robot to move when the joysticks fail to   *|
|*  return back to exact center.  You may need to play with the 'threshold' value to get it just right.   *|
|*                                                                                                        *|
|*                                        ROBOT CONFIGURATION                                             *|
|*    NOTES:                                                                                              *|
|*                                                                                                        *|
|*    MOTORS & SENSORS:                                                                                   *|
|*    [I/O Port]              [Name]              [Type]              [Description]                       *|
|*    Port D                  motorD              12V                 Right motor                         *|
|*    Port E                  motorE              12V                 Left motor                          *|
\*---------------------------------------------------------------------------------------------------4246-*/

#include "JoystickDriver.c"

// This allows us to ignore low readings that keep our robot in perpetual motion.
const int JOY_THRESHOLD = 10;

// Motor constants and state
const int CHOOSE_DRIVEMODE = 0;
const int ARCADE_DRIVE = 1;
const int TANK_DRIVE = 2;
const int MOTOR_POWER_INCREMENT = 20;
bool reverseDirection;
int percPow;
int DRIVE_MODE; // looks like a const,  por formating

// Conveyor
const int CONVEYOR_POWER = 100;
bool conveyorOn = false;
const int CONVEYOR_DUMP_UP_POS = 130;
const int CONVEYOR_DUMP_DOWN_POS = 0;
bool conveyorDumpUp = true;

// CLAWS - At startup, make sure they are 'in'
const int CLAWS_IN_POS = 0;
const int CLAWS_OUT_POS = 200;
bool clawsIn = true;

// At startup, place the bowling ball capture servo in the up position
const int BALLCAPTURE_UP_POS = 210;
const int BALLCAPTURE_DOWN_POS = 70;
bool ballCaptureUp = true;

void initializeRobot()
{
    // Default to full power on the motors, but turn them off.
    percPow = 100;
    motor[leftDrive] = 0;
    motor[rightDrive] = 0;
    DRIVE_MODE = CHOOSE_DRIVEMODE;
    reverseDirection = false;

    // Move the bowling ball capture arm up
    servo[bowlingBallServo] = BALLCAPTURE_UP_POS;
    ballCaptureUp = true;

    // Default the claws to be in
    servo[clawServo] = CLAWS_IN_POS;
    clawsIn = true;

    // Conveyor Off and Arm Up
    motor[conveyorMotor] = 0;
    conveyorOn = false;
    servo[conveyorDumpServo] = CONVEYOR_DUMP_UP_POS;
    conveyorDumpUp = true;
}

bool btnBBWP = false;
void moveBowlingBallServo()
{
    // Toggle the servo location when the button is released.
    bool btnBBIP = joy1Btn(4);
    if (!btnBBIP && btnBBWP)
        ballCaptureUp = !ballCaptureUp;
    btnBBWP = btnBBIP;

    if (ballCaptureUp) {
        servo[bowlingBallServo] = BALLCAPTURE_UP_POS;
    } else {
        servo[bowlingBallServo] = BALLCAPTURE_DOWN_POS;
    }
}

bool btn1WP = false, btn5WP = false, btn6WP = false;
void moveRobot()
{
    int lPow = 0;
    int rPow = 0;
    if (DRIVE_MODE == CHOOSE_DRIVEMODE) {
        if (joy1Btn(5)) {
            DRIVE_MODE = TANK_DRIVE;
        } else if (joy1Btn(6)) {
            DRIVE_MODE = ARCADE_DRIVE;
        }
    } else if (DRIVE_MODE == TANK_DRIVE) {
        // Make sure the joystick is read past the 'threshold' value
        if (abs(joystick.joy1_y1) > JOY_THRESHOLD)
            // Move proportionally
            lPow = joystick.joy1_y1 / 1.28;

        if (abs(joystick.joy1_y2) > JOY_THRESHOLD)
            // Move proportionally
            rPow = joystick.joy1_y2 / 1.28;
    } else if (DRIVE_MODE == ARCADE_DRIVE) {
        int nSpeedPower = 0;
        int nTurnPower = 0;
        if (abs(joystick.joy1_y1) > JOY_THRESHOLD)
            nSpeedPower = joystick.joy1_y1 / 1.28;
        if (abs(joystick.joy1_x2) > JOY_THRESHOLD)
            nTurnPower = joystick.joy1_x2 / 1.28;

        // Power and speed
        rPow = nSpeedPower - nTurnPower;
        lPow = nSpeedPower + nTurnPower;

        // If we're turning, halve the power
        if (abs(nTurnPower) > 0 && abs(nSpeedPower) > 25) {
            rPow /= 2;
            lPow /= 2;
        }
    }

    // Scale the motor power down
    lPow = lPow * percPow / 100;
    rPow = rPow * percPow / 100;

    // Reverse the front and rear of robot
    if (reverseDirection) {
        int tempL = lPow;
        lPow = -rPow;
        rPow = -tempL;
    }
    motor[leftDrive] = lPow;
    motor[rightDrive] = rPow;

    if (DRIVE_MODE != CHOOSE_DRIVEMODE) {
        // Buttons 5 and 6 decrease/increase the power in incremental chunks
        bool btn5IP = joy1Btn(5);
        if (btn5IP && !btn5WP) {
            if (percPow > MOTOR_POWER_INCREMENT)
                percPow -= MOTOR_POWER_INCREMENT;
        }
        btn5WP = btn5IP;

        bool btn6IP = joy1Btn(6);
        if (btn6IP && !btn6WP) {
            if (percPow <= (100 - MOTOR_POWER_INCREMENT))
                percPow += MOTOR_POWER_INCREMENT;
        }
        btn6WP = btn6IP;

        // Reverse the front and rear directions
        bool btn1IP = joy1Btn(1);
        if (btn1IP && !btn1WP)
            reverseDirection = !reverseDirection;
        btn1WP = btn1IP;
    }
}

void moveLiftArm()
{
    if (abs(joystick.joy2_y1) > JOY_THRESHOLD)
    {
        motor[motorG] = joystick.joy2_y1;
    }
    else
    {
        motor[motorG] = 0;
    }
}

void moveRotationArm()
{
    if (abs(joystick.joy2_y2) > JOY_THRESHOLD)
    {
        motor[motorE] = joystick.joy2_y2;
    }
    else
    {
        motor[motorE] = 0;
    }
}

bool btnBCWP = false;
void toggleBallConveyor()
{
    // Toggle the conveyor on/off
    bool btnBCIP = joy1Btn(7);
    if (!btnBCIP && btnBCWP)
        conveyorOn = !conveyorOn;
    btnBCWP = btnBCIP;

    // Enable the ball conveyor
    if (conveyorOn)
    {
        motor[conveyorMotor] = CONVEYOR_POWER;
    }
    else
    {
        motor[conveyorMotor] = 0;
    }
}

bool btnClawWP = false;
void toggleClaws()
{
    // Toggle the servo location when the button is released.
    bool btnClawIP = joy2Btn(5);
    if (!btnClawIP && btnClawWP)
        clawsIn = !clawsIn;
    btnClawWP = btnClawIP;

    // Claws
    if (clawsIn)
    {
        servo[clawServo] = CLAWS_IN_POS;
    }
    else
    {
        servo[clawServo] = CLAWS_OUT_POS;
    }
}

bool btnCDWP = false;
void toggleConveyorDump()
{
    // Toggle the servo location when the button is released.
    bool btnCDIP = joy2Btn(6);
    if (!btnCDIP && btnCDWP)
        conveyorDumpUp = !conveyorDumpUp;
    btnCDWP = btnCDIP;

    if (conveyorDumpUp)
    {
        servo[conveyorDumpServo] = CONVEYOR_DUMP_UP_POS;
    }
    else
    {
        servo[conveyorDumpServo] = CONVEYOR_DUMP_DOWN_POS;
    }
}

// Main routine
task main()
{
    initializeRobot();

    // wait for start of tele-op phase
    waitForStart();

    // Infinite loop:
    while (true)
    {
        getJoystickSettings(joystick);

        // Drive the robot
        moveRobot();

        // Rear bowling ball capture frame
        moveBowlingBallServo();

        // Lift Arm
        //moveLiftArm();

        // Rotation Arm
        //moveRotationArm();

        // Ball conveyor
        toggleBallConveyor();

        // Claws
        toggleClaws();

        //Conveyor dump
        toggleConveyorDump();
    }
}
