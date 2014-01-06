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

#include "JoystickDriver.c"
#include "hitechnic-gyro.h"

void initializeRobot();
float findIR(int iSpeed);
float inToTicks(float dist);
float ticksToIn(float ticks);
void placeBlock();
void move(int iSpeed, float dist);
void movet(int iSpeed, int nTicks);
void goOnRamp();
void dump();
void turn(int iSpeed, int iDegrees, int code);
int wait = 0;

const float D = 58;

const long   gcSyncInterval  = 100; //Defines the time interval between each check for collision and PID correction
const long   gcSyncTickError = 20;  //Defines how far the motors can be "off" before needing correction 50 ticks = ~12 degrees
const long   gcMotorStop = 10;
const long syncTime = 100;
const long tickError = 10;

bool atPosition = false;

const int R_CODE = 3;
const int L_CODE = 4;
const int RIGHT_IR_BORDER = 6;

int lastPos = 0;
task SearchTask() {
	while(!atPosition) {
		nxtDisplayString(2,"%i",SensorValue[ir]);
		//PlayTone(SensorValue[ir]*100,100);
		if(SensorValue[ir] == RIGHT_IR_BORDER) {
			atPosition = true;
		}
		lastPos = SensorValue[ir];
		EndTimeSlice();
	}
}

task main()
{
	bDisplayDiagnostics = false;
	initializeRobot();
	waitForStart();

  servoChangeRate[sWrist] = 0;
  waitForStart();

    servo[sWrist] = 125;
	motor[mLeftArm] = motor[mRightArm] = 50;
	wait1Msec(1100);
	motor[mLeftArm] = motor[mRightArm] = 0;
	servo[sIr] = 220;
	wait1Msec(1000);
	StartTask(SearchTask);
	wait1Msec(500);

	float distance = findIR(50);

	wait1Msec(1000);
	if(distance < D) {
		placeBlock();
    	move(50, D-distance);
	}
	goOnRamp();
	while(true){

	servo[sIr] = 128;
	}
}

void initializeRobot() {
	servo[sIr] = 128;
	servo[sWrist] = 0;
}

void placeBlock(){
	turn(55,74,R_CODE);
	dump();
	turn(55,68,L_CODE);
}

void dump() {
	servo[sWrist] = 100;
	wait1Msec(500);

	motor[mLeftArm] = motor[mRightArm] = -30;
	wait1Msec(200);
	motor[mLeftArm] = motor[mRightArm] = 0;
	wait1Msec(500);

	servo[sWrist] = 255;
	wait1Msec(150);

	motor[mLeftArm] = motor[mRightArm] = 50;
	wait1Msec(400);
	motor[mLeftArm] = motor[mRightArm] = 0;
	wait1Msec(500);

	servo[sWrist] = 128;
}

void goOnRamp() {
	motor[mLeftArm] = motor[mRightArm] = -30;
	wait1Msec(200);
	motor[mLeftArm] = motor[mRightArm] = 0;

	turn(55,60,R_CODE);
	move(60,42);
	turn(55,100,R_CODE);
	move(60,40);
	//PlaySoundFile("zeld_ow_001.rso");
}


void turn(int iSpeed, int iDegrees, int code)
{
	float vcurrposition = 0;
	int   vprevtime     = nPgmTime;
	int   vcurrtime;
	float vcurrRate;
	int   voffset;
	float deltaSecs;
	float degChange;

	if (code == L_CODE)
	{
		motor[mLeft]  = -iSpeed;
		motor[mRight] = iSpeed;
	}
	else if(code == R_CODE)
	{
		motor[mLeft]  = iSpeed;
		motor[mRight] = -iSpeed;
	}

	while (abs(vcurrposition) < iDegrees)
	{
		// This tells us the current rate of rotation in degrees per
		// second.
		vcurrRate = HTGYROreadRot(gyro);

		// How much time has elapsed since we last checked, which we use
		// to determine how far we've turned
		vcurrtime = nPgmTime;

		deltaSecs = (vcurrtime - vprevtime) / 1000.0;
		if (deltaSecs < 0)
		{
			deltaSecs = ((float)((vcurrtime + 1024) - (vprevtime + 1024))) / 1000.0;
		}

		// Calculate how many degrees the heading changed.
		degChange = vcurrRate * deltaSecs;
		vcurrposition = vcurrposition + degChange;
		vprevtime = vcurrtime;
	}

	motor[mLeft]  = 0;
	motor[mRight] = 0;

	wait1Msec(100);
}

void move(int iSpeed, float dist) {
	movet(iSpeed, abs((dist-1.75)*1000.0/13.0));
}

