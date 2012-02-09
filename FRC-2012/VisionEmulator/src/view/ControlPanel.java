package view;

import java.awt.GridBagConstraints;

import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;

import awt.GridBagJPanel;

import model.Camera;
import model.Robot;

public class ControlPanel extends GridBagJPanel {
    static final long serialVersionUID = -3181498060452390286L;

    // Camera and Robot information.
    private Camera camera;
    private Robot robot;

    public ControlPanel(Camera camera, Robot robot) {
        super();

        // Which row are we working on!
        int gridRow = 0;

        // Camera controls
//        JPanel cameraPanel = new CameraPanel(camera);
//        addComponent(cameraPanel, 0, gridRow++, GridBagConstraints.NORTH);

        // Robot controls
        JPanel robotPanel = new RobotPanel(robot);
        addComponent(robotPanel, 0, gridRow++, GridBagConstraints.NORTH);
    }
}
