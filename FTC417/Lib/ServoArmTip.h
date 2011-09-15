//
// ServoArmTip.h

// Movement of the arm tip is carried out by moving the tip to a set of intermediate 'waypoints'
// along the way to the destination. Waypoints are close enough together such that linear motion
// in servo arm space (where the coordinates are svposShoulder and svposElbow) is a reasonable
// approximation to linear motion in real geometric space: generally, of course, that is not true,
// as the geometry of how the arm move warps things. Having this linear approximation is the
// chief reason for computing waypoints.
//
// With that in mind, there are three modes of arm tip movement which may be specified:
//
//      linear - move the tip in a smooth linear line in real geometric space
//      step   - (rare) move the tip from way point to way point, but don't try to be smoothly
//               linear in so doing
//      direct - just move the arm tip directly to the final destination, in one fell swoop
//
typedef enum
    {
    MOVEARMTIP_LINEAR,         // move the arm in a smooth *linear* path to the destination
    MOVEARMTIP_STEP,           // (rare) move the arm from waypoint to waypoint, but don't worry about linearity
    MOVEARMTIP_DIRECT,         // just move the arm directly to the destination: just set the servos and go
    } MOVEARMTIP;

// The following functions are the main public routines in this file. MoveArmTo moves
// the arm directly to the indicated point, while MoveArmBy by indicated deltas from its
// current location.
//
// If the current svpos of the shoulder and elbow are known, they can be passed as a small
// performance optimization; if they are unknown, then -1 should be passed, and the servos
// will be queried for their current positions. It's always safe to pass -1 for these arguments,
// and this is recommended unless you have good reason to do otherwise.
//
// The boolean return values indicate whether the operation was successful (true) or whether
// the geometry of the arm prevents that location from being reached (false).
//
BOOL MoveArmTipTo(float x, float y,  MOVEARMTIP mode, int svposShoulder=-1, int svposElbow=-1);
BOOL MoveArmTipBy(float dx, float dy, MOVEARMTIP mode, int svposShoulder=-1, int svposElbow=-1);

// AdjustForearmAngle adjusts the elbow joint so that the forearm makes the indicated
// angle with the ground (nb: NOT the indicated angle with the upper arm). Note that,
// as usual, angles are measured in a CCW direction.
BOOL AdjustForearmAngle(ANGLE degTarget, int svposShoulder=-1, int svposElbow=-1);

//--------------------------------------------------------------------------------------

#ifndef HAS_ARM_SERVOS
#define HAS_ARM_SERVOS      0
#endif
#ifndef HAS_WRIST_SERVOS
#define HAS_WRIST_SERVOS    0
#endif
#ifndef HAS_PRELOAD_SERVOS
#define HAS_PRELOAD_SERVOS  0
#endif

//--------------------------------------------------------------------------------------

// Function for doing some tracing of where we think the arm tip currently is
#if !defined(ReportArmState)
    #define ReportArmState(sz, state)                                           \
        {                                                                       \
        }
#endif

// Function for actually moving the arm tip
#if !defined(MoveArmToWaypointStep)
    #define MoveArmToWaypointStep(svposShoulder, svposElbow, fServosValid)      \
        {                                                                       \
        fServosValid = FValidArmSvposs(svposShoulder, svposElbow);              \
        if (fServosValid)                                                       \
            {                                                                   \
            MoveServo(svoArmShoulder, svposShoulder);                           \
            MoveServo(svoArmElbow,    svposElbow);                              \
            WaitForServosNoBeep();                                              \
            }                                                                   \
        }
#endif

//--------------------------------------------------------------------------------------
// Sending new servo positions to the servo controller can consume a non-trival amount
// of time: the message is 8 bytes long, and each byte is 8 bits + 1 ack bit in length
// (I think; close enough for what we need here anyway). At 30kHz signalling speed, that
// takes about 2.5ms. This is in the same ballpark as the approx 4ms it takes to move
// an unloaded servo a distance of one svpos. Thus, there is a concern that asking for a lot of
// small movements (as can happen during Bresenham) might slow down the servos due
// to messaging overhead.
//
// (That, and using a non-zero value for SERVOCONTROLLER_STEP_TIME (necessary for having
// the servo controller tell us when it's done) is slower than the zero value, at least
// for small movements.)
//
// Therefore, taking our cue from RobotC's approach (I can't believe I said that), we
// throttle our small movements, only sending them out every once in a while.

typedef struct
    {
    int     svposShoulder;
    int     svposElbow;
    BOOL    fDirty;
    int     cSend;
    MILLI   msSendPrev;
    } BRESENSTEPSTATE;

