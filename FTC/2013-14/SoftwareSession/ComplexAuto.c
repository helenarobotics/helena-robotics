// These are robot specific
const int TICKS_PER_CM = 30;
const int TICKS_STRAIGHT_OVERRUN = 200;
const int TICKS_PER_DEGREE = 8 * 1080 / 360;
const int TICKS_TURN_OVERRUN = 35;

// If these are modified, the above may need to be altered slightly
const int STRAIGHT_MOTOR_POWER = 50;
const int TURN_MOTOR_POWER = 50;

// How long it takes the robot to quit moving after the motors are
// turned off
const int MOVE_WAIT_TIME = 200;

// Forward declarations
void turnClockwise(int degrees);
void driveStraight(int distanceInCm);

//
// Driving constants to help the robot drive consistently
//

// How often do we check for the motors being stalled or
// getting out of sync with left/right
const int DRIVE_CHECK_MS = 250;

// Ensure the encoders are within this many ticks of one another
const int ENCODER_SYNC_ERROR_TICKS = 50;

// How many ticks must the motors move every DRIVE_TICK_MS before we
// consider the motor 'moving' (ie' not stalled)
const int STALLED_TICKS = 10;

void driveForDistance(int targetLeftPower, int targetRightPower, int targetTicks)
{
    // Maximum/minimum allowed powers
    const int MAX_TARGET_POWER = 90;
    const int MIN_TARGET_POWER = 30;

    // We only allow modifying the power up to 10% to fix out-of-sync
    // encoders.
    const int MAX_SYNC_POWER_CHANGE = 10;

    // Keep track of the motor directions separately
    // (which makes the code easier to follow).
    int leftMotorDirection = 1;
    if (targetLeftPower < 0)
        leftMotorDirection = -1;
    targetLeftPower = abs(targetLeftPower);

    int rightMotorDirection = 1;
    if (targetRightPower < 0)
        rightMotorDirection = -1;
    targetRightPower = abs(targetRightPower);

    // Limit the target powers to give us some leeway to get the motors
    // synchronized.
    if (targetLeftPower > MAX_TARGET_POWER)
        targetLeftPower = MAX_TARGET_POWER;
    if (targetLeftPower < MIN_TARGET_POWER)
        targetLeftPower = MIN_TARGET_POWER;
    if (targetRightPower > MAX_TARGET_POWER)
        targetRightPower = MAX_TARGET_POWER;
    if (targetRightPower < MIN_TARGET_POWER)
        targetRightPower = MIN_TARGET_POWER;

    // Keep track of the each motor's current and previous encoder
    // values.
    int currLeftTicks, prevLeftTicks;
    currLeftTicks = prevLeftTicks = 0;
    int currRightTicks, prevRightTicks;
    currRightTicks = prevRightTicks = 0;

    // Reset the encoders and turn on the motors
    nMotorEncoder[motorA] = 0;
    nMotorEncoder[motorB] = 0;
    int currLeftPower = targetLeftPower;
    int currRightPower = targetRightPower;
    motor[motorA] = currLeftPower * leftMotorDirection;
    motor[motorB] = currRightPower * rightMotorDirection;

    // Clear the timer used to check for stalled motors or out-of-sync
    // left/right motor encoders.  We use the timer instead of the
    // nPgmTime variable to avoid dealing with 16-bit overflow values
    ClearTimer(T4);

    // Keep moving until both motors have moved the correct distance
    while (currLeftTicks < targetTicks || currRightTicks < targetTicks)
    {
        // If either one of the motors travelled enought distance turn
        // it off.
        currLeftTicks = abs(nMotorEncoder[motorA]);
        if (currLeftTicks >= targetTicks)
        {
            currLeftPower = 0;
            motor[motorA] = 0;
        }

        currRightTicks = abs(nMotorEncoder[motorB]);
        if (currRightTicks >= targetTicks)
        {
            currRightPower = 0;
            motor[motorB] = 0;
        }

        // Check for stalled/out-of-sync motors?
        if (time1[T4] >= DRIVE_CHECK_MS)
        {
            // Check if either of the motors is stalled.  If so,
            // shutdown both motors to be safe.
            if (abs(currLeftTicks - prevLeftTicks) < STALLED_TICKS ||
                abs(currRightTicks - prevRightTicks) < STALLED_TICKS)
            {
                // Turn off both motors and wait one second to see if
                // the stall clears.
                motor[motorA] = 0;
                motor[motorB] = 0;

                wait1Msec(1000);

                // Startup the motors again using the old powers!
                motor[motorA] = currLeftPower * leftMotorDirection;
                motor[motorB] = currRightPower * rightMotorDirection;
            }
            else if (currLeftPower != 0 && currRightPower != 0 &&
                     abs(currLeftTicks - currRightTicks) > ENCODER_SYNC_ERROR_TICKS)
            {
                // Both motors are running and the encoders are out of
                // sync.  To fix this, we need to either speed-up the
                // one with lower values, or slow-down the motor with
                // the larger values.
                //
                // We prefer speeding up the slower motor (since we
                // prefer faster robots vs slower robots) and if that
                // doesn't work, we'll slow down the faster motor.
                //
                // However, we have some rules to try and keep things
                // closer to target power values, and ensures we don't
                // end up completely turning off the motors to keep
                // things in sync.
                //
                // * If the faster motor has already been sped up, slow
                //   it down (try and keep us closer to the target
                //   motor values).
                // * Only speedup the slower motor if we haven't already
                //   sped it up the maximum amount
                // * If all else fails, slow down the faster motor only
                //   we haven't slowed it down the maximum amount
                if (currLeftTicks < currRightTicks)
                {
                    // Speed up left or slowdown right
                    if (currRightPower > targetRightPower)
                        currRightPower--;
                    else if (currLeftPower < targetLeftPower + MAX_SYNC_POWER_CHANGE)
                        currLeftPower++;
                    else if (currRightPower > targetRightPower - MAX_SYNC_POWER_CHANGE)
                        currRightPower--;
                }
                else
                {
                    // Speed up right or slowdown left
                    if (currLeftPower > targetLeftPower)
                        currLeftPower--;
                    else if (currRightPower < targetRightPower + MAX_SYNC_POWER_CHANGE)
                        currLeftPower++;
                    else if (currLeftPower > targetLeftPower - MAX_SYNC_POWER_CHANGE)
                        currLeftPower--;
                }

                // Update the motor powers with the updates values
                motor[motorA] = currLeftPower * leftMotorDirection;
                motor[motorB] = currRightPower * rightMotorDirection;
            }

            // Remember the current ticks so we can determine if
            // the motors are stalled next time.
            prevLeftTicks = currLeftTicks;
            prevRightTicks = currRightTicks;

            // Reset the timer
            ClearTimer(T4);
        }
    }

    // Turn off both motors
    motor[motorA] = 0;
    motor[motorB] = 0;

    // Wait for the robot to quit moving
    wait1Msec(MOVE_WAIT_TIME);
}

void driveStraight(int distanceInCm)
{
    // Move in one foot increments
    int power = STRAIGHT_MOTOR_POWER;
    if (distanceInCm < 0)
        power = -power;

    driveForDistance(power, power, TICKS_PER_CM * abs(distanceInCm) - TICKS_STRAIGHT_OVERRUN);
}

void turnClockwise(int degrees)
{
    int leftPower = TURN_MOTOR_POWER;
    if (degrees < 0)
        leftPower = -leftPower;

    driveForDistance(leftPower, -leftPower, TICKS_PER_DEGREE * abs(degrees) - TICKS_TURN_OVERRUN);
}

void initializeRobot()
{
    motor[motorA] = 0;
    motor[motorB] = 0;
}

task main()
{
    initializeRobot();

    // Place the cube in the scoring zone and move to the ramp
    driveStraight(45);
    turnClockwise(-45);
    driveStraight(1);
    driveStraight(-1);
    turnClockwise(-90);
    driveStraight(3);
    turnClockwise(90);
    driveStraight(2);
    turnClockwise(90);
    driveStraight(2);
}
