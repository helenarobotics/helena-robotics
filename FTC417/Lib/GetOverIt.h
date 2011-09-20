//
// GetOverIt.h
//
//----------------------------------------------------------------------------------
// Scoring
//----------------------------------------------------------------------------------

#if ROBOT_NAME==ROBOT_NAME_FTC417_2010_V12

#define svposShoulderScoringClear 255
#define svposElbowScoringClear    200

/* Move the servos into the indicated position for the purpose of scoring them in a rolling goal */
#define MoveSerovsForScoring(array, goal)       \
    { MoveSerovsForScoring_(array[goal][0], array[goal][1], array[goal][2], array[goal][3]); }

void
MoveSerovsForScoring_(int svposRot, int svposShoulder, int svposElbow,
    int svposWrist) {
    /* Sanity check */
    if (svposRot < 0 || svposShoulder < 0 || svposElbow < 0 || svposWrist < 0)
        return;

    /* Are the servos already there? If so, we've nothing to do         */
    /* This avoids some redundant useless servo movement, particularly  */
    /* in the magnetic case.                                            */
    int svposRotCur = GetServoValue(svoArmRotation);
    int svposShoulderCur = GetServoValue(svoArmShoulder);
    int svposElbowCur = GetServoValue(svoArmElbow);
    int svposWristCur = GetServoValue(svoArmWrist);
    if (svposRot == svposRotCur && svposShoulder == svposShoulderCur
        && svposElbow == svposElbowCur && svposWrist == svposWristCur)
        return;

    /* Move the shoulder and elbow to an upright enough position to clear the hardstop */
    /* and any magnetic baton that might be already scored                             */
    MoveServo(svoArmShoulder, svposShoulderScoringClear);
    MoveServo(svoArmElbow, svposElbowScoringClear);
    WaitForServos();

    /* Move the rotation and shoulder into position. The key point here is to get the   */
    /* rotation right, and we can get away with moving the shoulder at the same time so */
    /* we go ahead and do so */
    MoveServo(svoArmRotation, svposRot);
    MoveServo(svoArmShoulder, svposShoulder);
    WaitForServos();

    /* Get the wrist and elbow to the final correct angle */
    MoveServo(svoArmElbow, svposElbow);
    MoveServo(svoArmWrist, svposWrist);
    WaitForServos();
}

/* Move the arm back to the packed position after scoring. Be careful about clearance. */
void
PackServosAfterScoring() {
    // Raise things up to get clearance
    MoveServo(svoArmShoulder, svposShoulderScoringClear);
    MoveServo(svoArmElbow, svposElbowScoringClear);
    WaitForServos();

    // Get the wrist horizontal so that it too clears things
    MoveServo(svoArmWrist, svposArmWristHorizontal);
    WaitForServos();

    // Rotate back to home
    MoveServo(svoArmRotation, svposPackedRotation);
    WaitForServos();

    // Bring things on to packed
    PackArmSimply();
    WaitForServos();
}

/* How many degrees remain (if any) until we have to attend to the next */
/* magnetic baton that was detected by this sensor?                     */
BOOL
FDegUntilNextMagneticBatonSensor(MAGNETICSENSOR &sensor, ANGLE degRotor,
    OUT ANGLE &degResult) {
    BOOL fResult = false;
    degResult = 720.0;          // anything bigger than a full turn will do

    for (int idegDetectionRecord = 0;
        idegDetectionRecord < cdegMagDetectionRecordMax;
        idegDetectionRecord++) {
        if (sensor.rgfDetectionRecordValid[idegDetectionRecord]) {
            // How far has the rotor rotated since detection?
            ANGLE degRotated =
                degRotor - sensor.rgdegDetectionRecord[idegDetectionRecord];

            // How many degrees remain until we need to attend to it?
            ANGLE degRemaining =
                DEG_ROTOR_TURN_MAG_DISPENSE_BATON - degRotated;

            // If we've missed this by more than a half a baton, figure we
            // can't attend to this guy, and forget him
            if (degRemaining < -DEG_ROTOR_TURN_SINGLE_BATON_SCORE / 2) {
                // That baton is no longer with us. Remove it from our record of detection
                sensor.rgfDetectionRecordValid[idegDetectionRecord] = false;
            } else {
                // This is a mag baton we can still deal with. Remember where he is
                MinVar(degResult, degRemaining);
                fResult = true;
            }
        }
    }
    return fResult;
}

