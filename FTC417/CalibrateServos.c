//
// CalibrateServos.h
//

// We need the joystick here in Calibrate servos
#define USE_JOYSTICK_DRIVER 1

#define ROBOT_NAME ROBOT_NAME_FTC417_2010_V12       // until we generalize this file
#include "lib\MainPrelude.h"
#include "config\RobotConfig.h"
#include "lib\MainPostlude.h"

int         jyc                  = 1;               // which joystick controller
int         dsvpos               = 5;               // increment by which we move servos
DISPENSER   disp                 = DISP_HIGH;       // which dispenser do we calibrate to
int         svposRotorRotorSpeed = svposRotorStop;  // speed of the rotor

int AdjustSvpos(int svpos, int dsvpos)
    {
    // Holding the lower right button makes changes the movement calls
    // to move the servo to the extreme position in that direction.
    if (joyBtn(jyc, JOYBTN_RIGHTTRIGGER_LOWER))
        {
        if (dsvpos < 0)
            svpos = 0;
        else
            svpos = svposLast;
        }
    else
        svpos += dsvpos;
    //
    return svpos;
    }

// Are we controlling the joysticks in the direct-drive-servo or
// in the move-arm-tip mode or in the driving mode?
typedef enum { JYC1_MODE_SVPOS, JYC1_MODE_ARMTIP, JYC1_MODE_DRIVING } JYC1_MODE;

JYC1_MODE jyc1Mode = JYC1_MODE_SVPOS;

void CheckJoy1ModeChange(int jyc)
    {
    if (joyBtnOnce(jyc, JOYBTN_TOP_LEFT))
        {
        Beep(NOTE_C,3);
        jyc1Mode = JYC1_MODE_SVPOS;
        }
    else if (joyBtnOnce(jyc, JOYBTN_TOP_RIGHT))
        {
        Beep(NOTE_G,3);
        if (JYC1_MODE_SVPOS == jyc1Mode)
            {
            TRACE(("mode arm tip"));
            jyc1Mode = JYC1_MODE_ARMTIP;
            }
        else if (JYC1_MODE_ARMTIP == jyc1Mode)
            {
            TRACE(("mode driving"));
            jyc1Mode = JYC1_MODE_DRIVING;
            }
        else if (JYC1_MODE_DRIVING == jyc1Mode)
            {
            TRACE(("mode svpos"));
            jyc1Mode = JYC1_MODE_SVPOS;
            }
        }
    }

//------------------------------------------------------------------------------
// Common control
//------------------------------------------------------------------------------

void DoCommonControl(int jyc, int cm)
    {
    //--------------------------------------------------------------------
    // Adjust the dispenser arm increment with the right upper trigger
    if (joyBtnOnce(jyc,JOYBTN_RIGHTTRIGGER_UPPER))
        {
        if (1 == dsvpos)
            {
            dsvpos = 5;
            Beep(NOTE_E);
            }
        else
            {
            dsvpos = 1;
            Beep(NOTE_A);
            }
        }


#if HAS_ARM_SERVOS
    //--------------------------------------------------------------------
    //
    // L/R on the hat rotates the dispenser arm
    //
    if (joyHat(jyc, JOYHAT_LEFT))
        {
        int svpos = AdjustSvpos(GetServoValue(svoArmRotation), dsvpos);
        MoveServo(svoArmRotation, svpos);
        }
    if (joyHat(jyc, JOYHAT_RIGHT))
        {
        int svpos = AdjustSvpos(GetServoValue(svoArmRotation), -dsvpos);
        MoveServo(svoArmRotation, svpos);
        }
#endif

#if HAS_PRELOAD_SERVOS
    //--------------------------------------------------------------------
    //
    // L/R on the left joystick controls the preload arm
    //
    if (joyFlick(jyc,JOY_LEFT,JOYDIR_RIGHT))
        {
        int svpos = AdjustSvpos(GetServoValue(svoPreloadArm), dsvpos);
        PositionPreloadArm(svpos);
        }
    if (joyFlick(jyc,JOY_LEFT,JOYDIR_LEFT))
        {
        int svpos = AdjustSvpos(GetServoValue(svoPreloadArm), -dsvpos);
        PositionPreloadArm(svpos);
        }
#elif HAS_WRIST_SERVOS
    //--------------------------------------------------------------------
    //
    // L/R on the left joystick controls the wrist
    //
    if (joyFlick(jyc,JOY_LEFT,JOYDIR_RIGHT))
        {
        int svpos = AdjustSvpos(GetServoValue(svoArmWrist), dsvpos);
        MoveServo(svoArmWrist, svpos);
        }
    if (joyFlick(jyc,JOY_LEFT,JOYDIR_LEFT))
        {
        int svpos = AdjustSvpos(GetServoValue(svoArmWrist), -dsvpos);
        MoveServo(svoArmWrist, svpos);
        }
#endif

    //--------------------------------------------------------------------
    // Misc
/*
    if (joyHatOnce(jyc,JOYHAT_UP))
        {
        PositionUsingEOPD(disp, cm);
        }
    if (joyHatOnce(jyc,JOYHAT_DOWN))
        {
        DISPENSINGPATH path;
        MoveFromEOPDToDispensing(OUT path, disp, cm, false);
        }
*/
    //--------------------------------------------------------------------
    // Canned dispenser things

    if (joyBtnOnce(jyc,JOYBTN_LEFTTRIGGER_LOWER))  // toggle the dispenser at which we do our calculations
        {
        if (DISP_HIGH == disp)
            {
            disp = DISP_LOW;
            Beep(NOTE_A);
            }
        else if (DISP_LOW == disp)
            {
            disp = DISP_HIGH;
            Beep(NOTE_E);
            }
        else
            PlaySad();
        }
    }

