package robotics.helena;

import edu.wpi.first.wpilibj.Servo;

// This is the BallFeed mechanism, which shoots the ball by dropping
// the servo for a set period of time which allows the ball to be
// fed into the shooter.
class BallFeeder implements Runnable {
    private static final float SERVO_HOLD_POS = 60;
    private static final float SERVO_LAUNCH_POS = 360;

    private Servo launcher;
    private boolean shooting;

    BallFeeder() {
        launcher = new Servo(Configuration.SERVO_SHOOTER_FEEDER);
        shooting = false;

        Thread t = new Thread("BallFeed");
        t.start();
    }

    public void run() {
        while (true) {
            // We always start holding the ball.
            launcher.setAngle(SERVO_HOLD_POS);
            DataLogger.shooting = false;

            // Wait for the shoot command.
            synchronized (this) {
                while (!shooting) {
                    try {
                        this.wait();
                    } catch (InterruptedException ignored) {
                    }
                }
            }
            // Drop the synchronization lock and shoot a ball.
            // Drop the gate for a bit, and then loop to the top
            // which bring it back up.
            launcher.setAngle(SERVO_LAUNCH_POS);
            DataLogger.shooting = true;
            try {
                Thread.sleep(250);
            } catch (InterruptedException ignored) {
            }
        }
    }

    void shootBall() {
        synchronized (this) {
            // Ignore requests if we're already shooting
            if (!shooting) {
                shooting = true;
                this.notify();
            }
        }
    }
}
