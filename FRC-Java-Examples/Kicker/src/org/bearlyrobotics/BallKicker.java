package com.bearlyrobotics;

import edu.wpi.first.wpilibj.Compressor;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.RobotDrive;
import edu.wpi.first.wpilibj.SimpleRobot;
import edu.wpi.first.wpilibj.Solenoid;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.Watchdog;
import edu.wpi.first.wpilibj.camera.AxisCamera;

public class BallKicker extends SimpleRobot {
    //
    // Some constants that need to be tweaked for the robot
    //

    // How fast do we go when moving straight
    private static final double STRAIGHT_SPEED = 50 / 100;

    // How long does it take (in seconds) to move one foot if going
    // STRAIGHT_SPEED?
    private static final double MOVE_ONEFOOT_FORWARD_TIME = 0.7;

    // How fast do we spin?
    private static final double SPIN_SPEED = 25 / 100;

    // How long does it take us to spin in a full circle at SPIN_SPEED?
    private static final double FULL_CIRCLE_SPIN_TIME = 3.5;

    // The kicker setup
    private Compressor compressor;
    private Solenoid kicker;

    // The setup for moving the robot
    private RobotDrive drive;

    // Teleop control
    private Joystick joyStick;

    // Program startup begins here
    public BallKicker() {
        System.out.println("Starting Bearly BallKicker");

        // Create and startup the compressor using the default slots and
        // relay and pressure switch channels 1 and 1.
        compressor = new Compressor(1, 1);
        compressor.start();

        // This solenoid is what controls the kicker, and is located in
        // slot 8, channel 1.
        kicker = new Solenoid(8, 1);

        // Two motor setup
        drive = new RobotDrive(1, 2);

        // Create the joystick that will be used to control the 'bot
        // connected to USB port #1
        joyStick = new Joystick(1);

        // Startup the Camera
        AxisCamera.getInstance().writeCompression(0);
        AxisCamera.getInstance().writeBrightness(10);
        AxisCamera.getInstance().writeResolution(
            AxisCamera.ResolutionT.k160x120);
    }

    public void robotInit() {
        // Reset the robot to normal, in case it's not
        drive.drive(0.0, 0.0);

        // XXX - Repoint the camera back to center?
    }

    public void disabled() {
        // Nothing (yet)
    }

    /**
     * This function is called once each time the robot enters autonomous mode.
     */
    public void autonomous() {
        System.out.println("Autonomous start");

        // Turn off drive safety which causes the motors to stop running
        // if they don't get a command within a couple of ms.
        drive.setSafetyEnabled(false);

        // Make sure the motors are setup 'right'
        drive.setInvertedMotor(RobotDrive.MotorType.kRearLeft, false);
        drive.setInvertedMotor(RobotDrive.MotorType.kRearRight, false);

        // The order of events is:
        // * Go forward 5 feet
        // * Kick ball
        // * Go back 5 feet
        // * Turn left 45
        // * Go forward 5'
        // * Kick Ball
        // * Go back 5 feet
        // * Turn right 90
        // * Go forward 5'
        // * Kick Ball

        // Make sure we're stopped
        move(STOP, 0);

        // Kick the first ball and move back to the starting position.
        //  - Move forward 3.5 feet
        //  - Kick the ball
        //  - Backup 3.5 feet
        //  - Stop for ~1 second
        move(MOVE_STRAIGHT, 3.5);
        kickTheBall();
        move(MOVE_STRAIGHT, -3.5);
        move(STOP, 1.0);

        // Move to the second ball on the left, kick it, and then
        // move back to the starting location.
        //  - Spin left 45 degrees
        //  - Move forward 5.2 feet
        //  - Spin right 90 degrees
        //  - Move forward 1.5 feet
        //  - Kick the ball
        //  - Backup 1.5 feet
        //  - Spin left 90 degrees
        //  - Backup 5.2 feet
        //  - Stop for ~1 second
        move(SPIN, -45);
        move(MOVE_STRAIGHT, 5.2);
        move(SPIN, 90);
        move(MOVE_STRAIGHT, 1.5);
        kickTheBall();
        move(MOVE_STRAIGHT, -1.5);
        move(SPIN, -90);
        move(MOVE_STRAIGHT, -5.2);
        move(STOP, 1.0);

        // Finally, move to the ball on the right, kick it, and move
        // back to the starting point.
        move(SPIN, 90);
        move(MOVE_STRAIGHT, 5.2);
        move(SPIN, -90);
        move(MOVE_STRAIGHT, 1.5);
        kickTheBall();
        move(MOVE_STRAIGHT, -1.5);
        move(SPIN, 90);
        move(MOVE_STRAIGHT, -5.2);
        move(SPIN, -45);
        move(STOP, 1.0);

        drive.setSafetyEnabled(true);
        System.out.println("Autonomous end");
    }