// How much can we still turn until we have to deal with the next remaining baton?
BOOL
FDegUntilNextMagneticBaton(ANGLE degRotor, OUT ANGLE &degResult) {
    ANGLE degMagRemaining, degMagRemainingAux;
    BOOL fMagRemaining =
        FDegUntilNextMagneticBatonSensor(sensMagRotor, degRotor,
        degMagRemaining);
    BOOL fMagRemainingAux =
        FDegUntilNextMagneticBatonSensor(sensMagRotorAux, degRotor,
        degMagRemainingAux);

    degResult = Min(degMagRemaining, degMagRemainingAux);
    return fMagRemaining || fMagRemainingAux;
}

/* Are we carrying a magnetic baton? */
BOOL
FCarryingMagneticBaton() {
    /* Where is the angle rotor sensor now? We may not have been monitoring this sensor */
    /* in the blackboard, so we force a read now just to be sure.                       */
    LockBlackboard();
    ReadAngleSensor(sensAngleRotor);
    ANGLE degRotor = sensAngleRotor.deg;
    ReleaseBlackboard();

    ANGLE degRemaining;
    return FDegUntilNextMagneticBaton(degRotor, degRemaining);
}

typedef enum {
    ROLLING_GOAL_1,
    ROLLING_GOAL_2,
    ROLLING_GOAL_3,
    ROLLING_GOAL_4,
    ROLLING_GOAL_MAG,
} ROLLING_GOAL;

// Given that a rolling goal is sitting comfortably in the V at the front of the robot,
// score batons that we are holding into the indicated goal (where the goals are numbered
// from the current perspective as shown below) except that we are to score magnetic batons
// in the small <mag> goal in the center.
//
//            <4>
//       <1> <mag> <3>
//            <2>
//
int rgsvposScoreBackup[][4] = {
    {144, 255, 160, 100},       // ROLLING_GOAL_1
    {-1, -1, -1, -1},           // ROLLING_GOAL_2
    {100, 220, 160, 85},        // ROLLING_GOAL_3 // 105, 240, 172, 102
    {124, 215, 183, 102},       // ROLLING_GOAL_4
    {124, 244, 195, 85}         // ROLLING_GOAL_MAG
};

int rgsvposScoreNoBackup[][4] = {
    {-1, -1, -1, -1},           // ROLLING_GOAL_1
    {-1, -1, -1, -1},           // ROLLING_GOAL_2
    {90, 250, 150, 85},         // ROLLING_GOAL_3
    {124, 255, 170, 90},        // ROLLING_GOAL_4
    {-1, -1, -1, -1}            // ROLLING_GOAL_MAG
};

