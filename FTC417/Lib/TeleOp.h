//
// TeleOp.h
//

//-----------------------------------------------------------------------------
// State (nb: oldish)
//-----------------------------------------------------------------------------
BOOL fPreloadArmExtended = false;
BOOL fDispenserAtEOPD = false;
DISPENSER disp = DISP_MED;

//-----------------------------------------------------------------------------
// Joystick switching management
//-----------------------------------------------------------------------------
#ifndef USE_JOYSTICK_CONTROLLER_MODES
#define USE_JOYSTICK_CONTROLLER_MODES 1
#endif

typedef enum { JYC1_MODE_PASSIVE, JYC1_MODE_DRIVE, JYC1_MODE_ARM } JYC1_MODE;
typedef enum { JYC2_MODE_PASSIVE, JYC2_MODE_ARM } JYC2_MODE;

JYC1_MODE jyc1Mode = JYC1_MODE_DRIVE;
JYC2_MODE jyc2Mode = JYC2_MODE_ARM;

#define SetJoy1Mode(mode)                                               \
{                                                                       \
    if (JYC1_MODE_DRIVE == mode && JYC1_MODE_DRIVE != jyc1Mode) {       \
        jyc1Mode = JYC1_MODE_DRIVE;                                     \
        jyc2Mode = JYC2_MODE_ARM;                                       \
    } else if (JYC1_MODE_ARM == mode && JYC1_MODE_ARM != jyc1Mode) {    \
        jyc1Mode = JYC1_MODE_ARM;                                       \
        jyc2Mode = JYC2_MODE_PASSIVE;                                   \
    }                                                                   \
}

#define CheckJoy1ModeChange(jyc,joystick)
{
#if USE_JOYSTICK_CONTROLLER_MODES
    if (joyBtnOnce(jyc, JOYBTN_TOP_LEFT)) {
        Beep(NOTE_C, 30);
        SetJoy1Mode(JYC1_MODE_DRIVE);
    } else if (joyBtnOnce(jyc, JOYBTN_TOP_RIGHT)) {
        Beep(NOTE_G, 30);
        SetJoy1Mode(JYC1_MODE_ARM);
    }
#endif
}

//-----------------------------------------------------------------------------
// Logic common to both joysticks
//-----------------------------------------------------------------------------
BOOL
DoCommonJoystick(int jyc) {
    if (0) {
    }
    //----------------------------------------------------------
    // Running dispensing logic
    //----------------------------------------------------------
#if ROBOT_NAME==ROBOT_NAME_FTC417_2010_V12
    else if (joyHatOnce(jyc, JOYHAT_LEFT)) {
        // Configuring for dispensing from the high dispenser
        disp = DISP_HIGH;
    } else if (joyHatOnce(jyc, JOYHAT_UP)) {
        // Configuring for dispensing from the middle dispenser
        disp = DISP_MED;
    } else if (joyHatOnce(jyc, JOYHAT_RIGHT)) {
        // Configure for dispensing from the low dispenser
        disp = DISP_LOW;
    } else if (joyBtnOnce(jyc, JOYBTN_RIGHTTRIGGER_UPPER)) {
        // Run the automated dispensing logic.
        //
        // Idea: driver heads straight to goal, aligning approx close to
        // 90deg head it. We
        //      * turn 90 deg to the left
        //      * drive forward a bit, then do
        //        DriveForwardsAndStraightenUsingRightSonic
        //      * call PositionUsingEOPD to position us at the edge of
        //        the dispenser
        //      * call DispenseFromEOPDPosition to actually carry out
        //        the dispensing.
        // (some of that is now here)
        int cmFromWallCur = ReadSonic_Main(sensSonicRight, false);
        if (255 != cmFromWallCur) {
            // GetOverIt.h
            if (PositionUsingEOPD(disp, cmFromWallCur, RIGHT_SIDE)) {
                if (DispenseFromEOPDPosition(disp, cmFromWallCur,
                        STOP_BEFORE_DISPENSING)) {
//                    PackDispenserArmFromEOPD(disp, cmFromWallCur, RIGHT_SIDE);      // SerovArm.h
                } else {
                    PackArmSafelyFromAnywhere(false);
                }
            } else {
                // Make *sure* the arm isn't left out there dangling for
                // someone to hit
                PackArmSafelyFromAnywhere(false);
            }
        }
    }
#endif
    return true;
}

