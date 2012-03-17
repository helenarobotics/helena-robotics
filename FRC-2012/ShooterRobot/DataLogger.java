package robotics.helena;

import edu.wpi.first.wpilibj.DriverStationLCD;

// This class contains all the raw data that we want sent to the
// DriverStation.  It's a class with all public static variables so the code
// can just set the variables directly.
public class DataLogger extends Thread {
    //
    // Drive frame information
    //

    // Motor status
    public static boolean shifterStatus;
    // The RobotDrive class doesn't provide access to these
//    public static double leftMotorPower;
//    public static double rightMotorPower;

    // The calculated Gyro Angle
    public static double gyroAngle;

    //
    // Shooter information
    //

    // Shooter motors!
    public static double rpmTop;
    public static double rpmBottom;
    public static double powerTop;
    public static double powerBottom;

    // Lazy-susan on the shooter
    public static double shooterRotationAngle;
    public static double shooterRotatorPower;

    // Ball feeder
    public static boolean shooting;

    // Instantiate the sender thread
    static {
        new DataLogger();
    }

    // This class should never be instantiated externally.
    private DataLogger() {
        // Startup the thread that sends the data to the DriverStation
        super("DataLogger");
        start();
    }

    public void run() {
        DriverStationLCD driverStation = DriverStationLCD.getInstance();
        while (true) {
            String shootingStatus = "false";
            if (shooting)
                shootingStatus = "true";
            driverStation.println(
                DriverStationLCD.Line.kMain6, 1, shootingStatus);

            try {
                // Arbitrarily chosen constant.
                Thread.sleep(250);
            } catch (InterruptedException ignored) {
            }
            
        }
    }
}
