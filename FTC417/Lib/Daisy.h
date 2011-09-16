//
// Daisy.h
//
// Support for manipulating the daisy chain of HiTechnic motor and servo controllers.

//---------------------------------------------------------------------------------------
// I2C support
//---------------------------------------------------------------------------------------

I2CBUFFER reqDaisy;
I2CBUFFER repDaisy;

//---------------------------------------------------------------------------------------------------------
// Core controller
//---------------------------------------------------------------------------------------------------------

// Member variables to be found in every controller
#define COMMON_CONTROLLER_DATA  I2CLINK link; ubyte i2cAddr; short jController; BOOL fActive;

typedef struct {
COMMON_CONTROLLER_DATA} CONTROLLER;

// The I2C device address of a controller depends on the placement of the controller in the
// daisy chain. The first device closest to the NXT will have an address of 0x02, next one 0x04,
// then 0x06, and finally 0x08 for the fourth and final device.
#define I2CAddrFromJController(jController)     ((jController) + (jController))

#define InitializeController(fOverallSuccess, controller, link, jController)                        \
    {                                                                                               \
    BOOL fSuccess = true;                                                                           \
    controller.fActive = true;                                                                      \
                                                                                                    \
    /* A bit of parameter checking on the index. It's one-based, not-zero based. */                 \
    if (jController <= 0)                                                                           \
        fSuccess = false;                                                                           \
                                                                                                    \
    /* Make sure we have the right sensor type that we want, but try to avoid (paranoia) */         \
    /* setting the sensor type unnecessarily for fear of inadvertent side effects        */         \
    TSensorTypes type = sensorI2CCustomFast;                                                        \
    if (SensorType[link] != type)                                                                   \
        {                                                                                           \
        SensorType[link] = type;                                                                    \
        /* Exact wait duration is lore and rumor */                                                 \
        wait1Msec(10);                                                                              \
        }                                                                                           \
                                                                                                    \
    /* Initialize the remaining members */                                                          \
    controller.link        = link;                                                                  \
    controller.jController = jController;                                                           \
    controller.i2cAddr     = I2CAddrFromJController(jController);                                   \
                                                                                                    \
    if (fOverallSuccess) fOverallSuccess = fSuccess;                                                \
    }

//---------------------------------------------------------------------------------------------------------
// Motor controller
//---------------------------------------------------------------------------------------------------------

// Define a data type to hold encoder values
typedef long ENCOD;

typedef enumWord {
    // Low order two bits are a 2-bit field that controls how the motor is managed
    MOTORMODE_RUN_WITH_POWER_ONLY = 0,
        MOTORMODE_RUN_WITH_CONSTANT_SPEED = 1,
        MOTORMODE_RUN_TO_POSITION = 2, MOTORMODE_RESET_CURRENT_ENCODER = 3,
        // The remaining defined bits are individual flags
        MOTORMODE_LOCK = 0x04,
        MOTORMODE_REFLECTED = 0x08,
        MOTORMODE_NO_TIME_OUT = 0x10,
        MOTORMODE_ERROR = 0x40, MOTORMODE_BUSY = 0x80,
        // We force this value (using '|') in to all the modes we ever set
        MOTORMODE_FORCE = 0,    // ie: nothing: we rely on the fact that we're checking for stalls often enough that this isn't an issue
} MOTORMODE;

// Each motor controller keeps some state about each of its (possibly two) attached motors
typedef struct {
    MOTORMODE mode;             // the current mode in which this motor operates
    short power;                // the amount of power assigned there to. see HiTechnic_Motor_Controller_Brief_v1.3.pdf for details
    ENCOD encoder;              // the value of the encoder of this motor as of the last time we read it
    ENCOD encoderRawPrev;       // used only for lego motors
    ENCOD encoderTarget;
    int gearRatio;
    int pidKp;
    int pidKi;
    int pidKd;
    BOOL fActive;
    BOOL fForwardWhenLastPowered;
} MTR;

