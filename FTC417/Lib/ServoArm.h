//
// ServoArm.h
//
// Functionality related to moving the arms on the bot. Note that this file could
// still use some cleanup wrt configuration modularity.

#ifndef HAS_ARM_SERVOS
#define HAS_ARM_SERVOS      0
#endif
#ifndef HAS_WRIST_SERVOS
#define HAS_WRIST_SERVOS    0
#endif
#ifndef HAS_PRELOAD_SERVOS
#define HAS_PRELOAD_SERVOS  0
#endif

#ifndef USE_EXTERNAL_ARM_TIP_TEST
#define USE_EXTERNAL_ARM_TIP_TEST 0
#endif

#if !USE_EXTERNAL_ARM_TIP_TEST
//-----------------------------------------------------------------------------------
// Preload arm
//-----------------------------------------------------------------------------------

int PositionPreloadArm(int pos, MILLI msWaitIgnored=0)
// Position the preload arm to the indicated position, but
// make sure it's not outside of it's allowable range.
    {
#if HAS_PRELOAD_SERVOS
    ClampVar(pos, svposPreloadArmExtended, svposPreloadArmRetracted);
    MoveServo(svoPreloadArm, pos);
    WaitForServos();
#endif
    return pos;
    }

#if HAS_PRELOAD_SERVOS
#define ExtendPreloadArm()  PositionPreloadArm(svposPreloadArmExtended, 500)
#define RetractPreloadArm() PositionPreloadArm(svposPreloadArmRetracted, 500)
#else
#define ExtendPreloadArm()  { }
#define RetractPreloadArm() { }
#endif

void DumpPreload(BOOL fPreloadArmExtended)
    {
    TRACE(("dumping preload"));
    //
#if HAS_PRELOAD_SERVOS
    if (!fPreloadArmExtended)
        {
        ExtendPreloadArm();
        }
    //
    // open the gate to dump.
    //
    MoveServo(svoPreloadGate, svposPreloadGateOpen);
    WaitForServosMin(750);
    //
    // Retract back to safe position. We retract with the gate
    // open so as to be sure not to trap any of the batons when
    // we do close it.
    //
    PositionPreloadArm(svposPreloadArmReturnPosition, 1000);
    //
    // Close the gate
    //
    MoveServo(svoPreloadGate, svposPreloadGateClosed);
    WaitForServos();
#endif
    }

//-----------------------------------------------------------------------------------
// Low-level dispenser arm manipulation
//-----------------------------------------------------------------------------------

#define MoveDispenserArm(svposRotation, svposElbow, svposShoulder)  \
    {                                                               \
    MoveServo(svoArmRotation, svposRotation);                       \
    MoveServo(svoArmElbow,    svposElbow);                          \
    MoveServo(svoArmShoulder, svposShoulder);                       \
    }



//-----------------------------------------------------------------------------------
// Dispenser arm
//-----------------------------------------------------------------------------------

// Enums naming the various servos, logical arm positions, and dispensers involved
// in dispensing. These are use to parameterize distance and svpos queries.
typedef enum
    {
    SVOJNT_ELBOW    =0,
    SVOJNT_SHOULDER,
    SVOJNT_ORDER,
    SVOJNT_MAX,
    SVOJNT_ROTATION,        // nb: in rgsvposRotation, not in the tables, so *AFTER* _MAX
    } SVOJNT;

typedef enum
    {
    ARMPOS_EOPD     =0,
    ARMPOS_DISPINT  =1,
    ARMPOS_DISP     =2,
    ARMPOS_MAX      =3
    } ARMPOS;

typedef enum
    {
    DISP_HIGH       =0,
    DISP_MED        =1,
    DISP_LOW        =2
    } DISPENSER;

typedef enum
    {
    isvposRotationPacked,
    isvposRotationInt,
    isvposRotationEopd,
    isvposRotationMax,
    } ISVPOSROTATION;

#ifndef SdrAdj
#define SdrAdj(svpos)   (svpos)
#endif
#ifndef ElbAdj
#define ElbAdj(svpos)   (svpos)
#endif

#if HAS_ARM_SERVOS
// Designated positions of the rotation servo
int rgsvposRotation[] =
    {
    svposPackedRotation,            // packed
    svposIntRotation,
    233,                            // eopd, dispensing, etc
    };
#endif

