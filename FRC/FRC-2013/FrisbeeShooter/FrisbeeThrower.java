package robotics.helena;

import java.util.Timer;
import java.util.TimerTask;

import edu.wpi.first.wpilibj.Jaguar;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.Victor;

public class FrisbeeThrower {
    // The motor
    private Victor throwerMotor;

    // RPM Sensor
    private ArduinoRPMSensor rpmSensor;

    // Motor RPM maxspeed (helps the PID controller)
    public static final double MAX_RPM = 2500.0;

    // How long will we wait for the motors to come up to speed.
    private static final long MAX_MOTOR_WAIT_TIME = 8 * 1000;

    // How long should the motors have a speed that's 'good' enough for
    // us to consider them up-to-speed?
    private static final long MIN_MOTOR_CORRECT_TIME = 1000;

    // An effecient way to control the speed of the motors
    private MotorPIDController motorPID;

    // PID constants - XXX - Need tuning
    private static final double PID_KP = 0.00005;
    private static final double PID_KI = 0.0;
    private static final double PID_KD = PID_KP * 5.0;
    private static final double PID_TOLERANCE = 25.0;

    public FrisbeeThrower() {
        // Initialize the motors
        throwerMotor = new Victor(Configuration.VICTOR_FRISBEE_SHOOTER);
        throwerMotor.set(0);

        // Initialize the RPM sensor.
        rpmSensor = new ArduinoRPMSensor(Configuration.I2C_ARDUINO);

        // Initialize PID controller for the lower and upper
        // motors.
        motorPID = new MotorPIDController(
            "Thrower", rpmSensor, ArduinoRPMSensor.FIRST_MOTOR,
            throwerMotor, PID_KP, PID_KI, PID_KD,
            PID_TOLERANCE, MAX_RPM);
    }

    public void disable() {
        // Shut everything down!
        motorPID.disable();
        throwerMotor.set(0);
    }

    // Allow for toggling between full-range and 'magnified/precise'
    // throttle control for the motor speed.
    private boolean fullRange = true;
    private boolean wasBtn2Pressed = false;
    private static final int RPM_BTN = 2;

    // Set the thrower motor speed using the joystick!
    public void joystickControl(Joystick joy) {
        // Toggle when the button is pressed
        boolean nowPressed = joy.getRawButton(RPM_BTN);
        if (nowPressed && !wasBtn2Pressed) {
            // Calculate the new upper-lower range based on the current
            // throttle settings.
            fullRange = !fullRange;
        }
        wasBtn2Pressed = nowPressed;

        // Set the range for the throttle end-points.
        double btmRpmRange, topRpmRange;
        if (fullRange) {
            btmRpmRange = 0.0;
            topRpmRange = MAX_RPM;
        } else {
            // Make it so the throttle's current setting keeps the
            // RPM at the same setting.
            double newRange = MAX_RPM / 5.0;
            double currPower = Math.abs((joy.getThrottle() - 1.0) / 2.0);
            double currRPM = currPower * MAX_RPM;
            btmRpmRange = currRPM - currPower * newRange;
            topRpmRange = btmRpmRange + newRange;
        }

        // Read the joystick throttle to try and determine the speed of
        // the motor, but convert it to a number between 0 and 1.  Use
        // that number to set the RPM of the along with the range of the
        // throttle.
        setMotorPower(
            (joy.getThrottle() - 1.0) / 2.0, btmRpmRange, topRpmRange);
    }

    // The lower motor is really what controls things, so we'll use it
    // and have the upper motor slave to it.
    public void setThrowerRPM(double throwerRPM) {
        setPIDMotorRPM(throwerRPM);
    }

    // Wait for the motor to come up to speed.
    public boolean waitForMotor() {
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

            // Are we up to speed?
            if (motorPID.onTarget())
                numSuccess++;
            else
                numSuccess = 0;
        } while (maxWaitTries > 0 && numSuccess < wantSuccess);

        // We've either gotten there or timed out.  Either way, we're
        // going for it!
        return (numSuccess >= wantSuccess);
    }

    // Allows the motor to get up to speed before enabling the PID
    // controller.
    private volatile boolean startupActive = false;
    private class StartupTask extends TimerTask {
        private Timer t;
        private double throwerRPM;
        public StartupTask(Timer _t,double _throwerRPM) {
            startupActive = true;

            // Keep track of the timer that initiated us.
            t = _t;

            // We feed-forward the target power to the PID controller so
            // when we enable the controller, the speed control should
            // already be 'close' to the target value, thus decreasing
            // the time necessary to get a stable  speed.
            throwerRPM = _throwerRPM;
            double motorPwr = throwerRPM / MAX_RPM;
            motorPID.disable();
            motorPID.setTargetPower(-motorPwr);
            throwerMotor.set(-motorPwr);
        }

        public void run() {
            // Timer is running, so let the parent timer thread go away.
            t.cancel();

            motorPID.setTargetRpm(throwerRPM);

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

    private void setMotorPower(double motorPower,
                               double btmRange, double topRange) {
        // If we're in full range mode and the power is less than 10%,
        // ignore it and just set the power to zero as we're not going
        // to shoot any frisbees with such low power.
        if (fullRange && Math.abs(motorPower) < 0.1)
            motorPower = 0.0;

        // Calculate the RPM based on the full range available on the
        // throttle and set both RPMs
        double throwerRPM =
            Math.abs(motorPower * (topRange - btmRange)) + btmRange;
        setPIDMotorRPM(throwerRPM);
    }

    private void setPIDMotorRPM(double throwerRPM) {
        // If we're in the process of starting up the motors, let them
        // startup before we do anything else.
        if (startupActive)
            return;

        // If the motors aren't running, then start them up and let
        // them run for a bit before we use the PID controller on
        // them to avoid PID windup.
        if (motorPID.getRpm() == 0) {
            Timer t = new Timer();
            t.schedule(new StartupTask(t, throwerRPM), 2000);
        } else {
            motorPID.setTargetRpm(throwerRPM);
        }
    }
}
