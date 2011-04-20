#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTMotor,  HTServo)
#pragma config(Motor,  mtr_S1_C1_1,     mLTrack,       tmotorNormal, PIDControl, reversed, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     mRTrack,       tmotorNormal, PIDControl, encoder)
#pragma config(Motor,  mtr_S1_C2_1,     mBatonArm,     tmotorNormal, PIDControl, encoder)
#pragma config(Motor,  mtr_S1_C2_2,     mBridgeArm,    tmotorNormal, PIDControl, reversed, encoder)
#pragma config(Motor,  mtr_S1_C3_1,     mDispArm,      tmotorNormal, PIDControl, encoder)
#pragma config(Motor,  mtr_S1_C3_2,     mRGLiftArm,    tmotorNormal, PIDControl, reversed, encoder)
#pragma config(Servo,  srvo_S1_C4_1,    ,                     tServoStandard)
#pragma config(Servo,  srvo_S1_C4_3,    sBatonCup,            tServoStandard)
#pragma config(Servo,  srvo_S1_C4_4,    sRGTeethL,            tServoStandard)
#pragma config(Servo,  srvo_S1_C4_5,    sRGTeethR,            tServoStandard)
#pragma config(Servo,  srvo_S1_C4_6,    sDispCup,             tServoStandard)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*************************************************
* Code for Teleop Control
* By Bridger Howell
* Team 4309
**************************************************
* Controls
* --------
* Joystick 1:
*   Left Analog (TANK DRIVE MODE)
*      Controls the power and direction of left track motor (mLTrack)
*   Right Analog (TANK DRIVE MODE)
*      Controls the power and direction of right track motor (mRTrack)
* [[ OR ]]
*   Left Analog - Y-Axis (ARCADE TWOJOY DRIVE)
*      Controls the power (front/back) of both tracks (mlTrack & mRTrack)
*   Right Analog - X-Asix (ARCADE TWOJOY DRIVE)
*      Controls the direction of both tracks (mlTrack & mRTrack)
*   L1 {Upper Back Left}
*      Interrupts/reverses the rolling goal capture process to previous state
*         (mRGLiftArm & sRGTeethL/sRGTeethR)
*   R1 {Upper Back Right}
*      Initiates the rolling goal capture process
*         (mRGLiftArm & sRGTeethL/sRGTeethR)
* Joystick 2:
*   Left Analog
*      Y-Axis - Controls the dispensing arm (mDispArm)
*      X-Axis - Controls the dispensing cup rotation (sDispCup)
*   L1 {Upper Back Left}
*      Toggles between deploying and parking the bridge arm (mBridgeArm)
*   Right Analog
*      Controls the bridge arm (mBridgeArm)
*   R1 {Upper Back Right}
*      Toggles deployment of the baton/blocking arm (mBatonArm)
*   R2 {Lower Back Right}
*      Unloads the batons (sBatonCup)
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
* mDispArm
*      Controls the dispensing arm (front/center arm)
* mRGLiftArm
*      Controls the rolling goal lifting arm (rear/center arm)
**************************************************
* Servos
* ------
* sBatonCup
*      Controls the baton cup on baton arm (mBatonArm)
* sDispCup
*      Controls the cup on the dispensing arm
* sRGTeethL + sRGTeethR
*      Controls the teeth used in conjunction with rear arm (mRGLiftArm)
*/

#include "JoystickDriver.c"

// Mode for controlling the tank tracks
const int DRIVE_TANK          = 0;
const int DRIVE_ARCADE_ONEJOY = 1;
const int DRIVE_ARCADE_TWOJOY = 2;

int DRIVE_MODE = DRIVE_ARCADE_TWOJOY;

// The arm motors overshoot a bit, so until we write useful PID
// controllers for them, we consider 'zero' when moving back to the
// original location anything less than 150.
const int ARM_POS_ZERO_SLOP = 150;

//
// Baton/Blocking Arm constants (right arm)
//
const int BATON_ARM_DEPLOYED_POS = 680;

// Power to the baton arm
const int BATON_ARM_MOVE_POWER = 45;

// Baton dispenser open/close
const int BATON_DISPENSER_CLOSE = 244;
const int BATON_DISPENSER_OPEN = 104;

//
// Bridge Arm constants (left arm)
//