// The definition of the motor controller data structure
typedef struct {
    COMMON_CONTROLLER_DATA      // state for both motor and servo controllers
    MTR rgmtr[3];               // state we keep about our motors. Tetrix controller have a max of 2 motors; the Lego controller can have three
    int imotorctlr;             // our index into rgmotorctlr
    BOOL fHasActiveEncoder;     // does this controller have a mounted motor with an encoder?
    BOOL fDirtyPower;           // have we got power levels this fellow (probably) hasn't seen?
    BOOL fLegoController;       // is this the pseudo controller for the lego motors?
} MOTORCONTROLLER;

// Declaration of the motor controller variables. Up to four motor controllers and servo
// controllers may be attached in a daisy chain, in any combination, so a chain can have
// at most four controllers, which define here. NOTE: it is perhaps the case (though
// unverified) that one can have daisy chains on more than one of the sensor ports; that
// scenario (if more than four motor contollers were used) would require that this array
// be increased in size.
#define imotorctlrMax 5
int imotorctlrMac = 0;
MOTORCONTROLLER rgmotorctlr[imotorctlrMax];

#define imotorctlrLego      (imotorctlrMax-1)
#define legoController      rgmotorctlr[imotorctlrLego] // a pseudo controller: drives Lego motors, not Tetrix motors. Treated specially in code.

I2CBUFFER reqSpecial;

#define TraceControllerMode(controller)                                                 \
    {                                                                                   \
    FormatI2CReq(reqDaisy, controller.i2cAddr, 0x44);                                   \
    i2cSendReceiveDaisy(controller.link, reqDaisy, repDaisy, 4);                        \
    TRACE(("%d: %02x %02x %02x %02x", controller.i2cAddr, (int)repDaisy.rgb[0], (int)repDaisy.rgb[1], (int)repDaisy.rgb[2], (int)repDaisy.rgb[3])); \
    }

#define TraceControllerModes(message)       \
    {                                       \
    LockDaisy();                            \
    TRACE(("%s", message));                 \
    TraceControllerMode(rgmotorctlr[0]);    \
    TraceControllerMode(rgmotorctlr[1]);    \
    ReleaseDaisy();                         \
    }

#define SendLegoMotorMode(controller, imtr)                             \
    {                                                                   \
    if (controller.rgmtr[imtr].fActive)                                 \
        {                                                               \
        int mode = (controller.rgmtr[imtr].mode | MOTORMODE_FORCE);     \
                                                                        \
        bMotorReflected[imtr] = !!(mode & MOTORMODE_REFLECTED);         \
                                                                        \
        switch (mode & 0x03)                                            \
            {                                                           \
        case MOTORMODE_RUN_WITH_CONSTANT_SPEED:                         \
            nMotorPIDSpeedCtrl[imtr] = mtrSpeedReg;                     \
            break;                                                      \
        case MOTORMODE_RUN_WITH_POWER_ONLY:                             \
            nMotorPIDSpeedCtrl[imtr] = mtrNoReg;                        \
            break;                                                      \
        case MOTORMODE_RESET_CURRENT_ENCODER:                           \
            nMotorEncoder[imtr] = 0;                                    \
            controller.rgmtr[imtr].encoder = 0;                         \
            controller.rgmtr[imtr].encoderRawPrev = 0;                  \
            break;                                                      \
        case MOTORMODE_RUN_TO_POSITION:                                 \
            break;                                                      \
            }                                                           \
        }                                                               \
    }

