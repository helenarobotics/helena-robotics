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
 * arcade drive controls.
 *
 ************************************************************
 * Controls:
 * ---------
 *
 * Joystick 1:
 *   Left Analog - Y-Axis
 *      Controls the forward/backward direction of the robot
 *   Right Analog - X-Axis
 *      Controls the left/right rotation of the robot
 *
 *   Button 11
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
void disableMotors();

// Operator control
void showDisplay();
void moveRobot();
int expoJoystick(int eJoy);

// Two motor drive
void driveTwoMotor(int angle, int magnitude, int powerReduction);

// Are we running in reduced speed mode?
bool slowSpeedEnabled = false;

void disableMotors() {
    motor[leftMotor] = 0;
    motor[rightMotor] = 0;
}

void initializeRobot()
{
    // Make sure the motors are turned off
    disableMotors();
}

task main() {
    initializeRobot();

    // wait for start of tele-op phase
//    nxtDisplayString(2, "Waiting");
//    waitForStart();
//    nxtDisplayString(2, "       ");

    while (true) {
        // Display all the information on the screen
        showDisplay();

        // Get current joystick buttons and analog movements
        getJoystickSettings(joystick);

        // Move robot
        moveRobot();
    }
}

void showDisplay()
{
    nxtDisplayString(7, "SSE=%d", slowSpeedEnabled);
}

// Move the robot on the field using the joystick
bool slowSpeedButtonWasPressed = false;
void moveRobot()
{
    // Check the low-speed power setting.  If set, we'll reduce the motor
    // power's by half.
    bool btnPress = joy1Btn(11);
    if (!btnPress && slowSpeedButtonWasPressed)
    {
        // Beep to indicate a speed switch
        PlaySound(soundBlip);
        slowSpeedEnabled = !slowSpeedEnabled;
    }
    slowSpeedButtonWasPressed = btnPress;

    // Make things less sensitive around the center (a slight
    // dead-band), and more aggressive at the extremes.
    int xJoy2 = expoJoystick(joystick.joy1_x2);
    int yJoy1 = expoJoystick(joystick.joy1_y1);

    // Determine the magnitude and direction of the joystick from
    // the operator's perspective, where 0 degrees is going straight away.
    float magnitude = sqrt(pow(xJoy2, 2) + pow(yJoy1, 2));
    int operatorHeading = 0;
    if (abs(yJoy1) > 0 || abs(xJoy2) > 0)
        operatorHeading = radiansToDegrees(atan2(yJoy1, xJoy2));

    // Drive
    int powerReduction = 1;
    if (slowSpeedEnabled)
        powerReduction = 2;
    driveTwoMotor(operatorHeading, magnitude, powerReduction);
}

#define MAX(n1, n2)		(((n1) >= (n2)) ? (n1) : (n2))

void driveTwoMotor(int angle, int magnitude, int powerReduction)
{
    // Convert the power and angle into motor forward and right
    // powers.
    int forward = magnitude * cosDegrees(angle);
    int right = magnitude * sinDegrees(angle);

    // Combine the two for each motor
    int leftPow = forward + right;
    int rightPow = forward - right;

    // Ensure that we don't try to give too much power to the motor.
    float maxPow = MAX(leftPow, rightPow);
    if (maxPow > 100) {
        float reduction = 100.0 / maxPow;
        leftPow *= reduction;
        rightPow *= reduction;
    }

    // Reduction
    if (powerReduction > 1)
    {
        leftPow /= powerReduction;
        rightPow /= powerReduction;
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
    float floatJoy = (float)eJoy / 128.0;
    float result = SENSITIVITY * pow(floatJoy, 3) +
                   (1 - SENSITIVITY) * floatJoy;

    // Convert the number back to a motor power, which is between -100
    // and 100.
    return (int)(100.0 * result);
}
