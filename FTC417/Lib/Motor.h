//
// Motor.h
//
// Implementation of motor functionality.

#define ControllerOfMotor(motor)                rgmotorctlr[motor.imotorctlr]
#define ControllerFieldOfMotor(motor,field)     ControllerOfMotor(motor).field
#define MtrOf(motor)                            ControllerOfMotor(motor).rgmtr[motor.imtr]

#define IsLeftMotor(motor)                              (motor.imotor == motorLeft.imotor && motorLeft.fActive)
#define IsRightMotor(motor)                             (motor.imotor == motorRight.imotor && motorRight.fActive)
#define DifferentMotorControllers(motorRed, motorBlue)  (motorRed.imotorctlr != motorBlue.imotorctlr)

void InitializeMotor(IN OUT STICKYFAILURE& fOverallSuccess, MOTOR& motor, MOTORCONTROLLER& controllerParam, ubyte jmtr /*one based*/, string displayNameParam, int prop)
    {
    BOOL fSuccess = true;
    LockDaisy();

    InitializeLegoControllerIfNecessary();

    motor.imotorPaired = -1;

    /* Figure out which index this motor is in the rgmotor array */
    motor.imotor = -1;
    int i = 0;
    while (rgmotor[i].imotor >= 0)
        i++;
    motor.imotor = i;

    motor.imotorctlr    = controllerParam.imotorctlr;
    motor.imtr          = jmtr-1;
    motor.displayName   = displayNameParam;
    motor.encStallPrev  = 0;
    motor.msLastOkStall = 0;
    motor.msLastEncStall = 0;
    MtrOf(motor).power  = 0;
    motor.fReflected    = !!(MOTORPROP_REFLECTED & prop);
    if (motor.fReflected)
        {
        MtrOf(motor).mode |= MOTORMODE_REFLECTED;
        }

    motor.fActive = true;
    MtrOf(motor).fActive = true;

    motor.fHasEncoder = !!(MOTORPROP_ENCODER & prop) || ControllerOfMotor(motor).fLegoController;
    motor.fStallCheck = (motor.fHasEncoder && !(MOTORPROP_NOSTALLCHECK & prop));
    if (motor.fHasEncoder)
        {
        if (IsLeftMotor(motor) || IsRightMotor(motor))
            {
            ControllerOfMotor(motor).fHasActiveEncoder = true;
            }
        }

    SendMotorModePower(fSuccess, ControllerOfMotor(motor));
    waitForMotorControllerCommandCycle(ControllerOfMotor(motor));

    ReleaseDaisy();
    if (fOverallSuccess) fOverallSuccess = fSuccess;
    TraceInitializationResult1("motor[%s]", displayNameParam, fOverallSuccess);
    }


// Zero the values of the left and right motor encoders
// REVIEW: do we need the blackboard lock for this guy?
void ZeroEncoders()
    {
    LockDaisy();
    BOOL fSuccess = true;
    /**/
    MOTORMODE modePrevLeft, modePrevRight;
    SetControllerMtrRunMode(modePrevLeft,  ControllerOfMotor(motorLeft),  motorLeft.imtr,   MOTORMODE_RESET_CURRENT_ENCODER);
    SetControllerMtrRunMode(modePrevRight, ControllerOfMotor(motorRight), motorRight.imtr,  MOTORMODE_RESET_CURRENT_ENCODER);
    /**/
    SendMotorModePower(fSuccess, ControllerOfMotor(motorLeft));
    if (DifferentMotorControllers(motorLeft, motorRight))
        {
        SendMotorModePower(fSuccess, ControllerOfMotor(motorRight));
        }
    waitForMotorControllerCommandCycle2(ControllerOfMotor(motorLeft),ControllerOfMotor(motorRight));
    /**/
    MtrOf(motorLeft).mode = modePrevLeft;
    MtrOf(motorRight).mode = modePrevRight;
    /**/
    SendMotorModePower(fSuccess, ControllerOfMotor(motorLeft));
    if (DifferentMotorControllers(motorLeft, motorRight))
        {
        SendMotorModePower(fSuccess, ControllerOfMotor(motorRight));
        }
    waitForMotorControllerCommandCycle2(ControllerOfMotor(motorLeft),ControllerOfMotor(motorRight));
    /**/
    ReleaseDaisy();
    }

