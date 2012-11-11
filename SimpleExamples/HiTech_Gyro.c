// Ignore warnings about unreferenced functions
#pragma systemFile

// The driver for the gyro
#include "drivers/hitechnic-gyro.h"

// Externally visible routines
void initializeGyro(tSensors gyro);
float getGyroHeading();
float getGyroRate();
void calibrateGyro();
bool calibratingActive();

// Internal routines
void _calibrateGyroDrift();
float _readGyro(bool counterDrift = true);
task _gyroTask();

// The gyro sensor
tSensors gyroSensor;

// Keeps track of the robot heading and rate
float currHeading;
float currRate;

void initializeGyro(tSensors gyro)
{
    gyroSensor = gyro;

    // Configure and calibrate the gyro, and then start the gyro task to
    // keep track of the robot's heading.
    currHeading = 0;
    currRate = 0;

    // This calibrates the gyro
    _calibrateGyroDrift();

    StartTask(_gyroTask);
}

float getGyroHeading()
{
    return currHeading;
}

float getGyroRate()
{
    return currRate;
}

// Allow the operator to re-synchronize the robot to 'north'.
bool calibrating = false;
void calibrateGyro()
{
    // If we're already calibrating (or will be shortly), don't bother
    // checking again.
    if (calibrating)
        return;

    calibrating = true;
}

bool calibratingActive()
{
    return calibrating;
}

// The value the gyro returns at zero.  We'll subtract this from the
// readings to give us a 'real' zero reading, which should avoid some of
// the drift we're seeing.
void _calibrateGyroDrift()
{
    // Re-calibrate and reset the heading to 0
    HTGYROstartCal(gyroSensor);
}

int _readGyro()
{
    return HTGYROreadRot(gyroSensor);
}

// Task to keep track of the current heading using the HT Gyro
task _gyroTask()
{
    long prevTime = nPgmTime;
    while (true)
    {
        // Check to see if we need to calibrate the gyro
        if (calibrating)
        {
            PlaySound(soundBeepBeep);

            // Turn off the motors so the gyro can synchronize
            disableMotors();

            // Wait for the robot to stop in case it was moving.
            wait1Msec(1 * 1000);

            // Re-calculate the drift and reset the heading to 0
            _calibrateGyroDrift();
            currHeading = 0;

            // Done calibrating
            PlaySound(soundBeepBeep);
            calibrating = false;
        }

        // This tells us the current rate of rotation in degrees per
        // second.
        int rateNow = _readGyro();

        // How much time has elapsed since we last checked, which we use
        // to determine how far we've turned
        long now = nPgmTime;

        // Filter out the high-frequency 'noise' and really weird spikes
        // caused from the sensor bouncing around.  Something better
        // would be nice, but this seems to work well enough in
        // practice.
        if (abs(rateNow) > 3)
        {
            // deltaSecs will only go negative if we've if nPgmTime
            // wrapped around.  If so, re-calculate the difference as by
            // incrementing both now and prevTime by a large number so
            // both will be wrapped, and then re-calculate the
            // difference.
            float deltaSecs = (now - prevTime) / 1000.0;
            if (deltaSecs < 0)
            {
                now += 1024;
                prevTime += 1024;
                deltaSecs = (now - prevTime) / 1000.0;
            }

            // Calculate how many degrees the heading changed.
            float degChange = rateNow * deltaSecs;
            float newHeading = currHeading + degChange;

            // Make sure we stay in +- 180 range.  Not necessary, but it
            // helps when debugging.
            while (newHeading > 180.0)
                newHeading -= 360.0;
            while (newHeading < -180.0)
                newHeading += 360.0;

            // Update the currHeading with the properly calculated
            // heading.
            currHeading = newHeading;

            // Keep track of the rate with a bit of hysteresis
            currRate = currRate * 0.5 + rateNow * 0.5;
            nxtDisplayString(6, "%3.1f, %3.1f", currHeading, currRate);
        }
        prevTime = now;

        // Wait a bit..
        EndTimeSlice();
    }
}
