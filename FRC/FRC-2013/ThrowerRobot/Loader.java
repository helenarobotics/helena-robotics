package robotics.helena;

import edu.wpi.first.wpilibj.Solenoid;

public class Loader implements Runnable {
    private static final int PUSH_TIME = 500;
    private static final int SPIN_UP_TIME = 1000;
    private final Shooter shooter;
    private final Solenoid pusher;
    private boolean loading = false;
    private int rpmSpeed;

    public Loader(Shooter shooter) {
        rpmSpeed = 0;
        this.shooter = shooter;
        pusher = new Solenoid(Configuration.PUSHER_SOLENOID);
        new Thread(this, "Loader Thread").start();
    }

    public void loadNext() {
        synchronized (this) {
            if (!loading) {
                loading = true;
                this.notifyAll();
            }
        }
    }

    public void setRPM(int rpm) {
        rpmSpeed = rpm;
        shooter.setPower(rpm / Shooter.MAX_RPM);
    }

    public void run() {
        while (true) {
            synchronized (this) {
                while (!loading) {
                    try {
                        this.wait();
                    } catch (InterruptedException ignored) {
                    }
                }
            }

            /*
            int targetPower = rpmSpeed / Shooter.MAX_RPM;
            if (targetPower < 0) {
                long endTime = shooter.getChangeTime() + SPIN_UP_TIME;
                if (System.currentTimeMillis() < endTime) {
                    long waitTime = endTime - System.currentTimeMillis();
                    try {
                        Thread.sleep(waitTime);
                    }catch(InterruptedException ignored){
                    }
                }
            }
            */
            load();

            synchronized (this) {
                loading = false;
            }
        }
    }

    private void load() {
        pusher.set(true);
        try {
            Thread.sleep(PUSH_TIME);
        } catch (InterruptedException ignored) {
        }
        pusher.set(false);
    }
}
