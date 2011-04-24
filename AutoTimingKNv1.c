#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTMotor,  HTServo)
#pragma config(Motor,  motorA,          mDispWristL,   tmotorNormal, PIDControl, encoder)
#pragma config(Motor,  motorB,          mDispWristR,   tmotorNormal, PIDControl, encoder)
#pragma config(Motor,  mtr_S1_C1_1,     mLTrack,       tmotorNormal, PIDControl, reversed, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     mRTrack,       tmotorNormal, PIDControl, encoder)
#pragma config(Motor,  mtr_S1_C2_1,     mBatonArm,     tmotorNormal, PIDControl, reversed, encoder)
#pragma config(Motor,  mtr_S1_C2_2,     mBridgeArm,    tmotorNormal, PIDControl, reversed, encoder)
#pragma config(Motor,  mtr_S1_C3_1,     mDispArm,      tmotorNormal, PIDControl, reversed, encoder)
#pragma config(Motor,  mtr_S1_C3_2,     mRGLiftArm,    tmotorNormal, PIDControl, reversed, encoder)
#pragma config(Servo,  srvo_S1_C4_1,    ,                     tServoStandard)
#pragma config(Servo,  srvo_S1_C4_3,    sBatonCup,            tServoStandard)
#pragma config(Servo,  srvo_S1_C4_4,    sRGTeethL,            tServoStandard)
#pragma config(Servo,  srvo_S1_C4_5,    sRGTeethR,            tServoStandard)
#pragma config(Servo,  srvo_S1_C4_6,    sDispCup,             tServoStandard)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// This is necessary so that waitForStart() communication from the
// Samantha Field Control is received properly.  Otherwise,
// waitForStart() is undefined.
#include "JoystickDriver.c"

// Shared code that both Auto and Teleop use
#include "SharedKNv1.c"

/**************************************************
 * Code for Autonomous Mode
 * By Bridger Howell
 * Team 4309
 **************************************************
 * Motors
 * ------
 * mLTrack
 *      Controls tank track movement (left track)
 * mRTrack
 *      Controls tank track movement (right track)
 * mBatonArm
 *      Controls the baton/blocking arm with the attached baton cup (right arm)
 * mBridgeArm
 *      Controls bridge lowering arm (left arm)
 **************************************************
 * Servos
 * ------
 * sBatonCup
 *      Controls the baton cup on baton arm (mBatonArm)
 **************************************************
 */

// Time for movement
const int FOOT_TIME_MS = 800;
const int FULL_TURN_TIME_MS = 4800;

// Power constants
const int STRAIGHT_POWER = 100;
const int TURN_POWER = 50;

// Move the robot
typedef enum {
    STRAIGHT,
    TURN,
} cmdState;

void move(cmdState cmd, float amt);
void moveWait(cmdState cmd, float amt);

// Usability functions
bool stillMoving();
int calcMove(float dist);
int calcTurn(float deg);
task MoveTask();

void initializeRobot()
{
    // The tracks are controlled via the joystick and have no background
    // tasks, so turn off the motors and initialize their encoders.
    motor[mLTrack] = 0;
    motor[mRTrack] = 0;
    nMotorEncoder[mLTrack] = 0;
    nMotorEncoder[mRTrack] = 0;

    // Set the dispenser cup to it's center position
    servo[sDispCup] = DISPENSER_CUP_AUTO_POS;

    // Lift up the rolling goal 'teeth'
    servo[sRGTeethL] = RG_TEETH_LEFT_UP;
    servo[sRGTeethR] = RG_TEETH_RIGHT_UP;

    // Startup the routines that control the different robot
    // attachments (arms, servos, etc..)
    StartTask(BatonArmTask);
    StartTask(BatonCupTask);
    StartTask(BridgeArmTask);
    StartTask(MoveTask);
}