void
ScoreBatonsInRollingGoal(ROLLING_GOAL goal) {
    BOOL fCanScore = true;
    //
    // Set some defaults which *might* be modified by the goal-specific
    // logic below
    //
    int svposRotorSpeed = svposRotorFast;
    BOOL fHaveMagneticBaton = FCarryingMagneticBaton();
    BOOL fBackup = fHaveMagneticBaton;
    int cmBackup = 7.25;

    // Figure out if we have to back up
    switch (goal) {
    case ROLLING_GOAL_3:
    case ROLLING_GOAL_4:
        break;
    case ROLLING_GOAL_MAG:
    case ROLLING_GOAL_1:
        fBackup = true;
        break;
    case ROLLING_GOAL_2:
    default:
        fCanScore = false;
        break;
    }

    if (!fCanScore) {
        PlaySadNoWait();
    } else {
        /* Move the servos into initial position */
        if (fBackup) {
            // Back away from the goal just a bit so we can reach the
            // cylinders which are closest to us.
            //
            DriveForwards(-cmBackup, DRIVE_POWER_MEDIUM);
            //
            MoveSerovsForScoring(rgsvposScoreBackup, goal);
        } else {
            MoveSerovsForScoring(rgsvposScoreNoBackup, goal);
        }

        // Now empty the rotor of its batons.
        //
        // The geometry of the arm is such that we can only be carrying
        // five batons, but we turn through an extra sixth one just
        // to be sure.
        ANGLE degTurnMax = 6 * DEG_ROTOR_TURN_SINGLE_BATON_SCORE;
        //
        if (!fHaveMagneticBaton) {
            // Do it all in one fell swoop
            TurnRotorBy(degTurnMax, svposRotorSpeed);
        } else {
            // Harder. We need to consider each magnetic baton we have
            // and move to the mag goal when we are about to dispense same.

            // Figure out what the angleRotor sensor will read when we're done
            ANGLE degCur = StartReadingAngleSensor(sensAngleRotor, true);
            ANGLE degFinish = degCur + degTurnMax;

            // Loop until we've rotated at least that amount.
            for (;;) {
                // Have we reached the maximum amount of turn necessary?
                LockBlackboard();
                degCur = sensAngleRotor.deg;
                ReleaseBlackboard();
                //
                if (degCur >= degFinish)
                    break;

                // How far until the next magnetic baton?
                ANGLE degRemaining;
                BOOL fAnyMagnetic =
                    FDegUntilNextMagneticBaton(degCur, degRemaining);
                if (!fAnyMagnetic) {
                    // No more magnetic batons

                    // Do we have any more rotation to do?
                    ANGLE degToCompletion = degFinish - degCur;
                    if (degToCompletion >
                        DEG_ROTOR_TURN_SINGLE_BATON_SCORE * 0.5) {
                        // Move to the normal goal
                        MoveSerovsForScoring(rgsvposScoreBackup, goal);

                        // Turn through the remaining angle
                        TurnRotorBy(degToCompletion, svposRotorSpeed);
                    }
                    // And we're done
                    break;
                } else {
                    // At least one more magnetic baton

                    // If we're not to the magnetic baton yet, score any
                    // intervening non-magnetic ones
                    if (degRemaining > 0) {
                        // Move to the normal goal
                        MoveSerovsForScoring(rgsvposScoreBackup, goal);

                        // Dispense things up to the next magnetic
                        TurnRotorBy(degRemaining, svposRotorSpeed);
                    }
                    // Move to the magnetic goal
                    MoveSerovsForScoring(rgsvposScoreBackup, ROLLING_GOAL_MAG);

                    // Dispense the one magnetic baton
                    TurnRotorBy(DEG_ROTOR_TURN_SINGLE_BATON_SCORE,
                        svposRotorSpeed);
                }
            }

            StopReadingAngleSensor(sensAngleRotor);
        }
    }

    /* Pack the servos nicely after we're done */
    PackServosAfterScoring();
}

#endif

//----------------------------------------------------------------------------------
// Positioning
//----------------------------------------------------------------------------------

typedef enum { BACKGROUND_BLACK, BACKGROUND_RED, BACKGROUND_BLUE } BACKGROUND;

