import java.awt.Container;

import java.awt.Component;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;

import model.Camera;
import model.Robot;

import view.ControlPanel;
import view.ViewScreen;

public class GUI implements Runnable {
    public static void main(String[] args) {
        // Create an Application object
        GUI gui = new GUI();

        // Run the Swing thread.
        // invokeLater requires a class that implements the Runnable interface
        javax.swing.SwingUtilities.invokeLater(gui);
    }

    // UI fields made global since it makes things easier.
    private Container content;
    private GridBagLayout gb;

    public void run() {
        // Create a new window with a title
        JFrame frame = new JFrame("Shooter Tracker UI");

        // Allow the application to exit if the user closes the frame
        // by clicking on the X in the corner.
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        // Get the frame's content pane which is where the UI components
        // are added.
        content = frame.getContentPane();

        // Set the layout manager
        gb = new GridBagLayout();
        content.setLayout(gb);

        // The models
        final Camera camera = new Camera();
        final Robot robot = new Robot();

        // Top-level panel where the 'emulated camera images' will be
        // displayed.
        final ViewScreen viewScreen = new ViewScreen(camera, robot);
        addComponent(viewScreen, 0, 0, GridBagConstraints.NORTHWEST);

        // Control panel to control the image
        final JPanel controlPanel = new ControlPanel(camera, robot);
        addComponent(controlPanel, 1, 0, GridBagConstraints.NORTHEAST);

        // Finally, an Exit button!
        final JButton exitButton = new JButton("Exit");
        exitButton.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    System.exit(0);
                }
            });
        addComponent(exitButton, 1, 0, GridBagConstraints.SOUTH);

        // Make the frame visible.
        frame.pack();
        frame.setResizable(false);
        frame.setVisible(true);
    }

    // Convenience method to help us layout the components.
    private void addComponent(Component c, int gridx, int gridy, int anchor) {
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.gridx = gridx;
        gbc.gridy = gridy;
        gbc.insets = new Insets(10, 10, 10, 10);
        gbc.anchor = anchor;
        gb.setConstraints(c, gbc);
        content.add(c);
    }
}
