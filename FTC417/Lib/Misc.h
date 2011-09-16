//
// Misc.h
//

// typedefs to give is integers of known and easily named size
typedef int int16;
typedef long int32;

// an integer big enough to hold a value of the system clock
typedef int32 MILLI;

//--------------------------------------------------------------------------------------
// Macros for documentation etc
//--------------------------------------------------------------------------------------

// Document control of avoiding simultaneous function activations
#define CALLEDBY(iTask)         // Documents functions that can be called only by particular tasks
#define CALLED_WITH_BB_LOCK     // Documents functions that can be called by ANY task so long as the Blackboard is owned exclusively

// Document the flow of data for function parameters
#define IN
#define OUT

// Allow calibration programs to tune 'constants'
#ifndef CONST
#define CONST const
#endif

//--------------------------------------------------------------------------------------
// Funky stuff with pointers and references
//--------------------------------------------------------------------------------------

// Given a value of a type which structurally has BASECLASS as a prefix, cast the
// type of 'value' to be that of a BASECLASS&
#define BASE_CAST(BASECLASS, value)     ((BASECLASS&)(void&)(value))

//--------------------------------------------------------------------------------------
// Tracing
//--------------------------------------------------------------------------------------

#if 1
#define TRACE(x)  writeDebugStreamLine x
#else
#define TRACE(x)
#endif
#define TRACE_ALWAYS(x) writeDebugStreamLine x

MILLI msStart, msElapsedStart;

#define TraceElapsed()                                                    \
    {                                                                     \
    TRACE(("elapsed: %6.3fs", (float)(nSysTime - msElapsedStart) * 0.001)); \
    }

int cReportedInitFailures = 0;
#define TraceInitializationResult(sMsg, fValue)                             \
    {                                                                       \
    TRACE(("init: %s:%s", sMsg, fValue ? "ok" : "fail"));                   \
    if (!fValue && 0==cReportedInitFailures)                                \
        {                                                                   \
        cReportedInitFailures++;                                            \
        string sDisplay; StringFormat(sDisplay, "%s fail", sMsg);           \
        DisplayMessage(sDisplay);                                           \
        }                                                                   \
    }

#define TraceInitializationResult1(sFormat, value, fValue)                  \
    {                                                                       \
    string s; StringFormat(s, sFormat, value);                              \
    TraceInitializationResult(s, fValue);                                   \
    }

//--------------------------------------------------------------------------------------
// Some math. Implement as our own macros where needed so as to be task-insenstitive.
//--------------------------------------------------------------------------------------

#ifndef AssignStruct
#define AssignStruct(to, from)          memcpy(to, from, sizeof(to))
#endif
#ifndef Swap
#define Swap(a, b, t)                   { t = a; a = b; b = t; }
#endif
#ifndef ArraySize
#define ArraySize(globalArray)          (sizeof(globalArray) / sizeof(globalArray[0]))
#endif

#ifndef Max
#define Max(a,b)                        ((a) > (b) ? (a) : (b))
#endif
#ifndef Min
#define Min(a,b)                        ((a) < (b) ? (a) : (b))
#endif
#ifndef Sign
#define Sign(x)                         sgn(x)
#endif
#ifndef Abs
#define Abs(x)                          abs(x)
#endif
#ifndef Between
#define Between(low,x,high)             ((low) <= (x) && (x) <= (high))
#endif
#ifndef StrictlyBetween
#define StrictlyBetween(low,x,high)     ((low) <  (x) && (x) <  (high))
#endif

#ifndef MaxVar
#define MaxVar(var,val)                 { if (var < (val)) { var = (val); } }
#endif
#ifndef MinVar
#define MinVar(var,val)                 { if (var > (val)) { var = (val); } }
#endif

#define square(x)       ((x) * (x))
#define cube(x)         ((x) * (x) * (x))
#define timesTwo(x)     (2 * (x))
#define timesFour(x)    (4 * (x))

// ClampVar ensures a varaible is between the indicated bounds. ClampVarSign
// is similar, but negates the bounds if var is negative.
#define ClampVar(var,minVal,maxVal)     { if (var < (minVal)) { var = (minVal); } else if (var > (maxVal)) { var = (maxVal); } }
#define ClampVarSign(var,minVal,maxVal) { if (var < 0) ClampVar(var,-(maxVal),-(minVal)) else ClampVar(var,maxVal,minVal) }

