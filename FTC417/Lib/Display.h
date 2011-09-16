//
// Display.h
//
// We have an independent task that periodically writes things on our display

#ifndef DISPLAY_MESSAGE_DEFAULT
#define DISPLAY_MESSAGE_DEFAULT "<-all is well->"
#endif

typedef enum
    { DISPLAY_MODE_STOPPED, DISPLAY_MODE_SIMPLE,
        DISPLAY_MODE_FULL } DISPLAY_MODE;

DISPLAY_MODE displayMode = DISPLAY_MODE_STOPPED;
BOOL fDisplaySonic = false;
BOOL fDisplayEopdFront = false;
string szCurrentMessage = "";
MILLI msRestoreCurrentMessage = 0;

#define InitializeDisplayIfNeeded()                   \
    {                                                 \
    hogCpuNestable();                                 \
    if (DISPLAY_MODE_STOPPED == displayMode)          \
        {                                             \
        eraseDisplay();                               \
        displayMode = DISPLAY_MODE_SIMPLE;            \
        szCurrentMessage = DISPLAY_MESSAGE_DEFAULT;   \
        DisplayMessagePrim(szCurrentMessage);         \
        }                                             \
    releaseCpuNestable();                             \
    }

#define DisplayMessagePrim(szMessage)                 \
    {                                                 \
    nxtDisplayCenteredTextLine(7, "%s", szMessage);   \
    }

#undef DisplayMessage
void
CALLEDBY(iTaskMain)
DisplayMessage(string szMessage) {
    InitializeDisplayIfNeeded();
    //
    hogCpuNestable();
    szCurrentMessage = szMessage;
    DisplayMessagePrim(szMessage);
    releaseCpuNestable();
}

#undef DisplayMessageTemporarily
void
CALLEDBY(iTaskMain)
DisplayMessageTemporarily(int ms, string szMessage) {
    InitializeDisplayIfNeeded();
    msRestoreCurrentMessage = nSysTime + ms;    // math is in our task; the write is atomic
    DisplayMessagePrim(szMessage);
}

task
DisplayTask() {
    InitializeDisplayIfNeeded();
    //
    int cmFront = 255;
    int cmBack = 255;
    int cmLeft = 255;
    int cmRight = 255;
    int eopdFront = 0;
    int eopdFrontRight = 0;
    MILLI msLastBeep = nSysTime;
    //
    for (;;) {
        ENCOD encoderLeft = 0, encoderRight = 0;
        //
        float voltBatteryNxt = (float)nAvgBatteryLevel * 0.001;
        float voltBatteryExternal = vExternalBattery;
        //
        float compass = 0.0;
        //
        if (DISPLAY_MODE_FULL == displayMode) {
            LockBlackboard();
            //
            ReadEncoders(encoderLeft, encoderRight);
            //
            //
#if SensorIsDefined(sensnmCompass)
            compass = sensCompass.value;
#endif
            if (fDisplaySonic) {
                // Read the sonics
                //
#if SensorIsDefined(sensnmSonicFront)
                RawReadSonic(sensSonicFront);
                cmFront = sensSonicFront.cm;
#endif
#if SensorIsDefined(sensnmSonicBack)
                RawReadSonic(sensSonicBack);
                cmBack = sensSonicBack.cm;
#endif
#if SensorIsDefined(sensnmSonicLeft)
                RawReadSonic(sensSonicLeft);
                cmLeft = sensSonicLeft.cm;
#endif
#if SensorIsDefined(sensnmSonicRight)
                RawReadSonic(sensSonicRight);
                cmRight = sensSonicRight.cm;
#endif
            }
#if SensorIsDefined(sensnmEopdFront)
            if (fDisplayEopdFront) {
                RawReadEopd(sensEopdFront);
                eopdFront = sensEopdFront.value;
            }
#endif
#if SensorIsDefined(sensnmEopdFrontRight)
            if (fDisplayEopdFront) {
                RawReadEopd(sensEopdFrontRight);
                eopdFrontRight = sensEopdFrontRight.value;
            }
#endif
            //
            ReleaseBlackboard();
        }
        //
        // Sound an alarm if our battery level is getting depleted
        //
        MILLI msNow = nSysTime;
        const float voltBatteryExternalAlarmThreshold = 12.2;
        const float voltBatteryNxtAlarmThreshold = 6.3;
        if ((0 == voltBatteryExternal
                || voltBatteryExternalAlarmThreshold <= voltBatteryExternal)
            && (voltBatteryNxtAlarmThreshold <= voltBatteryNxt)) {
            msLastBeep = msNow;
        } else {
            if (msNow - msLastBeep >= 3000) {
                Beep(NOTE_G);
                msLastBeep = msNow;
            }
        }
        //
        nxtDisplayTextLine(0, "ext=%1.1f nxt=%1.1f   ", voltBatteryExternal,
            voltBatteryNxt);
        nxtDisplayTextLine(1, "L=%4d R=%4d", encoderLeft, encoderRight);
        //
        if (fDisplayEopdFront) {
            nxtDisplayTextLine(2, "eFL=%d eFR=%d", eopdFront, eopdFrontRight);
        }
        if (fDisplaySonic) {
            nxtDisplayTextLine(3, "F=%2d R=%d", cmFront, cmRight);
            nxtDisplayTextLine(4, "B=%2d L=%d", cmBack, cmLeft);
        }
        //
        if (msRestoreCurrentMessage != 0 && nSysTime > msRestoreCurrentMessage) {
            msRestoreCurrentMessage = 0;
            DisplayMessagePrim(szCurrentMessage);
        }
        //
        wait1Msec(msDisplayPolling);
    }
}

#define StartDisplayTask()      \
    {                           \
    StartTask(DisplayTask);     \
    }

#define SuspendDisplayTask()    \
    {                           \
    LockBlackboard();           \
    StopTask(DisplayTask);    \
    ReleaseBlackboard();        \
    }

#define ResumeDisplayTask()     \
    {                           \
    StartTask(DisplayTask);    \
    }
