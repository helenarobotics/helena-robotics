package robotics.helena.widget;

import edu.wpi.first.smartdashboard.gui.StaticWidget;
import edu.wpi.first.smartdashboard.properties.BooleanProperty;
import edu.wpi.first.smartdashboard.properties.IntegerProperty;
import edu.wpi.first.smartdashboard.properties.Property;

import javax.swing.*;
import java.awt.*;

public class RPMBarPWidget extends StaticWidget implements VarNotifier{
    public static final String NAME = "RPMBar PWidget (EK)";

    public final IntegerProperty minProp = new IntegerProperty(this, "Minimum", 0);
    public final IntegerProperty maxProp = new IntegerProperty(this, "Maximum", 2600);
    public final BooleanProperty horizProp = new BooleanProperty(this,"Horizontal", true);
    public final BooleanProperty textProp = new BooleanProperty(this, "Show Values", true);

    private JProgressBar topRPMBar;
    private JProgressBar btmRPMBar;
    private JProgressBar topTargetBar;
    private JProgressBar btmTargetBar;

    public void init() {
        WidgetComm.initialize();
        if(horizProp.getValue()){
            topRPMBar = new JProgressBar(SwingConstants.HORIZONTAL, minProp.getValue(), maxProp.getValue());
            btmRPMBar = new JProgressBar(SwingConstants.HORIZONTAL, minProp.getValue(), maxProp.getValue());
            topTargetBar = new JProgressBar(SwingConstants.HORIZONTAL, minProp.getValue(), maxProp.getValue());
            btmTargetBar = new JProgressBar(SwingConstants.HORIZONTAL, minProp.getValue(), maxProp.getValue());
            topRPMBar.setStringPainted(textProp.getValue());
            btmRPMBar.setStringPainted(textProp.getValue());
            topTargetBar.setStringPainted(textProp.getValue());
            btmTargetBar.setStringPainted(textProp.getValue());
            setPreferredSize(new Dimension(50,10));
            setLayout(new GridLayout(2,1,1,1));
            add(topRPMBar);
            add(topTargetBar);
            add(btmRPMBar);
            add(btmTargetBar);
        }else{
            topRPMBar = new JProgressBar(SwingConstants.HORIZONTAL, minProp.getValue(), maxProp.getValue());
            btmRPMBar = new JProgressBar(SwingConstants.HORIZONTAL, minProp.getValue(), maxProp.getValue());
            topTargetBar = new JProgressBar(SwingConstants.HORIZONTAL, minProp.getValue(), maxProp.getValue());
            btmTargetBar = new JProgressBar(SwingConstants.HORIZONTAL, minProp.getValue(), maxProp.getValue());
            topRPMBar.setStringPainted(textProp.getValue());
            btmRPMBar.setStringPainted(textProp.getValue());
            topTargetBar.setStringPainted(textProp.getValue());
            btmTargetBar.setStringPainted(textProp.getValue());
            setPreferredSize(new Dimension(50,10));
            setLayout(new GridLayout(2,1,1,1));
            add(topRPMBar);
            add(topTargetBar);
            add(btmRPMBar);
            add(btmTargetBar);
        }
        WidgetComm.registerVarNotifier(this);
    }

    public void propertyChanged(Property property) {
        removeAll();
        if(property != textProp){
            if(horizProp.getValue()){
                topRPMBar = new JProgressBar(SwingConstants.HORIZONTAL, minProp.getValue(), maxProp.getValue());
                btmRPMBar = new JProgressBar(SwingConstants.HORIZONTAL, minProp.getValue(), maxProp.getValue());
                topTargetBar = new JProgressBar(SwingConstants.HORIZONTAL, minProp.getValue(), maxProp.getValue());
                btmTargetBar = new JProgressBar(SwingConstants.HORIZONTAL, minProp.getValue(), maxProp.getValue());
                topRPMBar.setStringPainted(textProp.getValue());
                btmRPMBar.setStringPainted(textProp.getValue());
                topTargetBar.setStringPainted(textProp.getValue());
                btmTargetBar.setStringPainted(textProp.getValue());
                setPreferredSize(new Dimension(50,10));
                setLayout(new GridLayout(2,1,1,1));
                add(topRPMBar);
                add(btmRPMBar);
            }else{
                topRPMBar = new JProgressBar(SwingConstants.HORIZONTAL, minProp.getValue(), maxProp.getValue());
                btmRPMBar = new JProgressBar(SwingConstants.HORIZONTAL, minProp.getValue(), maxProp.getValue());
                topTargetBar = new JProgressBar(SwingConstants.HORIZONTAL, minProp.getValue(), maxProp.getValue());
                btmTargetBar = new JProgressBar(SwingConstants.HORIZONTAL, minProp.getValue(), maxProp.getValue());
                topRPMBar.setStringPainted(textProp.getValue());
                btmRPMBar.setStringPainted(textProp.getValue());
                topTargetBar.setStringPainted(textProp.getValue());
                btmTargetBar.setStringPainted(textProp.getValue());
                setPreferredSize(new Dimension(50,10));
                setLayout(new GridLayout(2,1,1,1));
                add(topRPMBar);
                add(btmRPMBar);
            }
        }
    }

    public void varUpdate() {
        if(WidgetComm.rpm1 != null)
            topRPMBar.setValue((int)WidgetComm.rpm1.doubleValue());
        if(WidgetComm.rpm1Target != null)
            topTargetBar.setValue((int)WidgetComm.rpm1Target.doubleValue());
        if(WidgetComm.rpm2 != null)
            btmRPMBar.setValue((int)WidgetComm.rpm2.doubleValue());
        if(WidgetComm.rpm2Target != null)
            btmTargetBar.setValue((int)WidgetComm.rpm2Target.doubleValue());
        if(textProp.getValue()){
            topRPMBar.setString("Top RPM: " + WidgetComm.rpm1);
            topTargetBar.setString("Top Target: " + WidgetComm.rpm1Target);
            btmRPMBar.setString("Lower RPM: " + WidgetComm.rpm2);
            btmTargetBar.setString("Lower Target: " + WidgetComm.rpm2Target);
        }
    }
}
