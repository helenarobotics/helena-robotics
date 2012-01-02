#pragma config(Hubs,  S1, HTMotor,  HTMotor,  none,     none)
#pragma config(Sensor, S1,     ,                    sensorI2CMuxController)
#pragma config(Motor,  mtr_S1_C1_1,     mConvey,       tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C1_2,     mBend,         tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,     mLeft,         tmotorNormal, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C2_2,     mRight,        tmotorNormal, openLoop)

#include "JoystickDriver.c"

void initializeRobot();
void getVars();
void getDrivePow();
void move();
void setBtnPressed();

bool btn1WP = false, btn5WP = false, btn6WP = false, raising = false, reverse = false;
int lPow = 0, rPow = 0, percPow = 100;
const int CHOOSE_DRIVE = 0;
const int TANK_DRIVE = 1;
const int ARCADE_DRIVE = 2;
int DRIVE_MODE = CHOOSE_DRIVE;

//task BeepTask();

task main(){
    initializeRobot();

    //waitForStart();

    while(true){
        getJoystickSettings(joystick); //Get joystick settings
        if(DRIVE_MODE == CHOOSE_DRIVE){
            if(joy1Btn(2))
                DRIVE_MODE = TANK_DRIVE;
            else if(joy1Btn(4))
                DRIVE_MODE = ARCADE_DRIVE;
        }else{
            nxtDisplayString(0,"Chosen");
            getVars();
            getDrivePow();
            move();
            setBtnPressed();
        }
    }
}

void getVars(){
    if(joy1Btn(5) && !btn5WP){
        if(percPow > 20)
            percPow -= 20;
    }

    else if(joy1Btn(6) && !btn6WP){
        if(percPow < 100)
            percPow += 20;
    }

    if(joy1Btn(1) && !btn1WP)
        reverse = !reverse;
}

void getDrivePow(){
    if(DRIVE_MODE == TANK_DRIVE){
        if(abs(joystick.joy1_y1) <= 10) //If it is in a small range
            lPow = 0; // Don't move
        else // Otherwise, move proportionally
            lPow = joystick.joy1_y1 / 1.28;

        if(abs(joystick.joy1_y1) <= 10) //If it is in a small range
            rPow = 0; // Don't move
        else // Otherwise, move proportionally
            rPow = joystick.joy1_y2 / 1.28;
    }else if(DRIVE_MODE == ARCADE_DRIVE){
        int nSpeedPower = 0;
        int nTurnPower = 0;
        if(abs(joystick.joy1_y1) > 10)
            nSpeedPower = joystick.joy1_y1 / 1.28;
        if(abs(joystick.joy1_x2) > 10)
            nTurnPower = joystick.joy1_x2 / 1.28;

        // Power and speed
        rPow = nSpeedPower - nTurnPower;
        lPow = nSpeedPower + nTurnPower;

        if(abs(nTurnPower) > 0 && abs(nSpeedPower) > 0){
            rPow/=2;
            lPow/=2;
        }
    }
    rPow = rPow * percPow/100;
    lPow = lPow * percPow/100;

    if(reverse){
        int t = lPow;
        lPow = -rPow;
        rPow = -t;
    }
}

void move(){
    motor[mLeft] = lPow; //Set the powers to the moters
    motor[mRight] = rPow;

    if(joy1Btn(6)) //While holding move
        motor[mConvey] = 100;
    else //Otherwise don't
        motor[mConvey] = 0;

    if(joy2Btn(5))
        motor[mBend] = 100;
    else if(joy2Btn(6))
        motor[mBend] = -100;
    else
        motor[mBend] = 0;

    if(joy2Btn(9) && joy2Btn(10) && !raising){ //Only pull pin once and when 9 & 10 are pressed
        raising = true;
        //servo[servoLift] = 100;
    }
}

void setBtnPressed(){
    btn1WP = joy1Btn(1);
    btn5WP = joy1Btn(5);
    btn6WP = joy1Btn(6);
}

void initializeRobot(){
    //servo[servoLift] = 0;
    //StartTask(BeepTask);
}

/*task BeepTask(){
    while(true){
        int frequency = 150;
        for(int i = 0; i < percPow; i+=20){
            frequency+=250;
        }

        PlayTone(frequency, 50);

		if(reverse)
		    PlaySound(soundDownwardTones);
		else
			PlaySound(soundUpwardTones);

	    wait1Msec(700);
	}
}*/
