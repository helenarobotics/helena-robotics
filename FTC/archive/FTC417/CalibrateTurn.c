//
// CalibrateTurn.h
//
#define USE_JOYSTICK_DRIVER 1
#include "lib\MainPrelude.h"
#include "config\RobotConfig.h"
#include "lib\MainPostlude.h"

//------------------------------------------------------------------
ENCOD encLeft;
ENCOD encRight;
ENCOD dencTurnIncrement = 100;
ENCOD dencTurn = dencTurnIncrement;
ANGLE dangleTurn = 90.0;
int cTurn = 4;
TURN_DIRECTION turnDirection = TURN_RIGHT;

//------------------------------------------------------------------
void
Beep(int note, string szMessage) {
    TRACE(("%s", szMessage));
    Beep(note);
}

task
main() {
    if (!InitializeMain(true, true))
        return;

    waitForStart();
    PlayHappy();

    // cause DisplayTask to show distance
    fDisplaySonic = true;

#if SensorIsDefined(sensnmGyroHorz)
    StartReadingGyroSensor(sensGyroHorz);
#endif

    for (;;) {
        // Scrub the variables
        ClampVar(cTurn, 1, intLast);

        float gyro = 0;
#if SensorIsDefined(sensnmGyroHorz)
        gyro = sensGyroHorz.deg;
#endif

        // Give feedback on the display
        nxtDisplayTextLine(3, "dang=%3.1f", dangleTurn);
        nxtDisplayTextLine(4, "denc=%d", dencTurn);
        nxtDisplayTextLine(5, "cTurn=%d", cTurn);
        nxtDisplayTextLine(6, "gyro=%.1f", gyro);

        // Get current information from the joystick
        if (!getJoystickSettings(joystick))
            continue;
        const int jyc = 1;

        // A never-taken conditional so that ALL the cases below are
        // simply 'else if'
        if (0) {
        } else if (joyHatOnce(jyc, JOYHAT_UP)) {
            //----------------------------------------------------------
            // Turn increment and direction management
            //----------------------------------------------------------
            dencTurn +=
                (TURN_RIGHT ==
                 turnDirection ? dencTurnIncrement : -dencTurnIncrement);
            Beep(NOTE_C_SHARP);
            TRACE(("dencTurn=%d", dencTurn));
        } else if (joyHatOnce(jyc, JOYHAT_DOWN)) {
            dencTurn -=
                (TURN_RIGHT ==
                 turnDirection ? dencTurnIncrement : -dencTurnIncrement);
            Beep(NOTE_D);
            TRACE(("dencTurn=%d", dencTurn));
        } else if (joyHatOnce(jyc, JOYHAT_RIGHT)) {
            dencTurn +=
                (TURN_RIGHT ==
                 turnDirection ? dencTurnIncrement / 10 : -(dencTurnIncrement /
                                                            10));
            Beep(NOTE_C_SHARP);
            TRACE(("dencTurn=%d", dencTurn));
        } else if (joyHatOnce(jyc, JOYHAT_LEFT)) {
            dencTurn -=
                (TURN_RIGHT ==
                 turnDirection ? dencTurnIncrement / 10 : -(dencTurnIncrement /
                                                            10));
            Beep(NOTE_D);
            TRACE(("dencTurn=%d", dencTurn));
        } else if (joyBtnOnce(jyc, JOYBTN_RIGHTTRIGGER_LOWER)) {
            // Init the turn denc 
            dencTurn =
                TURN_RIGHT ==
                turnDirection ? dencTurnIncrement : -dencTurnIncrement;
            Beep(NOTE_A);
            TRACE(("dencTurn=%d", dencTurn));
        } else if (joyBtnOnce(jyc, JOYBTN_RIGHTTRIGGER_UPPER)) {
            // Flip the turn direction
            dencTurn = -dencTurn;
            if (turnDirection == TURN_LEFT) {
                turnDirection = TURN_RIGHT;
                Beep(NOTE_E, "turning right");
            } else {
                turnDirection = TURN_LEFT;
                Beep(NOTE_F, "turning left");
            }
        } else if (joyBtnOnce(jyc, JOYBTN_LEFTTRIGGER_UPPER)) {
            //----------------------------------------------------------
            // Turn count
            //----------------------------------------------------------
            cTurn++;
            Beep(NOTE_E);
            TRACE(("cTurn=%d", cTurn));
        } else if (joyBtnOnce(jyc, JOYBTN_LEFTTRIGGER_LOWER)) {
            cTurn--;
            Beep(NOTE_A);
            TRACE(("cTurn=%d", cTurn));
        } else if (joyBtnOnce(jyc, JOYBTN_TOP_RIGHT)) {
            //----------------------------------------------------------
            // Do the turn!
            //----------------------------------------------------------

            // Turn the indicated denc left or right accoring to the
            // current turn direction
            ENCOD denc = dencTurn;

            // Zero out the compass and encoders. That way, the NXT
            // display will show useful data at the end of the turn
            ZeroEncoders();
            ZeroCompass();

            // Execute the turn. Turn off the sonic display for paranoia
            // reasons.
            fDisplaySonic = false;

            // Log anything that the turn might care to
            TelemetryEnable(true);
            for (int i = 0; i < cTurn; i++) {
                TurnRightEncod(TURN_POWER_MEDIUM, denc, -denc, TURN_BALANCE);
                wait1Msec(500);
            }
            TelemetryEnable(false);
            fDisplaySonic = true;
        } else if (joyBtnOnce(jyc, JOYBTN_TOP_LEFT)) {
            // Turn back to the angle before the turn just made
            ReadEncoders(encLeft, encRight);
            TurnRightEncod(TURN_POWER_MEDIUM, -encLeft, -encRight,
                           TURN_BALANCE);
        } else if (joyFlickOnce(jyc, JOY_RIGHT, JOYDIR_LEFT | JOYDIR_RIGHT)) {
            //----------------------------------------------------------
            // Calibrated turns by requested angles
            //----------------------------------------------------------

            // Turn right or left using in quanta of the indicated angle
            ZeroEncoders();
            ZeroCompass();

            TelemetryEnable(true);
            for (int i = 0; i < cTurn; i++) {
                if (joyFlick(1, JOY_RIGHT, JOYDIR_RIGHT))
                    TurnRight(dangleTurn, TURN_POWER_MEDIUM, TURN_BALANCE);
                else
                    TurnRight(-dangleTurn, TURN_POWER_MEDIUM, TURN_BALANCE);
                wait1Msec(500);
            }
            TelemetryEnable(false);
        } else if (joyFlickOnce(jyc, JOY_LEFT, JOYDIR_UP)) {
            // Reset to a full turn
            dangleTurn = 360;
            Beep(NOTE_D, "angle=360");
            cTurn = Rounded(360.0 / dangleTurn, int);
        } else if (joyFlickOnce(jyc, JOY_LEFT, JOYDIR_LEFT)) {
            // Cut the turn angle in half
            dangleTurn *= 0.5;
            Beep(NOTE_F_SHARP);
            TRACE(("angle/=2 -> %f", dangleTurn));
            cTurn = Rounded(360.0 / dangleTurn, int);
        } else if (joyFlickOnce(jyc, JOY_LEFT, JOYDIR_RIGHT)) {
            // Add 50% to angle
            dangleTurn *= 1.5;
            MinVar(dangleTurn, 360.0);
            Beep(NOTE_F_SHARP);
            TRACE(("angle*=1.5 -> %f", dangleTurn));
            cTurn = Rounded(360.0 / dangleTurn, int);
        } else if (joyFlickOnce(jyc, JOY_LEFT, JOYDIR_DOWN)) {
            // Double the turn angle
            dangleTurn *= 2.0;
            MinVar(dangleTurn, 360.0);
            Beep(NOTE_F_SHARP);
            TRACE(("angle*=2.0 -> %f", dangleTurn));
            cTurn = Rounded(360.0 / dangleTurn, int);
        }
    }
}
