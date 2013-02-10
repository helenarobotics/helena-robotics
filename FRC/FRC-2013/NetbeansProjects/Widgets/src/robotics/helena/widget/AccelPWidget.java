package robotics.helena.widget;

import edu.wpi.first.smartdashboard.gui.StaticWidget;
import edu.wpi.first.smartdashboard.properties.BooleanProperty;
import edu.wpi.first.smartdashboard.properties.IntegerProperty;
import edu.wpi.first.smartdashboard.properties.Property;
import edu.wpi.first.wpilibj.tables.ITableListener;
import edu.wpi.first.wpilibj.tables.ITable;
import javax.swing.*;
import java.awt.*;

public class AccelPWidget extends StaticWidget implements ITableListener {
    public static final String NAME = "AccelBar PWidget (EK)";

    public final IntegerProperty minProp = new IntegerProperty(this, "Minimum Value", 0);
    public final IntegerProperty maxProp = new IntegerProperty(this, "Maximum Value", 10);
    public final BooleanProperty horizProp = new BooleanProperty(this, "Horizontal Orientation?", true);
    public final BooleanProperty textProp = new BooleanProperty(this, "Show Values?", true);

    private JProgressBar xBar;
    private JProgressBar yBar;
    private JProgressBar zBar;

    public void init() {
        JPanel barPa = new JPanel();
        if (horizProp.getValue()) {
            barPa.setLayout(new GridLayout(3, 1, 2, 2));
            xBar = new JProgressBar(SwingConstants.HORIZONTAL, minProp.getValue() * 10, maxProp.getValue() * 10);
            yBar = new JProgressBar(SwingConstants.HORIZONTAL, minProp.getValue() * 10, maxProp.getValue() * 10);
            zBar = new JProgressBar(SwingConstants.HORIZONTAL, minProp.getValue() * 10, maxProp.getValue() * 10);
        } else {
            barPa.setLayout(new GridLayout(1, 3, 2, 2));
            xBar = new JProgressBar(SwingConstants.VERTICAL, minProp.getValue() * 10, maxProp.getValue() * 10);
            yBar = new JProgressBar(SwingConstants.VERTICAL, minProp.getValue() * 10, maxProp.getValue() * 10);
            zBar = new JProgressBar(SwingConstants.VERTICAL, minProp.getValue() * 10, maxProp.getValue() * 10);
        }

        if (textProp.getValue()) {
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
        TableVars.table.addTableListener(this);
        updateUI();
    }

    public void propertyChanged(Property property) {
        JPanel barPa = new JPanel();
        if (horizProp.getValue()) {
            barPa.setLayout(new GridLayout(3, 1, 2, 2));
            xBar = new JProgressBar(SwingConstants.HORIZONTAL, minProp.getValue() * 10, maxProp.getValue() * 10);
            yBar = new JProgressBar(SwingConstants.HORIZONTAL, minProp.getValue() * 10, maxProp.getValue() * 10);
            zBar = new JProgressBar(SwingConstants.HORIZONTAL, minProp.getValue() * 10, maxProp.getValue() * 10);
        } else {
            barPa.setLayout(new GridLayout(1, 3, 2, 2));
            xBar = new JProgressBar(SwingConstants.VERTICAL, minProp.getValue() * 10, maxProp.getValue() * 10);
            yBar = new JProgressBar(SwingConstants.VERTICAL, minProp.getValue() * 10, maxProp.getValue() * 10);
            zBar = new JProgressBar(SwingConstants.VERTICAL, minProp.getValue() * 10, maxProp.getValue() * 10);
        }

        if (textProp.getValue()) {
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

    @Override
    public void valueChanged(ITable itable, String key, Object o, boolean bln) {
        if (key.equals(TableVars.accelX)) {
            if (textProp.getValue()) {
               xBar.setString("X: " + o);
            } else {
               xBar.setString((String)o);
            }
            updateUI();
        } else if (key.equals(TableVars.accelY)) {
             if (textProp.getValue()) {
               yBar.setString("Y: " + o);
            } else {
               yBar.setString((String)o);
            }
            updateUI();
        } else if (key.equals(TableVars.accelZ)) {
             if (textProp.getValue()) {
               zBar.setString("Z: " + o);
            } else {
               zBar.setString((String)o);
            }
            updateUI();
        }
    }
}