// Update the target encoder settings, the motor modes, and the motor power levels in this controller
void
SendMotorTargetModePower(OUT BOOL & fSuccess, MOTORCONTROLLER & controller) {
    fSuccess = controller.fActive;
    if (fSuccess) {
        if (controller.fLegoController) {
            if (controller.rgmtr[0].fActive)
                nMotorEncoderTarget[0] = controller.rgmtr[0].encoderTarget;
            if (controller.rgmtr[1].fActive)
                nMotorEncoderTarget[1] = controller.rgmtr[1].encoderTarget;
            if (controller.rgmtr[2].fActive)
                nMotorEncoderTarget[2] = controller.rgmtr[2].encoderTarget;
            //
            SendLegoMotorMode(controller, 0);
            SendLegoMotorMode(controller, 1);
            SendLegoMotorMode(controller, 2);
            //
            if (controller.rgmtr[0].fActive)
                motor[0] = controller.rgmtr[0].power;
            if (controller.rgmtr[1].fActive)
                motor[1] = controller.rgmtr[1].power;
            if (controller.rgmtr[2].fActive)
                motor[2] = controller.rgmtr[2].power;
        } else {
            FormatI2CPrim(reqSpecial, controller.i2cAddr, 0x40, 12);
            PackLong(reqSpecial.rgb, IbI2CPayload(0),
                controller.rgmtr[0].encoderTarget);
            reqSpecial.rgb[IbI2CPayload(4)] =
                controller.rgmtr[0].mode | MOTORMODE_FORCE;
            reqSpecial.rgb[IbI2CPayload(5)] = controller.rgmtr[0].power;
            reqSpecial.rgb[IbI2CPayload(6)] = controller.rgmtr[1].power;
            reqSpecial.rgb[IbI2CPayload(7)] =
                controller.rgmtr[1].mode | MOTORMODE_FORCE;
            PackLong(reqSpecial.rgb, IbI2CPayload(8),
                controller.rgmtr[1].encoderTarget);
            fSuccess = i2cSendDaisy(controller.link, reqSpecial, 0);
        }
        if (fSuccess) {
            controller.fDirtyPower = false;
        }
    }
}

// Update the motor modes and the motor power levels in this controller
void
SendMotorModePower(OUT BOOL & fSuccess, MOTORCONTROLLER & controller) {
    fSuccess = controller.fActive;
    if (fSuccess) {
        if (controller.fLegoController) {
            SendLegoMotorMode(controller, 0);
            SendLegoMotorMode(controller, 1);
            SendLegoMotorMode(controller, 2);
            //
            if (controller.rgmtr[0].fActive)
                motor[0] = controller.rgmtr[0].power;
            if (controller.rgmtr[1].fActive)
                motor[1] = controller.rgmtr[1].power;
            if (controller.rgmtr[2].fActive)
                motor[2] = controller.rgmtr[2].power;
        } else {
            FormatI2CPrim(reqDaisy, controller.i2cAddr, 0x44, 4);
            reqDaisy.rgb[IbI2CPayload(0)] =
                controller.rgmtr[0].mode | MOTORMODE_FORCE;
            reqDaisy.rgb[IbI2CPayload(1)] = controller.rgmtr[0].power;
            reqDaisy.rgb[IbI2CPayload(2)] = controller.rgmtr[1].power;
            reqDaisy.rgb[IbI2CPayload(3)] =
                controller.rgmtr[1].mode | MOTORMODE_FORCE;
            fSuccess = i2cSendDaisy(controller.link, reqDaisy, 0);
        }
        if (fSuccess) {
            controller.fDirtyPower = false;
        }
    }
}

// Update the motor power levels in this controller, but only if
// we know we've got something dirty there.
void
SendMotorPower(OUT BOOL & fSuccess, MOTORCONTROLLER & controller) {
    fSuccess = controller.fActive;
    if (fSuccess && controller.fDirtyPower) {
        if (controller.fLegoController) {
            if (controller.rgmtr[0].fActive)
                motor[0] = controller.rgmtr[0].power;
            if (controller.rgmtr[1].fActive)
                motor[1] = controller.rgmtr[1].power;
            if (controller.rgmtr[2].fActive)
                motor[2] = controller.rgmtr[2].power;
        } else {
            FormatI2CPrim(reqDaisy, controller.i2cAddr, 0x45, 2);
            reqDaisy.rgb[IbI2CPayload(0)] = controller.rgmtr[0].power;
            reqDaisy.rgb[IbI2CPayload(1)] = controller.rgmtr[1].power;
            fSuccess = i2cSendDaisy(controller.link, reqDaisy, 0);
        }
        if (fSuccess) {
            controller.fDirtyPower = false;
        }
    }
}

// Remember the motor power along whether it has changed (and thus
// needs to be sent to the controller at our earliest convenience).
#define RecordMotorPower(imotorctlr, imtr, powerArg)                                        \
    {                                                                                       \
    if (rgmotorctlr[imotorctlr].rgmtr[imtr].power != (powerArg))                            \
        {                                                                                   \
        rgmotorctlr[imotorctlr].fDirtyPower = true;                                         \
        rgmotorctlr[imotorctlr].rgmtr[imtr].power = (powerArg);                             \
        if ((powerArg) != 0)                                                                \
            {                                                                               \
            rgmotorctlr[imotorctlr].rgmtr[imtr].fForwardWhenLastPowered = (powerArg) > 0;   \
            }                                                                               \
        }                                                                                   \
    }

