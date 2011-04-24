// this eliminates warning for "unreferenced" functions
#pragma systemFile

/**************************************************
 * Shared Code used by both Teleop and Autonomous
 * By Bridger Howell
 * Team 4309
 **************************************************
 */

// Determine the length of an array
#define ARRAYSIZE(a)        (sizeof(a)/sizeof((a)[0]))

//
// The BOUND macro limits the input (n) inside the bounds between the
// provided low (l) and high (h) limits.
//
#define BOUND(n, l, h)      (((n) < (l))? (l): ((n) > (h))? (h): (n))

//
// The DEADBANK macro ignores inputs (n) that are within the deadbank
// threshold (t).  This often used when reading analog joysticks which
// rarely center at zero.  Therefore, if the joystick is within the
// threshold, we will consider it 'zero'.  The 'expoJoystick' method
// performs a similar function, but we may need a larger deadband for
// certain controls.
//
#define DEADBAND(n,t)       ((abs(n) > (t))? (n): 0)

// The arm motors overshoot a bit, so until we write useful PID
// controllers for them, we consider 'zero' when moving back to a
// position anything less than this.
const int ARM_POS_ZERO_SLOP = 25;

//
// Baton/Blocking Arm constants (right arm)
//
const int BATON_ARM_DEPLOYED_POS = 760;

// Power to the baton arm
const int BATON_ARM_MOVE_POWER = 30;

// Baton dispenser open/close
const int BATON_DISPENSER_CLOSE = 244;
const int BATON_DISPENSER_OPEN = 104;

//
// Bridge Arm constants (left arm)
//

// How far to move the arm all the way out
const int BRIDGE_ARM_DEPLOYED_POS = 1320;

// Power to the bridge arm
const int BRIDGE_ARM_MOVE_POWER = 40;

//
// Dispenser Constants (front/center arm)
//

// How far to move the arm all the way up
const int DISPENSER_ARM_HIGHEST_POS = 3500;

// Power to the dispenser arm
const int DISPENSER_ARM_MOVE_POWER = 30;
const int DISPENSER_ARM_PRESET_MOVE_POWER = 10;

// The dispenser arm height in autonomous mode
const int DISPENSER_ARM_AUTO_PRESET_POS = 1650;

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

// The dispenser cup's start position at auto and teleop
const int DISPENSER_CUP_AUTO_POS = 0;
const int DISPENSER_CUP_TELEOP_POS = 216;

//
// Rolling Goal Arm constants (rear/center arm)
//

// The servos that control the Rolling Goal are the 'teeth' the control
// the goal at the top
const int RG_TEETH_LEFT_UP = 220;
const int RG_TEETH_LEFT_DOWN = 40;
const int RG_TEETH_RIGHT_UP = 20;
const int RG_TEETH_RIGHT_DOWN = 200;

// Forward method declarations
void moveBatonArm();
void toggleBatonArm();
void deployBatonArm();
void parkBatonArm();
void deployBatonArmWait();
void parkBatonArmWait();
task BatonArmTask();

void moveBatonCup();
void toggleBatonCup();
void closeBatonCup();
void openBatonCup();
task BatonCupTask();

void moveBridgeArm();
void deployBridgeArm();
void parkBridgeArm();
void deployBridgeArmWait();
void parkBridgeArmWait();
void toggleBridgeArm();
task BridgeArmTask();

void dispenserArmAuto();
void dispenserArmAutoWait();
void dispenserArmAutoParkWait();

void tweakDispArmDown();
void tweakDispArmUp();
task DispenserArmTask();

// XXX - This should be doing PID calculations
int calculateTetrixPower(int power, long remainDist);

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

void deployBatonArm()
{
    bState = MOVE_OUT;
}

void parkBatonArm()
{
    bState = MOVE_IN;
}

void deployBatonArmWait()
{
    deployBatonArm();
    while (bState != BATON_DEPLOYED)
        EndTimeSlice();
}

void parkBatonArmWait()
{
    parkBatonArm();
    while (bState != BATON_PARKED)
        EndTimeSlice();
}

