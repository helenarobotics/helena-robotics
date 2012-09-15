//
// MainPostlude.h
//

//---------------------------------------------------------------------------
// Provide defaults for anything that the robot-specific didn't.
//
// First we define any sensor name macros that the robot config did not.
// This allows us, elsewhere in the code, to use conditional compilation
// to adapt to a sensor's presence or absence.
//
// Subsequently, we define the motors, robot geometry defaults, etc.
//---------------------------------------------------------------------------

#define SENSOR_NOT_PRESENT   sensnmNone

#ifndef sensnmSonicLeft
#    define sensnmSonicLeft      SENSOR_NOT_PRESENT
#endif
#ifndef sensnmSonicBack
#    define sensnmSonicBack      SENSOR_NOT_PRESENT
#endif
#ifndef sensnmSonicRight
#    define sensnmSonicRight     SENSOR_NOT_PRESENT
#endif
#ifndef sensnmSonicFront
#    define sensnmSonicFront     SENSOR_NOT_PRESENT
#endif

#ifndef sensnmEopdFront
#    define sensnmEopdFront      SENSOR_NOT_PRESENT
#endif
#ifndef sensnmEopdFrontRight
#    define sensnmEopdFrontRight SENSOR_NOT_PRESENT
#endif
#ifndef sensnmEopdArm
#    define sensnmEopdArm        SENSOR_NOT_PRESENT
#endif

#ifndef sensnmGyroHorz
#    define sensnmGyroHorz       SENSOR_NOT_PRESENT
#endif
#ifndef sensnmGyroVert
#    define sensnmGyroVert       SENSOR_NOT_PRESENT
#endif

#ifndef sensnmMagRotor
#    define sensnmMagRotor       SENSOR_NOT_PRESENT
#endif
#ifndef sensnmMagRotorAux
#    define sensnmMagRotorAux    SENSOR_NOT_PRESENT
#endif

#ifndef sensnmColor
#    define sensnmColor          SENSOR_NOT_PRESENT
#endif
#ifndef sensnmAngleRotor
#    define sensnmAngleRotor     SENSOR_NOT_PRESENT
#endif
#ifndef sensnmCompass
#    define sensnmCompass        SENSOR_NOT_PRESENT
#endif
#ifndef sensnmTouchPreload
#    define sensnmTouchPreload   SENSOR_NOT_PRESENT
#endif

#ifndef motorLeft
#    define motorLeft  rgmotor[0]
#endif
#ifndef motorRight
#    define motorRight rgmotor[1]
#endif
#ifndef HAS_ENCODER_BASED_TURNING_TABLE
#    define HAS_ENCODER_BASED_TURNING_TABLE 0
#endif

#ifndef CM_BETWEEN_WHEELS
// default: somewhere in the ballpark
#    define CM_BETWEEN_WHEELS               ((27.5 + 19.5) * 0.5)
#endif
#ifndef CM_WHEEL_CIRCUMFERENCE
#    define CM_WHEEL_CIRCUMFERENCE          (3.0 * 2.54 * PI)   // default: 3 inch wheels
#endif
#ifndef MOTOR_GEARING
#    define MOTOR_GEARING                   1.0 // default: 1 to 1
#endif

#define ENCODER_TICKS_PER_MOTOR_REV     1440    // period, not overrideable

//---------------------------------------------------------------------------
// More definitions
//---------------------------------------------------------------------------

// Derived robot-related constants
const float cmPerMotorRev = CM_WHEEL_CIRCUMFERENCE * MOTOR_GEARING;
const float crevMotorPerCm = 1.0 / cmPerMotorRev;
const float cmPerEncoderTick =
    cmPerMotorRev / (float)ENCODER_TICKS_PER_MOTOR_REV;
const float encoderTickPerCm = 1.0 / cmPerEncoderTick;

#define RET_IF_FAIL(a, b)  { if (!(a)) { PlaySad(); /* DisplayMessage(b); */ return false; } }

#include "Sensor.h"
#include "Motor.h"
#include "Display.h"
#include "StopConditions.h"
#include "ServoArmTip.h"
#include "ServoRotor.h"
#include "ServoArm.h"
#include "Blackboard.h"
#include "Drive.h"
#include "Turn.h"
#include "DriveAids.h"
#include "GetOverIt.h"

//---------------------------------------------------------------------------
// Initialization
//---------------------------------------------------------------------------

BOOL
InitializeMain(BOOL fBackgroundTasks, BOOL fInitArmServos) {
    // Ensure we can hear the bot
    nVolume = kMaxVolumeLevel;

    // Give some feedback that initialization is at least starting
    Beep();

    // msStart is a good baseline value for time variables
    msStart = nSysTime;

    // Initialize the locks now so remaining code can liberally use
    InitializeLock(lockDaisy, "daisy");
    InitializeLock(lockBlackboard, "blackboard");

    // Start the display task so we can get error messages if we need to
    if (fBackgroundTasks) {
        StartDisplayTask();
    }
    // Initialize our telemetry infrastructure
    TelemetryInitialize();

    // We initialize the muxes and sensor before the motors and servos
    // since the latter have communication timeouts, and the initialization
    // of the former might take sufficiently long that said timeouts will be
    // triggered if done in the opposite order. We do motors after servos
    // because they have a shorter timeout.

    STICKYFAILURE fOverallSuccess = true;
    InitializeSensorMuxes(fOverallSuccess);
    InitializeSensors(fOverallSuccess);

    // Try to deal with the 'one-time after long power off sonic sensors
    // report incorrect types on muxes' bug
    if (!fOverallSuccess) {
        // No real reason, but it can only help.
        wait1Msec(500);

        fOverallSuccess = true;
        InitializeSensorMuxes(fOverallSuccess);
        InitializeSensors(fOverallSuccess);
    }
    InitializeServos(fOverallSuccess);
    InitializeMotors(fOverallSuccess);

    if (fOverallSuccess) {
        // Initialize turning and driving related state
        InitializeStopConditions(stop); // init the global variable
        InitializeTurns();

        if (fBackgroundTasks) {
            // Start the blackboard task and make sure it updates at
            // least once
            InitializeBlackboard();
            StartBlackboardTask();
            wait1Msec(2 * msBlackboardPolling);

            displayMode = DISPLAY_MODE_FULL;
        }

        MoveServosToInitialPositions();

        // Remember the time elapsed after initialization
        msElapsedStart = nSysTime;
    } else {
        PlaySad();
    }

    return fOverallSuccess;
}