//-----------------------------------------------------------------------------
// Logic for the main drive joystick
//-----------------------------------------------------------------------------
BOOL
DoDriveJoystick(int jyc) {
    BOOL fSuccess = true;

    CheckJoy1ModeChange(jyc, joystick);
    DoManualDrivingControl(jyc, joystick);      // in DriveAids.h
    DoCommonJoystick(jyc);

    if (0) {
    }
    //----------------------------------------------------------
    // Scoring (preloaded) batons
    //----------------------------------------------------------
#if ROBOT_NAME==ROBOT_NAME_FTC417_2010_V12
    else if (joyBtnOnce(jyc, 1)) {
        ScoreBatonsInRollingGoal(ROLLING_GOAL_1);
    } else if (joyBtnOnce(jyc, 3)) {
        ScoreBatonsInRollingGoal(ROLLING_GOAL_3);
    } else if (joyBtnOnce(jyc, 4)) {
        ScoreBatonsInRollingGoal(ROLLING_GOAL_4);
    }
#endif

#if ROBOT_NAME==ROBOT_NAME_FTC417_2010_V11
    else if (joyBtnOnce(jyc, JOYBTN_LEFTTRIGGER_UPPER)) {
        // Extend or retract the preload arm
        //
        if (!fPreloadArmExtended) {
            ExtendPreloadArm();
            fPreloadArmExtended = true;
        } else {
            RetractPreloadArm();
            fPreloadArmExtended = false;
        }
    } else if (joyBtnOnce(jyc, JOYBTN_LEFTTRIGGER_LOWER)) {
        // Dump the preloads (might be useful if there's a baton stuck
        // there somehow)
        if (fPreloadArmExtended) {
            DumpPreload(true);
        } else {
            DumpPreload(false);
            RetractPreloadArm();
        }
    }
#endif

    //----------------------------------------------------------
    // Defence
    //----------------------------------------------------------
#if ROBOT_NAME==ROBOT_NAME_FTC417_2010_V12
    else if (joyBtnOnce(jyc, JOYBTN_LEFTTRIGGER_UPPER)) {
        PackArmSafelyFromAnywhere(true);
    } else if (joyBtnOnce(jyc, JOYBTN_LEFTTRIGGER_LOWER)) {
        // Set the arm swinging freely so that it won't get damaged by
        // bots pushing on it
        DisableServoController(fSuccess,
            MainServoControllerOf(svoArmRotation));
    }
#endif

#if ROBOT_NAME==ROBOT_NAME_FTC417_2010_V11
    else if (joyBtnOnce(jyc, 1)) {
        // Pack arm normally
        PackArmSimply();
        WaitForServosOld(2000);
    } else if (joyBtnOnce(jyc, 2)) {
        // Pack arm from dead intermediate position
        MoveDispenserArm(svposIntRotation, svposIntElbow, svposIntShoulder);
        WaitForServosOld(500);

        MoveDispenserFromIntToPacked();
        WaitForServosOld(2000);
    } else if (joyBtnOnce(jyc, 3)) {
        // Pack arm from dead EOPD position
        int cm = 23;
        DISPENSER disp = DISP_LOW;

        int svposRotation, svposElbow, svposShoulder;
        LookupServoPositions(disp, cm, ARMPOS_DISPINT, svposRotation,
            svposElbow, svposShoulder);
        MoveDispenserArm(svposRotation, svposElbow, svposShoulder);
        WaitForServosOld(500);

        LookupServoPositions(disp, cm, ARMPOS_EOPD, svposRotation, svposElbow,
            svposShoulder);
        MoveDispenserArm(svposRotation, svposElbow, svposShoulder);
        WaitForServosOld(500);

        PackDispenserArmFromEOPD(disp, cm);
    }
#endif

#if ROBOT_NAME==ROBOT_NAME_FTC417_2010_V11
    else if (joyHatOnce(jyc, JOYHAT_UP)) {
        // Configuring for dispensing from the high dispenser
        disp = DISP_HIGH;
    } else if (joyHatOnce(jyc, JOYHAT_DOWN)) {
        // Configure for dispensing from the low dispenser
        disp = DISP_LOW;
    } else if (joyBtnOnce(jyc, JOYBTN_RIGHTTRIGGER_UPPER)) {
        // Toggle: deploy EOPD and position bot front-to-back / pack
        // from EOPD
        int cm = ReadSonic_Main(sensSonicLeft, false);
        if (cmSonicNil != cm) {
            if (!fDispenserAtEOPD) {
                fDispenserAtEOPD = PositionUsingEOPD(disp, cm);
            } else {
                PackDispenserArmFromEOPD(disp, cm);
                fDispenserAtEOPD = false;
            }
        }
    } else if (joyBtnOnce(jyc, JOYBTN_RIGHTTRIGGER_LOWER)) {
        // Dispense from EOPD
        int cm = ReadSonic_Main(sensSonicLeft, false);
        if (cmSonicNil != cm) {
            DispenseFromEOPDPosition(disp, cm);
        }
    }
#endif

    //----------------------------------------------------------
    // Cleanup
    //----------------------------------------------------------
    else
        fSuccess = false;

    return fSuccess;
}

