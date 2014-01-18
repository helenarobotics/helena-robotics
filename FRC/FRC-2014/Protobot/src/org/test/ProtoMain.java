/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package org.test;


import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.RobotDrive;
import edu.wpi.first.wpilibj.SimpleRobot;
import edu.wpi.first.wpilibj.Solenoid;
import edu.wpi.first.wpilibj.Victor;

/**
 * The VM is configured to automatically run this class, and to call the
 * functions corresponding to each mode, as described in the SimpleRobot
 * documentation. If you change the name of this class or the package after
 * creating this project, you must also update the manifest file in the resource
 * directory.
 */
public class ProtoMain extends SimpleRobot {
    Victor frontLeftDrive;
    Victor frontRightDrive;
    Victor backLeftDrive;
    Victor backRightDrive;
    RobotDrive drive;
    Joystick driveJoystick;
    Joystick auxJoystick;
    Solenoid mainLift;
    Solenoid jaw;
    
    public  ProtoMain() {
        System.out.println("Hello, y'all.");
        frontLeftDrive = new Victor(Config.FRONT_LEFT_VICTOR);
        frontRightDrive = new Victor(Config.FRONT_RIGHT_VICTOR);
        backLeftDrive = new Victor(Config.BACK_LEFT_VICTOR);
        backRightDrive = new Victor(Config.BACK_RIGHT_VICTOR);

        drive = new RobotDrive(frontLeftDrive, frontRightDrive,
                               backLeftDrive,  backRightDrive);
        driveJoystick = new Joystick(Config.DRIVE_JOYSTICK);
        auxJoystick = new Joystick(Config.AUXILIARY_JOYSTICK);
        //mainLift = new Solenoid(Config.LIFT_SOLENOID);
        jaw = new Solenoid(Config.JAW_SOLENOID);
    }
    
    /**
     * This function is called once each time the robot enters autonomous mode.
     */
    public void autonomous() {
        Victor testVictor = new Victor(3);
        testVictor.set(1.0d);
        try {
            Thread.sleep(1000);
        } catch(InterruptedException i) {
            
        }
        testVictor.set(0d);
    }

    /**
     * This function is called once each time the robot enters operator control.
     */
    
    private boolean lastLiftInput = false;
    public void operatorControl() {
        drive.setSafetyEnabled(false);
        
        while(isOperatorControl()) {
            //Drive
            switch(Config.DRIVE_TYPE) {
                case Config.ARCADE:
                    drive.arcadeDrive(driveJoystick);
                    break;
                case Config.MECHANUM:
                    drive.mecanumDrive_Polar(driveJoystick.getMagnitude(), driveJoystick.getDirectionDegrees(), driveJoystick.getTwist());
                    break;
                default:
                    System.err.println("###Unsupported Drive Type!");
                    break;
            }
            
            //Control Lift Solenoid
            /*if(auxJoystick.getRawButton(Config.LIFT_BUTTON) && !lastLiftInput) {
                mainLift.set(!mainLift.get());
                System.out.println("Kicker toggled to " + mainLift.get());
            }
            
            lastLiftInput = auxJoystick.getRawButton(Config.LIFT_BUTTON);
            */
            //Control Jaw Solenoid
            if(auxJoystick.getAxis(Joystick.AxisType.kY) > 0.5) {
                jaw.set(true);
            } else {
                jaw.set(false);
            }
        }
    }
    
    /**
     * This function is called once each time the robot enters test mode.
     */
    public void test() {
    
    }
}
