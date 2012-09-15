package robotics.helena;

import edu.wpi.first.wpilibj.DriverStationLCD;
import edu.wpi.first.wpilibj.Jaguar;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.RobotDrive;
import edu.wpi.first.wpilibj.Servo;
import edu.wpi.first.wpilibj.SimpleRobot;
import edu.wpi.first.wpilibj.Solenoid;
import edu.wpi.first.wpilibj.Victor;

public class SimpleFrame extends SimpleRobot {
    private Victor leftMotor;
    private Victor rightMotor;
    private Victor shooterMotor;
    private Victor shooter2Motor;
    private Jaguar rotation;
    //private Solenoid shifter;
    private Servo launcher;
    private static final float LAUNCH = 60;
    private static final float NOT_LAUNCH = 360;
    private boolean launch = true;
    private boolean launchWP = false;
    private DriverStationLCD driverStation = DriverStationLCD.getInstance();

    SimpleFrame() {
        System.out.println("Starting SimpleFrame");
        leftMotor = new Victor(1);
        rightMotor = new Victor(2);
        shooterMotor = new Victor(3);
        shooter2Motor = new Victor(4);
        rotation = new Jaguar(6);
        launcher = new Servo(1, 8);
        disabled();

        // Setup the transmission shifter.
        /*shifter = new Solenoid(1);
        shifter.set(true);*/
    }

    protected void disabled() {
        leftMotor.set(0);
        rightMotor.set(0);
        shooterMotor.set(0);
        launcher.set(LAUNCH);
    }

    public void autonomous() {
        System.out.println("No Autonomous mode code");
    }
    /**
     * This function is called once each time the robot enters operator control.
     */
    public void operatorControl() {
        System.out.println("Starting operatorControl");


        Joystick driveStick = new Joystick(1);
        Joystick rotStick = new Joystick(2);
        RobotDrive drive = new RobotDrive(leftMotor, rightMotor);

        drive.setInvertedMotor(RobotDrive.MotorType.kFrontLeft, false);
        drive.setInvertedMotor(RobotDrive.MotorType.kFrontRight, false);
       
        // Turn off drive safety which causes the motors to stop running
        // if they don't get a command within a couple of ms.
        drive.setSafetyEnabled(false);

        while (isOperatorControl() && isEnabled())
        {
            // Ignore the dead-spot near the center of the joystick
            if (Math.abs(driveStick.getMagnitude()) > 0.05)
            {
                // Add greater sensitivity at lower magnitudes
                drive.arcadeDrive(driveStick, true);
            }
            //joystickShifter(driveStick);

            rotation.set(rotStick.getAxis(Joystick.AxisType.kY));

            boolean trigBool = rotStick.getTrigger();
            if(trigBool && !launchWP)
                launch = !launch;
            launchWP = trigBool;

            if(launch)
                launcher.setAngle(LAUNCH);
            else
                launcher.setAngle(NOT_LAUNCH);

            shooterMotor.set(driveStick.getThrottle());
            shooter2Motor.set(driveStick.getThrottle());

            //Read the throttle to determin the speed of the shooter motor
            if(launch)
                driverStation.println(DriverStationLCD.Line.kMain6,1,"true");
            else
                driverStation.println(DriverStationLCD.Line.kMain6,1,"false");
            driverStation.updateLCD();
        }
    }

    /*private static final int TRIGGER_BTN = 1;
    private boolean wasPressed = false;
    private void joystickShifter(Joystick stick) {
        // Toggle the shifter when the shifter button is pressed
        boolean nowPressed = stick.getRawButton(TRIGGER_BTN);
        if (nowPressed && !wasPressed) {
            shifter.set(!shifter.get());
        }
        wasPressed = nowPressed;
    }*/
}