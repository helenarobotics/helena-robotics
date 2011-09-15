//
// Servos.h
//
// Core servo definitions and functionality.

//-----------------------------------------------------------------------------------
// SERVO struct and functions related thereto
//-----------------------------------------------------------------------------------

// Maximum servo position (per hardware)
#define svposLast 255

// Distinguish between the two different kinds of servos
typedef enum { SVOKIND_STANDARD, SVOKIND_CNTROT } SVOKIND;

typedef struct
    {
    BOOL    fActive;                // if false, then we ought not to try to move this servo
    int     isvo;                   // index of this structure in rgsvo

    // Primary servo
    int     isvoctlrMain;           // the servo controller for the main servo of this SERVO
    int     isvcMain;               // the index of the main servo on that servo controller

    BOOL    fReversed;              // whether we should invert the any incoming values to this servo
    int     svposLower;             // smallest valid value for this servo
    int     svposApogee;            // position of highest ascent of arm
    int     svposUpper;             // largest valid value for this servo

    float   dsvposToRadiansRaw;     // scaling constant for this servo to convert increments of svpos to increments of radians
    float   gearing;
    float   dsvposToRadians;
    float   radiansToDsvpos;

    // Paired servo
    int     isvoctlrPaired;         // which servo controller is the paired servo on
    int     isvcPaired;             // which servo is it on that controller
    BOOL    fReflectedPaired;       // if true, then paired gets 255-svpos; false: svpos
    int     dsvposPaired;           // value to add to the calculated value for the paired servo to get the actual value to use

    // Tripped servo
    int     isvoctlrTripped;        // which servo controller is the tripped servo on
    int     isvcTripped;            // which servo is it on that controller
    BOOL    fReflectedTripped;      // if true, then tripped gets 255-svpos; false: svpos
    int     dsvposTripped;          // value to add to the calculated value for the tripped servo to get the actual value to use

    // Quad servo
    int     isvoctlrQuad;           // which servo controller is the quad servo on
    int     isvcQuad;               // which servo is it on that controller
    BOOL    fReflectedQuad;         // if true, then quad gets 255-svpos; false: svpos
    int     dsvposQuad;             // value to add to the calculated value for the tripped servo to get the actually value to use

    // Characterization and display
    string  displayName;            // for human consumption
    SVOKIND kind;                   // standard vs continuous rotation

    // Waiting support
    float   msPerSvpos;             // how many ms does it take to move one svpos?
    float   scaleDurUp;             // factor to scale msPerSvposRate when raising arm
    float   scaleDurDown;           // factor to scale msPerSvposRate when lowering arm
    MILLI   msExtra;                // extra amount of time to allow on each move

    // Dynamic info
    MILLI   msCommandComplete;      // when will most recent last command complete for this servo?
    BOOL    fServoValueValid;       // REVIEW: old comment: can we ask ServoValue where this servo is?
    } SERVO;

// An FTC configuration can only support at most 12 servos; we
// allow for them all here (uggh: RobotC seems to preclude more
// elegant approaches).
SERVO rgsvo[12];
int   isvoMax = -1;                 // index of last valid entry in rgsvo[]

#define HasPairedServo(svo)         (svo.isvcPaired >= 0)
#define HasTrippedServo(svo)        (svo.isvcTripped >= 0)
#define HasQuadServo(svo)           (svo.isvcQuad >= 0)

#define MainServoControllerOf(svo)  rgsvoctlr[svo.isvoctlrMain]
#define MainSvcOf(svo)              MainServoControllerOf(svo).rgsvc[svo.isvcMain]

// Structure containing the current state of the arm for movement thereof
typedef struct
    {
    int     svposShoulder;          // current position of the shoulder servo
    int     svposElbow;             // current position of the elbow servo
    float   radShoulder;            // svposShoulder converted to radians
    float   radElbow;               // svposElbow converted to radians
    POINT   ptTip;                  // the location of the tip of the arm
    POINT   ptTipCenter;            // the location of the center of rotation of the forearm
    } ARMTIPSTATE;

// Function for testing whether a given servo position is valid for the given servo, etc
#define FValidSvpos(svo, svpos)                     Between(svo.svposLower, svpos, svo.svposUpper)
#define FValidArmSvposs(svposShoulder, svposElbow)  (FValidSvpos(svoArmShoulder, svposShoulder) && FValidSvpos(svoArmElbow, svposElbow))
#define FValidArmTipState(state)                    FValidArmSvposs(state.svposShoulder, state.svposElbow)

