#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTServo,  HTMotor)
#pragma config(Sensor, S2,     gyro,           sensorI2CHiTechnicGyro)
#pragma config(Sensor, S3,     ir,             sensorHiTechnicIRSeeker1200)
#pragma config(Motor,  mtr_S1_C1_1,     mRight,         tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C1_2,     mRightArm,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,     mLeft,     tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_2,     mLeftArm,      tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C4_1,     mLift,         tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C4_2,     mFlag,        tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S1_C3_1,    sIr,                  tServoNone)
#pragma config(Servo,  srvo_S1_C3_2,    sWrist,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_6,    servo6,               tServoNone)

#include "JoystickDriver.c";

task main()
{
	waitForStart();

	motor[mRight] = -50;
	motor[mLeft] = -50;

	wait1Msec(300000);

	motor[mRight] = 0;
	motor[mLeft] = 0;
}