//------------------------------------------------------------------------------
// SVPOS-based control
//------------------------------------------------------------------------------

void DoPackingControl(int jyc, int cm)
    {
    //--------------------------------------------------------------------
    // Numbered buttons do canned things, but they also have
    // sanity checks so that they are not invoked from the wrong positions.

    BOOL fButton1 = joyBtnOnce(jyc,1);
    BOOL fButton2 = joyBtnOnce(jyc,2);
    BOOL fButton3 = joyBtnOnce(jyc,3);

    if (fButton1)    // packed <-> intermediate
        {
        if (FRotationPacked())
            {
            MoveDispenserFromPackedToInt();
            }
        else if (FRotationIntermediate())
            {
            MoveDispenserFromIntToPacked();
            }
        else
            PlaySad();
        }

    if (fButton2)    // packed <--> EOPD
        {
        if (FRotationPacked())
            {
            // UnpackDispenserArmForReadingEopd(disp, cm);
            }
        else if (FRotationEopd())
            {
            // PackDispenserArmFromEOPD(disp, cm);
            }
        else
            PlaySad();
        }

    if (fButton3)   // just pack, dang it, from wherever we are
        {
        PackArmSafelyFromAnywhere(false);
        }
    }

void DoArmTipMovement(int jyc, int cm)
    {
    if (0)
        {
        }
#if HAS_ARM_SERVOS
    //--------------------------------------------------------------------
    // Up/down on the left joystick controls shoulder of dispenser arm

    else if (joyFlick(jyc,JOY_LEFT,JOYDIR_UP))
        {
        int svpos = AdjustSvpos(GetServoValue(svoArmShoulder), dsvpos);
        MoveServo(svoArmShoulder, svpos);
        }

    else if (joyFlick(jyc,JOY_LEFT,JOYDIR_DOWN))
        {
        int svpos = AdjustSvpos(GetServoValue(svoArmShoulder), -dsvpos);
        MoveServo(svoArmShoulder, svpos);
        }

    //--------------------------------------------------------------------
    //
    // Up/down on the right joystick controls elbow of dispenser arm
    //
    else if (joyFlick(jyc,JOY_RIGHT,JOYDIR_UP))
        {
        int svpos = AdjustSvpos(GetServoValue(svoArmElbow), dsvpos);
        MoveServo(svoArmElbow, svpos);
        }
    else if (joyFlick(jyc,JOY_RIGHT,JOYDIR_DOWN))
        {
        int svpos = AdjustSvpos(GetServoValue(svoArmElbow), -dsvpos);
        MoveServo(svoArmElbow, svpos);
        }
#endif
    }

