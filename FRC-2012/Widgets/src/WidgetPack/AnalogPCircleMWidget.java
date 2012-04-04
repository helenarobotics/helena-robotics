package robotics.helena.widget;

import edu.wpi.first.smartdashboard.gui.StaticWidget;
import edu.wpi.first.smartdashboard.properties.BooleanProperty;
import edu.wpi.first.smartdashboard.properties.ColorProperty;
import edu.wpi.first.smartdashboard.properties.Property;
import edu.wpi.first.smartdashboard.properties.StringProperty;

import java.awt.*;

public class AnalogPCircleMWidget extends StaticWidget implements VarNotifier {
    public static final String NAME = "AnalogCircle MPWidget (EK)";
    
    private Double direction = 0.0;
    private Double magnitude = 0.0;
    private Double twist = 0.0;
    private boolean activeDir = false;
    private boolean activeMagn = false;
    private boolean activeTwist = false;

    public final StringProperty dirVarProp = new StringProperty(this, "Direction Variable:", "");
    public final StringProperty magnVarProp = new StringProperty(this, "Magnitude Variable:", "");
    public final StringProperty twistVarProp = new StringProperty(this, "Twist Variable:", "");
    public final ColorProperty fillColorProp = new ColorProperty(this, "Fill Color", Color.black);
    public final ColorProperty twistColorProp = new ColorProperty(this, "Twist Color", Color.green);
    public final ColorProperty twistFillColorProp = new ColorProperty(this, "Twist Fill Color", Color.gray);
    public final ColorProperty directionColorProp = new ColorProperty(this, "Direction Color", Color.green);
    public final BooleanProperty showFillTwistProp = new BooleanProperty(this, "Show Fill Twist", true);
    

    public void init() {
        WidgetComm.initialize();
        setPreferredSize(new Dimension(50,50));
        if(!dirVarProp.getValue().equals(""))
            activeDir = true;
        if(!magnVarProp.getValue().equals(""))
            activeMagn = true;
        if(!twistVarProp.getValue().equals(""))
            activeTwist = true;
        WidgetComm.registerVarNotifier(this);
    }

    public void propertyChanged(Property property) {
        if(property == dirVarProp)
            activeDir = true;
        if(property == magnVarProp)
            activeMagn = true;
        if(property == twistVarProp)
            activeTwist = true;
    }

    public boolean validatePropertyChange(Property prop, Object value){
        if(prop instanceof StringProperty && WidgetComm.typeFromName((String)value) != WidgetComm.DOUBLE)
            return false;
        return true;
    }

    protected void paintComponent(Graphics g){
        if(activeTwist && activeDir && activeMagn){
            g.setColor(fillColorProp.getValue());
            g.fillOval(0,0,(int)getSize().getWidth(),(int)getSize().getHeight());

            if(showFillTwistProp.getValue()) {
                g.setColor(twistFillColorProp.getValue());
                g.fillArc(0,0,(int)getSize().getWidth(),(int)getSize().getHeight(),90,(int)(180*twist));
            }

            g.setColor(Color.black);
            g.drawOval(0,0,(int)getSize().getWidth(),(int)getSize().getHeight());

            g.setColor(directionColorProp.getValue());
            g.drawLine((int)getSize().getWidth()/2,(int)getSize().getHeight()/2,(int)(Math.cos(direction)*getSize().getWidth()/2 * magnitude + getSize().getWidth()/2),(int)(getSize().getHeight()/2 - Math.sin(direction)*getSize().getHeight()/2 * magnitude));

            g.setColor(twistColorProp.getValue());
            g.drawArc(0,0,(int)getSize().getWidth(),(int)getSize().getHeight(),90,(int)(180*twist));
        }else if(activeTwist){
            g.setColor(fillColorProp.getValue());
            g.fillOval(0,0,(int)getSize().getWidth(),(int)getSize().getHeight());

            if(showFillTwistProp.getValue()) {
                g.setColor(twistFillColorProp.getValue());
                g.fillArc(0,0,(int)getSize().getWidth(),(int)getSize().getHeight(),90,(int)(180*twist));
            }

            g.setColor(Color.black);
            g.drawOval(0,0,(int)getSize().getWidth(),(int)getSize().getHeight());

            g.setColor(twistColorProp.getValue());
            g.drawArc(0,0,(int)getSize().getWidth(),(int)getSize().getHeight(),90,(int)(180*twist));
        }else if(activeDir && activeMagn){
            g.setColor(fillColorProp.getValue());
            g.fillOval(0,0,(int)getSize().getWidth(),(int)getSize().getHeight());

            g.setColor(Color.black);
            g.drawOval(0,0,(int)getSize().getWidth(),(int)getSize().getHeight());

            g.setColor(directionColorProp.getValue());
            g.drawLine((int)getSize().getWidth()/2,(int)getSize().getHeight()/2,(int)(Math.cos(direction)*getSize().getWidth()/2 * magnitude + getSize().getWidth()/2),(int)(getSize().getHeight()/2 - Math.sin(direction)*getSize().getHeight()/2 * magnitude));
        }else{
            g.setFont(new Font("Dialog", Font.BOLD, 20));
            g.setColor(Color.red);
            g.drawString("Inactive", 5, 25);
        }
    }

    public void varUpdate() {
        if(activeDir && activeMagn && WidgetComm.getDoubleFromName(dirVarProp.getValue()) != null && WidgetComm.getDoubleFromName(magnVarProp.getValue()) != null) {
            direction = WidgetComm.getDoubleFromName(dirVarProp.getValue());
            magnitude = WidgetComm.getDoubleFromName(magnVarProp.getValue());
            repaint();
        }
        if(activeTwist && WidgetComm.getDoubleFromName(twistVarProp.getValue()) != null){
            twist = WidgetComm.getDoubleFromName(twistVarProp.getValue());
            repaint();
        }
    }
}
