#pragma config(Hubs,  S1, HTMotor,  none,     none,     none)
#pragma config(Motor,  mtr_S1_C1_1,     mLTrack,       tmotorNormal, PIDControl, reversed, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     mRTrack,       tmotorNormal, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/**************************************************
 * Controls
 * --------
 * Joystick 1:
 *   Left Analog (TANK DRIVE MODE)
 *      Controls the power and direction of left track motor (mLTrack)
 *   Right Analog (TANK DRIVE MODE)
 *      Controls the power and direction of right track motor (mRTrack)
 * [[ OR ]]
 *   Left Analog - Y-Axis (ARCADE ONEJOY & TWOJOY DRIVE)
 *      Controls the power (front/back) of both tracks (mLTrack & mRTrack)
 *   Left Analog - X-Axis (ARCADE ONEJOY DRIVE )
 *   Right Analog - X-Axis (ARCADE TWOJOY DRIVE)
 *      Controls the direction of both tracks (mLTrack & mRTrack)
 **************************************************
 * Motors
 * ------
 * mLTrack
 *      Controls tank track movement (left track)
 * mRTrack
 *      Controls tank track movement (right track)
 **************************************************
 */

#include "JoystickDriver.c"

//
// The BOUND macro limits the input (n) inside the bounds between the
// provided low (l) and high (h) limits.
//
#define BOUND(n, l, h)      (((n) < (l))? (l): ((n) > (h))? (h): (n))

// Mode for controlling the tank tracks
const int DRIVE_TANK          = 0;
const int DRIVE_ARCADE_ONEJOY = 1;
const int DRIVE_ARCADE_TWOJOY = 2;

int DRIVE_MODE = DRIVE_ARCADE_TWOJOY;

// Forward method declarations
void moveTracks();

float Power(float num, int exp);
int expoJoystick(int eJoy);

void initializeRobot()
{
    // The tracks are controlled via the joystick and have no background
    // tasks, so turn off the motors and initialize their encoders.
    motor[mLTrack] = 0;
    motor[mRTrack] = 0;
    nMotorEncoder[mLTrack] = 0;
    nMotorEncoder[mRTrack] = 0;
}

task main()
{
    // Setup the robot to be where we want it to be.
    initializeRobot();

    // Start when the teleop phase begins.
    waitForStart();

    // Loop indefinitely
    while (true) {
        // Get current joystick buttons and analog movements
        getJoystickSettings(joystick);

        // Move robot
        moveTracks();
    }
}

//
// Move the robot!
//
bool slowSpeedButtonWasPressed = false;
bool slowSpeedEnabled = false;
void moveTracks()
{
    int rPow, lPow;
    int nSpeedPower, nTurnPower;

    switch (DRIVE_MODE) {
    default:
    case DRIVE_TANK:
        // Make things less sensitive around the center (a slight
        // dead-band), and more aggressive at the extremes.
        lPow = expoJoystick(joystick.joy1_y1);
        rPow = expoJoystick(joystick.joy1_y2);

        // Unused, but avoids a calculation later
        nTurnPower = 0;
        break;

    case DRIVE_ARCADE_ONEJOY:
        // Make things less sensitive around the center (a slight
        // dead-band), and more aggressive at the extremes.
        nSpeedPower = expoJoystick(joystick.joy1_y1);
        nTurnPower = expoJoystick(joystick.joy1_x1);

        // Power and speed
        lPow = nSpeedPower + nTurnPower;
        rPow = nSpeedPower - nTurnPower;
        break;

    case DRIVE_ARCADE_TWOJOY:
        // Make things less sensitive around the center (a slight
        // dead-band), and more aggressive at the extremes.
        nSpeedPower = expoJoystick(joystick.joy1_y1);
        nTurnPower = expoJoystick(joystick.joy1_x2);

        // Power and speed
        lPow = nSpeedPower + nTurnPower;
        rPow = nSpeedPower - nTurnPower;
        break;
    }

    // Reduce turning power at speed by 5%
    if (nTurnPower != 0 && abs(nSpeedPower) > 30) {
        lPow -= nTurnPower / 20;
        rPow += nTurnPower / 20;
    }

    // Check the low-speed power setting.  If set, reduce power by half.
    bool btnPress = joy1Btn(9);
    if (!btnPress && slowSpeedButtonWasPressed)
        slowSpeedEnabled = !slowSpeedEnabled;
    slowSpeedButtonWasPressed = btnPress;
    if (slowSpeedEnabled) {
        lPow /= 2;
        rPow /= 2;
    }

    // Ensure we limit the motors!
    lPow = BOUND(lPow, -100, 100);
    rPow = BOUND(rPow, -100, 100);

    nxtDisplayString(1, "Pow L/R %d/%d", lPow, rPow);

    // XXX - Check to make sure these are working.
    nxtDisplayString(2, "Enc L/R %d/%d",
                     nMotorEncoder[mLTrack], nMotorEncoder[mRTrack]);

    motor[mLTrack] = lPow;
    motor[mRTrack] = rPow;
}

// These give us a nice exponential band to make using the robots
// controls easier.
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

// http://www.chiefdelphi.com/forums/showthread.php?p=921992
const float SENSITIVITY = 0.7;
int expoJoystick(int eJoy)
{
    // convert the joystick inputs to a floating point number
    // between -1 and +1
    float floatJoy = eJoy / 127.0;
    float result = SENSITIVITY * Power(floatJoy, 3) +
                   (1 - SENSITIVITY) * floatJoy;

    // Convert the number back to a motor power, which is between -100
    // and 100.
    return (int)(100.0 * result);
}