package robotics.helena;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.RobotDrive;
import edu.wpi.first.wpilibj.SimpleRobot;
import edu.wpi.first.wpilibj.Solenoid;
import edu.wpi.first.wpilibj.Victor;

public class ShooterRobot extends SimpleRobot {
    private Victor leftMotor;
    private Victor rightMotor;
    private Solenoid shifter;

    // Shooter
    private Shooter shooter;

    // Gyro
    private GyroSensor gyro;

    ShooterRobot() {
        System.out.println("Starting ShooterRobot");
        leftMotor = new Victor(Configuration.VICTOR_LEFT_DRIVE);
        rightMotor = new Victor(Configuration.VICTOR_RIGHT_DRIVE);
        leftMotor.set(0);
        rightMotor.set(0);

        // Setup the transmission shifter.
//        shifter = new Solenoid(1);
//        shifter.set(true);

        // The gyro
//        gyro = new GyroSensor(Configuration.GYRO_ANALOG_CHANNEL, 100);

        // Initialize the shooter
        shooter = new Shooter();
    }

    protected void disabled() {
        leftMotor.set(0);
        rightMotor.set(0);
        shooter.disable();
    }

    public void autonomous() {
        System.out.println("No Autonomous mode code");
    }

    /**
     * This function is called once each time the robot enters operator
     * control.
     *
     */
    public void operatorControl() {
        System.out.println("Starting operatorControl");

        // Drive control is done with the first joystick
        Joystick driveStick = new Joystick(Configuration.JOYSTICK_DRIVER);

        // The shooter is controlled with the second joystick
        Joystick shootStick = new Joystick(Configuration.JOYSTICK_SHOOTER);

        // Setup the drive system to move the robot!
        RobotDrive drive = new RobotDrive(leftMotor, rightMotor);
        drive.setInvertedMotor(RobotDrive.MotorType.kFrontLeft, false);
        drive.setInvertedMotor(RobotDrive.MotorType.kFrontRight, false);

        // Turn on drive safety which causes the motors to stop running
        // if they don't get a command within a couple of ms.
        drive.setSafetyEnabled(true);

        while (isOperatorControl() && isEnabled()) {
            // Feed the watchdog to keep the drive motors running
            getWatchdog().feed();

            // Ignore the dead-spot near the center of the joystick
            if (Math.abs(driveStick.getMagnitude()) > 0.05) {
                // Setting the second parameter to true adds greater
                // sensitivity at lower speeds.
                drive.arcadeDrive(driveStick, true);
            }
                
            // Shift the transmission when the button is pressed
            if (joystickTrigger(driveStick)) {
//                shifter.set(!shifter.get());
//                DataLogger.shifterStatus = shifter.get();
            }

            // Control the shooter with the second joystick.
            shooter.joystickControl(shootStick);
        }
    }

    private static final int TRIGGER_BTN = 1;
    private boolean wasPressed = false;
    private boolean joystickTrigger(Joystick joy) {
        boolean btnPressed = false;

        // Toggle the shifter when the shifter button is pressed
        boolean nowPressed = joy.getRawButton(TRIGGER_BTN);
        if (nowPressed && !wasPressed) {
            btnPressed = true;
        }
        wasPressed = nowPressed;

        return btnPressed;
    }
}