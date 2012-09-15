package view;

import java.awt.GridBagConstraints;

import java.util.Observable;
import java.util.Observer;

import javax.swing.JLabel;
import javax.swing.JPanel;

import awt.GridBagJPanel;

import ctl.CameraPanCtl;

import model.Camera;

public class CameraPanel extends GridBagJPanel implements Observer {
    static final long serialVersionUID = -6064147766105576852L;

    // Camera and Robot information.
    private Camera camera;

    // Labels for the robot data.
    private PositionJLabel azAngleLabel;
    private PositionJLabel incAngleLabel;

    public CameraPanel(Camera _camera) {
        super();

        // The model this effects
        camera = _camera;

        // Which row are we working on!
        int gridRow = 0;

        // Camera controls
        JLabel cameraLabel = new JLabel("Camera Controls");
        addComponent(cameraLabel, 0, gridRow++, 3, 1, GridBagConstraints.NORTH);

        // Movement controls
        JPanel cameraPanCtl = new CameraPanCtl(camera);
        addComponent(cameraPanCtl, 0, gridRow++, 3, 1, GridBagConstraints.NORTH);
        // Show the user the camera
        JLabel azLabel = new JLabel("Azimuth:      ");
        azAngleLabel = new PositionJLabel(camera.getAzimuthAngle());
        addComponent(azLabel, 0, gridRow, GridBagConstraints.NORTHWEST);
        addComponent(azAngleLabel, 1, gridRow++, GridBagConstraints.WEST);

        JLabel incLabel = new JLabel("Inclination:  ");
        incAngleLabel = new PositionJLabel(camera.getInclineAngle());
        addComponent(incLabel, 0, gridRow, GridBagConstraints.NORTHWEST);
        addComponent(incAngleLabel, 1, gridRow++, GridBagConstraints.WEST);

        // Watch the state of the robot so we can update the position
        // labels.
        camera.addObserver(this);
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
        azAngleLabel.updatePos(camera.getAzimuthAngle());
        incAngleLabel.updatePos(camera.getInclineAngle());
    }
}
