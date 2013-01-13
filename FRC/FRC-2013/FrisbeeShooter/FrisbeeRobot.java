package robotics.helena;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.RobotDrive;
import edu.wpi.first.wpilibj.Solenoid;
import edu.wpi.first.wpilibj.SimpleRobot;
import edu.wpi.first.wpilibj.Victor;

public class FrisbeeRobot extends SimpleRobot {
    private Victor leftMotor;
    private Victor rightMotor;
    private Solenoid shifter;

    // Frisbee Thrower
    private FrisbeeThrower thrower;

    FrisbeeRobot() {
        System.out.println("Starting FrisbeeRobot");
        leftMotor = new Victor(Configuration.VICTOR_LEFT_DRIVE);
        rightMotor = new Victor(Configuration.VICTOR_RIGHT_DRIVE);
        leftMotor.set(0);
        rightMotor.set(0);

        // Setup the transmission shifter.
        shifter = new Solenoid(Configuration.SHIFTER_SOLENOID);
        shifter.set(true);

        // Initialize the thrower
        thrower = new FrisbeeThrower();
    }

    protected void disabled() {
        leftMotor.set(0);
        rightMotor.set(0);
        thrower.disable();
    }

    public void autonomous() {
        System.out.println("Autonomous mode running");

        System.out.println("Autonomous mode completed");
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

        // The thrower is controlled with the second joystick
        Joystick throwStick = new Joystick(Configuration.JOYSTICK_THROWER);

        // Setup the drive system to move the robot!
        RobotDrive drive = new RobotDrive(leftMotor, rightMotor);
        drive.setInvertedMotor(RobotDrive.MotorType.kFrontLeft, false);
        drive.setInvertedMotor(RobotDrive.MotorType.kFrontRight, false);

        // Turn off drive safety which causes the motors to stop running
        // if they don't get a command within a couple of ms, but
        // doesn't appear to work properly.
        drive.setSafetyEnabled(false);

        while (isOperatorControl() && isEnabled()) {
            // Feed the watchdog to keep the drive motors running
//            getWatchdog().feed();

            // Setting the second parameter to true adds greater
            // sensitivity at lower speeds.
            drive.arcadeDrive(driveStick, true);

            // Shift the transmission when the button is pressed
            if (joystickTrigger(driveStick)) {
                shifter.set(!shifter.get());
            }

            // Control the thrower with the second joystick.
            thrower.joystickControl(throwStick);
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
