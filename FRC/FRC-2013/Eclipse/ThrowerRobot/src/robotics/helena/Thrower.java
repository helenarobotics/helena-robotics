package robotics.helena;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.SimpleRobot;

public class Thrower extends SimpleRobot {
    private final Shooter shooter;

    public Thrower() {
        shooter = new Shooter();
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
