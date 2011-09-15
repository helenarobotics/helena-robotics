//
// ServoRotor.h
//

#ifndef HAS_SERVO_ROTOR
#define HAS_SERVO_ROTOR 0
#undef  USE_ROTOR_STALL_CHECK
#define USE_ROTOR_STALL_CHECK 0
#endif

typedef enum { CAPTURE_BATONS, SPILL_BATONS, STOP_BEFORE_DISPENSING } BATON_DISPOSITION;

#if HAS_SERVO_ROTOR

#ifndef USE_ROTOR_STALL_CHECK
    #if SensorIsDefined(sensnmAngleRotor)
        #define USE_ROTOR_STALL_CHECK 1
    #else
        #define USE_ROTOR_STALL_CHECK 0
    #endif
#endif

//-----------------------------------------------------------------------------------
// Rotor stall check logic
//-----------------------------------------------------------------------------------

typedef enum
    {
    ROTOR_STALL_STATE_OK,
    ROTOR_STALL_STATE_JUST_STALLED,
    ROTOR_STALL_STATE_FLICK_REVERSE,
    ROTOR_STALL_STATE_STOP,
    } ROTOR_STALL_STATE;

ROTOR_STALL_STATE rotorStallState = ROTOR_STALL_STATE_OK;

#if USE_ROTOR_STALL_CHECK

    MILLI msLastRotorStallCheck = 0;
    int   cRotorStallCheckRequests = 0;

    #define ResetRotorStallCheck()  { msLastRotorStallCheck = 0; }

    // NB: Need the blackboard lock in order to be able to read the angle sensor
    #define CheckForRotorStall(msNow)                                                                                               \
        {                                                                                                                           \
        BOOL fFirstTime = (0 == msLastRotorStallCheck);                                                                             \
                                                                                                                                    \
        /* The rotor is ok if it has no power or if it's moved */                                                                   \
        BOOL fOK = fFirstTime || GetServoValue(svoRotor) == svposRotorStop || sensAngleRotor.degStallPrev != sensAngleRotor.deg;    \
                                                                                                                                    \
        if (fOK)                                                                                                                    \
            {                                                                                                                       \
            sensAngleRotor.msLastOkStall = msNow;                                                                                   \
            }                                                                                                                       \
        else if (msNow - sensAngleRotor.msLastOkStall > 300)                                                                        \
            {                                                                                                                       \
            PlaySadNoWait();                                                                                                        \
            TRACE(("rotor stall"));                                                                                                 \
            /* if we've just stalled, give outer layer a chance to deal with it, but otherwise we tell it to stop things */         \
            switch (rotorStallState)                                                                                                \
                {                                                                                                                   \
            case ROTOR_STALL_STATE_OK:                                                                                              \
                rotorStallState = ROTOR_STALL_STATE_JUST_STALLED;                                                                   \
                break;                                                                                                              \
            default:                                                                                                                \
                rotorStallState = ROTOR_STALL_STATE_STOP;                                                                           \
                break;                                                                                                              \
                }                                                                                                                   \
            }                                                                                                                       \
                                                                                                                                    \
        sensAngleRotor.degStallPrev = sensAngleRotor.deg;                                                                           \
        msLastRotorStallCheck       = msNow;                                                                                        \
        }

    #define StartRotorStallCheck()              \
        {                                       \
        if (0 == cRotorStallCheckRequests)      \
            {                                   \
            msLastRotorStallCheck = 0;          \
            }                                   \
        cRotorStallCheckRequests++;             \
        }

    #define StopRotorStallCheck()               \
        {                                       \
        cRotorStallCheckRequests--;             \
        }

    #define FDoRotorStallCheck()    (cRotorStallCheckRequests > 0)

#else

    #define ResetRotorStallCheck()
    #define StartRotorStallCheck()
    #define StopRotorStallCheck()

#endif

//-----------------------------------------------------------------------------------
// Low-level rotor speed management
//-----------------------------------------------------------------------------------

// Forward declaration
void SetRotorSpeed(int svposSpeed);

// For keeping track of what speed the app *wants* the rotor to go at
int svposRotorTargetSpeed = svposRotorStop;

// Given a rotor speed, compute the same speed but in the opposite rotor direction
#define ReverseRotorSpeed(speed)        (256 - (speed)) // 128 - ((speed) - 128)

#if USE_ROTOR_STALL_CHECK

// The loop of DoRotorTaskWork runs faster during the act of carrying out the flick
// in order to get more precision on its timing.
#define MS_ROTOR_TASK_FAST      75
#define MS_ROTOR_TASK_SLOW      300
MILLI msRotorTaskInterval  = MS_ROTOR_TASK_SLOW;
MILLI msRotorTaskCheckPrev = 0;
MILLI msRotorFlickReverse  = 0;
BOOL  fDoRotorTaskWork     = false;

