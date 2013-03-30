package robotics.helena;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.RobotDrive;
import edu.wpi.first.wpilibj.Victor;

public class DriveBase {
    // We allow the power to be incremented/decrement this amount with
    // specific joystick button presses
    private static final double POWER_CHANGE_AMOUNT_PERCENT = 10.0 / 100.0;

    private final Victor left;
    private final Victor right;
    private final RobotDrive drive;

    // Power modification from joystick settings
    private boolean halfPowerEnabled = false;
    private double powerRatio = 1.0;

    public DriveBase() {
        right = new Victor(Configuration.RIGHT_VICTOR);
        left = new Victor(Configuration.LEFT_VICTOR);
        drive = new RobotDrive(left, right);
        drive.setInvertedMotor(RobotDrive.MotorType.kRearLeft, true);
//        drive.setInvertedMotor(RobotDrive.MotorType.kRearRight, true);
    }

    public void move(Joystick driveJoy) {
        double direction = driveJoy.getDirectionDegrees();
        double magnitude = driveJoy.getMagnitude();
        DashboardComm.driveJoyDirection = direction;
        DashboardComm.driveJoyMagnitude = magnitude;

        double ratio = modifyMagnitude(driveJoy);

        double scaleX = driveJoy.getX() * ratio;
        double scaleY = driveJoy.getY() * ratio;
        drive.arcadeDrive(scaleX, scaleY);
    }

    // Joystick power button states
    private boolean halfPowerWasPressed = false;
    private boolean restorePowerWasPressed = false;
    private boolean incrementPowerWasPressed = false;
    private boolean decrementPowerWasPressed = false;

    private double modifyMagnitude(Joystick driveJoy) {
        //
        // Check all the joystick buttons that affect power
        //

        // Half-power
        boolean nowPressed = driveJoy.getRawButton(Configuration.HALF_POWER_BUTTON);
        if (nowPressed && !halfPowerWasPressed) {
            halfPowerEnabled = !halfPowerEnabled;

            // Have the power take affect
            if (halfPowerEnabled)
                powerRatio /= 2.0;
            else
                powerRatio *= 2.0;
        }
        halfPowerWasPressed = nowPressed;

        // Increment power
        nowPressed = driveJoy.getRawButton(Configuration.INCREMENT_POWER_BUTTON);
        if (nowPressed && !incrementPowerWasPressed) {
            powerRatio += POWER_CHANGE_AMOUNT_PERCENT;
        }
        incrementPowerWasPressed = nowPressed;

        // Decrement power
        nowPressed = driveJoy.getRawButton(Configuration.DECREMENT_POWER_BUTTON);
        if (nowPressed && !decrementPowerWasPressed) {
            powerRatio -= POWER_CHANGE_AMOUNT_PERCENT;
        }
        decrementPowerWasPressed = nowPressed;

        // Last, reset/restore power to stock settings
        nowPressed = driveJoy.getRawButton(Configuration.RESTORE_POWER_BUTTON);
        if (nowPressed && !restorePowerWasPressed) {
            // Reset back to stock
            halfPowerEnabled = false;
            powerRatio = 1.0;
        }
        restorePowerWasPressed = nowPressed;

        // Limits
        if (powerRatio > 1.0)
            powerRatio = 1.0;
        else if (powerRatio < POWER_CHANGE_AMOUNT_PERCENT)
            powerRatio = POWER_CHANGE_AMOUNT_PERCENT;

        DashboardComm.drivePowerRatio = powerRatio;

        // Apply the magnitude modifications to the joystick power
        return powerRatio;
    }
}
