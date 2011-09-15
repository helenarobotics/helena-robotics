//
// Turn.h
//
// NOTE: The ramping functionality would be critical in a wheel-based bot, but
//       it's causing us stability problems here (esp when the power gets low at
//       the end) so we turn it off for now. Once we get decent PID working, we
//       can probably turn it back on.

//----------------------------------------------------------------------------------------------
// Types
//----------------------------------------------------------------------------------------------

typedef enum { TURN_LEFT, TURN_RIGHT } TURN_DIRECTION;

#define ReverseTurnDirection(direction) ((direction)==TURN_LEFT ? TURN_RIGHT : TURN_LEFT)

//----------------------------------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------------------------------

#ifndef TURN_POWER_FULL
#define TURN_POWER_FULL     100
#endif
#ifndef TURN_POWER_FAST
#define TURN_POWER_FAST     100
#endif
#ifndef TURN_POWER_MEDIUM
#define TURN_POWER_MEDIUM    70
#endif
#ifndef TURN_POWER_SLOW
#define TURN_POWER_SLOW      40
#endif
#ifndef TURN_POWER_RAMPTO
#define TURN_POWER_RAMPTO    10
#endif
#ifndef TURN_POWER_STALL
#define TURN_POWER_STALL     10        // slower than this we risk the motors (exact level to be verified)
#endif

#ifndef TURN_BALANCE
#define TURN_BALANCE    (1.0)
#endif

// We begin the turn ramp when we're within so many encoder ticks of our goal
#ifndef TURN_RAMP_THRESHOLD
#define TURN_RAMP_THRESHOLD     dencTurnRampThreshold
#define TURN_RAMP_THRESHOLD_DEFAULT
ENCOD dencTurnRampThreshold;            // initialized by InitializeTurns
#endif

//----------------------------------------------------------------------------------------------
// Allow client programs to control the use of ramping and waiting if they desire
//----------------------------------------------------------------------------------------------

#ifndef USE_TURN_RAMP
#define USE_TURN_RAMP false // we wish we could, but it's just causing problems and we're out of testing runway
#endif

#ifndef MS_TURN_WAIT
#define MS_TURN_WAIT 100  // useful for v11 repeatability
#endif

//----------------------------------------------------------------------------------------------
// State passed between turn-related functions
//----------------------------------------------------------------------------------------------

typedef struct
// Internal state used by the turn logic, one instance for each of the left and right motors
    {
    int         imotor;                     // the index of the motor we are manipulating
    ENCOD       encStart;                   // the motor's encoder value as of the start of the turn
    ENCOD       encCur;                     // the motor's current encoder value
    ENCOD       encGoal;                    // the motor's encoder value we are trying to achieve
    ENCOD       dencCur;                    // the distance remaining: encGoal - encCur
    ENCOD       dencPrev;                   // the distance remaining the last time around the turn loop
    ENCOD       absdencCur;                 // the absolute value of dencCur
    ENCOD       dencRampThreshold;          // we start ramping the power down when dencCur is below this
    DIRECTION   direction;                  // whether this motor is going forward or backward
    int16       powerRampTo;                // the min power we are to ramp down to for this motor
    int16       powerTurn;                  // the power level we are to use for the non-ramped-down part of the turn
    float       fractionRampTo;             // precomputed: powerRampTo / powerTurn
    float       balance;                    // the balance to apply to the wheels during the turn
    BOOL        fActive;                    // whether this motor is turning (true) or has reached its goal (false)
    BOOL        fUseGyro;                   // whether we're turning based on the gyro or encoders
    } TURNSTATE;

#define MotorOfTurnState(state)     rgmotor[state.imotor]

//----------------------------------------------------------------------------------------------
// Power support
//----------------------------------------------------------------------------------------------

void CALLEDBY(iTaskMain) RampTurnPower(TURNSTATE& state, float fraction)
    {
    LockBlackboard();
    int16 power = 0;

    // If the motor is still active in the turn, then figure out
    // its power level given the ramping implied by 'fraction' and
    // any quantization in effect.
    if (state.fActive)
        {
        power = Rounded((float)(state.powerTurn) * fraction, int16);
        if (state.direction == BACKWARD)
            power = -power;
        }
    int16 powerLeft = power, powerRight = power;

    // Balance the power to the wheels in order to help even out turn rates
    BalancePower(powerLeft, powerRight, state.balance);

    // Apply this power to the appropriate motor
    if (IsLeftMotor(MotorOfTurnState(state)))
        {
        SetMotorPower(MotorOfTurnState(state),  powerLeft);
        }
    else
        {
        SetMotorPower(MotorOfTurnState(state), powerRight);
        }

    ReleaseBlackboard();
    }

