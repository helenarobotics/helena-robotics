package robotics.helena;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.SimpleRobot;
import edu.wpi.first.wpilibj.Victor;

/**
 * The VM is configured to automatically run this class, and to call the
 * functions corresponding to each mode, as described in the SimpleRobot
 * documentation. If you change the name of this class or the package after
 * creating this project, you must also update the manifest file in the resource
 * directory.
 */
public class FrisbeeThrower extends SimpleRobot {
    // The motor
    private Victor throwerMotor;

    FrisbeeThrower() {
        // Initialize the motor as the first one
        throwerMotor = new Victor(1);
        throwerMotor.set(0);
    }

    /**
     * This function is called once each time the robot enters autonomous mode.
     */
    public void autonomous() {

    }

    /**
     * This function is called once each time the robot enters operator control.
     */
    public void operatorControl() {
        // The thrower motor is controlled with the first joystick
        Joystick joy = new Joystick(1);

        while (isOperatorControl() && isEnabled()) {
            // Control the thrower motor with the joystick's throttle
            // control.
            setThrowerPower(joy.getThrottle());
        }
    }

    // Set the thrower motor speed using the joystick!
    public void setThrowerPower(double joyThrottle) {
        // Set the value of the throttle to 0 - 1.0, when it's normally
        // -1.0 <-> 1.0.
        double motorPower = joyThrottle + 1.0 / 2.0;

        // Leave the motor powered off at very low power settings.
        if (motorPower > 0.05)
            throwerMotor.set(motorPower);
        else
            throwerMotor.set(0);
    }
}
