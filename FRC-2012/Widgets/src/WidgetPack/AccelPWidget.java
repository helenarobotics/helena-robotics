package robotics.helena.widget;

import edu.wpi.first.smartdashboard.gui.StaticWidget;
import edu.wpi.first.smartdashboard.properties.BooleanProperty;
import edu.wpi.first.smartdashboard.properties.IntegerProperty;
import edu.wpi.first.smartdashboard.properties.Property;

import javax.swing.*;
import java.awt.*;

public class AccelPWidget extends StaticWidget implements VarNotifier {
    public static final String NAME = "AccelBar PWidget (EK)";

    public final IntegerProperty minProp = new IntegerProperty(this, "Minimum Value", 0);
    public final IntegerProperty maxProp = new IntegerProperty(this, "Maximum Value", 10);
    public final BooleanProperty horizProp = new BooleanProperty(this, "Horizontal Orientation?", true);
    public final BooleanProperty textProp = new BooleanProperty(this, "Show Values?", true);

    private JProgressBar xBar;
    private JProgressBar yBar;
    private JProgressBar zBar;

    public void init() {
        WidgetComm.initialize();
        JPanel barPa = new JPanel();
        if(horizProp.getValue()){
            barPa.setLayout(new GridLayout(3, 1, 2, 2));
            xBar = new JProgressBar(SwingConstants.HORIZONTAL,minProp.getValue()*10,maxProp.getValue()*10);
            yBar = new JProgressBar(SwingConstants.HORIZONTAL,minProp.getValue()*10,maxProp.getValue()*10);
            zBar = new JProgressBar(SwingConstants.HORIZONTAL,minProp.getValue()*10,maxProp.getValue()*10);
        }else{
            barPa.setLayout(new GridLayout(1, 3, 2, 2));
            xBar = new JProgressBar(SwingConstants.VERTICAL,minProp.getValue()*10,maxProp.getValue()*10);
            yBar = new JProgressBar(SwingConstants.VERTICAL,minProp.getValue()*10,maxProp.getValue()*10);
            zBar = new JProgressBar(SwingConstants.VERTICAL,minProp.getValue()*10,maxProp.getValue()*10);
        }

        if(textProp.getValue()){
            xBar.setStringPainted(true);
            yBar.setStringPainted(true);
            zBar.setStringPainted(true);
        }
        setLayout(new BorderLayout());

        barPa.add(xBar);
        barPa.add(yBar);
        barPa.add(zBar);
        add(new JLabel("Accelerometer"), BorderLayout.NORTH);
        add(barPa, BorderLayout.CENTER);
        WidgetComm.registerVarNotifier(this);
        updateUI();
    }

    public void propertyChanged(Property property) {
        JPanel barPa = new JPanel();
        if(horizProp.getValue()){
            barPa.setLayout(new GridLayout(3, 1, 2, 2));
            xBar = new JProgressBar(SwingConstants.HORIZONTAL,minProp.getValue()*10,maxProp.getValue()*10);
            yBar = new JProgressBar(SwingConstants.HORIZONTAL,minProp.getValue()*10,maxProp.getValue()*10);
            zBar = new JProgressBar(SwingConstants.HORIZONTAL,minProp.getValue()*10,maxProp.getValue()*10);
        }else{
            barPa.setLayout(new GridLayout(1, 3, 2, 2));
            xBar = new JProgressBar(SwingConstants.VERTICAL,minProp.getValue()*10,maxProp.getValue()*10);
            yBar = new JProgressBar(SwingConstants.VERTICAL,minProp.getValue()*10,maxProp.getValue()*10);
            zBar = new JProgressBar(SwingConstants.VERTICAL,minProp.getValue()*10,maxProp.getValue()*10);
        }

        if(textProp.getValue()){
            xBar.setStringPainted(true);
            yBar.setStringPainted(true);
            zBar.setStringPainted(true);
        }
        setLayout(new BorderLayout());

        barPa.add(xBar);
        barPa.add(yBar);
        barPa.add(zBar);
        add(new JLabel("Accelerometer"), BorderLayout.NORTH);
        add(barPa, BorderLayout.CENTER);
        updateUI();
    }

    public void varUpdate() {
        if(WidgetComm.accelX != null)
            xBar.setValue((int)(WidgetComm.accelX*10));
        if(WidgetComm.accelY != null)
            yBar.setValue((int)(WidgetComm.accelY*10));
        if(WidgetComm.accelZ != null)
            zBar.setValue((int)(WidgetComm.accelZ*10));

        if(textProp.getValue()){
            if(WidgetComm.accelX != null)
                xBar.setString("X: " + WidgetComm.accelX);
            if(WidgetComm.accelY != null)
                yBar.setString("Y: " + WidgetComm.accelY);
            if(WidgetComm.accelZ != null)
                zBar.setString("Z: " + WidgetComm.accelZ);
        }
        updateUI();
    }
}
