#pragma config(Motor,  motorA,          motorLeft,     tmotorNormal, PIDControl, encoder)
#pragma config(Motor,  motorB,          motorRight,    tmotorNormal, PIDControl, reversed, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "..\lib\JoystickDriver.h"
#include "..\lib\Music.h"

task
main() {
    Beep();
    int jyc = 1;

    while (true) {
        if (getJoystickSettings(joystick)) {
            if (joyBtn(jyc, 1)) {
                PlayHappy();
            } else if (joyBtnOnce(jyc, 2)) {
                PlaySad();
            } else if (joyBtnOnce(jyc, 3)) {
                PlayMusic(musicFifth, 100, NOTE_QUARTER);
            } else if (joyBtnOnce(jyc, 4)) {
                Beep();
                int value = 0;
                for (int j = 0; j < 10; j++) {
                    nDatalogSize = 6000;
                    for (int i = 0; i < 10000; i++) {
                        AddToDatalog(value);
                        value++;
                    }
                    SaveNxtDatalog();
                }
                Beep();
            }

            int stickRange = 128 - joyThrottleDeadZone;
            int motorRange = 100;
            int stick = joyY(jyc, JOY_LEFT);
            int motorPower = Max(0,
                abs(stick) - joyThrottleDeadZone) * motorRange / stickRange;
            if (stick < 0)
                motorPower = -motorPower;

            motor[motorLeft] = motorPower * 2;
            motor[motorRight] = motorPower;
        } else if (joyMessageCount() > 0 && nSysTime - joyMessageTime() > 1000) {
            Beep(NOTE_E);
        }
    }
}
