//
// CalibrateDrive.h
//
#ifndef BOOL
#define BOOL int
#endif

// We want herein to be able to turn the drive ramping on and off
BOOL  fUseDriveRamp     = false;
float driveBalance      = 1.0;  // actual initialization below in main()

#define USE_DRIVE_RAMP         fUseDriveRamp
#define DRIVE_BALANCE_FORWARD  driveBalance
#define DRIVE_BALANCE_BACKWARD driveBalance

#define USE_JOYSTICK_DRIVER 1   // we need the dang joystick
#include "lib\MainPrelude.h"
#include "config\RobotConfig.h"
#include "lib\MainPostlude.h"

//------------------------------------------------------------------
void Beep(int note, string szMessage) {
    TRACE(("%s", szMessage));
    Beep(note);
}

task main() {
    if (!InitializeMain(true,true))
        return;

    waitForStart();
    PlayHappy();

    // cause DisplayTask to show sonic readings
    fDisplaySonic = true;

    driveBalance = DRIVE_BALANCE_FORWARD_DEFAULT;
    DIRECTION direction  = FORWARD;
    int       cmDistance = 150;
    int       power      = DRIVE_POWER_FAST;

    for (;;) {
        // Scrub variables
        ClampVar(driveBalance,          0.0, 2.0);
        ClampVar(power,            0,   DRIVE_POWER_FULL);
        ClampVar(cmDistance,       0,   350);

        // Give feedback on the display
        nxtDisplayTextLine(4, "%d cm %s",         cmDistance, (direction==FORWARD ? "forward" : "backward"));
        nxtDisplayTextLine(5, "power=%d %s",      power,      (fUseDriveRamp ? "ramp" : "no ramp"));
        nxtDisplayTextLine(6, "bal=%4.3f",        driveBalance);

        // Get current information from the joystick
        getJoystickSettings(joystick);

        // The left and right joysticks give us manual driving control as usual
        DoManualDrivingControl(1,joystick);

        // A never-taken conditional so that ALL the cases below are
        // uniformly 'else if'
        if (0) {
        }
        else if (joyBtnOnce(1,JOYBTN_TOP_LEFT)) {
            //----------------------------------------------------------
            // Driving calibration
            //----------------------------------------------------------

            // Toggle the driving direction
            direction = ReverseDirection(direction);
            if (FORWARD==direction)
                Beep(NOTE_E, "forward");
            else
                Beep(NOTE_A, "backward");
        } else if (joyBtnOnce(1,JOYBTN_TOP_RIGHT)) {
            // Toggle the ramp on/off
            fUseDriveRamp = !fUseDriveRamp;
            if (fUseDriveRamp)
                Beep(NOTE_E, "ramp on");
            else
                Beep(NOTE_A, "ramp off");
        } else if (joyHatOnce(1,JOYHAT_UPLEFT)) {
            // Toggle the ramp on/off
            fUseDriveRamp = !fUseDriveRamp;
            if (fUseDriveRamp)
                Beep(NOTE_E, "ramp on");
            else
                Beep(NOTE_A, "ramp off");
        } else if (joyBtnOnce(1,1)) {
            // Go! Zero the encoders so that they're interesting at the
            // end of the drive
            ZeroEncoders();
            DriveForwards(OrientDistance(cmDistance,direction), power);

            // auto reverse the direction so we are more safe when testing!
            direction = ReverseDirection(direction);
        }
        //----------------------------------------------------------
        // Distance adjustment
        //----------------------------------------------------------

        else if (joyHatOnce(1,JOYHAT_RIGHT)) {
            cmDistance += 20;
            Beep(NOTE_G, "farther");
        } else if (joyHatOnce(1,JOYHAT_LEFT)) {
            cmDistance -= 20;
            Beep(NOTE_C, "closer");
        }

        //----------------------------------------------------------
        // Power adjustment
        //----------------------------------------------------------
        else if (joyHatOnce(1,JOYHAT_UP)) {
            power += 5;
            Beep(NOTE_D, "stronger");
        } else if (joyHatOnce(1,JOYHAT_DOWN)) {
            Beep(NOTE_G / 2, "weaker");
            power -= 5;
        }

        //----------------------------------------------------------
        // Power balance adjustment
        //----------------------------------------------------------
        else if (joyBtnOnce(1,JOYBTN_LEFTTRIGGER_UPPER)) {
            // Increment driveBalance by .01
            driveBalance += .01;
            Beep(NOTE_E, "+.01");
        } else if (joyBtnOnce(1,JOYBTN_LEFTTRIGGER_LOWER)) {
            // Decrement driveBalance by .01
            driveBalance -= .01;
            Beep(NOTE_A, "-.01");
        } else if (joyBtnOnce(1,JOYBTN_RIGHTTRIGGER_UPPER)) {
            // Increment driveBalance by .001
            driveBalance += .001;
            Beep(NOTE_E * 2, "+.001");
        } else if (joyBtnOnce(1,JOYBTN_RIGHTTRIGGER_LOWER)) {
            // Decrement driveBalance by .001
            driveBalance -= .001;
            Beep(NOTE_A * 2, "-.001");
        }
    }
}
