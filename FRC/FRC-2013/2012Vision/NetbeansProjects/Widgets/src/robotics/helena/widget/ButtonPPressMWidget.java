package robotics.helena.widget;

import edu.wpi.first.smartdashboard.gui.StaticWidget;
import edu.wpi.first.smartdashboard.properties.BooleanProperty;
import edu.wpi.first.smartdashboard.properties.ColorProperty;
import edu.wpi.first.smartdashboard.properties.Property;
import edu.wpi.first.smartdashboard.properties.StringProperty;
import edu.wpi.first.wpilibj.tables.ITableListener;
import edu.wpi.first.wpilibj.tables.ITable;
import java.awt.*;

public class ButtonPPressMWidget extends StaticWidget implements ITableListener {
    public static final String NAME = "ButtonPress MPWidget (EK)";

    public final StringProperty varProp = new StringProperty(this, "Variable Name:","");
    public final ColorProperty outlineColorProp = new ColorProperty(this, "Outline Color", Color.black);
    public final ColorProperty activeColorProp = new ColorProperty(this, "Active Color", Color.green);
    public final ColorProperty deactiveColorProp = new ColorProperty(this, "Deactive Color", Color.gray);
    public final BooleanProperty rectProp = new BooleanProperty(this, "Rectangular", true);

    private boolean pressed;
    private boolean active = false;

    protected void paintComponent(Graphics g) {
        if (active) {
            if (pressed)
                g.setColor(activeColorProp.getValue());
            else
                g.setColor(deactiveColorProp.getValue());
            if (rectProp.getValue()) {
                g.fillRect(0, 0, (int) getSize().getWidth(), (int) getSize().getHeight());
                g.setColor(outlineColorProp.getValue());
                g.drawRect(0, 0, (int) getSize().getWidth() - 1, (int) getSize().getHeight() - 1);
            } else {
                g.fillOval(0, 0, (int)getSize().getWidth(), (int)getSize().getHeight());
                g.setColor(outlineColorProp.getValue());
                g.drawOval(0, 0, (int)getSize().getWidth() - 1, (int)getSize().getHeight() - 1);
            }
        } else {
            g.setFont(new Font("Dialog", Font.BOLD, 10));
            g.setColor(Color.red);
            g.drawString("Inactive", 5, 15);
        }
    }

    public void init() {
        setPreferredSize(new Dimension(50, 50));
        TableVars.table.addTableListener(this);
    }

    public void propertyChanged(Property property) {
        //Nothing important needs to happen here
    }

    @Override
    public void valueChanged(ITable itable, String key, Object o, boolean bln) {
        if (key.equals(varProp.getValue())) {
            pressed = (boolean)o;
            repaint();
        }
    }
}