void CALLEDBY(iTaskMain) RampTurnPower(TURNSTATE& state)
    {
    // REVIEW: in future, we should (if we do ramps at all) do a ramp even when we're
    // turning using the gyro. But for now we don't bother (testing testing testing!).
    if (!USE_TURN_RAMP || state.fUseGyro || state.absdencCur > state.dencRampThreshold)
        RampTurnPower(state, 1.0);
    else
        {
        float encoderFraction = (float)state.absdencCur / (float)state.dencRampThreshold;
        float powerFraction   = state.fractionRampTo + (1.0-state.fractionRampTo) * encoderFraction;
        RampTurnPower(state, powerFraction);
        }
    }

//----------------------------------------------------------------------------------------------
// Turning
//----------------------------------------------------------------------------------------------

void GetEncoderDeltasForTurnLeft(ANGLE angle, ENCOD& dencLeft, ENCOD& dencRight);

void InitializeTurnState(TURNSTATE& state, MOTOR& motorParam, ENCOD denc, BOOL fUseGyroParam)          \
    {                                                         \
    state.imotor        = motorParam.imotor;                  \
    state.encCur        = state.encStart;                     \
    state.dencCur       = denc;                               \
    state.dencPrev      = denc;                               \
    state.encGoal       = state.encCur + denc;                \
    state.absdencCur    = Abs(denc);                          \
    state.dencRampThreshold = 0;                              \
    state.direction     = denc >= 0 ? FORWARD : BACKWARD;     \
    state.powerRampTo   = 0;                                  \
    state.powerTurn     = 0;                                  \
    state.fractionRampTo = 0;                                 \
    state.balance       = 1.0;                                \
    state.fActive       = (denc != 0);                        \
    state.fUseGyro      = fUseGyroParam;                      \
    }

#define DisplayNameTurnState(state) (MotorOfTurnState(state).displayName)

#define AchievedTurnTarget(state)   \
    {                               \
    state.fActive = false;          \
    }

#define OvershotTurn(state)                                           \
    {                                                                 \
    string szDisplayName = DisplayNameTurnState(state);               \
    TRACE(("turn: %s overshot p=%d c=%d", szDisplayName, state.dencPrev, state.dencCur)); \
    state.fActive = false;                                            \
    }

// A bit field of flags controlling turn behavior
typedef enum
    {
    TURNFLAG_NONE          = 0, // value to pass if one is not interested in any of the other flags
    //
    TURNFLAG_USE_ENCOD     = 1, // ignore the angle (literally: the value may be junk) and execute the turn based on the provided encoder values
    TURNFLAG_PROHIBIT_GYRO = 2, // even if a gyro is present, don't use it; instead compute encoders from angle and go from there
    } TURNFLAG;

