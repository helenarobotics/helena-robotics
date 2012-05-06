#pragma config(Hubs,  S1, HTServo,  HTMotor,  HTMotor,  HTMotor)
#pragma config(Sensor, S1,     ,                    sensorI2CMuxController)
#pragma config(Sensor, S2,     irSeeker,            sensorHiTechnicIRSeeker1200)
#pragma config(Sensor, S3,     gyro,                sensorI2CHiTechnicGyro)
#pragma config(Motor,  motorA,          ,              tmotorNormal, openLoop)
#pragma config(Motor,  motorB,          ,              tmotorNormal, openLoop)
#pragma config(Motor,  motorC,          ,              tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,     ballMotor,     tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C2_2,     basketTurnMotor, tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C3_1,     leftLifterMotor, tmotorNormal, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C3_2,     rightLifterMotor, tmotorNormal, openLoop)
#pragma config(Motor,  mtr_S1_C4_1,     DriveRight,    tmotorNormal, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C4_2,     DriveLeft,     tmotorNormal, openLoop, reversed, encoder)
#pragma config(Servo,  srvo_S1_C1_1,    flagServo,            tServoStandard)
#pragma config(Servo,  srvo_S1_C1_2,    rightGripperServo,    tServoStandard)
#pragma config(Servo,  srvo_S1_C1_3,    leftGripperServo,     tServoStandard)
#pragma config(Servo,  srvo_S1_C1_4,    TrightGripperServo,   tServoStandard)
#pragma config(Servo,  srvo_S1_C1_5,    TleftGripperServo,    tServoStandard)
#pragma config(Servo,  srvo_S1_C1_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                           Autonomous Mode Code Template
//
// This file contains a template for simplified creation of an autonomous program for an TETRIX robot
// competition.
//
// You need to customize two functions with code unique to your specific robot.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

#include "JoystickDriver.c"  //Include file to "handle" the Bluetooth messages.
#include "drivers/HTGyro-driver.h"

const int  vtile          = 2750; //based off of 4 in. diameter wheel
const int  vThreshHold    = 5;
const long vSyncInterval  = 250;
const long vSyncTickError = 50;

int    gReady = 0;
string gRun   = "ParkBallFront";
string gColor = "red";
string gPos   = "in";

typedef enum
  {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
  } cmdState;

typedef enum
  {
    OPENFULL,
    OPENFORWARD,
    CLOSED,
    CRATES
  } gripperState;

task ButtonTask()
  {
    nSchedulePriority = kHighPriority;

    switch(nNxtButtonPressed)
      {
        case kLeftButton:
          if (gReady == 0)
            {
              if (gRun == "ParkBallFront")
                {
                  gRun = "placecratesfar";
                }
              else if (gRun == "placecratesfar")
                {
                  gRun = "placecrates";
                }
              else if (gRun == "placecrates")
                {
                  gRun = "ParkBack";
                }
              else if (gRun == "ParkBack")
                {
                  gRun = "SpeedFront";
                }
              else if (gRun == "SpeedFront")
                {
                  gRun = "SpeedBack";
                }
              else if (gRun == "SpeedBack")
                {
                  gRun = "DefenseRamp";
                }
              else if (gRun == "DefenseRamp")
                {
                  gRun = "ParkBallFront";
                }

              nxtDisplayTextLine(3, gRun);
            }
          else if (gReady == 1)
            {
              if (gColor == "red")
                {
                  gColor = "blue";
                }
              else
                {
                  gColor = "red";
                }

              nxtDisplayTextLine(5, "Color: %s", gColor);
            }
          else if (gReady == 2)
            {
              if (gPos == "in")
                {
                  gPos = "out";
                }
              else
                {
                  gPos = "in";
                }

              nxtDisplayTextLine(6, "Position: %s", gPos);
            }

          break;
        case kRightButton:
          if (gReady == 0)
            {
              if (gRun == "ParkBallFront")
                {
                  gRun = "DefenseRamp";
                }
              else if (gRun == "DefenseRamp")
                {
                  gRun = "SpeedBack";
                }
              else if (gRun == "SpeedBack")
                {
                  gRun = "SpeedFront";
                }
              else if (gRun == "SpeedFront")
                {
                  gRun = "ParkBack";
                }
              else if (gRun == "ParkBack")
                {
                  gRun = "placecrates";
                }
              else if (gRun == "placecrates")
                {
                  gRun = "placecratesfar";
                }
              else if (gRun == "placecratesfar")
                {
                  gRun = "ParkBallFront";
                }

              nxtDisplayTextLine(3, gRun);
            }
          else if (gReady == 1)
            {
              if (gColor == "red")
                {
                  gColor = "blue";
                }
              else
                {
                  gColor = "red";
                }

              nxtDisplayTextLine(5, "Color: %s", gColor);
            }
          else if (gReady == 2)
            {
              if (gPos == "in")
                {
                  gPos = "out";
                }
              else
                {
                  gPos = "in";
                }

              nxtDisplayTextLine(6, "Position: %s", gPos);
            }

          break;
        case kEnterButton:
          gReady = gReady + 1;

          if (gReady == 1)
            {
              nxtDisplayTextLine(5, "Color: %s", gColor);
            }
          else if (gReady == 2)
            {
              nxtDisplayTextLine(6, "Position: %s", gPos);
            }

          break;
        case kExitButton:
          gReady = gReady - 1;

          if (gReady < 0)
            {
              gReady = 0;
            }

          if (gReady == 0)
            {
              nxtDisplayClearTextLine(5);
            }
          else if (gReady == 1)
            {
              nxtDisplayClearTextLine(6);
            }

          break;
      }

    return;
  }


