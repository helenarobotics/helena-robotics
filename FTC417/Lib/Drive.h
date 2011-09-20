//
// Drive.h
//
// We default to fixed drive balance being on as that's
// what we did in Washington State regionals.
#ifndef USE_DRIVE_FIXED_BALANCE
#define USE_DRIVE_FIXED_BALANCE 1
#endif

//----------------------------------------------------------------------------------------------
// types
//----------------------------------------------------------------------------------------------

typedef enum { FORWARD, BACKWARD } DIRECTION;

#define ReverseDirection(direction)         ((direction)==FORWARD ? BACKWARD : FORWARD)
#define OrientDistance(distance,direction)  ((direction)==FORWARD ? (distance) : -(distance))

//----------------------------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------------------------

#ifndef DRIVE_POWER_FULL
#define DRIVE_POWER_FULL   100
#endif
#ifndef DRIVE_POWER_FAST
#define DRIVE_POWER_FAST   100
#endif
#ifndef DRIVE_POWER_MEDIUM
#define DRIVE_POWER_MEDIUM 60
#endif
#ifndef DRIVE_POWER_SLOW
#define DRIVE_POWER_SLOW   45
#endif
#ifndef DRIVE_POWER_RAMPTO
#define DRIVE_POWER_RAMPTO 35
#endif
#ifndef DRIVE_POWER_STALL
#define DRIVE_POWER_STALL  10   // slower than this we risk the motors (level to be verified)
#endif

// We begin the drive ramp when we're within so many encoder ticks of our goal
#ifndef DRIVE_RAMP_THRESHOLD
#define DRIVE_RAMP_THRESHOLD    dencRampDriveThreshold
const ENCOD dencRampDriveThreshold = Rounded(4.0 * ENCODER_TICKS_PER_MOTOR_REV, ENCOD); // 4.0 wheel rotations: it just seemed to work nicely, but is a little far
#endif

// If asked to drive too short of a distance, we don't even bother, as we really
// can't do that well. We also stop the drive when we're within this distance
// of our target.
#ifndef CM_DRIVE_TOLERANCE
#define CM_DRIVE_TOLERANCE  0.25
#endif

// CM_DRIVE_TOLERANCE converted to encoder ticks
const ENCOD dencDriveTolerance =
Rounded((float)CM_DRIVE_TOLERANCE / (float)cmPerEncoderTick, ENCOD);

#ifndef DRIVE_BALANCE_FORWARD_DEFAULT
#define DRIVE_BALANCE_FORWARD_DEFAULT    1.0
#endif
#ifndef DRIVE_BALANCE_BACKWARD_DEFAULT
#define DRIVE_BALANCE_BACKWARD_DEFAULT   1.0
#endif

#ifndef DRIVE_BALANCE_FORWARD
#define DRIVE_BALANCE_FORWARD  DRIVE_BALANCE_FORWARD_DEFAULT
#endif
#ifndef DRIVE_BALANCE_BACKWARD
#define DRIVE_BALANCE_BACKWARD DRIVE_BALANCE_BACKWARD_DEFAULT
#endif

//----------------------------------------------------------------------------------------------
// Allow client programs to control the use of ramping and waiting if they desire
//----------------------------------------------------------------------------------------------

#ifndef USE_DRIVE_RAMP
#define USE_DRIVE_RAMP true
#endif

#ifndef MS_DRIVE_WAIT
#define MS_DRIVE_WAIT 0
#endif

//----------------------------------------------------------------------------------------------
// State passed between drive-related functions
//----------------------------------------------------------------------------------------------

typedef struct
// internal state used by the drive logic
{
    ENCOD denc;
    ENCOD absdenc;
    DIRECTION direction;
    ENCOD encLeftCur;
    ENCOD encLeftFirst;
    ENCOD encLeftGoal;
    ENCOD encRightCur;
    ENCOD encRightFirst;
    ENCOD encRightGoal;
    int16 powerRampTo;          // power to ramp down to at end of drive
    int16 powerDrive;           // power to use for the main body of the drive
    float fractionRampTo;       // precomputed: powerRampTo / powerDrive
} DRIVESTATE;

