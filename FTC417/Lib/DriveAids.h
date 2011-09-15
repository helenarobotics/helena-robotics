//
// DriveAids.h
//
//----------------------------------------------------------------------------------------------
// Configuration
//----------------------------------------------------------------------------------------------

#ifndef TURN_POWER_STRAIGHTEN
#define TURN_POWER_STRAIGHTEN TURN_POWER_FAST
#endif

#ifndef TURN_BALANCE_STRAIGHTEN
#define TURN_BALANCE_STRAIGHTEN TURN_BALANCE
#endif

#ifndef TURN_BALANCE_ADJUSTLATERAL
#define TURN_BALANCE_ADJUSTLATERAL TURN_BALANCE
#endif

//----------------------------------------------------------------------------------------------
// Simple aids
//----------------------------------------------------------------------------------------------

BOOL DriveForwardsAndStraightenUsingLeftSonic(float cmDistance, int powerLevel)
// Drive forwards the indicated amount, then straighten using the delta
// in reading on the left sonic sensor. Note: it would be reasonable to
// generalize this to be able to use the right sensor instead: we'd just
// have to turn the other direction.
    {
#if SensorIsDefined(sensnmSonicLeft)
    int  cmSonicStart = ReadSonic_Main(sensSonicLeft,true);
    BOOL fSuccess      = DriveForwards(cmDistance, powerLevel);
    int  cmSonicCur   = ReadSonic_Main(sensSonicLeft,true);
    //
    if (fSuccess)
        {
        float dy = (float)cmSonicCur - (float)cmSonicStart;
        // TRACE(("h=%5.3f y=%d dy=%5.3f", cmDistance, cmSonicStart, dy));
        //
        ANGLE angle = asin(dy / cmDistance);      // in radians
        angle = oneEightyOverPi * angle;          // to degrees
        //
        fSuccess = TurnRight(-angle, TURN_POWER_STRAIGHTEN, TURN_BALANCE_STRAIGHTEN);
        }
    //
    return fSuccess;
#else
    return false;
#endif
    }

BOOL DriveForwardsAndStraightenUsingRightSonic(float cmDistance, int powerLevel)
    {
#if SensorIsDefined(sensnmSonicRight)
    int  cmSonicStart = ReadSonic_Main(sensSonicRight,true);
    BOOL fSuccess     = DriveForwards(cmDistance, powerLevel);
    int  cmSonicCur   = ReadSonic_Main(sensSonicRight,true);
    //
    if (fSuccess)
        {
        float dy = (float)cmSonicCur - (float)cmSonicStart;
        // TRACE(("h=%5.3f y=%d dy=%5.3f", cmDistance, cmSonicStart, dy));
        //
        ANGLE angle = asin(dy / cmDistance);      // in radians
        angle = oneEightyOverPi * angle * -1;          // to degrees
TRACE(("straighten %f", angle));
        //
        fSuccess = TurnRight(-angle, TURN_POWER_STRAIGHTEN, TURN_BALANCE_STRAIGHTEN);
        }
    //
    return fSuccess;
#else
    return false;
#endif
    }

BOOL AdjustLaterally(float dcm)
// Adjust our position laterally by the indicated delta. We to this by
// executing a 45deg turn, driving the hypotenuse of the triangle, turning
// back 45deg, then driving back to position.
//
// Note that dcm may be positive or negative.
    {
    RET_IF_FAIL(TurnRight(-45, TURN_POWER_FAST, TURN_BALANCE_ADJUSTLATERAL),   "TurnRight");
    RET_IF_FAIL(DriveForwards(-sqrtTwo * dcm, DRIVE_POWER_SLOW),               "DriveForwards");
    RET_IF_FAIL(TurnRight(45, TURN_POWER_FAST, TURN_BALANCE_ADJUSTLATERAL),    "TurnBack");
    RET_IF_FAIL(DriveForwards(dcm, DRIVE_POWER_SLOW),                          "DriveBackwards");
    //
    return true;
    }

BOOL AdjustLaterallyUsingAngle(float dcm, ANGLE degAngle)
    {
    float radAngle   = degreesToRadians(degAngle);
    float hypotenuse = dcm / sin(radAngle);
    float leg        = dcm * cos(radAngle) / sin(radAngle);

    // Refuse to drive way too far to do the adjustment
    if (hypotenuse > 30)
        return false;

    RET_IF_FAIL(TurnRight(-degAngle, TURN_POWER_FAST, TURN_BALANCE_ADJUSTLATERAL),   "TurnRight");
    RET_IF_FAIL(DriveForwards(hypotenuse, DRIVE_POWER_SLOW),                         "DriveForwards");
    RET_IF_FAIL(TurnRight(degAngle, TURN_POWER_FAST, TURN_BALANCE_ADJUSTLATERAL),    "TurnBack");
    RET_IF_FAIL(DriveForwards(leg, DRIVE_POWER_SLOW),                                "DriveBackwards");

    return true;
    }

