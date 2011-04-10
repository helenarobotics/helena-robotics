#pragma config(Hubs,  S1, HTMotor,  HTServo,  HTMotor,  HTMotor)
#pragma config(Motor,  mtr_S1_C1_1,     mLTrack,       tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C1_2,     mRTrack,       tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C3_1,     mBlockAr,      tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C3_2,     mBridgeAr,     tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C4_1,     mDispAr,       tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C4_2,     mRGLiftAr,     tmotorNormal, openLoop)
#pragma config(Servo,  srvo_S1_C2_1,    sMouDisp,             tServoStandard)
#pragma config(Servo,  srvo_S1_C2_2,    sBatonCup,            tServoStandard)
#pragma config(Servo,  srvo_S1_C2_3,    sRoDisp,              tServoStandard)
#pragma config(Servo,  srvo_S1_C2_4,    sRGTeethL,            tServoStandard)
#pragma config(Servo,  srvo_S1_C2_5,    sRGTeethR,            tServoStandard)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/************************
* Code for Teleop Control
* By Bridger Howell
* Team 4309
*************************
* Motors-----------------
*************************
* mLTrack
*      Controls left side
*      of the track
* mRTrack
*      Controls right
*      side of the track
* mBlockAr
*      Controls the block
*      autonomous arm
* mBridgeAr
*      Controls bridge
*      lowering arm
* mDispAr
*      Controls the disp-
*      ensing arm
* mRGLiftAr
*      Controls the roll-
*      ing goal lifting
*      arm
*************************
* Servos-----------------
*************************
* sRGTeethL + sRGTeethR
*      Controls the teeth
*      for mRGLiftAr
* sBatonCup
*      Controls the baton
*      cup w/ mBlockAr
* sRoDisp
*      Controls the disp-
*      enser w/ mDispAr
* sMouDisp
*      Controls the disp-
*      ensing mouth w/get
*      mDispAr
**************************
*Controls-----------------
*Joystick 1---------------
* Left Analog
*    Controls the power
*    and postive/negative
*    direction of tracks
* Right Analog
*    Controls the directon
*    of movement of tracks
* R2 (Far Right Shoulder)
*    Toggles the rolling
*    goal lift arm and
*    lock teeth
*Joysitck 2---------------
* Left Analog
*    Controls the dispens-
*    ing arm
* Right Analog
*    Controls the bridge
*    arm
* L1 -Front Left Shoulder
*    Toggles dispenser
*    teeth position
* R1 -Front Right Shoulder
*    Toggles dispenser
*    mouth position
* R2 -Back Right Shoulder
*    Toggles Autonomous
*    baton arm position
*/

#include "JoystickDriver.c"

//Create & set flags
//Rolling Goal Flag
bool fiRGLift = false;
bool fiRGLiftAbort = false;

//Autonomous Block Arm Flag
bool fiBlockAr = false;

//Dispenser Flag
bool fiDisp = false;

//Dispenser Mouth Flag
bool fiDispMou = false;

//Time for RG Arm to drop [TWEAK]
const int RGARM_DROP_TIME = 4000;
const int RGARM_DROP_AMT = 1440 * 2;

//Time for RG Arm to lift [TWEAK]
const int RGARM_LIFT_TIME = 1000;
//Calculated based on 10 degrees
const int RGARM_LIFT_AMT = 160;

//Time for Autonomous Block Arm in milliseconds [TWEAK]
const int BA_TIME = 5000;

//Power for Motors [TWEAK]
const int BLOCK_ARM_POWER = 100;
const int RGLIFT_ARM_POWER = -30;
const int RGARM_LIFT_POWER = -100;

//Mode to drive; 0 is Tank, 1 is Arcade
const int MODE = 0;

//Servo Positions [TWEAK]
const int DISPENSER_MIN = 0;
const int DISPENSER_MAX = 140;
const int MOUTH_MIN = 0;
const int MOUTH_MAX = 140;
const int RGTEETH_RMIN = 200;
const int RGTEETH_RMAX = 0;
const int RGTEETH_LMIN = 40;
const int RGTEETH_LMAX = 240;

