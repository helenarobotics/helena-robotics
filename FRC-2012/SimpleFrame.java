package robotics.helena;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.RobotDrive;
import edu.wpi.first.wpilibj.SimpleRobot;
import edu.wpi.first.wpilibj.Solenoid;
import edu.wpi.first.wpilibj.Victor;

public class SimpleFrame extends SimpleRobot {
    // Drivetrain
    private Victor leftMotor;
    private Victor rightMotor;
    private Solenoid shifter;

    // Shooter
    private Victor shooterMotor;

    SimpleFrame() {
        System.out.println("Starting SimpleFrame");
        leftMotor = new Victor(1);
        rightMotor = new Victor(2);
        shooterMotor = new Victor(3);
        disabled();

        // Setup the transmission shifter.
        shifter = new Solenoid(1);
        shifter.set(true);
    }

    protected void disabled() {
        leftMotor.set(0);
        rightMotor.set(0);
        shooterMotor.set(0);
    }

    /**
     * This function is called once each time the robot enters autonomous mode.
     */
    public void autonomous() {
        System.out.println("No Autonomous mode code");
    }
    /**
     * This function is called once each time the robot enters operator control.
     */
    public void operatorControl() {
        System.out.println("Starting operatorControl");

        Joystick driveStick = new Joystick(1);
        Joystick shootStick = new Joystick(2);
        RobotDrive drive = new RobotDrive(leftMotor, rightMotor);

        drive.setInvertedMotor(RobotDrive.MotorType.kFrontLeft, false);
        drive.setInvertedMotor(RobotDrive.MotorType.kFrontRight, false);

        // Turn off drive safety which causes the motors to stop running
        // if they don't get a command within a couple of ms.
        drive.setSafetyEnabled(false);

        while (isOperatorControl() && isEnabled()) {
            // Ignore the dead-spot near the center of the drive joystick
            if (Math.abs(driveStick.getMagnitude()) > 0.05) {
                // Add greater sensitivity at lower magnitudes
                drive.arcadeDrive(driveStick, true);
            }
            joystickShifter(driveStick);

            // Read the throttle to determine the speed of the shooter
            // motor
            shooterMotor.set(shootStick.getThrottle());
        }
    }

    private static final int TRIGGER_BTN = 1;
    private boolean wasPressed = false;
    private void joystickShifter(Joystick stick) {
        // Toggle the shifter when the shifter button is pressed
        boolean nowPressed = stick.getRawButton(TRIGGER_BTN);
        if (nowPressed && !wasPressed) {
            shifter.set(!shifter.get());
        }
        wasPressed = nowPressed;
    }
}
