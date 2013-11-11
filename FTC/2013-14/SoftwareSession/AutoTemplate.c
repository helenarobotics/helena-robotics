
// Forward declarations
void turnRight(int degrees);
void turnLeft(int degrees);

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

void moveStraight(int distanceInCm)
{
    // Move in one foot increments
    int power = STRAIGHT_MOTOR_POWER;
    if (distanceInCm < 0)
        power = -power;

    nMotorEncoder[motorA] = 0;
    nMotorEncoder[motorB] = 0;
    motor[motorA] = power;
    motor[motorB] = power;
    int ticks = TICKS_PER_CM * abs(distanceInCm) - TICKS_STRAIGHT_OVERRUN;
    while (abs(nMotorEncoder[motorA]) < ticks ||
           abs(nMotorEncoder[motorB]) < ticks)
    {
        if (abs(nMotorEncoder[motorA]) >= ticks)
            motor[motorA] = 0;
        if (abs(nMotorEncoder[motorB]) >= ticks)
            motor[motorB] = 0;
    }
    motor[motorA] = 0;
    motor[motorB] = 0;

    // Wait for the robot to quit moving
    wait1Msec(MOVE_WAIT_TIME);
}

void turnLeft(int degrees)
{
    // If they give a negative value, we're turning the other way
    if (degrees < 0)
    {
        turnRight(abs(degrees));
        return;
    }
    nMotorEncoder[motorA] = 0;
    nMotorEncoder[motorB] = 0;
    motor[motorA] = -TURN_MOTOR_POWER;
    motor[motorB] = TURN_MOTOR_POWER;
    int ticks = TICKS_PER_DEGREE * degrees - TICKS_TURN_OVERRUN;
    while (abs(nMotorEncoder[motorA]) < ticks ||
           abs(nMotorEncoder[motorB]) < ticks)
    {
        if (abs(nMotorEncoder[motorA]) >= ticks)
            motor[motorA] = 0;
        if (abs(nMotorEncoder[motorB]) >= ticks)
            motor[motorB] = 0;
    }
    motor[motorA] = 0;
    motor[motorB] = 0;

    // Wait for the robot to stop moving
    wait1Msec(MOVE_WAIT_TIME);
}

void turnRight(int degrees)
{
    // If they give a negative value, we're turning the other way
    if (degrees < 0)
    {
        turnLeft(abs(degrees));
        return;
    }
    nMotorEncoder[motorA] = 0;
    nMotorEncoder[motorB] = 0;
    motor[motorA] = TURN_MOTOR_POWER;
    motor[motorB] = -TURN_MOTOR_POWER;
    int ticks = TICKS_PER_DEGREE * degrees;
    while (abs(nMotorEncoder[motorA]) < ticks ||
           abs(nMotorEncoder[motorB]) < ticks)
    {
        if (abs(nMotorEncoder[motorA]) >= ticks)
            motor[motorA] = 0;
        if (abs(nMotorEncoder[motorB]) >= ticks)
            motor[motorB] = 0;
    }
    motor[motorA] = 0;
    motor[motorB] = 0;

    // Wait for the robot to stop moving
    wait1Msec(MOVE_WAIT_TIME);
}

task main()
{
    initializeRobot();

    // Place the cube in the scoring zone and move to the ramp
    moveStraight(45);
    turnLeft(45);
    moveStraight(1);
    moveStraight(-1);
    turnLeft(90);
    moveStraight(3);
    turnRight(90);
    moveStraight(2);
    turnRight(90);
    moveStraight(2);
}
