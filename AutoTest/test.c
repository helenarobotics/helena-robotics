#pragma config(Hubs,  S1, HTMotor,  none,     none,     none)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Sensor, S2,     compass,        sensorHiTechnicIRSeeker1200)
#pragma config(Sensor, S3,     light,          sensorLightActive)
#pragma config(Sensor, S4,     sonar,          sensorSONAR)
#pragma config(Motor,  mtr_S1_C1_1,     motorD,        tmotorTetrix, PIDControl, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     motorE,        tmotorTetrix, PIDControl, reversed, encoder)

//moveRobot moves the robot forward
//rotateRobot rotates the robot
//	45 is 45 degrees to the left
// -45 is 45 degrees to the right
//getAngleToBeacon returns the angle relative to the robot's left

void moveRobot(float distance);
void rotateRobot(float angle);
float getAngleToBeacon();
float getAngleToBeaconB();
float firstAngle = 0;
float secondAngle = 0;
float thirdAngle = 0;
float r = 0;
task main()
{
	const int distance = 1;
	const float desiredX = 1*cosDegrees(45);
	const float desiredY = 1*sinDegrees(45);
	moveRobot(distance);
	firstAngle = -getAngleToBeaconB()+90;
	moveRobot(distance);
	secondAngle = -getAngleToBeaconB()+90;
	if(firstAngle < 90){
		thirdAngle = firstAngle - secondAngle;
		r = (distance * sinDegrees(180-firstAngle))/(sin(thirdAngle));
		float right = -cosDegrees(secondAngle)*r; //how far right of the ir beacon
		float down = sinDegrees(secondAngle)*r; //how far 'below' the ir beacon
	  float leftMov = right - desiredX;
	  float upMov = down - desiredY;
	  float movDist = pow(leftMov, 2) + pow(upMov, 2);
	  movDist = pow(movDist, 0.5);
	  float movAngle = atan((upMov)/(-leftMov)) - 90;
	  rotateRobot(movAngle);
	  moveRobot(movDist);
	  rotateRobot(45-movAngle);
	}else if(firstAngle < 180){
		thirdAngle = secondAngle - firstAngle;
		r = (distance * sinDegrees(180-firstAngle))/(sinDegrees(thirdAngle));
		float right = -cosDegrees(secondAngle)*r; //how far right of the ir beacon
		float down = sinDegrees(secondAngle)*r; //how far 'below' the ir beacon
	  float leftMov = right - desiredX;
	  float upMov = down - desiredY;
	  float movDist = pow(leftMov, 2) + pow(upMov, 2);
	  movDist = pow(movDist, 0.5);
	  nxtDisplayString(2, "%f", movDist);
	  nxtDisplayString(1, "(%f, %f)",right, down);
	  float movAngle = 90-radiansToDegrees(atan((upMov)/(leftMov)));
	  nxtDisplayString(0, "%f",movAngle);
	  rotateRobot(movAngle);
	  moveRobot(movDist);
	  rotateRobot(45-movAngle);
	}
}

const int FORWARD_POWER = 35;
const int MOVE_FOOT_TIME = 2000;
void moveRobot(float distance){
    if (distance == 0)
        return;

    // We need to wait now() + the amount of time to move one foot
    int endTime = nPgmTime + abs(distance) * MOVE_FOOT_TIME;

    // Turn on the motors and wait for the appropriate amount of time for the movement to occur
    if (distance > 0) {
        motor[motorD] = FORWARD_POWER;
        motor[motorE] = FORWARD_POWER;
    } else {
        motor[motorD] = -FORWARD_POWER;
        motor[motorE] = -FORWARD_POWER;
    }
    while (nPgmTime < endTime)
        EndTimeSlice();

    motor[motorD] = 0;
    motor[motorE] = 0;
}

const int TURN_POWER = 30;
const int COMPLETE_REVOLUTION_TIME = 10000;
void rotateRobot(float angle){
	    if (angle == 0)
        return;

    // How long it should take
    int endTime = nPgmTime + (abs(angle) / 360.0) * COMPLETE_REVOLUTION_TIME;

    // Turn on the motors and wait for the appropriate amount of time for the movement to occur
    if (angle < 0) {
        motor[motorD] = -TURN_POWER;
        motor[motorE] = TURN_POWER;
    } else {
        motor[motorD] = TURN_POWER;
        motor[motorE] = -TURN_POWER;
    }
    while (nPgmTime < endTime)
        EndTimeSlice();

    motor[motorD] = 0;
    motor[motorE] = 0;
}