// Check for rotor stall and deal with it when it occurs.
// Call this fairly often with the blackboard lock held.
#define DoRotorTaskStallWork(msNow)                                                                 \
    {                                                                                               \
    if (fDoRotorTaskWork && msNow - msRotorTaskCheckPrev >= msRotorTaskInterval)                    \
        {                                                                                           \
        msRotorTaskCheckPrev = msNow;                                                               \
        CheckLockHeld(lockBlackboard);                                                              \
                                                                                                    \
        /* Compute our stall state */                                                               \
        if (FDoRotorStallCheck())                                                                   \
            {                                                                                       \
            CheckForRotorStall(msNow);                                                              \
            }                                                                                       \
                                                                                                    \
        /* Process that stall state accordingly */                                                  \
        switch (rotorStallState)                                                                    \
            {                                                                                       \
        case ROTOR_STALL_STATE_OK:                                                                  \
            msRotorTaskInterval = MS_ROTOR_TASK_SLOW;                                               \
            break;                                                                                  \
        case ROTOR_STALL_STATE_JUST_STALLED:                                                        \
            /* Reverse the rotor direction */                                                       \
            MoveServo(svoRotor, ReverseRotorSpeed(svposRotorTargetSpeed), true);                    \
            msRotorFlickReverse = msNow;                                                            \
            rotorStallState     = ROTOR_STALL_STATE_FLICK_REVERSE;                                  \
            msRotorTaskInterval = MS_ROTOR_TASK_FAST;                                               \
            break;                                                                                  \
        case ROTOR_STALL_STATE_FLICK_REVERSE:                                                       \
            /* If the reversing has gone on long enough, then progress forward again */             \
            if (msNow - msRotorFlickReverse >= 500)                                                 \
                {                                                                                   \
                MoveServo(svoRotor, svposRotorTargetSpeed, true);                                   \
                rotorStallState = ROTOR_STALL_STATE_OK;                                             \
                msRotorTaskInterval = MS_ROTOR_TASK_SLOW;                                           \
                }                                                                                   \
            break;                                                                                  \
        case ROTOR_STALL_STATE_STOP:                                                                \
            /* stop the rotor */                                                                    \
            MoveServo(svoRotor, svposRotorStop, true);                                              \
            msRotorTaskInterval = MS_ROTOR_TASK_SLOW;                                               \
            break;                                                                                  \
            }                                                                                       \
        }                                                                                           \
    }

#define StartRotorTask()        { fDoRotorTaskWork = true; }
#define StopRotorTask()         { fDoRotorTaskWork = false; }

#else
// No rotor stall check

#define StartRotorTask()
#define StopRotorTask()

#endif

// If we have a magnetic sensor, then turn on its detection whenever
// the rotor is spinning. Also keep track of the angle too, as we need
// to count it's rotations.

#if SensorIsDefined(sensnmMagRotor) && !SensorIsDefined(sensnmMagRotorAux)
    #define DetectMagOnRotorStart()     { StartDetectingMagneticSensor(sensMagRotor); }
    #define DetectMagOnRotorStop()      { StopDetectingMagneticSensor(sensMagRotor);  }
#elif SensorIsDefined(sensnmMagRotor) && SensorIsDefined(sensnmMagRotorAux)
    #define DetectMagOnRotorStart()     { StartDetectingMagneticSensor(sensMagRotor); StartDetectingMagneticSensor(sensMagRotorAux);  }
    #define DetectMagOnRotorStop()      { StopDetectingMagneticSensor(sensMagRotor);  StopDetectingMagneticSensor(sensMagRotorAux);   }
#else
    #define DetectMagOnRotorStart()
    #define DetectMagOnRotorStop()
#endif

#if SensorIsDefined(sensnmAngleRotor)
    #define ReadAngleOnRotorStart()   StartReadingAngleSensor(sensAngleRotor,true)
    #define ReadAngleOnRotorStop()    { StopReadingAngleSensor(sensAngleRotor); }
#else
    #define ReadAngleOnRotorStart()   0
    #define ReadAngleOnRotorStop()
#endif

// Change the rotor speed, being sure to set up / tear down whatever
// monitoring may be necessary.
void SetRotorSpeed(int svposSpeed)
    {
    int svposCurSpeed = GetServoValue(svoRotor);
    if (svposCurSpeed == svposRotorStop && (svposSpeed) != svposRotorStop)
        {
        /* the rotor is about to start */
        ReadAngleOnRotorStart();
        DetectMagOnRotorStart();
        StartRotorTask();
        }

    // Lock to synchronize with RotorTask
    LockBlackboard();

    // If reversing motor, reset stall detection. This avoids a bug in which
    // we accidentally but validly see the same rotor angle position twice (as
    // we retrace our steps) and mistakenly think we are stalled.
    if (svposSpeed*svposCurSpeed < 0)
        {
        ResetRotorStallCheck();
        }

    svposRotorTargetSpeed = svposSpeed;
    MoveServo(svoRotor, svposSpeed, true);
    ReleaseBlackboard();
    WaitForServos();

    if (svposCurSpeed != svposRotorStop && (svposSpeed) == svposRotorStop)
        {
        /* the rotor just stopped */
        StopRotorTask();
        DetectMagOnRotorStop();
        ReadAngleOnRotorStop();
        rotorStallState = ROTOR_STALL_STATE_OK;
        }
    }