//Autonomous code start
task main()
{
    // Setup the robot to be where we want it to be.
    initializeRobot();

    // Start when the teleop phase begins.
    waitForStart();

    //
    // Do the autonomous thing
    //
    
    // Move the dispenser arm to the correct height.
    dispenserArmAutoWait();

    // Move forward up to the rolling goal
    moveWait(STRAIGHT, 2.5);

    // Move the baton arm to help guide the goal
    deployBatonArmWait();

    // Turn 'left' to capture the goal
    moveWait(TURN, -45.0);

    // Knock the bridge down
    deployBridgeArmWait();

    // Deploy the batons and wait for them to drop
    openBatonCup();
    wait1Msec(500);

    // Move across the bridge far enough so the rolling goal falls to
    // the other side.
    moveWait(STRAIGHT, 3.0);

    // Start the cleanup routines
    closeBatonCup();
    parkBatonArm();
    parkBridgeArm();

    // Backup to the center of the bridge.
    moveWait(STRAIGHT, -1.5);

    // Hopefully we're balanced on the bridge now
}

typedef enum {
    STOP,
    FORWARD,
    MOVING,
    BACKWARD,
    TURN_LEFT,
    TURN_RIGHT,
} motorState;

motorState mState = STOP;
int motorTime;
int motorPower;

bool stillMoving()
{
    // This makes sure that we can read the other thread's update to
    // mState.
    hogCPU();
    bool result = (mState != STOP);
    releaseCPU();
    return result;
}

void move(cmdState cmd, float amt)
{
    // Nothing to do!
    if (amt == 0.0)
        return;

    // Make sure we can safely move!
    if (stillMoving()) {
        nxtDisplayString(1, "%d", "Motor still moving");
        return;
    }

    motorState nState;
    switch (cmd) {
    case STRAIGHT:
        motorPower = STRAIGHT_POWER;
        motorTime = calcMove(abs(amt));
        if (amt > 0)
            nState = FORWARD;
        else
            nState = BACKWARD;
        break;

    case TURN:
        // Normalize the output to -180 <- +180
        while (amt < -180)
            amt += 360;
        while (amt > 180)
            amt -= 360;
        motorPower = TURN_POWER;
        motorTime = calcTurn(abs(amt));
        if (amt > 0)
            nState = TURN_RIGHT;
        else
            nState = TURN_LEFT;
        break;
    }
    // Safely update the state!
    hogCPU();
    mState = nState;
    releaseCPU();
}

void moveWait(cmdState cmd, float amt)
{
    move(cmd, amt);
    while (stillMoving())
        EndTimeSlice();
}

task MoveTask()
{
    while (true) {
        switch (mState) {
        case STOP:
            // Shutoff the motors
            motor[mLTrack] = 0;
            motor[mRTrack] = 0;
            break;

        case FORWARD:
            // Both motors go forward
            ClearTimer(T4);
            motor[mLTrack] = motorPower;
            motor[mRTrack] = motorPower;
            mState = MOVING;
            break;

        case BACKWARD:
            // Both motors go backward
            ClearTimer(T4);
            motor[mLTrack] = -motorPower;
            motor[mRTrack] = -motorPower;
            mState = MOVING;
            break;

        case TURN_LEFT:
            // Motors reversed
            ClearTimer(T4);
            motor[mLTrack] = -motorPower;
            motor[mRTrack] = motorPower;
            mState = MOVING;
            break;

        case TURN_RIGHT:
            // Motors reversed
            ClearTimer(T4);
            motor[mLTrack] = motorPower;
            motor[mRTrack] = -motorPower;
            mState = MOVING;
            break;

        case MOVING:
            if (time1[T4] >= motorTime) {
                // This makes sure that we safely set the mState so that
                // other threads can read it.
                hogCPU();
                mState = STOP;
                releaseCPU();
            }
            break;
        }
    }
}

int calcMove(float dist)
{
    return (int)(dist * (float)FOOT_TIME_MS);
}

int calcTurn(float deg)
{
    // How much of a a full turn are we doing here?
    return (int)((float)FULL_TURN_TIME_MS * deg / 360.0);
}
