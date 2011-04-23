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

// Encoder clicks for movement

//
// The diameter of the tracked wheel is ~2.5", so circum == pi * d
// (~7.6"), so one foot ~1.53 revolutions.  One revolution of the Tetrix
// motor == 1440 ticks, so 1.53 * 1440 = 2200.  (This result was
// verified by manually running the robot 6' and measuring the number of
// encoder ticks).
// 1 foot in ticks = 1440 * 12" / (2.5" * pi);
const int FOOT_STRAIGHT_TICKS = 2200;

// Because we are using tank tranks for moving the robot, turning
// introduces a huge amount of track slippage.  Depending on how fast we
// go, the slippage changes significantly, which makes it difficult to
// calculate this number mathematically.  This value was determined
// emperically by running a number of turning tests using a consistent
// power setting and turning the robot through a circle and averaging
// the result.
const int RIGHT_FULL_TURN_TICKS = 8800;
const int LEFT_FULL_TURN_TICKS = 7800;

// How often do we check the encoders to make sure they are mostly in
// sync.
const long SYNC_CHECK_TIME = 250;

// We allow the motors to be at most this many ticks out of sync until
// we attempt to correct things by changing motor power.
const long SYNC_TICK_ERROR = 50;

//
// Power constants
//

// We've determined by experience that by setting both motors to full
// power, it keeps them more in sync with one another.
const int STRAIGHT_POWER = 100;

// Changing this will effect the FULL_TURN_TICKS value (determined
// experimentally), so make sure to change both numbers in parallel.
const int TURN_POWER = 50;

// How can we move the robot?
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

    // Startup the routines that control the different robot
    // attachments (arms, servos, etc..)
    StartTask(BatonArmTask);
    StartTask(BatonCupTask);
    StartTask(BridgeArmTask);
    StartTask(MoveTask);
}

// Autonomous code start
task main()
{
    // Setup the robot to be where we want it to be.
    initializeRobot();

    // Start when the teleop phase begins.
    waitForStart();

    // Do the autonomous thing
    moveWait(STRAIGHT, 2.5);

    // Move forward up to the rolling goal
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
int motorPower;
int motorTicks;

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
        motorTicks = calcMove(amt);
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
        motorTicks = calcTurn(amt);
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
    long nxtSyncTime = nPgmTime + SYNC_CHECK_TIME;
    int lTargetPow, rTargetPow;
    int lPow, rPow;

    // Make it so the display doesn't print out garbage.
    lPow = 0;
    rPow = 0;

    while (true) {
        int lPos = abs(nMotorEncoder[mLTrack]);
        int rPos = abs(nMotorEncoder[mRTrack]);
        nxtDisplayString(2, "L/R %d/%d (%d/%d)", lPos, rPos, lPow, rPow);
        switch (mState) {
        case STOP:
            // Shutoff the motors
            rPow = 0;
            lPow = 0;
            break;

        case FORWARD:
            // Both motors go forward
            nMotorEncoder[mLTrack] = 0;
            nMotorEncoder[mRTrack] = 0;
            lTargetPow = lPow = motorPower;
            rTargetPow = rPow = motorPower;
            nxtSyncTime = nPgmTime + SYNC_CHECK_TIME;
            mState = MOVING;
            break;

        case BACKWARD:
            // Both motors go backward
            nMotorEncoder[mLTrack] = 0;
            nMotorEncoder[mRTrack] = 0;
            lTargetPow = lPow = -motorPower;
            rTargetPow = rPow = -motorPower;
            nxtSyncTime = nPgmTime + SYNC_CHECK_TIME;
            mState = MOVING;
            break;

        case TURN_LEFT:
            // Motors reversed
            nMotorEncoder[mLTrack] = 0;
            nMotorEncoder[mRTrack] = 0;
            lTargetPow = lPow = -motorPower;
            rTargetPow = rPow = motorPower;
            nxtSyncTime = nPgmTime + SYNC_CHECK_TIME;
            mState = MOVING;
            break;

        case TURN_RIGHT:
            // Motors reversed
            nMotorEncoder[mLTrack] = 0;
            nMotorEncoder[mRTrack] = 0;
            lTargetPow = lPow = motorPower;
            rTargetPow = rPow = -motorPower;
            nxtSyncTime = nPgmTime + SYNC_CHECK_TIME;
            mState = MOVING;
            break;

        case MOVING:
            // Shutdown each motor individually to try and make sure we
            // keep the tracks 'sort of' in sync.
            if (lPos >= motorTicks)
                lPow = 0;
            if (rPos >= motorTicks)
                rPow = 0;

            // Did we make it the full distance for both motors?
            if (lPos >= motorTicks && rPos >= motorTicks) {
                // Full reverse on the motors for one cylce to stop us
                // quickly.  Note, by changing the state to STOP, the
                // next time through this loop we'll stop the motors.
                motor[mLTrack] = -lPow;
                motor[mRTrack] = -rPow;
//                wait1MSec(10);

                // This makes sure that we safely set the mState so that
                // other threads can read it.
                hogCPU();
                mState = STOP;
                releaseCPU();
            } else if (nPgmTime >= nxtSyncTime) {
                // See if we're far enough 'out of sync' to attempts
                // motor speed corrections.
                int avg = lPos + rPos / 2;
                if (abs(lPos - avg) > SYNC_TICK_ERROR ||
                    abs(rPos - avg) > SYNC_TICK_ERROR) {
                    // We default to slowing down the motors.  The only
                    // time we speed up motors is if they have been
                    // previously slowed.
                    if (rPos < lPos) {
                        if (rPow < rTargetPow) {
                            rPow++;
                        } else {
                            lPow--;
                        }
                    } else {
                        if (lPow < lTargetPow) {
                            lPow++;
                        } else {
                            rPow--;
                        }
                    }
                }
                // Wait a bit before checking the encoder positions.
                nxtSyncTime = nPgmTime + SYNC_CHECK_TIME;
            }
            break;
        }
        motor[mLTrack] = lPow;
        motor[mRTrack] = rPow;
    }
}

int calcMove(float dist)
{
    return abs((int)(dist * (float)FOOT_STRAIGHT_TICKS));
}

int calcTurn(float deg)
{
    // How much of a a full turn are we doing here?
    if (deg < 0)
        return abs((int)((float)LEFT_FULL_TURN_TICKS * deg / 360.0));
    else
        return abs((int)((float)RIGHT_FULL_TURN_TICKS * deg / 360.0));
}
