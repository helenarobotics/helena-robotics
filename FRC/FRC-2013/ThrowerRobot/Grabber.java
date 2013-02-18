/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package robotics.helena;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.Servo;

public class Grabber {
    private Servo grabber;
    private final double OPEN_POS = 0;
    private final double CLOSED_POS = 1;
    private boolean open = false;

    public Grabber() {
        grabber = new Servo(Configuration.GRABBER_SERVO);
    }

    public void move(Joystick joy) {
        if (btnPressed(joy)) {
            if (open)
                grabber.set(CLOSED_POS);
            else
                grabber.set(OPEN_POS);
            open = !open;
        }
    }

    private boolean wasPressed = false;
    private boolean btnPressed(Joystick joy) {
        boolean btnPressed = false;

        // Toggle the shifter when the shifter button is pressed
        boolean nowPressed = joy.getRawButton(Configuration.GRABBER_BUTTON);
        if (nowPressed && !wasPressed) {
            btnPressed = true;
        }
        wasPressed = nowPressed;

        return btnPressed;
    }
}
