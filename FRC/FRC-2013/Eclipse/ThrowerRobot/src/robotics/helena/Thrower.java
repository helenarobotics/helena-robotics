package robotics.helena;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.SimpleRobot;

public class Thrower extends SimpleRobot {
    private final Shooter shooter;

    private static final int AUTO_SHOOTER_TARGET_RPM = 180;
    private static final int NUM_FRISBEES = 3;

    public Thrower() {
        // Create the shooter
        shooter = new Shooter();
    }

    public void autonomous() {
        // Unpack the shooter arm
        shooter.unpack();
        try {
            Thread.sleep(100);
        } catch (InterruptedException ignored) {
        }

        // Startup the shooter
        shooter.setTargetRpm(AUTO_SHOOTER_TARGET_RPM);

        // Shoot each of the frisbees
        for (int i = 0; i < NUM_FRISBEES; i++) {
            // Wait for the motor to get up to speed
            while (!shooter.inRange()) {
                try {
                    Thread.sleep(50);
                } catch (InterruptedException ignored) {
                }
            }
            shooter.shootFrisbee();
            // Add a delay to make sure we wait for the RPM calculation to catch up
            try {
                Thread.sleep(1000);
            } catch (InterruptedException ignored) {
            }
        }
        DashboardComm.resetTimer();
    }

    public void operatorControl() {
        //Setup drive motors
        DriveBase drive = new DriveBase();
        Joystick driveJoy = new Joystick(Configuration.DRIVE_JOY);
        Joystick frizJoy = new Joystick(Configuration.FRIZ_JOY);

        DashboardComm.startTimer();
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