/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                    initializeRobot
//
// Prior to the start of autonomous mode, you may want to perform some initialization on your robot.
// Things that might be performed during initialization include:
//   1. Move motors and servos to a preset position.
//   2. Some sensor types take a short while to reach stable values during which time it is best that
//      robot is not moving. For example, gyro sensor needs a few seconds to obtain the background
//      "bias" value.
//
// In many cases, you may not have to add any code to this function and it will remain "empty".
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
void initializeRobot()
  {
    // Place code here to initialize servos to starting positions.
    // Sensors are automatically configured and setup by ROBOTC. They may need a brief time to stabilize.
    eraseDisplay();
    nxtDisplayCenteredTextLine(1, "Autonomous");
    nxtDisplayTextLine(3, gRun);

    nNxtButtonTask = ButtonTask;
    nNxtExitClicks = 2;

    servo[leftGripperServo] = 240;
    servo[TleftGripperServo] = 240;
    servo[rightGripperServo] = 0;
    servo[TrightGripperServo] = 0;

    while (gReady < 3)
      {
        //Do nothing. This loop puts us in a wait mode while a team member selects the program,
        //alliance color, and position of the robot
      }

    nxtDisplayCenteredTextLine(7, "READY");

    //Calibrate the gyro sensor while the robot is still
    HTGYROstartCal(gyro);
    wait1Msec(1000);
  }