void toggleDispenser();
void dropDispenser();
void raiseDispenser();
task DispenseTask();

void toggleMouth();
void dropMouth();
void raiseMouth();
task MouthTask();

void toggleBlockArm();
void stopBlockArm();
task BlockTask();

void toggleRGLift();
void abortRGLift();
task RGLiftTask();

void initializeRobot()
{
    StartTask(DispenseTask);
    StartTask(MouthTask);
    StartTask(BlockTask);
    StartTask(RGLiftTask);
    nMotorEncoder[mRGLiftAr] = 0;
    motor[mLTrack] = 0;
    motor[mRTrack] = 0;
}

void moveDispenser()
{
    if (joy2Btn(5))
        fiDisp = true;
    else if (!joy2Btn(5) && fiDisp)
    {
        toggleDispenser();
        fiDisp = false;
    }
}

void moveDispMouth()
{
    if (joy2Btn(6))
        fiDispMou = true;
    else if (!joy2Btn(6) && fiDispMou)
    {
        toggleMouth();
        fiDispMou = false;
    }
}

void moveBlockArm()
{
    if (joy2Btn(8))
        fiBlockAr = true;
    else if (!joy2Btn(8) && fiBlockAr)
    {
        toggleBlockArm();
        fiBlockAr = false;
    }
}

void moveRGLift()
{
    if (joy1Btn(6))
        fiRGLift = true;
    else if (!joy1Btn(6) && fiRGLift)
    {
        toggleRGLift();
        fiRGLift = false;
    }

    if (joy1Btn(5))
        fiRGLiftAbort = true;
    else if (!joy1Btn(5) && fiRGLiftAbort)
    {
        abortRGLift();
        fiRGLiftAbort = false;
    }
}

void moveTracks()
{
    int rPow, lPow;

    //tank drive (above values set)
    if (MODE == 0)
    {
        rPow = (joystick.joy1_y1 * 100 / 127);
        lPow = (joystick.joy1_y2 * 100 / 127);
    } else if (MODE == 1)
    {
        //arcade drive
        float ratio;
        lPow = (joystick.joy1_y1 * 100 / 127);
        rPow = lPow;
        if (joystick.joy1_x2 < 0)
        {
            ratio = joystick.joy1_x2 * - 100 / 127;
            lPow -= ratio;
        }
        if (joystick.joy1_x2 > 0)
        {
            ratio = joystick.joy1_x2 * 100 / 127;
            rPow -= ratio;
        }
    }

    motor[mLTrack] = lPow;
    motor[mRTrack] = rPow;
}

void moveDispenseArm()
{
    //Move Dispensing Arm according to the y axis of the left analog on the 2nd joystick
    motor[mDispAr] = (joystick.joy2_y1 * 100 / 127);
}
void moveBridgeArm()
{
    //Move Bridge Arm according to the y axis of the right analog on the 2nd joystick
    motor[mBridgeAr] = (joystick.joy2_y2 * 100 / 127);
}



/* Main method of program-OBSOLETE----------------
 * 1. Create and initialize the Flags for Buttons.
 *    (Done at top of program)
 * 2. Run indefinate loop of the following:
 *    A. Get current joystick presses & movement
 *    B. Set the Flags created in step 1, based on
 *       the joysick configuration.
 *    C. Move the robot based on the flags and
 *       analog sticks.
 *         1.Move robot based on flags.
 *         2.Move robot based on analogs.
 */

//This contains the simple form of the above method.
task main()
{
    initializeRobot();

    // Start when the teleop phase begins.
    waitForStart();

    while (true) // Loop indefinately
    {
        //Get current joystick button and analog movement
        getJoystickSettings(joystick);

        //Move robot
        moveDispenser();
        moveDispMouth();
        moveBlockArm();
        moveRGLift();
        moveTracks();
        moveDispenseArm();
        moveBridgeArm();
    }
}

bool dispDown = true;
void toggleDispenser()
{
    dispDown = !dispDown;
}

void dropDispenser()
{
    dispDown = true;
}

void raiseDispenser()
{
    dispDown = false;
}