// How far to move the arm all the way out
const int BRIDGE_ARM_DEPLOYED_POS = 1200;

// Power to the bridge arm
// XXX - Unused, as the bridge arm is controlled by the joystick
const int BRIDGE_ARM_MOVE_POWER = 40;

//
// Dispenser Constants (front/center arm)
//

// XXX - How far to move the arm all the way out
// const int DISPENSER_ARM_DEPLOYED_POS = 1440 * 2;
    
// The dispenser cup's center position at start
const int DISPENSER_CUP_CENTER_POS = 0;

//
// Rolling Goal Arm constants (rear/center arm)
//

// 2 full rotations down from park (~90 degrees)!
const int RG_ARM_DROP_POS = 1440 * 2;

// How far to move up to 'lift' the rolling goal
const int RG_ARM_LIFT_AMT = 240;
const int RG_ARM_LIFT_POS = RG_ARM_DROP_POS - RG_ARM_LIFT_AMT;

// We don't need much power to move, but use it all for lifting the goal
const int RG_ARM_MOVE_POWER = 30;
const int RG_ARM_LIFT_POWER = 100;

// The maximum amount of time we'll use to 'lift' the goal at full power.
const int RG_ARM_MAX_LIFT_TIME = 5 * 1000;

// The servos that control the Rolling Goal are the 'teeth' the control
// the goal at the top
const int RG_TEETH_LEFT_UP = 220;
const int RG_TEETH_LEFT_DOWN = 40;
const int RG_TEETH_RIGHT_UP = 20;
const int RG_TEETH_RIGHT_DOWN = 200;

// Forward method declarations
void moveBatonArm();
void toggleBatonArm();
task BatonArmTask();

void moveBatonDrop();
void toggleBatonDrop();
/* unused
void closeBatonDrop();
void openBatonDrop();
*/
task BatonDropTask();

void moveBridgeArm();
void toggleBridgeArm();
task BridgeArmTask();

void moveDispenserArm();
void moveDispenserCup();

void moveRGLift();
void toggleRGLift();
void abortRGLift();
task RGLiftTask();

void moveTracks();

int calculateTetrixPower(int power, long remainDist);

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
    servo[sDispCup] = DISPENSER_CUP_CENTER_POS;

    // Startup the routines that control the different robot
    // attachments (arms, servos, etc..)
    StartTask(BatonArmTask);
    StartTask(BatonDropTask);
