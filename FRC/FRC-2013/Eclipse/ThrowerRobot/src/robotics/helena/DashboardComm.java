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

    // The communication protocol
    private static SmartDashboard s = new SmartDashboard();

    public static final int TIME_RUNNING = 0;
    public static final int TIME_STOPPED = 1;
    public static final int TIME_RESET = 2;
    private static int timeState = TIME_RESET;

    public static void startTimer() {
        if (timeState != TIME_RUNNING) {
            s.putNumber("Timer", TIME_RUNNING);
            timeState = TIME_RUNNING;
        }
    }

    public static void stopTimer() {
        if (timeState == TIME_RUNNING) {
            s.putNumber("Timer", TIME_STOPPED);
            timeState = TIME_STOPPED;
        }
    }

    public static void resetTimer() {
        if (timeState != TIME_RESET) {
            s.putNumber("Timer", TIME_RESET);
            timeState = TIME_RESET;
        }
    }

    // Automation mode for Thrower (unused)
    public static final int MO_NONE = 0;
    public static final int MO_AUTOSHOOT = 1;
    public static final int MO_SEMIAUTO = 2;
    public static final int MO_MANUAL = 3;
    private int mode = MO_NONE;

    public void setMode(int set) {
        mode = set;
    }

    //
    // Drive frame information
    //
/*
    // Drive joystick
    public static double joy1Direction;
    public static double joy1Magnitude;

    // Shooter joystick
    public static double joy2X;
    public static double joy2Y;

    // Motor status
    // The RobotDrive class doesn't provide access to these
//    public static double leftMotorPower;
//    public static double rightMotorPower;
*/

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
    //Results FROM camera processing
    public static int rpmBtmSwish;
    public static int rpmBtmBB;
*/
    // Instantiate the sender thread
    private static long startTime;
    static {
        new DashboardComm();
        startTime = System.currentTimeMillis();
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
            send();
            receive();
        }
    }

    public void send() {
/*
        // Only need to send Joystick and Sensor info
        // Joystick 1
        s.putDouble("Joystick 1 Direction", joy1Direction);
        s.putDouble("Joystick 1 Magnitude", joy1Magnitude);

        s.putInt("Mode", mode);

        //Joystick 2
        s.putDouble("Joystick 2 X", joy2X);
        s.putDouble("Joystick 2 Y", joy2Y);
*/
        s.putNumber(JOY2_THROTTLE, throwerThrottle);
        s.putBoolean(IS_THROWING, throwing);

        // RPM Sensor/PID control
        s.putNumber(THROWER_POWER, (int)(powerThrower * 100));
        s.putNumber(THROWER_RPM, (int)rpmThrower);
        s.putNumber(THROWER_TARGET_RPM, (int)rpmTarget);

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
        rpmBtmSwish = (int)s.getDouble("Target RPM 1 Swish");
        rpmBtmBB = (int)s.getDouble("Target RPM 1 BB");
*/
    }
}
