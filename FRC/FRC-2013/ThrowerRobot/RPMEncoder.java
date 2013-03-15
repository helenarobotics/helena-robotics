package robotics.helena;

import edu.wpi.first.wpilibj.Counter;
import edu.wpi.first.wpilibj.DigitalInput;

public class RPMEncoder implements Runnable {
    private final long INTERVAL_TIME = 10;

    private static final int MS_PER_MIN = 60000;
    private static final int TICKS_PER_REV = 1440;

    private static final float RPM_PER_TICKSMS = MS_PER_MIN / TICKS_PER_REV;

    private int curRpm;

    public RPMEncoder() {
        new Thread(this,"RPM Encoder").start();
    }

    public void run() {
        Counter counter = new Counter(
            new DigitalInput(Configuration.SHOOTER_ENCODER));
        counter.start();
        counter.reset();

        long lastReadTime = System.currentTimeMillis();
        while (true) {
            long now = System.currentTimeMillis();
            long dt = now - lastReadTime;
            if (dt > 0) {
                long ticks = counter.get();
                curRpm = (int)(RPM_PER_TICKSMS * ((float)(ticks) / (float)(dt)));
            }
            lastReadTime = now;
            counter.reset();

            // Wait a bit for the encoder to move
            try {
                Thread.sleep(INTERVAL_TIME);
            } catch (InterruptedException ignored) {}
        }
    }

    public int getRPM() {
        return curRpm;
    }
}
