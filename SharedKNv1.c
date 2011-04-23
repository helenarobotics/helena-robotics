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
const int BATON_ARM_DEPLOYED_POS = 720;

// Power to the baton arm
const int BATON_ARM_MOVE_POWER = 40;

// Baton dispenser open/close
const int BATON_DISPENSER_CLOSE = 244;
const int BATON_DISPENSER_OPEN = 104;

//
// Bridge Arm constants (left arm)
//

// How far to move the arm all the way out
const int BRIDGE_ARM_DEPLOYED_POS = 1200;

// Power to the bridge arm
const int BRIDGE_ARM_MOVE_POWER = 40;

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
            targetPos = BRIDGE_ARM_DEPLOYED_POS;
            if (armPos >= BRIDGE_ARM_DEPLOYED_POS) {
                brState = BRIDGE_DEPLOYED;
                // Reverse the motor a bit to reduce slop, per a posting
                // my Dick Swan (author or RobotC for NXT).
                motor[mBridgeArm] = -2;
            }
            break;

            // Fall through
        case BRIDGE_DEPLOYED:
            // We allow the users to move the bridge arm in the deployed
            // position, so we can't set a target position and hold it
            // here.
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
                // XXX - Check if these are correct for this motor?
                if (targetPos > armPos)
                    motor[mBridgeArm] = armPower;
                else
                    motor[mBridgeArm] = -armPower;
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
