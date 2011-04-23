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

/**************************************************
 * Code for Tele-Operated Mode
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
 *   Left Analog - Y-Axis (ARCADE ONEJOY & TWOJOY DRIVE)
 *      Controls the power (front/back) of both tracks (mLTrack & mRTrack)
 *   Left Analog - X-Axis (ARCADE ONEJOY DRIVE )
 *   Right Analog - X-Axis (ARCADE TWOJOY DRIVE)
 *      Controls the direction of both tracks (mLTrack & mRTrack)
 *   L1 {Upper Back Left}
 *      Interrupts/reverses the rolling goal capture process to previous state
 *         (mRGLiftArm & sRGTeethL/sRGTeethR)
 *   R1 {Upper Back Right}
 *      Initiates the rolling goal capture process
 *         (mRGLiftArm & sRGTeethL/sRGTeethR)
 *   CL {Center Button just Left of 'Logitech' Logo }
 *      Toggles slow-speed drive mode. When enabled, the robot moves at
 *      half-speed.
 *
 * Joystick 2:
 *   Left Analog
 *      Controls the dispensing arm wrist pos (mDispWristL/R)
 *
 *   L1 {Upper Back Left}
 *      Toggles between deploying and parking the bridge arm (mBridgeArm)
 *
 *   Right Analog
 *      Y-Axis - Controls the dispensing arm (mDispArm)
 *   Left HAT
 *      X-Axis - Controls the dispensing cup rotation (sDispCup)
 *      Y-Axis - Small adjustments to the preset dispensing arm pos (mDispArm)
 *   R1 {Upper Back Right}
 *      Toggles deployment of the baton/blocking arm (mBatonArm)
 *   R2 {Lower Back Right}
 *      Unloads the batons (sBatonCup)
 *   B4 {Button 4 on the right-portion of the controller}
 *      Moves the dispenser arm to the highest preset pos (mDispArm)
 *   B1/B3 {Button 1 & 3 on the right-portion of the controller}
 *      Moves the dispenser arm to the medium preset pos (mDispArm)
 *   B2 {Button 2 on the right-portion of the controller}
 *      Moves the dispenser arm to the lowest preset pos (mDispArm)
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
 * mDispWristL
 *      Controls the dispensing arm left wrist (front/center arm)
 * mDispWristR
 *      Controls the dispensing arm right wrist (front/center arm)
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

// Shared code that both Auto and Teleop use
#include "SharedKNv1.c"

// Mode for controlling the tank tracks
const int DRIVE_TANK          = 0;
const int DRIVE_ARCADE_ONEJOY = 1;
const int DRIVE_ARCADE_TWOJOY = 2;

int DRIVE_MODE = DRIVE_ARCADE_TWOJOY;

// The arm motors overshoot a bit, so until we write useful PID
// controllers for them, we consider 'zero' when moving back to a
// position anything less than this.
const int ARM_POS_ZERO_SLOP = 25;

//
// Dispenser Constants (front/center arm)
//

// How far to move the arm all the way up
const int DISPENSER_ARM_HIGHEST_POS = 3500;

// Power to the dispenser arm
const int DISPENSER_ARM_MOVE_POWER = 30;
const int DISPENSER_ARM_PRESET_MOVE_POWER = 10;

// The three preset heights for the dispenser arm
const int DISPENSER_ARM_HIGH_PRESET_POS = 3000;
const int DISPENSER_ARM_MED_PRESET_POS = 2000;
const int DISPENSER_ARM_LOW_PRESET_POS = 1000;

// The maximum amount we can 'tweak' the preset heights.
const int DISPENSER_ARM_TWEAK_LIMIT =
    (DISPENSER_ARM_HIGH_PRESET_POS - DISPENSER_ARM_MED_PRESET_POS) / 2;

// How close does the arm need to be to the setpoint to consider it
// 'good enough'?
const int DISPENSER_ARM_PRESET_SLOP = 20;

// The dispenser cup's center position at start
const int DISPENSER_CUP_CENTER_POS = 128;

// Dispenser arm 'wrist'
const int DISPENSER_WRIST_DEPLOYED_POS = 420;

// Power to the dispenser wrist
const int DISPENSER_WRIST_MOVE_UP_POWER = -40;
const int DISPENSER_WRIST_MOVE_DOWN_POWER = 25;

//
// Rolling Goal Arm constants (rear/center arm)
//

// 2 full rotations down from park (~90 degrees)!
const int RG_ARM_DROP_POS = 1440 * 2;

// How far to move up to 'lift' the rolling goal
const int RG_ARM_LIFT_AMT = 240;
const int RG_ARM_LIFT_POS = RG_ARM_DROP_POS - RG_ARM_LIFT_AMT;

// We don't need much power to move it up/down, but use it all for
// lifting the goal
const int RG_ARM_MOVE_POWER = 40;
const int RG_ARM_LIFT_POWER = 100;

// The maximum amount of time we'll use to 'lift' the goal at full
// power.  If we can't get there, this avoids burning up the motors.
const int RG_ARM_MAX_LIFT_TIME = 5 * 1000;

// The servos that control the Rolling Goal are the 'teeth' the control
// the goal at the top
const int RG_TEETH_LEFT_UP = 220;
const int RG_TEETH_LEFT_DOWN = 40;
const int RG_TEETH_RIGHT_UP = 20;
const int RG_TEETH_RIGHT_DOWN = 200;

// Forward method declarations
void moveTracks();

void moveDispenserControls();
task DispenserArmTask();

void moveDispenserWrist();

void moveRGLift();
void toggleRGLift();
void abortRGLift();
task RGLiftTask();

float Power(float num, int exp);
int expoJoystick(int eJoy);

void initializeRobot()
{
    // The tracks are controlled via the joystick and have no background
    // tasks, so turn off the motors and initialize their encoders.
    motor[mLTrack] = 0;
    motor[mRTrack] = 0;
    nMotorEncoder[mLTrack] = 0;
    nMotorEncoder[mRTrack] = 0;

    // Dispenser arm setup. Turn off the motor and reset the encoder,
    // and then sync the left/right motors together, so we only have to
    // control the left motor.
    motor[mDispWristL] = 0;
    nMotorEncoder[mDispWristL] = 0;
    nSyncedMotors = synchAB;

    // Startup the routines that control the different robot
    // attachments (arms, servos, etc..)
    StartTask(BatonArmTask);
    StartTask(BatonCupTask);
    StartTask(BridgeArmTask);
    StartTask(DispenserArmTask);
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
        // Get current joystick buttons and analog movements
        getJoystickSettings(joystick);

        // Move robot
        moveTracks();
        moveBridgeArm();
        moveDispenserControls();
        moveDispenserWrist();

        moveBatonArm();
        moveBatonCup();
        moveRGLift();
    }
}

//
// Move the robot!
//

bool slowSpeedButtonWasPressed = false;
bool slowSpeedEnabled = false;
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

        // Unused, but avoids a calculation later
        nTurnPower = 0;
        break;

    case DRIVE_ARCADE_ONEJOY:
        // Make things less sensitive around the center (a slight
        // dead-band), and more aggressive at the extremes.
        nSpeedPower = expoJoystick(joystick.joy1_y1);
        nTurnPower = expoJoystick(joystick.joy1_x1);

        // Power and speed
        lPow = nSpeedPower + nTurnPower;
        rPow = nSpeedPower - nTurnPower;
        break;

    case DRIVE_ARCADE_TWOJOY:
        // Make things less sensitive around the center (a slight
        // dead-band), and more aggressive at the extremes.
        nSpeedPower = expoJoystick(joystick.joy1_y1);
        nTurnPower = expoJoystick(joystick.joy1_x2);

        // Power and speed
        lPow = nSpeedPower + nTurnPower;
        rPow = nSpeedPower - nTurnPower;
        break;
    }

    // Reduce turning power at speed by 5%
    if (nTurnPower != 0 && abs(nSpeedPower) > 30) {
        lPow -= nTurnPower / 20;
        rPow += nTurnPower / 20;
    }

    // Check the low-speed power setting.  If set, reduce power by half.
    bool btnPress = joy1Btn(9);
    if (!btnPress && slowSpeedButtonWasPressed)
        slowSpeedEnabled = !slowSpeedEnabled;
    slowSpeedButtonWasPressed = btnPress;
    if (slowSpeedEnabled) {
        lPow /= 2;
        rPow /= 2;
    }

    // Limit power to +-100
    lPow = BOUND(lPow, -100, 100);
    rPow = BOUND(rPow, -100, 100);

    nxtDisplayString(1, "E L/R %d/%d",
                     nMotorEncoder[mLTrack], nMotorEncoder[mRTrack]);
    nxtDisplayString(2, "P L/R %d/%d", lPow, rPow);

    motor[mLTrack] = lPow;
    motor[mRTrack] = rPow;
}

//
// Dispenser Arm Controls (front/center)
//
typedef enum {
    DISPENSER_JOYSTICK,
    DISPENSER_LOW_PRESET,
    DISPENSER_MED_PRESET,
    DISPENSER_HIGH_PRESET,
} dispState;

dispState dState = DISPENSER_JOYSTICK;
int tweakDispArmAmt = 0;

void moveDispenserControls()
{
    // Move Dispensing Arm.

    // Are any of the preset height buttons being pressed.  If they
    // are, assume we want to go to a 'preset' height.
    if (joy2Btn(4) == 1) {
        dState = DISPENSER_HIGH_PRESET;
    } else if (joy2Btn(1) == 1 || joy2Btn(3) == 1) {
        dState = DISPENSER_MED_PRESET;
    } else if (joy2Btn(2) == 1) {
        dState = DISPENSER_LOW_PRESET;
    } else {
        // Read the joystick and check if we're using joystick control!
        int armPower = expoJoystick(joystick.joy2_y2);
        if (abs(armPower) > 0 && dState != DISPENSER_JOYSTICK)
            dState = DISPENSER_JOYSTICK;

        // We only allow the joystick to move the robot if we're in
        // joystick control.  Otherwise we'll fight the preset code and
        // turn the motors off.
        if (dState == DISPENSER_JOYSTICK) {
            // Don't let the arm move if we're at the endpoints.
            int armPos = nMotorEncoder[mDispArm];
            if ((armPos <= 10 && armPower < 0) ||
                (armPos >= DISPENSER_ARM_HIGHEST_POS && armPower > 0)) {
                motor[mDispArm] = 0;
            } else {
                // Limit the power.
                motor[mDispArm] =
                    BOUND(armPower,
                          -DISPENSER_ARM_MOVE_POWER , DISPENSER_ARM_MOVE_POWER);
            }
        }
    }

    // Controls the Dispenser cup rotation and allows for small
    // movements of the dispenser arm when we use the preset height
    // buttons.  Note, the rotation controls are much more forgiving
    // than the dispenser arm tweak controls, which must be an exact hit
    // since the former are expected to be used more often.
    int dispenseCmd = joystick.joy2_TopHat;
    switch (dispenseCmd) {
    case 0:
        // Tweak the dispenser arm up if we're not in JOYSTICK mode
        if (dState != DISPENSER_JOYSTICK)
            tweakDispArmAmt++;
        break;

    case 4:
        // Tweak the dispenser arm down.
        if (dState != DISPENSER_JOYSTICK)
            tweakDispArmAmt--;
        break;

    case 1:
    case 2:
    case 3:
        // Rotate the cup counter-clockwise
        servo[sDispCup] = ServoValue[sDispCup] - 5;
        break;

    case 5:
    case 6:
    case 7:
        // Rotate the cup clockwise
        servo[sDispCup] = ServoValue[sDispCup] + 5;
        break;

    case -1:
    default:
        // Ignored
        break;
    }

    // Limit the tweak amount.
    tweakDispArmAmt =
        BOUND(tweakDispArmAmt,
              -DISPENSER_ARM_TWEAK_LIMIT, DISPENSER_ARM_TWEAK_LIMIT);
}

task DispenserArmTask()
{
    // Set the dispenser cup to it's center position
    servo[sDispCup] = DISPENSER_CUP_CENTER_POS;

    // Turn off the motor and reset the encoder.  Note, we assume the
    // arm is tucked into the robot at program start.
    motor[mDispArm] = 0;
    nMotorEncoder[mDispArm] = 0;
    while (true) {
        long targetPos = -1;

        switch (dState) {
        case DISPENSER_JOYSTICK:
            // The user is in control, so we don't have a target.
            break;

        case DISPENSER_LOW_PRESET:
            // Move to and hold the low arm position
            targetPos = DISPENSER_ARM_LOW_PRESET_POS;
            break;

        case DISPENSER_MED_PRESET:
            // Move to and hold the medium arm position
            targetPos = DISPENSER_ARM_MED_PRESET_POS;
            break;

        case DISPENSER_HIGH_PRESET:
            // Move to and hold the high arm position
            targetPos = DISPENSER_ARM_HIGH_PRESET_POS;
            break;
        }

        // If we're not doing joystick, then we're trying to set the
        // position.
        if (dState != DISPENSER_JOYSTICK) {
            // Include any minor changes to the arm height
            targetPos += tweakDispArmAmt;

            // Ignore the command if the target is already at the bottom
            // or top of the arm's range, or if we're 'close enough' to
            // the target.  The targetPos can only be too big or too
            // small if they user continually continues to tweak the
            // height adjustment beyond a reasonable amount.
            long armPos = nMotorEncoder[mDispArm];
            if (targetPos <= 0 || targetPos >= DISPENSER_ARM_HIGHEST_POS ||
                abs(armPos - targetPos) <= DISPENSER_ARM_PRESET_SLOP) {
                // Turn off the motor
                motor[mDispArm] = 0;
            } else {
                // Gotta move to get there!
                int armPower = calculateTetrixPower(
                    DISPENSER_ARM_PRESET_MOVE_POWER, targetPos - armPos);
                if (targetPos > armPos)
                    // It takes more power to go up vs. down!
                    motor[mDispArm] = armPower + 4;
                else
                    motor[mDispArm] = -armPower;
            }
        }
        EndTimeSlice();
    }
}

void moveDispenserWrist()
{
    // Allow joystick movements.
    int armPower = -expoJoystick(joystick.joy2_y1);
    int armPos = nMotorEncoder[mDispWrist];

    // Limit the amount of power allowed.
    armPower = BOUND(armPower, DISPENSER_WRIST_MOVE_UP_POWER,
                     DISPENSER_WRIST_MOVE_DOWN_POWER);
    if ((armPos <= 5 && armPower < 0) ||
        (armPos >= DISPENSER_WRIST_DEPLOYED_POS && armPower > 0))
        motor[mDispWristL] = 0;
    else
        motor[mDispWristL] = armPower;
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
    // Turn off the motor and reset the encoder.  Note, we assume the
    // arm is tucked into the robot at program start.
    motor[mRGLiftArm] = 0;
    nMotorEncoder[mRGLiftArm] = 0;
    while (true) {
        long armPos = nMotorEncoder[mRGLiftArm];
        nxtDisplayString(0,"%d (%d) %d", lState, armPos);

        switch (lState) {
        case PARKED:
            servo[sRGTeethL] = RG_TEETH_LEFT_UP;
            servo[sRGTeethR] = RG_TEETH_RIGHT_UP;
            // Assume the RG Arm is still up
            break;

        case DROP_ARM:
            motor[mRGLiftArm] = calculateTetrixPower(
                RG_ARM_MOVE_POWER, armPos - RG_ARM_DROP_POS);
            if (armPos >= RG_ARM_DROP_POS) {
                motor[mRGLiftArm] = 0;
                lState = READY;
            }
            break;

        case RAISE_ARM:
            motor[mRGLiftArm] = calculateTetrixPower(
                -RG_ARM_MOVE_POWER, armPos);
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
                RG_ARM_MOVE_POWER, armPos - RG_ARM_DROP_POS);
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

// These give us a nice exponential band to make using the robots
// controls easier.
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