task DispenseTask()
{
    while (true)
    {
        if (dispDown)
            servo[sRoDisp] = DISPENSER_MIN;
        else
            servo[sRoDisp] = DISPENSER_MAX;
        wait1Msec(10);
    }
}

bool mouthDown = true;
void toggleMouth()
{
    mouthDown = !mouthDown;
}

void dropMouth()
{
    mouthDown = true;
}

void raiseMouth()
{
    mouthDown = false;
}

task MouthTask()
{
    while (true)
    {
        if (mouthDown)
            servo[sMouDisp] = MOUTH_MIN;
        else
            servo[sMouDisp] = MOUTH_MAX;
        wait1Msec(10);
    }
}

typedef enum
{
    NOT_MOVING,
    MOVE_LEFT,
    MOVING_LEFT,
    MOVE_RIGHT,
    MOVING_RIGHT
} blockState;

blockState bState = NOT_MOVING;
blockState lastDirection = MOVE_LEFT;

void toggleBlockArm()
{
    if (lastDirection == MOVE_LEFT)
        bState = MOVE_RIGHT;
    else
        bState = MOVE_LEFT;
}

void stopBlockArm()
{
    bState = NOT_MOVING;
}

task BlockTask()
{
    //XXX Keep track of how long the movement has progressed
    while (true)
    {
        switch (bState)
        {
        case NOT_MOVING:
            motor[mBlockAr] = 0;
            break;
        case MOVE_LEFT:
            time1[T1] = 0;
            motor[mBlockAr] = BLOCK_ARM_POWER;
            bState = MOVING_LEFT;
            lastDirection = MOVE_LEFT;
            break;
        case MOVING_LEFT:
            if (time1[T1] > BA_TIME)
                bState = NOT_MOVING;
            break;
        case MOVE_RIGHT:
            time1[T1] = 0;
            motor[mBlockAr] = -BLOCK_ARM_POWER;
            bState = MOVING_RIGHT;
            lastDirection = MOVE_RIGHT;
            break;
        case MOVING_RIGHT:
            if (time1[T1] > BA_TIME)
                bState = NOT_MOVING;
            break;
        default:
            nxtDisplayString(3, "BLOCK ARM ERROR %d", bState);
            break;
        }
        wait1Msec(10);
    }
}

typedef enum
{
    PARKED,
    DROP_ARM,
    DROPPING_ARM,
    RAISE_ARM,
    RAISING_ARM,
    READY,
    START_CAPTURE,
    LOWERING_TEETH,
    LIFT_RG,
    LIFTING_RG,
    LOADED,
    UNLOAD,
    LOWERING_RG,
    RAISING_TEETH
} liftState;

liftState lState = PARKED;

void toggleRGLift()
{
    switch (lState)
    {
    case PARKED:
        lState = DROP_ARM;
        break;
    case DROP_ARM:
    case DROPPING_ARM:
    case RAISE_ARM:
    case RAISING_ARM:
        //Ignore
        break;
    case READY:
        lState = START_CAPTURE;
        break;
    case START_CAPTURE:
    case LOWERING_TEETH:
    case LIFT_RG:
    case LIFTING_RG:
        //Ignore
        break;
    case LOADED:
        lState = UNLOAD;
        break;
    case UNLOAD:
    case LOWERING_RG:
    case RAISING_TEETH:
    default:
        //Ignore
        break;
    }
}

void abortRGLift()
{
    switch (lState)
    {
    case PARKED:
    case RAISE_ARM:
    case RAISING_ARM:
        //Ignore
        break;
    case DROP_ARM:
    case DROPPING_ARM:
    case READY:
        lState = RAISE_ARM;
        break;
    case START_CAPTURE:
    case LOWERING_TEETH:
        lState = RAISING_TEETH;
        break;
    case LIFT_RG:
    case LIFTING_RG:
    case LOADED:
        lState = UNLOAD;
        break;
    case UNLOAD:
    case LOWERING_RG:
    case RAISING_TEETH:
    default:
        //Ignore
        break;
    }
}

int hal = 0;