// Read the value of the left and right encoders into the indicated variables
// You must own the daisy lock to call this function because otherwise
// there's contention for the formal parameters. Normally, that is accomplished
// within the macro ReadEncoders.
void ReadEncodersInternal(OUT ENCOD& encLeft, OUT ENCOD& encRight)
    {
    BOOL fSuccess = true;
    encLeft = encRight = 0;
    CheckLockHeld(lockDaisy);

    // Read the left encoder
    //
    BOOL fReadLeftController = false;
    if (motorLeft.fActive && motorLeft.fHasEncoder)
        {
        while (true)
            {
            ReadControllerEncoders(fSuccess, ControllerOfMotor(motorLeft));
            if (fSuccess)
                {
                fReadLeftController = true;
                encLeft = MtrOf(motorLeft).encoder;
                break;
                }
            //
            ReleaseDaisy();
            EndTimeSlice();
            LockDaisy();
            }
        }
    //
    // Read the right encoder
    //
    if (motorRight.fActive && motorRight.fHasEncoder)
        {
        if (!fReadLeftController || DifferentMotorControllers(motorLeft,motorRight))
            {
            while (true)
                {
                ReadControllerEncoders(fSuccess, ControllerOfMotor(motorRight));
                if (fSuccess)
                    {
                    encRight = MtrOf(motorRight).encoder;
                    break;
                    }
                //
                ReleaseDaisy();
                EndTimeSlice();
                LockDaisy();
                }
            }
        else
            {
            // We already have the info we need in the (shared) motor controller
            encRight = MtrOf(motorRight).encoder;
            }
        }
    }

#define ReadEncoders(encLeft, encRight)                 \
    {                                                   \
    LockDaisy();                                        \
    ReadEncodersInternal(encLeft, encRight);            \
    ReleaseDaisy();                                     \
    }

// Legacy API for when ReadEncoders required the blackboard lock, not the daisy lock
#define ReadEncoders_Lock(encLeft, encRight)    ReadEncoders(encLeft, encRight)

//---------------------------------------------------------------------------------------
// Managing motor power levels
//---------------------------------------------------------------------------------------

// This needs to change according to whether or not we need to do manual
// reversing of the motor power and encoder values or whether we've managed
// to get the motor controller to do that for us.
#define InternalPowerForMotor(motor, power)     (power)


// The documentation to the HiTechnic Motor Controller has the following to say about
// how it interprets power levels:
//
//      This field may be set in the range of -100 to +100. 0 is used to set Off/Brake.
//      Any value above 100 will be treated as 100, except –128 which is used to set
//      Off/Float. Negative values run the motor in the reverse direction. The power
//      should be set positive when being used as a parameter for the 'Run to position'
//      command. This field sets the rotation rate in 'Run with constant speed' and maximum
//      rotation rate in 'Run to position' in the range of -1000 – +1000 degrees per second
//      represented by the range -100 – +100. As with 'PWM mode', 0 is used to set Off/Brake
//      while –128 is used to set Off/Float.
//
// We here use motors in the 'Run with constant speed' mode (see MOTORMODE_RUN_WITH_CONSTANT_SPEED).
// Thus, our power levels are interpreted as a requested rotation rate in units of tens of
// degrees per second: power==10 -> 100 deg/s, power==50 -> 500 deg/s, etc.
//
// That's the requested speed. However, on some robots it has been observed that, due to friction,
// etc, the maximum speed that can be achieved on one or more of its motors is somewhat less than
// 1000 deg/s. If one of the motors has significantly less such friction that the other, then this
// can cause problems when the motors are both commanded to go at, say, a power level of 100: the
// right motor (say) may actually be able to achieve a speed of 1000 deg/s, whereas the left motor
// may max out at 720 deg/s, causing the robot to veer left when a straight-ahead motion was
// expected.
//
// This issue is exacerbated by fluctuating power levels.
//
// This issue could be addressed by simply using lower power-levels all around in higher levels
// of the software. While viable, this approach is tedious.
//
// As an alternative, we provide here the motorPowerScale variable. This floating point value is
// used to compress the normal [-100,100] power range to a smaller one (or possibly a larger one
// with a clipped upper range, though that would be weird). In our example above, setting the
// value of motorPowerScale to 0.72 would avoid maxing out either motor, resulting in straighter
// driving behaviour, though at the cost, of course, of somewhat reduced overall speed.
//
// The motorPowerScale variable bears a conceptual resemblance to the (non-functional, documentation
// to the contrary notwithstanding) RobotC built-in variable nMaxRegulatedSpeed12V.

