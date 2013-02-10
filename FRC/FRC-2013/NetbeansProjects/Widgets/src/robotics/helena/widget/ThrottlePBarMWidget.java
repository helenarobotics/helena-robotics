package robotics.helena.widget;

import edu.wpi.first.smartdashboard.gui.StaticWidget;
import edu.wpi.first.smartdashboard.properties.ColorProperty;
import edu.wpi.first.smartdashboard.properties.Property;
import edu.wpi.first.smartdashboard.properties.StringProperty;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.tables.ITableListener;

import java.awt.*;

public class ThrottlePBarMWidget extends StaticWidget implements ITableListener {
    public static final String NAME = "ThrottleBar MPWidget (EK)";

    public final StringProperty varProp = new StringProperty(this,"Variable Name:","Joystick 2 Throttle");
    public final ColorProperty blankColorProp = new ColorProperty(this, "Blank Color", Color.gray);
    public final ColorProperty fillColorProp = new ColorProperty(this, "Fill Color", Color.blue);
    public final ColorProperty outlineColorProp = new ColorProperty(this, "Outline Color", Color.black);

    private double throttle = -1;
    private boolean active = false;

    public void init() {
        setPreferredSize(new Dimension(25, 75));
        TableVars.table.addTableListener(this);
    }

    public void propertyChanged(Property prop) {
        //Nothing important needs to happen here
    }

    @Override
    public void valueChanged(ITable itable, String key, Object o, boolean bln) {
        if (key.equals(TableVars.j2Throttle)) {
            throttle = (double)o;
            repaint();
        }
    }

    protected void paintComponent(Graphics g) {
        if (active) {
            g.setColor(blankColorProp.getValue());
            g.fillRect(0, 0, (int)getSize().getWidth(), (int)getSize().getHeight() - 2);
            g.setColor(fillColorProp.getValue());
            g.fillRect(0, (int)((throttle + 1.0) * ((int)getSize().getHeight())), (int)getSize().getWidth(), (int)( -throttle * ((int)getSize().getHeight())));
        } else {
            g.setFont(new Font("Dialog", Font.BOLD, 20));
            g.setColor(Color.red);
            g.drawString("Inactive", 5, 25);
        }
    }
}
