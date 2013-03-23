package robotics.helena;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.SimpleRobot;

public class Thrower extends SimpleRobot {
    private final Shooter shooter;
//    private final Grabber grabber;
//    private final Lift lift;

    public Thrower() {
        shooter = new Shooter();
//        grabber = new Grabber();
//        lift = new Lift();
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
//            grabber.move(frizJoy);
//            lift.control(frizJoy);

            // Run the shooter!
            shooter.control(frizJoy);
        }
    }

    /**
     * This function is called once each time the robot enters test mode.
     */
    public void test() {
    }
}
