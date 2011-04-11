#pragma config(Hubs,  S1, HTMotor,  HTServo,  HTMotor,  HTMotor)
#pragma config(Motor,  mtr_S1_C1_1,     mLTrack,       tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C1_2,     mRTrack,       tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C3_1,     mBlockArm,      tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C3_2,     mBridgeArm,     tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C4_1,     mDispArm,       tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C4_2,     mRGLiftArm,     tmotorNormal, openLoop)
#pragma config(Servo,  srvo_S1_C2_1,    sDispMouth,           tServoStandard)
#pragma config(Servo,  srvo_S1_C2_2,    sDispTeeth,           tServoStandard)
#pragma config(Servo,  srvo_S1_C2_3,    sBatonCup,            tServoStandard)
#pragma config(Servo,  srvo_S1_C2_4,    sRGTeethL,            tServoStandard)
#pragma config(Servo,  srvo_S1_C2_5,    sRGTeethR,            tServoStandard)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*************************************************
* Code for Teleop Control
* By Bridger Howell
* Team 4309
**************************************************
* Motors
* ------
* mLTrack
*      Controls left side of the track
* mRTrack
*      Controls right side of the track
* mBlockArm
*      Controls the block arm with the attached baton cup (left)
* mBridgeArm
*      Controls bridge lowering arm (left)
* mDispArm
*      Controls the dispensing arm (front/center)
* mRGLiftArm
*      Controls the rolling goal lifting arm (rear/center)
**************************************************
* Servos
* ------
* sRGTeethL + sRGTeethR
*      Controls the teeth used in conjunction with mRGLiftArm
* sBatonCup
*      Controls the baton cup in conjunction mBlockArm
* sDispTeeth
*      Controls the dispenser teeth in conjunction with sDispMouth & mDispArm
* sDispMouth
*      Controls the dispensing mouth in conjunction with sDispTeeth & mDispArm
**************************************************
* Controls
* --------
* Joystick 1:
*   Left Analog
*      Controls the power and direction of left track motor (mLTrack)
*   Right Analog
*      Controls the power and direction of right track motor (mRTrack)
*   L1 {Upper Back Left}
*      Aborts the rolling goal process before it completes
*         (mRGLiftArm &  sRGTeethL/sRGTeethR)
*   R1 {Upper Back Right}
*      Toggles the rolling goal lift arm and lock teeth
*         (mRGLiftArm & sRGTeethL/sRGTeethR)
* Joysitck 2:
*   Left Analog
*      Controls the dispensing arm (mDispArm)
*   L1 {Upper Back Left}
*      Toggles dispenser teeth position (sDispTeeth)
*   L2 {Lower Back Left}
*      Toggles dispenser mouth position (sDispMouth)
*   Right Analog
*      Controls the bridge arm (mBridgeArm)
*   R1 {Upper Back Right}
*      Toggles deployment of the blocking/baton arm (mBlockArm)
*   R2 {Lower Back Right}
*      Unloads the batons (sBatonCup)
*/

// XXX - Need BatonCup code

#include "JoystickDriver.c"

// Mode to drive
const int DRIVE_TANK_LINEAR   = 0;
const int DRIVE_TANK_EXPO     = 1;
const int DRIVE_ARCADE_LINEAR = 2;
const int DRIVE_ARCADE_EXPO   = 3;

int DRIVE_MODE = DRIVE_TANK_LINEAR;

// The arm motors overshoot a bit, so until we write useful PID
// controllers for them, we consider 'zero' when moving back to the
// original location anything less than 150.
const int ARM_ZERO_SLOP = 150;

//
// Blocking Arm constants
//
const int BLOCK_ARM_DEPLOYED_POS = 1440 * 2;

// Power to the blocking arm
const int BLOCK_ARM_MOVE_POWER = 30;

//
// Bridge Arm constants
//

// How far to move the arm all the way out
const int BRIDGE_ARM_DEPLOYED_POS = 1440 * 2;

/*
  Unused, as the bridge arm is controlled by the joystick
// Power to the blocking arm
const int BRIDGE_ARM_MOVE_POWER = 30;
*/

