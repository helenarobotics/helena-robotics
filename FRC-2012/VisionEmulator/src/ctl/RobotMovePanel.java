package ctl;

import java.awt.Component;
import java.awt.FlowLayout;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.Timer;

import model.Robot;

// This class keeps track of the camera panning controls.
public class RobotMovePanel extends JPanel {
    static final long serialVersionUID = 6148694585428512407L;

    // The model we're controlling!
    private Robot robot;

    public RobotMovePanel(Robot _robot) {
        super(new FlowLayout());

        // Keep track of what we're controlling!
        robot = _robot;

        // Move-In button
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
                    timer = new Timer(100, moveInAction);
                    timer.start();
                }
                public void mouseReleased(MouseEvent e) {
                    timer.stop();
                }
            });
        add(moveInButton);

        // Move-out
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
                    timer = new Timer(100, moveOutAction);
                    timer.start();
                }
                public void mouseReleased(MouseEvent e) {
                    timer.stop();
                }
            });
        add(moveOutButton);
    }
}
