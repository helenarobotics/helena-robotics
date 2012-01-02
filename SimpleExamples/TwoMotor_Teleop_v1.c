#pragma config(Hubs,  S1, HTMotor,  none,     none,     none)
#pragma config(Sensor, S1,     ,                    sensorI2CMuxController)
#pragma config(Motor,  mtr_S1_C1_1,     leftMotor,          tmotorNormal, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C1_2,     rightMotor,         tmotorNormal, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/************************************************************
 * Simple Teleop Mode Two Motor (left/right) Drive Code
 ************************************************************
 *
 * This program is setup to move the robot using the joystick following
 * tank drive controls.
 *
 ************************************************************
 * Controls:
 * ---------
 *
 * Joystick 1:
 *   Left Analog - Y-Axis
 *      Controls the power and direction of left motor (leftMotor)
 *   Right Analog - Y-Axis
 *      Controls the power and direction of right motor (rightMotor)
 *
 *   Left Analog - Y-Axis
 *      Toggles slow-speed drive mode. When enabled, the robot moves at
 *      half-speed.
 *
 ************************************************************
 * Hardware Configuration
 * ----------------------
 * A single HiTechnic Motor Controller with two motors should be
 * connected to Port 1 of the NXT
 * 
 * Motors
 * ------
 * leftMotor  - Connected to Motor 1 of Motor Controller: Controls left motor
 * rightMotor - Connected to Motor 2 of Motor Controller: Controls right motor
 * 
 * Joystick
 * --------
 * A single Logitech USB joystick must be connected to the computer and
 * verified to be working.
 ************************************************************
 */

//Include file to "handle" the joystick messages from Bluetooth and/or
//the Samantha module.
#include "JoystickDriver.c"

// Forward declarations
void moveSimple();
int expoJoystick(int eJoy);
float Power(float num, int exp);

void initializeRobot() {
    motor[leftMotor] = 0;
    motor[rightMotor] = 0;
}


task main() {
    initializeRobot();

    // wait for start of tele-op phase
//    waitForStart();

    while (true) {
        // Get current joystick buttons and analog movements
        getJoystickSettings(joystick);

        // Move robot
        moveTank();
    }
}

// Move the motor on the field
bool slowSpeedButtonWasPressed = false;
bool slowSpeedEnabled = false;
void moveTank()
{
    // Make things less sensitive around the center (a slight
    // dead-band), and more aggressive at the extremes.
    int leftPow = expoJoystick(joystick.joy1_y1);
    int rightPow = expoJoystick(joystick.joy1_y2);

    // Check the low-speed power setting.  If set, reduce power by half.
    bool btnPress = joy1Btn(11);
    nxtDisplayString(2, "Btn=%d", btnPress);
    if (!btnPress && slowSpeedButtonWasPressed)
        slowSpeedEnabled = !slowSpeedEnabled;
    slowSpeedButtonWasPressed = btnPress;
    if (slowSpeedEnabled) {
        leftPow /= 2;
        rightPow /= 2;
    }

    motor[leftMotor] = leftPow;
    motor[rightMotor] = rightPow;
}

// http://www.chiefdelphi.com/forums/showthread.php?p=921992
const float SENSITIVITY = 0.7;
int expoJoystick(int eJoy)
{
    // convert the joystick inputs to a floating point number
    // between -1 and +1
    float floatJoy = (float)eJoy / 127.0;
    float result = SENSITIVITY * Power(floatJoy, 3) +
                   (1 - SENSITIVITY) * floatJoy;

    // Convert the number back to a motor power, which is between -100
    // and 100.
    return (int)(100.0 * result);
}

// RobotC's built-in doesn't work with negative numbers very well.
float Power(float num, int exp)
{
    // require positive integer for the exponent
    if (exp <= 0)
        return 0;

    float result = num;
    for (int i = 1; i < exp; i++)
        result *= num;
    return result;
}
