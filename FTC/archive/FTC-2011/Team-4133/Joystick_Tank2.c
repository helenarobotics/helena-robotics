#pragma config(Hubs,  S1, HTMotor,  none,     none,     none)
#pragma config(Sensor, S4,     sensorSonar,         sensorSONAR)
#pragma config(Motor,  mtr_S1_C1_1,     leftMotor,     tmotorNormal, PIDControl, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     rightMotor,    tmotorNormal, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "JoystickDriver.c"

//TJoystick joystick;

void motorControlProportional()
{
  int nLeftMotor;
  int nRightMotor;

  nLeftMotor = joystick.joy1_y1;
  nRightMotor = joystick.joy1_y2;

  motor[rightMotor] = nRightMotor; // use power settings from Y-axis
  motor[leftMotor]  = nLeftMotor;
}
/*
void buttonControl()
{
  if (joystick.joy1_Buttons == 02)
  {
     servo[servoRight] = 120;
     servo[servoLeft] = 76;
  }
  if (joystick.joy1_Buttons == 01)
  {
     servo[servoRight] = 0;
     servo[servoLeft] = 196;
  }
  if (joystick.joy1_Buttons == 03)
  {
     servo[servoRight] = 170
     servo[servoLeft] = 26;
  }
}
*/
task main()
{
  while (true)
  {
    getJoystickSettings(joystick);
    motorControlProportional();
    //buttonControl();
  }
}
