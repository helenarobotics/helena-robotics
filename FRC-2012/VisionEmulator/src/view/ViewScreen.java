package view;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;

import java.util.Observable;
import java.util.Observer;

import javax.swing.JPanel;

import model.Camera;
import model.Robot;

public class ViewScreen extends JPanel implements Observer {
    static final long serialVersionUID = -7075535651446670324L;

    // Camera and Robot information.
    private Camera camera;
    private Robot robot;

    // The hoop display objects
    private DistortHoop hoop;
//    private Hoop hoops[];

    public ViewScreen(Camera _camera, Robot _robot) {
        // Turn on double-buffering
        super(true);
        setPreferredSize(new Dimension(800, 600));
        setBackground(Color.BLACK);
        setForeground(Color.WHITE);

        // Keep track of the objects we're monitoring.
        camera = _camera;
        robot = _robot;

        // We'll be drawing images onto this, so no layout!
        setLayout(null);

        hoop = new DistortHoop(0, robot.getHeight());
/*
        // Create the hoops
        hoops = new Hoop[4];

        // Bottom hoop
        hoops[0] = new Hoop(0, 35.0, 28.0 - robot.getHeight());

        // Middle two hoops
        hoops[1] = new Hoop(-27.375, 1.5, 61.0 - robot.getHeight());
        hoops[2] = new Hoop(+27.375, 1.5, 61.0 - robot.getHeight());

        // Top hoop
        hoops[3] = new Hoop(0, -35.0, 98.0 - robot.getHeight());
*/
        // Subscribe to changes in the model
        camera.addObserver(this);
        robot.addObserver(this);
    }

    // One of the models (Camera or Robot) was updated, so repaint
    // the screen.
    public void update(Observable o, Object arg) {
        repaint();
    }

    public void paint(Graphics g) {
        // Parent constructor first...
        super.paint(g);

        try {
            // Make the center of the screen 0, 0
            g.translate(getWidth() / 2, getHeight() / 2);

            hoop.paint(g, getWidth(), getHeight(), robot, camera);
            // Draw the hoops
//            for (Hoop h: hoops)
//                h.paint(g, robot, camera);
        } finally {
            // Back to standard locations.
            g.translate(-getWidth() / 2, -getHeight() / 2);
        }
    }
}