BOOL CALLEDBY(iTaskMain) TurnRightCore(ANGLE angle, int16 power, TURNFLAG flags, ENCOD dencLeft, ENCOD dencRight, STOPCONDITIONS& stop, float balance=TURN_BALANCE)
    {
    BOOL fSuccess = !stop.fRequireStop;

    // Figure out if we need to need to do an encoder-based turn
    // or whether we can use the gyro. In the former case, the caller
    // might have provided the encoder values already, or we might
    // need to compute them here.
    BOOL fUseGyro = false;
#if SensorIsDefined(sensnmGyroHorz)
    fUseGyro = !(flags & TURNFLAG_PROHIBIT_GYRO);
#endif
    if (fUseGyro)
        {
        // Set dencLeft and dencRight to some value with the right sign so that we
        // know which way to turn each wheel.
        if (angle >= 0.0)
            {
            dencLeft  =  1.0;
            dencRight = -1.0;
            }
        else
            {
            dencLeft  = -1.0;
            dencRight =  1.0;
            }
        }
    else if (!(flags & TURNFLAG_USE_ENCOD))
        {
        // We can't use the gyro, and the caller didn't give us encoder counts,
        // so we have to figure them out for ourselves.
        GetEncoderDeltasForTurnLeft(-angle, dencLeft, dencRight);
        }
    //
    if ((fUseGyro && angle != 0.0) || (dencLeft!=0 || dencRight!=0))
        {
        TRACE(("TurnRight(%.0f,%d,%d,%d,%d)", angle,power,dencLeft,dencRight,fUseGyro));

        // Initialize the state variables
        TURNSTATE stateLeft, stateRight;
        ReadEncoders_Lock(stateLeft.encStart, stateRight.encStart);
        InitializeTurnState(stateLeft,  motorLeft,  dencLeft, fUseGyro);
        InitializeTurnState(stateRight, motorRight, dencRight, fUseGyro);

        // Power levels default to the ramp bottom and the requested full power
        stateLeft .powerRampTo       = TURN_POWER_RAMPTO;
        stateLeft .powerTurn         = power;
        stateLeft .dencRampThreshold = TURN_RAMP_THRESHOLD;
        stateLeft .balance           = balance;
        //
        stateRight.powerRampTo       = TURN_POWER_RAMPTO;
        stateRight.powerTurn         = power;
        stateRight.dencRampThreshold = TURN_RAMP_THRESHOLD;
        stateRight.balance           = balance;

        // Make sure that the power levels are absolutely within bounds
        ClampVar(stateLeft.powerTurn,     TURN_POWER_STALL, TURN_POWER_FULL);
        ClampVar(stateRight.powerTurn,    TURN_POWER_STALL, TURN_POWER_FULL);
        ClampVar(stateLeft.powerRampTo,   TURN_POWER_STALL, stateLeft.powerTurn);
        ClampVar(stateRight.powerRampTo,  TURN_POWER_STALL, stateRight.powerTurn);

        // We suspend the display task purely as a perf optimization.
        SuspendDisplayTask();

        // Suspend the Blackboard task itself, since our SpeculativelyUpdateBlackboard
        // call below will do all that stuff on this thread quite nicely and with
        // less contention.
        SuspendBlackboard();

        // Power the motors. Do this *before* messing with the gyro so we don't
        // have that few ms of delay as we start them up: it might be messing
        // with the gyro.
        RampTurnPower(stateLeft);
        RampTurnPower(stateRight);
        SendMotorPowers();

        // If we're using the gyro, then add that to our list of stop conditions
#if SensorIsDefined(sensnmGyroHorz)
        BOOL fUnReadGyro = false;       // if true on exit, we need to call a balancing StopReadingGyroSensor()
        int32 cGyroReadStart = 0;
        MILLI msGyroStart;
        if (fUseGyro)
            {
            LockBlackboard();

            // Make sure we know exactly where the gyro is right now. Remember
            // in a BOOL to later undo the StartingReadingGyroSensor.
            StartReadingGyroSensor(sensGyroHorz);
            fUnReadGyro = true;
            MILLI msNow = nSysTime;
            ReadGyroSensor(sensGyroHorz, msNow);

            // Record some stats so we can print some debugging tidbits later
            cGyroReadStart = sensGyroHorz.cRead;
            msGyroStart    = msNow;

            // Figure out what we want the target gyro angle to be. Note that measured
            // angles are in degrees CCW (i.e.: to the left) so that we need to subtract
            // rather than add 'angle' to compute the target.
            //
            // We also need to compensate for the fact that our gyro is acting up
            // and seems to need an error correction.
            const float perTurnError = -5.510247 * sgn(angle);
            const float perDegError  = 0.025104;
            const float totalError   = (perTurnError + perDegError * angle);
            sensGyroHorz.degTarget   = (sensGyroHorz.deg - angle) - totalError;
            sensGyroHorz.comparison  = (angle > 0) ? COMPARE_LE : COMPARE_GE;

            // Make sure we stop on that condition
            stop.fStopOnGyroHorz = true;

            ReleaseBlackboard();
            }
#endif
        // Start the monitoring of the sensors for detecting when we should stop
        ArmStopConditions(stop);

        // Off to the races!
        for(;;)
            {
            if (fUseGyro)
                {
            #if 0
                LockBlackboard();
                TelemetryAddInt32(telemetry, nSysTime - msStart);
                TelemetryAddFloat(telemetry, sensGyroHorz.degCWPerS);
                TelemetryAddFloat(telemetry, sensGyroHorz.deg);
                TelemetryAddInt16(telemetry, stateLeft.powerTurn);
                ReleaseBlackboard();
                TelemetrySend(telemetry);
            #endif
                }
            else
                {
                ReadEncoders_Lock(stateLeft.encCur, stateRight.encCur);

                // How far remains to our goal for each motor?
                stateLeft.dencCur  = stateLeft.encGoal  - stateLeft.encCur;
                stateRight.dencCur = stateRight.encGoal - stateRight.encCur;
                }

            // Stop if the sensors say to
            if (CheckStopConditions(stop))
                {
                StopRobot();                    // ASAP!
                //
                fSuccess = true;
                stop.fStopConditionReached = true;

                // Report the fraction of the requested turn that remains
                if (!fUseGyro)
                    {
                    float leftFraction  = (float)stateLeft.dencCur  / (float)dencLeft;
                    float rightFraction = (float)stateRight.dencCur / (float)dencRight;
                    stop.fractionRemaining = leftFraction;
                    TRACE(("turn: stop reached: %1.3f %1.3f", leftFraction, rightFraction));
                    }
                else
                    {
                    #if SensorIsDefined(sensnmGyroHorz)
                    LockBlackboard();
                    stop.fractionRemaining = (float)abs(sensGyroHorz.deg - sensGyroHorz.degTarget) / abs(angle);
                    ReleaseBlackboard();
                    TRACE(("turn: stop reached: %1.3f", stop.fractionRemaining));
                    #endif
                    }
                break;
                }

            if (!fUseGyro)
                {
                // Figure out if we've hit the target or overshot on each of the motors
                stateLeft.absdencCur  = Abs(stateLeft.dencCur);
                stateRight.absdencCur = Abs(stateRight.dencCur);
                //
                if (stateLeft.fActive  && (float)stateLeft.absdencCur  < dencDriveTolerance) AchievedTurnTarget(stateLeft);
                if (stateRight.fActive && (float)stateRight.absdencCur < dencDriveTolerance) AchievedTurnTarget(stateRight);
                //
                if (stateLeft.fActive  && Sign(stateLeft.dencCur)  != Sign(stateLeft.dencPrev))  OvershotTurn(stateLeft);
                if (stateRight.fActive && Sign(stateRight.dencCur) != Sign(stateRight.dencPrev)) OvershotTurn(stateRight);

                // If both motors are finished, then we're done
                if (!stateLeft.fActive && !stateRight.fActive)
                    break;
                }

            // Adjust the power to each motor (if necessary)
            RampTurnPower(stateLeft);
            RampTurnPower(stateRight);
            SendMotorPowers();

            if (!fUseGyro)
                {
                // Prepare for the next time around the loop
                stateLeft.dencPrev = stateLeft.dencCur;
                stateRight.dencPrev = stateRight.dencCur;
                }

            // Update the blackboard ourselves (recall we suspended the actual blackboard task)
            SpeculativelyUpdateBlackboard();
            }
        //
        ResumeBlackboard();
        ResumeDisplayTask();
        //
        StopRobot();
        DisarmStopConditions(stop);
#if SensorIsDefined(sensnmGyroHorz)
        if (fUseGyro && stop.fStopConditionReached)
            {
            int32 cRead = sensGyroHorz.cRead - cGyroReadStart;
            TRACE(("goal=%.0f ach=%.0f", sensGyroHorz.degTarget, sensGyroHorz.degDetected));
            TRACE(("count=%d rate=%.1f", cRead, (float)cRead / ( (float)(nSysTime - msGyroStart) ) * 1000.0));
            }
        if (fUnReadGyro)
            {
            StopReadingGyroSensor(sensGyroHorz);
            }
#endif
        //
        // Wait for robot inertia to settle to improve navigation.
        //
        if (0 != MS_TURN_WAIT)
            wait1Msec(MS_TURN_WAIT);
        //
        DriveTurnDemoWait();
        }
    //
    return fSuccess;
    }

