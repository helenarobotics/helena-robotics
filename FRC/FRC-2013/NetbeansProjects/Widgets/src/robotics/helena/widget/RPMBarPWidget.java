package robotics.helena.widget;

import edu.wpi.first.smartdashboard.gui.StaticWidget;
import edu.wpi.first.smartdashboard.properties.BooleanProperty;
import edu.wpi.first.smartdashboard.properties.IntegerProperty;
import edu.wpi.first.smartdashboard.properties.Property;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.tables.ITableListener;

import javax.swing.*;
import java.awt.*;

public class RPMBarPWidget extends StaticWidget implements ITableListener{
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
        TableVars.table.addTableListener(this);
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

    @Override
    public void valueChanged(ITable itable, String key, Object o, boolean bln) {
        if(key.equals(TableVars.rpm1)){
            if(textProp.getValue())
                topRPMBar.setString("Top " + (String)o);
            else
                topRPMBar.setValue((int)o);
        }else if(key.equals(TableVars.rpm1Target)){
            if(textProp.getValue())
                topTargetBar.setString("TopTar " + (String)o);
            else
                topTargetBar.setValue((int)o);
        }else if(key.equals(TableVars.rpm2)){
            if(textProp.getValue())
                btmRPMBar.setString("Btm " + (String)o);
            else
                btmRPMBar.setValue((int)o);
        }else if(key.equals(TableVars.rpm2Target)){
            if(textProp.getValue())
                btmTargetBar.setString("BtmTar " + (String)o);
            else
                btmTargetBar.setValue((int)o);
        }
    }
}