#if HAS_ARM_SERVOS
int rgsvposHighDispenser[][SVOJNT_MAX*ARMPOS_MAX] =
    {   // 23
        { ElbAdj(158), SdrAdj(23)  , 0,
          ElbAdj(80),  SdrAdj(73)  , 0,
          ElbAdj(200), SdrAdj(93)  , 0},

        // 24
        { ElbAdj(180), SdrAdj(43)  , 0,
          ElbAdj(91),  SdrAdj(93)  , 0,
          ElbAdj(211), SdrAdj(103) , 0},

        // 25
        { ElbAdj(195), SdrAdj(50)  , 0,
          ElbAdj(135), SdrAdj(128) , 0,
          ElbAdj(245), SdrAdj(118) , 0},

        // 26
        { ElbAdj(204), SdrAdj(64)  , 0,
          ElbAdj(128), SdrAdj(90)  , 0,
          ElbAdj(255), SdrAdj(120) , 0},
    };

int rgsvposLowDispenser[][SVOJNT_MAX*ARMPOS_MAX] =
    {
        // 17
        { ElbAdj(85),  SdrAdj(20)  , 0,
          ElbAdj(145), SdrAdj(160) , 1,    // fBottomBeforeTop
          ElbAdj(225), SdrAdj(200) , 0},

        // 18
        { ElbAdj(85),  SdrAdj(20)  , 0,
          ElbAdj(145), SdrAdj(160) , 0,
          ElbAdj(225), SdrAdj(200) , 0},

        // 19
        { ElbAdj(85),  SdrAdj(15)  , 0,
          ElbAdj(135), SdrAdj(155) , 0,
          ElbAdj(205), SdrAdj(185) , 0},

        // 20
        { ElbAdj(105), SdrAdj(10)  , 0,
          ElbAdj(135), SdrAdj(140) , 0,
          ElbAdj(235), SdrAdj(200) , 0},

        // 21
        { ElbAdj(115), SdrAdj(25)  , 0,
          ElbAdj(145), SdrAdj(145) , 0,
          ElbAdj(215), SdrAdj(183) , 0},

        // 22
        { ElbAdj(154), SdrAdj(8)   , 0,
          ElbAdj(136), SdrAdj(140) , 0,
          ElbAdj(245), SdrAdj(200) , 0},

        // 23
        { ElbAdj(173), SdrAdj(36)  , 0,
          ElbAdj(113), SdrAdj(150) , 0,
          ElbAdj(233), SdrAdj(196) , 0}, // iffy dispensing
    };
#endif
                                  /* hi    mid  lo */
int rgcmDispenseTurnMin      [3] = { 18,   255, 18 };  // min dist need to even try to correct our distance from wall
int rgcmDispenseMin          [3] = { 23,   255, 17 };  // min dist at which can we dispense
int rgcmDispenseCorrect      [3] = { 24,   255, 20 };  // dist we move to if we correct our distance from the wall
int rgcmDispenseMax          [3] = { 26+1, 255, 23+1 };  // (one more than) last distance at which we can dispense
int rgcmUnpackSwingTopHighMax[3] = { 27,   255, 28 };  // at this distance or greater, we don't need to swing the top arm high to clear

// NOTE: Give current usage and RobotC bugs these MUST NOT call a function (though prob could call an intrinsic)
#define CmDispenseCanTurnMin(disp)     rgcmDispenseTurnMin[disp]
#define CmDispenseMin(disp)            rgcmDispenseMin[disp]
#define CmDispenseCorrect(disp)        rgcmDispenseCorrect[disp]
#define CmDispenseMax(disp)            rgcmDispenseMax[disp]
#define CmUnpackSwingTopHighMax(disp)  rgcmUnpackSwingTopHighMax[disp]