/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                    move
//
// This is a generic movement routine that handles turning the motors on and off as well as
// watching encoder values to detect when movement is impaired.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
void move(int speed, int vdistance, cmdState cmd)
  {
    int  vPrevLeftPos  = 0;
    int  vPrevRightPos = 0;
    int  vCurrLeftPos  = 0;
    int  vCurrRightPos = 0;
    int  vLeftPower    = speed;
    int  vRightPower   = speed;
    int  vRightOffset  = 1;
    int  vLeftOffset   = 1;
    long vNxtSyncTime  = nPgmTime + vSyncInterval;

    //Set our motor offsets based on the supplied command
    switch (cmd)
      {
        case FORWARD:
          //To move forward both motor powers should be positive
          vRightOffset = 1;
          vLeftOffset  = 1;
          break;
        case BACKWARD:
          //To move backward both motor powers should be negative
          vRightOffset = -1;
          vLeftOffset  = -1;
          break;
        case LEFT:
          //To turn left, the left motor must run backward and the right motor forward
          vRightOffset = 1;
          vLeftOffset  = -1;
          break;
        case RIGHT:
          //To turn right, the right motor must run backward and the left motor forward
          vRightOffset = -1;
          vLeftOffset  = 1;
          break;
      }

    //Reset our encoders prior to movement
    nMotorEncoder[DriveLeft]  = 0;
    nMotorEncoder[DriveRight] = 0;

    //Start up the motors
    motor[DriveLeft]  = vLeftPower * vLeftOffset;
    motor[DriveRight] = vRightPower * vRightOffset;

    //Loop until both motors have traveled the required distance
    while(abs(nMotorEncoder[DriveLeft]) < vdistance || abs(nMotorEncoder[DriveRight]) < vdistance)
      {
        //Determine the current value of the encoders
        vCurrLeftPos  = abs(nMotorEncoder[DriveLeft]);
        vCurrRightPos = abs(nMotorEncoder[DriveRight]);

        //We only perform error correction at specific intervals
        if (nPgmTime >= vNxtSyncTime)
          {
            //See if we are far enough 'out of sync' to warrant speed corrections
            //To do this, we average the encoder values and then compare each individual motor to that average
            int avg = (vCurrLeftPos + vCurrRightPos) / 2;

            if (abs(vCurrLeftPos - avg) > vSyncTickError || abs(vCurrRightPos - avg) > vSyncTickError)
              {
                //We are out of sync. Determine which side is falling behind the other and adjust the speed
                //We default to slowing down the motors. The only time we speed up a motoro is if it was
                //previously slowed
                if (vCurrLeftPos < vCurrRightPos)
                  {
                    if (vLeftPower < speed)
                      {
                        vLeftPower       = vLeftPower + 1;
                        motor[DriveLeft] = vLeftPower * vLeftOffset;
                      }
                    else
                      {
                        vRightPower       = vRightPower - 1;
                        motor[DriveRight] = vRightPower * vRightOffset;
                      }
                  }
                else
                  {
                    if (vRightPower < speed)
                      {
                        vRightPower       = vRightPower + 1;
                        motor[DriveRight] = vRightPower * vRightOffset;
                      }
                    else
                      {
                        vLeftPower       = vLeftPower - 1;
                        motor[DriveLeft] = vLeftPower * vLeftOffset;
                      }
                  }
              }

            //Now we check to ensure we have not run into an object preventing us from moving.
            if ((vCurrLeftPos <= (vPrevLeftPos + vThreshHold)) || (vCurrRightPos <= (vPrevRightPos + vThreshHold)))
              {
                //At least one motor is stuck. Turn off motors and wait for a period of time to avoid motor burn out.
                motor[DriveLeft]  = 0;
                motor[DriveRight] = 0;

                wait1Msec(1000);

                motor[DriveLeft]  = vLeftPower * vLeftOffset;
                motor[DriveRight] = vRightPower * vRightOffset;
              }

            vNxtSyncTime = nPgmTime + vSyncInterval;

            vPrevLeftPos  = vCurrLeftPos;
            vPrevRightPos = vCurrRightPos;
          }
      }

    motor[DriveLeft]  = 0;
    motor[DriveRight] = 0;

    //The following line is used to pause the robot in between movements
    wait1Msec(100);
  }


/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                    GyroTurn
//
// This is a generic movement routine turns the robot a specific number of degrees using a gyro
// sensor to measure angular velocity
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
void GyroTurn(int vspeed, int vdegrees, cmdState cmd)
  {
    float vcurrposition = 0;
    int vprevtime = nPgmTime;
    int vcurrtime;
    int vcurrRate;
    int voffset;
    float deltaSecs;
    float degChange;

    if (cmd == LEFT)
      {
        motor[DriveLeft]  = -1*vspeed;
        motor[DriveRight] = vspeed;
        voffset = -1;
      }
    else
      {
        motor[DriveLeft]  = vspeed;
        motor[DriveRight] = -1*vspeed;
        voffset = 1;
      }

    while (vcurrposition < vdegrees)
      {
        // This tells us the current rate of rotation in degrees per
        // second.
        vcurrRate = HTGYROreadRot(gyro)*voffset;

        // How much time has elapsed since we last checked, which we use
        // to determine how far we've turned
        vcurrtime = nPgmTime;

        deltaSecs = (vcurrtime - vprevtime) / 1000.0;
        if (deltaSecs < 0)
          {
            deltaSecs = ((float)((vcurrtime + 1024) - (vprevtime + 1024))) / 1000.0;
          }

        // Calculate how many degrees the heading changed.
        degChange = (float)vcurrRate * deltaSecs;
        vcurrposition = vcurrposition + degChange;

        vprevtime = vcurrtime;
      }

    motor[DriveLeft]  = 0;
    motor[DriveRight] = 0;

    //The following line is used to pause the robot in between movements
    wait1Msec(100);
  }


