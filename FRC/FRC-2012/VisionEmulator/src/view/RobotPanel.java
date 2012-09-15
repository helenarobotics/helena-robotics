package view;

import java.awt.GridBagConstraints;

import java.util.Observable;
import java.util.Observer;

import javax.swing.JLabel;
import javax.swing.JPanel;

import awt.GridBagJPanel;

import ctl.RobotMoveCtl;

import model.Robot;

public class RobotPanel extends GridBagJPanel implements Observer {
    static final long serialVersionUID = -9195455780246045250L;

    // Robot information.
    private Robot robot;

    // Labels for the robot data.
    private PositionJLabel xPosLabel;
    private PositionJLabel zPosLabel;
    private PositionJLabel rotPosLabel;

    public RobotPanel(Robot _robot) {
        super();

        // Keep track of the robot
        robot = _robot;

        // Which row are we working on!
        int gridRow = 0;

        // Label the panel!
        JLabel positionLabel = new JLabel("Robot Position");
        addComponent(positionLabel, 0, gridRow++, 3, 1, GridBagConstraints.NORTH);

        // Movement controls
        JPanel robotMoveCtl = new RobotMoveCtl(robot);
        addComponent(robotMoveCtl, 0, gridRow++, 3, 1, GridBagConstraints.NORTH);

        // Show the user the state of the robot
        JLabel xLabel = new JLabel("X-Position:  ");
        xPosLabel = new PositionJLabel(robot.getXOffset());
        addComponent(xLabel, 0, gridRow, GridBagConstraints.NORTHWEST);
        addComponent(xPosLabel, 1, gridRow++, GridBagConstraints.WEST);

        JLabel zLabel = new JLabel("Z-Position:  ");
        zPosLabel = new PositionJLabel(robot.getZOffset());
        addComponent(zLabel, 0, gridRow, GridBagConstraints.NORTHWEST);
        addComponent(zPosLabel, 1, gridRow++, GridBagConstraints.WEST);

        JLabel rotLabel = new JLabel("Rotation:   ");
        rotPosLabel = new PositionJLabel(robot.getRotation());
        addComponent(rotLabel, 0, gridRow, GridBagConstraints.NORTHWEST);
        addComponent(rotPosLabel, 1, gridRow++, GridBagConstraints.WEST);

        // Watch the state of the robot so we can update the position
        // labels.
        robot.addObserver(this);
    }

    private class PositionJLabel extends JLabel {
        // Keep Java happy
        static final long serialVersionUID = -1L;

        PositionJLabel(int pos) {
            super();
            updatePos(pos);
        }

        void updatePos(int pos) {
            setText("" + pos);
        }
    }

    // The Robot model was updated, so update the information.
    public void update(Observable o, Object arg) {
        xPosLabel.updatePos(robot.getXOffset());
        zPosLabel.updatePos(robot.getZOffset());
        rotPosLabel.updatePos(robot.getRotation());
    }
}