#define InitializeWaypointStepThrottle(state)                               \
    {                                                                       \
    state.fDirty     = false;                                               \
    state.cSend      = 0;                                                   \
    state.msSendPrev = nSysTime;                                            \
    }

#define FinishWaypointStepThrottle(state, fForce)                           \
    {                                                                       \
    if (state.fDirty)                                                       \
        {                                                                   \
        if (!USE_SERVO_CONTROLLER_WAITS || fForce || 0 == state.cSend)      \
            {                                                               \
            MILLI msNow = nSysTime;                                         \
            MoveServo(svoArmShoulder, state.svposShoulder);                 \
            MoveServo(svoArmElbow,    state.svposElbow);                    \
            WaitForServosNoBeep();                                          \
            /**/                                                            \
            state.fDirty = false;                                           \
            state.cSend++;                                                  \
            state.msSendPrev = msNow;                                       \
            }                                                               \
        else                                                                \
            {                                                               \
            state.cSend++;                                                  \
            if (3 == state.cSend)                                           \
                {                                                           \
                state.cSend = 0;                                            \
                }                                                           \
            }                                                               \
        }                                                                   \
    }

#define MoveArmToWaypointStepThrottle(svposShoulderVal, svposElbowVal, state, fServosValid)      \
    {                                                                       \
    fServosValid = FValidArmSvposs(svposShoulderVal, svposElbowVal);        \
    if (fServosValid)                                                       \
        {                                                                   \
        state.svposShoulder = svposShoulderVal;                             \
        state.svposElbow    = svposElbowVal;                                \
        state.fDirty        = true;                                         \
        FinishWaypointStepThrottle(state, false);                           \
        }                                                                   \
    }

//--------------------------------------------------------------------------------------

BOOL MoveArmToWaypoint(ARMTIPSTATE& stateCur, ARMTIPSTATE& stateWaypoint, MOVEARMTIP mode)
// Move the arm from it's current state to that of the waypoint, in a manner
// indicated by the current mode.
    {
#if HAS_ARM_SERVOS
    ArmTipAngleToSvpos(stateWaypoint);

    BOOL fServosValid = true;

    // If a linear movement is requested, use Bresenham's algorithm to get there.
    // Note we are linear in the servo position space, NOT in real-world geometric
    // space.
    if (MOVEARMTIP_LINEAR == mode)
        {
        // A very readable and pleaseant description of Bresenham's line drawing
        // algorithm can be found here (well worth the read: it's a cute, well
        // known and useful algorithm):
        //
        //      http://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
        //
        // Since we have to physically move servos in time order rather than setting
        // random access pixels on a screen, we here use the variation which "control[s]
        // the points in order of appearance" (but with a bug fixed in the code shown
        // in the article: the second swap needed to be omitted).
        //
        int y0 = stateCur.svposElbow;
        int y1 = stateWaypoint.svposElbow;
        int x0 = stateCur.svposShoulder;
        int x1 = stateWaypoint.svposShoulder;
        //
        BRESENSTEPSTATE stateBresen;
        InitializeWaypointStepThrottle(stateBresen);
        //
        BOOL fSteep = abs(y1 - y0) > abs(x1 - x0);
        if (fSteep)
            {
            int t;
            Swap(x0, y0, t);
            Swap(x1, y1, t);
            }
        //
        int dxErr = abs(x1 - x0);
        int dyErr = abs(y1 - y0);
        int err   = dxErr / 2;
        int y     = y0;
        int dx = (x0 < x1 ? 1 : -1);
        int dy = (y0 < y1 ? 1 : -1);
        //
        int x = x0;
        while (fServosValid)
            {
            if (fSteep)
                {
                MoveArmToWaypointStepThrottle(y, x, IN OUT stateBresen, OUT fServosValid);
                }
            else
                {
                MoveArmToWaypointStepThrottle(x, y, IN OUT stateBresen, OUT fServosValid);
                }
            //
            if (x == x1)
                break;
            //
            err -= dyErr;
            if (err < 0)
                {
                y += dy;
                err += dxErr;
                }
            //
            x += dx;
            }

        // Make sure we send any pending final movement and wait for it to complete.
        FinishWaypointStepThrottle(IN OUT stateBresen, true);
        }
    else
        {
        // No Bresenham: just go there directly.
        MoveArmToWaypointStep(stateWaypoint.svposShoulder, stateWaypoint.svposElbow, OUT fServosValid);
        }

    // Update the state variable. Note that for the sake of stability of the outer algorithm it's important
    // that we *don't* round/quantize this to where the arm actually went but rather to where we wanted it
    // to go ideally.
    AssignStruct(stateCur, stateWaypoint);

    // Report where we are
    ReportArmState("w ", stateCur);

    // Answer whether we got all the way there, or only part way
    return fServosValid;
#else
    return false;
#endif
    }


