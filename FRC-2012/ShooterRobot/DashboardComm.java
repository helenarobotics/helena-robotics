package robotics.helena;

import edu.wpi.first.wpilibj.DriverStationLCD;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;


// This class contains all the raw data that we want sent to the
// DriverStation.  It's a class with all public static variables so the code
// can just set the variables directly.
public class DashboardComm extends Thread {
    // The communication protocol
    private static SmartDashboard s = new SmartDashboard();

    public static final int TR_DISABLED = 0;
    public static final int TR_LO = 1;
    public static final int TR_HI = 2;

    public static final int TIME_RUNNING = 0;
    public static final int TIME_STOPPED = 1;
    public static final int TIME_RESET = 2;
    private static int timeState = TIME_RESET;

    public static void startTimer() {
        if (timeState != TIME_RUNNING) {
            s.putInt("Timer", TIME_RUNNING);
            timeState = TIME_RUNNING;
        }
    }

    public static void stopTimer() {
        if (timeState == TIME_RUNNING) {
            s.putInt("Timer", TIME_STOPPED);
            timeState = TIME_STOPPED;
        }
    }

    public static void resetTimer() {
        if (timeState != TIME_RESET) {
            s.putInt("Timer", TIME_RESET);
            timeState = TIME_RESET;
        }
    }

    // Automation mode
    public static final int MO_NONE = 0;
    public static final int MO_AUTOSHOOT = 1;
    public static final int MO_SEMIAUTO = 2;
    public static final int MO_MANUAL = 3;
    public static final int MO_BALANCE = 4;
    private int mode = MO_NONE;

    public void setMode(int set) {
        mode = set;
    }

    //
    // Drive frame information
    //

    // Drive joystick
    public static double joy1Direction;
    public static double joy1Magnitude;

    // Shooter joystick
    public static double joy2X;
    public static double joy2Y;

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
    public static double shooterThrottle;
    public static double rpmTop;
    public static double rpmTopTarget;
    public static double rpmBottom;
    public static double rpmBottomTarget;
    public static double powerTop;
    public static double powerBottom;

    // Lazy-susan on the shooter
    public static double shooterRotationAngle;
    public static double shooterRotatorPower;

    // Ball feeder
    public static boolean shooting;

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
                Thread.sleep(500);
            } catch (InterruptedException ignored) {
            }
            send();
        }
    }

    public void send() {
        //Only need to send Joystick and Sensor info
        //Joystick 1
        s.putDouble("Joystick 1 Direction", joy1Direction);
        s.putDouble("Joystick 1 Magnitude", joy1Magnitude);
        s.putBoolean("Transmission", shifterStatus);

        s.putInt("Mode", mode);

        //Joystick 2
        s.putDouble("Joystick 2 X", joy2X);
        s.putDouble("Joystick 2 Y", joy2Y);
        s.putDouble("Joystick 2 Throttle", shooterThrottle);
        s.putBoolean("BallFeeder", shooting);

        //For Field - Gyro
//        s.putDouble("Gyro", gyroAngle);

        //Accelerometer
//        s.putDouble("Accel X", accel.getAcceleration().kX);
//        s.putDouble("Accel Y", accel.getAcceleration().kY);
//        s.putDouble("Accel Z", accel.getAcceleration().kZ);

        //RPM Sensor
        s.putDouble("RPM 1", (int)rpmBottom);
        s.putDouble("RPM 2", (int)rpmTop);
        s.putDouble("RPM 1 Target", (int)rpmTopTarget);
        s.putDouble("RPM 2 Target", (int)rpmBottomTarget);
    }
}
