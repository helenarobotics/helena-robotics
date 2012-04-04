package robotics.helena.widget;

import edu.wpi.first.smartdashboard.gui.StaticWidget;
import edu.wpi.first.smartdashboard.properties.BooleanProperty;
import edu.wpi.first.smartdashboard.properties.ColorProperty;
import edu.wpi.first.smartdashboard.properties.Property;
import edu.wpi.first.smartdashboard.properties.StringProperty;

import java.awt.*;

public class ButtonPPressMWidget extends StaticWidget implements VarNotifier {
    public static final String NAME = "ButtonPress MPWidget (EK)";
    
    public final StringProperty varProp = new StringProperty(this, "Variable Name:","");
    public final ColorProperty outlineColorProp = new ColorProperty(this, "Outline Color", Color.black);
    public final ColorProperty activeColorProp = new ColorProperty(this, "Active Color", Color.green);
    public final ColorProperty deactiveColorProp = new ColorProperty(this, "Deactive Color", Color.gray);
    public final BooleanProperty rectProp = new BooleanProperty(this, "Rectangular", true);

    private boolean pressed;
    private boolean active = false;

    protected void paintComponent(Graphics g){
        if(active){
            if(pressed)
                g.setColor(activeColorProp.getValue());
            else
                g.setColor(deactiveColorProp.getValue());
            if(rectProp.getValue()){
                g.fillRect(0, 0, (int) getSize().getWidth(), (int) getSize().getHeight());
                g.setColor(outlineColorProp.getValue());
                g.drawRect(0, 0, (int) getSize().getWidth(), (int) getSize().getHeight());
            }else{
                g.fillOval(0,0,(int)getSize().getWidth(),(int)getSize().getHeight());
                g.setColor(outlineColorProp.getValue());
                g.drawOval(0,0,(int)getSize().getWidth(),(int)getSize().getHeight());
            }
        }else{
            g.setFont(new Font("Dialog", Font.BOLD, 20));
            g.setColor(Color.red);
            g.drawString("Inactive", 5, 25);
        }
    }

    public void init() {
        WidgetComm.initialize();
        setPreferredSize(new Dimension(15,15));
        if(!varProp.getValue().equals(""))
            active = true;
        WidgetComm.registerVarNotifier(this);
    }

    public void propertyChanged(Property property) {
        if(property == varProp)
            active = true;
    }
    public boolean validatePropertyChange(Property prop, Object value){
        if(prop == varProp && WidgetComm.typeFromName((String)value) != WidgetComm.BOOLEAN)
            return false;
        return true;
    }

    public void varUpdate() {
        if(WidgetComm.getBooleanFromName(varProp.getValue()) != null){
            pressed = WidgetComm.getBooleanFromName(varProp.getValue());
            repaint();
        }
    }
}
