//
// Autonomous.h
//
//--------------------------------------------------------------------------------------
// V12 autonomous configuration
//--------------------------------------------------------------------------------------

// Do we try to score batons (true) or do we (false) just push the
// rolling goal around as needed. Note: setting to false is PURELY a
// time optimization (when we're not actually carrying batons, of course).
BOOL fScorePreloadedBatons  = false;

// Do we (true) push the rolling goal to mid-field to try to block others
// or (false) just push it around enough to be able to score reliably and
// to get it out of our own way.
BOOL fPushGoalToMidField   = true;

// Once we deal with the rolling goal, do we head across the bridge for
// the middle dispenser (true) or do we revert to the 'classic' strategy
// of going up the cliff (false)
BOOL fGoForMiddleDispenser = false;

// If fGoForMiddleDispenser is false, do we (true) go for the gusto of trying
// in the 'classic' manner to dispense from the left dispenser or do
// we just instead go balance on the bridge (false)
BOOL fGoForLeftDispenser   = false;

//--------------------------------------------------------------------------------------
// V12 autonomous logic
//--------------------------------------------------------------------------------------

#if ROBOT_NAME==ROBOT_NAME_FTC417_2010_V12
BOOL DoAutonomous()
    {
    msElapsedStart = nSysTime;
    TRACE(("**** begin autonomous ****"));
    DisplayMessage("<- autonomous ->");

    fHaltProgramOnMotorStall = true;

    // Validate parameters: we can't do the classic strategy from the right side
    if (STARTING_SIDE_RIGHT==startingSide)
        {
        fGoForMiddleDispenser = true;
        }

    // Set a reflection scale so we can have this code work from both left and right
    const int sideReflection = (STARTING_SIDE_LEFT == startingSide ? 1 : -1);

    // Turn a little bit right so that we'll edge away from the wall, which
    // we need to do before we can complete a 90 deg turn.
    const ANGLE degInitial = 40;
    RET_IF_FAIL(DriveForwards(9.0),                         "init drive");
    RET_IF_FAIL(TurnRight(degInitial * sideReflection),     "init turn");

    // Drive away from the wall
    RET_IF_FAIL(DriveForwards(60.0),                        "away from wall");

    // Turn the remaining amount necessary to be square on at the rolling goal
    const ANGLE deg90Remaining = 90.0 - degInitial + 6; // last term is fudge
    RET_IF_FAIL(TurnRight(deg90Remaining * sideReflection), "to90");
    wait1Msec(200);     // settle some inertia

    // Drive to the goal and nudge up against it. Exactly how far we push it
    // depends on what our intent here is.
    float cmExtra = 0.0;
    if (fPushGoalToMidField)
        cmExtra = 46.0;
    else if (fGoForMiddleDispenser)
        cmExtra = 25.0;    // we need to get the goal out of the way of the bridge (distance to be tuned)
    else
        cmExtra = 5.0;     // still worth it to deal with the castors? (distance to be tuned)
    //
    const float cmToGoal     = 24.0 + 5.0 + 5.0 + cmExtra;
    const float cmToGoalSlow = 12.0 + 5.0 + 5.0 + cmExtra;
    const float cmToGoalFast = cmToGoal - cmToGoalSlow;
    RET_IF_FAIL(DriveForwards(cmToGoalFast),                            "fast to goal");
    wait1Msec(100);
    RET_IF_FAIL(DriveForwards(cmToGoalSlow, DRIVE_POWER_SLOW + 10),     "slow to goal");

    // Score the batons if requested
    if (fScorePreloadedBatons)
        {
        ScoreBatonsInRollingGoal(ROLLING_GOAL_4);
        }

    // To the middle? Or to 'classic'?
    if (fGoForMiddleDispenser)
        {
        // Drive backwards so as to approach bridge.
        RET_IF_FAIL(DriveForwards(-(cmExtra + 4.0)),        "approach bridge"); // was -50 when cmExtra was 46

        // Turn to point to the bridge
        RET_IF_FAIL(TurnRight(-94 * sideReflection),        "point to bridge");

        // Over the bridge and through the woods
        RET_IF_FAIL(DriveForwards(210),                     "over bridge");

        // Use the front sonic to get a good read on how far we are
        // from the wall, then drive until to our desired distance.
        int cmFromWallCur     = ReadSonic_Main(sensSonicFront, true);
        int cmFromWallDesired = 37;
        RET_IF_FAIL(DriveForwards(cmFromWallCur - cmFromWallDesired), "to desired distance");

        // Turn parallel to the wall, pointing to the left side of the field
        RET_IF_FAIL(TurnRight(-90),                         "point left");     // NOTE: *not* -90 * sideReflection

        // Approach the middle goal and straighten out
        int cmExtra = (STARTING_SIDE_RIGHT==startingSide ? 0 : 0);
        RET_IF_FAIL(DriveForwardsAndStraightenUsingRightSonic(-(72+33) * sideReflection + cmExtra, DRIVE_POWER_FAST), "straighten");

        // How far are we from the wall
        cmFromWallCur = ReadSonic_Main(sensSonicRight, true);
        TRACE(("eopd cm=%d", cmFromWallCur));

        // Deploy the EOPD on the ARM and do the lateral dance
        if (PositionUsingEOPD(DISP_MED, cmFromWallCur, RIGHT_SIDE))
            {
            // Dispense things
            if (DispenseFromEOPDPosition(DISP_MED, cmFromWallCur, SPILL_BATONS))
                {
                // Put the arm away nicely
                PackDispenserArmFromEOPD(DISP_MED, cmFromWallCur, RIGHT_SIDE);
                }
            }

        // Make *sure* the arm isn't left out there dangling for someone to hit
        PackArmSafelyFromAnywhere(false);
        }

    else
        {
        // 'Classic' mode

        // Drive until the sonic is in range, then read the sonic
        // to see how far we are from the wall
        int cmFromWallCur;
        for (;;)
            {
            cmFromWallCur = ReadSonic_Main(sensSonicBack, false);
            if (cmFromWallCur != cmSonicNil)
                break;
            DriveForwards(-80);
            }

        // Back up to the desired distance.
        int cmFromWallDesired = 17;
        DriveForwards(cmFromWallDesired - cmFromWallCur);

        // Turn towards the cliff
        TurnRight(-93.0);

        // Get up on the cliff before starting to look for white, as the leading edge
        // of the cliff sometimes reads as white.
        RET_IF_FAIL(DriveForwards(20, DRIVE_POWER_FAST), "OnToCliff");

        // Now we should be aligned to drive straight up the ramp.
        // Drive up the ramp until we hit the white line at the lip of the cliff.
        RET_IF_FAIL(DriveToWhiteLine(80, 0.0, BACKGROUND_BLACK), "DriveToWhiteLine");

        // Check the sonic sensor to see if there's someone in our way or not.
        // Note that we want the space in front of the dispenser to be clear, but
        // we are ok with an opposing robot still sitting there in its initial
        // eighteen inch square. Thus, the distance checked here is subtle.
        int cmFront = ReadSonic_Main(sensSonicFront,false);
        TRACE(("bot in way? %dcm", cmFront));

        BOOL fRobotInTheWay = (cmFront < 90) || !fGoForLeftDispenser; // REVIEW: threshold value needs verification

        if (!fRobotInTheWay)
            {
            // To which dispenser are we headed?
            DISPENSER disp = (TEAM_COLOR_RED==teamColor) ? DISP_LOW : DISP_HIGH;

            // Drive over cliff.
            RET_IF_FAIL(DriveForwards(59), "over cliff");
            wait1Msec(400);     // wait for inertia to settle

            // Turn so that we will drive farther away from the wall
            RET_IF_FAIL(TurnRight(28), "clear disp");

            // Drive to past the dispenser and further away from wall
            RET_IF_FAIL(DriveForwards(70), "clear disp");

            // Straighten back up, but pointing back the way we came
            RET_IF_FAIL(TurnRight(180-12), "straighten");

            // more to come ....
            }
        else
            {
            // Robot is blocking our way
            PlaySadNoWait();
            //
            BACKGROUND background = (TEAM_COLOR_BLUE==teamColor) ? BACKGROUND_RED : BACKGROUND_BLUE;
            GoBalanceOnBridge(0.0, background);
            //
            // Be silly. Note that if we are the blue team, then we balance on the red bridge.
            //
            TraceElapsed();
            if (TEAM_COLOR_BLUE==teamColor) PlayMusic(musicLadyInRed,120,NOTE_QUARTER);
            }
        }

    return true;
    }

