package robotics.helena.widget;

import edu.wpi.first.smartdashboard.gui.StaticWidget;
import edu.wpi.first.smartdashboard.properties.Property;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class TableEditor extends StaticWidget implements ActionListener {
    public static final String NAME = "TableEditor (EK)";

    private JTextField keyFi = new JTextField();
    private JTextField valueFi = new JTextField();
    private String[] typeAr = {"Integer", "Boolean", "Double"};
    private JComboBox types;
    private JLabel keyLa = new JLabel("Key: ");
    private JLabel valueLa = new JLabel("Value: ");
    private JButton commitBu = new JButton("Commit");

    public void init() {
        setPreferredSize(new Dimension(150, 100));
        setLayout(new GridLayout(3, 2, 2, 2));
        add(keyLa);
        add(keyFi);
        add(valueLa);
        add(valueFi);
        types = new JComboBox(typeAr);
        add(types);
        add(commitBu);
        commitBu.addActionListener(this);
    }

    public void propertyChanged(Property property) {
        //Not relevant
    }

    public void actionPerformed(ActionEvent e) {
        if (types.getSelectedIndex() == 0) {
            TableVars.table.putInt(keyFi.getText(), Integer.parseInt(valueFi.getText()));
        } else if (types.getSelectedIndex() == 1) {
            TableVars.table.putBoolean(keyFi.getText(), Boolean.parseBoolean(valueFi.getText()));
        } else {
            TableVars.table.putDouble(keyFi.getText(), Double.parseDouble(valueFi.getText()));
            System.out.println("Setting " + keyFi.getText() + " to " + Double.parseDouble(valueFi.getText()));
        }
    }
}
