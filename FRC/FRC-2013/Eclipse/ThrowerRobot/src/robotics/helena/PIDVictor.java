package robotics.helena;

import edu.wpi.first.wpilibj.PIDController;
import edu.wpi.first.wpilibj.PIDOutput;
import edu.wpi.first.wpilibj.PIDSource;
import edu.wpi.first.wpilibj.Victor;

public class PIDVictor {
    private final Victor motor1;
    private final Victor motor2;
    private final VictorPIDSource rpmSrc;
    private final VictorPIDOutput motorCtl;
    private final PIDController pidCtl;
    private final double rpmTolerance;

    private boolean enabled;

    public PIDVictor(Victor _motor1,
                     Victor _motor2,
                     RPMEncoder encoder,
                     double kP, double kI, double kD,
                     double _rpmTolerance, double maxRpm) {
        motor1 = _motor1;
        motor2 = _motor2;
        rpmSrc = new VictorPIDSource(encoder);
        motorCtl = new VictorPIDOutput(motor1, motor2);
        pidCtl = new PIDController(kP, kI, kD, rpmSrc, motorCtl);

        // RPM tolerance
        rpmTolerance = _rpmTolerance;
        pidCtl.setTolerance(rpmTolerance);

        // Set the minimum and maximum RPM range
        pidCtl.setInputRange(0, maxRpm);

        // The motor range is actually all positive (actually negative
        // and limited in the PIDOutput cass), but we need it to be able
        // to go negative in order to reduce power.
        pidCtl.setOutputRange(-1.0, 1.0);

        // By default, we're not enabled until we get a target value
        enabled = false;
    }

    private class VictorPIDSource implements PIDSource {
        final RPMEncoder encoder;
        double rpm;

        VictorPIDSource(final RPMEncoder _encoder) {
            encoder = _encoder;
        }

        public double pidGet() {
            rpm = encoder.getRPM();
            DashboardComm.rpmThrower = rpm;
            return rpm;
        }
    }

    private class VictorPIDOutput implements PIDOutput {
        final Victor motor1;
        final Victor motor2;
        double currValue = 0.0;

        VictorPIDOutput(final Victor _motor1, final Victor _motor2) {
            motor1 = _motor1;
            motor2 = _motor2;
        }

        public void feedForward(double _currValue) {
            currValue = _currValue;
        }

        public void pidWrite(double output) {
            // RPM or speed control needs to take into account the
            // previous value, since we want to 'slow' the motor down,
            // not stop and reverse it.

            // Output is subtracted since we use negative powers to give
            // us the proper direction on the robot.  Note, the RPM
            // counter doesn't care the direction, just the value of the
            // RPM.
            double newVal = currValue - output;

            // Limit power from 0 -> -1.0 to keep the motor from
            // reversing itself if the controls oscillate too
            // far one way or the other.
            currValue = Math.min(-1.0, Math.max(0.0, newVal));

            DashboardComm.powerThrower = currValue;
            motor1.set(currValue);
            motor2.set(currValue);
        }
    }

    public void disable() {
        enabled = false;
        pidCtl.reset();
        motor1.set(0);
        motor2.set(0);
    }

    public double getPower() {
        return motorCtl.currValue;
    }

    public void setPower(double power) {
        // If PID is disabled, pass the power directly
        // to the motor raw.
        if (enabled) {
            motor1.set(power);
            motor2.set(power);
        }
        else
            setTargetPower(power);
    }

    public void setTargetPower(double power) {
        enabled = true;
        motorCtl.feedForward(power);
        DashboardComm.throwerThrottle = power;
    }

    public double getRpm() {
        return rpmSrc.rpm;
    }

    public void setTargetRpm(double rpm) {
        enabled = true;
        if (rpm == 0) {
            if (pidCtl.isEnable()) {
                pidCtl.reset();
                pidCtl.setSetpoint(0);
                setTargetPower(0);
                rpmSrc.rpm = 0;
            }
            motor1.set(0);
            motor2.set(0);
        } else {
            if (!pidCtl.isEnable())
                pidCtl.enable();
            pidCtl.setSetpoint(rpm);
        }
    }

    public boolean onTarget() {
        double diff = Math.abs(getRpm() - pidCtl.getSetpoint());
        return (diff < rpmTolerance);
    }
}