BOOL ComputeElbowRotationToPoint(IN OUT ARMTIPSTATE& stateWaypointInOut, IN POINT& ptWaypoint)
// Given that the state of the arm is as found in stateWaypoint, rotate (only)
// the elbow so the arm tip reaches ptWaypoint. That ptWaypoint can be reached
// in this way is guaranteed by caller.
    {
    BOOL fGeometryCanReach = HAS_ARM_SERVOS;
#if HAS_ARM_SERVOS
    // RobotC has bugs involving nested structs, so accessing stateWaypointInOut.ptTip, for example,
    // just won't work. So we copy the struct to a local entry and copy it back on exit.
    ARMTIPSTATE stateWaypoint; AssignStruct(stateWaypoint, stateWaypointInOut);

    const float radElbowCur = stateWaypoint.radElbow;

    const float cmTolerance = 0.1;                                  // we aim for 1mm of the target
    const float sqcmTolerance = square(cmTolerance);

    const float dsvposQuantum = 15;                                 // seems to work ok
    const float dradQuantumElbow = dsvposQuantum * svoArmElbow.dsvposToRadians;

    // Figure out which direction to move the elbow
    float dradElbowSmall = dradQuantumElbow;
    POINT ptTipTheta; AssignPoint(ptTipTheta, stateWaypoint.ptTip);
    float sqdYThetaNextMove = sqdist(ptTipTheta, ptWaypoint);
    //
    int cFlip = 0;
    for (;;)
        {
        stateWaypoint.radElbow = radElbowCur + dradElbowSmall;
        ComputeArmTipLocation(stateWaypoint);
        int iCompare; circularOrder(OUT iCompare, ptTipTheta, ptWaypoint, stateWaypoint.ptTip, sqdYThetaNextMove);
        if (iCompare < 0)
            {
            dradElbowSmall = -dradElbowSmall;   // We moved in the wrong direction
            cFlip++;
            if (cFlip==2)
                {
                fGeometryCanReach = false;
                goto Done;
                }
            }
        else if (iCompare==0)
            {
            break;                              // We moved toward ptWaypoint, and not too far. Goldilocks
            }
        else
            {
            cFlip = 0;
            dradElbowSmall *= 0.5;              // We moved too far.
            }
        }

    // OK. dradElbowSmall moves us in the right direction, and not too far. Next
    // we find a multple of dradElbowSmall that moves us too far; together these
    // will form a bracket around our target.
    float dradElbowBig = timesTwo(dradElbowSmall);
    for (;;)
        {
        stateWaypoint.radElbow = radElbowCur + dradElbowBig;
        ComputeArmTipLocation(stateWaypoint);
        int iCompare; circularOrder(OUT iCompare, ptTipTheta, ptWaypoint, stateWaypoint.ptTip, sqdYThetaNextMove);
        if (iCompare < 0)
            {
            // code completeness: we sould never actually reach this branch
            fGeometryCanReach = false;
            goto Done;
            }
        else if (iCompare==0)
            {
            dradElbowBig *= 2;             // We moved toward ptWaypoint, but not overshooting it
            }
        else
            {
            break;
            }
        }

    // Now dradElbowSmall and dradElbowBig span what we're looking for. Refine the bracket
    // until we get close enough, and we're done!
    for (;;)
        {
        float dradElbowMid = (dradElbowSmall + dradElbowBig) * 0.5;

        stateWaypoint.radElbow = radElbowCur + dradElbowMid;
        ComputeArmTipLocation(stateWaypoint);

        float sqd = sqdist(stateWaypoint.ptTip, ptWaypoint);
        if (sqd < sqcmTolerance)
            break;

        int iCompare; circularOrder(OUT iCompare, ptTipTheta, ptWaypoint, stateWaypoint.ptTip, sqdYThetaNextMove);
        if (iCompare < 0)
            {
            // code completeness: we sould never actually reach this branch
            fGeometryCanReach = false;
            goto Done;
            }
        else if (iCompare==0)
            {
            dradElbowSmall = dradElbowMid;
            }
        else
            {
            dradElbowBig = dradElbowMid;
            }
        }

Done:
    AssignStruct(stateWaypointInOut, stateWaypoint);
#endif
    return fGeometryCanReach;
    }