//----------------------------------------------------------------------------------------------
// Computing encoder ticks necessary to turn a given angle
//----------------------------------------------------------------------------------------------

// Note: the array mpangledenc of turn-to-encoder-values is defined in robot.h

void GetEncoderDeltasForTurnLeft(ANGLE angle, ENCOD& dencLeft, ENCOD& dencRight)
// Find out how the motor encoders need to change to accomplish a turn of the indicated magnitude
    {
#if HAS_ENCODER_BASED_TURNING_TABLE
    // Ensure the angle in the range [-180.0, 180.0)
    NormalizeAngleVar(angle);

    // Initialize indices into the turn table
    int iFirst = 0;
    int iLast  = sizeof(mpangledenc) / sizeof(mpangledenc[0])-1;    // the number of entries in the table - 1

    // Find the line in the table that is <= angle. We use a binary search.
    // Invariant: the line we're looking for is in the index range [iFirst, iLast].
    int iCur;
    BOOL fFound = false;
    while (true)
        {
        iCur = (iFirst + iLast) >> 1;
        if (iFirst > iLast)
            break;
        //
        float angleCur = mpangledenc[iCur][0];
        //
        if (angle > angleCur)
            iFirst = iCur + 1;
        else if (angle < angleCur)
            iLast = iCur - 1;
        else
            {
            fFound = true;
            break;
            }
        }

    // Allow for us to be able to scale the table entries. This has historically
    // been used as a quick hack to adjust mpangledenc table values to adjust for
    // different surfaces, such as carpet vs foam pad.
    const float scale = mpangledencScaleFactor;

    // If we've hit it exactly, the use the entry, otherwise interpolate
    if (fFound)
        {
        dencLeft  = Rounded(mpangledenc[iCur][1] * scale, ENCOD);
        dencRight = Rounded(mpangledenc[iCur][2] * scale, ENCOD);
        }
    else
        {
        int iNext = iCur + 1;
        float dangle        =                 angle - mpangledenc[iCur][0];
        float dangleMax     = mpangledenc[iNext][0] - mpangledenc[iCur][0];
        float angleFraction = dangle / dangleMax;
        //
        float dencLeftFloat  = mpangledenc[iCur][1] + angleFraction * (mpangledenc[iNext][1]-mpangledenc[iCur][1]);
        float dencRightFloat = mpangledenc[iCur][2] + angleFraction * (mpangledenc[iNext][2]-mpangledenc[iCur][2]);
        //
        dencLeft  = Rounded(dencLeftFloat  * scale, ENCOD);
        dencRight = Rounded(dencRightFloat * scale, ENCOD);
        }
    TRACE(("a=%3.1f i=%d l=%d r=%d", angle, iCur, dencLeft, dencRight));
#else
    dencLeft = dencRight = 0;
#endif
    }

