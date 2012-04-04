package robotics.helena.widget;

import edu.wpi.first.smartdashboard.gui.StaticWidget;
import edu.wpi.first.smartdashboard.properties.ColorProperty;
import edu.wpi.first.smartdashboard.properties.Property;

import javax.swing.*;
import java.awt.*;

public class ModePWidget extends StaticWidget implements VarNotifier {
    public static final String NAME = "Mode PWidget (EK)";

    public final ColorProperty noColor = new ColorProperty(this, "No Mode", Color.black);
    public final ColorProperty autoColor = new ColorProperty(this, "Automatic Mode", Color.green);
    public final ColorProperty semiColor = new ColorProperty(this, "Semi-Automatic Mode", Color.cyan);
    public final ColorProperty manuColor = new ColorProperty(this, "Manual Mode", Color.orange);
    public final ColorProperty balColor = new ColorProperty(this, "Balance Mode", Color.yellow);

    private int mode = NONE;
    private JLabel stateLa = new JLabel("NO MODE");

    public static final int NONE = 0;
    public static final int AUTOSHOOT = 1;
    public static final int SEMIAUTO = 2;
    public static final int MANUAL = 3;
    public static final int BALANCE = 4;

    public void init() {
        WidgetComm.initialize();
        WidgetComm.registerVarNotifier(this);
        add(stateLa);
        drawForeground();
    }

    public void propertyChanged(Property property) {
        drawForeground();
    }

    public void varUpdate() {
        if(WidgetComm.mode != null){
            mode = WidgetComm.mode;
            drawForeground();
        }
    }

    public void drawForeground(){
        switch(mode) {
            case NONE:
                stateLa.setForeground(noColor.getValue());
                stateLa.setText("NO MODE");
                break;
            case AUTOSHOOT:
                stateLa.setForeground(autoColor.getValue());
                stateLa.setText("AUTOSHOOT MODE");
                break;
            case SEMIAUTO:
                stateLa.setForeground(semiColor.getValue());
                stateLa.setText("SEMIAUTO MODE");
                break;
            case MANUAL:
                stateLa.setForeground(manuColor.getValue());
                stateLa.setText("MANUAL MODE");
                break;
            case BALANCE:
                stateLa.setForeground(balColor.getValue());
                stateLa.setText("BALANCE MODE");
                break;
        }
    }
}