//    StartTask(BridgeArmTask);
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

        moveBatonArm();
        moveBatonDrop();
        moveDispenserCup();
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
    int nSpeedPower, nTurnPower;

    switch (DRIVE_MODE) {
    default:
    case DRIVE_TANK:
        // Make things less sensitive around the center (a slight
        // dead-band), and more aggressive at the extremes.
        lPow = expoJoystick(joystick.joy1_y1);
        rPow = expoJoystick(joystick.joy1_y2);
        break;

    case DRIVE_ARCADE_ONEJOY:
        // Make things less sensitive around the center (a slight
        // dead-band), and more aggressive at the extremes.
        nSpeedPower = expoJoystick(joystick.joy1_y1);
        nTurnPower = expoJoystick(joystick.joy1_x1);

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

    case DRIVE_ARCADE_TWOJOY:
        // Make things less sensitive around the center (a slight
        // dead-band), and more aggressive at the extremes.
        nSpeedPower = expoJoystick(joystick.joy1_y1);
        nTurnPower = expoJoystick(joystick.joy1_x2);

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
// Baton Arm Control (right arm)
//
bool batonArmButtonWasPressed = false;
void moveBatonArm()
{
    bool btnPress = joy2Btn(6);
    if (!btnPress && batonArmButtonWasPressed)
        toggleBatonArm();
    batonArmButtonWasPressed = btnPress;
}

typedef enum {
    BATON_PARKED,
    MOVE_OUT,
    BATON_DEPLOYED,
    MOVE_IN
} batonState;

batonState bState = BATON_PARKED;

void toggleBatonArm()
{
    int armPos = nMotorEncoder[mBatonArm];
    switch (bState) {
    case BATON_PARKED:
    case MOVE_IN:
        bState = MOVE_OUT;
        break;

    case MOVE_OUT:
    case BATON_DEPLOYED:
        bState = MOVE_IN;
        break;
    }
}

task BatonArmTask()
{
    // Reset the encoder.  Note, we assume the arm is tucked into the
    // robot at program start.
    nMotorEncoder[mBatonArm] = 0;
    while (true) {
        long armPos = abs(nMotorEncoder[mBatonArm]);

        switch (bState) {
        case BATON_PARKED:
        case BATON_DEPLOYED:
            motor[mBatonArm] = 0;
            break;

        case MOVE_OUT:
            motor[mBatonArm] = calculateTetrixPower(
                BATON_ARM_MOVE_POWER, abs(armPos - BATON_ARM_DEPLOYED_POS));
            if (armPos >= BATON_ARM_DEPLOYED_POS)
                bState = BATON_DEPLOYED;
            break;

        case MOVE_IN:
            motor[mBatonArm] = calculateTetrixPower(
                -BATON_ARM_MOVE_POWER, abs(armPos));
            if (armPos <= BATON_ARM_DEPLOYED_POS / 4)
                bState = BATON_PARKED;
            break;

        default:
            nxtDisplayString(3, "BATON ARM ERROR %d", bState);
            break;
        }
        EndTimeSlice();
    }
}

// Baton dropper
bool batonDropWasPressed = false;
void moveBatonDrop()
{
    bool btnPress = joy2Btn(8);
    if (!btnPress && batonDropWasPressed)
        toggleBatonDrop();
    batonDropWasPressed = btnPress;
}

bool batonDropClosed = true;
void toggleBatonDrop()
{
    batonDropClosed = !batonDropClosed;
}
/* unused
void closeBatonDrop()
{
    batonDropClosed = true;
}

void openBatonDrop()
{
    batonDropClosed = false;
}
*/
task BatonDropTask()
{
    while (true) {
        if (batonDropClosed)
            servo[sBatonCup] = BATON_DISPENSER_CLOSE;
        else
            servo[sBatonCup] = BATON_DISPENSER_OPEN;

        // Give the other threads a chance to run.
        EndTimeSlice();
    }
}

//
// Bridge Arm Controls (left arm)
//
bool bridgeArmButtonWasPressed = false;
void moveBridgeArm()
{
    if (true) {
    // Move Bridge Arm.  Don't let the arm move if we're at the endpoints
    int armPower = expoJoystick(joystick.joy2_y2);
    if (abs(armPower) > BRIDGE_ARM_MOVE_POWER) {
        if (armPower < 0)
            armPower = -BRIDGE_ARM_MOVE_POWER;
         else
            armPower = BRIDGE_ARM_MOVE_POWER;
    }
    if ((abs(nMotorEncoder[mBridgeArm]) <= 10 && armPower < 0) ||
        (abs(nMotorEncoder[mBridgeArm]) >= BRIDGE_ARM_DEPLOYED_POS && armPower > 0))
        motor[mBridgeArm] = 0;
    else
        motor[mBridgeArm] = armPower;
    } else {
    bool btnPress = joy2Btn(7);
    if (!btnPress && bridgeArmButtonWasPressed)
        toggleBridgeArm();
    bridgeArmButtonWasPressed = btnPress;
    }
}

typedef enum {
    BRIDGE_PARKED,
    BRIDGE_OUT,
    BRIDGE_DEPLOYED,
    BRIDGE_IN
} bridgeState;

bridgeState brState = BRIDGE_PARKED;

void toggleBridgeArm()
{
    int armPos = nMotorEncoder[mBridgeArm];
    switch (brState) {
    case BRIDGE_PARKED:
    case BRIDGE_IN:
        brState = BRIDGE_OUT;
        break;

    case BRIDGE_OUT:
    case BRIDGE_DEPLOYED:
        brState = BRIDGE_IN;
        break;
    }
}

task BridgeArmTask()
{
    // Reset the encoder.  Note, we assume the arm is tucked into the
    // robot at program start.
    nMotorEncoder[mBridgeArm] = 0;
    while (true) {
        long armPos = abs(nMotorEncoder[mBridgeArm]);

        switch (brState) {
        case BRIDGE_PARKED:
            // Keep the arm parked!
            if (armPos < 0)
                motor[mBridgeArm] = calculateTetrixPower(
                    -BRIDGE_ARM_MOVE_POWER, abs(armPos));
            else if armPos > BRIDGE_ARM_DEPLOYED_POS)
                motor[mBridgeArm] = calculateTetrixPower(
                    BRIDGE_ARM_MOVE_POWER, abs(armPos));
            else
                motor[mBridgeArm] = 0;
            break;
            
        case BRIDGE_DEPLOYED:
            // Keep the arm deployed!
            if (armPos > BRIDGE_ARM_DEPLOYED_POS)
                motor[mBridgeArm] = calculateTetrixPower(
                    -BRIDGE_ARM_MOVE_POWER,
                    abs(armPos - BRIDGE_ARM_DEPLOYED_POS));
            else if (armPos < (BRIDGE_ARM_DEPLOYED_POS - ARM_POS_ZERO_SLOP))
                motor[mBridgeArm] = calculateTetrixPower(
                    BRIDGE_ARM_MOVE_POWER, abs(ARM_POS_ZERO_SLOP));
            else
                motor[mBridgeArm] = 0;
            break;

        case BRIDGE_OUT:
            motor[mBridgeArm] = calculateTetrixPower(
                BRIDGE_ARM_MOVE_POWER, abs(armPos - BRIDGE_ARM_DEPLOYED_POS));
            if (armPos >= BRIDGE_ARM_DEPLOYED_POS)
                brState = BRIDGE_DEPLOYED;
            break;

        case BRIDGE_IN:
            motor[mBridgeArm] = calculateTetrixPower(
                -BRIDGE_ARM_MOVE_POWER, abs(armPos));
                brState = BRIDGE_PARKED;
            break;

        default:
            nxtDisplayString(3, "BRIDGE ARM ERROR %d", brState);
            break;
        }
        EndTimeSlice();
    }
}