/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                    findBeacon
//
// This routine uses the IR seeker sensor to orient the robot based on the IR beacon
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
void findBeacon()
  {
    int speed = 70;
    int vEncoderVal;

    //First we must turn the robot until the beacon is in front of us
    while (SensorValue[irSeeker] > 5)
      {
        motor[DriveLeft]  = speed;
        motor[DriveRight] = speed * (-1);
      }

    while (SensorValue[irSeeker] < 5)
      {
        motor[DriveLeft]  = speed * (-1);
        motor[DriveRight] = speed;
      }

    //beacon should now be in front of us. As the IR sensor has a wide range for what it considers to be in front of us
    //we need to sweep the robot first left then right to find the full range of where the beacon is
    while (SensorValue[irSeeker] <= 5)
      {
        motor[DriveLeft]  = speed * (-1);
        motor[DriveRight] = speed;
      }

    motor[DriveLeft]  = 0;
    motor[DriveRight] = 0;

    //Reset the motor encoder at this position to note our furthest point to the left
    nMotorEncoder[DriveLeft] = 0;

    while (SensorValue[irSeeker] >= 5)
      {
        motor[DriveLeft]  = speed;
        motor[DriveRight] = speed* (-1);
      }

    motor[DriveLeft]  = 0;
    motor[DriveRight] = 0;

    vEncoderVal = nMotorEncoder[DriveLeft];

    //Now that we know the range, the beacon should be rougly in the middle of the range.
    //Need to add an offset as our IR seeker sensor is not mounted directly in the center
    //of the robot
    vEncoderVal = (vEncoderVal + 400) / 2;

    //Turn to the middle of the range
    while (nMotorEncoder[DriveLeft] > vEncoderVal)
      {
        motor[DriveLeft]  = speed * (-1);
        motor[DriveRight] = speed;
      }

    //The beacon should now be directly in front of us. Stop the motors
    motor[DriveLeft]  = 0;
    motor[DriveRight] = 0;
  }


/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                    grippers
//
// This moves the gripper arms to one of three predefined positions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
void grippers(gripperState grpSt)
  {
    if (grpSt == OPENFULL)
      {
        servo[leftGripperServo]   = 90;
        servo[TleftGripperServo]  = 90;
        servo[rightGripperServo]  = 180;
        servo[TrightGripperServo] = 180;
      }
    else if (grpSt == OPENFORWARD)
      {
        servo[leftGripperServo]   = 110;
        servo[TleftGripperServo]  = 110;
        servo[rightGripperServo]  = 140;
        servo[TrightGripperServo] = 140;
      }
    else if (grpSt == CLOSED)
      {
        servo[leftGripperServo]   = 240;
        servo[TleftGripperServo]  = 240;
        servo[rightGripperServo]  = 0;
        servo[TrightGripperServo] = 0;
      }
    else if (grpSt == CRATES)
      {
        servo[leftGripperServo]   = 175;
        servo[TleftGripperServo]  = 175;
        servo[rightGripperServo]  = 85;
        servo[TrightGripperServo] = 85;
      }
  }


