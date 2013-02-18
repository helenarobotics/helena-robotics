package robotics.helena;

import edu.wpi.first.wpilibj.PIDController;
import edu.wpi.first.wpilibj.PIDOutput;
import edu.wpi.first.wpilibj.PIDSource;
import edu.wpi.first.wpilibj.Victor;

public class ShooterPIDController {
    private class ShooterPIDSource implements PIDSource {
        final ArduinoRPMSensor rpmSensor;
        final byte rpmSensorNum;
        final String logHdr;
        double rpm;

        ShooterPIDSource(final String logPrefix,
                         final ArduinoRPMSensor _rpmSensor,
                         final byte _rpmSensorNum) {
            logHdr = logPrefix + "RPM";
            rpmSensor = _rpmSensor;
            rpmSensorNum = _rpmSensorNum;
        }

        // Limit log output
        private int runCount = 0;
        public double pidGet() {
            rpm = rpmSensor.getRPM(rpmSensorNum);
//            if ((runCount++ % 25) == 0)
//                System.out.println(logHdr + "=" + rpm);
            return rpm;
        }
    }

    private class ShooterPIDOutput implements PIDOutput {
        final String logHdr;
        final Victor motor;
        double currValue = 0.0;

        ShooterPIDOutput(final String logPrefix, final Victor _motor) {
            logHdr = logPrefix + "Pwr";
            motor = _motor;
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
            if (newVal < -1.0)
                newVal = -1.0;
            else if (newVal > 0.0)
                newVal = 0.0;
//            System.out.println(logHdr + "=" + newVal + ", Change=" + output);
            motor.set(newVal);
            currValue = newVal;
        }
    }

    private final Victor motor;
    private final ShooterPIDSource rpmSrc;
    private final ShooterPIDOutput motorCtl;
    private final PIDController pidCtl;
    private final double rpmTolerance;

    public ShooterPIDController(String logPrefix,
                                ArduinoRPMSensor sensor, byte rpmSensorNum,
                                Victor _motor,
                                double kP, double kI, double kD,
                                double _rpmTolerance, double maxRpm) {
        rpmTolerance = _rpmTolerance;
        motor = _motor;
        rpmSrc = new ShooterPIDSource(logPrefix, sensor, rpmSensorNum);
        motorCtl = new ShooterPIDOutput(logPrefix, motor);
        pidCtl = new PIDController(kP, kI, kD, rpmSrc, motorCtl);
        // RPM tolerance
        pidCtl.setTolerance(rpmTolerance);

        // Set the minimum and maximum RPM range
        pidCtl.setInputRange(0, maxRpm);

        // The motor range is actually all positive (actually negative
        // and limited in the PIDOutput cass), but we need it to be able
        // to go negative in order to reduce power.
        pidCtl.setOutputRange(-1.0, 1.0);
    }

    public void disable() {
        pidCtl.reset();
        motor.set(0);
    }

    public double getPower() {
        return motorCtl.currValue;
    }

    public void setTargetPower(double power) {
        motorCtl.feedForward(power);
    }

    public double getRpm() {
        return rpmSrc.rpm;
    }

    public void setTargetRpm(double rpm) {
        if (rpm == 0) {
            if (pidCtl.isEnable()) {
                pidCtl.reset();
                pidCtl.setSetpoint(0);
                setTargetPower(0);
                rpmSrc.rpm = 0;
            }
            motor.set(0);
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