//-----------------------------------------------------------------------------------
// User-level APIs
//-----------------------------------------------------------------------------------

#define StartRotor()   { SetRotorSpeed(svposRotorFast); }
#define StopRotor()    { SetRotorSpeed(svposRotorStop); }

#if ROBOT_NAME==ROBOT_NAME_FTC417_2010_V11
#define MS_ROTOR_DISPENSING_TIME          (4500+100) // 4500 is the absolute min to fully dispense (v11), and 100 is just a bit extra
#elif ROBOT_NAME==ROBOT_NAME_FTC417_2010_V12
#define MS_ROTOR_DISPENSING_TIME          (4800)
#else
#define MS_ROTOR_DISPENSING_TIME          0
#endif

// Assuming that the arm is in the correct dispensing position,
void UnloadDispenser(BATON_DISPOSITION batonDisposition)
    {
    StartRotorStallCheck();
    //
    fRotorDispensingStopValid = false;
    float degStart = ReadAngleOnRotorStart();
    float degCaptureStop = degStart + DEG_ROTOR_TURN_DISP_TO_MAG + DEG_ROTOR_TURN_MAG_CARRY_BATON;
    StartRotor();
    //
    // Rotate until either the time needed to dispense is complete
    // or until we reach a limit that someone indicated (such as
    // that which were we to go beyond we'd lose a magnetic baton)
    //
    MILLI msDispensingStop = nSysTime + MS_ROTOR_DISPENSING_TIME;
    BOOL fDone = false;
    while (!fDone && nSysTime < msDispensingStop)
        {
#if SensorIsDefined(sensnmAngleRotor)
        if (fRotorDispensingStopValid || CAPTURE_BATONS==batonDisposition)
            {
            LockBlackboard();
            //
            if (fRotorDispensingStopValid)
                fDone = fDone || sensAngleRotor.deg >= degRotorDispensingStop;
            //
            if (CAPTURE_BATONS==batonDisposition)
                fDone = fDone || sensAngleRotor.deg >= degCaptureStop;
            //
            ReleaseBlackboard();
            }
#endif
        EndTimeSlice();
        }
    //
    StopRotor();
    ReadAngleOnRotorStop();
    //
    StopRotorStallCheck();
    }

#if SensorIsDefined(sensnmAngleRotor)
// Assuming that the rotor is currently stopped, rotate it through
// the indicated number of degrees, then stop it again.
void TurnRotorBy(ANGLE angle, int svposSpeed)
    {
    // Calculate an appropriate timeout value. This is VERY rough.
    // Note also that the acheived rotor speed is HIGHLY non-linear
    // in the requested svpos value. More experimentation could give
    // us a mathematical model, but that work has not yet been done.
    MILLI msFullRotation;
    int dsvpos = abs(svposSpeed - 128);
    if (dsvpos < abs(svposRotorSlow - 128) / 2)
        {
        // *really* slow
        msFullRotation = 6000;
        }
    else if (dsvpos < abs(svposRotorFast - 128) / 2)
        {
        // slow
        msFullRotation = 3000;
        }
    else
        {
        // fast
        msFullRotation = 1500;
        }
    MILLI msTimeout = Rounded((float)msFullRotation / 360.0 * abs(angle), MILLI);
    //
    // Find out where the angle sensor is now so we can compute
    // what it will be when the turn is complete
    //
    ANGLE degCur    = StartReadingAngleSensor(sensAngleRotor, true);
    ANGLE degTarget = degCur + angle;
    //
    // Start things up and monitor until done.
    //
    SetRotorSpeed(svposSpeed);
    MILLI msTurnStart = nSysTime;
    BOOL fDone = false;
    while (!fDone && nSysTime - msTurnStart < msTimeout)
        {
        // be nice to other tasks
        EndTimeSlice();

        // Are we done yet?
        LockBlackboard();
        if (sensAngleRotor.deg >= degTarget)
            {
            fDone = true;
            }
        ReleaseBlackboard();
        }
    SetRotorSpeed(svposRotorStop);
    //
    StopReadingAngleSensor(sensAngleRotor);
    }
#endif

#else
// No servo rotor

#define StartRotor()                        {  }
#define StopRotor()                         {  }
#define UnloadDispenser(batonDisposition)   {  }

#endif

//-----------------------------------------------------------------------------------
// Internal nits
//-----------------------------------------------------------------------------------

#ifndef DoRotorTaskStallWork
#define DoRotorTaskStallWork(msNow)
#endif