/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                    parkballfront
//
// This is one of the main autonomous programs that performs the necessary actions to move the
// robot from the starting position down the ramp and then to the front parking zone while knocking
// over crates along the way and hopefully pushing the bowling ball into the front parking zone
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
void parkballfront (string vcolor, string vposition, int vspeed)
  {
    float vpostile = .5;

    if (vposition == "in")
      {
        vpostile = vpostile + 1;
      }

    //Start by moving down off the home zone
    move(70, 4100, FORWARD);
    //wait for robot to settle
    wait1Msec(100);

    //If we start on the inside, then use grippers to flip over crates
    if (vposition == "in")
      {
        //Move grippers out
        grippers(OPENFORWARD);
        //Give grippers time to move
        wait1Msec(100);

        //Raise the lift
        motor[leftLifterMotor] = 50;
        motor[rightLifterMotor] = 50;
        wait1Msec(600);
        motor[leftLifterMotor] = 0;
        motor[rightLifterMotor] = 0;
      }

    //Next turn in toward the center of the field
    if (vcolor == "red")
      {
        GyroTurn (vspeed, 90, LEFT);
      }
    else
      {
        GyroTurn (vspeed, 90, RIGHT);
      }

    //Move gripppers out
    grippers(OPENFORWARD);

    //Giver grippers time to move
    wait1Msec(100);

    //Move forward again to position robot by bowling ball
    move(vspeed, vtile * vpostile, FORWARD);

    //turn
    if (vcolor == "red")
    {
      GyroTurn (vspeed, 20, LEFT);
    }
    else
    {
      GyroTurn (vspeed, 20, RIGHT);
    }

    //Move forward again to position robot by bowling ball
    move(40, vtile * .4, FORWARD);

    //Turn
    if (vcolor == "red")
    {
      GyroTurn (vspeed, 25, LEFT);
    }
    else
    {
      GyroTurn (vspeed, 25, RIGHT);
    }

    //Move forward toward corner
    move(75, vtile * 1.5, FORWARD);

    //Close grippers
    grippers(CLOSED);
    wait1Msec(100);

    //Use the beacon to orient the robot
    findBeacon();

    //Move forward another tile
    move(vspeed, vtile * 0.5, FORWARD);

    //Use the beacon to orient the robot
    findBeacon();

    //Move forward into parking zone
    move(vspeed, vtile * 1, FORWARD);
  }


/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                    parkback
//
// This is one of the main autonomous programs that performs the necessary actions to move the
// robot from the starting position down the ramp and then to the back parking zone while knocking
// over crates along the way
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
void parkback (string vcolor, string vposition, int vspeed)
  {

    //Start by moving down off the home zone
    move(50, 4100, FORWARD);

    //wait for robot to settle
    wait1Msec(100);

    //If we start on the inside, then use grippers to flip over crates
    if (vposition == "in")
      {
        //Move grippers out
        grippers(OPENFORWARD);
        //Give grippers time to move
        wait1Msec(100);

        //Raise the lift
        motor[leftLifterMotor] = 50;
        motor[rightLifterMotor] = 50;
        wait1Msec(600);
        motor[leftLifterMotor] = 0;
        motor[rightLifterMotor] = 0;

        //Next turn in toward the center of the field to knock over the crates
        if (vcolor == "red")
          {
            GyroTurn (vspeed, 90, LEFT);
          }
        else
          {
            GyroTurn (vspeed, 90, RIGHT);
          }

        //Next turn back toward the back parking zone
        if (vcolor == "red")
          {
            GyroTurn (vspeed, 90, RIGHT);
          }
        else
          {
            GyroTurn (vspeed, 90, LEFT);
          }

        //Close grippers
        grippers(CLOSED);
      }

    //Move forward into the parking zone
    move(vspeed, vtile * 3, FORWARD);
  }