BOOL LookupServoPositions(DISPENSER disp, int cm, ARMPOS armPos, int& svposRotation, int& svposElbow, int& svposShoulder)
// Return the svpos for the indicated arm position of the servos at the
// indicated dispenser, given that the left ultrasound reads cm. If the parameters
// are out of bounds, return a negative number for the parameter
    {
    svposRotation = svposElbow = svposShoulder = -1; // assume failure
    BOOL fSuccess = false;
    //
#if HAS_ARM_SERVOS
    if (CmDispenseMin(disp) <= cm && cm < CmDispenseMax(disp))
        {
        int dcm         = cm-CmDispenseMin(disp);        // rgsvpos arrays are indexed starting with the min value
        int isvpos      = armPos*SVOJNT_MAX;             // form index into the 2-d matrix manually
        //
        if (DISP_HIGH == disp)
            {
            svposRotation = rgsvposRotation[isvposRotationEopd];
            svposElbow    = rgsvposHighDispenser[dcm][isvpos+SVOJNT_ELBOW];
            svposShoulder = rgsvposHighDispenser[dcm][isvpos+SVOJNT_SHOULDER];
            fSuccess = true;
            }
        else if (DISP_LOW == disp)
            {
            svposRotation = rgsvposRotation[isvposRotationEopd];
            svposElbow    = rgsvposLowDispenser[dcm][isvpos+SVOJNT_ELBOW];
            svposShoulder = rgsvposLowDispenser[dcm][isvpos+SVOJNT_SHOULDER];
            fSuccess = true;
            }
        }
#endif
    //
    return fSuccess;
    }

int LookupServoPosition(DISPENSER disp, int cm, ARMPOS armPos, SVOJNT svojnt)
// Return the svpos for the indicated arm position of the indicated servo at the
// indicated dispenser, given that the left ultrasound reads cm. If the parameters
// are out of bounds, return a negative number.
//
// REVIEW: We should really simplify things and get rid of this function.
    {
    int svposRotation;
    int svposElbow;
    int svposShoulder;
    if (LookupServoPositions(disp, cm, armPos, OUT svposRotation, OUT svposElbow, OUT svposShoulder))
        {
        switch (svojnt)
            {
        case SVOJNT_ROTATION: return svposRotation;
        case SVOJNT_ELBOW:    return svposElbow;
        case SVOJNT_SHOULDER: return svposShoulder;
            }
        }
    return -1;
    }

#define SvposEOPD(disp, cm, svojnt)     LookupServoPosition(disp, cm, ARMPOS_EOPD,    svojnt)
#define SvposDispInt(disp, cm, svojnt)  LookupServoPosition(disp, cm, ARMPOS_DISPINT, svojnt)
#define SvposDisp(disp, cm, svojnt)     LookupServoPosition(disp, cm, ARMPOS_DISP,    svojnt)

//--------------------------------------------------------------------------------------------

void RaiseTopArmForPackOrUnpack(DISPENSER disp, int cm, int svposCur)
// Where the top arm needs to be as we pack or unpack from the EOPD
// position depends on how close we are to the dispenser.
    {
#if HAS_ARM_SERVOS
    if (cm < CmUnpackSwingTopHighMax(disp))
        {
        // The elbow always has an apogee defined
        MoveServo(svoArmElbow, svoArmElbow.svposApogee);
        WaitForServos();
        }
    else
        {
        if (svposCur != svposIntElbow)
            {
            MoveServo(svoArmElbow, svposIntElbow);
            WaitForServos();
            }
        }
#endif
    }

#if HAS_ARM_SERVOS

    #define MoveDispenserFromPackedToInt()                                                              \
        {                                                                                               \
        /* Move to the intermediate position from the packed position*/                                 \
        MoveDispenserArm(svposIntRotation, svposIntElbow, svposIntShoulder);                            \
        WaitForServos();                                                                                \
        }

    #define MoveDispenserFromIntToPacked()                                                              \
        {                                                                                               \
        /* Move to the packed position from the intermediate position*/                                 \
        MoveDispenserArm(svposIntToPackedRotation, svposIntToPackedElbow, svposIntToPackedShoulder);    \
        WaitForServos();                                                                                \
        MoveDispenserArm(svposPackedRotation, svposPackedElbow, svposPackedShoulder);                   \
        WaitForServos();                                                                                \
        }
#else

    #define MoveDispenserFromPackedToInt() { }
    #define MoveDispenserFromIntToPacked() { }

#endif

