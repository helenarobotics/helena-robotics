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

    // The hoop display objects.
    private Hoop hoops[];

    public ViewScreen(Camera _camera, Robot _robot) {
        // Turn on double-buffering
        super(true);
//        setPreferredSize(new Dimension(1024, 768));
//        setPreferredSize(new Dimension(800, 600));
        setPreferredSize(new Dimension(640, 480));
        setBackground(Color.BLACK);
        setForeground(Color.WHITE);

        // We'll be drawing images onto this, so no layout!
        setLayout(null);

        // Create the hoops
        hoops = new Hoop[4];

        // Bottom hoop
        hoops[0] = new Hoop(0, 35.0);

        // Middle two hoops
        hoops[1] = new Hoop(-27.375, 1.5);
        hoops[2] = new Hoop(+27.375, 1.5);

        // Top hoop
        hoops[3] = new Hoop(0, -35.0);

        // Keep track of the objects we're monitoring.
        camera = _camera;
        robot = _robot;

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

        // Find the center of the screen
        int xPos = getWidth() / 2;
        int yPos = getHeight() / 2;

        // XXX - Calculate how to display based on scale, azimuth, and
        // inclination.

        // Calculate the robot distance from the basket.
        double dist = Math.sqrt(Math.pow(robot.getXOffset(), 2) +
                                Math.pow(robot.getYOffset(), 2));

        // @ 10' (120"), scale is ~8, and at 5' (60"), it should be 16.
        double scale = 960 / dist;
        xPos -= (int)(5 * camera.getAzimuthAngle() / scale);
        yPos += (int)(5 * camera.getInclineAngle() / scale);

        // Draw the hoops
        for (Hoop h: hoops)
            h.paint(g, xPos, yPos, scale);
    }
}
