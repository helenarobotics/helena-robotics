package robotics.helena;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.Victor;

public class Shooter {
    public static final int MAX_RPM = 225;
    private static final int RPM_TOLERANCE = 5;

    private static final double JOY_Y_SHOOTER_ADJUSTMENT_PERCENT = 20.0 / 100;

    private final Loader loader;
    private final PIDVictor pidShooter;

    // PID tuning - Ok, but can be improved
    private static final double kP = 0.0005;
    private static final double kI = 0.0;
    private static final double kD = kP * 10.0;

    private boolean shooterOn = false;

    public Shooter() {
        pidShooter = new PIDVictor(
                new Victor(Configuration.SHOOTER1_VICTOR),
                new Victor(Configuration.SHOOTER2_VICTOR),
                new RPMEncoder(Configuration.SHOOTER_ENCODER),
                kP, kI, kD,
                RPM_TOLERANCE, MAX_RPM);
        pidShooter.setTargetRpm(0);
        loader = new Loader();
    }

    public void unpack() {
        loader.unpack();
    }

    public void control(Joystick joy) {
        // Is the shooter turned on?
        checkShooterState(joy);

        if (shooterOn) {
            // Motor control.  Coarse settings are done via the joystick throttle,
            // and finer settings (+- 20%) are done via the y-axis of the joystick.
            double motorCtl = Math.abs((joy.getThrottle() - 1.0) / 2.0);
            motorCtl -= (joy.getY() * JOY_Y_SHOOTER_ADJUSTMENT_PERCENT);
            // Limit the control from 0 <-> 1
            motorCtl = Math.max(0, Math.min(1.0, motorCtl));
            pidShooter.setTargetRpm((int)(motorCtl * MAX_RPM));

            // Only allow the user to fire a frisbee if the shooter is spun up
            // *OR* if the user chooses to over-ride by holding down the OVER-RIDE
            // button.
            boolean inRange = pidShooter.onTarget() || joy.getRawButton(Configuration.OVERRIDE_BUTTON);
            if (inRange && joystickTrigger(joy))
                shootFrisbee();
        } else
            pidShooter.setTargetRpm(0);
    }

    public void setTargetRpm(int rpm) {
        pidShooter.setTargetRpm(rpm);
    }

    public void shootFrisbee() {
        loader.loadNext();
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

    private long startInRangeTime = 0;
    public boolean inRange() {
        boolean onTarget = false;
        if (pidShooter.onTarget()) {
            // We have to be in-range for 1 second
            long now = System.currentTimeMillis();
            if (startInRangeTime == 0)
                startInRangeTime = now;
            else if (now - startInRangeTime > 250)
                onTarget = true;
        } else
            startInRangeTime = 0;
        return onTarget;
    }
}
