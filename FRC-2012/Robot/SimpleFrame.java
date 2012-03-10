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
    private Shooter shooter;

    SimpleFrame() {
        super();
        System.out.println("Starting SimpleFrame");

        // Initialize the drive train
        leftMotor = new Victor(1);
        rightMotor = new Victor(2);
        leftMotor.set(0);
        rightMotor.set(0);

        // Setup the transmission shifter.
        shifter = new Solenoid(1);
        shifter.set(true);

        // Initialize the shooter
        shooter = new Shooter(3, 4);
    }

    protected void disabled() {
        leftMotor.set(0);
        rightMotor.set(0);
        shooter.disable();
    }

    /**
     * This function is called once each time the robot enters autonomous mode.
     */
    public void autonomous() {
        System.out.println("No Autonomous mode code");
    }

    /**
     * This function is called once each time the robot enters operator
     * control.
     */
    public void operatorControl() {
        System.out.println("Starting operatorControl");

        Joystick driveStick = new Joystick(1);
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
            // motor and convert it to a number between 0 and 1 and use
            // it to set the RPM of the shooter.
            double throttle = (1.0 + driveStick.getThrottle()) / 2.0;
            shooter.setRPM(throttle * Shooter.MAX_RPM);
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
