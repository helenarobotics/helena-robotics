package robotics.helena;

import edu.wpi.first.wpilibj.DriverStationLCD;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

// This class contains all the raw data that we want sent to the
// DriverStation.  It's a class with all public static variables so the code
// can just set the variables directly.
public class DashboardComm extends Thread {
    // Thrower constants
    public static final String IS_THROWING = "Throwing";
    public static final String THROWER_POWER = "Thrower Power";
    public static final String THROWER_TARGET_RPM = "Thrower Target RPM";
    public static final String THROWER_RPM = "Thrower RPM";
    public static final String THROWER_RPM_ERROR = "Thrower RPM Error";

    public static final int TIME_RUNNING = 0;
    public static final int TIME_STOPPED = 1;
    public static final int TIME_RESET = 2;
    private static int timeState = TIME_RESET;

    public static void startTimer() {
        if (timeState != TIME_RUNNING) {
            SmartDashboard.putNumber("Timer", TIME_RUNNING);
            timeState = TIME_RUNNING;
        }
    }

    public static void stopTimer() {
        if (timeState == TIME_RUNNING) {
            SmartDashboard.putNumber("Timer", TIME_STOPPED);
            timeState = TIME_STOPPED;
        }
    }

    public static void resetTimer() {
        if (timeState != TIME_RESET) {
            SmartDashboard.putNumber("Timer", TIME_RESET);
            timeState = TIME_RESET;
        }
    }

    // Drive joystick
    public static double driveJoyDirection;
    public static double driveJoyMagnitude;
    public static double drivePowerRatio;

    // Thrower information globals
    public static double throwerThrottle;
    public static double rpmThrower;
    public static double rpmTarget;
    public static double powerThrower;

    // Frisbee feeder
    public static boolean throwing;

    // Instantiate the sender thread
    static {
        new DashboardComm();
    }

    // This class should never be instantiated externally.
    private DashboardComm() {
        // Startup the thread that sends the data to the DriverStation
        super("DashboardComm");
        start();
    }

    public void run() {
        DriverStationLCD driverStation = DriverStationLCD.getInstance();
        while (true) {
            try {
                // Arbitrarily chosen constant.
                Thread.sleep(50);
            } catch (InterruptedException ignored) {
            }

            // Drive information
            SmartDashboard.putNumber("DriveJoystick Direction", driveJoyDirection);
            SmartDashboard.putNumber("DriveJoystick Magnitude", driveJoyMagnitude);
            SmartDashboard.putNumber("DrivePowerRatio", drivePowerRatio);

            // Thrower
            SmartDashboard.putNumber(THROWER_RPM, (int)rpmThrower);
            SmartDashboard.putNumber(THROWER_TARGET_RPM, (int)rpmTarget);
            SmartDashboard.putNumber(THROWER_POWER, (int)(-powerThrower * 100));
            SmartDashboard.putNumber(THROWER_RPM_ERROR, (int)(rpmTarget - rpmThrower));
            SmartDashboard.putBoolean(IS_THROWING, throwing);
        }
    }
}
