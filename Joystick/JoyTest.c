//Include file to handle joystick interface
#include "ImprovedJoystick.c"

task main()
{
    while (true) {
        // Update variables with current joystick values
        getJoystickSettings(joystick);

        int x = joystick.joy1_x1;
        int y = joystick.joy1_y1;
        nxtDisplayString(0, "X <%d>     ", x);
        nxtDisplayString(1, "Y <%d>     ", y);

        x = joystick.joy1_x2;
        y = joystick.joy1_y2;
        nxtDisplayString(2, "X <%d>    ", x);
        nxtDisplayString(3, "Y <%d>    ", y);

        nxtDisplayString(5, "%s", (joy1Btn(5) ? "Y" : "N"));
    }
}
