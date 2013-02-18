package robotics.helena;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.RobotDrive;
import edu.wpi.first.wpilibj.Victor;

public class DriveBase {
    private final Victor frontLeft;
    private final Victor frontRight;
    private final Victor rearLeft;
    private final Victor rearRight;
    private final RobotDrive drive;

    public DriveBase() {
        frontLeft = new Victor(Configuration.FRONT_LEFT_VICTOR);
        frontRight = new Victor(Configuration.FRONT_RIGHT_VICTOR);
        rearLeft = new Victor(Configuration.REAR_LEFT_VICTOR);
        rearRight = new Victor(Configuration.REAR_RIGHT_VICTOR);
        drive = new RobotDrive(frontLeft, rearLeft, frontRight, rearRight);
        /*
        drive.setInvertedMotor(RobotDrive.MotorType.kFrontLeft, false);
        drive.setInvertedMotor(RobotDrive.MotorType.kRearLeft, false);
        drive.setInvertedMotor(RobotDrive.MotorType.kFrontRight, false);
        drive.setInvertedMotor(RobotDrive.MotorType.kRearRight, false);
        */
    }

    void move(Joystick driveJoy) {
        drive.arcadeDrive(driveJoy);
    }
}
