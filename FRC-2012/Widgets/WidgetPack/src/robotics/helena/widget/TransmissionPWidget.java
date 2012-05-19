package robotics.helena.widget;

import edu.wpi.first.smartdashboard.gui.StaticWidget;
import edu.wpi.first.smartdashboard.properties.ColorProperty;
import edu.wpi.first.smartdashboard.properties.IntegerProperty;
import edu.wpi.first.smartdashboard.properties.Property;

import javax.swing.*;
import java.awt.*;

public class TransmissionPWidget extends StaticWidget implements VarNotifier {
    public static final String NAME = "Transmission PWidget (EK)";

    public final IntegerProperty textSize = new IntegerProperty(this, "Text Size", 20);
    public final ColorProperty hiColor = new ColorProperty(this, "HI Transmission Color", Color.red);
    public final ColorProperty loColor = new ColorProperty(this, "LO Transmission Color", Color.green);

    private boolean hiTransmission = true;
    private JLabel transLa;

    public void init() {
        WidgetComm.initialize();
        transLa = new JLabel();
        transLa.setFont(new Font("Dialog",Font.BOLD, textSize.getValue()));
        add(transLa);
        WidgetComm.registerVarNotifier(this);
        changeTransmission();
    }

    public void propertyChanged(Property property) {
        if(property == textSize)
            transLa.setFont(new Font("Dialog",Font.BOLD, textSize.getValue()));
        else
            changeTransmission();
    }

    public void changeTransmission(){
        if(hiTransmission){
            transLa.setText("HI");
            transLa.setForeground(hiColor.getValue());
        }else{
            transLa.setText("LO");
            transLa.setForeground(loColor.getValue());
        }
    }

    public void varUpdate() {
        if(WidgetComm.transmission != null){
            hiTransmission = WidgetComm.transmission;
            changeTransmission();
        }
    }
}