#ifndef DEFAULT_MOTOR_POWER_SCALE
#define DEFAULT_MOTOR_POWER_SCALE 1.0
#endif

#ifndef motorPowerScale
float motorPowerScale = DEFAULT_MOTOR_POWER_SCALE;
#endif

// Record the desire that this motor have the indicated power level. Note that this
// doesn't actually take effect until a subsequent SendMotorPowers().
//
// Note that the blackboard lock is (currently) required for calling this function,
// though that should reasonably change to be the daisy lock.
//
void SetMotorPower(MOTOR& motor, int powerArg)
    {
    short power_ = Rounded((float)powerArg * motorPowerScale, short);
    ClampVar(power_, -100, 100);
    RecordMotorPower(
            motor.imotorctlr,
            motor.imtr,
            InternalPowerForMotor(motor,power_));
    if (motor.imotorPaired != -1)
        {
        RecordMotorPower(
            rgmotor[motor.imotorPaired].imotorctlr,
            rgmotor[motor.imotorPaired].imtr,
            InternalPowerForMotor(rgmotor[motor.imotorPaired], power_));
        }
    }

// Send the current power levels to all the active controllers.
#define SendMotorPowers()                                                       \
    {                                                                           \
    BOOL fSuccess = true;                                                       \
    LockDaisy();                                                                \
    for (int imotorctlr = 0; imotorctlr < imotorctlrMax; imotorctlr++)          \
        {                                                                       \
        SendMotorPower(OUT fSuccess, rgmotorctlr[imotorctlr]);                  \
        }                                                                       \
    ReleaseDaisy();                                                             \
    }

#define StopRobot()                 \
    {                               \
    LockBlackboard();               \
    SetMotorPower(motorLeft,  0);   \
    SetMotorPower(motorRight, 0);   \
    SendMotorPowers();              \
    ReleaseBlackboard();            \
    }

//---------------------------------------------------------------------------------------
// Stall detection logic: avoid motor burnout
//---------------------------------------------------------------------------------------

// Our behaviour when we encounter a motor stall can be adjusted
BOOL fHaltProgramOnMotorStall = true;

// We've hit a stall. Deal with it according to how the current app wishes.
void AbortDueToMotorStall(MOTOR& motor, MILLI ms)
    {
    hogCpuNestable();  /* paranoia: we don't want another task to restart the motors after we turn them off */
    SetMotorPower(motorLeft, 0);
    SetMotorPower(motorRight, 0);
    SendMotorPowers();
    PlayFifthNoWait();
    TRACE(("stall: %dms %s", ms, motor.displayName));
    if (fHaltProgramOnMotorStall)
        {
        StopAllTasks();
        }
    else
        {
        wait1Msec(300);

        // Reinit the motors after the stall in an attempt to diagnose odd
        // bug seen in pushing war wherein motors appears to lose config
        // and so, e.,g, we don't know which motors are to be reversed.
        STICKYFAILURE fOverallSuccess = true;
        InitializeMotors(fOverallSuccess);
        }
    releaseCpuNestable();
    }