//
// Dispenser Constants
//

// How far to move the arm all the way out
const int DISPENSER_ARM_DEPLOYED_POS = 1440 * 2;

// Servo 'teeth'
const int DISPENSER_TEETH_DOWN = 0;
const int DISPENSER_TEETH_UP = 140;

// Servo 'mouth'
const int DISPENSER_MOUTH_OPEN = 0;
const int DISPENSER_MOUTH_CLOSED = 140;

//
// Rolling Goal Arm constants
//

// 2 full rotations down from park (~90 degrees)!
const int RG_ARM_DROP_POS = 1440 * 2;

// How far to move up to 'lift' the rolling goal
const int RG_ARM_LIFT_AMT = 160;
const int RG_ARM_LIFT_POS = RG_ARM_DROP_POS - RG_ARM_LIFT_AMT;

// We don't need much power to move, but use it all for lifting the goal
const int RG_ARM_MOVE_POWER = -30;
const int RG_ARM_LIFT_POWER = -100;

// The servos that control the Rolling Goal are the 'teeth' the control
// the goal at the top
const int RG_TEETH_LEFT_UP = 40;
const int RG_TEETH_LEFT_DOWN = 240;
const int RG_TEETH_RIGHT_UP = 200;
const int RG_TEETH_RIGHT_DOWN = 0;

// Forward method declarations
void moveBlockArm();
void toggleBlockArm();
task BlockArmTask();

void moveBridgeArm();

void moveDispenserArm();

void moveDispenserMouth();
void toggleDispenserMouth();
/*
void closeDispenserMouth();
void openDispenserMouth();
*/
task DispenserMouthTask();

void moveDispenserTeeth();
void toggleDispenserTeeth();
/*
void closeDispenserTeeth();
void openDispenserTeeth();
*/
task DispenserTeethTask();

void moveRGLift();
void toggleRGLift();
void abortRGLift();
task RGLiftTask();

void moveTracks();

void initializeRobot()
{
    // Turn off the motors
    motor[mLTrack] = 0;
    motor[mRTrack] = 0;

    // The bridge arm, dispenser arm, and tracks are all controlled via
    // the joystick and have no background tasks.

    // Reset the encoders on the joystick controlled arms.
    nMotorEncoder[mBridgeArm] = 0;
    nMotorEncoder[mDispArm] = 0;

    // Startup the routines that control the different robot
    // attachments (arms, servos, etc..)
    StartTask(BlockArmTask);
    StartTask(DispenserMouthTask);
    StartTask(DispenserTeethTask);
    StartTask(RGLiftTask);

}

/* Main method of program
 * 1. Run indefinite loop of the following:
 *    A. Get current joystick settings.
 *    B. Check the settings to determine the robots movements/behavior
 *    C. Have the robot perform the duties (either directly, or
 *       indirectly inside the tasks).
 */
task main()
{
    // Setup the robot to be where we want it to be.
    initializeRobot();

    // Start when the teleop phase begins.
    waitForStart();

    // Loop indefinitely
    while (true) {
        // Get current joystick button and analog movement
        getJoystickSettings(joystick);

        // Move robot
        moveBridgeArm();
        moveDispenserArm();

        moveBlockArm();
        moveDispenserMouth();
        moveDispenserTeeth();
        moveRGLift();
        moveTracks();
    }
}

