//
// MouseBot\RobotMouseBot.h
//
// Static configuration information about the robot: its geometry,
// sensors, etc.
// Note that edits to this file invariably need to remain in sync with
// changes to the boilerplate at the start of .c files.

//-----------------------------------------------------------------------------
// Constants regarding the wheels and motors
//-----------------------------------------------------------------------------

#define CM_BETWEEN_WHEELS           ((27.5 + 19.5) * 0.5)
#define CM_WHEEL_CIRCUMFERENCE      (3.0 * 2.54 * PI)   // they're 3" wheels, even though they are treads
#define MOTOR_GEARING               1.0 // the mechanical advantage from motor to wheel

//-----------------------------------------------------------------------------
// Motor and servo controllers
//-----------------------------------------------------------------------------
#define motorControllerLeft  rgmotorctlr[0]
#define motorControllerRight rgmotorctlr[1]
#define motorLeftFront      rgmotor[0]
#define motorLeftBack       rgmotor[1]
#define motorRightFront     rgmotor[2]
#define motorRightBack      rgmotor[3]

void
InitializeMotors(IN OUT STICKYFAILURE &fOverallSuccess) {
    InitializeMotorController(fOverallSuccess, motorControllerLeft, I2CLINK_1,
        1);
    InitializeMotorController(fOverallSuccess, motorControllerRight, I2CLINK_1,
        2);

    InitializeMotor(fOverallSuccess, motorLeftFront, motorControllerLeft, 1,
        "motorLeftFront", MOTORPROP_ENCODER);
    InitializeMotor(fOverallSuccess, motorLeftBack, motorControllerLeft, 2,
        "motorLeftBack", MOTORPROP_ENCODER);
    InitializeMotor(fOverallSuccess, motorRightFront, motorControllerRight, 1,
        "motorRightFront", MOTORPROP_ENCODER);
    InitializeMotor(fOverallSuccess, motorRightBack, motorControllerRight, 2,
        "motorRightBack", MOTORPROP_ENCODER);
}

//-----------------------------------------------------------------------------
// Definition and initialization of sensors (see also boilerplate.h)
//-----------------------------------------------------------------------------
void
InitializeSensors(IN OUT STICKYFAILURE &fOverallSuccess) {
    // This robot has no sensors, and so nothing needs to be done in
    // InitializeSensors
}

//-----------------------------------------------------------------------------
// Servos
//-----------------------------------------------------------------------------
void
InitializeServos(IN OUT STICKYFAILURE &fOverallSuccess) {
    // This robot has no serovs, and so nothing needs to be done in
    // InitializeServos
}

//-----------------------------------------------------------------------------
// Miscelleanous
//-----------------------------------------------------------------------------