// Publically readable value of our external battery
float vExternalBattery = 0.0;

void
ReadLegoEncoder(MOTORCONTROLLER & controller, int imtr) {
    /* We mult lego encoders values by 4 so all encoders (Tetrix & Lego) are in 1440 ticks per rev */
    if (controller.rgmtr[imtr].fActive) {
        /* Read the lego motor encoder. Its value may have wrapped, and what we do to
         * deal with that (ie: to unwrap the value) depends on whether we were going
         * forward (and thus expecting the encoder value to increase) or backward,
         * (expecting it to decrease). */
        int32 encoderRaw = (long)nMotorEncoder[imtr] * 4;

        const int32 encoderWrap = (int32) 65536 * 4;
        const int32 encoderWrapThresholdDetect = (int32) 16384 * 4;

        /* Did the encoder wrap? This is detected as the case where the
         * signs of the current and previous values differ but they're
         * not small enough to have actually crossed over zero. */

        BOOL fWrapped = false;
        if (Sign(encoderRaw) != Sign(controller.rgmtr[imtr].encoderRawPrev)) {
            if (encoderRaw > encoderWrapThresholdDetect
                || controller.rgmtr[imtr].encoderRawPrev >
                encoderWrapThresholdDetect) {
                fWrapped = true;
            }
        }

        /* Update the accumulated unwrapped encoder value. A persnicity issue
         * is that the encoders sometimes continue to move even after power has
         * been shut off, but the SetMotorPower function takes care to remember
         * which direction the motors were going, even if they're off now. */

        controller.rgmtr[imtr].encoder +=
            (encoderRaw - controller.rgmtr[imtr].encoderRawPrev);

        if (fWrapped) {
            if (controller.rgmtr[imtr].fForwardWhenLastPowered)
                controller.rgmtr[imtr].encoder += encoderWrap;
            else
                controller.rgmtr[imtr].encoder -= encoderWrap;
        }

        /* Remember state for the next call */
        controller.rgmtr[imtr].encoderRawPrev = encoderRaw;
    }
}

// Read the current values of this controller's motor's encoders. Because it's
// cheap and convenient, at this moment we also read the battery voltage.
#define ReadControllerEncoders(fSuccess, controller)                                        \
    {                                                                                       \
    fSuccess = controller.fActive;                                                          \
    if (fSuccess)                                                                           \
        {                                                                                   \
        if (controller.fLegoController)                                                     \
            {                                                                               \
            ReadLegoEncoder(controller,0);                                                  \
            ReadLegoEncoder(controller,1);                                                  \
            ReadLegoEncoder(controller,2);                                                  \
            }                                                                               \
        else                                                                                \
            {                                                                               \
            FormatI2CReq(reqDaisy, controller.i2cAddr, 0x4c);                               \
            fSuccess = i2cSendReceiveDaisy(controller.link, reqDaisy, repDaisy, 10);        \
            if (fSuccess)                                                                   \
                {                                                                           \
                controller.rgmtr[0].encoder = UnpackLong(repDaisy.rgb, 0);                  \
                controller.rgmtr[1].encoder = UnpackLong(repDaisy.rgb, 4);                  \
                /* The battery value provided is in units of 20mv */                        \
                vExternalBattery = (float)Unpack10BitAnalog(repDaisy.rgb, 8) * 0.020;       \
                }                                                                           \
            }                                                                               \
        }                                                                                   \
    }

