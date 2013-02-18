package robotics.helena;

import edu.wpi.first.wpilibj.Counter;
import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.Victor;

public class Shooter {
    static final int MAX_RPM = 500;
    private final Victor shooter1;
    private final Victor shooter2;
    private final Counter encoder;
    private double curPower;
    private long powerChangeTime = 0;

    public Shooter() {
        shooter1 = new Victor(Configuration.SHOOTER1_VICTOR);
        shooter2 = new Victor(Configuration.SHOOTER2_VICTOR);
        encoder = new Counter(new DigitalInput(2));
        encoder.start();
        shooter1.set(0);
        shooter2.set(0);
    }

    public void setPower(double power) {
        if (curPower != power) {
            curPower = power;
            shooter1.set(curPower);
            shooter2.set(curPower);
            powerChangeTime = System.currentTimeMillis();
        }
    }

    public long getChangeTime() {
        return powerChangeTime;
    }

    public double getPower() {
        return shooter1.get();
    }
}
