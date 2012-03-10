package robotics.helena;

import edu.wpi.first.wpilibj.PIDController;
import edu.wpi.first.wpilibj.PIDOutput;
import edu.wpi.first.wpilibj.PIDSource;
import edu.wpi.first.wpilibj.Victor;

public class Shooter {
    // The motors that make up the shooter
    private Victor lowerMotor;
    private Victor upperMotor;

    // RPM Sensor
    private ArduinoRPMSensor rpmSensor;

    // RPM maxspeed (helps the PID controller)
    public static final double MAX_RPM = 2000.0;

    // Give the upper motor a 5% slower rate than the lower motor to
    // give the ball some backspin.  In reality the speed difference is
    // greater than 5% since the upper motor has a larger pulley on the
    // shooter, but we'll give it a bit more delta here.
    private static final double UPPER_BIAS = 0.95;

    // An effecient way to control the speed of the motors
    private PIDController lowerPID;
    private PIDController upperPID;

    // PID constants - XXX - Need tuning
    private static final double LOWER_KP = 1.0;
    private static final double LOWER_KI = 0.0;
    private static final double LOWER_KD = 0.0;
    private static final double LOWER_TOLERANCE = 10.0;

    private static final double UPPER_KP = 1.0;
    private static final double UPPER_KI = 0.0;
    private static final double UPPER_KD = 0.0;
    private static final double UPPER_TOLERANCE = 10.0;

    Shooter(int lowerVicNum, int upperVicNum) {
        // Initialize the motors
        lowerMotor = new Victor(lowerVicNum);
        upperMotor = new Victor(upperVicNum);
        lowerMotor.set(0);
        upperMotor.set(0);

        // Initialize the RPM sensor.
        rpmSensor = new ArduinoRPMSensor(1);

        // Initialize the setup for the lower PID controller
        PIDSource lowerRPM = new PIDSource() {
                public double pidGet() {
                    return rpmSensor.getRPM(ArduinoRPMSensor.BOTTOM_MOTOR);
                }
            };
        PIDOutput lowerMotorCtl = new PIDOutput() {
                // RPM or speed control needs to take into account the
                // previous value, since we want to 'slow' the motor
                // down, not stop and reverse it.
                private double previousValue = 0.0;

                public void pidWrite(double output) {
                    double newVal = previousValue + output;
                    lowerMotor.set(newVal);
                    previousValue = newVal;
                }
            };
        lowerPID = new PIDController(LOWER_KP, LOWER_KI, LOWER_KD,
                                     lowerRPM, lowerMotorCtl);
        lowerPID.setTolerance(LOWER_TOLERANCE);

        // Set the minimum and maximum RPM range
        lowerPID.setInputRange(0, MAX_RPM);

        // The motor range is all positive
        lowerPID.setOutputRange(0, 1.0);

        // Initialize the setup for the lower PID controller
        PIDSource upperRPM = new PIDSource() {
                public double pidGet() {
                    return rpmSensor.getRPM(ArduinoRPMSensor.TOP_MOTOR);
                }
            };
        PIDOutput upperMotorCtl = new PIDOutput() {
                // RPM or speed control needs to take into account the
                // previous value, since we want to 'slow' the motor
                // down, not stop and reverse it.
                private double previousValue = 0.0;

                public void pidWrite(double output) {
                    double newVal = previousValue + output;
                    upperMotor.set(newVal);
                    previousValue = newVal;
                }
            };
        upperPID = new PIDController(UPPER_KP, UPPER_KI, UPPER_KD,
                                     upperRPM, upperMotorCtl);
        upperPID.setTolerance(UPPER_TOLERANCE);

        // Set the PID ranges
        lowerPID.setInputRange(0, MAX_RPM * UPPER_BIAS);
        upperPID.setOutputRange(0, 1.0);
    }

    public void disable() {
        // Shut everything down!
        lowerPID.disable();
        upperPID.disable();
        lowerMotor.set(0);
        upperMotor.set(0);
    }

    public void setRPM(double rpm) {
        // Give the upper motor a 5% slower rate than the upper motor.
        // In reality the speed is less than that since the upper motor
        // has a larger pulley on the shooter, but we'll give it a bit
        // more delta here.
        lowerPID.setSetpoint(rpm);
        upperPID.setSetpoint(rpm * UPPER_BIAS);
    }
}
