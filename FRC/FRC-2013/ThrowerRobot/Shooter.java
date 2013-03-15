package robotics.helena;

import edu.wpi.first.wpilibj.Counter;
import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.Victor;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

public class Shooter {
    public static final int MAX_RPM = 225;
    private static final int RPM_TOLERANCE = 5;
    private final RPMEncoder encoder;
    private final PIDVictor shooter1;
    private final PIDVictor shooter2;

    public Shooter() {
        encoder = new RPMEncoder();
        shooter1 = new PIDVictor(
            "Shooter1", new Victor(Configuration.SHOOTER1_VICTOR), encoder,
            1.0, 0.0, 0.0,
            RPM_TOLERANCE, MAX_RPM);
        shooter2 = new PIDVictor(
            "Shooter2", new Victor(Configuration.SHOOTER2_VICTOR), encoder,
            1.0, 0.0, 0.0,
            RPM_TOLERANCE, MAX_RPM);
        shooter1.setPower(0);
        shooter2.setPower(0);
        SmartDashboard.putNumber("Power", 0);
    }

    public void setPower(double power) {
        if (getPower() != power) {
            shooter1.setPower(power);
            shooter2.setPower(power);
            SmartDashboard.putNumber("Throttle Power", power);
        }
    }

    public double getPower() {
        return shooter1.getPower();
    }
}
