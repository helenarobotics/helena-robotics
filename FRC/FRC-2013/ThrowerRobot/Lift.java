/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package robotics.helena;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.Victor;

public class Lift {
    private final Victor lift;
    public Lift(){
        lift = new Victor(Configuration.LIFT_VICTOR);
    }
    
    public void control(Joystick joy){
        if(joy.getRawButton(Configuration.LIFT_UP_BUTTON))
            lift.set(0.5);
        else if(joy.getRawButton(Configuration.LIFT_DOWN_BUTTON))
            lift.set(-0.5);
        else
            lift.set(0);
    }
}