#if 0
    // This method of rounding produces magnitudes which are insensitive to sign: 2.5 rounds to 3 just
    // as -2.5 rounds to -3. This is ideally what we want. However, we always end up evaluating
    // the argument x twice, and there's no way to avoid that in the macro (and using a function isn't
    // safely usable across tasks)
#define Rounded(x,type)           ((x) < 0 ? ((type)((float)(x) - 0.5)) : ((type)((float)(x) + 0.5)))
#else
    // This method of rounding avoids the multiple evaluation of the argument. The downside is that
    // the exact-half-negatives round up instead of down: -2.5 rounds to -2 (though -2.75 still rounds
    // to -3). We can live with the tradeoff. (Note that calling intrinsics is task-safe.)
#define Rounded(x,type)           ((type)(floor((float)(x) + 0.5)))
#endif

#define FloorDiv(x,d)                 floor((float)(x) / (float)(d))

typedef union {
    float f;
    struct {
        unsigned char b3;
        unsigned char b2;
        unsigned char b1;
        unsigned char b0;
    };
} FLOAT_LONG;

// Is this a finite IEEE floating point number?
// See http://en.wikipedia.org/wiki/Single_precision_floating-point_format
#define IsFinite(fSuccess, pair, floatVal)                                       \
    {                                                                           \
    pair.f = floatVal;                                                          \
    fSuccess = !((pair.b0 & 0x7F) == 0x7F && (pair.b1 & 0x80) == 0x80);          \
    }

//--------------------------------------------------------------------------------------
// Other
//--------------------------------------------------------------------------------------

#define InfiniteIdleLoop()      { while(true) { wait1Msec(1000); } }

typedef enum
// Enumerate the various tasks in our system
{
    iTaskMain,
    iTaskBlackboard,
    iTaskDisplay,
    iTaskMax,                   // not a real task
} ITASK;

// Constants controlling the speed of our various spin loops
#define msBlackboardPolling 50  // was 100
#define msDisplayPolling    750

// Constants reflecting the range of the data type 'int' and 'int16'
#define intFirst  (-32768)
#define intLast   32767

// Support for recording a target value and a desired comparison against same.
typedef enum {
    COMPARE_LT, COMPARE_GT, COMPARE_LE, COMPARE_GE
} COMPARISON;

BOOL CALLED_WITH_BB_LOCK
Compare(int cur, int target, COMPARISON comparison) {
    switch (comparison) {
    case COMPARE_LT:
        return cur < target;
    case COMPARE_GT:
        return cur > target;
    case COMPARE_LE:
        return cur <= target;
    case COMPARE_GE:
        return cur >= target;
    }
    return false;
}

//--------------------------------------------------------------------------------------
// Joystick modal waits - try to avoid using: use joyBtnOnce etc instead
//--------------------------------------------------------------------------------------

#define WaitJoyCondition(cond)            \
    {                                     \
    while (true)                          \
        {                                 \
        getJoystickSettings(joystick);    \
        if (cond) break;                  \
        wait1Msec(50);                    \
        }                                 \
    }

#define WaitJoyConditionGone(cond)        \
    {                                     \
    while (true)                          \
        {                                 \
        getJoystickSettings(joystick);    \
        if (!(cond)) break;               \
        wait1Msec(50);                    \
        }                                 \
    }

#define WaitForButton(button)         WaitJoyCondition(joy1Btn(button))
#define WaitForButtonRelease(button)  WaitJoyConditionGone(joy1Btn(button))

#define WaitForButtonJoy(joy,button)         WaitJoyCondition(joyBtn(joy,button))
#define WaitForButtonReleaseJoy(joy,button)  WaitJoyConditionGone(joyBtn(joy,button))

#ifndef DO_DRIVE_TURN_DEMO_WAIT
#define DO_DRIVE_TURN_DEMO_WAIT 0
#endif

#ifndef DriveTurnDemoWait
#ifndef __FTC_FIELD_H__
BOOL fProgramDemoMode = false;  // Make misc.h usable even w/o ftcfield.h
#endif
#define DriveTurnDemoWait()                                                 \
/* Called at the end of each drive and turn. Waits for the user to give */  \
/* the ok before proceeding onwards.                                    */  \
    {                                                                       \
    if (fProgramDemoMode)                                                   \
        {                                                                   \
        PlayHappy();                                                        \
        WaitForButton(JOYBTN_RIGHTTRIGGER_UPPER);                           \
        }                                                                   \
    }

#endif
