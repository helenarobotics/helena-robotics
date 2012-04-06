package robotics.helena;

import java.util.Timer;
import java.util.TimerTask;

import edu.wpi.first.wpilibj.Jaguar;
import edu.wpi.first.wpilibj.Joystick;
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
    private static final long MAX_MOTOR_WAIT_TIME = 8 * 1000;

    // How long should the motors have a speed that's 'good' enough for
    // us to consider them up-to-speed?
    private static final long MIN_MOTOR_CORRECT_TIME = 1000;

    // An effecient way to control the speed of the motors
    private ShooterPIDController lowerPID;
    private ShooterPIDController upperPID;

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

        // Initialize the RPM sensor.
        rpmSensor = new ArduinoRPMSensor(Configuration.I2C_ARDUINO);

        // Setup the rotationMotor
        rotationMotor = new Jaguar(Configuration.JAGUAR_SHOOTER_ROTATION);
        rotationMotor.set(0);

        // Finally, the ball feeder
        feeder = new BallFeeder();

        // Initialize PID controller for the lower and upper shooter
        // motors.
        lowerPID = new ShooterPIDController(
            "Btm", rpmSensor, ArduinoRPMSensor.BOTTOM_MOTOR,
            lowerMotor, LOWER_KP, LOWER_KI, LOWER_KD,
            LOWER_TOLERANCE, MAX_LOWER_RPM);

        upperPID = new ShooterPIDController(
            "Top", rpmSensor, ArduinoRPMSensor.TOP_MOTOR,
            upperMotor, UPPER_KP, UPPER_KI, UPPER_KD,
            UPPER_TOLERANCE, MAX_LOWER_RPM * UPPER_BIAS);
    }

    public void disable() {
        // Shut everything down!
        lowerPID.disable();
        lowerMotor.set(0);
        upperPID.disable();
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

        // Rotate the shooter on the lazy susan
        double joyX = joy.getX();
        if (Math.abs(joyX) > 0.1)
            rotationMotor.set(joyX);
        else
            rotationMotor.set(0);

        // Set the shooter motor speed.
        joystickThrottle(joy);
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
            if (lowerPID.onTarget() && upperPID.onTarget())
                numSuccess++;
            else
                numSuccess = 0;
        } while (maxWaitTries > 0 && numSuccess < wantSuccess);

        // We've either gotten there or timed out.  Either way, we're
        // going for it!
        return (numSuccess >= wantSuccess);
    }

    // Allows the motors to get up to speed before enabling the PID
    // controller.
    private volatile boolean startupActive = false;
    private class StartupTask extends TimerTask {
        private double lowerRPM, upperRPM;
        public StartupTask(double _lowerRPM, double _upperRPM) {
            startupActive = true;

            // We feed-forward the target power to the PID controller so
            // when we enable the controller, the speed control should
            // already be 'close' to the target value, thus decreasing
            // the time necessary to get a stable shooter speed.
            lowerRPM = _lowerRPM;
            double lowerPwr = lowerRPM / MAX_LOWER_RPM;
            lowerPID.disable();
            lowerPID.setTargetPower(-lowerPwr);
            lowerMotor.set(-lowerPwr);

            upperRPM = _upperRPM;
            double upperPwr = upperRPM / MAX_LOWER_RPM;
            upperPID.disable();
            upperPID.setTargetPower(-upperPwr);
            upperMotor.set(-upperPwr);
        }

        public void run() {
            lowerPID.setTargetRpm(lowerRPM);
            upperPID.setTargetRpm(upperRPM);

            // Give the PID controller a bit more time to read the RPM's
            // before considering the startup complete.  Otherwise, we
            // might try to restart the motors if the RPM value isn't
            // read.
            try {
                Thread.sleep(500);
            } catch (InterruptedException ignored) {
            }
            startupActive = false;
        }
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

    // Toggle between full-range and 'magnified' throttle control for
    // the shooter motor speeds.
    private boolean fullRange = true;
    private boolean wasBtn2Pressed = false;
    private static final int RPM_BTN = 2;
    private void joystickThrottle(Joystick joy) {
        // Range of the throttle end-points
        double btmRpmRange, topRpmRange;

        // Toggle when the button is pressed
        boolean nowPressed = joy.getRawButton(RPM_BTN);
        if (nowPressed && !wasBtn2Pressed) {
            // Calculate the new upper-lower range based on the current
            // throttle settings.
            fullRange = !fullRange;
        }
        wasBtn2Pressed = nowPressed;

        if (fullRange) {
            btmRpmRange = 0.0;
            topRpmRange = MAX_LOWER_RPM;
        } else {
            // Make it so the throttle's current setting keeps the
            // RPM at the same setting.
            double newRange = MAX_LOWER_RPM / 5.0;
            double currPower = Math.abs((joy.getThrottle() - 1.0) / 2.0);
            double currRPM = currPower * MAX_LOWER_RPM;
            btmRpmRange = currRPM - currPower * newRange;
            topRpmRange = btmRpmRange + newRange;
        }

        // Read the throttle to determine the speed of the shooter motor
        // and convert it to a number between 0 and 1.  Use that number
        // to set the RPM of the shooter along with the range of the
        // throttle.
        setLowerPower(
            (joy.getThrottle() - 1.0) / 2.0, btmRpmRange, topRpmRange);

        // Update the Dashboard with the latest PID values.
        DashboardComm.rpmBottom = lowerPID.getRpm();
        DashboardComm.powerBottom = lowerPID.getPower();
        DashboardComm.rpmTop = upperPID.getRpm();
        DashboardComm.powerTop = upperPID.getPower();
    }

    private void setLowerPower(double lowerPower,
                               double btmRange, double topRange) {
        DashboardComm.shooterThrottle = lowerPower;

        // If we're in full range mode and the power is less than 10%,
        // ignore it and just set the power to zero as we're not going
        // to shoot any baskets with such low power.
        if (fullRange && Math.abs(lowerPower) < 0.1)
            lowerPower = 0.0;

        // Calculate the RPM based on the full range available on the
        // throttle and set both RPMs
        double lowerRPM =
            Math.abs(lowerPower * (topRange - btmRange)) + btmRange;
        setPIDMotors(lowerRPM, lowerRPM * UPPER_BIAS);
    }

    private void setPIDMotors(double lowerRPM, double upperRPM) {
        // If we're in the process of starting up the motors, let them
        // startup before we do anything else.
        if (startupActive)
            return;

        // Update the dashboard
        DashboardComm.rpmBottomTarget = lowerRPM;
        DashboardComm.rpmTopTarget = upperRPM;
        
        // If the motors aren't running, then start them up and let
        // them run for a bit before we use the PID controller on
        // them to avoid PID windup.
        if (lowerPID.getRpm() == 0) {
            new Timer().schedule(new StartupTask(lowerRPM, upperRPM), 2000);
        } else {
            lowerPID.setTargetRpm(lowerRPM);
            upperPID.setTargetRpm(upperRPM);
        }
    }
}
