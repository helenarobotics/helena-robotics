package ctl;

import java.awt.GridBagConstraints;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

import javax.swing.JButton;
import javax.swing.Timer;

import awt.GridBagJPanel;

import model.Robot;

// This class keeps track of the camera panning controls.
public class RobotMoveCtl extends GridBagJPanel {
    static final long serialVersionUID = 7489210016077070671L;

    // Keeps track robot position
    private Robot robot;

    public RobotMoveCtl(Robot _robot) {
        super();

        // The model this effects
        robot = _robot;

        // Movement buttons
        JButton moveInButton = new JButton("Move In");
        final ActionListener moveInAction = new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    robot.moveIn();
                }
            };
        moveInButton.addMouseListener(new MouseListener() {
                private Timer timer;
                public void mouseClicked(MouseEvent ignored) { }
                public void mouseEntered(MouseEvent ignored) { }
                public void mouseExited(MouseEvent ignored) { }
                public void mousePressed(MouseEvent e) {
                    // This guarantee's at least some movement
                    robot.moveIn();

                    // Keep moving as long as the button is clicked
                    timer = new Timer(100, moveInAction);
                    timer.start();
                }
                public void mouseReleased(MouseEvent e) {
                    timer.stop();
                }
            });
        addComponent(moveInButton, 1, 0, GridBagConstraints.CENTER);

        JButton strafeLeftButton = new JButton("Strafe Left");
        final ActionListener strafeLeftAction = new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    robot.strafeLeft();
                }
            };
        strafeLeftButton.addMouseListener(new MouseListener() {
                private Timer timer;
                public void mouseClicked(MouseEvent ignored) { }
                public void mouseEntered(MouseEvent ignored) { }
                public void mouseExited(MouseEvent ignored) { }
                public void mousePressed(MouseEvent e) {
                    // This guarantee's at least some movement
                    robot.strafeLeft();

                    // Keep moving as long as the button is clicked
                    timer = new Timer(100, strafeLeftAction);
                    timer.start();
                }
                public void mouseReleased(MouseEvent e) {
                    timer.stop();
                }
            });
        addComponent(strafeLeftButton, 0, 1, GridBagConstraints.WEST);

        JButton moveResetButton = new JButton("Reset");
        moveResetButton.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    robot.resetPosition();
                }
            });
        addComponent(moveResetButton, 1, 1, GridBagConstraints.CENTER);

        JButton strafeRightButton = new JButton("Strafe Right");
        final ActionListener strafeRightAction = new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    robot.strafeRight();
                }
            };
        strafeRightButton.addMouseListener(new MouseListener() {
                private Timer timer;
                public void mouseClicked(MouseEvent ignored) { }
                public void mouseEntered(MouseEvent ignored) { }
                public void mouseExited(MouseEvent ignored) { }
                public void mousePressed(MouseEvent e) {
                    // This guarantee's at least some movement
                    robot.strafeRight();

                    // Keep moving as long as the button is clicked
                    timer = new Timer(100, strafeRightAction);
                    timer.start();
                }
                public void mouseReleased(MouseEvent e) {
                    timer.stop();
                }
            });
        addComponent(strafeRightButton, 2, 1, GridBagConstraints.EAST);

        JButton moveOutButton = new JButton("Move Out");
        final ActionListener moveOutAction = new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    robot.moveOut();
                }
            };
        moveOutButton.addMouseListener(new MouseListener() {
                private Timer timer;
                public void mouseClicked(MouseEvent ignored) { }
                public void mouseEntered(MouseEvent ignored) { }
                public void mouseExited(MouseEvent ignored) { }
                public void mousePressed(MouseEvent e) {
                    // This guarantee's at least some movement
                    robot.moveOut();

                    // Keep moving as long as the button is clicked
                    timer = new Timer(100, moveOutAction);
                    timer.start();
                }
                public void mouseReleased(MouseEvent e) {
                    timer.stop();
                }
            });
        addComponent(moveOutButton, 1, 2, GridBagConstraints.CENTER);

        JButton rotateLtButton = new JButton("Rotate Left");
        final ActionListener rotateLtAction = new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    robot.rotateLeft();
                }
            };
        rotateLtButton.addMouseListener(new MouseListener() {
                private Timer timer;
                public void mouseClicked(MouseEvent ignored) { }
                public void mouseEntered(MouseEvent ignored) { }
                public void mouseExited(MouseEvent ignored) { }
                public void mousePressed(MouseEvent e) {
                    // This guarantee's at least some movement
                    robot.rotateLeft();

                    // Keep moving as long as the button is clicked
                    timer = new Timer(50, rotateLtAction);
                    timer.start();
                }
                public void mouseReleased(MouseEvent e) {
                    timer.stop();
                }
            });
        addComponent(rotateLtButton, 0, 3, GridBagConstraints.CENTER);

        JButton rotateRtButton = new JButton("Rotate Right");
        final ActionListener rotateRtAction = new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    robot.rotateRight();
                }
            };
        rotateRtButton.addMouseListener(new MouseListener() {
                private Timer timer;
                public void mouseClicked(MouseEvent ignored) { }
                public void mouseEntered(MouseEvent ignored) { }
                public void mouseExited(MouseEvent ignored) { }
                public void mousePressed(MouseEvent e) {
                    // This guarantee's at least some movement
                    robot.rotateRight();

                    // Keep moving as long as the button is clicked
                    timer = new Timer(50, rotateRtAction);
                    timer.start();
                }
                public void mouseReleased(MouseEvent e) {
                    timer.stop();
                }
            });
        addComponent(rotateRtButton, 2, 3, GridBagConstraints.CENTER);
    }
}
