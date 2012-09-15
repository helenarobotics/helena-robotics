package org.bearlyrobotics;

import edu.wpi.first.wpilibj.Jaguar;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.RobotDrive;
import edu.wpi.first.wpilibj.SimpleRobot;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.Watchdog;

public class MecRobot extends SimpleRobot {
    // These are connected to the mechanum wheels
    private Jaguar frontLeft;
    private Jaguar rearLeft;
    private Jaguar frontRight;
    private Jaguar rearRight;

    // Program startup begins here
    public MecRobot() {
        System.out.println("Starting Bearly MecRobot");

        // Verify these are correct
        frontLeft = new Jaguar(1);
        rearLeft = new Jaguar(2);
        frontRight = new Jaguar(3);
        rearRight = new Jaguar(4);
    }

    public void robotInit() {
        // Reset the robot to normal, in case it's not
        disabled();
    }

    public void disabled() {
        // Stop all the motors
        frontLeft.stopMotor();
        rearLeft.stopMotor();
        frontRight.stopMotor();
        rearRight.stopMotor();
    }

    /**
     * This function is called once each time the robot enters autonomous mode.
     */
    public void autonomous() {
        System.out.println("Auto start");

        System.out.println("Verifying correct motor is moving forward");
        for (int i = 1; i <= 4; i++) {
            System.out.print("Motor " + i + " - ");
            Jaguar motor = null;
            switch (i) {
            case 1:
                System.out.println("Front-Left");
                motor = frontLeft;
                break;
            case 2:
                System.out.println("Rear-Left");
                motor = rearLeft;
                break;
            case 3:
                System.out.println("Front-Right");
                motor = frontRight;
                break;
            case 4:
                System.out.println("Rear-Right");
                motor = rearRight;
                break;
            }

            // Give the motor 50% power going forward
            motor.set(0.5);

            // Give us operator some time to verify the motors
            Timer.delay(5.0);

            // Shutoff the motor
            motor.stopMotor();
        }

        System.out.println("Auto end");
    }

    /**
     * This function is called once each time the robot enters operator
     * control.
     */
    public void operatorControl() {
        System.out.println("Teleop start");

        // Create the joystick that will be used to control the 'bot
        // connected to USB port #1
        Joystick joyStick = new Joystick(1);

        // Four motor Mecanum setup for moving the robot
        //   Front-Left
        //   Rear-Left
        //   Front-Right
        //   Rear-Right
        RobotDrive drive = new RobotDrive(frontLeft, rearLeft, frontRight, rearRight);

        // Reverse the motors due to the way the joyStick is setup to
        // make the robot behave correctly.
        drive.setInvertedMotor(RobotDrive.MotorType.kFrontLeft, true);
        drive.setInvertedMotor(RobotDrive.MotorType.kRearLeft, true);
        drive.setInvertedMotor(RobotDrive.MotorType.kFrontRight, false);
        drive.setInvertedMotor(RobotDrive.MotorType.kRearRight, false);

        while (isOperatorControl() && isEnabled()) {
            double power = joyStick.getMagnitude();
            // Leave a dead-zone around the center of the stick
            if (Math.abs(power) < 0.05)
                power = 0.0;
            double direction = joyStick.getDirectionDegrees();
            double strafe = joyStick.getTwist();

            // Note, we increase sensitivity when we go slow...
            drive.mecanumDrive_Polar(power, direction, strafe);

            // feed the user watchdog at every period when in autonomous
            Watchdog.getInstance().feed();
        }
        System.out.println("Teleop end");
    }
}
