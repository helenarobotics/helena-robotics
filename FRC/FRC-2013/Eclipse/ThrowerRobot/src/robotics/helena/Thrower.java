package robotics.helena;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.SimpleRobot;

public class Thrower extends SimpleRobot {
    private static final double FRIZ_JOY_Y_SHOOTER_ADJUSTMENT_PERCENT = 20.0 / 100;

    private final Loader loader;
    private final Shooter shooter;

//    private final Grabber grabber;
//    private final Lift lift;

    public Thrower() {
        shooter = new Shooter();
        loader = new Loader(shooter);
//        grabber = new Grabber();
//        lift = new Lift();
    }

    public void autonomous() {
    }

    boolean shooterOn = false;

    public void operatorControl() {
        //Setup drive motors
        DriveBase drive = new DriveBase();
        Joystick driveJoy = new Joystick(Configuration.DRIVE_JOY);
        Joystick frizJoy = new Joystick(Configuration.FRIZ_JOY);

        while (isOperatorControl()) {
            // Drive motors
            drive.move(driveJoy);
//            grabber.move(frizJoy);
//            lift.control(frizJoy);

            //
            // Check the joystick buttons
            //
            
            // Is the shooter turned on?
            checkShooterState(driveJoy);

            if (shooterOn) {
                // Frisbee motor.  Coarse settings are done via the throttle, and
                // finer settings (+- 20%) are done via the y-axis of the joystick.
                double motorPower = (frizJoy.getThrottle() - 1.0) / 2.0;
                motorPower += (frizJoy.getY() * FRIZ_JOY_Y_SHOOTER_ADJUSTMENT_PERCENT);
                shooter.setPower(motorPower);

                // Shoots frisbee if trigger pressed - Note, this is only allowed
                // if the shooter is on.
                if (joystickTrigger(frizJoy)) {
                    System.out.println("Load Me");
                    loader.loadNext();
                }
            }
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

    /**
     * This function is called once each time the robot enters test mode.
     */
    public void test() {
    }
}
