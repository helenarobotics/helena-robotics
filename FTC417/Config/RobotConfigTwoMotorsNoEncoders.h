//
// RobotTwoMotorsNoEncoders.h
//
//-------------------------------------------------------------------------------------------------------
// Motor and servo controllers
//-------------------------------------------------------------------------------------------------------

#define motorController     rgmotorctlr[0]
#define motorRight          rgmotor[0]
#define motorLeft           rgmotor[1]

void InitializeMotors(IN OUT STICKYFAILURE& fOverallSuccess)
    {
    // The one motor controller on this robot is controller #1 in the daisy
    // chain of controllers on sensor port S1 (and on this bot, that's the ONLY controller
    // in said daisy chain).
    InitializeMotorController(fOverallSuccess, motorController, I2CLINK_1, 1);

    // Our two motors are motors #1 and #2 respectively on our motor controller
    InitializeMotor(fOverallSuccess, motorLeft,  motorController, 1, "left",  MOTORPROP_NONE);
    InitializeMotor(fOverallSuccess, motorRight, motorController, 2, "right", MOTORPROP_REFLECTED);
    }

//-------------------------------------------------------------------------------------------------------
// Definition and initialization of sensors (see also boilerplate.h)
//-------------------------------------------------------------------------------------------------------

void InitializeSensors(IN OUT STICKYFAILURE& fOverallSuccess)
    {
    // This robot has no sensors, and so nothing needs to be done in InitializeSensors
    }

//-------------------------------------------------------------------------------------------------------
// Servos
//-------------------------------------------------------------------------------------------------------

void InitializeServos(IN OUT STICKYFAILURE& fOverallSuccess)
    {
    // This robot has no serovs, and so nothing needs to be done in InitializeServos
    }