//--------------------------------------------------------------------------------------
// V11 autonomous logic
//--------------------------------------------------------------------------------------
#elif ROBOT_NAME==ROBOT_NAME_FTC417_2010_V11

BOOL DoAutonomousPreloads(OUT ANGLE& angleCur)
// From the starting position, navigate to the rolling goal and dump our preloads.
// Return through the angleCur OUT parameter the angle we believe we have turned
// through in the process.
    {
    // Turn towards the rolling goal
    const ANGLE angleInitialTurn = 45;
    RET_IF_FAIL(TurnRight(angleInitialTurn), "TurnRight");
    wait1Msec(100);

    // Drive to the rolling goal
    RET_IF_FAIL(DriveBackwards(63.0), "DriveBackwards");

    // Give feedback as to which team color we are. This message is here (and not
    // earlier) for historical reasons: at one time, we actually had to drive across
    // the line around the starting corner detecting it with a color sensor in order
    // to learn which team color we were.
    if (TEAM_COLOR_RED==teamColor)
        {
        TRACE(("we are red team"));
        DisplayMessage("we are red team");
        }
    else
        {
        TRACE(("we are blue team"));
        DisplayMessage("we are blue team");
        }

    // Extend the preload arm so we can turn and see if touch sensor triggers against the rolling goal.
    ExtendPreloadArm();

    // Turn until the touch sensor hits the rolling goal
    const ANGLE angleSweepMax = 55;
    InitializeStopConditions(stop);
    stop.fStopOnTouchPreload = true;
    RET_IF_FAIL(TurnRight(angleSweepMax, TURN_POWER_MEDIUM, stop), "TurnRight");
    ANGLE angleSweepActual = (stop.fStopConditionReached ? (1.0 - stop.fractionRemaining) : 1.0) * angleSweepMax;

    // Back up a bit to get the preload arm right over the goal
    const ANGLE angleSweepAdjust  = -2.0;
    InitializeStopConditions(stop);
    RET_IF_FAIL(TurnRight(angleSweepAdjust, TURN_POWER_FAST, stop), "TurnLeft");

    // Dump the preloaded battons into the rolling goal!
    DumpPreload(true);
    RetractPreloadArm();

    // Point towards the wall in order to get a good reading from the front sonic
    // We want to effect a 90 degree turn from our initial starting position, so
    // we turn through whatever angle remains of that from our current angle
    ANGLE angleFudge = 0.0;
    angleCur         = angleInitialTurn + angleSweepActual + angleSweepAdjust + angleFudge;
    TRACE(("sweep=%3.1f cur=%3.1f", angleSweepActual, angleCur));

    return true;
    }

