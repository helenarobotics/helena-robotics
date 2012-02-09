package view;

import java.awt.Color;
import java.awt.Graphics;

import javax.swing.JPanel;

import model.Camera;
import model.Robot;

class Hoop {
    // What color is the tape?
    private static final Color TAPE_COLOR = Color.RED;

    // All values are in inches!
    private static final double backboardOutsideWidth = 44.0;
    private static final double backboardOutsideHeight = 31.5;

    // The tape box inside the backboard!
    private static final double tapeOutsideWidth = 24.0;
    private static final double tapeOutsideHeight = 18.0;

    private static final double tapeInsideWidth = 20.0;
    private static final double tapeInsideHeight = 14.0;

    // The offset of this hoop in inches from the 'center' of the
    // origin.
    private final double xOffset;
    private final double yOffset;

    Hoop(double _xOffset, double _yOffset) {
        xOffset = _xOffset;
        yOffset = _yOffset;
    }

    void paint(Graphics g, Robot robot, Camera camera) {
        // Calculate the robot's distance from the center of the hoops
        double distance = Math.sqrt(Math.pow(robot.getXOffset(), 2) +
                                    Math.pow(robot.getYOffset(), 2));

        // XXX - @ 10' (120"), scale is ~8, and at 5' (60"), it should be 16.
        double scale = 960 / distance;

        // The origin is based on where the robot's location
        int xOrigin = -(int)(robot.getXOffset() * scale);
        int yOrigin = 0;

        // The rotation of the robots will move the location of the
        // backboard from the robot's perpective.  The math used is
        // based on: 1 = sin(x)^2 + cos(x)^2

        // XXX - Have the camera affect the display
//        xOrigin -= (int)(5 * camera.getAzimuthAngle() / scale);
//        yOrigin += (int)(5 * camera.getInclineAngle() / scale);

        // Now that we have the center of all the hoops at the
        // correct position, calculate the offset from the center
        // for this particular hoop.
        xOrigin += (int)(xOffset * scale);
        yOrigin += (int)(yOffset * scale);

        // Draw the hoop now!
        Color origColor = null;
        try {
            // Grab the current color
            origColor = g.getColor();

            // Draw the backboard!
            int width = (int)(backboardOutsideWidth * scale);
            int height = (int)(backboardOutsideHeight * scale);
            int startX = xOrigin - width / 2;
            int startY = yOrigin - height / 2;
            g.drawRect(startX, startY, width, height);

            // Draw the reflective tape with fillRect to give it width
            width = (int)(tapeOutsideWidth * scale);
            height = (int)(tapeOutsideHeight * scale);
            startX = xOrigin - width / 2;
            startY = yOrigin - height / 2;
            g.setColor(TAPE_COLOR);
            g.fillRect(startX, startY, width, height);

            // 'Unfill' to the inner amount
            width = (int)(tapeInsideWidth * scale);
            height = (int)(tapeInsideHeight * scale);
            startX = xOrigin - width / 2;
            startY = yOrigin - height / 2;
            g.setColor(Color.BLACK);
            g.fillRect(startX, startY, width, height);
        } finally {
            if (origColor != null)
                g.setColor(origColor);
        }
    }
}