BOOL MoveFromTo(ARMTIPSTATE& stateStart, float x, float y, MOVEARMTIP mode)
// Given that the arm is in the indicated start location, move it to ptTo. If fLinear is true,
// then move it smoothly there in a straight line as best we can; if false, then move it in
// one fell swoop. Answer whether we moved all the way to the destination (we might move just
// part way, if we can't physically get there).
    {
    TRACE(("--at      %.2f,%.2f %d %d", stateStart.ptTip.x, stateStart.ptTip.y, stateStart.svposShoulder, stateStart.svposElbow));
    TRACE(("--move to %.2f,%.2f", x, y));
#if HAS_ARM_SERVOS

    POINT ptTo;
    ptTo.x = x;
    ptTo.y = y;

    // Can the logical geometry of the arm let us reach where we're told to go?
    BOOL fGeometryCanReach = true;

    // The geometry aside, are the servo positions necessary to get there in fact valid ones?
    BOOL fServosValid = true;

    // Some constants that govern our movement
    const float cmTolerance = 0.1;                                  // we aim for 1mm of the target
    const float sqcmTolerance = square(cmTolerance);
    const float dsvposQuantum = 15;                                 // seems to work ok
    const float dradQuantumShoulder = dsvposQuantum * svoArmShoulder.dsvposToRadians;

    // Report the first point (for debugging: we're already there)
    ReportArmState("s ", stateStart);

    // Loop a long of series of moderately spaced waypoints until we get to where we are going
    ARMTIPSTATE stateCur, stateFinal;
    AssignStruct(stateCur, stateStart);
    while (fGeometryCanReach && fServosValid)
        {
        // Any point might be our last (we might give up moving further), so prepare for that.
        AssignStruct(stateFinal, stateCur);

        // Are we close enough to the target to be said to be done?
        const float sqdTipCurTo = sqdist(stateCur.ptTip, ptTo);
        if (sqdTipCurTo <= sqcmTolerance)
            break;

        // Set up for figuring out where to go next
        ARMTIPSTATE stateWaypoint; AssignStruct(stateWaypoint, stateCur);

        // Figure out the incremement by which we should move radShoulder. We start by guessing an
        // increment for radShoulder, then seeing if it's viable, and adjusting if not.
        float dradShoulder = dradQuantumShoulder;
        POINT ptWaypoint;
        int cFlip = 0;
        for (;;)
            {
            // Have we run out of radShoulder attempts?
            if (0 == dradShoulder)
                {
                // REVIEW: needs more testing, but at least we don't infinite loop
                fGeometryCanReach = false;
                goto Done;
                }

            // Where would the move in radShoulder alone take us?
            stateWaypoint.radShoulder = stateCur.radShoulder + dradShoulder;
            ComputeArmTipLocation(stateWaypoint);

            // Intersect that new position with the line of movement and calculate some distances.
            // Before the increment, one of these points was coincident with ptTip, and the other
            // further away. We assume that the increment is small enough so that former is still
            // in fact nearest.
            POINT ptU, ptV;
            intersectLineCircle(ptU, ptV, stateCur.ptTip, ptTo, stateWaypoint.ptTipCenter, rTip);

            // If the math blows up then in fact there's no intersection
            FLOAT_LONG pair;
            BOOL fFinite; IsFinite(fFinite, pair, ptU.x);
            if (fFinite) IsFinite(fFinite, pair, ptU.y);
            if (fFinite) IsFinite(fFinite, pair, ptV.x);
            if (fFinite) IsFinite(fFinite, pair, ptV.y);
            if (!fFinite)
                {
                // There is no intersection - we stepped away too far
                dradShoulder *= 0.5;
                continue;
                }

            // Which of U&V is closest to where we'd move to?
            if (sqdist(stateWaypoint.ptTip, ptU) < sqdist(stateWaypoint.ptTip, ptV))
                { AssignPoint(ptWaypoint, ptU); }
            else
                { AssignPoint(ptWaypoint, ptV); }

            // What's the relative ordering?
            int iCompare; linearOrder(OUT iCompare, stateCur.ptTip, ptTo, ptWaypoint, sqdTipCurTo);
            if (iCompare < 0)
                {
                dradShoulder = -dradShoulder;       // We went the wrong way
                cFlip++;
                if (cFlip==2)
                    {
                    fGeometryCanReach = false;
                    goto Done;
                    }
                }
            else if (iCompare == 0)
                {
                break;                              // We went the right way, and not too far.
                }
            else
                {
                cFlip = 0;
                dradShoulder *= 0.5;                // We went too far; we overshot
                }
            }

        // Ok: rotate the elbow until we hit ptWaypoint. We just need to figure out
        // what angle will get us there. Were that it was easy to figure that out.
        fGeometryCanReach = ComputeElbowRotationToPoint(IN OUT stateWaypoint, IN ptWaypoint);
        if (!fGeometryCanReach)
            goto Done;

        // Except in direct mode, we visit all the intermediate way points
        if (MOVEARMTIP_DIRECT != mode)
            {
            // Move the arm there
            fServosValid = MoveArmToWaypoint(stateCur, stateWaypoint, mode);
            }
        else
            {
            // Just move there conceptually; we'll actually move the arm at the end
            AssignStruct(stateCur, stateWaypoint);
            }
        }

Done:

    // In direct mode, we make only this one movement at the end (in the other modes we
    // will have already moved here).
    //
    if (MOVEARMTIP_DIRECT == mode)
        {
        if (fServosValid)
            {
            fServosValid = MoveArmToWaypoint(stateStart, stateFinal, mode);
            }
        }

    ReportArmState("e ", stateFinal);

    return fGeometryCanReach && fServosValid;
#else
    return false;
#endif
    }

