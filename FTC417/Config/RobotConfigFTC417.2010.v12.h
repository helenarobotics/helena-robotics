//
// RobotV12.h
//
// Static configuration information about the robot: its geometry and sensors.

//-------------------------------------------------------------------------------------------------------
// Servo calibration
//-------------------------------------------------------------------------------------------------------

// Pair of corresponding measured values. Best if measured close to
// the apogee, but that's not entirely critical
#define svposShoulderCalibrate  232
#define degShoulderCalibrate     90     // degrees where zero is shoulder flat, 90 is shoulder vertical, >90 is shoulder snugged into packed

// Put the elbow servo at svpos zero, then measure the angle
// of the arm linkage that's attached thereto. Usually it's slighly
// *above* the horizontal; here that is recorded a *negative* angle.
#define degElbowCalibrate      (-5)   // as measured, 2011.04.25

// The apogee is nice to know, but really only effects servo movement rate estimatoin
#define svposShoulderApogee     215

//-------------------------------------------------------------------------------------------------------
// Constants regarding the wheels and motors
//-------------------------------------------------------------------------------------------------------

#define CM_BETWEEN_WHEELS               ((38.3 + 30.) * 0.5)
#define CM_WHEEL_CIRCUMFERENCE          (3.0 * 2.54 * PI)       // they're 3" wheels, even though they are treads
#define MOTOR_GEARING                   1.5                     // the mechanical advantage from motor to wheel

//-------------------------------------------------------------------------------------------------------
// Motors
//-------------------------------------------------------------------------------------------------------

// This bot has *two* motor controllers, which control six real motors that
// appear to us in software as four logical ones.

#define motorController     rgmotorctlr[0]
#define motorControllerAux  rgmotorctlr[1]
#define motorLeft           rgmotor[0]
#define motorRight          rgmotor[1]
#define motorLeftAux        rgmotor[2]
#define motorRightAux       rgmotor[3]

void InitializeMotors(IN OUT STICKYFAILURE& fOverallSuccess)
    {
    // This bot has two motor controllers, which are first and second in the daisy chain on sensor connector #1
    InitializeMotorController(fOverallSuccess, motorController,    I2CLINK_1, 1);
    InitializeMotorController(fOverallSuccess, motorControllerAux, I2CLINK_1, 2);

    // This bot has six physical motors:
    //
    //      one on each channel of motorController
    //      two on each channel of motorControllerAux, each two wired in parallel on their respective channels
    //
    // These appear to us as four *logical* motors: we can't 'see' that there are two motors on
    // each channel of motorControllerAux. Given how the chain drive works, we need the
    // motors on motorControllerAux to run in the reverse direction from the corresponding
    // motors on motorController.
    //
    // We only ever look at the encoders on motorController, since it's the one with both motorLeft and motorRight.

    InitializeMotor(fOverallSuccess, motorLeft,  motorController, 1, "left",  MOTORPROP_ENCODER | MOTORPROP_NONE     );
    InitializeMotor(fOverallSuccess, motorRight, motorController, 2, "right", MOTORPROP_ENCODER | MOTORPROP_REFLECTED);

    InitializeMotor(fOverallSuccess, motorLeftAux,  motorControllerAux, 1, "leftAux",  MOTORPROP_REFLECTED     );
    InitializeMotor(fOverallSuccess, motorRightAux, motorControllerAux, 2, "rightAux", MOTORPROP_NONE          );

    motorLeft.imotorPaired  = motorLeftAux.imotor;
    motorRight.imotorPaired = motorRightAux.imotor;
    }

//-------------------------------------------------------------------------------------------------------
// Definition and initialization of sensors
//-------------------------------------------------------------------------------------------------------

// One MUST #define macros like this for each of the sensor one has as elsewhere
// in the code conditional compilation tests for their presence or absence. Morover,
// one MUST use the stock set of macro names. See main.h for further details.