task BatonArmTask()
{
    // Turn off the motor and reset the encoder.  Note, we assume the
    // arm is tucked into the robot at program start.
    motor[mBatonArm] = 0;
    nMotorEncoder[mBatonArm] = 0;
    while (true) {
        long armPos = nMotorEncoder[mBatonArm];
        long targetPos = -1;
        switch (bState) {
        case MOVE_OUT:
            if (armPos >= BATON_ARM_DEPLOYED_POS)
                bState = BATON_DEPLOYED;
            // fall through
        case BATON_DEPLOYED:
            targetPos = BATON_ARM_DEPLOYED_POS;
            break;

        case MOVE_IN:
            if (armPos <= ARM_POS_ZERO_SLOP)
                bState = BATON_PARKED;
            // fall through
        case BATON_PARKED:
            targetPos = 0;
            break;

        default:
            nxtDisplayString(3, "BATON ARM ERROR %d", bState);
            break;
        }

        // Do we need to move the arm?
        if (targetPos >= 0) {
            // No need to do anything if we're 'close enough' to the
            // target.
            if (abs(armPos - targetPos) <= ARM_POS_ZERO_SLOP) {
                // Turn off the motor
                motor[mBatonArm] = 0;
            } else {
                // Gotta move to get there!
                int armPower = calculateTetrixPower(
                    BATON_ARM_MOVE_POWER, targetPos - armPos);
                // XXX - Check if these are correct for this motor?
                if (targetPos > armPos)
                    motor[mBatonArm] = armPower;
                else
                    motor[mBatonArm] = -armPower;
            }
        }
        EndTimeSlice();
    }
}

// Baton dropper
bool batonCupWasPressed = false;
void moveBatonCup()
{
    bool btnPress = joy2Btn(8);
    if (!btnPress && batonCupWasPressed)
        toggleBatonCup();
    batonCupWasPressed = btnPress;
}

bool batonCupClosed = true;
void toggleBatonCup()
{
    batonCupClosed = !batonCupClosed;
}

void closeBatonCup()
{
    batonCupClosed = true;
}

void openBatonCup()
{
    batonCupClosed = false;
}