//-----------------------------------------------------------------------------
// Logic for the 'arm' joystick
//-----------------------------------------------------------------------------

MILLI msTelemetryBias;
BOOL fmsTelemetryBiasSet = false;

// Given a displacement on a joystick how far should that move a servo
float
ScaleJoystickThrottle(float djoy, float range) {
    float scale = range / (128.0 - (float)joyThrottleDeadZone);
    float result =
        (float)(abs(djoy) - joyThrottleDeadZone) * scale * sgn(djoy);
    return result;
}

#if ROBOT_NAME==ROBOT_NAME_FTC417_2010_V12
int svposRotorRotorSpeed = svposRotorStop;      // speed of the rotor
#endif

void
DoArmJoystick(int jyc) {
    CheckJoy1ModeChange(jyc, joystick);
    DoCommonJoystick(jyc);

    const MOVEARMTIP mode = MOVEARMTIP_LINEAR;

    if (0) {
    }
    //----------------------------------------------------------
    // Scoring
    //----------------------------------------------------------

#if ROBOT_NAME==ROBOT_NAME_FTC417_2010_V12
    else if (joyBtnOnce(jyc, 1)) {
        ScoreBatonsInRollingGoal(ROLLING_GOAL_1);
    } else if (joyBtnOnce(jyc, 3)) {
        ScoreBatonsInRollingGoal(ROLLING_GOAL_3);
    } else if (joyBtnOnce(jyc, 4)) {
        ScoreBatonsInRollingGoal(ROLLING_GOAL_4);
    }
    //--------------------------------------------------------------------
    // Arm movement
    //--------------------------------------------------------------------
    const int dsvpos = 5;

    //--------------------------------------------------------------------
    // Up/down on the left joystick controls shoulder of dispenser arm
    if (joyFlick(jyc, JOY_LEFT, JOYDIR_UP)) {
        int svpos = GetServoValue(svoArmShoulder) + dsvpos;
        MoveServo(svoArmShoulder, svpos);
        WaitForServos();
    }

    if (joyFlick(jyc, JOY_LEFT, JOYDIR_DOWN)) {
        int svpos = GetServoValue(svoArmShoulder) - dsvpos;
        MoveServo(svoArmShoulder, svpos);
        WaitForServos();
    }
    //--------------------------------------------------------------------
    // Up/down on the right joystick controls elbow of dispenser arm
    if (joyFlick(jyc, JOY_RIGHT, JOYDIR_UP)) {
        int svpos = GetServoValue(svoArmElbow) + dsvpos;
        MoveServo(svoArmElbow, svpos);
        WaitForServos();
    }
    if (joyFlick(jyc, JOY_RIGHT, JOYDIR_DOWN)) {
        int svpos = GetServoValue(svoArmElbow) - dsvpos;
        MoveServo(svoArmElbow, svpos);
        WaitForServos();
    }
    //--------------------------------------------------------------------
    // L/R on the hat rotates the dispenser arm
    if (joyHat(jyc, JOYHAT_LEFT)) {
        int svpos = GetServoValue(svoArmRotation) + dsvpos;
        MoveServo(svoArmRotation, svpos);
        WaitForServos();
    }
    if (joyHat(jyc, JOYHAT_RIGHT)) {
        int svpos = GetServoValue(svoArmRotation) - dsvpos;
        MoveServo(svoArmRotation, svpos);
        WaitForServos();
    }
    //--------------------------------------------------------------------
    // L/R on the left joystick controls the wrist
    if (joyFlick(jyc, JOY_LEFT, JOYDIR_RIGHT)) {
        int svpos = GetServoValue(svoArmWrist) + dsvpos;
        MoveServo(svoArmWrist, svpos);
        WaitForServos();
    }
    if (joyFlick(jyc, JOY_LEFT, JOYDIR_LEFT)) {
        int svpos = GetServoValue(svoArmWrist) - dsvpos;
        MoveServo(svoArmWrist, svpos);
        WaitForServos();
    }
    //--------------------------------------------------------------------
    // Left upper button dispenses; left lower packs thereafter
    if (joyBtnOnce(jyc, JOYBTN_LEFTTRIGGER_UPPER)) {
        // start / stop the dispenser rotating
        UnloadDispenser(CAPTURE_BATONS);
    }
    if (joyBtnOnce(jyc, JOYBTN_LEFTTRIGGER_LOWER)) {
        PackArmSafelyFromAnywhere(false);
    }
#endif
}

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------
#if 0
#define DoTeleOpTelemetry()
#else
#define DoTeleOpTelemetry()                                     \
    {                                                           \
        if (TelemetryIsEnabled())                               \
        {                                                       \
            MILLI msNow = nSysTime;                             \
            ENCOD encLeft, encRight;                            \
            ReadEncoders(encLeft, encRight);                    \
            LockBlackboard();                                   \
            int powerLeft  = MtrOf(motorLeft).power;            \
            int powerRight = MtrOf(motorRight).power;           \
            ReleaseBlackboard();                                \
                                                                \
            TelemetryAddInt32(telemetry.serialNumber);          \
            TelemetryAddInt32(msNow - msTelemetryBias);         \
            TelemetryAddInt32(encLeft);                         \
            TelemetryAddInt32(encRight);                        \
            TelemetryAddInt16(powerLeft);                       \
            TelemetryAddInt16(powerRight);                      \
            TelemetryAddInt16(joystick.msg.serialNumber);       \
            TelemetryAddInt32(msNow - joystick.msg.msReceived); \
            TelemetrySend();                                    \
                                                                \
            telemetry.serialNumber++;                           \
        }                                                       \
    }
