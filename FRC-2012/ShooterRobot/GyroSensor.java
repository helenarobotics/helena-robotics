package robotics.helena;

import edu.wpi.first.wpilibj.Gyro;

// This is a simple wrapper around the Gyro sensor so we can feed it's
// data to the DriverStation on a regular basis.
class GyroSensor implements Runnable {
    private final Gyro gyro;
    private final long sleepTime;

    GyroSensor(int analogChannel, long sleepTime) {
        this.sleepTime = sleepTime;
        gyro = new Gyro(analogChannel);
        Thread t = new Thread("GyroSensor");
        t.start();
    }

    public void run() {
        while (true) {
            DashboardComm.gyroAngle = gyro.getAngle();

            // Wait some time and do it again!
            try {
                Thread.sleep(sleepTime);
            } catch (InterruptedException ignored) {
            }
        }
    }
}