void
ReadControllerPid(OUT BOOL & fSuccess, MOTORCONTROLLER & controller) {
    fSuccess = controller.fActive;
    if (fSuccess) {
        if (controller.fLegoController) {
            // REVIEW: is there anything reasonable to do?
        } else {
            FormatI2CReq(reqDaisy, controller.i2cAddr, 0x56);
            fSuccess =
                i2cSendReceiveDaisy(controller.link, reqDaisy, repDaisy, 8);
            if (fSuccess) {
                controller.rgmtr[0].gearRatio = repDaisy.rgb[0];
                controller.rgmtr[0].pidKp = repDaisy.rgb[1];
                controller.rgmtr[0].pidKi = repDaisy.rgb[2];
                controller.rgmtr[0].pidKd = repDaisy.rgb[3];
                controller.rgmtr[1].gearRatio = repDaisy.rgb[4];
                controller.rgmtr[1].pidKp = repDaisy.rgb[5];
                controller.rgmtr[1].pidKi = repDaisy.rgb[6];
                controller.rgmtr[1].pidKd = repDaisy.rgb[7];
            }
        }
    }
}

// Communicate with this motor controller in such a way that it doesn't time out
#define TouchMotorController(controller)                                            \
    {                                                                               \
    if (controller.fLegoController)                                                 \
        {                                                                           \
        }                                                                           \
    else                                                                            \
        {                                                                           \
        FormatI2CReq(reqDaisy, controller.i2cAddr, 0x44);                           \
        i2cSendReceiveDaisy(controller.link, reqDaisy, repDaisy, 1);                \
        }                                                                           \
    }

void
SendControllerPid(OUT BOOL & fSuccess, MOTORCONTROLLER & controller) {
    fSuccess = controller.fActive;
    if (fSuccess) {
        if (controller.fLegoController) {
            // REVIEW: can we do anything here?
        } else {
            FormatI2CPrim(reqDaisy, controller.i2cAddr, 0x56, 8)
                reqDaisy.rgb[IbI2CPayload(0)] = controller.rgmtr[0].gearRatio;
            reqDaisy.rgb[IbI2CPayload(1)] = controller.rgmtr[0].pidKp;
            reqDaisy.rgb[IbI2CPayload(2)] = controller.rgmtr[0].pidKi;
            reqDaisy.rgb[IbI2CPayload(3)] = controller.rgmtr[0].pidKd;
            reqDaisy.rgb[IbI2CPayload(4)] = controller.rgmtr[1].gearRatio;
            reqDaisy.rgb[IbI2CPayload(5)] = controller.rgmtr[1].pidKp;
            reqDaisy.rgb[IbI2CPayload(6)] = controller.rgmtr[1].pidKi;
            reqDaisy.rgb[IbI2CPayload(7)] = controller.rgmtr[1].pidKd;
            fSuccess = i2cSendDaisy(controller.link, reqDaisy, 0);
        }
    }
}

// Update the lower two bits (the run mode selector) of this motor
#define SetControllerMtrRunMode(modePrev, controller, imtr, sel)                    \
    {                                                                               \
    modePrev = controller.rgmtr[imtr].mode;                                         \
    controller.rgmtr[imtr].mode = ((modePrev & ~0x03) | ((sel) & 0x03));            \
    }

// From what we understand, the motor controller only updates its state on a tick
// of an internal clock which ticks every n ms (the value of n is not clear). Thus, if you
// send a command (like a mode change) to the controller, you need to guarantee that a
// tick occurs in order to ensure it's effect before doing an action (like another mode
// change) that will turn that off.
//
//
#define waitForMotorControllerCommandCycle(controller)    { if (!controller.fLegoController) wait1Msec(51); }   // 26 is too low (sometimes!), 51 seems reliable
#define waitForMotorControllerCommandCycle2(controllerA, controllerB)   \
    {                                                                   \
    if (!controllerA.fLegoController || !controllerB.fLegoController)   \
        {                                                               \
        wait1Msec(51);                                                  \
        }                                                               \
    }

#define InitializeLegoControllerIfNecessary()                       \
    {                                                               \
    if (!legoController.fActive)                                    \
        {                                                           \
        legoController.fLegoController   = true;                    \
        legoController.fHasActiveEncoder = true;                    \
        legoController.imotorctlr        = imotorctlrLego;          \
        legoController.fActive           = true;                    \
        }                                                           \
    }