BOOL
DriveToWhiteLine(float cmDistance, float cmExtra, BACKGROUND background) {
    InitializeStopConditions(stop);
    int power = DRIVE_POWER_FAST;
    //
#if SensorIsDefined(sensnmEopdFront) || SensorIsDefined(sensnmColor)
    //
    stop.fRequireStop = true;
    stop.cmAdditional = cmExtra;
    //
    // EOPD readings (long range, raw)
    //  White: 206-214
    //  Red:   211-223 (actually also lower)
    //  Black:   0-  4
    //  Blue:   26- 38
    // Conclusion: we can't see a white line on a red background
    // with the EOPD sensor. So we'll have to use a color.
    //
    switch (background) {
    case BACKGROUND_BLACK:
    case BACKGROUND_BLUE:
#if SensorIsDefined(sensnmEopdFront)
        stop.fStopOnEopdFront = true;
        InitializeEopdStopConditions(sensEopdFront);
        sensEopdFront.comparison = COMPARE_GT;
#if ROBOT_NAME==ROBOT_NAME_FTC417_2010_V11
        sensEopdFront.target = 100;
#endif
#if ROBOT_NAME==ROBOT_NAME_FTC417_2010_V12
        sensEopdFront.target = 175;
#endif
        break;
#endif
    case BACKGROUND_RED:
#if SensorIsDefined(sensnmColor)
        power = DRIVE_POWER_SLOW;       // try not to overshoot the line
        stop.fStopOnColor = true;
        stop.cmAdditional -= dcmEopdFrontColor; // TO DO: if this goes negative, the algorithm needs improvement
        SetTargetColor(sensColor, 255, 240, 240);       // Color levels are ok, but certainly not optimized
#endif
        break;
    }
#endif
    //
    return DriveForwards(cmDistance, stop, power);
}

BOOL
GoBalanceOnBridge(ANGLE fieldHeadingZero, BACKGROUND background)
// We're at the white line at the top of the cliff. Go balance on the adjacent bridge!
{
#if ROBOT_NAME==ROBOT_NAME_FTC417_2010_V11 || ROBOT_NAME==ROBOT_NAME_FTC417_2010_V12
    // Balance using encoder-based turning

#if ROBOT_NAME==ROBOT_NAME_FTC417_2010_V11
    const ENCOD denc = 2300;    // correct empirically for 90 right turn (or so) on the cliff
    const ENCOD dencBack = 2150;
    const int cmToBridge = 104;
    const MILLI msBridgeWait = 2000;
#elif ROBOT_NAME==ROBOT_NAME_FTC417_2010_V12
    const ENCOD denc = 1840;    // correct empirically for 90 right turn (or so) on the cliff
    const ENCOD dencBack = 1750;
    const int cmToBridge = 102;
    const MILLI msBridgeWait = 150;
#endif
    InitializeStopConditions(stop);

    // Backup a bit from the white line
    RET_IF_FAIL(DriveBackwards(10), "backwards");

    // Turn right 90 degrees (on cliff)
    RET_IF_FAIL(TurnRightEncod(TURN_POWER_FAST, denc, -denc), "turning right");

    // Use rear sensor to get dist from wall, then drive to middle-ish of bridge
    int cm = 20;                // rough default if we lack a rear sonic REVIEW: '20' needs to be checked
#if SensorIsDefined(sensnmSonicBack)
    cm = ReadSonic_Main(sensSonicBack, true);
#endif
    RET_IF_FAIL(DriveForwards(cmToBridge - cm), "onto bridge");

    // Turn back left 90 degrees (this actually overturns a bit: we're on the bridge now)
    RET_IF_FAIL(TurnRightEncod(TURN_POWER_FAST, -dencBack, dencBack),
        "turning left");
    wait1Msec(msBridgeWait);

    // Drive to the white line and a little bit more to balance.
    // However, if we miss the white line drive far enough to drive
    // off of the bridge and onto the other side (more points than
    // being on bridge but not balanced).

    // If we can use the front EOPD, then this is amount past seeing the
    // white line that we should drive in order to balance. When balancing
    // on the blue bridge, this is what we actually use.

    // On a red background, we'll be using the color sensor, which means
    // by the time we see the line we're already past the balance point (though
    // only just a bit). Rather than backing up a very tiny amount, which is hard
    // to do well, we continue past by a chunk, then back up that larger amount.

#if ROBOT_NAME==ROBOT_NAME_FTC417_2010_V11
    int cmWhiteToEopd = 18;
    MILLI msRed = 2000;
    int pwrRed = DRIVE_POWER_SLOW;
    int cmRedFudge = (BACKGROUND_RED == background ? 10 : 0);
    int cmRedFudgeBack = cmRedFudge + 2;
#elif ROBOT_NAME==ROBOT_NAME_FTC417_2010_V12
    int cmWhiteToEopd = 12.5;
    MILLI msRed = 100;
    int pwrRed = 70;
    int cmRedFudge = (BACKGROUND_RED == background ? 7 : 0);
    int cmRedFudgeBack = 0;
#endif

    // How far to go in total
    int cmPastWhite = cmWhiteToEopd + cmRedFudge;

    // The '180' here is far enough to get us off the bridge if for some reason
    // we totally miss seeing the white line.
    RET_IF_FAIL(DriveToWhiteLine(180, cmPastWhite, background), "ToWhite+bal");
    if (cmRedFudge != 0 && cmRedFudgeBack != 0) {
        wait1Msec(msRed);       // dissipate the inertia
        RET_IF_FAIL(DriveForwards(-cmRedFudgeBack, pwrRed), "back");
    }
    //
    return true;
#else
    return false;
#endif
}