float getAngleToBeacon(){
    // Sweep to get 3 transitions, then go back.
    int startTime = nPgmTime;
	int curSection = SensorValue[S2];
    if(curSection <= 5) { //Go Left
    		float angleTotal = 0.0;
    		int measurements = 0;
    		motor[motorD] = TURN_POWER;
    		motor[motorE] = -TURN_POWER;
			for(int i = 0; i < 3; i++){
			while(curSection == SensorValue[S2])
				EndTimeSlice();
			curSection = SensorValue[S2];
			float centerOffset = 30*(curSection-5) - 15;
			int elapsed = nPgmTime - startTime;
			angleTotal += (float)elapsed/(float)COMPLETE_REVOLUTION_TIME * 360.0 - centerOffset;
			nxtDisplayString(6, "%d @ %i = %f", centerOffset, curSection, angleTotal);
			measurements++;
			}
			int elapsedTime = nPgmTime - startTime;
			motor[motorD] = -TURN_POWER;
			motor[motorE] = TURN_POWER;
			startTime = nPgmTime;
			while(nPgmTime < startTime + elapsedTime)
				endTimeSlice();
			motor[motorD] = 0;
			motor[motorE] = 0;

			return angleTotal/(float)measurements + 90;
  	} else { //Go Left

  	}
  	return 0.0;
}

float getAngleToBeaconB()
{
	int numSweeps = 1;
    int totalAngleValue = 0;

    // Start sweeping
    for (int i = 0; i < numSweeps; i++) {
        // Sweep left 90 degrees
        int startTime = nPgmTime;
        motor[motorD] = TURN_POWER;
        motor[motorE] = -TURN_POWER;

        // Wait for an IR sensor hit
        int prevIrSection = SensorValue[S2];
        int leftAngle = 0;
        while (leftAngle == 0) {
            // Sweeping left, we calculate the angle the first time we read
            // the transition from section 5->6 on the IR sensor
            int irSection = SensorValue[S2];
            if (leftAngle == 0 && prevIrSection == 5 && irSection == 6) {
                // Approximate based on time
                float sweepTime = nPgmTime - startTime;
                leftAngle = -sweepTime/COMPLETE_REVOLUTION_TIME* 360.0;
            }
            prevIrSection = irSection;
            EndTimeSlice();
        }
		int lTime = nPgmTime - startTime;
		float lRot = (float)lTime/COMPLETE_REVOLUTION_TIME *360.0;
        // Sweep the other way
        startTime = nPgmTime;
        motor[motorD] = -TURN_POWER;
        motor[motorE] = TURN_POWER;
        int rightAngle = 0;
        while (rightAngle == 0) {
            // Sweeping right, we calculate the angle the first time we read
            // the transaction from section 5 -> 4 on the IR sensor
            int irSection = SensorValue[S2];
            if (rightAngle == 0 && prevIrSection == 5 && irSection == 4) {
                // Approximate based on time
                float sweepTime = nPgmTime - startTime;
                rightAngle = sweepTime/COMPLETE_REVOLUTION_TIME *360.0 - lRot;
            }
            prevIrSection = irSection;
            EndTimeSlice();
        }
		int rTime = nPgmTime - startTime;
        // Return back to starting position
		startTime = nPgmTime;
		if(rTime > lTime){
	        motor[motorD] = TURN_POWER;
	        motor[motorE] = -TURN_POWER;
	        while (startTime+(lTime-rTime) > nPgmTime)
	            EndTimeSlice();			
		}else{
	        motor[motorD] = -TURN_POWER;
	        motor[motorE] = TURN_POWER;
	        while (startTime+(lTime-rTime) > nPgmTime)
	            EndTimeSlice();
    	}

        // Done, turn off the motors
        motor[motorD] = 0;
        motor[motorE] = 0;

        // Average angle is the addition of the left and right angles
        totalAngleValue += (leftAngle + rightAngle) / 2;
    }

    // Return angle relative to the robot
    float averageAngle = totalAngleValue / numSweeps;
    return averageAngle;
}