//----------------------------------------------------------------------------------------------
// Helper functions
//----------------------------------------------------------------------------------------------

#ifdef TURN_RAMP_THRESHOLD_DEFAULT
#define InitializeTurns()                                         \
    { /* the ramp threshold is approx 45 degrees */               \
    ENCOD dencLeft, dencRight;                                    \
    GetEncoderDeltasForTurnLeft(45.0, dencLeft, dencRight);       \
    dencTurnRampThreshold = Abs(dencRight); /*Abs is paranoia*/   \
    }
#else
#define InitializeTurns()
#endif

#ifndef TURNFLAG_DEFAULT
#define TURNFLAG_DEFAULT    TURNFLAG_NONE
#endif

BOOL TurnRight(ANGLE angle, int16 powerLevel, STOPCONDITIONS& stop, float balance=TURN_BALANCE)
    {
    return TurnRightCore(angle, powerLevel, TURNFLAG_DEFAULT, 0, 0, stop, balance);
    }

BOOL TurnRight(ANGLE angleToTurn, int16 powerLevel, float balance)
    {
    STOPCONDITIONS stop; InitializeStopConditions(stop);        // review: could probably use global 'stop' rather than a new local
    return TurnRight(angleToTurn, powerLevel, stop, balance);
    }

BOOL TurnRightEncod(int powerLevel, ENCOD dencLeft, ENCOD dencRight, float balance=TURN_BALANCE)
    {
    STOPCONDITIONS stop; InitializeStopConditions(stop);        // review: could probably use global 'stop' rather than a new local
    return TurnRightCore(0, powerLevel, (TURNFLAG)(TURNFLAG_USE_ENCOD | TURNFLAG_DEFAULT), dencLeft, dencRight, stop, balance);
    }

BOOL TurnRight(ANGLE angleToTurn)
    {
    return TurnRight(angleToTurn, TURN_POWER_MEDIUM, TURN_BALANCE);
    }

#define TurnLeftEnc(angleToTurn)                 TurnRight(-(angleToTurn))