task BatonCupTask()
{
    while (true) {
        if (batonCupClosed)
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
typedef enum {
    BRIDGE_PARKED,
    BRIDGE_OUT,
    BRIDGE_DEPLOYED,
    BRIDGE_IN
} bridgeState;

bridgeState brState = BRIDGE_PARKED;

bool bridgeArmButtonWasPressed = false;
void moveBridgeArm()
{
    // Check the bridge deployment button
    bool btnPress = joy2Btn(5);
    if (!btnPress && bridgeArmButtonWasPressed)
        toggleBridgeArm();
    bridgeArmButtonWasPressed = btnPress;
}

void deployBridgeArm()
{
    brState = BRIDGE_OUT;
}

void parkBridgeArm()
{
    brState = BRIDGE_IN;
}

void deployBridgeArmWait()
{
    deployBridgeArm();
    while (brState != BRIDGE_DEPLOYED)
        EndTimeSlice();
}

void parkBridgeArmWait()
{
    parkBridgeArm();
    while (brState != BRIDGE_PARKED)
        EndTimeSlice();
}

void toggleBridgeArm()
{
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
    // Turn off the motor and reset the encoder.  Note, we assume the
    // arm is tucked into the robot at program start.
    motor[mBridgeArm] = 0;
    nMotorEncoder[mBridgeArm] = 0;
    while (true) {
        long armPos = nMotorEncoder[mBridgeArm];
        long targetPos = -1;

        switch (brState) {
        case BRIDGE_IN:
            if (armPos <= ARM_POS_ZERO_SLOP) {
                brState = BRIDGE_PARKED;
                // Reverse the motor a bit to reduce slop, per a posting
                // my Dick Swan (author or RobotC for NXT).
                motor[mBridgeArm] = 2;
            }
            // Fall through
        case BRIDGE_PARKED:
            // Keep the arm at the parked position!
            targetPos = 0;
            break;

        case BRIDGE_OUT:
            if (armPos >= BRIDGE_ARM_DEPLOYED_POS)
                brState = BRIDGE_DEPLOYED;
            // Fall through
        case BRIDGE_DEPLOYED:
            // We allow the users to move the bridge arm in the deployed
            // position, so we can't set a target position and hold it
            // here.
            targetPos = BRIDGE_ARM_DEPLOYED_POS;
            break;

        default:
            nxtDisplayString(3, "BRIDGE ARM ERROR %d", brState);
            break;
        }

        // Do we need to move the arm?
        if (targetPos >= 0) {
            // No need to do anything if we're 'close enough' to the
            // target.
            if (abs(armPos - targetPos) <= ARM_POS_ZERO_SLOP) {
                // Turn off the motor
                motor[mBridgeArm] = 0;
            } else {
                // Gotta move to get there!
                int armPower = calculateTetrixPower(
                    BRIDGE_ARM_MOVE_POWER, targetPos - armPos);
                if (targetPos > armPos)
                    motor[mBridgeArm] = armPower;
                else
                    motor[mBridgeArm] = -armPower;
            }
        }
        EndTimeSlice();
    }
}

//
// Dispenser Arm Controls (front/center)
//
typedef enum {
    DISPENSER_PARKED,
    DISPENSER_JOYSTICK,
    DISPENSER_AUTO_PRESET,
    DISPENSER_LOW_PRESET,
    DISPENSER_MED_PRESET,
    DISPENSER_HIGH_PRESET,
} dispState;

dispState dState = DISPENSER_PARKED;

void dispenserArmAuto()
{
    // Move Dispensing Arm to the preset height used in auto.
    dState = DISPENSER_AUTO_PRESET;
}

void dispenserArmAutoWait()
{
    dispenserArmAuto();

    // We assume that the previous position is parked (zero), so we wait
    // until the arm gets there.
    int armPos = nMotorEncoder[mDispArm];
    while (armPos < DISPENSER_ARM_AUTO_PRESET_POS) {
        EndTimeSlice();
        armPos = nMotorEncoder[mDispArm];
    }
}

void dispenserArmAutoParkWait()
{
    // Move Dispensing Arm to the parked position in preperation for the
    // teleop phase.
    dState = DISPENSER_PARKED;

    // Wait until we get close to the zero position.
    int armPos = nMotorEncoder[mDispArm];
    while (armPos > 20) {
        EndTimeSlice();
        armPos = nMotorEncoder[mDispArm];
    }
}

int tweakDispArmAmt = 0;

void tweakDispArmDown()
{
    tweakDispArmAmt--;
}

void tweakDispArmUp()
{
    tweakDispArmAmt++;
}

task DispenserArmTask()
{
    // Turn off the motor and reset the encoder.  Note, we assume the
    // arm is tucked into the robot at program start.
    motor[mDispArm] = 0;
    nMotorEncoder[mDispArm] = 0;
    while (true) {
        long targetPos = -1;

        switch (dState) {
        case DISPENSER_PARKED:
            // Get down!
            targetPos = 0;
            break;

        case DISPENSER_JOYSTICK:
            // The user is in control, so we don't have a target.
            break;

        case DISPENSER_AUTO_PRESET:
            // Move to and hold the low arm position
            targetPos = DISPENSER_ARM_AUTO_PRESET_POS;
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
                    motor[mDispArm] = -(armPower - 2);
            }
        }
        EndTimeSlice();
    }
}

const int MIN_POWER = 20;
const int SLOW_START_DIST = 720;

int calculateTetrixPower(int power, long remainDist)
{
    // We only reduce power if the calculations if the request is for
    // more than MIN_POWER and that we have a 'bit of distance' to go.
    if (abs(power) < MIN_POWER || abs(remainDist) > SLOW_START_DIST)
        return power;

    // 50% linear reduction in power based on how far we have remaining.
    power -= (int)((float)power / 2.0 *
                   (((float)SLOW_START_DIST - (float)abs(remainDist)) /
                    (float)SLOW_START_DIST));

    // Limit ourself to at least MIN_POWER.
    if (abs(power) < MIN_POWER) {
        if (power < 0)
            power = -MIN_POWER;
        else
            power = MIN_POWER;
    }
    return power;
}
