#pragma config(Hubs,  S1, HTMotor,  HTServo,  none,     none)
#pragma config(Motor,  mtr_S1_C1_1,     leftMotor,     tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C1_2,     rightMotor,    tmotorNormal, openLoop)
#pragma config(Servo,  srvo_S1_C2_1,    servo1,                     tServoNormal)
#pragma config(Servo,  srvo_S1_C2_2,    servo2,                     tServoNormal)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "JoystickDriver.c"

//TJoystick joystick;

void motorControlProportional()
{
  int nMotorPower;
  int nDirection;

  nMotorPower = joystick.joy1_y1;
  nDirection  = joystick.joy1_x1;
  if (nDirection < 0)
  {
    // Turn left

    motor[rightMotor] = nMotorPower; // use power settings from Y-axis
    motor[leftMotor]  = nMotorPower * (64 + nDirection) / 64;
  }
  else
  {
    // Turn right

    motor[leftMotor]  = nMotorPower; // use power settings from Y-axis
    motor[rightMotor] = nMotorPower * (64 - nDirection) / 64;
  }
}

void buttonControl()
{
int delta = 2;                        // Create int 'delta' to the be Servo Change Rate.

  if (joystick.joy1_Buttons == 03)
  {
     servoChangeRate[servo1] = delta;          // Slow the Servo Change Rate down to only 'delta' positions per update.

    if(ServoValue[servo1] < 128)              // If servo1 is closer to 0 (than 255):
    {
      while(ServoValue[servo1] < 255)               // While the ServoValue of servo1 is less than 255:
      {
        servo[servo1] = 255;                              // Move servo1 to position to 255.
      }
    }

    wait1Msec(1000);                          // Wait 1 second.

    if(ServoValue[servo1] >= 128)             // If servo1 is closer to 255 (than 0):
    {
      while(ServoValue[servo1] > 0)                 // While the ServoValue of servo1 is greater than 0:
      {
        servo[servo1] = 0;                                // Move servo1 to position to 0.
      }
    }

    wait1Msec(1000);                          // Wait 1 second.
  }

}

task main()
{
  while (true)
  {
    getJoystickSettings(joystick);
    motorControlProportional();
    buttonControl();
  }
}