#define sensnmSonicLeft      SensorOnMux(4,1)
#define sensnmSonicBack      SensorOnMux(4,2)
#define sensnmEopdFront      SensorOnMux(4,3)
#define sensnmSonicFront     SensorOnMux(4,4)

#define sensnmEopdFrontRight SensorOnMux(3,1)
#define sensnmColor          SensorOnMux(3,2)
#define sensnmGyroHorz       SensorOnMux(3,3)
#define sensnmSonicRight     SensorOnMux(3,4)

#define sensnmMagRotor       SensorOnMux(2,1)
#define sensnmAngleRotor     SensorOnMux(2,2)
#define sensnmEopdArm        SensorOnMux(2,3)
#define sensnmMagRotorAux    SensorOnMux(2,4)

SONICSENSOR                  sensSonicFront;
SONICSENSOR                  sensSonicBack;
SONICSENSOR                  sensSonicLeft;
SONICSENSOR                  sensSonicRight;
EOPDSENSOR                   sensEopdFront;
GYROSENSOR                   sensGyroHorz;
COLORSENSOR                  sensColor;
EOPDSENSOR                   sensEopdFrontRight;
EOPDSENSOR                   sensEopdArm;
ANGLESENSOR                  sensAngleRotor;
MAGNETICSENSOR               sensMagRotor;
MAGNETICSENSOR               sensMagRotorAux;

#define AngleSensorFromIndex(isensangle,member)    (sensAngleRotor.member)

void InitializeSensors(IN OUT STICKYFAILURE& fOverallSuccess)
    {
    InitializeSonicSensor(fOverallSuccess, "left",  sensSonicLeft,      sensnmSonicLeft);
    InitializeSonicSensor(fOverallSuccess, "right", sensSonicRight,     sensnmSonicRight);
    InitializeSonicSensor(fOverallSuccess, "front", sensSonicFront,     sensnmSonicFront);
    InitializeSonicSensor(fOverallSuccess, "back",  sensSonicBack,      sensnmSonicBack);
    InitializeEopdSensor (fOverallSuccess, "epodF", sensEopdFront,      sensnmEopdFront,        EOPD_RAW,       EOPD_LONG);
    InitializeGyroSensor (fOverallSuccess, "gyro",  sensGyroHorz,       sensnmGyroHorz);
    InitializeColorSensor(fOverallSuccess, "color", sensColor,          sensnmColor);
    InitializeEopdSensor (fOverallSuccess, "eopdA", sensEopdArm,        sensnmEopdArm,          EOPD_RAW,       EOPD_SHORT);
    InitializeEopdSensor (fOverallSuccess, "eopdFR",sensEopdFrontRight, sensnmEopdFrontRight,   EOPD_RAW,       EOPD_LONG);
    InitializeAngleSensor(fOverallSuccess, "angle", sensAngleRotor,     sensnmAngleRotor);
    InitializeMagneticSensor(fOverallSuccess, "mag",    sensMagRotor,   sensnmMagRotor);
    InitializeMagneticSensor(fOverallSuccess, "magAux", sensMagRotorAux,sensnmMagRotorAux);

    // Set the angle sensor gearing. The gear attached to the servo has 40 teeth,
    // and the one attached to the angle sensor has 36 teeth. Thus, the angle sensor
    // rotates TOO FAST. Set to a value less than one to compensate.
    //
    // Further, we wish to reverse the direction of the sensor, so that positive values
    // correspond with increasing duration in which the rotor has been operating in its
    // normal unload dispenser / score batons rotation. So we negate the gearing constant.
    sensAngleRotor.gearing = -36.0 / 40.0;

    // Our two magnetic sensors have an associated angle sensor
    sensMagRotor.isensangleAssociated = 0;
    sensMagRotorAux.isensangleAssociated = 0;

    // Our gyro sensor has been observed to overread a bit: e.g.: what the gyro
    // thinks is 360.0 degrees is in fact a bit less. So we need to scale back the readings.
    const float perDegError = 0.025104158;
    sensGyroHorz.degCWPerSScale = 1.0 / (1.0 + perDegError);

    TraceInitializationResult("sensors", fOverallSuccess);
    }

