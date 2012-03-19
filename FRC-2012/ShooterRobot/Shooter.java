package robotics.helena;

import edu.wpi.first.wpilibj.Jaguar;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.PIDController;
import edu.wpi.first.wpilibj.PIDOutput;
import edu.wpi.first.wpilibj.PIDSource;
import edu.wpi.first.wpilibj.Victor;

public class Shooter {
    // The shooter motors
    private Victor lowerMotor;
    private Victor upperMotor;

    // RPM Sensor
    private ArduinoRPMSensor rpmSensor;

    // Rotation motor
    private Jaguar rotationMotor;

    // Ball Feeder
    private BallFeeder feeder;

    // RPM maxspeed (helps the PID controller)
    private static final double MAX_RPM = 2000.0;

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

    public Shooter() {
        // Initialize the motors
        lowerMotor = new Victor(Configuration.VICTOR_LOWER_SHOOTER);
        upperMotor = new Victor(Configuration.VICTOR_UPPER_SHOOTER);
        lowerMotor.set(0);
        upperMotor.set(0);

        // Initialize the RPM sensor.
        rpmSensor = new ArduinoRPMSensor(Configuration.I2C_ARDUINO);

        // Setup the rotationMotor
        rotationMotor = new Jaguar(Configuration.JAGUAR_SHOOTER_ROTATION);
        rotationMotor.set(0);

        // Finally, the ball feeder
        feeder = new BallFeeder();

        // Initialize the setup for the lower PID controller
        PIDSource lowerRPM = new PIDSource() {
                public double pidGet() {
                    double btmRPM =
                        rpmSensor.getRPM(ArduinoRPMSensor.BOTTOM_MOTOR);
                    System.out.println("BtmRPM=" + btmRPM);
                    DataLogger.rpmBottom = btmRPM;
                    return btmRPM;
                }
            };
        PIDOutput lowerMotorCtl = new PIDOutput() {
                // RPM or speed control needs to take into account the
                // previous value, since we want to 'slow' the motor
                // down, not stop and reverse it.
                private double previousValue = 0.0;

                public void pidWrite(double output) {
                    double newVal = previousValue + output;
                    System.out.println(
                        "BtmPow=" + newVal + ", Change=" + output);
                    lowerMotor.set(newVal);
                    previousValue = newVal;
                    DataLogger.powerBottom = newVal;
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
                    double topRPM =
                        rpmSensor.getRPM(ArduinoRPMSensor.TOP_MOTOR);
                    System.out.println("TopRPM=" + topRPM);
                    DataLogger.rpmTop = topRPM;
                    return topRPM;
                }
            };
        PIDOutput upperMotorCtl = new PIDOutput() {
                // RPM or speed control needs to take into account the
                // previous value, since we want to 'slow' the motor
                // down, not stop and reverse it.
                private double previousValue = 0.0;

                public void pidWrite(double output) {
                    double newVal = previousValue + output;
                    System.out.println(
                        "TopPow=" + newVal + ", Change=" + output);
                    upperMotor.set(newVal);
                    previousValue = newVal;
                    DataLogger.powerTop = newVal;
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
        rotationMotor.set(0);
    }

    public void joystickControl(Joystick joy) {
        // Shoot the ball!
        if (joystickTrigger(joy))
            feeder.shootBall();

        // Toggle between 'PID' and raw throttle control for the shooter
        // motors (debugging).
        joystickRpm(joy);

        // Read the throttle to determine the speed of the shooter motor
        // and convert it to a number between 0 and 1.  Use that number
        // to set the RPM of the shooter.
        setRPM((joy.getThrottle() - 1.0) / 2.0);
    }

    private static final int TRIGGER_BTN = 1;
    private boolean wasTrigPressed = false;
    private boolean joystickTrigger(Joystick joy) {
        boolean btnPressed = false;
        // Toggle when the button is pressed
        boolean nowPressed = joy.getRawButton(TRIGGER_BTN);
        if (nowPressed && !wasTrigPressed) {
            btnPressed = true;
        }
        wasTrigPressed = nowPressed;

        return btnPressed;
    }

    // Toggle between PID and straight throttle control for the shooter
    // motor speeds
    private boolean rawThrottle = true;
    private boolean wasRpmPressed = false;
    private static final int RPM_BTN = 2;
    private void joystickRpm(Joystick joy) {
        boolean btnPressed = false;
        // Toggle when the button is pressed
        boolean nowPressed = joy.getRawButton(RPM_BTN);
        if (nowPressed && !wasRpmPressed) {
            rawThrottle = !rawThrottle;
            if (!rawThrottle) {
                // Re-enable the PID controls
                lowerPID.enable();
                upperPID.enable();
            } else {
                // Disable PID
                lowerPID.disable();
                upperPID.disable();
            }
        }
        wasRpmPressed = nowPressed;
    }

    private void setRPM(double power) {
        // Give the upper motor a 5% slower rate than the upper motor.
        // In reality the speed is less than that since the upper motor
        // has a larger pulley on the shooter, but we'll give it a bit
        // more delta here.
        if (rawThrottle) {
            lowerMotor.set(power);
            upperMotor.set(power * UPPER_BIAS);
        } else {
            // Use PID control
            double rpm = power * MAX_RPM;
            lowerPID.setSetpoint(rpm);
            upperPID.setSetpoint(rpm * UPPER_BIAS);
        }
    }
}