//----------------------------------------------------------------------------------------------
// Manual driving
//----------------------------------------------------------------------------------------------

#if USE_JOYSTICK_DRIVER

#if ROBOT_NAME==ROBOT_NAME_OMNI_BOT

void DoManualDrivingControl(int joy, TJoystick& joystick)
    {
    int ctlMoveX = joyLeftX(joy);
    int ctlMoveY = joyLeftY(joy);      // -128 to  127
    int ctlTurn = -joyRightX(joy);     //  128 to -127

    // Implement the deadzone in the middle
    const int sensitivityThreshold = 15;
    int sgnMoveX = Sign(ctlMoveX);
    int sgnMoveY = Sign(ctlMoveY);
    int sgnTurn = Sign(ctlTurn);
    ctlMoveX = Max(0, abs(ctlMoveX) - sensitivityThreshold);
    ctlMoveY = Max(0, abs(ctlMoveY) - sensitivityThreshold);
    ctlTurn = Max(0, abs(ctlTurn) - sensitivityThreshold);
    ctlMoveX = ctlMoveX * sgnMoveX;
    ctlMoveY = ctlMoveY * sgnMoveY;
    ctlTurn = ctlTurn   * sgnTurn;

    // scale to +- 100
    float scale = 50.0 / (128.0 - (float)sensitivityThreshold);
    ctlMoveX = Rounded((float)ctlMoveX * scale, int);
    ctlMoveY = Rounded((float)ctlMoveY * scale, int);
    ctlTurn  = Rounded((float)ctlTurn  * scale, int);

    int powerLeftFront  = -ctlMoveY -ctlMoveX -ctlTurn;
    int powerRightFront =  ctlMoveY -ctlMoveX -ctlTurn;
    int powerLeftBack   = -ctlMoveY +ctlMoveX -ctlTurn;
    int powerRightBack  =  ctlMoveY +ctlMoveX -ctlTurn;

    LockBlackboard();
    SetMotorPower(motorLeftFront,  powerLeftFront);
    SetMotorPower(motorRightFront, powerRightFront);
    SetMotorPower(motorLeftBack,   powerLeftBack);
    SetMotorPower(motorRightBack,  powerRightBack);
    SendMotorPowers();
    ReleaseBlackboard();
    }

#else

void DoManualDrivingControl(int joy, TJoystick& joystick)
// Interpret the joysticks and manually drive the bot
    {
    // Extract values from the joysticks that we find useful
    int ctlPower    =  joyLeftY(joy);      // -128 to  127
    int ctlSteering = -joyRightX(joy);     //  128 to -127

    // Remove sensitivity in the middle of the controls.
    // At the same time, avoid a big control jump at the
    // edge of the dead zone.
    const int sensitivityThreshold = joyThrottleDeadZone;
    int sgnPower    = Sign(ctlPower);
    int sgnSteering = Sign(ctlSteering);

    ctlPower    = Max(0, abs(ctlPower)    - sensitivityThreshold);
    ctlSteering = Max(0, abs(ctlSteering) - sensitivityThreshold);

    ctlPower    = ctlPower    * sgnPower;
    ctlSteering = ctlSteering * sgnSteering;

    // Convert the range of controls from +-128 to +-100. Do
    // so symmetrically for both positive and negative numbers.
    float scale = 100.0 / (128.0 - (float)sensitivityThreshold);
    ctlPower    = Rounded((float)ctlPower    * scale, int);
    ctlSteering = Rounded((float)ctlSteering * scale, int);
    // if (ctlPower != 0 || ctlSteering != 0) {  TRACE(("power=%d steering=%d", ctlPower, ctlSteering)); }

    // Update the motor power. The SetMotorPower internals will
    // clamp the power provided to +-100.
    int16 powerLeft  = ctlPower - ctlSteering;
    int16 powerRight = ctlPower + ctlSteering;

    // Correct the power balance to the wheels in order to better drive straight
    float balance = BalanceFromDirection(ctlPower >= 0 ? FORWARD : BACKWARD);
    BalancePower(powerLeft, powerRight, balance);
    // if (powerLeft != 0 || powerRight != 0) { TRACE(("l=%d r=%d", powerLeft, powerRight)); }

    LockBlackboard();
    SetMotorPower(motorLeft,  powerLeft);
    SetMotorPower(motorRight, powerRight);
    SendMotorPowers();
    ReleaseBlackboard();
    }

#endif
#endif
