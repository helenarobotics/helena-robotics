//moveRobot moves the robot forward
//rotateRobot rotates the robot
//	45 is 45 degrees to the left
// -45 is 45 degrees to the right
//getAngleToBeacon returns the angle relative to the robot's left

void moveRobot(float dist);
void rotateRobot(float degrees);
float getAngleToBeacon();

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

	}else if(firstAngle < 180){
		thirdAngle = secondAngle - firstAngle;
		r = (distance * sinDegrees(180-firstAngle))/(sinDegrees(thirdAngle));
		float right = -cosDegrees(secondAngle)*r; //how far right of the ir beacon
		float down = sinDegrees(secondAngle)*r; //how far 'below' the ir beacon
	  float leftMov = right - desiredX;
	  float upMov = down - desiredY;
	  float movDist = pow(leftMov, 2) + pow(upMov, 2);
	  movDist = pow(movDist, 0.5);
	  float movAngle = 90-radiansToDegrees(atan((upMov)/(leftMov)));
	  rotateRobot(movAngle);
	  moveRobot(movDist);
	  rotateRobot(45-movAngle);
	}
}

void moveRobot(float dist){

}

void rotateRobot(float degrees){

}

float getAngleToBeacon(){

}
