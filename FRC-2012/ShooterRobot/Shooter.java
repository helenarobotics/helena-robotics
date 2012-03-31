package robotics.helena;

import java.util.Timer;
import java.util.TimerTask;

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

    // Lower Motor RPM maxspeed (helps the PID controller)
    public static final double MAX_LOWER_RPM = 2500.0;

    // Give the upper motor a 5% slower rate than the lower motor to
    // give the ball some backspin.  In reality the speed difference is
    // greater than 5% since the upper motor has a larger pulley on the
    // shooter, but we'll give it a bit more delta here.
    private static final double UPPER_BIAS = 0.95;

    // How long will we wait for the motors to come up to speed.
    private static final long MAX_MOTOR_WAIT_TIME = 4 * 1000;

    // How long should the motors have a speed that's 'good' enough for
    // us to consider them up-to-speed?
    private static final long MIN_MOTOR_CORRECT_TIME = 500;

    // An effecient way to control the speed of the motors
    private PIDController lowerPID;
    private PIDController upperPID;

    // PID constants - XXX - Need tuning
    private static final double LOWER_KP = 0.00005;
    private static final double LOWER_KI = 0.0;
    private static final double LOWER_KD = LOWER_KP * 5.0;
    private static final double LOWER_TOLERANCE = 50.0;

    private static final double UPPER_KP = 0.00005;
    private static final double UPPER_KI = 0.0;
    private static final double UPPER_KD = UPPER_KP * 5.0;
    private static final double UPPER_TOLERANCE = 50.0;

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
                int count = 0;
                public double pidGet() {
                    double btmRPM =
                        rpmSensor.getRPM(ArduinoRPMSensor.BOTTOM_MOTOR);
                    if ((count++ % 25) == 0)
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
                    // Output is subtracted since we use negative powers
                    // to give us the proper direction as the RPM
                    // counter doesn't care the direction, just the
                    // value of the RPM.
                    double newVal = previousValue - output;

                    // Limit power from 0 -> -1.0 to keep the motor from
                    // reversing itself if the controls oscillate too
                    // far one way or the other.
                    if (newVal < -1.0)
                        newVal = -1.0;
                    else if (newVal > 0.0)
                        newVal = 0.0;
//                    System.out.println(
//                        "BtmPow=" + newVal + ", Change=" + output);
                    lowerMotor.set(newVal);
                    previousValue = newVal;
                    DataLogger.powerBottom = newVal;
                }
            };
        lowerPID = new PIDController(LOWER_KP, LOWER_KI, LOWER_KD,
                                     lowerRPM, lowerMotorCtl);
        lowerPID.setTolerance(LOWER_TOLERANCE);

        // Set the minimum and maximum RPM range
        lowerPID.setInputRange(0, MAX_LOWER_RPM);

        // The motor range is truly all position (actually negative and
        // limited in the PIDOutput cass), but we need it to be able to
        // go negative in order to reduce power.
        lowerPID.setOutputRange(-1.0, 1.0);

        // Initialize the setup for the lower PID controller
        PIDSource upperRPM = new PIDSource() {
                int count = 0;
                public double pidGet() {
                    double topRPM =
                        rpmSensor.getRPM(ArduinoRPMSensor.TOP_MOTOR);
                    if ((count++ % 25) == 0)
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
                    // Output is subtracted since we use negative powers
                    // to give us the proper direction as the RPM
                    // counter doesn't care the direction, just the
                    // value of the RPM.
                    double newVal = previousValue - output;

                    // Limit power from 0 -> -1.0 to keep the motor from
                    // reversing itself if the controls oscillate too
                    // far one way or the other.
                    if (newVal < -1.0)
                        newVal = -1.0;
                    else if (newVal > 0.0)
                        newVal = 0.0;
//                    System.out.println(
//                        "TopPow=" + newVal + ", Change=" + output);
                    upperMotor.set(newVal);
                    previousValue = newVal;
                    DataLogger.powerTop = newVal;
                }
            };
        upperPID = new PIDController(UPPER_KP, UPPER_KI, UPPER_KD,
                                     upperRPM, upperMotorCtl);
        upperPID.setTolerance(UPPER_TOLERANCE);

        // Set the PID ranges
        upperPID.setInputRange(0, MAX_LOWER_RPM * UPPER_BIAS);
        upperPID.setOutputRange(-1.0, 1.0);
    }

    public void disable() {
        // Shut everything down!
        lowerPID.reset();
        lowerMotor.set(0);
        upperPID.reset();
        upperMotor.set(0);
        rotationMotor.set(0);
    }

    public void joystickControl(Joystick joy) {
        // Shoot the ball!
        if (joystickTrigger(joy))
            shootBall();

        // Toggle between 'PID' and raw throttle control for the shooter
        // motors (debugging).
        joystickRpm(joy);

        // Rotate the shooter on the lazy susan
        rotationMotor.set(joy.getX());

        // Read the throttle to determine the speed of the shooter motor
        // and convert it to a number between 0 and 1.  Use that number
        // to set the RPM of the shooter.
        setLowerPower((joy.getThrottle() - 1.0) / 2.0);
    }

    public void shootBall() {
        feeder.shootBall();
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
    private boolean rawThrottle = false;
    private double targetLowerPower = 0.0;

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
                lowerPID.reset();
                upperPID.reset();
            }
        }
        wasRpmPressed = nowPressed;
    }

    // The lower motor is really what controls things, so we'll use it
    // and have the upper motor slave to it.
    public void setLowerRPM(double lowerRPM) {
        if (rawThrottle) {
            // Convert RPM to power
            targetLowerPower = lowerRPM / MAX_LOWER_RPM;
            lowerMotor.set(targetLowerPower);
            upperMotor.set(targetLowerPower * UPPER_BIAS);
        } else {
            setPIDMotors(lowerRPM, lowerRPM * UPPER_BIAS);
        }
    }

    // Wait for the motors to come up to speed.
    public boolean waitForMotors() {
        // How long will we wait for the motors to come up to speed.
        int maxWaitTries = 50;
        final long myWaitPeriod = MAX_MOTOR_WAIT_TIME / maxWaitTries;

        // How many successful reading do we have to have for
        final long wantSuccess = MIN_MOTOR_CORRECT_TIME / myWaitPeriod;
        long numSuccess = 0;
        do {
            // Wait a bit
            try {
                Thread.sleep(myWaitPeriod);
            } catch (InterruptedException ignored) {
            }
            maxWaitTries--;

            // Are we up to speed on both motors?
            if (!rawThrottle) {
                if (lowerPID.onTarget() && upperPID.onTarget())
                    numSuccess++;
                else
                    numSuccess = 0;
            } else {
                // Read the motor speeds.
                int motorRpms[] = rpmSensor.getRPMs();

                // We need to keep track of the target power
                double targetLowerRPM = targetLowerPower * MAX_LOWER_RPM;
                double targetUpperRPM = targetLowerRPM * UPPER_BIAS;
                if (Math.abs(
                        targetLowerRPM - motorRpms[1]) < LOWER_TOLERANCE &&
                    Math.abs(
                        targetUpperRPM - motorRpms[0]) < UPPER_TOLERANCE)
                    numSuccess++;
                else
                    numSuccess = 0;
            }
        } while (maxWaitTries > 0 || numSuccess >= wantSuccess);

        // We've either gotten there or timed out.  Either way, we're
        // going for it!
        return (numSuccess >= wantSuccess);
    }

    private void setLowerPower(double lowerPower) {
        // If the power is less than 10%, ignore it and just set the
        // power to zero as we're not going to shoot any baskets with
        // the low power.
        if (Math.abs(lowerPower) < 0.1)
            lowerPower = 0.0;

        // Disable PID control and turn the shooter motors off
        if (lowerPower == 0.0) {
            // Disable PID (if enabled)
            if (lowerPID.isEnable()) {
                lowerPID.reset();
                lowerPID.setSetpoint(0);
            }
            if (upperPID.isEnable()) {
                upperPID.reset();
                upperPID.setSetpoint(0);
            }
            // Turn-off motors
            lowerMotor.set(0);
            upperMotor.set(0);
            return;
        }

        // Run the motors
        if (rawThrottle) {
            targetLowerPower = lowerPower;
            lowerMotor.set(targetLowerPower);
            upperMotor.set(targetLowerPower * UPPER_BIAS);
        } else {
            double lowerRPM = lowerPower * MAX_LOWER_RPM;
            setPIDMotors(lowerRPM, lowerRPM * UPPER_BIAS);
        }
    }

    private void setPIDMotors(double lowerRPM, double upperRPM) {
        if (!lowerPID.isEnable())
            lowerPID.enable();
        if (!upperPID.isEnable())
            upperPID.enable();
        lowerPID.setSetpoint(lowerRPM);
        upperPID.setSetpoint(upperRPM);
    }
}