// color sensor is effectively this far inboard from the front EOPD
#define dcmEopdFrontColor       18      // WRONG: not actually yet measured

//-------------------------------------------------------------------------------------------------------
// Servos
//-------------------------------------------------------------------------------------------------------

#define degShoulderServoRange   198

#define HAS_ARM_SERVOS     1
#define HAS_WRIST_SERVOS   1
#define HAS_PRELOAD_SERVOS 0
#define HAS_SERVO_ROTOR    1

#define svoArmRotation      rgsvo[0]
#define svoArmShoulder      rgsvo[1]
#define svoArmElbow         rgsvo[2]
#define svoArmWrist         rgsvo[3]
#define svoRotor            rgsvo[4]

#define servoController3        rgsvoctlr[0]
#define servoController4        rgsvoctlr[1]
#define servoControllerDummy    servoController3

#define dsvposShoulder      0
#define dsvposElbow         0

#define SdrAdj(svpos)       ((svpos) + dsvposShoulder)   // WRONG
#define ElbAdj(svpos)       ((svpos) + dsvposElbow)      // WRONG

void InitializeServos(IN OUT STICKYFAILURE& fOverallSuccess)
    {
    /* Our two servo controllers are #3 and #4 in the daisy chain (the motor controllers come first) */
    InitializeServoController(fOverallSuccess, servoController3, I2CLINK_1, 3);
    InitializeServoController(fOverallSuccess, servoController4, I2CLINK_1, 4);
    /**/
    InitializeServo(fOverallSuccess, svoArmRotation,     "rot",   servoController3,1,  servoController3,2,      SVOKIND_STANDARD, HAS_ARM_SERVOS);
    InitializeServo(fOverallSuccess, svoArmShoulder,     "sdr",   servoController4,2,  servoController4,1,      SVOKIND_STANDARD, HAS_ARM_SERVOS);
    InitializeServo(fOverallSuccess, svoArmElbow,        "elb",   servoController4,5,  servoController4,4,      SVOKIND_STANDARD, HAS_ARM_SERVOS);
    InitializeServo(fOverallSuccess, svoRotor,           "rtr",   servoController3,5,  servoControllerDummy,0,  SVOKIND_CNTROT,   HAS_ARM_SERVOS);
    InitializeServo(fOverallSuccess, svoArmWrist,        "wrist", servoController3,6,  servoControllerDummy,0,  SVOKIND_STANDARD, HAS_WRIST_SERVOS);
    /**/
    svoArmRotation.fReflectedPaired = false;
    svoArmRotation.svposUpper       = svposLast;
    svoArmRotation.svposLower       = 0;
    /**/
    svoArmShoulder  .svposUpper       = 250;
    svoArmShoulder  .svposLower       = 0;
    svoArmShoulder  .svposApogee      = svposShoulderApogee;
    //
    svoArmShoulder  .isvoctlrTripped  = servoController4.isvoctlr;
    svoArmShoulder  .isvcTripped      = 3-1;
    svoArmShoulder  .isvoctlrQuad     = servoController4.isvoctlr;
    svoArmShoulder  .isvcQuad         = 6-1;
    //
    svoArmShoulder  .fReflectedPaired  = true;
    svoArmShoulder  .fReflectedTripped = true;
    svoArmShoulder  .fReflectedQuad    = false;
    /**/
    svoArmElbow     .svposUpper     = 233; /* empirical: *just* avoids hitting the stop */
    svoArmElbow     .svposLower     = 0;
    svoArmElbow     .svposApogee    = svoArmElbow.svposUpper;
    /**/
    SetServoWaits(svoArmElbow,      2.5, 1.6, 0);
    SetServoWaits(svoArmShoulder,   3.0, 2.0, 0);
    SetServoWaits(svoArmRotation,   3.0, 3.0, 0);
    }