// The arm is assumed to be in its packed position. Raise it and rotate
// over to the left side of the bot to the position in which we can use the
// EOPD on the end of the arm to sense the location of the baton dispenser.
BOOL UnpackDispenserArmForReadingEopd(DISPENSER disp, int cm, SIDE side)
    {
    BOOL fSuccess = HAS_ARM_SERVOS;
#if ROBOT_NAME==ROBOT_NAME_FTC417_2010_V12

    MoveServo(svoArmElbow, 60);
    WaitForServos();

    if (LEFT_SIDE == side)
        {
        // NOT WELL TESTED
        MoveServo(svoArmRotation, svposEopdRotation);
        WaitForServos();

        MoveArmTipTo(cm - 10 + 5, 16.5, MOVEARMTIP_DIRECT);
        WaitForServos();
        }
    else
        {
        MoveServo(svoArmWrist, svposArmWristHorizontal);
        MoveServo(svoArmRotation, svposPackedRotation - 3);
        WaitForServos();

        const float dcmOriginToRightSonic = (21.5 - 0.8);
        float x = cm - 9.5 + dcmOriginToRightSonic; // cant reach from 48cm
        float y = 13.5;

        float xCur, yCur;
        GetArmTipLocation(OUT xCur, OUT yCur);

        // TRACE(("cm=%d", cm));
        // TRACE(("move to %f %f", x, yCur));

        MoveArmTipTo(x, yCur, MOVEARMTIP_DIRECT);
        WaitForServos();
        MoveArmTipTo(x, y,    MOVEARMTIP_DIRECT);
        WaitForServos();
        }

#elif ROBOT_NAME==ROBOT_NAME_FTC417_2010_V11
    // We have an intermediate staging position about half way along
    // to the full EOPD reading position.
    MoveDispenserFromPackedToInt();

    // Raise the top of the arm to where it needs to be to clear the wall as we rotate
    MoveServo(svoArmShoulder, svoArmShoulder.svposApogee);
    RaiseTopArmForPackOrUnpack(disp, cm, svposIntElbow);

    // Rotate to the EOPD position.
    int svoposRotation = rgsvposRotation[isvposRotationEopd];
    MoveServo(svoArmRotation, svoposRotation);
    WaitForServos();

    // From the Reference point, we have to subtact 10cm from the distance
    // from the wall to get to our point. We add 5 because the reference point
    // is NOT the origin from the geometry and the y coordinate stays the same. This gets
    // the arm right in front of the dispensor, not in dispensing mode.
    //
    // Move the arm down to the indicated point, which is only approximately horizontal
    // in the forewarm. (Note: MoveArmTo does the WaitForServos internally.)
    MoveArmTipTo(cm - 10 + 5, 16.5, MOVEARMTIP_DIRECT);

    // Level out the forearm so the EOPD sensor on the end is perpendicular to the wall.
    // That should help it get more reliable readings.
    AdjustForearmAngle(0.0);
#endif
    return fSuccess;
    }

#if ROBOT_NAME==ROBOT_NAME_FTC417_2010_V11
    #define PackArmSafelyFromAnywhere(fConservativelyRotate)     { PackArmSimply(); WaitForServos(); }
#elif ROBOT_NAME==ROBOT_NAME_FTC417_2010_V12

    void PackArmSafelyFromAnywhere(BOOL fConservativelyRotate)
        {
        // If the arm is down low we really need to be conservative lest the
        // basically linear path from the current location into the packed one
        // intersects the side of the robot
        //
        // Needs more thinking: nice idea, but very unexpected if manual packing moves
        // have been made in interim.
        if (!fConservativelyRotate && false)
            {
            float x, y;
            GetArmTipLocation(x, y);
            if (y < 0)
                fConservativelyRotate = true;
            }

        // If we're not at the right rotation, we can't really tell what to do
        if (!fConservativelyRotate)
            {
            if (svposPackedRotation != GetServoValue(svoArmRotation))
                fConservativelyRotate = true;
            }

        // In the conservative case, up goes the arm and we rotate
        if (fConservativelyRotate)
            {
            MoveServo(svoArmElbow, svoArmElbow.svposApogee);
            WaitForServos();
            MoveServo(svoArmRotation, svposPackedRotation);
            WaitForServos();
            }

        // Finally, do the easy, putting everything to bed
        MoveServo(svoArmWrist, svposArmWristHorizontal);
        PackArmSimply();
        WaitForServos();
        }
#else
    #define PackArmSafelyFromAnywhere(fConservativelyRotate)     {  }
#endif


