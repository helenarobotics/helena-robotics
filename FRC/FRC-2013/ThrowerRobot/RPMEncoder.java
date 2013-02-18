/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package robotics.helena;

import edu.wpi.first.wpilibj.Counter;
import edu.wpi.first.wpilibj.DigitalInput;

public class RPMEncoder implements Runnable{
    private int curRpm;
    private final long INTERVAL_TIME = 10;
    
    private static final int MS_PER_MIN = 60000;
    private static final int TICKS_PER_REV = 1440;
    
    private static final float RPM_PER_TICKSMS = MS_PER_MIN / TICKS_PER_REV;
    
    public RPMEncoder(){
        new Thread(this,"RPM Encoder").start();
    }

    public void run() {
        Counter counter = new Counter(new DigitalInput(Configuration.SHOOTER_ENCODER));
        counter.start();
        counter.reset();
        long lastReadTime = System.currentTimeMillis();
        while(true){
            long now = System.currentTimeMillis();
            long dt = now - lastReadTime;
            if(dt > 0){
                long ticks = counter.get();
                curRpm = (int)(RPM_PER_TICKSMS * (ticks/dt));
            }
            lastReadTime = now;
            counter.reset();
            try{
                Thread.sleep(INTERVAL_TIME);
            }catch(InterruptedException ignored){}
        }
    }        
    
    public int getRPM(){
        return curRpm;
    }
}
