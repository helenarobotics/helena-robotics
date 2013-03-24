package robotics.helena;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.Victor;

public class Shooter {
    public static final int MAX_RPM = 225;
    private static final int RPM_TOLERANCE = 5;

    private static final double JOY_Y_SHOOTER_ADJUSTMENT_PERCENT = 20.0 / 100;

    private final Loader loader;
    private final PIDVictor pidShooter;

    private boolean shooterOn = false;

    public Shooter() {
        pidShooter = new PIDVictor(
                new Victor(Configuration.SHOOTER1_VICTOR),
                new Victor(Configuration.SHOOTER2_VICTOR),
                new RPMEncoder(Configuration.SHOOTER_ENCODER),
                1.0, 0.0, 0.0,
                RPM_TOLERANCE, MAX_RPM);
        pidShooter.setPower(0);
        loader = new Loader();
    }

    public void setPower(double power) {
        if (pidShooter.getPower() != power)
            pidShooter.setPower(power);
    }

    public void shootFrisbee() {
        loader.loadNext();
    }

    public void control(Joystick joy) {
        // Is the shooter turned on?
        checkShooterState(joy);

        if (shooterOn) {
            // Motor control.  Coarse settings are done via the joystick throttle,
            // and finer settings (+- 20%) are done via the y-axis of the joystick.
            double motorPower = (joy.getThrottle() - 1.0) / 2.0;
            motorPower += (joy.getY() * JOY_Y_SHOOTER_ADJUSTMENT_PERCENT);
            DashboardComm.powerThrower = motorPower;
            setPower(motorPower);

            // Shoot frisbee if trigger pressed - Note, this is only allowed
            // if the shooter is on.
            if (joystickTrigger(joy))
                shootFrisbee();
        }
    }

    // Shooter button state
    private boolean fireWasPressed = false;
    private boolean shooterEnabledWasPressed = false;

    private boolean joystickTrigger(Joystick joy) {
        boolean btnPressed = false;

        // Fire a frisbee when the button is pressed
        boolean nowPressed = joy.getRawButton(Configuration.FIRE_BUTTON);
        if (nowPressed && !fireWasPressed) {
            btnPressed = true;
        }
        fireWasPressed = nowPressed;

        return btnPressed;
    }

    private void checkShooterState(Joystick joy) {
        // Check the button state
        boolean nowPressed = joy.getRawButton(Configuration.SHOOTER_ENABLE_BUTTON);
        if (nowPressed && !shooterEnabledWasPressed) {
            shooterOn = !shooterOn;
        }
        shooterEnabledWasPressed = nowPressed;
    }
}