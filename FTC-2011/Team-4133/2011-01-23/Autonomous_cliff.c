#pragma config(Hubs,  S1, HTMotor,  HTServo,  HTMotor,  HTMotor)
#pragma config(Sensor, S2,     wallTouch,           sensorTouch)
#pragma config(Sensor, S4,     irSeeker,            sensorHiTechnicIRSeeker1200)
#pragma config(Motor,  mtr_S1_C1_1,     leftFrontMotor, tmotorNormal, PIDControl, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     rightFrontMotor, tmotorNormal, PIDControl, reversed, encoder)
#pragma config(Motor,  mtr_S1_C3_1,     leftBackMotor, tmotorNormal, PIDControl, encoder)
#pragma config(Motor,  mtr_S1_C3_2,     rightBackMotor, tmotorNormal, PIDControl, reversed, encoder)
#pragma config(Motor,  mtr_S1_C4_1,     elevator,      tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C4_2,     motorI,        tmotorNormal, openLoop)
#pragma config(Servo,  srvo_S1_C2_1,    leftScoop,            tServoStandard)
#pragma config(Servo,  srvo_S1_C2_3,    rightScoop,           tServoStandard)
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

#include "JoystickDriver.c"  //Include file to "handle" the Bluetooth messages.


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
  servo[leftScoop] = 176;
  servo[rightScoop] = 20;
}

void findBeacon()
{
  if(SensorValue[irSeeker] == 5)
  {
    motor[leftFrontMotor] = 60;
    //motor[leftBackMotor] = -60;
    motor[rightFrontMotor] = 60;
    //motor[rightBackMotor] = -60;
  }

  if(SensorValue[irSeeker] > 5)
  {
    motor[leftFrontMotor] = 60;
    //motor[leftBackMotor] = -60;
    motor[rightFrontMotor] = -60;
    //motor[rightBackMotor] = 60;
  }

  if(SensorValue[irSeeker] < 5)
  {
    motor[leftFrontMotor] = -60;
    //motor[leftBackMotor] = 60;
    motor[rightFrontMotor] = 60;
    //motor[rightBackMotor] = -60;
  }
}

void moveForward(int i_distance, int i_power)
{
  nMotorEncoder[leftFrontMotor] = 0;
  nMotorEncoder[leftBackMotor] = 0;
  nMotorEncoder[rightFrontMotor] = 0;
  nMotorEncoder[rightBackMotor] = 0;

  nMotorEncoderTarget[leftFrontMotor] = i_distance;
  nMotorEncoderTarget[leftBackMotor] = i_distance;
  nMotorEncoderTarget[rightFrontMotor] = i_distance;
  nMotorEncoderTarget[rightBackMotor] = i_distance;

  motor[leftFrontMotor] = i_power;
  motor[leftBackMotor] = i_power;
  motor[rightFrontMotor] = i_power;
  motor[rightBackMotor] = i_power;

  while(nMotorRunState[leftFrontMotor] != runStateIdle || nMotorRunState[rightFrontMotor] != runStateIdle)
  {
    //do nothing
  }
}

void turnLeft(int i_distance, int i_power)
{
  nMotorEncoder[leftFrontMotor] = 0;
  nMotorEncoder[leftBackMotor] = 0;
  nMotorEncoder[rightFrontMotor] = 0;
  nMotorEncoder[rightBackMotor] = 0;

  nMotorEncoderTarget[leftFrontMotor] = i_distance;
  nMotorEncoderTarget[leftBackMotor] = i_distance;
  nMotorEncoderTarget[rightFrontMotor] = i_distance;
  nMotorEncoderTarget[rightBackMotor] = i_distance;

  motor[leftFrontMotor] = i_power*-1;
  motor[leftBackMotor] = i_power*-1;
  motor[rightFrontMotor] = i_power;
  motor[rightBackMotor] = i_power;

  while(nMotorRunState[leftFrontMotor] != runStateIdle || nMotorRunState[rightFrontMotor] != runStateIdle)
  {
    //do nothing
  }
  motor[leftBackMotor] = 0;
  motor[rightBackMotor] = 0;
}

void turnRight(int i_distance, int i_power)
{
  nMotorEncoder[leftFrontMotor] = 0;
  nMotorEncoder[leftBackMotor] = 0;
  nMotorEncoder[rightFrontMotor] = 0;
  nMotorEncoder[rightBackMotor] = 0;

  nMotorEncoderTarget[leftFrontMotor] = i_distance;
  nMotorEncoderTarget[leftBackMotor] = i_distance;
  nMotorEncoderTarget[rightFrontMotor] = i_distance;
  nMotorEncoderTarget[rightBackMotor] = i_distance;

  motor[leftFrontMotor] = i_power;
  motor[leftBackMotor] = i_power;
  motor[rightFrontMotor] = i_power*-1;
  motor[rightBackMotor] = i_power*-1;

  while(nMotorRunState[leftFrontMotor] != runStateIdle || nMotorRunState[rightFrontMotor] != runStateIdle)
  {
    //do nothing
  }
  motor[leftBackMotor] = 0;
  motor[rightBackMotor] = 0;
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

  waitForStart(); // Wait for the beginning of autonomous phase.

  moveForward((1400*9),50);
//  turnRight((1400*6),40);
//  moveForward((1400*16),50);
//  turnLeft((1400*6),40);
//  moveForward((1400*2),50);

}