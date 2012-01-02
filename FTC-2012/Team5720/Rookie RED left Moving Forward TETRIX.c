#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTMotor,  HTServo)
#pragma config(Sensor, S1,     ,                    sensorI2CMuxController)
#pragma config(Motor,  mtr_S1_C1_1,     motorD,        tmotorNormal, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C1_2,     motorE,        tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,     motorF,        tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C2_2,     motorG,        tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C3_1,     motorH,        tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C3_2,     motorI,        tmotorNormal, openLoop)
#pragma config(Servo,  srvo_S1_C4_1,    servo1,               tServoStandard)
#pragma config(Servo,  srvo_S1_C4_2,    servo2,               tServoStandard)
#pragma config(Servo,  srvo_S1_C4_3,    bowlingballservo,     tServoStandard)
#pragma config(Servo,  srvo_S1_C4_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C4_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C4_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "JoystickDriver.c"

/*--------------------------------------------------------------------------------------------------------*\
|*                                                                                                        *|
|*                                         - Moving Forward -                                             *|
|*                                       ROBOTC on NXT + TETRIX                                           *|
|*                                                                                                        *|
|*  This program runs your TETRIX robot forward for a designated amount of time.                          *|
|*                                                                                                        *|
|*                                        ROBOT CONFIGURATION                                             *|
|*    NOTES:                                                                                              *|
|*    1)  Remember to use 'Robot -> Motors and Sensors Setup'to set up your motor configuration.          *|
|*    2)  If using kit TETRIX bot, remember to reverse Motor E in 'Motor and Sensor Setup'.               *|
|*                                                                                                        *|
|*    MOTORS & SENSORS:                                                                                   *|
|*    [I/O Port]              [Name]              [Type]              [Description]                       *|
|*    Port 1                  none                TETRIX Controller   TETRIX                              *|
|*    Port 1 - Motor 1        motorD              12V                 Right motor                         *|
|*    Port 1 - Motor 2        motorE              12V                 Left motor                          *|
\*---------------------------------------------------------------------------------------------------4246-*/

// At startup, place the servo in the down position
const int SERVO_UP_POS = 210;
const int SERVO_DOWN_POS = 85;

void initializeRobot()
{

   motor[motorD] = 0;   // Motor D is run at a power level of 25. forward
   motor[motorH] = 0;   // Motor H is run at a power level of 25.
    servo[bowlingballservo] = SERVO_UP_POS;
    servo[servo1] = 0;
    servo[servo2] = 0;
}

task main()

{
 initializeRobot();
  waitForStart();   // wait for start of tele-op phase


   motor[motorD] = 25;   // Motor D is run at a power level of 25. forward
   motor[motorH] = 25;   // Motor H is run at a power level of 25.
   wait1Msec(1500);       // The program waits 3000 milliseconds (3 seconds) before running further code

   motor[motorD] = 0;   // Motor D stop.
   motor[motorH] = 0;   // Motor H stop.
   wait1Msec(800);

   motor[motorD] = -50;   // Motor D is run at a power level of -25. turn left
   motor[motorH] = 50;   // Motor H is run at a power level of 25.
   wait1Msec(700);

   motor[motorD] = 0;   // Motor D stop.
   motor[motorH] = 0;   // Motor H stop.
   wait1Msec(800);

   motor[motorD] = 50;   // Motor D is run at a power level of 25. forward
   motor[motorH] = 50;   // Motor H is run at a power level of 25.
   wait1Msec(1500);

   motor[motorD] = 0;   // Motor D stop.
   motor[motorH] = 0;   // Motor H stop.
   wait1Msec(800);

   int delta = 50;


  {
    servoChangeRate[servo1] = delta;          // Slow the Servo Change Rate down to only 'delta' positions per update.

    if(ServoValue[servo1] < 128)              // If servo1 is closer to 0 (than 255):
    {
        if (ServoValue[servo1] < 255) {
            servo[servo1] = 255;        // Move servo1 to position to 255.
            while(ServoValue[servo1] < 255)               // While the ServoValue of servo1 is less than 255:
                EndTimeSlice();
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

    motor[motorD] = -50;
    motor[motorH] = -50;
    wait1Msec(500);

   motor[motorD] = -50;   // Motor D is run at a power level of 25. forward
   motor[motorH] = 50;   // Motor H is run at a power level of 25.
   wait1Msec(600);

   motor[motorD] = 0;
    motor[motorH] = 0;
    wait1Msec(800);




   motor[motorD] = 50;   // Motor D is run at a power level of 25. forward
   motor[motorH] = 50;   // Motor H is run at a power level of 25.
   wait1Msec(3000);
}