void
InitializeMotorController(IN OUT STICKYFAILURE & fOverallSuccess,
    MOTORCONTROLLER & controller, I2CLINK link,
    short jController /*one-based */ ) {
    BOOL fSuccess = true;
    LockDaisy();

    // Figure out which controller index we are
    int i = 0;
    controller.imotorctlr = -1;
    while (rgmotorctlr[i].imotorctlr >= 0)
        i++;
    controller.imotorctlr = i;

    if (imotorctlrLego == controller.imotorctlr) {
        InitializeLegoControllerIfNecessary();
    } else {
        // Do core level controller initialization.
        InitializeController(fSuccess, controller, link, jController);
        controller.fLegoController = false;

        // Remember (one more than) the last value we see
        if (imotorctlrMac <= i)
            imotorctlrMac = i + 1;
    }

    // Read the pid values
    if (fSuccess) {
        ReadControllerPid(fSuccess, controller);
    }
    // Initialize the controller state to known values
    controller.fHasActiveEncoder = false;

    controller.rgmtr[0].mode = MOTORMODE_RESET_CURRENT_ENCODER;
    controller.rgmtr[1].mode = MOTORMODE_RESET_CURRENT_ENCODER;
    controller.rgmtr[2].mode = MOTORMODE_RESET_CURRENT_ENCODER;
    controller.rgmtr[0].power = 0;
    controller.rgmtr[1].power = 0;
    controller.rgmtr[2].power = 0;
    controller.rgmtr[0].encoder = 0;
    controller.rgmtr[1].encoder = 0;
    controller.rgmtr[2].encoder = 0;
    controller.rgmtr[0].encoderRawPrev = 0;
    controller.rgmtr[1].encoderRawPrev = 0;
    controller.rgmtr[2].encoderRawPrev = 0;
    controller.rgmtr[0].encoderTarget = 0;
    controller.rgmtr[1].encoderTarget = 0;
    controller.rgmtr[2].encoderTarget = 0;
    controller.rgmtr[0].fActive = false;
    controller.rgmtr[1].fActive = false;
    controller.rgmtr[2].fActive = false;
    controller.rgmtr[0].fForwardWhenLastPowered = true;
    controller.rgmtr[1].fForwardWhenLastPowered = true;
    controller.rgmtr[2].fForwardWhenLastPowered = true;

    controller.fDirtyPower = false;

    // Propagate the state to the controller
    if (fSuccess) {
        SendMotorTargetModePower(fSuccess, controller);
        waitForMotorControllerCommandCycle(controller);
    }
    // Having done that, we change the mode to something reasonable
    if (fSuccess) {
        MOTORMODE modeHelper;
        SetControllerMtrRunMode(modeHelper, controller, 0,
            MOTORMODE_RUN_WITH_CONSTANT_SPEED);
        SetControllerMtrRunMode(modeHelper, controller, 1,
            MOTORMODE_RUN_WITH_CONSTANT_SPEED);
        SetControllerMtrRunMode(modeHelper, controller, 2,
            MOTORMODE_RUN_WITH_CONSTANT_SPEED);
        SendMotorModePower(fSuccess, controller);
        waitForMotorControllerCommandCycle(controller);
    }

    ReleaseDaisy();
    if (fOverallSuccess)
        fOverallSuccess = fSuccess;
    TraceInitializationResult1("mtrcnt(%d)", jController, fOverallSuccess);
}

//---------------------------------------------------------------------------------------------------------
// Servo controller
//---------------------------------------------------------------------------------------------------------

#define isvcMax   6             // max number of servos per HiTechnic servo controller

typedef struct {
    int svpos;                  // current commanded position of this servo
} SVC;

typedef enumWord {
    SVOPWM_ENABLE = 0x00,       // servos powered, but with a 10-second timeout
        SVOPWM_ENABLE_NO_TIMEOUT = 0xAA,        // servos powered, but no timeout
        SVOPWM_DISABLE = 0xFF,  // servos not powered
} SVOPWM;

typedef struct {
    COMMON_CONTROLLER_DATA      // state common between motor and servo controllers
    int isvoctlr;               // the index of this servo controller in rgsvoctlr
    BOOL fDirty;                // whether we have state here that we haven't yet told the servo controller
    BOOL fJustPowered;
    SVOPWM pwmMode;             // current power mode of the servo controller
    MILLI msLastSend;           // instant at which we last did an I2C transmission to the controller
    SVC rgsvc[isvcMax];         // state of all the servos on this controller
} SVOCTLR;

