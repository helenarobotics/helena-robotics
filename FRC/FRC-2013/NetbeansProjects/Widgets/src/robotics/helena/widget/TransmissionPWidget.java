package robotics.helena.widget;

import edu.wpi.first.smartdashboard.gui.StaticWidget;
import edu.wpi.first.smartdashboard.properties.ColorProperty;
import edu.wpi.first.smartdashboard.properties.IntegerProperty;
import edu.wpi.first.smartdashboard.properties.Property;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.tables.ITableListener;

import javax.swing.*;
import java.awt.*;

public class TransmissionPWidget extends StaticWidget implements ITableListener {
    public static final String NAME = "Transmission PWidget (EK)";

    public final IntegerProperty textSize = new IntegerProperty(this, "Text Size", 20);
    public final ColorProperty hiColor = new ColorProperty(this, "HI Transmission Color", Color.red);
    public final ColorProperty loColor = new ColorProperty(this, "LO Transmission Color", Color.green);

    private boolean hiTransmission = true;
    private JLabel transLa;

    public void init() {
        transLa = new JLabel();
        transLa.setFont(new Font("Dialog", Font.BOLD, textSize.getValue()));
        add(transLa);
        TableVars.table.addTableListener(this);
        changeTransmission();
    }

    public void propertyChanged(Property property) {
        if (property == textSize)
            transLa.setFont(new Font("Dialog", Font.BOLD, textSize.getValue()));
        else
            changeTransmission();
    }

    public void changeTransmission() {
        if (hiTransmission) {
            transLa.setText("HI");
            transLa.setForeground(hiColor.getValue());
        } else {
            transLa.setText("LO");
            transLa.setForeground(loColor.getValue());
        }
    }

    @Override
    public void valueChanged(ITable itable, String key, Object o, boolean bln) {
        if (key.equals(TableVars.transmission)) {
            hiTransmission = (boolean)o;
            changeTransmission();
        }
    }
}
