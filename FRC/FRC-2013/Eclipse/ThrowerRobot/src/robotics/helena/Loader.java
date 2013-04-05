package robotics.helena;

import edu.wpi.first.wpilibj.Victor;

public class Loader implements Runnable {
    private static final int PUSH_TIME = 150;
    private static final int PULL_TIME = 70;
    private final Victor pusher;
    private boolean loading = false;

    public Loader() {
        pusher = new Victor(Configuration.PUSHER_VICTOR);
        new Thread(this, "Loader Thread").start();
    }

    public void pack() {
        // By default, the arm starts in, move it back out.
        push();
    }

    public void unpack() {
        // By default, the arm starts in, move it back out.
        pull();
    }

    public void loadNext() {
        synchronized (this) {
            if (!loading) {
                loading = true;
                this.notifyAll();
            }
        }
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
        push();
        pull();
    }

    private void push() {
        // Move the arm in
        pusher.set(-1);
        try {
            Thread.sleep(PUSH_TIME);
        } catch (InterruptedException ignored) {
        }

        // Quit moving!
        pusher.set(0);
    }

    private void pull() {
        // Move the arm back out.  Note, we expect the
        // arm to hit something on the way back in, so moving
        // out we use a smaller number to (hopefully) move us
        // to the same starting position.
        pusher.set(1);
        try {
            Thread.sleep(PULL_TIME);
        } catch (InterruptedException ignored) {
        }

        // Quit moving!
        pusher.set(0);
    }
}
