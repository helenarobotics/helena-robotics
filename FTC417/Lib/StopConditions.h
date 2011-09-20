//
// StopConditions.h
//

typedef struct {
    // sensor conditions that should cause some action to stop
    BOOL fStopOnTouchPreload;
    BOOL fStopOnEopdArm;
    BOOL fStopOnEopdFront;
    BOOL fStopOnEopdFrontRight;
    BOOL fStopOnAngleRotor;
    BOOL fStopOnColor;
    BOOL fStopOnGyroHorz;
    BOOL fStopOnGyroVert;

    BOOL fRequireStop;          // if true, then turning and driving
    // fail unless the stop condition is hit

    float cmAdditional;         // for driving: an additional distance
    // to travel after stop

    BOOL fStopConditionReached;
    float fractionRemaining;
} STOPCONDITIONS;

// a global STOPCONDITIONS that can be used by the main task
STOPCONDITIONS stop;

void
InitializeStopConditions(STOPCONDITIONS &stop) {
    stop.fStopOnTouchPreload = false;
    stop.fStopOnEopdArm = false;
    stop.fStopOnEopdFront = false;
    stop.fStopOnEopdFrontRight = false;
    stop.fStopOnAngleRotor = false;
    stop.fStopOnColor = false;
    stop.fStopOnGyroHorz = false;
    stop.fStopOnGyroVert = false;

    stop.fRequireStop = false;
    stop.cmAdditional = 0.0;
    stop.fStopConditionReached = false;
    stop.fractionRemaining = 0.0;
}

void
ArmStopConditions(STOPCONDITIONS &stop) {
#if SensorIsDefined(sensnmTouchPreload)
    if (stop.fStopOnTouchPreload)
        StartDetectingTouchSensor(sensTouchPreload);
#endif
#if SensorIsDefined(sensnmEopdFront)
    if (stop.fStopOnEopdFront)
        StartDetectingEopdSensor(sensEopdFront);
#endif
#if SensorIsDefined(sensnmEopdFrontRight)
    if (stop.fStopOnEopdFrontRight)
        StartDetectingEopdSensor(sensEopdFrontRight);
#endif
#if SensorIsDefined(sensnmEopdArm)
    if (stop.fStopOnEopdArm)
        StartDetectingEopdSensor(sensEopdArm);
#endif
#if SensorIsDefined(sensnmAngleRotor)
    if (stop.fStopOnAngleRotor)
        StartDetectingAngleSensor(sensAngleRotor);
#endif
#if SensorIsDefined(sensnmColor)
    if (stop.fStopOnColor)
        StartDetectingColorSensor(sensColor);
#endif
#if SensorIsDefined(sensnmGyroHorz)
    if (stop.fStopOnGyroHorz)
        StartDetectingGyroSensor(sensGyroHorz);
#endif
#if SensorIsDefined(sensnmGyroVert)
    if (stop.fStopOnGyroVert)
        StartDetectingGyroSensor(sensGyroVert);
#endif
}

void
DisarmStopConditions(STOPCONDITIONS &stop) {
#if SensorIsDefined(sensnmTouchPreload)
    if (stop.fStopOnTouchPreload)
        StopDetectingTouchSensor(sensTouchPreload);
#endif
#if SensorIsDefined(sensnmEopdFront)
    if (stop.fStopOnEopdFront)
        StopDetectingEopdSensor(sensEopdFront);
#endif
#if SensorIsDefined(sensnmEopdFrontRight)
    if (stop.fStopOnEopdFrontRight)
        StopDetectingEopdSensor(sensEopdFrontRight);
#endif
#if SensorIsDefined(sensnmEopdArm)
    if (stop.fStopOnEopdArm)
        StopDetectingEopdSensor(sensEopdArm);
#endif
#if SensorIsDefined(sensnmAngleRotor)
    if (stop.fStopOnAngleRotor)
        StopDetectingAngleSensor(sensAngleRotor);
#endif
#if SensorIsDefined(sensnmColor)
    if (stop.fStopOnColor)
        StopDetectingColorSensor(sensColor);
#endif
#if SensorIsDefined(sensnmGyroHorz)
    if (stop.fStopOnGyroHorz)
        StopDetectingGyroSensor(sensGyroHorz);
#endif
#if SensorIsDefined(sensnmGyroVert)
    if (stop.fStopOnGyroVert)
        StopDetectingGyroSensor(sensGyroVert);
#endif
}

BOOL
CheckStopConditions(STOPCONDITIONS &stop) {
    BOOL fStop = false;
    LockBlackboard();
    //
#if SensorIsDefined(sensnmTouchPreload)
    if (!fStop && stop.fStopOnTouchPreload)
        fStop = sensTouchPreload.fDetected;
#endif
    //
#if SensorIsDefined(sensnmEopdFront)
    if (!fStop && stop.fStopOnEopdFront)
        fStop = sensEopdFront.fDetected;
#endif

#if SensorIsDefined(sensnmEopdFrontRight)
    if (!fStop && stop.fStopOnEopdFrontRight)
        fStop = sensEopdFrontRight.fDetected;
#endif

#if SensorIsDefined(sensnmEopdArm)
    if (!fStop && stop.fStopOnEopdArm)
        fStop = sensEopdArm.fDetected;
#endif

#if SensorIsDefined(sensnmAngleRotor)
    if (!fStop && stop.fStopOnAngleRotor)
        fStop = sensAngleRotor.fDetected;
#endif

#if SensorIsDefined(sensnmColor)
    if (!fStop && stop.fStopOnColor)
        fStop = sensColor.fDetected;
#endif

#if SensorIsDefined(sensnmGyroHorz)
    if (!fStop && stop.fStopOnGyroHorz)
        fStop = sensGyroHorz.fDetected;
#endif

#if SensorIsDefined(sensnmGyroVert)
    if (!fStop && stop.fStopOnGyroVert)
        fStop = sensGyroVert.fDetected;
#endif
    ReleaseBlackboard();
    return fStop;
}