#endif

#define DoTeleOp()                                                      \
{                                                                       \
    DisplayMessage("<- tele op ->");                                    \
    fHaltProgramOnMotorStall = false;                                   \
    for (;;) {                                                          \
        if (getJoystickSettings(joystick)) {                            \
            /* Do Joystick Controller #1 */                             \
            switch (jyc1Mode) {                                         \
            case JYC1_MODE_PASSIVE:                                     \
                break;                                                  \
                                                                        \
            case JYC1_MODE_DRIVE:                                       \
                DoDriveJoystick(1);                                     \
                break;                                                  \
                                                                        \
            case JYC1_MODE_ARM:                                         \
                DoArmJoystick(1);                                       \
                break;                                                  \
            }                                                           \
                                                                        \
            /* Do Joystick Controller #2 */                             \
            switch (jyc2Mode) {                                         \
            case JYC2_MODE_PASSIVE:                                     \
                break;                                                  \
                                                                        \
            case JYC2_MODE_ARM:                                         \
                DoArmJoystick(2);                                       \
                break;                                                  \
            }                                                           \
        } else if (nSysTime - joystick.msg.msReceived >                 \
                   MS_JOYSTICK_FCS_DISCONNECTED_THRESHOLD) {            \
            /*                                                          \
             * We haven't received a new message from the FCS in WAY    \
             * too long so we have to consider ourselves disconnected.  \
             * We take steps to reign in a possibly runaway robot.      \
             */                                                         \
            StopRobot();                                                \
                                                                        \
            /*                                                          \
             * Sound an alarm if we've LOST communication rather than   \
             * never seen it in the first place.                        \
             */                                                         \
            if (joystick.msg.serialNumber != 0) {                       \
                /* SOMETHING audible for helpful debugging; could be better */ \
                Beep(NOTE_E);                                           \
            }                                                           \
        }                                                               \
        DoTeleOpTelemetry();                                            \
                                                                        \
        /* Be nice: let other tasks run */                              \
        EndTimeSlice();                                                 \
    }                                                                   \
}