BOOL
PositionUsingEOPD(DISPENSER disp, int cm, SIDE side)
// Given that we parallel to the dispenser at a reasonable distance, unpack the EOPD
// and use it to position ourselves front-to-back at exactly the position needed
// to dispense.
{
#if SensorIsDefined(sensnmEopdArm)

    int driveDirReflection = LEFT_SIDE == side ? 1 : -1;

    // Note: a zero reading on the EOPD is essentially 'infinite' distance.
    // Larger readings represent closer distances, and this is in proportion
    // to the inverse square of the distance: half the distance gives four times
    // the reading. If you remember that the EOPD is measuring an amount of
    // reflected light, this all makes sense.
    RET_IF_FAIL(UnpackDispenserArmForReadingEopd(disp, cm, side),
        "UnpackEOPD");

    // Drive until EOPD sees a 'close' value
    InitializeStopConditions(stop);
    stop.fStopOnEopdArm = true;
    stop.fRequireStop = true;
    sensEopdArm.target = 10;
    sensEopdArm.comparison = COMPARE_GT;
    RET_IF_FAIL(DriveForwards(55 * driveDirReflection, stop, 30), "DriveToEOPD");       // 30

    // Settle down the inertia
    wait1Msec(2000);            // should be lower //800

    // now drive back slowly until no EOPD
    InitializeStopConditions(stop);
    stop.fStopOnEopdArm = true;
    stop.fRequireStop = true;
    sensEopdArm.target = 2;
    sensEopdArm.comparison = COMPARE_LT;

    RET_IF_FAIL(DriveForwards(-25 * driveDirReflection, stop, 12), "FineTuneEOPD");     // v11 was power=20
    // At -30 was too far over at the dispenser. At -27cm at a low power of 11 it aligns really well.
    // The high power theory is correct about the arm moving too much, however the distance issue was not an EOPD or a
    // ultrasonic issue, but rather we were driving too far when it aligns.

    return true;
#else
    return false;
#endif
}

