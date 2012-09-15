//
// MainPrelude.h
//

//---------------------------------------------------------------------------
// Do better booleans: there appear to be issues when trying to pass a
// one byte 'bool' by reference. Using an 'int' fixes the problem.
//---------------------------------------------------------------------------
#ifndef BOOL
#    define BOOL int
#endif

// A STICKYFAILURE is a boolean that is initially true but once set to
// false is to remain so. This is useful, for example, for carring out a
// long initialization sequence (that might call many functions) and
// testing the overall success at the end.
#define STICKYFAILURE BOOL

//---------------------------------------------------------------------------
// Include any system headers we need
//---------------------------------------------------------------------------

#ifndef USE_FTCFIELD
#    define USE_FTCFIELD 1
#endif

#ifndef USE_JOYSTICK_DRIVER
#    define USE_JOYSTICK_DRIVER 1
#endif

#if USE_JOYSTICK_DRIVER
#    ifndef USE_DISPLAY_DIAGNOSTICS
#        define USE_DISPLAY_DIAGNOSTICS 0
#    endif
#endif

#include "Music.h"

#if USE_FTCFIELD
#    include "FTCField.h"
#endif

#if USE_JOYSTICK_DRIVER
#    include "JoystickDriver.h"
#endif

#include "Misc.h"
#include "TelemetryFTC.h"
#include "Geometry.h"
#include "Lock.h"
#include "I2C.h"
#include "SensorMux.h"
#include "SensorDecl.h"
#include "Daisy.h"
#include "MotorDecl.h"
#include "Servo.h"

//---------------------------------------------------------------------------
// Headers for the functions to be implemented by each robot configuration
//---------------------------------------------------------------------------

#ifndef InitializeMotors
void InitializeMotors(IN OUT STICKYFAILURE &fOverallSuccess);
#endif
#ifndef InitializeSensors
void InitializeSensors(IN OUT STICKYFAILURE &fOverallSuccess);
#endif
#ifndef InitializeServos
void InitializeServos(IN OUT STICKYFAILURE &fOverallSuccess);
#endif
