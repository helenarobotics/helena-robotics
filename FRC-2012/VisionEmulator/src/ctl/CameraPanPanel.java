package ctl;

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

import model.Camera;

// This class keeps track of the camera panning controls.
public class CameraPanPanel extends JPanel {
    static final long serialVersionUID = 6888917891958253130L;

    // Keeps track of how the camera is tilted on the robot
    private Camera camera;

    // UI fields made global since it makes things easier.
    private GridBagLayout gb;

    public CameraPanPanel(Camera _camera) {
        super(new GridBagLayout());
        gb = (GridBagLayout)getLayout();

        // The model this effects
        camera = _camera;

        // Pan buttons
        JButton panUpButton = new JButton("Pan Up");
        final ActionListener panUpAction = new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    camera.panUp();
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
                    camera.panLeft();
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
                    camera.resetAngle();
                }
            });
        addComponent(panResetButton, 1, 1, GridBagConstraints.CENTER);

        JButton panRightButton = new JButton("Pan Right");
        final ActionListener panRightAction = new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    camera.panRight();
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
                    camera.panDown();
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