#define isvoctlrMax 4           // Maximum number of HiTechnic servo controllers in the daisy chain
SVOCTLR rgsvoctlr[isvoctlrMax];

#define RecordSvoPosition(isvoctlr, isvc, svposNew)             \
    {                                                           \
    rgsvoctlr[isvoctlr].rgsvc[isvc].svpos = svposNew;           \
    rgsvoctlr[isvoctlr].fDirty = true;                          \
    }

// To quote from the servo controller documentation and from experience:
//
// The Step time field sets the step time for the servo channel which has the furthest to move. Other
// servo channels which are not at their designated positions yet will run at a slower rate to ensure
// they reach their destination positions at the same time. This value may range from 1 – 15. If the
// field is set to 0, no timing s applied (ie; the servos move at their maximum rate) and the
// QueryServoMovementComplete() request no longer returns sensical information.
//
// That all said, the servos themselves don't electrically actually have any means by which
// they can report their position. So, the servo controller is just guessing as to when the
// servos will be done. We can guess just as well, even better, what with our knowledge of
// apogee, calibration step, and so on. So we don't use the servo controller waits by default.
//
#ifndef USE_SERVO_CONTROLLER_WAITS
#define USE_SERVO_CONTROLLER_WAITS  0
#endif

#if USE_SERVO_CONTROLLER_WAITS
#define SERVOCONTROLLER_STEP_TIME   1
#else
#define SERVOCONTROLLER_STEP_TIME   0
#endif

#define DisableServoController(fSuccess, controller)        \
    {                                                       \
    controller.pwmMode = SVOPWM_DISABLE;                    \
    SendServoPwmMode(fSuccess, controller);                 \
    }

#define SendServoPwmMode(fSuccess, controller)                  \
    {                                                           \
    fSuccess = controller.fActive;                              \
    if (fSuccess)                                               \
        {                                                       \
        FormatI2CPrim(reqDaisy, controller.i2cAddr, 0x48, 1);   \
        reqDaisy.rgb[IbI2CPayload(0)] = controller.pwmMode;     \
                                                                \
        fSuccess = i2cSendDaisy(controller.link, reqDaisy, 0);  \
        if (fSuccess)                                           \
            {                                                   \
            controller.msLastSend = nSysTime;                   \
            }                                                   \
        }                                                       \
    }

// If the controller doesn't yet know about any of the requested servo positions, then tell it.
void
SendServoPositions(OUT BOOL & fSuccess, SVOCTLR & controller, BOOL fArm = true) {
    fSuccess = controller.fActive;
    if (fSuccess && controller.fDirty) {
        if (fArm && controller.pwmMode == SVOPWM_DISABLE) {
            controller.pwmMode = SVOPWM_ENABLE;
            controller.fJustPowered = false;
        }

        FormatI2CPrim(reqDaisy, controller.i2cAddr, 0x41, 1 + isvcMax + 1);
        reqDaisy.rgb[IbI2CPayload(0)] = SERVOCONTROLLER_STEP_TIME;
        reqDaisy.rgb[IbI2CPayload(1)] = controller.rgsvc[0].svpos;
        reqDaisy.rgb[IbI2CPayload(2)] = controller.rgsvc[1].svpos;
        reqDaisy.rgb[IbI2CPayload(3)] = controller.rgsvc[2].svpos;
        reqDaisy.rgb[IbI2CPayload(4)] = controller.rgsvc[3].svpos;
        reqDaisy.rgb[IbI2CPayload(5)] = controller.rgsvc[4].svpos;
        reqDaisy.rgb[IbI2CPayload(6)] = controller.rgsvc[5].svpos;
        reqDaisy.rgb[IbI2CPayload(7)] = controller.pwmMode;
        fSuccess = i2cSendDaisy(controller.link, reqDaisy, 0);
        if (fSuccess) {
            controller.fDirty = false;
            controller.msLastSend = nSysTime;
        }
    }
}