// These give us a nice exponential band to make using the robots
// controls easier.
float Power(float num, int exp) {
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

// Move the robot!
void moveTracks()
{
    int rPow, lPow;

    switch (DRIVE_MODE) {
    default:
    case DRIVE_TANK_LINEAR:
        rPow = joystick.joy1_y1 * 100 / 127;
        lPow = joystick.joy1_y2 * 100 / 127;
        break;

    case DRIVE_TANK_EXPO:
        // Make things less sensitive around the center (a slight
        // dead-band), and more aggressive at the extremes.
        rPow = expoJoystick(joystick.joy1_y1);
        lPow = expoJoystick(joystick.joy1_y2);
        break;

    case DRIVE_ARCADE_LINEAR:
        // Simplification of Bridger's code - NWW
        lPow = (joystick.joy1_y1 * 100 / 127);
        rPow = lPow;
        lPow -= joystick.joy1_x2 * 100 / 127;
        break;

    case DRIVE_ARCADE_EXPO:
        // Make things less sensitive around the center (a slight
        // dead-band), and more aggressive at the extremes.
        int nSpeedPower = expoJoystick(joystick.joy1_y1);
        int nTurnPower = expoJoystick(joystick.joy1_x2);

        // Power and speed
        lPow = nSpeedPower - nTurnPower;
        rPow = nSpeedPower + nTurnPower;

        // Reduce turning power at speed by 5%
        if (abs(nSpeedPower) > 30) {
            lPow += nTurnPower / 20;
            rPow -= nTurnPower / 20;
        }

        // XXX - If we're in slow speed mode, reduce power by half
        if (false) {
            lPow /= 2;
            rPow /= 2;
        }
        break;
    }

    motor[mLTrack] = lPow;
    motor[mRTrack] = rPow;
}

//
// Blocking Arm Control (right arm)
//
bool blockArmButtonWasPressed = false;
void moveBlockArm()
{
    bool btnPress = joy2Btn(8);
    if (!btnPress && blockArmButtonWasPressed)
        toggleBlockArm();
    blockArmButtonWasPressed = btnPress;
}

typedef enum {
    BLOCK_PARKED,
    MOVE_LEFT,
    BLOCK_DEPLOYED,
    MOVE_RIGHT
} blockState;

blockState bState = BLOCK_PARKED;

void toggleBlockArm()
{
    switch (blockState) {
    case BLOCK_PARKED:
    case MOVE_RIGHT:
        bState = MOVE_LEFT;
        break;

    case MOVE_LEFT:
    case BLOCK_DEPLOYED:
        bState = MOVE_RIGHT;
        break;
    }
}

task BlockArmTask()
{
    // Reset the encoder.  Note, we assume the arm is tucked into the
    // robot at program start.
    nMotorEncoder[mBlockArm] = 0;
    while (true) {
        long armPos = abs(nMotorEncoder[mBlockArm]);

        switch (bState) {
        case BLOCK_PARKED:
        case BLOCK_DEPLOYED:
            motor[mBlockArm] = 0;
            break;

        case MOVE_LEFT:
            motor[mBlockArm] = BLOCK_ARM_MOVE_POWER;
            if (armPos >= BLOCK_ARM_DEPLOYED_POS)
                bState = BLOCK_DEPLOYED;
            break;

        case MOVE_RIGHT:
            motor[mBlockArm] = -BLOCK_ARM_MOVE_POWER;
            if (armPos <= ARM_ZERO_SLOP)
                bState = BLOCK_PARKED;
            break;

        default:
            nxtDisplayString(3, "BLOCK ARM ERROR %d", bState);
            break;
        }
        EndTimeSlice();
    }
}

//
// Bridge Arm Controls (left arm)
//
void moveBridgeArm()
{
    // Move Bridge Arm.  Don't let the folks move the arm if we're at
    // the end
//    int armPower = expoJoystick(joystick.joy2_y2);
    int armPower = joystick.joy2_y2 * 100 / 127;
    if ((nMotorEncoder[mBridgeArm] <= 10 && power < 0) ||
        (nMotorEncoder[mBridgeArm] >= BRIDGE_ARM_DEPLOYED_POS && power > 0))
        motor[mBridgeArm] = 0;
    else
        motor[mBridgeArm] = armPower;
}

//
// Dispenser Arm Controls (front/center)
//

// Moves the arm using the joystick controls
void moveDispenserArm()
{
    // Move Dispensinge Arm.  Don't let the folks move the arm if we're at
    // the end
//    int armPower = expoJoystick(joystick.joy2_y1);
    int armPower = joystick.joy2_y1 * 100 / 127;
    if ((nMotorEncoder[mDispArm] <= 10 && power < 0) ||
        (nMotorEncoder[mDispArm] >= DISPENSE_ARM_DEPLOYED_POS && power > 0))
        motor[mDispArm] = 0;
    else
        motor[mDispArm] = armPower;
}

// Dispenser Mouth Open/Closed
bool dispMouthButtonWasPressed = false;
void moveDispenserMouth()
{
    bool btnPress = joy2Btn(7);
    if (!btnPress && dispMouthButtonWasPressed)
        toggleDispenserMouth();
    dispMouthButtonWasPressed = btnPress;
}

// Dispenser mouth routines
bool dispMouthClosed = false;
void toggleDispenserMouth()
{
    dispMouthClosed = !dispMouthClosed;
}
/*
void closeDispenserMouth()
{
    dispMouthClosed = true;
}

void openDispenserMouth()
{
    dispMouthClosed = false;
}
*/
task DispenserMouthTask()
{
    while (true) {
        if (dispMouthClosed)
            servo[sDispMouth] = DISPENSER_MOUTH_CLOSED;
        else
            servo[sDispMouth] = DISPENSER_MOUTH_OPEN;

        EndTimeSlice();
    }
}

// Dispenser teeth raise/lower routines
bool dispTeethButtonWasPressed = false;
void moveDispenserTeeth()
{
    bool btnPress = joy2Btn(5);
    if (!btnPress && dispTeethButtonWasPressed)
        toggleDispenserTeeth();
    dispTeethButtonWasPressed = btnPress;
}

bool dispTeethDown = true;
void toggleDispenserTeeth()
{
    dispTeethDown = !dispTeethDown;
}
/*
void closeDispenserTeeth()
{
    dispTeethDown = true;
}

void openDispenserTeeth()
{
    dispTeethDown = false;
}
*/
task DispenserTeethTask()
{
    while (true) {
        if (dispTeethDown)
            servo[sDispTeeth] = DISPENSER_TEETH_DOWN;
        else
            servo[sDispTeeth] = DISPENSER_TEETH_UP;

        // Give the other threads a chance to run.
        EndTimeSlice();
    }
}

// Rolling Goal has two control buttons that affect each other.  One for
// 'continuing' the normal process, and the other for 'aborting' a
// in-progress process.
bool rgLiftButtonWasPressed = false;
bool rgAbortButtonWasPressed = false;
void moveRGLift()
{
    bool btnPress = joy1Btn(6);
    if (!btnPress && rgLiftButtonWasPressed)
        toggleRGLift();
    rgLiftButtonWasPressed = btnPress;

    btnPress = joy1Btn(5);
    if (!btnPress && rgAbortButtonWasPressed)
        abortRGLift();
    rgAbortButtonWasPressed = btnPress;
}

typedef enum {
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
    switch (lState) {
    case PARKED:
        lState = DROP_ARM;
        break;
    case DROP_ARM:
    case DROPPING_ARM:
    case RAISE_ARM:
    case RAISING_ARM:
        // Ignore
        break;
    case READY:
        lState = START_CAPTURE;
        break;
    case START_CAPTURE:
    case LOWERING_TEETH:
    case LIFT_RG:
    case LIFTING_RG:
        // Ignore
        break;
    case LOADED:
        lState = UNLOAD;
        break;
    case UNLOAD:
    case LOWERING_RG:
    case RAISING_TEETH:
    default:
        // Ignore
        break;
    }
}

void abortRGLift()
{
    switch (lState) {
    case PARKED:
    case RAISE_ARM:
    case RAISING_ARM:
        // Ignore
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
        // Ignore
        break;
    }
}

task RGLiftTask()
{
    // Reset the encoder.  Note, we assume the arm is in the 'PARKED'
    // position at program start.
    nMotorEncoder[mRGLiftArm] = 0;
    while (true) {
        long armPos = abs(nMotorEncoder[mRGLiftArm]);
        nxtDisplayString(0,"%d (%d) %d", lState, armPos);

        switch (lState) {
        case PARKED:
            servo[sRGTeethL] = RG_TEETH_LEFT_UP;
            servo[sRGTeethR] = RG_TEETH_RIGHT_UP;
            // Assume the RG Arm is still up
            break;

        case DROP_ARM:
            motor[mRGLiftArm] = RG_ARM_MOVE_POWER;
            lState = DROPPING_ARM;
            break;

        case DROPPING_ARM:
            if (armPos >= RG_ARM_DROP_POS) {
                motor[mRGLiftArm] = 0;
                lState = READY;
            }
            break;

        case RAISE_ARM:
            motor[mRGLiftArm] = -RG_ARM_MOVE_POWER;
            lState = RAISING_ARM;
            break;

        case RAISING_ARM:
            // XXX - Give us a bit of slop when parking so we don't
            // overshoot and jam the arm.
            if (armPos <= ARM_ZERO_SLOP) {
                motor[mRGLiftArm] = 0;
                lState = PARKED;
            }
            break;

        case READY:
            // Assume the RG Arm is still down
            servo[sRGTeethL] = RG_TEETH_LEFT_UP;
            servo[sRGTeethR] = RG_TEETH_RIGHT_UP;
            break;

        case START_CAPTURE:
            // Move the teeth down
            servo[sRGTeethL] = RG_TEETH_LEFT_DOWN;
            servo[sRGTeethR] = RG_TEETH_RIGHT_DOWN;
            lState = LOWERING_TEETH;
            break;

        case LOWERING_TEETH:
            // Keep the teeth moving down!
            servo[sRGTeethL] = RG_TEETH_LEFT_DOWN;
            servo[sRGTeethR] = RG_TEETH_RIGHT_DOWN;
            // Once the teeth are down get ready to lift up the arm.
            if (ServoValue[sRGTeethL] == RG_TEETH_LEFT_DOWN &&
                ServoValue[sRGTeethR] == RG_TEETH_RIGHT_DOWN)
                lState = LIFT_RG;
            break;

        case LIFT_RG:
            // Keep the teeth down in case they want to move back up
            servo[sRGTeethL] = RG_TEETH_LEFT_DOWN;
            servo[sRGTeethR] = RG_TEETH_RIGHT_DOWN;

            // Lift up the arm!
            motor[mRGLiftArm] = -RG_ARM_LIFT_POWER;
            lState = LIFTING_RG;
            break;

        case LIFTING_RG:
            // Again, keep those teeth down!
            servo[sRGTeethL] = RG_TEETH_LEFT_DOWN;
            servo[sRGTeethR] = RG_TEETH_RIGHT_DOWN;

            // Are we done lifting?
            if (armPos <= RG_ARM_LIFT_POS) {
                // Lifted!
                motor[mRGLiftArm] = 0;
                lState = LOADED;
            }
            break;

        case LOADED:
            // Still forcing those teeth down!
            servo[sRGTeethL] = RG_TEETH_LEFT_DOWN;
            servo[sRGTeethR] = RG_TEETH_RIGHT_DOWN;

            // If the lifting arm sags, lift it back up!
            if (armPos > RG_ARM_LIFT_POS)
                motor[mRGLiftArm] = -RG_ARM_LIFT_POWER;
            else
                motor[mRGLiftArm] = 0;
            break;

        case UNLOAD:
            // Get ready to drop the goal
            motor[mRGLiftArm] = RG_ARM_MOVE_POWER;
            lState = LOWERING_RG;
            break;

        case LOWERING_RG:
            if (armPos >= RG_ARM_DROP_POS) {
                // Done lowering, so start moving the teeth back up
                motor[mRGLiftArm] = 0;
                servo[sRGTeethL] = RG_TEETH_LEFT_UP;
                servo[sRGTeethR] = RG_TEETH_RIGHT_UP;
                lState = RAISING_TEETH;
            }
            break;

        case RAISING_TEETH:
            // Keeping those teeth moving up
            servo[sRGTeethL] = RG_TEETH_LEFT_UP;
            servo[sRGTeethR] = RG_TEETH_RIGHT_UP;
            if (ServoValue[sRGTeethL] == RG_TEETH_LEFT_UP &&
                ServoValue[sRGTeethR] == RG_TEETH_RIGHT_UP)
                lState = READY;
            break;

        default:
            nxtDisplayString(3, "RG ARM ERROR # %d", lState);
            break;
        }
    }
}
