#pragma config(Hubs,  S1, HTMotor,  HTMotor,  none,     none)
#pragma config(Sensor, S4,     gyro,                sensorI2CHiTechnicGyro)
#pragma config(Motor,  mtr_S1_C1_1,     frontLeft,     tmotorNormal, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C1_2,     frontRight,    tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,     rearLeft,      tmotorNormal, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C2_2,     rearRight,     tmotorNormal, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// The driver for the gyro
#include "drivers/HTGYRO-driver.h"

//Include file to "handle" the Bluetooth messages.
#include "JoystickDriver.c"

// Forward declarations
void moveOmni();
int expoJoystick(int eJoy);
float Power(float num, int exp);
task gyroTask();

// Keeps track of the current heading since we started.
float currHeading;

void initializeRobot()
{
    // Calibrate the gyro, and then start the gyro task to keep track of
    // the robot's heading.
    HTGYROstartCal(gyro);
    currHeading = 0;
    StartTask(gyroTask);

    motor[frontLeft] = 0;
    motor[frontRight] = 0;
    motor[rearLeft] = 0;
    motor[rearRight] = 0;
}

// Allow the operator to re-synchronize the robot to 'north'.  Note,
// when this happens, we turn off the motors and let the robot go idle
// for a bit.
bool calibrating = false;
bool calibrateGyroButtonWasPressed = false;

void checkCalibration()
{
    // If we're already calibrating (or will be shortly), don't bother
    // checking again.
    if (calibrating)
        return;

    bool btnPress = joy1Btn(6);
    if (!btnPress && calibrateGyroButtonWasPressed)
        calibrating = true;
    calibrateGyroButtonWasPressed = btnPress;
}

// Task to keep track of the current heading using the HT Gyro
task gyroTask()
{
    long prevTime = nPgmTime;

    while (true) {
        // Check to see if we want to calibrate the gyro
        if (calibrating == true) {
            PlaySound(soundBeepBeep);

            // Turn off the motors so the gyro can synchronize
            motor[frontLeft] = 0;
            motor[frontRight] = 0;
            motor[rearLeft] = 0;
            motor[rearRight] = 0;

            // Wait for the robot to stop in case it was moving.
            wait1Msec(1 * 1000);

            // Re-calibrate and reset the heading to 0
            HTGYROstartCal(gyro);
            currHeading = 0;

            // Done calibrating
            PlaySound(soundBeepBeep);
            calibrating = false;
        }

        // This tells us the current rate of rotation in degrees per
        // second.
        int currRate = HTGYROreadRot(gyro);

        // How much time has elapsed since we last checked, which we use
        // to determine how far we've turned
        long now = nPgmTime;

        // Filter out the high-frequency 'noise' and really weird spikes
        // caused from the sensor bouncing around.  Something better
        // would be nice, but this works well enough in practice.
        if (abs(currRate) > 3) {
            // deltaSecs will only go negative if we've if nPgmTime
            // wrapped around.  If so, re-calculate the difference as by
            // incrementing both now and prevTime by a large number so
            // prevTime will both are both wrapped, and then
            // re-calculate the difference.
            float deltaSecs = (now - prevTime) / 1000.0;
            if (deltaSecs < 0) {
                now += 1024;
                prevTime += 1024;
                deltaSecs = (now - prevTime) / 1000.0;
            }

            // Calculate how many degrees the heading changed.
            float degChange = (float)currRate * deltaSecs;
            currHeading += degChange;

            // Make sure we stay in +- 180 range.  Not necessary, but it
            // helps when debugging.
            while (currHeading > 180.0)
                currHeading -= 360.0;
            while (currHeading < -180.0)
                currHeading += 360.0;
        }
        prevTime = now;

        // Wait a bit..
        EndTimeSlice();
    }
}

