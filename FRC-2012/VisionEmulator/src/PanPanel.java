import java.awt.Component;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.Timer;

// This class keeps track of the camera panning controls.
class PanPanel extends JPanel {
    static final long serialVersionUID = 8207174934040299405L;

    // Limit how far the camera can tilt
    private static final int MAX_ANGLE_INCLINE = 30;
    private static final int MIN_ANGLE_INCLINE = -30;
    private static final int MAX_ANGLE_AZIMUTH = 45;
    private static final int MIN_ANGLE_AZIMUTH = -45;

    // Keeps track of how the camera is tilted on the robot
    private int angleIncline;
    private int angleAzimuth;
    
    // UI fields made global since it makes things easier.
    private GridBagLayout gb;

    // The model we're controlling!
    private CameraPanel cp;

    PanPanel(CameraPanel _cp) {
        super(new GridBagLayout());
        gb = (GridBagLayout)getLayout();

        // Set the defaults!
        angleIncline = 0;
        angleAzimuth = 0;

        // Keep track of what we're controlling!
        cp = _cp;
        cp.setCameraAzimuth(angleAzimuth);
        cp.setCameraInclination(angleIncline);

        // Pan buttons
        JButton panUpButton = new JButton("Pan Up");
        final ActionListener panUpAction = new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    if (angleIncline < MAX_ANGLE_INCLINE) {
                        angleIncline++;
                        cp.setCameraInclination(angleIncline);
                    }
                }
            };
        panUpButton.addMouseListener(new MouseListener() {
                private Timer timer;
                public void mouseClicked(MouseEvent ignored) { }
                public void mouseEntered(MouseEvent ignored) { }
                public void mouseExited(MouseEvent ignored) { }
                public void mousePressed(MouseEvent e) {
                    timer = new Timer(25, panUpAction);
                    timer.start();
                }
                public void mouseReleased(MouseEvent e) {
                    timer.stop();
                }
            });
        addComponent(panUpButton, 1, 0, GridBagConstraints.CENTER);

        JButton panLeftButton = new JButton("Pan Left");
        final ActionListener panLeftAction = new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    if (angleAzimuth > MIN_ANGLE_AZIMUTH) {
                        angleAzimuth--;
                        cp.setCameraAzimuth(angleAzimuth);
                    }
                }
            };
        panLeftButton.addMouseListener(new MouseListener() {
                private Timer timer;
                public void mouseClicked(MouseEvent ignored) { }
                public void mouseEntered(MouseEvent ignored) { }
                public void mouseExited(MouseEvent ignored) { }
                public void mousePressed(MouseEvent e) {
                    timer = new Timer(25, panLeftAction);
                    timer.start();
                }
                public void mouseReleased(MouseEvent e) {
                    timer.stop();
                }
            });
        addComponent(panLeftButton, 0, 1, GridBagConstraints.WEST);

        JButton panResetButton = new JButton("Reset");
        panResetButton.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    angleAzimuth = 0;
                    angleIncline = 0;
                    cp.setCameraAzimuth(angleAzimuth);
                    cp.setCameraInclination(angleIncline);
                }
            });
        addComponent(panResetButton, 1, 1, GridBagConstraints.CENTER);

        JButton panRightButton = new JButton("Pan Right");
        final ActionListener panRightAction = new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    if (angleAzimuth < MAX_ANGLE_AZIMUTH) {
                        angleAzimuth++;
                        cp.setCameraAzimuth(angleAzimuth);
                    }
                }
            };
        panRightButton.addMouseListener(new MouseListener() {
                private Timer timer;
                public void mouseClicked(MouseEvent ignored) { }
                public void mouseEntered(MouseEvent ignored) { }
                public void mouseExited(MouseEvent ignored) { }
                public void mousePressed(MouseEvent e) {
                    timer = new Timer(25, panRightAction);
                    timer.start();
                }
                public void mouseReleased(MouseEvent e) {
                    timer.stop();
                }
            });
        addComponent(panRightButton, 2, 1, GridBagConstraints.EAST);

        JButton panDownButton = new JButton("Pan Down");
        final ActionListener panDownAction = new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    if (angleIncline > MIN_ANGLE_INCLINE) {
                        angleIncline--;
                        cp.setCameraInclination(angleIncline);
                    }
                }
            };
        panDownButton.addMouseListener(new MouseListener() {
                private Timer timer;
                public void mouseClicked(MouseEvent ignored) { }
                public void mouseEntered(MouseEvent ignored) { }
                public void mouseExited(MouseEvent ignored) { }
                public void mousePressed(MouseEvent e) {
                    timer = new Timer(25, panDownAction);
                    timer.start();
                }
                public void mouseReleased(MouseEvent e) {
                    timer.stop();
                }
            });
        addComponent(panDownButton, 1, 2, GridBagConstraints.CENTER);
    }

    // Convenience method to help us layout the components.
    private void addComponent(Component c, int gridx, int gridy, int anchor) {
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.gridx = gridx;
        gbc.gridy = gridy;
        gbc.anchor = anchor;
        gb.setConstraints(c, gbc);
        add(c);
    }
}