////////////////////////////////////////////////////////////////////////////////////////////
//                                    Defense Ramp                                        //
//   park in front of other teams' ramp, making it harder for them to score in            //
//   autonomous.                                                                          //
//                                                                                        //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////
 void DefenseRamp (string vcolor, string vposition, int vspeed)
{
    //Start by moving down off the home zone
    move(50, 4100, FORWARD);

    //wait for robot to settle
    wait1Msec(100);

    //move forward 1 tiles
    move(50, vtile * 1, FORWARD);

    //turn
    if (vcolor == "red")
          {
            GyroTurn (vspeed, 45, LEFT);
          }
        else
          {
            GyroTurn (vspeed, 45, RIGHT);
          }

    //move forward 2 tiles
     move(50, vtile * 2, FORWARD);

    //turn
    if (vcolor == "red")
          {
            GyroTurn (vspeed, 45, LEFT);
          }
        else
          {
            GyroTurn (vspeed, 45, RIGHT);
          }
    //move forward 1 tiles
     move(50, vtile * 1, FORWARD);
    //turn
    if (vcolor == "red")
          {
            GyroTurn (vspeed, 90, LEFT);
          }
        else
          {
            GyroTurn (vspeed, 90, RIGHT);
          }
     //move forward to the center of the ramp
      move(50, vtile * 0.5, FORWARD);
    }
//////////////////////////////////////////////////////////////////////////////////////
//                                PlaceCrates                                       //
//////////////////////////////////////////////////////////////////////////////////////

