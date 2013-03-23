package robotics.helena;

import edu.wpi.first.wpilibj.Victor;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.tables.ITableListener;
import edu.wpi.first.wpilibj.networktables.NetworkTable;

public class AutoControl {
    static class AngleAdjust implements Runnable, ITableListener {
//        private static final int START_MODE = 0;
//        private static final int ON_MODE = 1;
//        private int mode = START_MODE;

        private double angle;
        private double inputEnergy;
        private double neededEnergy;
        private long lastTime;
        private static final int THRESHOLD = 3;
        private static final double km = 3.1415; // This constant needs to be determined
        private static final double kf = 6.2832; // Another constant to be determined

        private final Victor left;
        private final Victor right;

        public AngleAdjust() {
            left = new Victor(Configuration.LEFT_VICTOR);
            right = new Victor(Configuration.RIGHT_VICTOR);
            NetworkTable.getTable("*SmartDashboard*").addTableListener(this);
        }

        public void run() {
            while (Math.abs(angle) > THRESHOLD) {
                if (new Double(angle) != null) {
                    if (new Long(lastTime) != null) {
                        inputEnergy += km * (System.currentTimeMillis() - lastTime) / 1000;
                    }
                    lastTime = System.currentTimeMillis();
                    neededEnergy = km * Math.abs(angle);
                    if (inputEnergy < kf)
                        neededEnergy += kf;

                    if (angle > 0) {
                        left.set(1);
                        left.set(-1);
                    } else {
                        left.set(-1);
                        right.set(1);
                    }
                }
                try {
                    Thread.sleep(10);
                } catch (InterruptedException ignored) {
                }
            }
            left.set(0);
            right.set(0);
        }

        public void valueChanged(ITable itable, String key, Object o, boolean bln) {
            if (key.equals("Angle")) {
                angle = ((Double)o).doubleValue();
            }
        }
    }
}