//---------------------------------------------------------------------------------------

#define MoveArmProlog(svposShoulder, svposElbow)                            \
    ARMTIPSTATE stateCur;                                                   \
    {                                                                       \
    if (svposShoulder < 0) svposShoulder = GetServoValue(svoArmShoulder);   \
    if (svposElbow    < 0) svposElbow    = GetServoValue(svoArmElbow);      \
    stateCur.svposShoulder = svposShoulder;                                 \
    stateCur.svposElbow    = svposElbow;                                    \
    ArmTipSvposToAngle(stateCur);                                           \
    ComputeArmTipLocation(stateCur);                                        \
    }

BOOL GetArmTipLocation(OUT float& x, OUT float& y)
    {
#if HAS_ARM_SERVOS
    int svposShoulder = -1, svposElbow = -1;
    MoveArmProlog(svposShoulder, svposElbow);
    x = stateCur.ptTip.x;
    y = stateCur.ptTip.y;
    return true;
#else
    x = y = 0;
    return false;
#endif
    }

BOOL MoveArmTipTo(float x, float y, MOVEARMTIP mode, int svposShoulder, int svposElbow)
// Given that the arm is at the indicated location, move it to the indicated point
    {
#if HAS_ARM_SERVOS
    MoveArmProlog(svposShoulder, svposElbow);
    return MoveFromTo(stateCur, x,y , mode);
#else
    return false;
#endif
    }

BOOL MoveArmTipBy(float dx, float dy, MOVEARMTIP mode, int svposShoulder, int svposElbow)
// Given that the arm is in the indicated position, move it by the indicated amount
    {
#if HAS_ARM_SERVOS
    MoveArmProlog(svposShoulder, svposElbow);
    return MoveFromTo(stateCur, stateCur.ptTip.x + dx, stateCur.ptTip.y + dy, mode);
#else
    return false;
#endif
    }

// AdjustForearmAngle adjusts the elbow joint so that the forearm makes the indicated
// angle with the ground (nb: NOT the indicated angle with the upper arm). Note that,
// as usual, angles are measured in a CCW direction.
BOOL AdjustForearmAngle(ANGLE degTarget, int svposShoulder, int svposElbow)
    {
    BOOL fSuccess = true;
TRACE(("AdjustForearmAngle"));
    //
#if HAS_ARM_SERVOS
    // Find out the svpos and angles of the shoulder and elbow, and the
    // location of the arm tip and its center of rotation.
    MoveArmProlog(svposShoulder, svposElbow);

    // Compute the location that the arm tip will be at when
    // it's at the indicated angle relative to the ground. This
    // just takes some simple trig.
    float radTarget = degreesToRadians(degTarget);
    float cmForearm = dist(stateCur.ptTip, stateCur.ptTipCenter);
    POINT ptTarget;
    ptTarget.x      = stateCur.ptTipCenter.x + cmForearm * cos(radTarget);
    ptTarget.y      = stateCur.ptTipCenter.y + cmForearm * sin(radTarget);

    // Compute what rotation of the elbow will take us to that target
    ARMTIPSTATE stateTarget; AssignStruct(stateTarget, stateCur);
    ReportArmState("s", stateTarget);
    TRACE(("%.2f %.2f", stateCur.ptTipCenter.x, stateCur.ptTipCenter.y));
    TRACE(("%.2f %.2f", ptTarget.x, ptTarget.y));

    fSuccess = ComputeElbowRotationToPoint(stateTarget, ptTarget);
    if (fSuccess)
        {
        // Actually rotate the arm tip to that position
        fSuccess = MoveArmToWaypoint(stateCur, stateTarget, MOVEARMTIP_DIRECT);
        }
#else
    fSuccess = false;
#endif
    //
    return fSuccess;
    }