#define PackArmSimply() MoveDispenserArm(svposPackedRotation, svposPackedElbow, svposPackedShoulder)

// Positions for the packed dispenser arm
#define svposPackedRotation         0
#define svposPackedElbow            0
#define svposPackedShoulder         210

// On v11, we have an 'intermediate intermediate' position, but not so much on v12
#define svposIntToPackedRotation    svposPackedRotation
#define svposIntToPackedElbow       svposPackedElbow
#define svposIntToPackedShoulder    svposIntShoulder

// Positions for the intermediate *unpacking* position (on the way to EOPD)
// These are WRONG for v12
#define svposIntRotation            111
#define svposIntElbow               (60  + dsvposElbow)
#define svposIntShoulder            (241 + dsvposShoulder)

#define svposEopdRotation           233

#define svposArmWristHorizontal     10

// Speed constants for the rotor. Note that we've got a whole bunch of headroom
// numerically, but in practice, we've found that the speed response curve is
// highly non-linear, and peak speed is reached long before the numerical range
// is exhausted.
#define svposRotorStop 128
#define svposRotorSlow (128-12)
#define svposRotorFast (128-32)

//---------------------------------------------------------------------------------------
// Arm geometry
//---------------------------------------------------------------------------------------

// Constants relating to the 'hand' at the end of arm which can hold batons.

#define DEG_ROTOR_TURN_DISP_TO_MAG           80.0       // WRONG! need to actually measure
// When dispensing, how many degrees from initial baton engagement
// until it's at the mag sensor detect position?

#define DEG_ROTOR_TURN_MAG_CARRY_BATON      100.0      // WRONG! need to actually measure
// How many degrees after detecting a mag baton can the rotor be turned through
// and still safely and reliably be carrying said baton?

#define DEG_ROTOR_TURN_MAG_DISPENSE_BATON   100.0       // WRONG! need to actually measure
// How many degrees after detecting a mag baton can the rotor be turned through
// before we need to focus on scoring *that* baton

#define ROTOR_FINGER_BATON_SLOTS            8
// How many slots in the rotor's fingers for carrying batons

#define DEG_ROTOR_TURN_SINGLE_BATON_SCORE   (360.0 / ROTOR_FINGER_BATON_SLOTS)
// How far should we turn the rotor to score a single baton?



// A bevy of constants about the length and angles of various parts of the arm.
// Consult the Geometer's Sketchpad drawing (v12\arm2\Model.gsp) for interpretation.
const float
    xP  = 0,
    yP  = 0, // 16.5,
    dAE = 17.6,
    dAH = 4.8,
    dBT = 17.6,
    dDF = 5.6,
    dFI = 11.2,
    dHI = 6.4,
    dNC = 22.4,
    dPQ = 4.0,
    dQN = 3.2,
    dxCA = 6.4,
    dxCD = 3.4,
    dyCA = 0.8,
    dyCD = 0.22,
    dAI = sqrt(dAH*dAH + dHI*dHI),
    dxXY = 12.9,
    dyXY = -0.01,                            // NB: negative of what's in the .GSP
    rGearBig = 3.2,
    rGearSmall = 1.6,
    gammaZero = degreesToRadians(degElbowCalibrate),

    svposRange      = 255.0,
    shoulderGearing = 2.0,
    degAlphaZero    = degShoulderCalibrate - (degShoulderServoRange * svposShoulderCalibrate) / (shoulderGearing * svposRange),
    alphaZero       = degreesToRadians(degAlphaZero), // was 0.0

    dYZ       = dBT,
    rTip      = dYZ;