void placecrates (string vcolor, string vposition, int vspeed)
  {
    //Start by moving down off the home zone
    move(70, 4100, FORWARD);

    //wait for robot to settle
    wait1Msec(100);
    //Next turn in toward the center of the field
    if (vcolor == "red")
      {
        GyroTurn (vspeed, 30, LEFT);
      }
    else
      {
        GyroTurn (vspeed, 30, RIGHT);
      }

    //push crate forward
    move(vspeed, vtile*0.5,FORWARD);

    //Next turn in toward the center of the field
    if (vcolor == "red")
      {
        GyroTurn (vspeed, 30, RIGHT);
      }
    else
      {
        GyroTurn (vspeed, 30, LEFT);
      }

    //back up
    move(vspeed, vtile*0.3, BACKWARD);


    //Next turn in toward our crates
    if (vcolor == "red")
      {
        GyroTurn (vspeed, 60, RIGHT);
      }
    else
      {
        GyroTurn (vspeed, 60, LEFT);
      }

    //Move gripppers out
    grippers(OPENFULL);
    //Giver grippers time to move
    wait1Msec(200);

    //Move forward again to position robot by crates
    move(vspeed, vtile * 0.8, FORWARD);

    //Move gripppers in
    grippers(CRATES);
    wait1Msec(250);

    //lift crates slightly
    motor[leftLifterMotor]  = 50;
    motor[rightLifterMotor] = 50;
    wait1Msec(350);
    motor[leftLifterMotor]  = 0;
    motor[rightLifterMotor] = 0;
    wait1Msec(100);

    //Move forward
    move(vspeed, vtile * 1, BACKWARD);

    //Move forward to push bowlingball to zone
    move(100, vtile * 1, BACKWARD);

    //Next turn in toward our zone
    if (vcolor == "red")
      {
        GyroTurn (vspeed, 120, RIGHT);
      }
    else
      {
        GyroTurn (vspeed, 120, LEFT);
      }

    //Move forward to put crates in zone
    move(vspeed, vtile * .6, FORWARD);

    //Move gripppers out to drop crates
    grippers(OPENFULL);
    wait1Msec(250);

    //drop lift to release crates
    motor[leftLifterMotor]  = -50;
    motor[rightLifterMotor] = -50;
    wait1Msec(250);
    motor[leftLifterMotor]  = 0;
    motor[rightLifterMotor] = 0;
    wait1Msec(100);

    //Move backward away from crates
    move(vspeed, vtile * .1, BACKWARD);

    //raise lift so we can flip crates
    motor[leftLifterMotor]  = 60;
    motor[rightLifterMotor] = 60;
    wait1Msec(350);
    motor[leftLifterMotor]  = 0;
    motor[rightLifterMotor] = 0;
    wait1Msec(100);

    //turn to flip over crates
    if (vcolor == "red")
      {
        GyroTurn (vspeed, 45, LEFT);
      }
    else
      {
        GyroTurn (vspeed, 45, RIGHT);
      }

    //raise lift so arms clear crates
    motor[leftLifterMotor]  = 50;
    motor[rightLifterMotor] = 50;
    wait1Msec(250);
    motor[leftLifterMotor]  = 0;
    motor[rightLifterMotor] = 0;

    //Next turn in toward park zone
    if (vcolor == "red")
      {
        GyroTurn (vspeed, 50, LEFT);
      }
    else
      {
        GyroTurn (vspeed, 50, RIGHT);
      }

    //Move forward again to position robot by park zone
    move(vspeed, vtile * .5, BACKWARD);

    //Next turn to orient toward IR beacon
    if (vcolor == "red")
      {
        GyroTurn (vspeed, 160, RIGHT);
      }
    else
      {
        GyroTurn (vspeed, 160, LEFT);
      }

    //lower lift so not blocking IR sensor
    motor[leftLifterMotor]  = -50;
    motor[rightLifterMotor] = -50;
    wait1Msec(450);
    motor[leftLifterMotor]  = 0;
    motor[rightLifterMotor] = 0;

    //orient to beacon
    findBeacon();

    //Move forward into park zone
    move(vspeed, vtile * 1, FORWARD);
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
//                                     place crates far side                                      //
////////////////////////////////////////////////////////////////////////////////////////////////////
void placecratesfar (string vcolor, string vposition, int vspeed)
  {
    float vpostile = .5;

    if (vposition == "in")
      {
        vpostile = vpostile + 1;
      }

    //Start by moving down off the home zone
    move(70, 4100, FORWARD);

    //wait for robot to settle
    wait1Msec(100);

    //If we start on the inside, then use grippers to flip over crates
    if (vposition == "in")
      {
        //Move grippers out
        grippers(OPENFORWARD);
        //Give grippers time to move
        wait1Msec(100);

        //Raise the lift
        motor[leftLifterMotor] = 50;
        motor[rightLifterMotor] = 50;
        wait1Msec(600);
        motor[leftLifterMotor] = 0;
        motor[rightLifterMotor] = 0;
      }

    //Next turn in toward the center of the field
    if (vcolor == "red")
      {
        GyroTurn (vspeed, 90, LEFT);
      }
    else
      {
        GyroTurn (vspeed, 90, RIGHT);
      }

    //Move gripppers out
    grippers(OPENFORWARD);

    //Giver grippers time to move
    wait1Msec(100);

    //Move forward again to position robot by bowling ball
    move(vspeed, vtile * vpostile, FORWARD);

    //turn
    if (vcolor == "red")
    {
      GyroTurn (vspeed, 10, LEFT);
    }
    else
    {
      GyroTurn (vspeed, 10, RIGHT);
    }

    //Move forward again to position robot by bowling ball
    move(40, vtile * .4, FORWARD);

    //Turn
    if (vcolor == "red")
    {
      GyroTurn (vspeed, 15, LEFT);
    }
    else
    {
      GyroTurn (vspeed, 15, RIGHT);
    }

    //Move forward toward corner
    move(75, vtile * 1.5, FORWARD);

    //back to position for turns
    move (75, vtile * 1, BACKWARD);

    //move grippers in
    grippers(CLOSED);
    wait1Msec(250);

    //Turn
    if (vcolor == "red")
    {
      GyroTurn (vspeed, 90, RIGHT);
    }
    else
    {
      GyroTurn (vspeed, 90, LEFT);
    }

    //move forward
    move(75, vtile * 1.5, FORWARD);

    //Turn
    if (vcolor == "red")
    {
      GyroTurn (vspeed, 90, RIGHT);
    }
    else
    {
      GyroTurn (vspeed, 90, LEFT);
    }

    //Move gripppers out
    grippers(OPENFULL);
    wait1Msec(250);

    //move forward
    move(75, vtile * 1.3, FORWARD);

    //Move gripppers in
    grippers(CRATES);
    wait1Msec(250);

    motor[leftLifterMotor]  = 50;
    motor[rightLifterMotor] = 50;
    wait1Msec(350);
    motor[leftLifterMotor]  = 0;
    motor[rightLifterMotor] = 0;
    wait1Msec(100);

    //back to position robot
    move(75, vtile * 1, BACKWARD);

    //Turn
    if (vcolor == "red")
    {
      GyroTurn (vspeed, 115, RIGHT);
    }
    else
    {
      GyroTurn (vspeed, 115, LEFT);
    }

    //forward to goal
    move(75, vtile * 2, FORWARD);

    //Move gripppers out to drop crates
    grippers(OPENFULL);
    wait1Msec(250);

    //drop lift to release crates
    motor[leftLifterMotor]  = -50;
    motor[rightLifterMotor] = -50;
    wait1Msec(250);
    motor[leftLifterMotor]  = 0;
    motor[rightLifterMotor] = 0;
    wait1Msec(100);

    //Move backward away from crates
    move(vspeed, vtile * .1, BACKWARD);

    //raise lift so we can flip crates
    motor[leftLifterMotor]  = 60;
    motor[rightLifterMotor] = 60;
    wait1Msec(350);
    motor[leftLifterMotor]  = 0;
    motor[rightLifterMotor] = 0;
    wait1Msec(100);

    //turn to flip over crates
    if (vcolor == "red")
      {
        GyroTurn (vspeed, 45, LEFT);
      }
    else
      {
        GyroTurn (vspeed, 45, RIGHT);
      }

    //raise lift so arms clear crates
    motor[leftLifterMotor]  = 50;
    motor[rightLifterMotor] = 50;
    wait1Msec(250);
    motor[leftLifterMotor]  = 0;
    motor[rightLifterMotor] = 0;

    //Next turn in toward park zone
    if (vcolor == "red")
      {
        GyroTurn (vspeed, 50, LEFT);
      }
    else
      {
        GyroTurn (vspeed, 50, RIGHT);
      }

    //Move forward again to position robot by park zone
    move(vspeed, vtile * .5, BACKWARD);

    //Next turn to orient toward IR beacon
    if (vcolor == "red")
      {
        GyroTurn (vspeed, 160, RIGHT);
      }
    else
      {
        GyroTurn (vspeed, 160, LEFT);
      }

    //lower lift so not blocking IR sensor
    motor[leftLifterMotor]  = -50;
    motor[rightLifterMotor] = -50;
    wait1Msec(450);
    motor[leftLifterMotor]  = 0;
    motor[rightLifterMotor] = 0;

    //orient to beacon
    findBeacon();

    //Move forward into park zone
    move(vspeed, vtile * 1, FORWARD);
  }

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                         Main Task
//
// The following is the main code for the autonomous robot operation. Customize as appropriate for
// your specific robot.
//
// The types of things you might do during the autonomous phase (for the 2008-9 FTC competition)
// are:
//
//   1. Have the robot follow a line on the game field until it reaches one of the puck storage
//      areas.
//   2. Load pucks into the robot from the storage bin.
//   3. Stop the robot and wait for autonomous phase to end.
//
// This simple template does nothing except play a periodic tone every few seconds.
//
// At the end of the autonomous period, the FMS will autonmatically abort (stop) execution of the program.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
task main()
  {
    initializeRobot();

    waitForStart(); // Wait for the beginning of autonomous phase.

    ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////
    ////                                                   ////
    ////    Add your robot specific autonomous code here.  ////
    ////                                                   ////
    ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////

    ////////////////////////////////
    // Debugging                  //
    ////////////////////////////////
    //moveforward (50,4000);
    //leftturn(50,v90turn);
    //GyroTurn(50,90,LEFT);
    //findBeacon();
    //driveBeacon();

    //////////////////////////////////
    //autonomous routines           //
    //////////////////////////////////
    servo[flagServo] = 100;

    if (gRun == "ParkBallFront")
      {
        parkballfront(gColor, gPos, 70);
      }
    else if (gRun == "SpeedFront")
      {
        parkballfront(gColor, gPos, 100);
      }
    else if (gRun == "SpeedBack")
      {
        parkback(gColor, gPos, 100);
      }
    else if (gRun == "DefenseRamp")
      {
        DefenseRamp(gColor, gPos, 70);
      }
    else if (gRun == "placecrates")
      {
        placecrates(gColor, gPos, 70);
      }
    else if (gRun == "placecratesfar")
      {
        placecratesfar(gColor, gPos, 70);
      }
    else
      {
        parkback(gColor, gPos, 70);
      }
  }