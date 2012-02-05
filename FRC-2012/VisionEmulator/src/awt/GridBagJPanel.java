package awt;

import java.awt.Component;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;

import javax.swing.JPanel;

public class GridBagJPanel extends JPanel {
    static final long serialVersionUID = 3622479044192078902L;

    // layout made global so we can call addComponent with less parms
    private GridBagLayout gb;

    public GridBagJPanel() {
        super(new GridBagLayout());
        gb = (GridBagLayout)getLayout();
    }

    // Convenience method to help us layout the components.
    protected void addComponent(Component c, int gridx, int gridy, int anchor) {
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.gridx = gridx;
        gbc.gridy = gridy;
        gbc.anchor = anchor;
        gb.setConstraints(c, gbc);
        add(c);
    }
}
