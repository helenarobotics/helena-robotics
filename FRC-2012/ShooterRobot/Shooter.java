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

    // Bridge Arm
    private Victor bridgeMotor;

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
    private static final long MAX_MOTOR_WAIT_TIME = 8 * 1000;

    // How long should the motors have a speed that's 'good' enough for
    // us to consider them up-to-speed?
    private static final long MIN_MOTOR_CORRECT_TIME = 750;

    // An effecient way to control the speed of the motors
    private PIDController lowerPID;
    private PIDController upperPID;

    // PID constants - XXX - Need tuning
    private static final double LOWER_KP = 0.00005;
    private static final double LOWER_KI = 0.0;
    private static final double LOWER_KD = LOWER_KP * 5.0;
    private static final double LOWER_TOLERANCE = 25.0;

    private static final double UPPER_KP = 0.00005;
    private static final double UPPER_KI = 0.0;
    private static final double UPPER_KD = UPPER_KP * 5.0;
    private static final double UPPER_TOLERANCE = 25.0;

    public Shooter() {
        // Initialize the motors
        lowerMotor = new Victor(Configuration.VICTOR_LOWER_SHOOTER);
        upperMotor = new Victor(Configuration.VICTOR_UPPER_SHOOTER);
        lowerMotor.set(0);
        upperMotor.set(0);

        // Bridge Motor
        bridgeMotor = new Victor(Configuration.VICTOR_BRIDGE_ARM);

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
//                    if ((count++ % 25) == 0)
//                        System.out.println("BtmRPM=" + btmRPM);
                    DashboardComm.rpmBottom = btmRPM;
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
                    DashboardComm.powerBottom = newVal;
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
//                    if ((count++ % 25) == 0)
//                        System.out.println("TopRPM=" + topRPM);
                    DashboardComm.rpmTop = topRPM;
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
                    DashboardComm.powerTop = newVal;
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
        // Keep track of the shooter joystick controls.
        DashboardComm.joy2X = joy.getX();
        DashboardComm.joy2Y = joy.getY();

        // Shoot the ball!
        if (joystickTrigger(joy))
            shootBall();

        // Toggle between 'PID' and raw throttle control for the shooter
        // motors (debugging).
        joystickRpm(joy);

        // Rotate the shooter on the lazy susan
        double joyX = joy.getX();
        if (Math.abs(joyX) > 0.1)
            rotationMotor.set(joyX);
        else
            rotationMotor.set(0);

        // Move the bridge arm
        double joyY = joy.getY();
        if (Math.abs(joyY) > 0.1) {
            if (joyY < 0)
                bridgeMotor.set(-0.25);
            else
                bridgeMotor.set(0.25);
        } else
            bridgeMotor.set(0);

        // Read the throttle to determine the speed of the shooter motor
        // and convert it to a number between 0 and 1.  Use that number
        // to set the RPM of the shooter.
        setLowerPower((joy.getThrottle() - 1.0) / 2.0);
    }

    public void shootBall() {
        feeder.shootBall();
    }

    // The lower motor is really what controls things, so we'll use it
    // and have the upper motor slave to it.
    public void setLowerRPM(double lowerRPM) {
        setPIDMotors(lowerRPM, lowerRPM * UPPER_BIAS);
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
            maxWaitTries--;
            try {
                Thread.sleep(myWaitPeriod);
            } catch (InterruptedException ignored) {
            }

            // Are we up to speed on both motors?            
            int motorRpms[] = rpmSensor.getRPMs();
            int lowerDiff =
                Math.abs((int)lowerPID.getSetpoint() - motorRpms[1]);
            int upperDiff =
                Math.abs((int)upperPID.getSetpoint() - motorRpms[0]);
            if (lowerDiff < LOWER_TOLERANCE && upperDiff < UPPER_TOLERANCE)
                numSuccess++;
            else
                numSuccess = 0;
        } while (maxWaitTries > 0 && numSuccess < wantSuccess);

        // We've either gotten there or timed out.  Either way, we're
        // going for it!
        return (numSuccess >= wantSuccess);
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

    // The range of the throttle control.
    private double rpmRange[] = { 0.0, MAX_LOWER_RPM };

    // Toggle between full-range and 'magnified' throttle control for
    // the shooter motor speeds.
    private boolean fullRange = true;
    private boolean wasBtn2Pressed = false;
    private static final int RPM_BTN = 2;
    private void joystickRpm(Joystick joy) {
        // Toggle when the button is pressed
        boolean nowPressed = joy.getRawButton(RPM_BTN);
        if (nowPressed && !wasBtn2Pressed) {
            // Calculate the new upper-lower range based on the current
            // throttle settings.
            fullRange = !fullRange;
            if (fullRange) {
                rpmRange[0] = 0.0;
                rpmRange[1] = MAX_LOWER_RPM;
            } else {
                // Make it so the throttle's current setting keeps the
                // RPM at the same setting.
                double newRange = MAX_LOWER_RPM / 5.0;
                double currPower = Math.abs((joy.getThrottle() - 1.0) / 2.0);
                double currRPM = currPower * MAX_LOWER_RPM;
                rpmRange[0] = currRPM - currPower * newRange;
                rpmRange[1] = rpmRange[0] + newRange;
            }
        }
        wasBtn2Pressed = nowPressed;
    }

    private void setLowerPower(double lowerPower) {
        DashboardComm.shooterThrottle = lowerPower;

        if (fullRange) {
            // If we're in full range mode and the power is less than 10%,
            // ignore it and just set the power to zero as we're not going
            // to shoot any baskets with such low power.
            if (Math.abs(lowerPower) < 0.1)
                lowerPower = 0.0;

            // Disable PID control and turn the shooter motors off
            if (lowerPower == 0.0) {
                // Disable PID (if enabled)
                if (lowerPID.isEnable()) {
                    lowerPID.reset();
                    lowerPID.setSetpoint(0);
                    DashboardComm.rpmBottom = 0;
                }
                if (upperPID.isEnable()) {
                    upperPID.reset();
                    upperPID.setSetpoint(0);
                    DashboardComm.rpmTop = 0;
                }
                // Turn-off motors
                lowerMotor.set(0);
                upperMotor.set(0);
                return;
            }
        }

        // Calculate the RPM based on the full range available on the
        // throttle and set both RPMs
        double lowerRPM = lowerPower * (rpmRange[1] - rpmRange[0]);
        setPIDMotors(lowerRPM, lowerRPM * UPPER_BIAS);
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