void YZFromPThetaGamma(OUT POINT& ptY, OUT POINT& ptZ, float theta, float gamma)
// Compute point X from C and gamma. FYI: This code automatically generated by
// Mathematica from symbolic algebra model of the arm.
    {
    POINT ptC;

    const float v000 = alphaZero + (rGearSmall * theta) / rGearBig;
    ptC.x = (xP + dNC * cos(v000)) - dPQ;
    ptC.y = dQN + yP + dNC * sin(v000);

    const float v00 = square(dFI);
    const float v01 = square(dAI);
    const float v02 = square(dHI);
    const float v03 = square(dAH);
    const float v04 = -dQN;
    const float v05 = gamma + gammaZero;
    const float v06 = 1 / dAH;
    const float v07 = dDF * sin(v05);
    const float v08 = dDF * cos(v05);
    const float v09 = dxCA + dxCD + v08;
    const float v10 = square(v09);
    const float v11 = (dyCA + v07) - dyCD;
    const float v12 = square(v11);
    const float v13 = v10 + v12;
    const float v14 = 1 / v13;
    const float v15 = (v00 + v13) - v01;
    const float v16 = sqrt(v14 * (v00 + 0.25 * v14 * v15 * (((v01 - v12) - v10) - v00)));
    const float v17 = (((dyCD + 0.5 * v11 * v14 * v15) - v09 * v16) - v07) - dyCA;
    const float v18 = (((v11 * v16 + 0.5 * v09 * v14 * v15) - v08) - dxCD) - dxCA;
    const float v19 = square(v17);
    const float v20 = square(v18);
    const float v21 = v19 + v20;
    const float v22 = 1 / v21;
    const float v23 = (v03 + v21) - v02;
    const float v24 = sqrt(v22 * (v03 + 0.25 * v22 * v23 * (((v02 - v20) - v19) - v03)));
    ptY.x = dxCA + dxXY + ptC.x + dAE * v06 * (0.5 * v18 * v22 * v23 - v17 * v24);
    ptY.y = dyXY + v04 + ptC.y + dAE * v06 * (v18 * v24 + 0.5 * v17 * v22 * v23);
    ptZ.x = dxCA + dxXY + ptC.x;
    ptZ.y = dyXY + v04 + ptC.y;
    }

#define ComputeArmTipLocation(state)                                                        \
    {                                                                                       \
    YZFromPThetaGamma(state.ptTip, state.ptTipCenter, state.radShoulder, state.radElbow);   \
    }

// Convert angles to svpos in this state, and back again. In the v12 model, larger angles
// are higher (unlike v11). This corresponds to numerically higher svpos as well (to be confirmed).
#define ArmTipSvposToAngle(state)                                                           \
    {                                                                                       \
    state.radShoulder = (float)(state.svposShoulder) * svoArmShoulder.dsvposToRadians;      \
    state.radElbow    = (float)(state.svposElbow)    * svoArmElbow.dsvposToRadians;         \
    }

#define ArmTipAngleToSvpos(state)                                                           \
    {                                                                                       \
    state.svposShoulder = Rounded(state.radShoulder * svoArmShoulder.radiansToDsvpos, int); \
    state.svposElbow    = Rounded(state.radElbow    * svoArmElbow.radiansToDsvpos, int);    \
    }

//---------------------------------------------------------------------------------------
// Debugging the arm tip math - these are omitted in final code
//---------------------------------------------------------------------------------------

#ifndef USE_ARM_MOVE_DEBUG
#define USE_ARM_MOVE_DEBUG 0
#endif

#if USE_ARM_MOVE_DEBUG

void CFromPTheta(OUT POINT& ptC, float theta)
    {
    const float v00 = alphaZero + (rGearSmall * theta) / rGearBig;
    ptC.x = (xP + dNC * cos(v00)) - dPQ;
    ptC.y = dQN + yP + dNC * sin(v00);
    }

