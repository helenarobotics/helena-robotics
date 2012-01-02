#pragma config(Hubs,  S1, HTServo,  HTMotor,  HTMotor,  HTMotor)
#pragma config(Sensor, S1,     ,                    sensorI2CMuxController)
#pragma config(Motor,  mtr_S1_C2_1,     DriveRight,    tmotorNormal, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C2_2,     DriveLeft,     tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C3_1,     motorH,        tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C3_2,     motorI,        tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C4_1,     BallPickUp,    tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C4_2,     motorI,        tmotorNormal, openLoop)
#pragma config(Servo,  srvo_S1_C1_1,    servo1,               tServoNone)
#pragma config(Servo,  srvo_S1_C1_2,    servo2,               tServoNone)
#pragma config(Servo,  srvo_S1_C1_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S1_C1_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C1_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C1_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                           Autonomous Mode Code Template
//
// This file contains a template for simplified creation of an autonomous program for an TETRIX robot
// competition.
//
// You need to customize two functions with code unique to your specific robot.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

#include "JoystickDriver.c"  // Include file to "handle" the Bluetooth messages.
int vtile = 1333;
int v90turn = 900;
int v45turn = 450;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                    initializeRobot
//
// Prior to the start of autonomous mode, you may want to perform some initialization on your robot.
// Things that might be performed during initialization include:
//   1. Move motors and servos to a preset position.
//   2. Some sensor types take a short while to reach stable values during which time it is best that
//      robot is not moving. For example, gyro sensor needs a few seconds to obtain the background
//      "bias" value.
//
// In many cases, you may not have to add any code to this function and it will remain "empty".
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

void initializeRobot()
{
    // Place code here to sinitialize servos to starting positions.
    // Sensors are automatically configured and setup by ROBOTC. They may need a brief time to stabilize.
    return;
}

void moveforward(int speed, int vtime)
{
    motor[DriveLeft] = speed;
    motor[DriveRight] = speed;

    wait1Msec(vtime);

    motor[DriveLeft] = 0;
    motor[DriveRight] = 0;
}

void rightturn(int speed, int vtime)
{
    motor[DriveLeft] = speed * (-1);
    motor[DriveRight] = speed;

    wait1Msec(vtime);

    motor[DriveLeft] = 0;
    motor[DriveRight] = 0;
}

void leftturn(int speed, int vtime)
{
    motor[DriveLeft] = speed;
    motor[DriveRight] = speed * (-1);

    wait1Msec(vtime);

    motor[DriveLeft] = 0;
    motor[DriveRight] = 0;
}

void parkballfront(string vcolor, string vposition)
{
    int vpostile = 1;

    if (vposition =="in")
    {
        vpostile = vpostile + 1;
    }
    moveforward(50, 2000);

    if (vcolor =="red")
    {
        leftturn(50, v90turn);
    }
    else
    {
        rightturn(50, v90turn);
    }
    moveforward(50, vtile * vpostile);
    if (vcolor =="red")
    {
        leftturn(50, v45turn);
    }
    else
    {
        rightturn(50, v45turn);
    }
    moveforward(50, vtile * 3);

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                         Main Task
//
// The following is the main code for the autonomous robot operation. Customize as appropriate for
// your specific robot.
//
// The types of things you might do during the autonomous phase (for the 2008-9 FTC competition)
// are:
//
//   1. Have the robot follow a line on the game field until it reaches one of the puck storage
//      areas.
//   2. Load pucks into the robot from the storage bin.
//   3. Stop the robot and wait for autonomous phase to end.
//
// This simple template does nothing except play a periodic tone every few seconds.
//
// At the end of the autonomous period, the FMS will autonmatically abort (stop) execution of the program.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

task main()
{
    initializeRobot();

//    waitForStart(); // Wait for the beginning of autonomous phase.

    ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////
    ////                                                   ////
    ////    Add your robot specific autonomous code here.  ////
    ////                                                   ////
    ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////

    ////////////////////////////////
    // straight to back from blue //
    ////////////////////////////////
//    moveforward (50,vtile);
//    ftturn(50,v90turn);

    //////////////////////////////////
    //park with ball front from blue//
    //////////////////////////////////
    parkballfront("blue","in");
}