//
// Dispenser Arm Controls (front/center)
//

// Moves the arm using the joystick controls
void moveDispenserArm()
{
    // Move Dispensing Arm.  Don't let the arm move if we're at the
    // endpoints.
    int armPower = expoJoystick(joystick.joy2_y1);
/*
//    int armPower = joystick.joy2_y1 * 100 / 127;
    if ((nMotorEncoder[mDispArm] <= 10 && armPower < 0) ||
        (nMotorEncoder[mDispArm] >= DISPENSER_ARM_DEPLOYED_POS && armPower > 0))
        motor[mDispArm] = 0;
    else
*/
        motor[mDispArm] = armPower;
}

void moveDispenserCup()
{
    int cupControl = expoJoystick(joystick.joy2_x1);
    if (cupControl > 10)
        servo[sDispCup] = ServoValue[sDispCup] - 5;
    else if (cupControl < -10)
        servo[sDispCup] = ServoValue[sDispCup] + 5;
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
    RAISE_ARM,
    RAISING_ARM,
    READY,
    START_CAPTURE,
    LOWER_TEETH,
    LIFT_GOAL,
    LOADED,
    UNLOAD_GOAL,
    RAISE_TEETH
} liftState;

liftState lState = PARKED;

void toggleRGLift()
{
    switch (lState) {
    case PARKED:
        lState = DROP_ARM;
        break;
    case DROP_ARM:
    case RAISE_ARM:
        // Ignore
        break;
    case READY:
        lState = START_CAPTURE;
        break;
    case START_CAPTURE:
    case LOWER_TEETH:
    case LIFT_GOAL:
        // Ignore
        break;
    case LOADED:
        lState = UNLOAD_GOAL;
        break;
    case UNLOAD_GOAL:
    case RAISE_TEETH:
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
        // Ignore
        break;
    case DROP_ARM:
    case READY:
        lState = RAISE_ARM;
        break;
    case START_CAPTURE:
    case LOWER_TEETH:
        lState = RAISE_TEETH;
        break;
    case LIFT_GOAL:
    case LOADED:
        lState = UNLOAD_GOAL;
        break;
    case UNLOAD_GOAL:
    case RAISE_TEETH:
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
            motor[mRGLiftArm] = calculateTetrixPower(
                RG_ARM_MOVE_POWER, abs(armPos - RG_ARM_DROP_POS));
            // XXX - Give us a bit of slop when parking so we don't
            // overshoot and jam the arm.
            if (armPos >= RG_ARM_DROP_POS) {
                motor[mRGLiftArm] = 0;
                lState = READY;
            }
            break;

        case RAISE_ARM:
            motor[mRGLiftArm] = calculateTetrixPower(
                -RG_ARM_MOVE_POWER, abs(armPos));
            if (armPos <= ARM_POS_ZERO_SLOP) {
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
            lState = LOWER_TEETH;
            break;

        case LOWER_TEETH:
            // Keep the teeth moving down!
            servo[sRGTeethL] = RG_TEETH_LEFT_DOWN;
            servo[sRGTeethR] = RG_TEETH_RIGHT_DOWN;
            if (ServoValue[sRGTeethL] == RG_TEETH_LEFT_DOWN &&
                ServoValue[sRGTeethR] == RG_TEETH_RIGHT_DOWN) {
                lState = LIFT_GOAL;
                // Keep track of how long we take lifting the goal
                time1[T1] = 0;
            }
            break;

        case LIFT_GOAL:
            // Keep the teeth down in case they want to move back up
            servo[sRGTeethL] = RG_TEETH_LEFT_DOWN;
            servo[sRGTeethR] = RG_TEETH_RIGHT_DOWN;

            // Keep lifting up the arm until we get there.  Note, we go
            // 100% full power here since we're trying to get the goal
            // up in the air (no need to slow down when we get close).
            // However, we'll only keep trying to lift it for
            // RG_ARM_MAX_LIFT_TIME to avoid burning out the motors.
            motor[mRGLiftArm] = -RG_ARM_LIFT_POWER;
            if (armPos <= RG_ARM_LIFT_POS ||
                time1[T1] >= RG_ARM_MAX_LIFT_TIME) {
                // Lifted!
                motor[mRGLiftArm] = 0;
                lState = LOADED;

                // Reset the time again if we've hit the target.
                if (time1[T1] < RG_ARM_MAX_LIFT_TIME)
                    time1[T1] = 0;
            }
            break;

        case LOADED:
            // Still forcing those teeth down!
            servo[sRGTeethL] = RG_TEETH_LEFT_DOWN;
            servo[sRGTeethR] = RG_TEETH_RIGHT_DOWN;

            // If the lifting arm sags, lift it back up with full power!
            if (armPos > RG_ARM_LIFT_POS) {
                // Only keep lifting if we were able to lift it!
                if (time1[T1] < RG_ARM_MAX_LIFT_TIME)
                    motor[mRGLiftArm] = -RG_ARM_LIFT_POWER;
                else
                    // Give up!
                    motor[mRGLiftArm] = 0;
            } else {
                motor[mRGLiftArm] = 0;

                // Motor turned off because we hit the target lift
                // position, so we can reset the lift time.
                time1[T1] = 0;
            }
            break;

        case UNLOAD_GOAL:
            // Keep droping the arm until we get low enough
            motor[mRGLiftArm] = calculateTetrixPower(
                RG_ARM_MOVE_POWER, abs(armPos - RG_ARM_DROP_POS));
            if (armPos >= RG_ARM_DROP_POS) {
                // Done lowering, so start moving the teeth back up
                motor[mRGLiftArm] = 0;
                servo[sRGTeethL] = RG_TEETH_LEFT_UP;
                servo[sRGTeethR] = RG_TEETH_RIGHT_UP;
                lState = RAISE_TEETH;
            }
            break;

        case RAISE_TEETH:
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

const int MIN_POWER = 10;
const int SLOW_START_DIST = 1000;

int calculateTetrixPower(int power, long remainDist)
{
    // We only do the calculations if the request is for more than MIN_POWER.
    if (abs(power) < MIN_POWER || remainDist > SLOW_START_DIST)
        return power;

    if (true) {
        // These numbers are determined via trial and error.  I'm sure their
        // is a better way of calculating them, probably using some
        // calculation that takes power into consideration.
        if (remainDist < 250)
            power = power / 2;
        else if (remainDist < 500)
            power = power * 3 / 4;
        else if (remainDist < 1000)
            power = power * 9 / 10;
    } else {
        // Limit ourself to at least MIN_POWER
        power = (int)((float)power * (float)remainDist /
                      (float)SLOW_START_DIST);
    }

    if (abs(power) < MIN_POWER) {
        if (power < 0)
            power = -MIN_POWER;
        else
            power = MIN_POWER;
    }
    return power;
}
