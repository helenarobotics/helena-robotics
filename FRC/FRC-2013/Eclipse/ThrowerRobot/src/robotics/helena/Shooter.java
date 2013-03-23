package robotics.helena;

import edu.wpi.first.wpilibj.Victor;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

public class Shooter {
    public static final int MAX_RPM = 225;
    private static final int RPM_TOLERANCE = 5;
    private final PIDVictor shooter;

    public Shooter() {
        shooter = new PIDVictor(
            new Victor(Configuration.SHOOTER1_VICTOR),
            new Victor(Configuration.SHOOTER2_VICTOR),
            new RPMEncoder(Configuration.SHOOTER_ENCODER),
            1.0, 0.0, 0.0,
            RPM_TOLERANCE, MAX_RPM);
        shooter.setPower(0);
    }

    public void setPower(double power) {
        if (getPower() != power) {
            shooter.setPower(power);
            SmartDashboard.putNumber("Throttle Power", power);
        }
    }

    public double getPower() {
        return shooter.getPower();
    }
}