// Have the servos on this controller stopped moving?
void
QueryServoMovementComplete(OUT BOOL & fSuccess, SVOCTLR & controller,
    OUT BOOL & fComplete) {
    fSuccess = controller.fActive;
    if (fSuccess) {
        FormatI2CReq(reqDaisy, controller.i2cAddr, 0x40);
        fSuccess = i2cSendReceiveDaisy(controller.link, reqDaisy, repDaisy, 1);
        if (fSuccess) {
            controller.msLastSend = nSysTime;
        }
    }
    fComplete = fSuccess && (repDaisy.rgb[0] == 0);
}

int32 msTickleServosPrev = 0;

// Touch each of the servo controllers so they don't time out and power themselves off. Call this every
// so often (quite often is fine).
#define TickleServos(msNow)                                                                                 \
    {                                                                                                       \
    /* HiTechnic servo controllers have 10 sec time out. So we use a threshold close to that */             \
    /* But not too close. */                                                                                \
    const MILLI msThreshold = 6000;                                                                         \
    if (0 == msTickleServosPrev || msNow - msTickleServosPrev > 1000)                                       \
        {                                                                                                   \
        LockDaisy();                                                                          \
        for (int isvoctlr = 0; isvoctlr < isvoctlrMax; isvoctlr++)                                          \
            {                                                                                               \
            if (rgsvoctlr[isvoctlr].fActive)                                                                \
                {                                                                                           \
                if (msNow - rgsvoctlr[isvoctlr].msLastSend > msThreshold)                                   \
                    {                                                                                       \
                    BOOL fSuccess = true;                                                                   \
                    BOOL fComplete;                                                                         \
                    QueryServoMovementComplete(fSuccess, rgsvoctlr[isvoctlr], fComplete);                   \
                    }                                                                                       \
                }                                                                                           \
            }                                                                                               \
        ReleaseDaisy();                                                                       \
        }                                                                                                   \
    }

void
InitializeServoController(IN OUT STICKYFAILURE & fOverallSuccess,
    SVOCTLR & controller, I2CLINK link, short jController) {
    BOOL fSuccess = true;
    LockDaisy();

    // Core level initialization
    InitializeController(fSuccess, controller, link, jController);

    // Where is this controller in rgsvoctlr?
    int i = 0;
    controller.isvoctlr = -1;
    while (rgsvoctlr[i].isvoctlr >= 0)
        i++;
    controller.isvoctlr = i;

    if (fSuccess) {
        // Figure out where servos are as best we can in order to avoid
        // inadvertently moving them as much as possible. Also find out
        // what the current pwm mode is of the controller.
        FormatI2CReq(reqDaisy, controller.i2cAddr, 0x42);
        fSuccess =
            i2cSendReceiveDaisy(controller.link, reqDaisy, repDaisy,
            isvcMax + 1);
        if (fSuccess) {
            // Upon a power cycle (of the 12v power?) servo positions are all reported
            // as 128 and the controller mode is disabled. Detect that condition here.
            controller.fJustPowered = true;

            controller.msLastSend = nSysTime;
            for (int isvc = 0; isvc < isvcMax; isvc++) {
                int svpos = repDaisy.rgb[isvc];
                controller.rgsvc[isvc].svpos = svpos;
                controller.fJustPowered = controller.fJustPowered
                    && (128 == svpos);
                TRACE(("%d", svpos));
            }
            controller.pwmMode = repDaisy.rgb[isvcMax];
            controller.fJustPowered = controller.fJustPowered
                && (SVOPWM_DISABLE == controller.pwmMode);
            TRACE(("pwmMode=0x%02x pwrCycle=%d", controller.pwmMode,
                    controller.fJustPowered));
        } else {
            for (int isvc = 0; isvc < isvcMax; isvc++) {
                controller.rgsvc[isvc].svpos = 128;     // 128 is the default used by the servo controller. Makes sense, esp. ack'ing that servos are sometimes paired.
            }
            controller.pwmMode = SVOPWM_DISABLE;
            controller.fJustPowered = true;
        }
    }
    // Initialize remining servo controller state
    controller.fDirty = false;
    ReleaseDaisy();

    if (fOverallSuccess)
        fOverallSuccess = fSuccess;
    TraceInitializationResult1("svocnt(%d)", jController, fOverallSuccess);
}
