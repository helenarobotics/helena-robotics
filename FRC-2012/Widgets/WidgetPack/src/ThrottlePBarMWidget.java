package robotics.helena.widget;

import edu.wpi.first.smartdashboard.gui.StaticWidget;
import edu.wpi.first.smartdashboard.properties.ColorProperty;
import edu.wpi.first.smartdashboard.properties.Property;
import edu.wpi.first.smartdashboard.properties.StringProperty;

import java.awt.*;

public class ThrottlePBarMWidget extends StaticWidget implements VarNotifier{
    public static final String NAME = "ThrottleBar MPWidget (EK)";

    public final StringProperty varProp = new StringProperty(this,"Variable Name:","Joystick 2 Throttle");
    public final ColorProperty blankColorProp = new ColorProperty(this, "Blank Color", Color.gray);
    public final ColorProperty fillColorProp = new ColorProperty(this, "Fill Color", Color.blue);
    public final ColorProperty outlineColorProp = new ColorProperty(this, "Outline Color", Color.black);

    private double throttle = -1;
    private boolean active = false;

    public void init(){
        WidgetComm.initialize();
        setPreferredSize(new Dimension(25,75));
        WidgetComm.registerVarNotifier(this);
    }

    public void propertyChanged(Property prop){
        //Nothing important needs to happen here
    }

    public void varUpdate(){
        active = WidgetComm.getDouble(varProp.getValue()) != null;
        if(active){
            throttle = WidgetComm.getDouble(varProp.getValue());
            repaint();
        }
    }

    protected void paintComponent(Graphics g){
        if(active){
            g.setColor(outlineColorProp.getValue());
            g.fillRect(0,0,(int)getSize().getWidth(),(int)getSize().getHeight());
            g.setColor(blankColorProp.getValue());
            g.fillRect(1,1,(int)getSize().getWidth()-2,(int)getSize().getHeight()-2);
            g.setColor(fillColorProp.getValue());
            g.fillRect(1,(int)((1-(throttle+1.0)/2)*((int)getSize().getHeight()-2)),(int)getSize().getWidth()-2,(int)((throttle+1)/2*((int)getSize().getHeight()-2)+2));
        }else{
            g.setFont(new Font("Dialog", Font.BOLD, 20));
            g.setColor(Color.red);
            g.drawString("Inactive", 5, 25);
        }
    }
}
