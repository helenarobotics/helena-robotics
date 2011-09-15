//
// Blackboard.h
//

//---------------------------------------------------------------------------------------
// Blackboard-related data
//---------------------------------------------------------------------------------------

int32         cBlackboardUpdate = 0;    // how many times has the blackboard been updated?
int32         msBlackboardUpdate;       // time at which the last blackboard update occurred

//---------------------------------------------------------------------------------------
// Reading the blackboard
//---------------------------------------------------------------------------------------

void CALLED_WITH_BB_LOCK UpdateBlackboard();

// Execute the blackboard update the blackboard even though we're not
// running on the blackboard task. This can usefully serve as the core
// 'idle' loop of things like turning and driving.
#define SpeculativelyUpdateBlackboard()                                         \
    {                                                                           \
    /* We just go ahead and refresh the data now? I mean, */                    \
    /* what else is there to do now, anyway?              */                    \
    /**/                                                                        \
    LockBlackboard();                                                           \
    UpdateBlackboard();                                                         \
    ReleaseBlackboard();                                                        \
    /**/                                                                        \
    /* Let other tasks run at least a bit. */                                   \
    /**/                                                                        \
    EndTimeSlice();                                                             \
    }

//---------------------------------------------------------------------------------------
// Ultrasonic
//---------------------------------------------------------------------------------------

// Wait until a new reading is available on this sonic sensor
#define WaitForNewSonicReading(sensor)                                                      \
    {                                                                                       \
    /* The rate at which ultrasonic sensor can be read is limited, both by the speed of */  \
    /* sound and internal implementation issues:                                        */  \
    /*      http://www.robotc.net/forums/viewtopic.php?f=1&t=2224&hilit=ultrasonic      */  \
    /* In short, the ultrasonic sensors take one reading in ~77ms, so if we want a new  */  \
    /* reading we need to be at least that long since the last one; we use 80 to be     */  \
    /* sure. More correctly, that was the native RobotC read speed; with us here using  */  \
    /* own I2C routines, we might be able to do faster (see end of the thread above).   */  \
    const MILLI msMin   = 80;                                                               \
    const MILLI msNow   = nSysTime;                                                         \
    const MILLI msDelta = msNow - sensor.msLastRead;                                        \
    const MILLI msWait  = msMin - msDelta;                                                  \
    if (msWait > 0)                                                                         \
        {                                                                                   \
        ReleaseBlackboard();                                                                \
        wait1Msec(msWait);                                                                  \
        LockBlackboard();                                                                   \
        }                                                                                   \
    /**/                                                                                    \
    sensor.msLastRead = nSysTime;                                                           \
    }

int CALLEDBY(iTaskMain) ReadSonic_Main(SONICSENSOR& sensor, BOOL fRequireFinite)
// We read until we get two readings in a row with basically the same value.
// Note that this function can a very long time to complete.
// Return cmSonicNil if there's no reading available; that will never happen
// if fAllowInfinite is false (we'll spin until that doesn't happen).
    {
    LockBlackboard();
    RawReadSonic(sensor); int cmCur = sensor.cm;
    int cmPrev;
    for (int iRetry=0; iRetry < 10 || (fRequireFinite && (cmSonicRawNil==cmCur || cmSonicRawNil==cmPrev)); iRetry++)
        {
        cmPrev = cmCur;
        WaitForNewSonicReading(sensor);
        RawReadSonic(sensor);
        cmCur  = sensor.cm;
        if (fRequireFinite && (cmSonicRawNil==cmCur || cmSonicRawNil==cmPrev))
            continue;
        if (Abs(cmCur - cmPrev) <= 1)
            break;
        }
    int result = cmCur==cmSonicRawNil ? cmSonicNil : cmCur;
    ReleaseBlackboard();
    return result;
    }

//---------------------------------------------------------------------------------------
// Compass
//
// REVIEW: This is a little stale, and needs updating
//---------------------------------------------------------------------------------------

#if SensorIsDefined(sensnmCompass)
ANGLE CALLEDBY(iTaskMain) ReadCompass()
// Access the compass value from the main task
    {
    LockBlackboard();
    ANGLE angle = sensCompass.value;
    ReleaseBlackboard();
    return angle;
    }
#else
#define ReadCompass()    (0.0)
#endif