void DFromCGamma(OUT POINT& ptD, float xC, float yC, float gamma)
    {
    ptD.x = xC - dxCD;
    ptD.y = yC + dyCD;
    }

void FFromCGamma(OUT POINT& ptF, float xC, float yC, float gamma)
    {
    const float v00 = gamma + gammaZero;
    ptF.x = (xC - dDF * cos(v00)) - dxCD;
    ptF.y = (dyCD + yC) - dDF * sin(v00);
    }

void XFromCGamma(OUT POINT& ptX, float xC, float yC, float gamma)
    {
    const float v00 = square(dFI);
    const float v01 = square(dAI);
    const float v02 = square(dHI);
    const float v03 = square(dAH);
    const float v04 = gamma + gammaZero;
    const float v05 = 1 / dAH;
    const float v06 = dDF * sin(v04);
    const float v07 = dDF * cos(v04);
    const float v08 = dxCA + dxCD + v07;
    const float v09 = square(v08);
    const float v10 = (dyCA + v06) - dyCD;
    const float v11 = square(v10);
    const float v12 = v09 + v11;
    const float v13 = 1 / v12;
    const float v14 = (v00 + v12) - v01;
    const float v15 = sqrt(v13 * (v00 + 0.25 * v13 * v14 * (((v01 - v11) - v09) - v00)));
    const float v16 = (((dyCD + 0.5 * v10 * v13 * v14) - v08 * v15) - v06) - dyCA;
    const float v17 = (((v10 * v15 + 0.5 * v08 * v13 * v14) - v07) - dxCD) - dxCA;
    const float v18 = square(v16);
    const float v19 = square(v17);
    const float v20 = v18 + v19;
    const float v21 = 1 / v20;
    const float v22 = (v03 + v20) - v02;
    const float v23 = sqrt(v21 * (v03 + 0.25 * v21 * v22 * (((v02 - v19) - v18) - v03)));
    ptX.x = dxCA + xC + dAE * v05 * (0.5 * v17 * v21 * v22 - v16 * v23);
    ptX.y = (yC + dAE * v05 * (v17 * v23 + 0.5 * v16 * v21 * v22)) - dQN;
    }

#endif

//-------------------------------------------------------------------------------------------------------
// Turn calibration
//-------------------------------------------------------------------------------------------------------

// Table of motor encoder deltas necessary to achieve certain angles of turn.
// This table was created by empirical observation of the robot. Each entry in
// the table has three entries: a target turn angle, then left and right encoder
// deltas needed to achieve that angle. The table must be sorted in increasing
// order by angle.

#define HAS_ENCODER_BASED_TURNING_TABLE 1

#define TURNFLAG_DEFAULT TURNFLAG_PROHIBIT_GYRO

float mpangledenc[][3] =
    {
        { -180.0000, 3215.0, -3215.0 },
        { -90.0000,  1574.0, -1574.0 },
        { -45.0000,   709.0,  -709.0 },
        { -28.58,     400.0,  -400.0 },
        { -16.64,     200.0,  -200.0 },
        {  -4.00,      55.0,   -55.0 }, // manual fudge, linear = 48.1
        {   0.0,        0.0,     0.0 },
        {   4.00,     -55.0,    55.0 }, // manual fudge, linear = 50.0
        {  16.00,    -200.0,   200.0 },
        {  27.67,    -400.0,   400.0 },
        { 45.0000,   -748.0,   748.0 },
        { 90.0000,  -1644.0,  1644.0 },
        { 180.0000, -3403.0,  3403.0 },
    };

#define mpangledencScaleFactor 1.0

//-------------------------------------------------------------------------------------------------------
// Miscellaneous
//-------------------------------------------------------------------------------------------------------

#ifndef MS_DRIVE_WAIT
#define MS_DRIVE_WAIT       100
#endif

#define DRIVE_POWER_SLOW        15
#define TURN_POWER_STRAIGHTEN   TURN_POWER_MEDIUM