//----------------------------------------------------------------------------------------------
// Support
//----------------------------------------------------------------------------------------------

// Balance the power to the left and right wheels according to the indicated
// factor, but always by *reducing* the power to one wheel or the other. Note
// that powerLeft and powerRight both must be variables, and both are IN OUT.
#define BalancePower(powerLeft, powerRight, balance)                \
    {                                                               \
    if ((balance) > 1.0)                                            \
        {                                                           \
        /* left motor gets smaller power */                         \
        int16 power = powerLeft;                                    \
        powerLeft  = Rounded((float)power / (balance), int16);      \
        }                                                           \
    else                                                            \
        {                                                           \
        /* right motor gets smaller power */                        \
        int16 power = powerRight;                                   \
        powerRight = Rounded((float)power * (balance), int16);      \
        }                                                           \
    }

#define BalanceFromDirection(direction) (FORWARD==(direction) ? DRIVE_BALANCE_FORWARD : DRIVE_BALANCE_BACKWARD)

void
CALLEDBY(iTaskMain)
RampDrivePower(DRIVESTATE &state, float fraction) {
    // Compute the power level as the fraction of the driving power
    int16 power = Rounded((float)(state.powerDrive) * fraction, int16);

    // Adjust the power to the left and right according to a balance setting
    // which accounts for any intrinsic lopsidedness in the driving of the bot.
    int16 powerLeft = power, powerRight = power;
    //
    float balance = 1.0;
#if USE_DRIVE_FIXED_BALANCE
    balance = BalanceFromDirection(state.direction);
#endif
    BalancePower(powerLeft, powerRight, balance);

    // Set the actual power
    LockBlackboard();
    //
    switch (state.direction) {
    case FORWARD:
        SetMotorPower(motorLeft, powerLeft);
        SetMotorPower(motorRight, powerRight);
        break;
    case BACKWARD:
        SetMotorPower(motorLeft, -powerLeft);
        SetMotorPower(motorRight, -powerRight);
        break;
    }
    //
    SendMotorPowers();
    ReleaseBlackboard();
}

void
CALLEDBY(iTaskMain)
RampDrivePower(DRIVESTATE &state) {
    if (state.absdenc > DRIVE_RAMP_THRESHOLD || !USE_DRIVE_RAMP)
        RampDrivePower(state, 1.0);
    else {
        float encoderFraction =
            (float)state.absdenc / (float)DRIVE_RAMP_THRESHOLD;
        float powerFraction =
            state.fractionRampTo + (1.0 -
            state.fractionRampTo) * encoderFraction;
        RampDrivePower(state, powerFraction);
    }
}

#define InitializePIDForDrive(pid,name,dencLeft,dencRight)  \
    {                                                       \
    InitializeSyncRatioPID(pid,name,dencLeft, dencRight);   \
    pid.dencErrAccumLower = -1000.0;                        \
    pid.dencErrAccumUpper =  1000.0;                        \
    pid.kp = 0.001000; /* was 0.000625 */                   \
    pid.ki = 0.0;                                           \
    pid.kd = 0.0;                                           \
    }

//----------------------------------------------------------------------------------------------
// main drive logic
//----------------------------------------------------------------------------------------------

