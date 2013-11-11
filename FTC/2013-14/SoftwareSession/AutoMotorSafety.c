// Forward declarations
void turnClockwise(int degrees);

// Motor safety check
const int CHECK_TIME_MS = 250;
const int MIN_TICKS_PER_CHECK = 36;

// These are robot specific
const int TICKS_PER_CM = 8 * 1440 / 260;
const int TICKS_STRAIGHT_OVERRUN = 200;
const int TICKS_PER_DEGREE = 117;
const int TICKS_TURN_OVERRUN = 35;

// If these are modified, the above may need to be altered slightly
const int STRAIGHT_MOTOR_POWER = 50;
const int TURN_MOTOR_POWER = 50;

const int MOVE_WAIT_TIME = 200;

void initializeRobot()
{
    motor[motorA] = 0;
    motor[motorB] = 0;
}



void driveForDistance(int leftMotorPowerTarget, int rightMotorPowerTarget, int ticks)
{
    int leftMotorPower = abs(leftMotorPowerTarget);
    int leftMotorPowerDirection = 1;
    if (leftMotorPowerTarget < 0)
        leftMotorPowerDirection = -1;

    int rightMotorPower = abs(rightMotorPowerTarget);
    int rightMotorPowerDirection = 1;
    if (rightMotorPowerTarget < 0)
        rightMotorPowerDirection = -1;

    motor[motorA] = leftMotorPower * leftMotorPowerDirection;
    motor[motorB] = rightMotorPower * rightMotorPowerDirection;

    nMotorEncoder[motorA] = 0;
    nMotorEncoder[motorB] = 0;

    // Motor safety and consistent encoder tick checking
    unsigned long nextCheckTime = nPgmTime + CHECK_TIME_MS;

    int leftMotorTicks = abs(nMotorEncoder[motorA]);
    int rightMotorTicks = abs(nMotorEncoder[motorB]);
    int prevCheckLeftTicks = leftMotorTicks;
    int prevCheckRightTicks = rightMotorTicks;
    while (leftMotorTicks < ticks || rightMotorTicks < ticks)
    {
        if (leftMotorTicks >= ticks)
            motor[motorA] = 0;
        if (rightMotorTicks >= ticks)
            motor[motorB] = 0;

        // Do we need to check for stalled motors?
        if (nPgmTime > nextCheckTime)
        {
            // Going backwards should never occur, but we handle it!
            int leftMoved = abs(leftMotorTicks - prevCheckLeftTicks);
            int rightMoved = abs(rightMotorTicks - prevCheckRightTicks);

            // Only checked for stalled motors if the motor is enabled
            if ((motor[motorA] > 0 && leftMoved < MIN_TICKS_PER_CHECK) ||
                (motor[motorB] > 0 && rightMoved < MIN_TICKS_PER_CHECK))
            {
                // Turn off the motors for a bit to see if it clearsn
                motor[motorA] = 0;
                motor[motorB] = 0;
                wait1Msec(500);
                motor[motorA] = leftMotorPower;
                motor[motorA] = rightMotorPower;
            }
            nextCheckTime = nPgmTime + CHECK_TIME_MS;
            prevCheckLeftTicks = leftMotorTicks;
            prevCheckRightTicks = rightMotorTicks;
        }
        leftMotorTicks = abs(nMotorEncoder[motorA]);
        rightMotorTicks = abs(nMotorEncoder[motorB]);
    }
    motor[motorA] = 0;
    motor[motorB] = 0;

    // Wait for the robot to quit moving
    wait1Msec(MOVE_WAIT_TIME);
}

void moveStraight(int distanceInCm)
{
    // Move in one foot increments
    int power = STRAIGHT_MOTOR_POWER;
    if (distanceInCm < 0)
        power = -power;

    driveForDistance(power, power, TICKS_PER_CM * abs(distanceInCm) - TICKS_STRAIGHT_OVERRUN);
}


void turnClockwise(int degrees)
{
    // Move in one foot increments
    int leftPower = TURN_MOTOR_POWER;
    if (degrees < 0)
        leftPower = -leftPower;

    driveForDistance(leftPower, -leftPower, TICKS_PER_DEGREE * abs(degrees) - TICKS_TURN_OVERRUN);
}

task main()
{
    initializeRobot();

    // Do the move thing..
    moveStraight(10);
    turnClockwise(90);
}
