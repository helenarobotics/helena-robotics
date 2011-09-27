#pragma config(Hubs,  S1, HTMotor,  HTServo,  HTMotor,  HTMotor)
#pragma config(Sensor, S2,     wallTouch,           sensorTouch)
#pragma config(Sensor, S4,     irSeeker,            sensorHiTechnicIRSeeker1200)
#pragma config(Motor,  mtr_S1_C1_1,     leftFrontMotor, tmotorNormal, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     rightFrontMotor, tmotorNormal, openLoop, reversed, encoder)
#pragma config(Motor,  mtr_S1_C3_1,     leftBackMotor, tmotorNormal, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C3_2,     rightBackMotor, tmotorNormal, openLoop, reversed, encoder)
#pragma config(Motor,  mtr_S1_C4_1,     elevator,      tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C4_2,     dispenser,     tmotorNormal, openLoop)
#pragma config(Servo,  srvo_S1_C2_1,    left1Scoop,           tServoStandard)
#pragma config(Servo,  srvo_S1_C2_2,    left2Scoop,           tServoStandard)
#pragma config(Servo,  srvo_S1_C2_3,    right1Scoop,          tServoStandard)
#pragma config(Servo,  srvo_S1_C2_4,    right2Scoop,          tServoStandard)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "JoystickDriver.c"  //Include file to "handle" the Bluetooth messages.

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                    initializeRobot
//
// Prior to the start of tele-op mode, you may want to perform some initialization on your robot
// and the variables within your program.
//
// In most cases, you may not have to add any code to this function and it will remain "empty".
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
void initializeRobot()
{
  //servo[leftScoop] = 90;
  //servo[rightScoop] = 90;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                               adjJoystickMotorPower
//
// As the range of values for the joysticks are -128 to 127 and the range of power settings for the
// motors are -100 to 100. We must adjust the joystick values to be within the motor values. This
// will allow the best driving experience for the robot.
//
// Also, since the joysticks do not always center exactly at position 0 when released, we use a
// thresh hold value to provide a small range of values that all equate to no movement
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
int adjJoystickMotorPower(int i_joystickReading, int i_rangeMax, int i_threshHold)
{
  int vMotorPower = i_joystickReading;

  //Compare to the thresh hold to account for variations in calibration
  if (abs(vMotorPower) < i_threshHold)
  {
    vMotorPower = 0;
  }
  else
  {
    //The value is outside the thresh hold. We now need to adjust to the
    //desired range. To do this first we need to subtract our thresh hold value
    vMotorPower = vMotorPower - i_threshHold;

    //Next we adjust for our range
    vMotorPower = floor((vMotorPower * i_rangeMax) / (127 - i_threshHold));
  }

  return vMotorPower;
}

void nudgeMovement(string direction)
{
  int leftSpeed = 20;
  int rightSpeed = 20;

  if (direction == "LEFT")
  {
    leftSpeed = leftSpeed * -1;
  }
  if (direction == "RIGHT")
  {
    rightSpeed = rightSpeed * -1;
  }
  if (direction == "BACK")
  {
    leftSpeed = leftSpeed * -1;
    rightSpeed = rightSpeed * -1;
  }

  motor[rightFrontMotor] = rightSpeed;
  motor[rightBackMotor] = rightSpeed;
  motor[leftFrontMotor] = leftSpeed;
  motor[leftBackMotor] = leftSpeed;
  wait1Msec(100);
  motor[rightFrontMotor] = 0;
  motor[rightBackMotor] = 0;
  motor[leftFrontMotor] = 0;
  motor[leftBackMotor] = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                      MotorControl
//
// This procedure is responsible for translating the joystick readings into motor power for all
// the various motors on the robot
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
void movementControl()
{
  int leftMotorPower;
  int rightMotorPower;
  int maxMotorPower;

  if (joy1Btn(5)||joy1Btn(6))
  {
    maxMotorPower = 100;
  }else if (joy1Btn(7)||joy1Btn(8))
  {
    maxMotorPower = 20;
  }else
  {
    maxMotorPower = 60;
  }

  if (joy1Btn(1))
  {
    nudgeMovement("LEFT");
  }else if (joy1Btn(2))
  {
    nudgeMovement("BACK");
  }else if (joy1Btn(3))
  {
    nudgeMovement("RIGHT");
  }else if (joy1Btn(4))
  {
    nudgeMovement("FRONT");
  }else
  {
    leftMotorPower = adjJoystickMotorPower(joystick.joy1_y1, maxMotorPower, 5);
    rightMotorPower = adjJoystickMotorPower(joystick.joy1_y2, maxMotorPower, 5);

    motor[rightFrontMotor] = rightMotorPower;
    motor[rightBackMotor] = rightMotorPower;
    motor[leftFrontMotor] = leftMotorPower;
    motor[leftBackMotor] = leftMotorPower;
  }
}

void elevatorControl()
{
  int elevatorPower;
  int elevatorMaxPower;

  if (joy2Btn(5))
  {
    elevatorMaxPower = 75;
  }else if (joy2Btn(7))
  {
    elevatorMaxPower = 25;
  }else
  {
    elevatorMaxPower = 50;
  }

  elevatorPower = adjJoystickMotorPower(joystick.joy2_y1, elevatorMaxPower, 5);

  motor[elevator] = elevatorPower;

  if (joy2Btn(8))
  {
    motor[dispenser] = -50;
  }
  else
  {
    motor[dispenser] = 0;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                      MotorControl
//
// This procedure is responsible for performing the appropriate actions based on which buttons
// are pressed on the joystick
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
void scoopControl()
{

  if (joy2Btn(1))
  {
    servo[left1Scoop] = 70;
    servo[left2Scoop] = 70;
    servo[right1Scoop] = 70;
    servo[right2Scoop] = 70;
  }
  if (joy2Btn(2))
  {
    servo[left1Scoop] = 0;
    servo[left2Scoop] = 0;
    servo[right1Scoop] = 0;
    servo[right2Scoop] = 0;
  }
  if (joy2Btn(3))
  {
    servo[left1Scoop] = 100;
    servo[left2Scoop] = 100;
    servo[right1Scoop] = 100;
    servo[right2Scoop] = 100;
  }
  if (joy2Btn(4))
  {
    servo[left1Scoop] = 150;
    servo[left2Scoop] = 150;
    servo[right1Scoop] = 150;
    servo[right2Scoop] = 150;
  }
  /*if (joy2Btn(6))
  {
    servo[left1Scoop] = 190;
    servo[left2Scoop] = 190;
    servo[right1Scoop] = 190;
    servo[right2Scoop] = 190;
  }*/
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                         Main Task
//
// The following is the main code for the tele-op robot operation.
//
// Game controller / joystick information is sent periodically (about every 50 milliseconds) from
// the FMS (Field Management System) to the robot. Most tele-op programs will follow the following
// logic:
//   1. Loop forever repeating the following actions:
//   2. Get the latest game controller / joystick settings that have been received from the PC.
//   3. Perform appropriate actions based on the joystick + buttons settings. This is usually a
//      simple action:
//      *  Joystick values are usually directly translated into power levels for a motor or
//         position of a servo.
//      *  Buttons are usually used to start/stop a motor or cause a servo to move to a specific
//         position.
//   4. Repeat the loop.
//
// Your program needs to continuously loop because you need to continuously respond to changes in
// the game controller settings.
//
// At the end of the tele-op period, the FMS will autonmatically abort (stop) execution of the program.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

task main()
{
  initializeRobot();

  waitForStart();   // wait for start of tele-op phase

  while (true)
  {
    getJoystickSettings(joystick);
    movementControl();
    elevatorControl();
    scoopControl();
  }
}
