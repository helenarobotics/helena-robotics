package robotics.helena;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.RobotDrive;
import edu.wpi.first.wpilibj.SimpleRobot;
import edu.wpi.first.wpilibj.Victor;

public class Thrower extends SimpleRobot {
    private final Loader loader;
    private final Shooter shooter;
    //private final Grabber grabber;
    //private final Lift lift;
    public Thrower() {
        shooter = new Shooter();
        loader = new Loader(shooter);
        //grabber = new Grabber();
        //lift = new Lift();
    }

    public void autonomous() {
    }

    public void operatorControl() {
        //Setup drive motors
        DriveBase drive = new DriveBase();
        Joystick driveJoy = new Joystick(Configuration.DRIVE_JOY);
        Joystick frizJoy = new Joystick(Configuration.FRIZ_JOY);

        while (isOperatorControl()) {
            // Drive motors
            drive.move(driveJoy);
            //grabber.move(frizJoy);
            //lift.control(frizJoy);
            // Frisbee motor
            double motorPower = (frizJoy.getThrottle() - 1.0) / 2.0;
            //System.out.println("Mot=" + motorPower);
            shooter.setPower(motorPower);

            //Loads frisbee if trigger pressed
            if (joystickTrigger(frizJoy)) {
                System.out.println("Load Me");
                loader.loadNext();
            }
        }
    }
    
    private boolean wasPressed = false;
    private boolean joystickTrigger(Joystick joy) {
        boolean btnPressed = false;

        // Toggle the shifter when the shifter button is pressed
        boolean nowPressed = joy.getRawButton(Configuration.LOAD_BUTTON);
        if (nowPressed && !wasPressed) {
            btnPressed = true;
        }
        wasPressed = nowPressed;

        return btnPressed;
    }

    /**
     * This function is called once each time the robot enters test mode.
     */
    public void test() {
    }
}
