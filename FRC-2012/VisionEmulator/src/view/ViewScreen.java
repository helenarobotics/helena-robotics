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

    // Hoop information
    private static final double BOTTOM_HOOP_HEIGHT = 28.0;
    private static final double BOTTOM_HOOP_OFFSET = 0;

    private static final double MIDDLE_HOOP_HEIGHT = 61.0;
    private static final double MIDDLE_HOOP_OFFSET = 54.475 / 2.0;

    private static final double TOP_HOOP_HEIGHT = 98.0;
    private static final double TOP_HOOP_OFFSET = 0;

    // Camera and Robot information.
    private Camera camera;
    private Robot robot;

    // The hoop display objects
    private DistortHoop hoops[];
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

        // Create the hoops
        hoops = new DistortHoop[4];

        // Bottom hoop
        hoops[0] = new DistortHoop(BOTTOM_HOOP_OFFSET, BOTTOM_HOOP_HEIGHT);

        // Middle two hoops
        hoops[1] = new DistortHoop(-MIDDLE_HOOP_OFFSET, MIDDLE_HOOP_HEIGHT);
        hoops[2] = new DistortHoop(MIDDLE_HOOP_OFFSET, MIDDLE_HOOP_HEIGHT);

        // Top hoop
        hoops[3] = new DistortHoop(TOP_HOOP_OFFSET, TOP_HOOP_HEIGHT);

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

            // Draw the hoops
            for (DistortHoop h: hoops)
                h.paint(g, getSize(), robot, camera);
        } finally {
            // Back to standard locations.
            g.translate(-getWidth() / 2, -getHeight() / 2);
        }
    }
}
