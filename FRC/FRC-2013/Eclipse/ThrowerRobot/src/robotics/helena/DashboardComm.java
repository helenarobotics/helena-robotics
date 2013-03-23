package robotics.helena;

import edu.wpi.first.wpilibj.DriverStationLCD;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

// This class contains all the raw data that we want sent to the
// DriverStation.  It's a class with all public static variables so the code
// can just set the variables directly.
public class DashboardComm extends Thread {
    // Thrower constants
    public static final String JOY2_THROTTLE = "Joystick 2 Throttle";
    public static final String IS_THROWING = "Throwing";

    public static final String THROWER_POWER = "Thrower Power";
    public static final String THROWER_RPM = "Thrower RPM";
    public static final String THROWER_TARGET_RPM = "Thrower Target RPM";

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

    // Automation mode for Thrower (unused)
/*    
    public static final int MO_NONE = 0;
    public static final int MO_AUTOSHOOT = 1;
    public static final int MO_SEMIAUTO = 2;
    public static final int MO_MANUAL = 3;
    private int mode = MO_NONE;

    public void setMode(int set) {
        mode = set;
    }
*/
    // Drive joystick
    public static double driveJoyDirection;
    public static double driveJoyMagnitude;
    public static double drivePowerRatio;

    // Shooter joystick
//    public static double shootJoyX;
//    public static double shootJoyY;

    //
    // Thrower information globals
    //

    // Thrower motors!
    public static double throwerThrottle;
    public static double rpmThrower;
    public static double rpmTarget;
    public static double powerThrower;

    // Frisbee feeder
    public static boolean throwing;

/*
    // Results FROM camera processing
    public static int rpmBtmSwish;
    public static int rpmBtmBB;
*/
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

    private long startTime;
    public void run() {
        startTime = System.currentTimeMillis();

        @SuppressWarnings("unused")
        DriverStationLCD driverStation = DriverStationLCD.getInstance();
        while (true) {
            try {
                // Arbitrarily chosen constant.
                Thread.sleep(50);
            } catch (InterruptedException ignored) {
            }
            send();
            receive();
        }
    }

    public void send() {
        // Drive information
        SmartDashboard.putNumber("DriveJoystick Direction", driveJoyDirection);
        SmartDashboard.putNumber("DriveJoystick Magnitude", driveJoyMagnitude);
        SmartDashboard.putNumber("DrivePowerRatio", drivePowerRatio);
/*
        SmartDashboard.putInt("Mode", mode);

        // Shooter
        SmartDashboard.putNumber("ShootJoystick X", shootJoyX);
        SmartDashboard.putNumber("ShootJoystick Y", shootJoyY);
*/
        SmartDashboard.putNumber(JOY2_THROTTLE, throwerThrottle);
        SmartDashboard.putBoolean(IS_THROWING, throwing);

        // RPM Sensor/PID control
        SmartDashboard.putNumber(THROWER_POWER, (int)(powerThrower * 100));
        SmartDashboard.putNumber(THROWER_RPM, (int)rpmThrower);
        SmartDashboard.putNumber(THROWER_TARGET_RPM, (int)rpmTarget);

        // Logging for later analysis
        System.out.println("Time:" + (System.currentTimeMillis() - startTime));
        System.out.println("\t" + JOY2_THROTTLE + "=" + throwerThrottle);
        System.out.println("\t" + IS_THROWING + "=" + throwing);
        System.out.println("\t" + THROWER_POWER + "=" + (int)(powerThrower * 100));
        System.out.println("\t" + THROWER_TARGET_RPM + "=" + (int)rpmTarget);
        System.out.println("\t" + THROWER_RPM + "=" + (int)rpmThrower);
        System.out.println("");
    }

    public void receive() {
/*
        rpmBtmSwish = (int)SmartDashboard.getNumber("Target RPM 1 Swish");
        rpmBtmBB = (int)SmartDashboard.getNumber("Target RPM 1 BB");
*/
    }
}