BOOL PackDispenserArmFromEOPD(DISPENSER disp, int cm, SIDE side)
// Inverse of UnpackDispenserArmForReadingEopd
    {
    BOOL fSuccess = HAS_ARM_SERVOS;
#if ROBOT_NAME==ROBOT_NAME_FTC417_2010_V12

    PackArmSafelyFromAnywhere(false);

#elif ROBOT_NAME==ROBOT_NAME_FTC417_2010_V11
    int svposShoulder = svoArmShoulder.svposApogee;
    int svposElbow = GetServoValue(svoArmElbow);
    MoveServo(svoArmShoulder, svposShoulder);
    RaiseTopArmForPackOrUnpack(disp, cm, svposElbow);

    // Swing back to intermediate rotation
    MoveServo(svoArmRotation, svposIntRotation);
    WaitForServosOld(250);

    // Set top & bottom to intermediate positions
    MoveDispenserArm(svposIntRotation, svposIntElbow, svposIntShoulder);
    WaitForServosOld(250);

    // Pack it away
    MoveDispenserFromIntToPacked();
#endif
    return fSuccess;
    }

//--------------------------------------------------------------------------------------------

// This is a structure which describes points along a path from the EOPD
// position to the dispensing position.

typedef struct
    {
    BOOL  fSvpos;
    int   svposShoulder;
    int   svposElbow;
    float x;
    float y;
    } ARMPOINT;

typedef struct
    {
    int         cpt;
    ARMPOINT    rgpt[3];
    } DISPENSINGPATH;

#define InitializeDispensingPath(path)  { path.cpt = 0; }

#define AddToDispensingPath(path, ptFrom)                               \
    {                                                                   \
    path.rgpt[path.cpt].fSvpos        = ptFrom.fSvpos;                  \
    path.rgpt[path.cpt].svposElbow    = ptFrom.svposElbow;              \
    path.rgpt[path.cpt].svposShoulder = ptFrom.svposShoulder;           \
    path.rgpt[path.cpt].x             = ptFrom.x;                       \
    path.rgpt[path.cpt].y             = ptFrom.y;                       \
    path.cpt++;                                                         \
    }

// Compute the path from the EOPD to the dispensing position. NOTE: the EOPD position itself is always
// the first point in the returned path.
#define ComputeDispensingPath(fSuccess, path, disp, cm)                                                                     \
    {                                                                                                                       \
    InitializeDispensingPath(path);                                                                                         \
    ARMPOINT pt;                                                                                                            \
                                                                                                                            \
    /* The first point in the path is the EOPD position, which is where we currently are */                                 \
    pt.svposShoulder = GetServoValue(svoArmShoulder);                                                                       \
    pt.svposElbow    = GetServoValue(svoArmElbow);                                                                          \
    pt.fSvpos        = true;                                                                                                \
    AddToDispensingPath(path, pt);                                                                                          \
                                                                                                                            \
    /* We have an intermediate dispensing position, and a dispensing position */                                            \
    int svposRotation, svposElbow, svposShoulder;                                                                           \
    fSuccess = LookupServoPositions(disp, cm, ARMPOS_DISPINT, OUT svposRotation, OUT svposElbow, OUT svposShoulder);        \
    if (fSuccess)                                                                                                           \
        {                                                                                                                   \
        pt.svposShoulder = svposShoulder;                                                                                   \
        pt.svposElbow    = svposElbow;                                                                                      \
        AddToDispensingPath(path, pt);                                                                                      \
        fSuccess = LookupServoPositions(disp, cm, ARMPOS_DISP, OUT svposRotation, OUT svposElbow, OUT svposShoulder);       \
        if (fSuccess)                                                                                                       \
            {                                                                                                               \
            pt.svposShoulder = svposShoulder;                                                                               \
            pt.svposElbow    = svposElbow;                                                                                  \
            AddToDispensingPath(path, pt);                                                                                  \
            }                                                                                                               \
        }                                                                                                                   \
    }

// Move the arm to the indicated ARMPOINT
#define MoveToArmPoint(pt, fForward)                                                            \
    {                                                                                           \
    if (!pt.fSvpos)                                                                             \
        {                                                                                       \
        MoveArmTipTo(pt.x, pt.y, MOVEARMTIP_DIRECT);                                            \
        }                                                                                       \
    else if (fForward)                                                                          \
        {                                                                                       \
        MoveServo(svoArmElbow,    pt.svposElbow);                                               \
        MoveServo(svoArmShoulder, pt.svposShoulder);    WaitForServos();                        \
        }                                                                                       \
    else                                                                                        \
        {                                                                                       \
        MoveServo(svoArmShoulder, pt.svposShoulder);                                            \
        MoveServo(svoArmElbow,    pt.svposElbow);       WaitForServos();                        \
        }                                                                                       \
    }

