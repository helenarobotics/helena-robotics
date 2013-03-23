package robotics.helena;

import edu.wpi.first.wpilibj.Victor;

public class Loader implements Runnable {
    private static final int PUSH_TIME = 300;
    private static final int SPIN_UP_TIME = 1000;
    private final Shooter shooter;
    private final Victor pusher;
    private boolean loading = false;
    private int rpmSpeed;

    public Loader(Shooter shooter) {
        rpmSpeed = 0;
        this.shooter = shooter;
        pusher = new Victor(Configuration.PUSHER_VICTOR);
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

            load();

            synchronized (this) {
                loading = false;
            }
        }
    }

    private void load() {
        pusher.set(-1);
        try {
            Thread.sleep(PUSH_TIME);
        } catch (InterruptedException ignored) {
        }
        pusher.set(1);
        try {
            Thread.sleep(50);
        } catch (InterruptedException ignored) {
        }
        pusher.set(0);
    }
}