// Set the waiting parameters for a servo
#define SetServoWaits(svo, scaleUp, scaleDown, msExtraParam)        \
    {                                                               \
    svo.scaleDurUp   = scaleUp;                                     \
    svo.scaleDurDown = scaleDown;                                   \
    svo.msExtra      = msExtraParam;                                \
    }

#define SetServoGearing(svo, gearingVal)                            \
    {                                                               \
    svo.gearing = gearingVal;                                       \
    svo.dsvposToRadians = svo.dsvposToRadiansRaw / gearingVal;      \
    svo.radiansToDsvpos = 1.0 / svo.dsvposToRadians;                \
    }

// Initialize an individual SERVO.
void InitializeServo(
        IN OUT STICKYFAILURE& fOverallSuccess,
        SERVO& svo,
        string displayName,
        SVOCTLR& controllerMain,    int jsvcMain,   /* one based */
        SVOCTLR& controllerPaired,  int jsvcPaired, /* one based */
        SVOKIND kind, int fActive, float degPhysicalServoRange=195.0)
    {
    BOOL fSuccess = true;
    svo.fActive           = !!fActive;

    /* Figure out which index this servo is in the rgsvo array */
    svo.isvo = -1;
    int i = 0;
    while (rgsvo[i].isvo >= 0)
        i++;
    svo.isvo = i;
    MaxVar(isvoMax, i);

    svo.isvoctlrMain      = controllerMain.isvoctlr;
    svo.isvcMain          = jsvcMain -1;
    if (svo.isvcMain < 0) fSuccess = false; // We require the main servo to be always there, but there may or may not be a paired servo.

    svo.isvoctlrPaired    = controllerPaired.isvoctlr;
    svo.isvcPaired        = jsvcPaired -1;
    svo.fReflectedPaired  = true;
    svo.dsvposPaired      = 0;

    svo.isvoctlrTripped   = 0;
    svo.isvcTripped       = -1;
    svo.fReflectedTripped = false;
    svo.dsvposTripped     = 0;

    svo.isvoctlrQuad      = 0;
    svo.isvcQuad          = -1;
    svo.fReflectedQuad    = false;
    svo.dsvposQuad        = 0;

    svo.fReversed         = false;
    int svposPer20ms      = 10;         // OLD COMMENT: default value is '10'; can't make much faster, but could make slower
    svo.msPerSvpos        = 20. / (float)svposPer20ms;
    svo.svposLower        = 0;
    svo.svposApogee       = -1;                                 // NB: no apogee by default. REVIEW.
    svo.svposUpper        = svposLast;
    svo.scaleDurUp        = 3.;                                 // just a guess for a default
    svo.scaleDurDown      = 2.;                                 // just a guess for a default
    svo.msExtra           = (SVOKIND_STANDARD==kind ? 0 : 0);   // just a guess for a default
    svo.displayName       = displayName;
    svo.kind              = kind;
    //
    const float radPhysicalServoRange = degreesToRadians(degPhysicalServoRange);
    const float dsvposServoRange      = 255.;       // per specifications
    svo.dsvposToRadiansRaw   = radPhysicalServoRange / dsvposServoRange;
    SetServoGearing(svo, 1.0);
    //
    svo.msCommandComplete = nSysTime;
    svo.fServoValueValid  = !controllerMain.fJustPowered;       // nb: we assume all ctlrs are on the same power source
    //
    if (fOverallSuccess) fOverallSuccess = fSuccess;
    TraceInitializationResult1("svo[%s]", displayName, fOverallSuccess);
    }




