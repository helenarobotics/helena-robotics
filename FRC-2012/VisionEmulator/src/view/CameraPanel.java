package view;

import java.awt.GridBagConstraints;

import javax.swing.JLabel;
import javax.swing.JPanel;

import awt.GridBagJPanel;

import ctl.CameraPanCtl;

import model.Camera;

public class CameraPanel extends GridBagJPanel {
    static final long serialVersionUID = 781606162575220696L;

    // Camera and Robot information.
    private Camera camera;

    public CameraPanel(Camera camera) {
        super();

        // Which row are we working on!
        int gridRow = 0;

        // Camera controls
        JLabel cameraLabel = new JLabel("Camera Controls");
        addComponent(cameraLabel, 0, gridRow++, GridBagConstraints.NORTH);
        JPanel cameraPanCtl = new CameraPanCtl(camera);
        addComponent(cameraPanCtl, 0, gridRow++, GridBagConstraints.SOUTH);
    }
}