#if SensorIsDefined(sensnmCompass)
void ZeroCompass()
    {
    LockBlackboard();
    sensCompass.valueZero = 0.0;
    ReadCompassSensor(sensCompass);
    sensCompass.valueZero = sensCompass.value;
    ReleaseBlackboard();
    }
#else
#define ZeroCompass()  {}
#endif

//---------------------------------------------------------------------------------------
// Initialization and update
//---------------------------------------------------------------------------------------

#if SensorIsDefined(sensnmCompass)
    #define DetectCompass() { StartReadingCompassSensor(sensCompass); }
#else
    #define DetectCompass() {}
#endif

#define InitializeBlackboard()                            \
    {                                                     \
    cBlackboardUpdate = 0;                                \
    DetectCompass();                                      \
    ZeroCompass();                                        \
    }

void CALLED_WITH_BB_LOCK UpdateBlackboard()
// Do the work of updating the blackboard. Note that (per Dave Schilling) we can't actually read
// the compass (or any of the other sensors?) faster than 100Hz, so we enforce a throttle here.
// That won't be significant when we're called from the BlackboardTask (it doesn't call that often),
// but might be when we're called manually via SpeculativelyUpdateBlackboard().
//
// REVIEW: further research indicates that's probably hookum, esp. when sensor are on a mux.
// But actual removal of that throttle needs more testing before it can be implemented.
    {
    const MILLI msBlackboardThrottle = 8;   // was 11
    MILLI msNow = nSysTime;                 // read the current clock
    //
    // We do the gyro's every time through because they're really sensitive time-wise.
    //
#if SensorIsDefined(sensnmGyroHorz)
    ReadGyroSensor(sensGyroHorz, msNow);
#endif
#if SensorIsDefined(sensnmGyroVert)
    ReadGyroSensor(sensGyroVert, msNow);
#endif
    //
    if (0==cBlackboardUpdate || (msNow - msBlackboardUpdate) > msBlackboardThrottle)
        {
        msBlackboardUpdate = msNow;
        //
#if SensorIsDefined(sensnmCompass)
        ReadCompassSensor(sensCompass);
#endif
#if SensorIsDefined(sensnmTouchPreload)
        ReadTouchSensor(sensTouchPreload);
#endif
#if SensorIsDefined(sensnmEopdArm)
        ReadEopdSensor(sensEopdArm);
#endif
#if SensorIsDefined(sensnmEopdFront)
        ReadEopdSensor(sensEopdFront);
#endif
#if SensorIsDefined(sensnmEopdFrontRight)
        ReadEopdSensor(sensEopdFrontRight);
#endif
#if SensorIsDefined(sensnmAngleRotor)
        ReadAngleSensor(sensAngleRotor);
#endif
#if SensorIsDefined(sensnmMagRotor)
        ReadMagneticSensor(sensMagRotor);
#endif
#if SensorIsDefined(sensnmMagRotorAux)
        ReadMagneticSensor(sensMagRotorAux);
#endif
#if SensorIsDefined(sensnmColor)
        ReadColorSensor(sensColor);
#endif
        //
        // Do other misc things that need doing
        //
        CheckForMotorStall(msNow);
        TickleServos(msNow);
        DoRotorTaskStallWork(msNow);
        //
        cBlackboardUpdate++;
        }
    }

task BlackboardTask()
    {
    // We just go round and round
    //
    for (;;)
        {
        // Since we're going to update the state in the blackboard, make
        // sure that no one is reading it at the moment, and that no one
        // will do so while we do our updates
        //
        LockBlackboard();
        //
        // Do the work to update things
        //
        UpdateBlackboard();
        //
        // Let others read the state
        //
        ReleaseBlackboard();
        //
        // Wait a while before we grab the lock again: otherwise, pretty
        // much no one will ever be able to get in to read it, since
        // we'd always have it locked.
        //
        wait1Msec(msBlackboardPolling);
        }
    }



//---------------------------------------------------------------------------------------
// Startup
//---------------------------------------------------------------------------------------

#define StartBlackboardTask()   { StartTask(BlackboardTask);    }

// We'd like to just 'suspendTask' & 'resumeTask', but apparently those functions aren't functional in RobotC
#define SuspendBlackboard()                                                 \
    {                                                                       \
    /* get the lock to ensure that the task isn't anywhere important */     \
    LockBlackboard();                                                       \
    StopTask(BlackboardTask);                                               \
    ReleaseBlackboard();                                                    \
    }
#define ResumeBlackboard()      { StartTask(BlackboardTask);   }