BOOL DoAutonomous()
// Implement our autonomous strategy
    {
    msElapsedStart = nSysTime;
    TRACE(("**** begin autonomous ****"));
    DisplayMessage("<- autonomous ->");

    fHaltProgramOnMotorStall = true;

    // Record the initial starting position in the trancript. This is helpful
    // for debugging near misses on the preloads
    if (true)
        {
        int cmFront = 255, cmRight = 255;
#if SensorIsDefined(sensnmSonicRight)
        cmRight = ReadSonic_Main(sensSonicRight, false);
#endif
#if SensorIsDefined(sensnmSonicFront)
        cmFront = ReadSonic_Main(sensSonicFront, false);
#endif
        TRACE(("cmF=%d, cmR=%d", cmFront, cmRight));
        }

    // Dump the preloads
    ANGLE angleCur = 0.;
    RET_IF_FAIL(DoAutonomousPreloads(OUT angleCur), "DoAutonomousPreloads");

    // Line up perpendicular to the wall
    RET_IF_FAIL(TurnRight(90.0 - angleCur), "TurnToWall");

    // Drive forwards until we get close to the wall. We go a certain distance
    // based on the ultrasound (it's as close as we can reasonably read?), and
    // then a little bit more.
    RET_IF_FAIL(DriveForwardsToSonic(27,10), "DriveForwardsToSonic");

    // Turn back to original angle, but facing forwards, not backwards
    RET_IF_FAIL(TurnRight(90), "Turn90");

    //Back up so that the ultrasonic is not reading on the dispenser
    RET_IF_FAIL(DriveBackwards(5.0), "DriveBackwards");

    // Drive a bit, then use the wall to straighten out
    RET_IF_FAIL(DriveForwardsAndStraightenUsingLeftSonic(55, DRIVE_POWER_FAST), "DriveForwardsAndStraightenLeft");

    // Get up on the ramp before starting to look for white, as the leading edge
    // of the ramp sometimes reads as white.
    RET_IF_FAIL(DriveForwards(20, DRIVE_POWER_FAST), "OnToRamp");

    // Now we should be aligned to drive straight up the ramp.
    // Drive up the ramp until we hit the white line at the lip of the cliff.
    RET_IF_FAIL(DriveToWhiteLine(70, 0.0, BACKGROUND_BLACK), "DriveToWhiteLine");

    // Check the sonic sensor to see if there's someone in our way or not.
    // Note that we want the space in front of the dispenser to be clear, but
    // we are ok with an opposing robot still sitting there in its initial
    // eighteen inch square. Thus, the distance checked here is subtle.
    int cmFront = cmSonicNil;
#if SensorIsDefined(sensnmSonicFront)
    cmFront = ReadSonic_Main(sensSonicFront,false);
#endif
    TRACE(("Sonic is %d", cmFront));

    BOOL fRobotInTheWay = (cmFront < 90);

    if (!fRobotInTheWay)
        {
        // To which dispenser are we headed?
        DISPENSER disp = (TEAM_COLOR_RED==teamColor) ? DISP_LOW : DISP_HIGH;

        // Drive over cliff.
        RET_IF_FAIL(DriveForwards(59), "OverCliff");

        // Drive on to mats at the other side of the low goal. Make
        // sure that we clear the left sonic past the dispenser so that
        // at the low dispenser we still read off the wall.
        RET_IF_FAIL(DriveForwardsAndStraightenUsingLeftSonic(72, DRIVE_POWER_FAST), "AfterCliff");

        // Adjust distance to wall if we have to and are so able
        int cm = cmSonicNil;
#if SensorIsDefined(sensnmSonicLeft)
        cm = ReadSonic_Main(sensSonicLeft,true);
#endif

        if (cm < CmDispenseCanTurnMin(disp))
            {
            float dcmCenterRotation = cm + 16; //distance from center of rotation to the wall
            float dcmHalfDiagonal   = 30;      //half diagonal of the bot, for calculation reasons
            ANGLE radAngleBeta      = asin(21. / dcmHalfDiagonal); //half length over hypotenuse
            ANGLE radAngleTheta     = asin(dcmCenterRotation / dcmHalfDiagonal) - radAngleBeta;
            RET_IF_FAIL(AdjustLaterallyUsingAngle((float)(CmDispenseCorrect(disp) - cm), radiansToDegrees(radAngleTheta)),"AdjustLaterally");
            }
        else if (cm < CmDispenseMin(disp))
            {
            TRACE(("too close: cm=%d", cm));
            RET_IF_FAIL(AdjustLaterally((float)(CmDispenseCorrect(disp) - cm)), "AdjustLaterally");
            }
        else if (cm > 70)
            {
            // Paranoia: avoid clobbering things (not tested)
            TRACE(("way too far: cm=%d", cm));
            }
        else if (cm >= CmDispenseMax(disp))
            {
            TRACE(("too far: cm=%d", cm));
            RET_IF_FAIL(AdjustLaterally((float)(CmDispenseCorrect(disp) - cm)), "AdjustLaterally");
            }
        else
            {
            TRACE(("dist ok: cm=%d", cm));
            }

        // Unpack the arm if we have enough room
#if SensorIsDefined(sensnmSonicLeft)
        cm = ReadSonic_Main(sensSonicLeft,true);
#endif
        TRACE(("cmAfterAdjust=%d", cm));

        if (cm >= CmDispenseMin(disp) && cm < CmDispenseMax(disp))
            {
            RET_IF_FAIL(PositionUsingEOPD(disp,cm), "PositionUsingEOPD");

            // rest a bit (may not be needed: we're trying to see why the calibrated values sometimes fail)
            wait1Msec(200);

            // Dispense, then pack the arm
            RET_IF_FAIL(DispenseFromEOPDPosition(disp, cm), "Dispense");
            RET_IF_FAIL(PackDispenserArmFromEOPD(disp, cm), "PackArmFromEOPD");
            }
        else
            {
            if (cm < CmDispenseMin(disp))
                TRACE(("too close for EOPD"));
            else
                TRACE(("too far for EOPD"));
            PlaySad();
            }
        //
        TraceElapsed();
        }
    else
        {
        PlaySad();  // robot is blocking our way
        BACKGROUND background = (TEAM_COLOR_BLUE==teamColor) ? BACKGROUND_RED : BACKGROUND_BLUE;
        GoBalanceOnBridge(0.0, background);
        //
        // Be silly. Note that if we are the blue team, then we balance on the red bridge.
        //
        TraceElapsed();
        if (TEAM_COLOR_BLUE==teamColor) PlayMusic(musicLadyInRed,120,NOTE_QUARTER);
        }

    return true;
    }

//--------------------------------------------------------------------------------------
// Other autonomous logic
//--------------------------------------------------------------------------------------
#else
BOOL DoAutonomous()
    {
    DisplayMessage("<- autonomous ->");
    return true;
    }
#endif