void DoSvposControl(int jyc, int cm)
    {
    DoPackingControl(jyc, cm);
    DoArmTipMovement(jyc, cm);

    if (0)
        {
        }

#if HAS_ARM_SERVOS

    //--------------------------------------------------------------------
    //
    // Dispensing
    //
    else if (joyBtnOnce(jyc,4))
        {
        // DispenseFromEOPDPosition(disp, cm);
        }

    else if (joyBtnOnce(jyc,JOYBTN_LEFTTRIGGER_UPPER))  // start / stop the dispenser rotating
        {
        int cBeep = 0;
        switch (svposRotorRotorSpeed)
            {
        case svposRotorStop: svposRotorRotorSpeed = svposRotorSlow; cBeep = 1; break;
        case svposRotorSlow: svposRotorRotorSpeed = svposRotorFast; cBeep = 2; break;
        case svposRotorFast: svposRotorRotorSpeed = svposRotorStop; cBeep = 3; break;
            }
        //
        SetRotorSpeed(svposRotorRotorSpeed);
        //
        for (int iBeep = 0; iBeep < cBeep; iBeep++)
            {
            Beep();
            }
        }

#endif
    }

//------------------------------------------------------------------------------
// Horizontal / vertical control
//------------------------------------------------------------------------------

void DoHVControl(int jyc, int cm)
    {
    float cmMove = dsvpos * 1; // 0.5;            // ie: 1/2 cm or 5 cm
    MOVEARMTIP mode = MOVEARMTIP_LINEAR;

    //--------------------------------------------------------------------
    // Arm movement

    if (joyFlick(jyc, JOY_RIGHT, JOYDIR_UP))
        {
        // Arm tip up
        if (!MoveArmTipBy(0, cmMove, mode))
            {
            PlaySad();
            }
        }
    if (joyFlick(jyc, JOY_RIGHT, JOYDIR_DOWN))
        {
        // Arm tip down
        if (!MoveArmTipBy(0, -cmMove, mode))
            {
            PlaySad();
            }
        }
    if (joyFlick(jyc, JOY_RIGHT, JOYDIR_LEFT))
        {
        // Arm tip out
        if (!MoveArmTipBy(cmMove, 0, mode))
            {
            PlaySad();
            }
        }
    if (joyFlick(jyc, JOY_RIGHT, JOYDIR_RIGHT))
        {
        // Arm tip in
        if (!MoveArmTipBy(-cmMove, 0, mode))
            {
            PlaySad();
            }
        }

    //--------------------------------------------------------------------
    // Scoring

#if ROBOT_NAME==ROBOT_NAME_FTC417_2010_V12

    if (joyBtnOnce(jyc,1))
        {
        ScoreBatonsInRollingGoal(ROLLING_GOAL_1);
        }
    if (joyBtnOnce(jyc,3))
        {
        ScoreBatonsInRollingGoal(ROLLING_GOAL_3);
        }
    if (joyBtnOnce(jyc,4))
        {
        ScoreBatonsInRollingGoal(ROLLING_GOAL_4);
        }

#endif
    //--------------------------------------------------------------------
    // Dispensing

    if (joyBtnOnce(jyc,JOYBTN_LEFTTRIGGER_UPPER))  // execute the full unload-dispenser logic
        {
        TRACE(("unloading dispenser"));
        // UnloadDispenser();
        }

    if (true)
        {
        // Use the left joystick (up/down) as a throttle for the rotor.
        // Note: rotational servos seem to be highly non-linear in speed.
        //
        int ctlPower = joyLeftY(jyc);
        int sgnPower = Sign(ctlPower);
        ctlPower     = Max(0, abs(ctlPower) - joyThrottleDeadZone);
        ctlPower     = ctlPower * sgnPower;
        float scale  = 128.0 / (128.0 - (float)joyThrottleDeadZone);
        ctlPower     = Rounded((float)ctlPower * scale, int);
        //
        // For rotation servos, 128 is the zero speed
        //
        ctlPower     = ctlPower + 128;
        SetRotorSpeed(ctlPower);
        }
    }

//------------------------------------------------------------------------------
// Logic for the main drive joystick
//------------------------------------------------------------------------------

