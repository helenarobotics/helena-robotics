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

    // RPM maxspeed (helps the PID controller)
    private static final double MAX_RPM = 2000.0;

    // Give the upper motor a 5% slower rate than the lower motor to
    // give the ball some backspin.  In reality the speed difference is
    // greater than 5% since the upper motor has a larger pulley on the
    // shooter, but we'll give it a bit more delta here.
    private static final double UPPER_BIAS = 0.9;

    // An effecient way to control the speed of the motors
    private PIDController lowerPID;
    private PIDController upperPID;

    // PID constants - XXX - Need tuning
    private static final double LOWER_KP = 0.00002;
    private static final double LOWER_KI = 0.0;
    private static final double LOWER_KD = LOWER_KP * 5.0;
    private static final double LOWER_TOLERANCE = 50.0;

    private static final double UPPER_KP = 0.00002;
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

        // The motor range is truly all position (actually negative and
        // limited in the PIDOutput cass), but we need it to be able to
        // go negative in order to reduce power.
        lowerPID.setOutputRange(-1.0, 1.0);

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
        upperPID.setOutputRange(-1.0, 1.0);
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

        // Rotate the shooter on the lazy susan
        rotationMotor.set(joy.getX());

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
                // XXX - Tune only one motor at a time
                lowerMotor.set(0);
//                lowerPID.enable();
                upperPID.enable();
            } else {
                // Disable PID
                lowerPID.disable();
                upperPID.disable();
            }
        }
        wasRpmPressed = nowPressed;
    }

    // Allows the motors to get up to speed before enabling the PID
    // controller.
    private class StartUpTask extends TimerTask {
        private double lowerRPM, upperRPM;

        public StartUpTask(double _lowerRPM, double _upperRPM) {
            lowerRPM = _lowerRPM;
            upperRPM = _upperRPM;
//            lowerPID.disable();
//            lowerMotor.set(lowerRPM / MAX_RPM);
            upperPID.disable();
            upperMotor.set(upperRPM / MAX_RPM);
        }

        public void run() {
//            lowerPID.setSetpoint(lowerRPM);
//            lowerPID.enable();
            upperPID.setSetpoint(upperRPM);
            upperPID.enable();
        }
    }

    private void setRPM(double power) {
        // If the power is less than 10%, ignore it and just set the
        // power to zero as we're not going to shoot any baskets with
        // such lower power!
        if (Math.abs(power) < 0.1) {
            power = 0;
        }

        // Turn the shooter off and disable controls!
        if (power == 0.0) {
            // Disable PID
            lowerPID.disable();
            lowerPID.setSetpoint(0);
            lowerMotor.set(0);
            upperPID.disable();
            upperPID.setSetpoint(0);
            upperMotor.set(0);
            return;
        }

        // Give the upper motor a 5% slower rate than the upper motor.
        // In reality the speed is less than that since the upper motor
        // has a larger pulley on the shooter, but we'll give it a bit
        // more delta here.
        if (rawThrottle) {
            lowerMotor.set(power);
            upperMotor.set(power * UPPER_BIAS);
        } else {
            // Use PID control
            double rpm = Math.abs(power * MAX_RPM);

            // If the motors aren't running, then start them up and let
            // them run for a bit before we use the PID controller on
            // them to avoid PID windup.
            if (power > 0 && !upperPID.isEnable()) {
                new Timer().schedule(
                    new StartUpTask(rpm, rpm * UPPER_BIAS), 2000);
            } else {
//                lowerPID.setSetpoint(rpm);
                upperPID.setSetpoint(rpm * UPPER_BIAS);
            }
            // Only print out a new value if the setpoint changes significantly
            if (Math.abs((rpm * UPPER_BIAS) - upperPID.getSetpoint()) > 5)
                System.out.println("SP=" + (rpm * UPPER_BIAS) +
                                   ", PP=" + upperPID.getSetpoint());
        }
    }
}
