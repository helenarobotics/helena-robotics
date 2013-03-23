package robotics.helena;

import edu.wpi.first.wpilibj.Victor;

public class Shooter {
    public static final int MAX_RPM = 225;
    private static final int RPM_TOLERANCE = 5;
    private final PIDVictor pidShooter;

    public Shooter() {
        pidShooter = new PIDVictor(
                new Victor(Configuration.SHOOTER1_VICTOR),
                new Victor(Configuration.SHOOTER2_VICTOR),
                new RPMEncoder(Configuration.SHOOTER_ENCODER),
                1.0, 0.0, 0.0,
                RPM_TOLERANCE, MAX_RPM);
        pidShooter.setPower(0);
    }

    public void setPower(double power) {
        if (pidShooter.getPower() != power) {
            pidShooter.setPower(power);
        }
    }
}
