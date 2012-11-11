#pragma config(Hubs,  S1, HTMotor,  HTMotor,  none,     none)
#pragma config(Sensor, S4,     gyro,                sensorI2CHiTechnicGyro)
#pragma config(Motor,  mtr_S1_C1_1,     frontLeft,     tmotorNormal, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C1_2,     frontRight,    tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,     rearLeft,      tmotorNormal, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C2_2,     rearRight,     tmotorNormal, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//Include file to "handle" the Bluetooth messages.
#include "JoystickDriver.c"

// Forward declaration of routine used by Gyro
void disableMotors();

// The driver for the gyro
#include "HiTech_Gyro.c"

// Operator control
void showDisplay();
void moveOmni();
int expoJoystick(int eJoy);
void checkCalibration();

// Holonomic drive control
void driveHolo(int angle, int magnitude, int rotation, int powerReduction);

// Are we running in reduced speed mode?
bool slowSpeedEnabled = false;

void disableMotors()
{
    motor[frontLeft] = 0;
    motor[frontRight] = 0;
    motor[rearLeft] = 0;
    motor[rearRight] = 0;
}

void initializeRobot()
{
    // Make sure the motors are turned off
    disableMotors();

    // Initialize the gyro
    initializeGyro(gyro);
}

task main()
{
    initializeRobot();

    // wait for start of tele-op phase
//    nxtDisplayString(2, "Waiting");
//    waitForStart();
//    nxtDisplayString(2, "       ");

    while (true)
    {
        // Display all the information on the screen
        showDisplay();

        // Get current joystick buttons and analog movements
        getJoystickSettings(joystick);

        // Don't move if we're calibrating the gyro
        if (!calibratingActive())
        {
            // Move robot
            moveOmni();

            // Check to see if we want to calibrate the gyro.
            checkCalibration();
        }
    }
}

void showDisplay()
{
    // Display the robot and IR heading
    nxtDisplayString(0, "Heading=%3.1f", getGyroHeading());

    nxtDisplayString(7, "SSE=%d", slowSpeedEnabled);
}

// Move the robot on the field using the joystick
bool slowSpeedButtonWasPressed = false;
void moveOmni()
{
    // Check the low-speed power setting.  If set, we'll reduce the motor
    // power's by half.
    bool btnPress = joy1Btn(9);
    if (!btnPress && slowSpeedButtonWasPressed)
    {
        // Beep to indicate a speed switch
        PlaySound(soundBlip);
        slowSpeedEnabled = !slowSpeedEnabled;
    }
    slowSpeedButtonWasPressed = btnPress;

    // Make things less sensitive around the center (a slight
    // dead-band), and more aggressive at the extremes.
    int xJoy1 = expoJoystick(joystick.joy1_x1);
    int yJoy1 = expoJoystick(joystick.joy1_y1);
    int xJoy2 = expoJoystick(joystick.joy1_x2) / 2; // Reduce spin-speed

    // Determine the magnitude and direction of the joystick from
    // the operator's perspective, where 0 degrees is going straight away.
    float magnitude = sqrt(pow(xJoy1, 2) + pow(yJoy1, 2));
    int operatorHeading = 0;
    if (abs(yJoy1) > 0 || abs(xJoy1) > 0)
        operatorHeading = radiansToDegrees(atan2(yJoy1, xJoy2));

    // Now that we have the operator's perspective, translate the
    // direction to the robot's perspective.
    float robotHeading = operatorHeading - getGyroHeading();

    // Drive holonomic
    int powerReduction = 1;
    if (slowSpeedEnabled)
        powerReduction = 2;
    driveHolo(robotHeading, magnitude, xJoy2, powerReduction);
}

// Allow the operator to re-synchronize the robot to 'north'.  Note,
// when this happens, we turn off the motors and let the robot go idle
// for a bit.
bool calibrateGyroButtonWasPressed = false;

void checkCalibration()
{
    // If we're already calibrating (or will be shortly), don't bother
    // checking again.
    if (calibratingActive())
        return;

    bool btnPress = joy1Btn(6);
    if (!btnPress && calibrateGyroButtonWasPressed)
        calibrateGyro();
    calibrateGyroButtonWasPressed = btnPress;
}

void driveHolo(int angle, int magnitude, int rotation, int powerReduction)
{
    // Normalize the angle to -180 <-> +180
    while (angle > 180)
        angle -= 360;
    while (angle < -180)
        angle += 360;

    // Convert the power and angle into motor forward and right
    // powers.
    int forward = magnitude * cosDegrees(angle);
    int right = magnitude * sinDegrees(angle);
    int rotCw = rotation;

    // Holonomic drive wheel math
    int flPow = forward + rotCw + right;
    int frPow = forward - rotCw - right;
    int rlPow = forward + rotCw - right;
    int rrPow = forward - rotCw + right;

    // Make sure none of the wheel power's exceed 100%.  If so, reduce
    // all by the same amount.
    int max = abs(flPow);
    if (abs(frPow) > max)
        max = abs(frPow);
    if (abs(rlPow) > max)
        max = abs(rlPow);
    if (abs(rrPow) > max)
        max = abs(rrPow);

    if (max > 100)
    {
        // Reduce each motor power by the same amount;
        float reduction = 100.0 / (float)max;
        flPow *= reduction;
        frPow *= reduction;
        rlPow *= reduction;
        rrPow *= reduction;
    }

    // Reduction
    if (powerReduction > 1)
    {
        flPow /= powerReduction;
        frPow /= powerReduction;
        rlPow /= powerReduction;
        rrPow /= powerReduction;
    }

    motor[frontLeft] = flPow;
    motor[frontRight] = frPow;
    motor[rearLeft] = rlPow;
    motor[rearRight] = rrPow;
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

    // Convert the number back to a joystick value
    return (int)(128.0 * result);
}
