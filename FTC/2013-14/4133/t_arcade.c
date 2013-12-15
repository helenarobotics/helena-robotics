#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTServo,  HTMotor)
#pragma config(Sensor, S2,     gyro,           sensorI2CHiTechnicGyro)
#pragma config(Sensor, S3,     ir,             sensorHiTechnicIRSeeker1200)
#pragma config(Motor,  mtr_S1_C1_1,     mRight,         tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C1_2,     mLeft,        tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C2_1,     mRightArm,     tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_2,     mLeftArm,      tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C4_1,     mLift,         tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C4_2,     mFlag,        tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S1_C3_1,    sIr,                  tServoNone)
#pragma config(Servo,  srvo_S1_C3_2,    sWrist,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "JoystickDriver.c";

void tankDrive();
void arcadeDrive();
void lift();
void turnFlag();
void moveArm();
void moveWrist();
void changeSpeeds();
bool isPressed1(int button);
bool isPressed2(int button);
void savePrevState();

int factor = 3;
//Factor divided by 3 - Makes "gears" for the robot speed
//	1 - 1/3 "low" speed
//  2 - 2/3 "medium" speed
//  3 - 3/3 "high" speed

bool prev1 [12] = {false, false, false, false,
				   false, false, false, false,
				   false, false, false, false};
bool prev2 [12] = {false, false, false, false,
				   false, false, false, false,
				   false, false, false, false};

int mPsP = 3;
unsigned long moveTime;
void initializeRobot() {
	servo[sWrist] = 0;
	servoChangeRate[sWrist] = 0;
	//ervo[sIr] = 0;
	//servo[servo3] = 0;
	///servo[servo4] = 0;
	//servo[servo5] = 0;
	//servo[servo6] = 0;
	moveTime = nPgmTime;

	servo[sIr] = 128;
}

task main()
{
	initializeRobot();
	waitForStart();
	while(true) {
		getJoystickSettings(joystick);

		//tankDrive();
		arcadeDrive();

		changeSpeeds();


		moveArm();
		moveWrist();

		lift();
		turnFlag();

		savePrevState();
	}
}


//Drive the motors for the drive base on the robot
//	Control by Joystick 1
// 	Left analog to left motor and right analog to right mo
void tankDrive() {
	float lPower = joystick.joy1_y1/1.28*factor/3.00;
	float rPower = joystick.joy1_y2/1.28*factor/3.00;
	motor[mLeft] = lPower;
	motor[mRight] = rPower;
}

void arcadeDrive() {
	float turning = joystick.joy1_x2/1.28*factor/3.00;
	float forward = joystick.joy1_y1/1.28*factor/3.00;
	motor[mLeft] = forward+turning;
	motor[mRight] = forward-turning;
}

void changeSpeeds() {
	if(isPressed1(1))
		factor = 1.5;
	else if(isPressed1(2))
		factor = 2;
	else if(isPressed1(3))
		factor = 3;
}


static const int THRESHOLD = 15;
void moveArm() {
	float power = joystick.joy2_y1/1.28;
	if(abs(power) < THRESHOLD)
		power = 0;
	if(!joy2Btn(5)) {
		// If moving down, reduce the power to slow down the arm (gravity)
		if(joystick.joy2_y1 < 0)
			power*=0.1;
	}
	motor[mLeftArm] =  motor[mRightArm] = power;
}

int topPos = 50;
int btmPos = 130;

void moveWrist() {
	if(moveTime < nPgmTime) {
		if(joy2Btn(3)) {
			servo[sWrist] = servo[sWrist]-1;
		} else if(joy2Btn(2)) {
			servo[sWrist] = servo[sWrist]+1;
		} else if(joy2Btn(1)) {
			servo[sWrist] = btmPos;
		} else if(joy2Btn(4)) {
			servo[sWrist] = topPos;
		}
		moveTime = nPgmTime+mPsP;
	}
}


//Saves the state for use in the next loop
void savePrevState() {
	for(int i = 0; i < 12; i++) {
		prev1[i] = (bool)joy1Btn(i+1);
		prev2[i] = (bool)joy2Btn(i+1);
	}
}

void lift() {
	motor[mLift] = (joy2Btn(6)? 100 : (joy2Btn(8)? -100 : 0));
}

void turnFlag() {
	motor[mFlag] = (joy1Btn(6)? 100 : (joy1Btn(8)? -100 : 0));
}

//Returns true if the button was "just" pressed
bool isPressed1(int button) {
	return prev1[button-1] && joy1Btn(button);
}

bool isPressed2(int button) {
	return prev2[button-1] && joy2Btn(button);
}