MILLI msLastMotorStallCheck = 0;
MILLI msLastMotorControllerTickle = 0;

// Does this motor effectively not have enough power to be worried about a stall thereon?
#define FMotorNotPoweredStallCheck(motor)   (abs(MtrOf(motor).power) <= 5)

// Check for motor stall, and 'abort' if stall is detected.
// Further, be sure in all cases to tickle all the active
// motor controllers, lest they time out and reset themselves.
#define CheckForMotorStall(msNow)                                                                                                                   \
    {                                                                                                                                               \
    BOOL fFirstTime = (0 == msLastMotorStallCheck);                                                                                                 \
    /* We have a throttle on how often we check for stall due to a worry about reading the motor encoders too frequently */                         \
    if (fFirstTime || msNow > msLastMotorStallCheck + 200) /* 200 is just a rule of thumb */                                                        \
        {                                                                                                                                           \
        /* Tickle motor contollers that won't be tickled by ReadEncoders. */                                                                        \
        /* But we only have to avoid the 2.5s timeout.                    */                                                                        \
        if (msNow > msLastMotorControllerTickle + 1000)                                                                                             \
            {                                                                                                                                       \
            LockDaisy();                                                                                                                            \
            for (int imotorctlr = 0; imotorctlr < imotorctlrMac; imotorctlr++)                                                                      \
                {                                                                                                                                   \
                if (rgmotorctlr[imotorctlr].fActive && !rgmotorctlr[imotorctlr].fHasActiveEncoder)                                                  \
                    {                                                                                                                               \
                    TouchMotorController(rgmotorctlr[imotorctlr]);                                                                                  \
                    }                                                                                                                               \
                }                                                                                                                                   \
            ReleaseDaisy();                                                                                                                         \
            msLastMotorControllerTickle = msNow;                                                                                                    \
            }                                                                                                                                       \
                                                                                                                                                    \
        /* Find out where the motors are right now */                                                                                               \
        ENCOD encLeft, encRight;                                                                                                                    \
        ReadEncoders(encLeft, encRight);                                                                                                            \
                                                                                                                                                    \
        /* A motor is ok if it has no power or if it's moved (or a couple of other things) */                                                       \
        BOOL fLeftOK  = fFirstTime || !motorLeft.fStallCheck  || FMotorNotPoweredStallCheck(motorLeft) || motorLeft.encStallPrev  != encLeft;       \
        BOOL fRightOK = fFirstTime || !motorRight.fStallCheck || FMotorNotPoweredStallCheck(motorRight) || motorRight.encStallPrev != encRight;     \
        const int msAbortStall = 750;                                                                                           \
                                                                                                                                \
        if (fLeftOK)                                                                                                            \
            {                                                                                                                   \
            motorLeft.msLastOkStall = msNow;                                                                                    \
            }                                                                                                                   \
        else if (msNow - motorLeft.msLastOkStall > msAbortStall)                                                                \
            {                                                                                                                   \
            AbortDueToMotorStall(motorLeft, msNow - motorLeft.msLastOkStall);                                                   \
            }                                                                                                                   \
                                                                                                                                \
        if (fRightOK)                                                                                                           \
            {                                                                                                                   \
            motorRight.msLastOkStall = msNow;                                                                                   \
            }                                                                                                                   \
        else if (msNow - motorRight.msLastOkStall > msAbortStall)                                                               \
            {                                                                                                                   \
            AbortDueToMotorStall(motorRight, msNow - motorRight.msLastOkStall);                                                 \
            }                                                                                                                   \
                                                                                                                                \
        motorLeft.encStallPrev  = encLeft;                                                                                      \
        motorRight.encStallPrev = encRight;                                                                                     \
        msLastMotorStallCheck = msNow;                                                                                          \
        }                                                                                                                       \
    }