task main()
{
    initializeRobot();

    // wait for start of tele-op phase
//    nxtDisplayString(2, "Waiting");
//    waitForStart();
//    nxtDisplayString(2, "       ");

    while (true) {
        // Display the robot heading
        nxtDisplayString(0, "Heading=%3.1f", currHeading);

        // Get current joystick buttons and analog movements
        getJoystickSettings(joystick);

        // Move robot
        moveOmni();

        // Check to see if we want to calibrate the gyro.
        checkCalibration();
    }
}

// Move the robot on the field
bool slowSpeedButtonWasPressed = false;
bool slowSpeedEnabled = false;
void moveOmni()
{
    // Don't move if we're calibrating the gyro
    if (calibrating)
        return;

    // Make things less sensitive around the center (a slight
    // dead-band), and more aggressive at the extremes.
    int xJoy1 = expoJoystick(joystick.joy1_x1);
    int yJoy1 = expoJoystick(joystick.joy1_y1);
    int xJoy2 = expoJoystick(joystick.joy1_x2);

    // Determine the magnitude and direction of the joystick from
    // the operator's perspective, where 0 degrees is going staight away.
    // This is overly complex, but at least I can understand it. - NWW
    int magnitude = sqrt(Power(xJoy1, 2) + Power(yJoy1, 2));
    int operatorHeading = 0;
    if (yJoy1 == 0) {
        // Since we divide by yJoy1 in atan, we can't tell the direction
        // when we have no up/down change.
        if (xJoy1 == 0)
            operatorHeading = 0;
        else if (xJoy1 > 0)
            operatorHeading = 90;
        else
            operatorHeading = -90;
    } else {
        // atan gives strange reading for negative numbers, so we'll
        // figure out what the real direction is after calculating the
        // angle if in the upper-right polar quadrant.
        int heading = radiansToDegrees(atan(abs((float)yJoy1 / (float)xJoy1)));

        // Convert the heading to be correct for the correct quadrant
        if (yJoy1 > 0) {
            if (xJoy1 > 0)
                operatorHeading = heading;
            else
                operatorHeading = -heading;
        } else {
            if (xJoy1 > 0)
                operatorHeading = 180 - heading;
            else
                operatorHeading = -(180 - heading);
        }
    }

    // Now that we have the operator's perspective, we need to determine
    // the translated power and direction based on the robot's
    // perspective.
    float robotHeading = operatorHeading - currHeading;

    // Ok, we have the direction of the joystick from the robot's
    // perspective, now we just need to convert the magnitude into
    // forward and right powers.
    float sinDir = sinDegrees(robotHeading);
    float cosDir = cosDegrees(robotHeading);

    // Simple math!
    int forward = (float)magnitude * sinDir;
    int right = (float)magnitude * cosDir;
    int rotCw = xJoy2;

    // Keep the heading offset in mind to see how
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

    if (max > 100) {
        // RobotC violates so many C-compiler rules
        // regarding conversions it's not funny.
        //
        // We have to cast the result to a float which is then changed
        // to a int due to the data type, otherwise the result stays an
        // int even with the explicit casts.
        float reduction = (float)100.0 / (float)max;
        flPow = (float)((float)flPow * reduction);
        frPow = (float)((float)frPow * reduction);
        rlPow = (float)((float)rlPow * reduction);
        rrPow = (float)((float)rrPow * reduction);
    }

    // Check the low-speed power setting.  If set, reduce power by half.
    bool btnPress = joy1Btn(9);
    if (!btnPress && slowSpeedButtonWasPressed) {
        // Beep to indicate a speed switch
        PlaySound(soundBlip);
        slowSpeedEnabled = !slowSpeedEnabled;
    }
    slowSpeedButtonWasPressed = btnPress;
    if (slowSpeedEnabled) {
        flPow /= 2;
        frPow /= 2;
        rlPow /= 2;
        rrPow /= 2;
    }
    nxtDisplayString(1, "SSE=%d", slowSpeedEnabled);

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
