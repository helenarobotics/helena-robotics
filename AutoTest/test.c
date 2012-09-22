#pragma config(Hubs,  S1, HTMotor,  HTServo,  none,     none)
#pragma config(Sensor, S1,     snIR,           sensorNone)
#pragma config(Motor,  mtr_S1_C1_1,     motorD,        tmotorNone, openLoop)
#pragma config(Motor,  mtr_S1_C1_2,     motorE,        tmotorNone, openLoop)
#pragma config(Servo,  srvo_S1_C2_1,    svIR,                 tServoStandard)
#pragma config(Servo,  srvo_S1_C2_2,    servo2,               tServoNone)
#pragma config(Servo,  srvo_S1_C2_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S1_C2_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C2_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C2_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
void moveRobot(float ftRight,float ftForward);
void rotateRobot(float angle);
void putRing();
float findIRBeacon(int nSweeps);

//distance to stop from peg
const float PEG_DIST;
const float MOVE_DIST;

task main()
{
	//Find the IR Beacon
	float firstAngle = findIRBeacon(2);
	//MOVE ROBOT FW 2 FT
	moveRobot(0,MOVE_DIST);
	float secondAngle = findIRBeacon(2);

	if(firstAngle < 90){
		//Create a vector of secondAngle and r
		float r = (MOVE_DIST*sin(firstAngle))/(sin(firstAngle - secondAngle));
		//Find how far to the right and how far up you have to move
		//NOTE: may want to use cosDegrees if using degrees
		float right = cos(secondAngle)*r;
		float up = sin(secondAngle)*r;
		if(right > up){
			//Move right right-up
			moveRobot(right-up,0);

		}else if(right < up){
			//Move down up-right
			moveRobot(0,up-right);
		}
		//Rotate 45 degrees ccw
		rotateRobot(-45);
		//Move robot forward to an amount before the post
		moveRobot(0,sqrt(2)*up-PEG_DIST);
		putRing();
	}else if(firstAngle == 90){
		//Somehow, the robot is already facting the beacon
		//I have no idea how far to move
		//This SHOULD not happen
	}else{
		//Create a vector of secondAngle and r
		float r = (MOVE_DIST*sin(firstAngle))/(sin(secondAngle-firstAngle));
		//Find how far to the right and how far up you have to move
		//NOTE: may want to use cosDegrees if using degrees
		float left = -cos(secondAngle)*r;
		float up = sin(secondAngle)*r;
		//Move to be at a 45 degree angle to the peg - it may be better
		//to move away from the peg to line up, in case you get too close
		if(left > up){
			//Move left left-up
			moveRobot(up-left,0);

		}else if(left < up){
			//move up up-left
			moveRobot(0,up-left);
		}
		//Rotate 45 degrees cw
		rotateRobot(45);
		//Move robot forward to an amount before the post
		moveRobot(0,sqrt(2)*up-PEG_DIST);
		putRing();
	}
}

//IR seeking constants
//The value above which a signal is 'detected'
const int IR_THRES;

//The angle that the servo covers
const float ANGLE_RANGE;

//The angle of the minimum retlative to the robot
const float ANGLE_ROBOT;

//Servos typically updates every 20ms
const int UPDATE_INTERVAL = 20;

const int MIN_POS = servoMinPos[svIR];
const int MAX_POS = servoMaxPos[svIR];
//const int NEU_POS = (MAX_POS+MIN_POS)/2; NOT USED
const int RANGE = MAX_POS-MIN_POS;

//percent of total range the serco moves in 1 second.
const float CHG_RATE;
//the definition for the servo change rate
const int CHG_RATE_VALUE = CHG_RATE * RANGE / UPDATE_INTERVAL;

float findIRBeacon(int nSweeps)
{
	servoChangeRate[svIR] = CHG_RATE_VALUE;

	//determine the theoretical amount of time to wait, before we tell the servo to move
	int waitTime = abs(ServoValue[svIR]-MIN_POS)/(CHG_RATE/1000);
	//set servo to minimum position;
	servo[svIR] = MIN_POS;
	//wait the theoretical amount of time it should take the servo to move.
	wait1Msec(waitTime);

	int totalServoValue = 0;
	int nValues = 0;
	//Start sweeping
	for(int i = 0; i < nSweeps; i++){
		int timeOffset = 1/(CHG_RATE/1000); //probably could be a constant

		//From min to max
		int startTime = nPgmTime;
		servo[svIR] = MAX_POS;
		while(nPgmTime <= timeOffset+startTime){
			//Try to detect a hit
			if(SensorValue[snIR] >= IR_THRES){
				totalServoValue += MIN_POS+(nPgmTime-startTime)*CHG_RATE/1000;//Approximate based on time;
				nValues++;
			}
		}

		//From max to min
		startTime = nPgmTime;
		servo[svIR] = MIN_POS;
		while(nPgmTime < timeOffset+startTime){
			//Try to detect a hit
			if(SensorValue[snIR] >= IR_THRES){
				totalServoValue+= MAX_POS-(nPgmTime-startTime)*CHG_RATE/1000;//Approximate based on time;
				nValues++;
			}
		}
	}
	float averageValue = totalServoValue/nValues;
	float percentAt = (averageValue-MIN_POS)/(MAX_POS-MIN_POS);
	return percentAt*ANGLE_RANGE+ANGLE_ROBOT; //Return angle relative to the robot
}

//Move robot, but retain direction
void moveRobot(float ftRight, float ftForward){

}

//Rotate robot, but retain position
void rotateRobot(float angle){

}
//Put ring on peg
void putRing(){

}
