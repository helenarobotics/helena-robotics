#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTServo,  none)
#pragma config(Sensor, S1,     ,                    sensorI2CMuxController)
#pragma config(Motor,  motorA,          Nxtrightmotor, tmotorNormal, PIDControl, encoder)
#pragma config(Motor,  motorB,          Nxtleftmotor,  tmotorNormal, PIDControl, encoder)
#pragma config(Motor,  mtr_S1_C1_1,     Leftmotor,     tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C1_2,     Rightmotor,    tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,     Ballmotor,     tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C2_2,     Armmotor,      tmotorNormal, openLoop, reversed)
#pragma config(Servo,  srvo_S1_C3_1,    Rightservo,           tServoStandard)
#pragma config(Servo,  srvo_S1_C3_2,    Leftservo,            tServoStandard)
#pragma config(Servo,  srvo_S1_C3_3,    Rotatorservo,         tServoStandard)
#pragma config(Servo,  srvo_S1_C3_4,    clawservo,            tServoStandard)
#pragma config(Servo,  srvo_S1_C3_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//#pragma config(Motor,  mtr_S1_C2_2,     Armmotor,     tmotorNormal, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "JoystickDriver.c"

//TJoystick joystick;

task main()
{
 int threshold = 20;
 int location = 120;
 int clawPos = 125;
 bool btn1WP = false;
 bool btn1IP = false;
 bool btn4WP = false;
 bool btn4IP = false;

 servo[Rotatorservo] = 0;

 while (true)
  {
    btn1IP = joy2Btn(1);
    btn4IP = joy2Btn(4);
    nxtDisplayString(0, "encoder=%d", nMotorEncoder[Armmotor]);//Up is Positive
    getJoystickSettings(joystick);
      if(abs(joystick.joy1_y2) > threshold)
          motor[Rightmotor] = joystick.joy1_y2;
        else
          motor[Rightmotor] = 0;

      if(abs(joystick.joy1_y1) > threshold)
          motor[Leftmotor] = -joystick.joy1_y1;
        else
          motor[Leftmotor] = 0;

      if(abs(joystick.joy2_y2) > threshold)
         motor[Nxtleftmotor] = joystick.joy2_y2;
        else
          motor[Nxtleftmotor] = 0;

      if(abs(joystick.joy2_y1) > threshold)
          motor[Nxtrightmotor] = joystick.joy2_y1;
        else
          motor[Nxtrightmotor] = 0;

      if(joy1Btn(6))
        motor[Ballmotor] = 100;
      else
        if(joy1Btn(8))
            motor[Ballmotor] = -100;
        else
            motor[Ballmotor] = 0;

            //int armPos=nMotorEncoder[Armmotor];
/*
      if(joy2Btn(6)  && armPos < 720)
        motor[Armmotor] = 80;
      else
        if(joy2Btn(8) && armPos > 0)
            motor[Armmotor] = -10;
        else
            motor[Armmotor] = 0;
*/
      if(joy2Btn(6))
        motor[Armmotor] = 80;
      else if(joy2Btn(8))
        motor[Armmotor] = -10;
      else
         motor[Armmotor] = 0;

      if(joy2Btn(2))
      {
        servo[Rightservo] = 116;
        servo[Leftservo] = 128;
      }
         if(joy2Btn(3))
      {
           servo[Rightservo] = 0;
           servo[Leftservo] = 255;
      }
     if(btn1IP && !btn1WP){
        location = location + 10;
        if(location > 255)
          location = 255;
        servo[Rotatorservo] = location;
     }

     else if(btn4WP && !btn4IP){
        location = location - 10;
        if(location <  0)
          location = 0;
        servo[Rotatorservo] = location;
     }
     nxtDisplayString(1, "%d", joy1Btn(2));
     if(joy1Btn(2)){
        wait1Msec(5);
        clawPos += 1;
        if(clawPos > 255)
          clawPos = 255;
        servo[clawservo] = clawPos;
     }else if(joy1Btn(3)){
        wait1Msec(5);
        clawPos -= 1;
        if(clawPos < 0)
          clawPos = 0;
        servo[clawservo] = clawPos;
     }

     btn1WP = btn1IP;
     btn4WP = btn4IP;
  }
}