    // XXX - Should be an enum, but not supported in the FRC JVM
    private static final int STOP = 0;
    private static final int MOVE_STRAIGHT = 1;
    private static final int SPIN = 3;

    // Move the robot!
    private void move(int cmd, double amt) {
        // The last thing we do after everything else is stop the
        // motors, which is the reason for try/finally.
        try {
            double motorSpeed = 0;
            double turnAngle = 0;
            double waitTime = 0;

            switch (cmd) {
            case STOP:
                // Don't need to do anything else, since the defaults
                // are all correct.
                System.out.println("Stop for " + amt);
                break;
            case MOVE_STRAIGHT:
                // Convert the amount to move (in feet) to a time that
                // it will take us to move that far.
                waitTime = Math.abs(amt * MOVE_ONEFOOT_FORWARD_TIME);
                motorSpeed = STRAIGHT_SPEED;
                if (amt < 0)
                    motorSpeed = -motorSpeed;
                System.out.println("Move for " + (int)(waitTime * 1000) +
                                   " ms @ " + motorSpeed);
                break;
            case SPIN:
                // Sanity check
                if (amt < -360 || amt > 360) {
                    System.out.println("Bad turn amount " + amt);
                    return;
                }
                // If people give us weird angles, conver them back more
                // effecient ones.  This normalizes the angles from -180
                // <> +180
                if (amt > 180)
                    amt = amt - 360;
                else if (amt < -180)
                    amt = amt + 360;

                // Spin right for positive, spin left for negative.
                motorSpeed = SPIN_SPEED;
                if (amt > 0)
                    turnAngle = 1.0;
                else
                    turnAngle = -1.0;

                // Finally, calculate the time we take to make a circle
                // is a fraction of the time it takes to make a full
                // circle.
                waitTime = Math.abs(FULL_CIRCLE_SPIN_TIME * amt / 360);
                System.out.println("SPIN(" + amt + ") for " +
                                   (int)(waitTime * 1000) + " ms @ " +
                                   turnAngle);
                break;
            default:
                System.out.println("Unknown CMD " + cmd);
                break;
            }
            drive.drive(motorSpeed, turnAngle);
            Timer.delay(waitTime);
        } finally {
            // Shutoff the motor
            System.out.println("Stop");
            drive.drive(0.0, 0.0);
        }
    }

    /**
     * This function is called once each time the robot enters operator
     * control.
     */
    public void operatorControl() {
        System.out.println("Teleop start");
        // Reverse the motors due to the way the joyStick is setup to
        // make the robot behave correctly.
        drive.setInvertedMotor(RobotDrive.MotorType.kRearLeft, true);
        drive.setInvertedMotor(RobotDrive.MotorType.kRearRight, true);

        while (isOperatorControl() && isEnabled()) {
            // Only kick the ball if we've got enough pressure
            // and the button is pressed
            if (checkButton(joyStick) &&
                compressor.getPressureSwitchValue())
                kickTheBall();

            // Note, we increase sensitivity when we go slow...
            drive.arcadeDrive(joyStick, true);
            Timer.delay(0.005);

            // feed the user watchdog at every period when in autonomous
            Watchdog.getInstance().feed();
        }
        System.out.println("Teleop end");
    }

    private static final int TRIGGER = 1;
    private boolean wasPressed = false;
    private boolean checkButton(Joystick stick) {
        // Was the button pressed?
        boolean released = false;

        // Fire when the button is released
        boolean nowPressed = stick.getRawButton(TRIGGER);
        if (!nowPressed && wasPressed) {
            released = true;
            System.out.println("button released");
        }
        wasPressed = nowPressed;
        return released;
    }

    private void kickTheBall() {
        System.out.println("FIRE");
        kicker.set(true);
        // Wait a half-second
        Timer.delay(0.5);

        // Done kicking, et the compressor build up
        kicker.set(false);
    }
}