void WaitForServos(MILLI msMinWait=0, BOOL fBeep=true)
// Wait for all the servos to get to their commanded positions, or for
// the indicated minimum wait, whichever is greater.
    {
    BOOL fSuccess = true;

    // Propagate any pending commands to any of the servo controllers
    LockDaisy();
    for (int isvoctlr = 0; isvoctlr < isvoctlrMax; isvoctlr++)
        {
        if (rgsvoctlr[isvoctlr].fActive)
            {
            SendServoPositions(fSuccess, rgsvoctlr[isvoctlr]);
            }
        }
    ReleaseDaisy();

    // Carry out the wait
    if (!USE_SERVO_CONTROLLER_WAITS)
        {
        // Do the wait based on math we've calibrated
        MILLI msComplete = msStart;
        for (int isvo = 0; isvo < (int)isvoMax; isvo++)
            {
            if (rgsvo[isvo].fActive)
                {
                MILLI ms = rgsvo[isvo].msCommandComplete;
                if (msComplete < ms)
                    msComplete = ms;
                }
            }
        MILLI msNow   = nSysTime;
        MILLI msWaitT = msComplete - msNow;
        MILLI msWait  = Max(msMinWait, msWaitT);

        if (msWait > 0)
            {
            wait1Msec(msWait);
            if (fBeep) { Beep(NOTE_F); }
            }
        }
    else
        {
        // Actually ask the servo controllers themselves whether any servos are still moving
        BOOL fAllComplete;
        BOOL fDidAnyWaiting = false;
        LockDaisy();
        do  {
            fAllComplete = true;
            for (int isvoctlr = 0; fAllComplete && isvoctlr < isvoctlrMax; isvoctlr++)
                {
                if (rgsvoctlr[isvoctlr].fActive)
                    {
                    fSuccess = true;
                    BOOL fComplete;
                    QueryServoMovementComplete(fSuccess, rgsvoctlr[isvoctlr], fComplete);
                    if (fSuccess)
                        {
                        fAllComplete = fAllComplete && fComplete;
                        }
                    else
                        fAllComplete = false;
                    }
                }
            if (!fAllComplete)
                {
                // Let others run while we wait here
                fDidAnyWaiting = true;
                ReleaseDaisy();
                EndTimeSlice();
                LockDaisy();
                }
            }
        while (!fAllComplete);
        ReleaseDaisy();
        //
        if (fBeep && fDidAnyWaiting) { Beep(NOTE_F); }
        }
    }

#define WaitForServosNoBeep()               WaitForServos(0,     false)
#define WaitForServosMin(msMin)             WaitForServos(msMin)
#define WaitForServosOld(msOld)             WaitForServos(0)
#define WaitForServosMinOld(msMin, msOld)   WaitForServos(msMin)

// Internal support
#define ExtToIntServo(svo,svpos)            (svo.fReversed ? svposLast - (svpos) : (svpos))
#define IntToExtServo(svo,svpos)            (svo.fReversed ? svposLast - (svpos) : (svpos))

// Return the current location of the indicated servo
#define GetInternalServoValue(svo)          MainSvcOf(svo).svpos
#define GetServoValue_(svo)                 IntToExtServo(svo,GetInternalServoValue(svo))

// We were experiencing code-gen bugs when (what's now) GetServoValue_() was
// expanded often inside one function. So now we expand it just once, inside
// the GetServoValue *function*, which everyone else then calls.
int GetServoValue(SERVO& svo)
    {
    return GetServoValue_(svo);
    }