// This starts at the EOPD position, moves and dispenses, then (v11) unmoves
// and returns to EOPD or (v12) clears the dispenser enought such that a subsequent
// packing request will work w/o jamming on the dispenser.
BOOL
DispenseFromEOPDPosition(DISPENSER disp, int cm,
    BATON_DISPOSITION batonDisposition) {
    BOOL fSuccess = HAS_ARM_SERVOS;

#if ROBOT_NAME==ROBOT_NAME_FTC417_2010_V12

    // Where are the bottoms of the dispensers?
    float yDispenserBottom;
    switch (disp) {
    case DISP_LOW:
        yDispenserBottom = 8.3;
        break;                  // measured by tony romano
    case DISP_MED:
        yDispenserBottom = 15.5;
        break;                  // measured by tony romano
    case DISP_HIGH:
        yDispenserBottom = 22.5;
        break;                  // measured by tony romano
    }

    // What do we know about the height at which we want to dispense?
    float dyBelowDispenser = 3.0;
    float yTargetFieldCoords = yDispenserBottom - dyBelowDispenser;

    // How high is the arm coordinate system above the field?
    float dyCoordinateSystem = 17.1;

    // Convert that height from field coordinates to bot coordinates by
    // subtracting the height of the arm coordinate system origin from the field
    float yTarget = yTargetFieldCoords - dyCoordinateSystem;

    TRACE(("cm           = %d", cm));
    TRACE(("yBotom       = %f", yDispenserBottom));
    TRACE(("yTargetField = %f", yTargetFieldCoords));
    TRACE(("yTarget      = %f", yTarget));

    // The arm is at the EOPD position. We need to compute its X coordinate
    // so we can execute a downward vertical movement.
    float xCur, yCur;
    GetArmTipLocation(OUT xCur, OUT yCur);

    // Move the arm vertically down to just below the dispensing location
    MoveArmTipTo(xCur, yTarget, MOVEARMTIP_DIRECT);
    WaitForServos();

    // Remember (precisely) where the arm is right now so we can return
    // to this location after dispensing
    int svposShoulder = GetServoValue(svoArmShoulder);
    int svposElbow = GetServoValue(svoArmElbow);

    // Move the arm outwards horizontally a bit to just underneath
    // where we want to be
    float xOut = xCur + 6.0;
    MoveArmTipTo(xOut, yTarget, MOVEARMTIP_DIRECT);
    WaitForServos();

    // The wrist/rotor sometimes bounces a bit in these movements.
    // Wait for that to settle down.
    wait1Msec(500);             // probably not needed

    float dyUp = 6.7;
    if (STOP_BEFORE_DISPENSING == batonDisposition) {
        dyUp = 3.0;
    }
    // Move the rotor up and into the dispensing position
    MoveArmTipTo(xOut, yTarget + dyUp, MOVEARMTIP_DIRECT);
    WaitForServos();

    if (STOP_BEFORE_DISPENSING == batonDisposition) {
        return true;
    }
    // Move the tip out and in a bit to try to ensure that the
    // rotor is seated correctly: sometimes only one of the seats
    // gets in the right spot, and the other is hooked underneath
    // the dispenser
    wait1Msec(500);             // was 1000; may not be needed at all
    float dx = 1.25;            // was 1.5, which seemed risky of slipping off *both* seats
    MoveArmTipBy(-dx, 0, MOVEARMTIP_DIRECT);
    wait1Msec(500);             // was 1000; may not be needed at all
    MoveArmTipBy(dx, 0, MOVEARMTIP_DIRECT);

    // Unload the batons
    UnloadDispenser(batonDisposition);

    // Get the arm out of harm's way so that subsequent packing will clear the dispenser
    MoveServo(svoArmShoulder, svposShoulder);
    MoveServo(svoArmElbow, svposElbow);
    WaitForServos();

    // Get some clearance so we don't hit the bot on the way back
    // Better: move vertically to the zero y coord
    MoveArmTipBy(-2, 17 + 6, MOVEARMTIP_DIRECT);

#elif ROBOT_NAME==ROBOT_NAME_FTC417_2010_V11
    if (CmDispenseMin(disp) <= cm && cm < CmDispenseMax(disp)) {
        // Move the arm into position, starting the rotor a-spinning for the last movement
        DISPENSINGPATH path;
        fSuccess = MoveFromEOPDToDispensing(OUT path, disp, cm, true);
        if (fSuccess) {
            // Dispense! The rotor is stopped on exit.
            UnloadDispenser();

            // Back out along the path
            for (int i = path.cpt - 2; i >= 0; i--) {
                MoveToArmPoint(path.rgpt[i], false);
            }
        }
    } else
        fSuccess = false;
#endif

    return fSuccess;
}