BOOL
CALLEDBY(iTaskMain)
DriveForwards(float cmDistance, STOPCONDITIONS &stop, int16 power)
// Drive the indicated distance. Return success or failure.
// NB: Thus function can at times WRITE to the stop conditions.
{
  FunctionStart:
    BOOL fSuccess = !stop.fRequireStop;
    //
    if (Abs(cmDistance) >= CM_DRIVE_TOLERANCE) {
        DRIVESTATE state;

        // Set things up so that we can stop when we're supposed to
        ArmStopConditions(stop);

        // How far do we need to go in encoder ticks?
        float crevMotor = crevMotorPerCm * cmDistance;
        state.denc = Rounded(crevMotor * ENCODER_TICKS_PER_MOTOR_REV, ENCOD);

        // Where are the motors now?
        ReadEncoders_Lock(state.encLeftCur, state.encRightCur);

        // Do so math to figure out where the motors will be when we're done
        state.encLeftFirst = state.encLeftCur;
        state.encRightFirst = state.encRightCur;
        state.encLeftGoal = state.encLeftCur + state.denc;
        state.encRightGoal = state.encRightCur + state.denc;
        state.absdenc = Abs(state.denc);
        state.direction = cmDistance >= 0 ? FORWARD : BACKWARD;
        //
        state.powerRampTo = DRIVE_POWER_RAMPTO;
        state.powerDrive = power;
        ClampVar(state.powerDrive, DRIVE_POWER_STALL, DRIVE_POWER_FULL);
        ClampVar(state.powerRampTo, DRIVE_POWER_STALL, state.powerDrive);
        state.fractionRampTo =
            (float)state.powerRampTo / (float)state.powerDrive;
        //
        TRACE(("------------------------"));
        TRACE(("DriveForwards(%3.1f,%d)", cmDistance, power));

        // Power the motors
        RampDrivePower(state);

        // Off to the races!
        ENCOD dencPrev = state.denc;
        for (;;) {
            // Stop if the sensors say to
            if (CheckStopConditions(stop)) {
                TRACE(("drive: stop condition reached"));

                // If we're done, then we're done!
                if (stop.cmAdditional == 0.0) {
                    fSuccess = true;
                    break;
                }
                // We're asked to go an additional distance now that we've stopped.
                // Implement that using by emulating a recursive call with a goto
                // back to the start of the function. NB: this 'feature' probably
                // wasn't really worth the trouble, but hey.
                cmDistance = stop.cmAdditional;
                InitializeStopConditions(stop);
                goto FunctionStart;
            }
            // How far have the motors gone?
            ReadEncoders_Lock(state.encLeftCur, state.encRightCur);

            // How much more distance do we have to go? NB: we (somewhat arbitrarily)
            // only look at the LEFT encoder to measure the distance. Idea: might
            // it be better to AVERAGE the two encoders? That's not yet explored, as
            // we're not seeing any issues attributable to the left-only approach.
            state.denc = state.encLeftGoal - state.encLeftCur;
            state.absdenc = Abs(state.denc);
            //
            if (state.absdenc <= dencDriveTolerance) {
                break;
            }
            //
            if (Sign(state.denc) != Sign(dencPrev)) {
                TRACE(("drive: overshot: cur=%d prev=%d", state.denc,
                        dencPrev));
                break;
            }
            //
            RampDrivePower(state);
            //
            dencPrev = state.denc;
            //
            SpeculativelyUpdateBlackboard();
        }
        //
        StopRobot();
        DisarmStopConditions(stop);
        //
        // Wait for robot inertia to settle to improve navigation.
        //
        if (0 < MS_DRIVE_WAIT)
            wait1Msec(MS_DRIVE_WAIT);
        //
        DriveTurnDemoWait();
    }
    //
    return fSuccess;
}

#define DriveBackwards(cmDistance) DriveForwards(-(cmDistance))

BOOL
DriveForwards(float cmDistance) {
    STOPCONDITIONS stop;
    InitializeStopConditions(stop);
    return DriveForwards(cmDistance, stop, DRIVE_POWER_FAST);
}

BOOL
DriveForwardsToSonic(int cmSonic, float cmExtra)
// Drive forwards until the (front) ultrasonic sensor reads the indicated value, then a little extra
{
#if SensorIsDefined(sensnmSonicFront)
    int cmCur;
    //
    // Drive forwards until the ultrasound gives us a reading
    //
    for (;;) {
        cmCur = ReadSonic_Main(sensSonicFront, false);
        if (cmCur != cmSonicNil)
            break;
        if (!DriveForwards(20)) // 20 is arbitrary here, and hasn't really be tested well
            return false;
    }
    //
    // Once we have the reading, drive the remaining distance
    //
    return DriveForwards((float)(cmCur - cmSonic) + cmExtra);
#else
    return false;
#endif
}

BOOL
DriveForwards(float cmDistance, int power) {
    STOPCONDITIONS stop;
    InitializeStopConditions(stop);
    return DriveForwards(cmDistance, stop, power);
}