void movet(int iSpeed, int nTicks)
{
	//Set up variables
	//For the encoder tic we multiply the incoming distance by the calculated number of tics when
	//traveling one inch. To get the calculated value you must figure out the circumference of the
	//wheel, tread, etc. There are 1440 tics in a single revolution of the motor
	int  vEncoderTic   = nTicks;
	int  vPrevLeftPos  = 0;
	int  vPrevRightPos = 0;
	int  vLeftPower    = iSpeed;
	int  vRightPower   = iSpeed;
	int  vSpeed        = iSpeed;
	bool end = false;

	//Reset our encoders prior to movement
	nMotorEncoder[mLeft]  = 0;
	nMotorEncoder[mRight] = 0;

	//Start up the motors
	motor[mLeft]  = vLeftPower;
	motor[mRight] = vRightPower;

	unsigned long vNxtSyncTime  = nPgmTime + gcSyncInterval+200;

	//Loop until both motors have traveled the required distance
	while((abs(nMotorEncoder[mLeft]) < vEncoderTic || abs(nMotorEncoder[mRight]) < vEncoderTic) && !end)
	{
		//Determine the current value of the encoders
		int vCurrLeftPos  = abs(nMotorEncoder[mLeft]);
		int vCurrRightPos = abs(nMotorEncoder[mRight]);

		//We only perform error correction at specific intervals
		if (nPgmTime >= vNxtSyncTime)
		{
			if(/*nPgmTime - vStartTime > 500 && */abs(vCurrLeftPos - vPrevLeftPos) < gcMotorStop && motor[mLeft] != 0)
				end = true;

			if(/*nPgmTime - vStartTime > 500 && */abs(vCurrRightPos - vPrevRightPos) < gcMotorStop && motor[mLeft] != 0)
				end = true;

			//See if we are far enough 'out of sync' to warrant speed corrections
			if (!end && abs(vCurrLeftPos - vCurrRightPos) > gcSyncTickError)
			{
				//We are out of sync. Determine which side is falling behind the other and adjust the speed
				//We default to slowing down th?e motors. The only time we speed up a motor is if it was
				//previously slowed
				if (vCurrLeftPos < vCurrRightPos)
				{
					if (vLeftPower < vSpeed)
						motor[mLeft] = (vLeftPower+=2);
					else
						motor[mRight] = (vRightPower-=2);
				}
				else
				{
					if (vRightPower < vSpeed)
						motor[mRight] = (vRightPower+=2);
					else
						motor[mLeft] = (vLeftPower-=2);
				}
			}


			//Use the interval to calculate the next sync time
			vNxtSyncTime = nPgmTime + gcSyncInterval;

			//Store the current positions in the previous position values for the next time through the loop
			vPrevLeftPos  = vCurrLeftPos;
			vPrevRightPos = vCurrRightPos;
		}

		if(abs(nMotorEncoder[mLeft]) >= vEncoderTic)
			motor[mLeft] = 0;

		if(abs(nMotorEncoder[mRight]) >= vEncoderTic)
			motor[mRight] = 0;
	}

	//We finished our movement so turn off the motors
	motor[mLeft]  = 0;
	motor[mRight] = 0;

	if(end) {
		PlaySound(soundBeepBeep);
		PlaySound(soundDownwardTones);
	}

	while(end) {

	servo[sIr] = 128;
		//	continue;
	}

	//The following line is used to pause the robot in between movements
	wait1Msec(100);
}

const float switchPoint = 22;

float findIR(int iSpeed) {
	nMotorEncoder[mLeft] = nMotorEncoder[mRight] = 0;
	int vCurrLeftPos   = 0;
	int vCurrRightPos  = 0;
	int vEncoderTic    = abs(inToTicks(D));
	int  vLeftPower    = iSpeed;
	int  vRightPower   = iSpeed;
	int  vSpeed        = iSpeed;

	unsigned long vNxtSyncTime  = nPgmTime + syncTime;

	//Reset our encoders prior to movement
	nMotorEncoder[mLeft]  = 0;
	nMotorEncoder[mRight] = 0;

	//Start up the motors
	motor[mLeft]  = vLeftPower;
	motor[mRight] = vRightPower;

	//Loop until both motors have traveled the required distance
	while((abs(nMotorEncoder[mLeft]) < vEncoderTic || abs(nMotorEncoder[mRight]) < vEncoderTic) && !atPosition)
	{
		//Determine the current value of the encoders
		vCurrLeftPos  = abs(nMotorEncoder[mLeft]);
		vCurrRightPos = abs(nMotorEncoder[mRight]);

		nxtDisplayString(0,"%i",abs(vCurrLeftPos-vCurrRightPos));

		if(ticksToIn((vCurrLeftPos+vCurrRightPos)/2) >= switchPoint)
			servo[sIr] = 210;

		//We only perform error correction at specific intervals
		if (nPgmTime >= vNxtSyncTime)
		{
			//See if we are far enough 'out of sync' to warrant speed corrections
			if (abs(vCurrLeftPos - vCurrRightPos) > tickError)
			{
				//We are out of sync. Determine which side is falling behind the other and adjust the speed
				//We default to slowing down the motors. The only time we speed up a motor is if it was
				//previously slowed
				if (vCurrLeftPos < vCurrRightPos)
				{
					if (vLeftPower < vSpeed)
						motor[mLeft] = (vLeftPower+=2);
					else
						motor[mRight] = (vRightPower-=2);
				}
				else
				{
					if (vRightPower < vSpeed)
						motor[mRight] = (vRightPower+=2);
					else
						motor[mLeft] = (vLeftPower-=2);
				}
			}

			if(abs(nMotorEncoder[mRight]) >= vEncoderTic)
				motor[mRight] = 0;

			if(abs(nMotorEncoder[mLeft]) >= vEncoderTic)
				motor[mLeft] = 0;

			//Use the interval to calculate the next sync time
			vNxtSyncTime = nPgmTime + syncTime;

			//Store the current positions in the previous position values for the next time through the loop
			//vPrevLeftPos  = vCurrLeftPos;
			//vPrevRightPos = vCurrRightPos;
		}
	}

	//We finished our movement so turn off the motors
	motor[mLeft]  = 0;
	motor[mRight] = 0;
	return ticksToIn((abs(nMotorEncoder[mLeft])+abs(nMotorEncoder[mRight]))/2);
}

float inToTicks(float dist) {
	 return (dist-1.75)*1000.0/13.0;
}

float ticksToIn(float ticks) {
	return 13.0*ticks/1000.0;
}
