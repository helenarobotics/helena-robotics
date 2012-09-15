package ctl;

import java.awt.GridBagConstraints;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

import javax.swing.JButton;
import javax.swing.Timer;

import awt.GridBagJPanel;

import model.Camera;

// This class keeps track of the camera panning controls.
public class CameraPanCtl extends GridBagJPanel {
    static final long serialVersionUID = -6580021515338452405L;

    // Keeps track of how the camera is tilted on the robot
    private Camera camera;

    public CameraPanCtl(Camera _camera) {
        super();

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
                    // This guarantee's at least some movement
                    camera.panUp();

                    // Keep moving as long as the button is clicked
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
                    // This guarantee's at least some movement
                    camera.panLeft();

                    // Keep moving as long as the button is clicked
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
                    // This guarantee's at least some movement
                    camera.panRight();

                    // Keep moving as long as the button is clicked
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
                    // This guarantee's at least some movement
                    camera.panDown();

                    // Keep moving as long as the button is clicked
                    timer = new Timer(25, panDownAction);
                    timer.start();
                }
                public void mouseReleased(MouseEvent e) {
                    timer.stop();
                }
            });
        addComponent(panDownButton, 1, 2, GridBagConstraints.CENTER);
    }
}