// We're at the EOPD position. Move to dispensing, reporting the path we take to get there.
// It is assumed here that the EOPD and dispensing positions have the same rotational component.
BOOL MoveFromEOPDToDispensing(OUT DISPENSINGPATH& path, DISPENSER disp, int cm, BOOL fRotate)
    {
    BOOL fSuccess = HAS_ARM_SERVOS;
#if HAS_ARM_SERVOS
    ComputeDispensingPath(fSuccess, path, disp, cm);
    if (fSuccess)
        {
        // Do all the movements but one. We skip the first point, as that's the EOPD position, which we're already at
        for (int i = 1; i < path.cpt-1; i++)
            {
            MoveToArmPoint(path.rgpt[i], true);
            }

        // Start the rotor for the last movement, if asked
        if (fRotate)
            {
            StartRotor();
            }

        // Do the last movement
        MoveToArmPoint(path.rgpt[path.cpt-1], true);
        }
#endif
    return fSuccess;
    }


//-----------------------------------------------------------------------------------
// Initialization
//-----------------------------------------------------------------------------------

#if HAS_ARM_SERVOS
// Is the arm rotation servo ad one of the indicated positions? We use the half-way
// point between the positions as angle of demarcation.
BOOL FRotationAt(ISVPOSROTATION isvposRotation)
    {
    int svposLowThreshold  =
        (0==isvposRotation
            ? 0
            : (rgsvposRotation[isvposRotation] + rgsvposRotation[isvposRotation-1]) / 2);

    int svposHighThreshold =
        (isvposRotationMax-1==isvposRotation
            ? svposLast
            : (rgsvposRotation[isvposRotation] + rgsvposRotation[isvposRotation+1]) / 2);

    int svpos = GetServoValue(svoArmRotation);

    return Between(svposLowThreshold, svpos, svposHighThreshold);
    }
#endif

// Is the dispenser arm at the packed rotation? Other locations?
#if ROBOT_NAME==ROBOT_NAME_FTC417_2010_V11
    #define FRotationPacked()           FRotationAt(isvposRotationPacked)
    #define FRotationIntermediate()     FRotationAt(isvposRotationInt)
    #define FRotationEopd()             FRotationAt(isvposRotationEopd)
#elif ROBOT_NAME==ROBOT_NAME_FTC417_2010_V12
    #define FRotationPacked()           FRotationAt(isvposRotationPacked)   // REVIEW
    #define FRotationIntermediate()     FRotationAt(isvposRotationInt)      // REVIEW
    #define FRotationEopd()             FRotationAt(isvposRotationEopd)     // REVIWE
#else
    #define FRotationPacked()           true
    #define FRotationIntermediate()     false
    #define FRotationEopd()             false
#endif

void MoveServosToInitialPositions()
    {
#if HAS_ARM_SERVOS
    // Did the 12v power just come on?
    BOOL fRotSvoControllerJustPowered = MainServoControllerOf(svoArmRotation).fJustPowered;
    if (fRotSvoControllerJustPowered)
        {
        // If the servo controllers just powered on, if we try to move ANY servo on a controller,
        // then ALL the servos on that controller will move, whether or not we've told the controller
        // that we'd like them to go to something other than their default '128' svpos. That's to
        // be avoided, as '128' is almost never exactly what we want it to move to (though it's indeed
        // safe (assuming range of motion ok), since paired servos won't fight (much) and rotational
        // servos are stopped.
        //
        // So what we do is 'move' ALL our servos to the packed location, but don't 'WaitForServos'
        // until they're all done. We will, though, assume that the arm was put in what's close
        // to the packed position.
        //
        PackArmSimply();
        MoveServo(svoRotor, svposRotorStop);
#if HAS_WRIST_SERVOS
        MoveServo(svoArmWrist, svposArmWristHorizontal);
#endif
        // Now we're in the state that if any of the servos are powered, reasonable things will happen.
        }
#endif
#if HAS_WRIST_SERVOS
    MoveServo(svoArmWrist, svposArmWristHorizontal);
#endif
#if HAS_PRELOAD_SERVOS
    MoveServo(svoPreloadArm,  svposPreloadArmRetracted);
    MoveServo(svoPreloadGate, svposPreloadGateClosed);
#endif
#if HAS_ARM_SERVOS
    PackArmSafelyFromAnywhere(false);
    MoveServo(svoRotor, svposRotorStop);
#endif
    // Wait for all the movement to complete
    WaitForServos();
    }

#endif