task RGLiftTask()
{
    while (true)
    {
        nxtDisplayString(0,"%d (%d) %d", lState, abs(nMotorEncoder[mRGLiftAr]), hal);
        switch (lState)
        {
        case PARKED:
            servo[sRGTeethL] = RGTEETH_LMAX;
            servo[sRGTeethR] = RGTEETH_RMAX;
            //Assume the RG Arm is up
            break;
        case DROP_ARM:
            time1[T2] = 0;
            motor[mRGLiftAr] = RGLIFT_ARM_POWER;
            lState = DROPPING_ARM;
            break;
        case DROPPING_ARM:
            if (time1[T2] > RGARM_DROP_TIME || abs(nMotorEncoder[mRGLiftAr]) >= RGARM_DROP_AMT)
            {
                hal = abs(nMotorEncoder[mRGLiftAr]);
                motor[mRGLiftAr] = 0;
                lState = READY;
            }
            break;
        case RAISE_ARM:
            time1[T2] = 0;
            motor[mRGLiftAr] = -RGLIFT_ARM_POWER;
            lState = RAISING_ARM;
            break;
        case RAISING_ARM:
            if (time1[T2] > RGARM_DROP_TIME || abs(nMotorEncoder[mRGLiftAr]) <= 150)
            {
                motor[mRGLiftAr] = 0;
                lState = PARKED;
            }
            break;
        case READY:
            servo[sRGTeethL] = RGTEETH_LMAX;
            servo[sRGTeethR] = RGTEETH_RMAX;
            //Assume the RG Arm is down
            break;
        case START_CAPTURE:
            servo[sRGTeethL] = RGTEETH_LMIN;
            servo[sRGTeethR] = RGTEETH_RMIN;
            lState = LOWERING_TEETH;
            break;
        case LOWERING_TEETH:
            servo[sRGTeethL] = RGTEETH_LMIN;
            servo[sRGTeethR] = RGTEETH_RMIN;
            if (ServoValue[sRGTeethL] == RGTEETH_LMIN && ServoValue[sRGTeethR] == RGTEETH_RMIN)
                lState = LIFT_RG;
            break;
        case LIFT_RG:
            servo[sRGTeethL] = RGTEETH_LMIN;
            servo[sRGTeethR] = RGTEETH_RMIN;
            time1[T2] = 0;
            motor[mRGLiftAr] = -RGARM_LIFT_POWER;
            lState = LIFTING_RG;
            break;
        case LIFTING_RG:
            servo[sRGTeethR] = RGTEETH_RMIN;
            servo[sRGTeethL] = RGTEETH_LMIN;
            if (time1[T2] > RGARM_LIFT_TIME || abs(nMotorEncoder[mRGLiftAr]) <= (RGARM_DROP_AMT - RGARM_LIFT_AMT))
            {
                motor[mRGLiftAr] = 0;
                lState = LOADED;
                time1[T2] = 0;
            }
            break;
        case LOADED:
            servo[sRGTeethR] = RGTEETH_RMIN;
            servo[sRGTeethL] = RGTEETH_LMIN;
            if (abs(nMotorEncoder[mRGLiftAr]) > (RGARM_DROP_AMT - RGARM_LIFT_AMT))
            {
                    motor[mRGLiftAr] = -RGARM_LIFT_POWER;
                } else {
                motor[mRGLiftAr] = 0;
            }
            break;
        case UNLOAD:
            time1[T2] = 0;
            motor[mRGLiftAr] = RGLIFT_ARM_POWER;
            lState = LOWERING_RG;
            break;
        case LOWERING_RG:
            if (time1[T2] > RGARM_LIFT_TIME || abs(nMotorEncoder[mRGLiftAr]) >= RGARM_DROP_AMT)
            {
                motor[mRGLiftAr] = 0;
                servo[sRGTeethR] = RGTEETH_RMAX;
                servo[sRGTeethL] = RGTEETH_LMAX;
                lState = RAISING_TEETH;
            }
            break;
        case RAISING_TEETH:
            if (ServoValue[sRGTeethR] == RGTEETH_RMAX && ServoValue[sRGTeethL] == RGTEETH_LMAX)
            {
                lState = READY;
            }
            break;
        default:
            nxtDisplayString(3, "RG ARM ERROR # %d", lState);
            break;
        }
    }
}
