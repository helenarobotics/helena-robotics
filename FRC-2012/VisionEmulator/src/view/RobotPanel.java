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
    private PositionJLabel yPosLabel;
    private PositionJLabel rotPosLabel;

    public RobotPanel(Robot _robot) {
        super();

        // Keep track of the robot
        robot = _robot;

        // Which row are we working on!
        int gridRow = 0;

        // Label the panel!
        JLabel positionLabel = new JLabel("Robot Position");
        addComponent(positionLabel, 0, gridRow++, GridBagConstraints.SOUTH);

        // Movement controls
        JPanel robotMoveCtl = new RobotMoveCtl(robot);
        addComponent(robotMoveCtl, 0, gridRow++, GridBagConstraints.SOUTH);

        // Show the user the state of the robot
        JLabel xLabel = new JLabel("X-Position:");
        xPosLabel = new PositionJLabel(robot.getXOffset());
        addComponent(xLabel, 0, gridRow, GridBagConstraints.NORTHWEST);
        addComponent(xPosLabel, 1, gridRow++, GridBagConstraints.WEST);

        JLabel yLabel = new JLabel("Y-Position:");
        yPosLabel = new PositionJLabel(robot.getYOffset());
        addComponent(yLabel, 0, gridRow, GridBagConstraints.NORTHWEST);
        addComponent(yPosLabel, 1, gridRow++, GridBagConstraints.WEST);

        JLabel rotLabel = new JLabel("Rotation  :");
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
            // Make sure we keep space for 4 characters
            StringBuffer sb = new StringBuffer();
            sb.append(pos);
            while (sb.length() < 4)
                sb.insert(0, " ");
            
            setText(sb.toString());
        }
    }

    // One of the models (Camera or Robot) was updated, so repaint
    // the screen.
    public void update(Observable o, Object arg) {
//        if (o instanceof Robot) {
            xPosLabel.updatePos(robot.getXOffset());
            yPosLabel.updatePos(robot.getYOffset());
            rotPosLabel.updatePos(robot.getRotation());
//            repaint();
//        }
    }
}
