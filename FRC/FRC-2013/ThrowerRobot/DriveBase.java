package robotics.helena;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.RobotDrive;
import edu.wpi.first.wpilibj.Victor;

public class DriveBase {
    private final Victor left;
    private final Victor right;
    private final RobotDrive drive;

    public DriveBase() {
        right = new Victor(Configuration.RIGHT_VICTOR);
        left = new Victor(Configuration.LEFT_VICTOR);
        drive = new RobotDrive(left, right);
/*
        drive.setInvertedMotor(RobotDrive.MotorType.kFrontLeft, false);
        drive.setInvertedMotor(RobotDrive.MotorType.kFrontRight, false);
*/
    }

    void move(Joystick driveJoy) {
        drive.arcadeDrive(driveJoy);
    }
}