BOOL MoveServo(SERVO& svo, int svposNew, BOOL fMoveImmediately = false)
// Start the servo moving to a new location, and figure out when it's going to get there.
    {
    BOOL fSuccess = true;
    if (svo.fActive)
        {
        // svposNew and the svo.svposLower etc positions in svo are in external (unreversed) orientation

        // Only position this servo within its configured allowable range
        ClampVar(svposNew, svo.svposLower, svo.svposUpper);

        // Reverse the requested value if necessary
        if (svo.fReversed)
            svposNew = svposLast - svposNew;

        // From here on, svpos are internal

        // Figure out how long it will take to complete the command. Are we raising or
        // lowering the servo? That affects which rate-scaling constant to use.
        // If we know the apogee, we calculate up/down based thereon; if we don't
        // then 'up' is simply the direction of increasing svpos.
        int svposCur   = GetInternalServoValue(svo);
        int dsvpos     = svposNew - svposCur;
        MILLI msCommandDuration;
        //
        if (SVOKIND_CNTROT == svo.kind)
            {
            // We don't actually need to wait much for continuous rotation servos
            msCommandDuration = 10;   // a guess, needs testing
            }
        else if (!svo.fServoValueValid)
            {
            // We have to be entirely pessimistic, since we don't really know where the servo is
            float msPerSvpos = svo.msPerSvpos * Max(svo.scaleDurUp, svo.scaleDurDown);
            msCommandDuration = Rounded((float)svposLast * msPerSvpos, long);
            }
        else if (svo.svposApogee >= 0)
            {
            int svposApogee = ExtToIntServo(svo,svo.svposApogee);
            int dApogeeCur  = svposCur - svposApogee;
            int dApogeeNew  = svposNew - svposApogee;

            // We calculate the rates for moving 'up' and 'down' separately. The path to the
            // apogee and the path after the apogee have different rates.
            if ((dApogeeCur < 0 && dApogeeNew > 0) || (dApogeeCur > 0 && dApogeeNew < 0))
                {
                // The path passes *through* the apogee. The first part of that path
                // is 'up'; the second part is 'down'.
                float factorUp   = svo.scaleDurUp   * (float)Abs(dApogeeCur);
                float factorDown = svo.scaleDurDown * (float)Abs(dApogeeNew);
                msCommandDuration = Rounded(svo.msPerSvpos*(factorUp + factorDown), long);
                // TRACE(("up=%.1f down=%.1f", factorUp * svo.msPerSvpos, factorDown * svo.msPerSvpos));
                }
            else
                {
                // The path remains on one side of the apogee
                BOOL fUp = Abs(dApogeeNew) <= Abs(dApogeeCur);
                float msPerSvpos = svo.msPerSvpos * (fUp ? svo.scaleDurUp : svo.scaleDurDown);
                msCommandDuration = Rounded((float)abs(dsvpos) * msPerSvpos, long);
                // TRACE(("dsvpos=%d %s", dsvpos, (fUp ? "up" : "down")));
                }
            }
        else
            {
            // No apogee; 'up' is just the direction of increasing svpos
            BOOL fUp = dsvpos >= 0;
            float msPerSvpos = svo.msPerSvpos * (fUp ? svo.scaleDurUp : svo.scaleDurDown);
            msCommandDuration = Rounded((float)abs(dsvpos) * msPerSvpos, long);
            // TRACE(("dsvpos=%d %s", dsvpos, (fUp ? "inc" : "dec")));
            }

        // Add a margin of safety
        msCommandDuration += svo.msExtra;

        // Record in our servo controllers where we want them to go.

        // Deal with the main servo
        if (true)
            {
            // REVIEW: why doesn't the main 'fReversed' logic work exactly like the fReflected stuff?
            ClampVar(svposNew, 0, svposLast);
            RecordSvoPosition(svo.isvoctlrMain, svo.isvcMain, svposNew);
            }

        // Deal with the paired servo
        if (HasPairedServo(svo))
            {
            // Figure out where the paired servo is going to go
            int svposPaired = (svo.fReflectedPaired  ? svposLast - svposNew : svposNew);
            svposPaired += svo.dsvposPaired;
            ClampVar(svposPaired,  0, svposLast);
            RecordSvoPosition(svo.isvoctlrPaired, svo.isvcPaired, svposPaired);
            }

        // Deal with the tripped servo
        if (HasTrippedServo(svo))
            {
            int svposTripped = (svo.fReflectedTripped ? svposLast - svposNew : svposNew);
            svposTripped += svo.dsvposTripped;
            ClampVar(svposTripped, 0, svposLast);
            RecordSvoPosition(svo.isvoctlrTripped, svo.isvcTripped, svposTripped);
            }

        // Deal with the Quad servo
        if (HasQuadServo(svo))
           {
           int svposQuad = (svo.fReflectedQuad ? svposLast - svposNew : svposNew);
           svposQuad += svo.dsvposQuad;
           ClampVar(svposQuad, 0, svposLast);
           RecordSvoPosition(svo.isvoctlrQuad, svo.isvcQuad, svposQuad);
           }

        // If we've been asked to, then move the servos immediately (otherwise we'll wait
        // until a WaitForServos call to start things going)
        if (fMoveImmediately)
            {
            LockDaisy();
            if (0 <= svo.isvoctlrMain)    SendServoPositions(fSuccess, rgsvoctlr[svo.isvoctlrMain]);
            if (0 <= svo.isvoctlrPaired)  SendServoPositions(fSuccess, rgsvoctlr[svo.isvoctlrPaired]);
            if (0 <= svo.isvoctlrTripped) SendServoPositions(fSuccess, rgsvoctlr[svo.isvoctlrTripped]);
            if (0 <= svo.isvoctlrQuad)    SendServoPositions(fSuccess, rgsvoctlr[svo.isvoctlrQuad]);
            ReleaseDaisy();
            }

        // Having commanded that we go there, we can reasonably expect that we know
        // where the servo actually is in order to judge future command durations.
        svo.fServoValueValid = true;

        // Remember when this servo will be done
        svo.msCommandComplete = nSysTime + msCommandDuration;
        }

    return fSuccess;
    }