task main()
    {
    if (!InitializeMain(true,true)) return; // will pack the servos
    waitForStart();
    PlayHappy();

    fHaltProgramOnMotorStall = false;
    fDisplayEopdFront = true;

#if SensorIsDefined(sensnmMagRotor)
    StartReadingMagneticSensor(sensMagRotor);
#endif
#if SensorIsDefined(sensnmAngleRotor)
    StartReadingAngleSensor(sensAngleRotor,false);
#endif
#if SensorIsDefined(sensnmGyroHorz)
    StartReadingGyroSensor(sensGyroHorz);
#endif

    while(true)
        {
        // Wait for any servo motion to cease
        WaitForServos();

        int cmSonic = cmSonicNil;
#if SensorIsDefined(sensnmSonicLeft)
        cmSonic = ReadSonic_Main(sensSonicLeft,false);
#endif
        // If we're far far away, pretend we're at the maximum
        // dispensing position; that way, the positioning logic
        // will at least DO something.
        //
        int cmLeft = cmSonic;
        if (cmSonicNil == cmLeft || cmLeft >= CmDispenseMax(disp))
            cmLeft = CmDispenseMax(disp)-1;
        //
        int svposElbow = 0, svposShoulder = 0, svposRotation = 0, svposWrist = 0, svposPre = 0;
#if HAS_ARM_SERVOS
        svposElbow    = GetServoValue(svoArmElbow);
        svposShoulder = GetServoValue(svoArmShoulder);
        svposRotation = GetServoValue(svoArmRotation);
#endif
#if HAS_WRIST_SERVOS
        svposWrist = GetServoValue(svoArmWrist);
#endif
#if HAS_PRELOAD_SERVOS
        svposPre   = GetServoValue(svoPreloadArm);
#endif
        if (!fDisplayEopdFront)
            {
            nxtDisplayTextLine(2, "cm=%d", cmSonic);
            }
        //
        if (getJoystickSettings(joystick))
            {
            CheckJoy1ModeChange(jyc);
            //
            if (0)
                {
                }
            else if (JYC1_MODE_SVPOS == jyc1Mode)
                {
                DoCommonControl(jyc,cmLeft);

                nxtDisplayTextLine(3, "rot=%3d elb=%3d", svposRotation, svposElbow);
                nxtDisplayTextLine(4, "sdr=%3d wst=%3d", svposShoulder, svposWrist);
                //
                DoSvposControl(jyc, cmLeft);
                }
            else if (JYC1_MODE_ARMTIP == jyc1Mode)
                {
                DoCommonControl(jyc,cmLeft);

                ARMTIPSTATE state;
                state.svposShoulder = svposShoulder;
                state.svposElbow    = svposElbow;
                ArmTipSvposToAngle(state);
                ComputeArmTipLocation(state);

                float degShoulder = radiansToDegrees(state.radShoulder);
                float degElbow    = radiansToDegrees(state.radElbow);

                nxtDisplayTextLine(3, "s=%.1f e=%.1f", degShoulder, degElbow);
                nxtDisplayTextLine(4, "x=%.2f y=%.2f", state.ptTip.x, state.ptTip.y);
                //
                DoHVControl(jyc, cmLeft);
                }
            else if (JYC1_MODE_DRIVING == jyc1Mode)
                {
                DoManualDrivingControl(jyc, joystick);
                }

            LockBlackboard();
            int mag = 0, ang = 0;
            float gyro = 0;
            #if SensorIsDefined(sensnmMagRotor)
            mag = Rounded(sensMagRotor.value, int);
            #endif
            #if SensorIsDefined(sensnmAngleRotor)
            ang = Rounded(sensAngleRotor.deg, int);
            #endif
            #if SensorIsDefined(sensnmGyroHorz)
            gyro = sensGyroHorz.deg;
            #endif
            ReleaseBlackboard();
            nxtDisplayTextLine(5, "ang=%d mag=%d", ang, mag);
            nxtDisplayTextLine(6, "gyro=%.1f", gyro);
            }
        }

#if SensorIsDefined(sensnmMagRotor)
    StopReadingMagneticSensor(sensMagRotor);
#endif
#if SensorIsDefined(sensnmAngleRotor)
    StopReadingAngleSensor(sensAngleRotor);
#endif
    }
