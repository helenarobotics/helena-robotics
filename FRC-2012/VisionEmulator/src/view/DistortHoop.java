package view;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Polygon;
import java.awt.geom.Point2D;
import java.awt.geom.Point2D.Double;

import javax.swing.JPanel;

import model.Camera;
import model.Robot;

class DistortHoop {
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
    // bottom of the backboard.  yOffset is the height fromt the floor,
    // and zOffset is the distance from the backboard.
    private final double xOffset;
    private final double yOffset;
    private final double zOffset;

    // Backboard points
    private Point2D bbPts[];

    // Outside reflective tape points
    private Point2D ortPts[];

    // Inside reflective tape points
    private Point2D irtPts[];
    
    DistortHoop(double _xOffset, double _yOffset) {
        xOffset = _xOffset;

        // We place the offsets at the center of the hoop's
        // backboard. Since the hoop is on the bottom of the backboard,
        // increase the zOffset by half the height of the backboard.
        yOffset = _yOffset + backboardOutsideHeight / 2;

        // The hoops are on the back wall, which by convention is at
        // location 0.
        zOffset = 0;

        // We have three rectangular 'objects' here, Backboard, outside
        // reflective tape, and inside reflective tape.  For each of
        // these objects, we calculate all four corners.
        // - UpperLeft
        // - UpperRight
        // - LowerRight
        // - LowerLeft

        // Backboard
        bbPts = cornerPoints(
            xOffset, yOffset, backboardOutsideWidth, backboardOutsideHeight);
        
        // Outside Reflective Tape border
        ortPts = cornerPoints(
            xOffset, yOffset, tapeOutsideWidth, tapeOutsideHeight);

        // Inside Reflective Tape border
        irtPts = cornerPoints(
            xOffset, yOffset, tapeInsideWidth, tapeInsideHeight);
    }

    void paint(Graphics g, Dimension screenDimension,
               Robot robot, Camera camera) {
        // Calculate the angles from the robot to the four corners
        // in all three planes (XY, XZ, and YZ).
        double bbAngles[][] = calcAngles(robot, bbPts);
        double ortAngles[][] = calcAngles(robot, ortPts);
        double irtAngles[][] = calcAngles(robot, irtPts);

        // Determine within the XY plane where the robot's camera is
        // pointing along the backboard, which will help determine where
        // we draw the pixels (left or right of the camera's center
        // point).
        //
        // The robot rotations only changes the XOffset, we can easily
        // determine this location since we know one leg of a right
        // triangle (YOffset), and the angle (rotation), which can give
        // us the additional XOffset.
        double rotXOffset = robot.getZOffset() *
                            Math.cos(Math.toRadians(robot.getRotation()));

        // The camera's center-point along the back wall
        Point2D cameraCenter =
            new Point2D.Double(robot.getXOffset() + rotXOffset, yOffset);

        // Go through each of the angles to see if we subtract or add to
        // them based on whether or not the point is left/right to the
        // cameraCenter.
        bbAngles = correctAngles(robot.getRotation(), cameraCenter,
                                 bbPts, bbAngles);
        ortAngles = correctAngles(robot.getRotation(), cameraCenter,
                                  ortPts, ortAngles);
        irtAngles = correctAngles(robot.getRotation(), cameraCenter,
                                  irtPts, irtAngles);

        // At this point, we can map the angles to the pixel lengths, but
        // the pixels we display on the screen may be smaller/larger
        // than the camera view, so scale the visual screen to what is
        // being shown.
        double scaledHorizPixels = Camera.HORIZONTAL_ANGLE_TO_PIXELS *
            screenDimension.getWidth() / Camera.HORIZONTAL_PIXELS;
        double scaledVertPixels = Camera.VERTICAL_ANGLE_TO_PIXELS *
            screenDimension.getHeight() / Camera.VERTICAL_PIXELS;

        // Create the 3 Polygons that make up the 4 points of the
        // 'rectangle'.
        Polygon bbPoly = createPoly(scaledHorizPixels, scaledVertPixels,
                                    bbAngles);
        Polygon ortPoly = createPoly(scaledHorizPixels, scaledVertPixels,
                                     ortAngles);
        Polygon irtPoly = createPoly(scaledHorizPixels, scaledVertPixels,
                                     irtAngles);

        // Draw the hoop now!
        Color origColor = null;
        try {
            // Grab the current color
            origColor = g.getColor();

            // Draw the backboard!
            g.drawPolygon(bbPoly);

            // Draw the reflective tape with fillRect to give it width
            g.setColor(TAPE_COLOR);
            g.fillPolygon(ortPoly);

            // 'Unfill' to the inner amount
            g.setColor(Color.BLACK);
            g.fillPolygon(irtPoly);
        } finally {
            if (origColor != null)
                g.setColor(origColor);
        }
    }

    // The four corners of the objects.
    private enum REC_PTS { UPPER_LEFT, UPPER_RIGHT, LOWER_RIGHT, LOWER_LEFT };

    // Return a array containing the upper-left, upper-right,
    // lower-left, and lower-right points of the 'rectangle'.
    private Point2D[] cornerPoints(double x, double y, double w, double h) {
        Point2D pts[] = new Point2D[REC_PTS.values().length];

        // Because y is always 0, we only store X/Z values.
        pts[REC_PTS.UPPER_LEFT.ordinal()] =
            new Point2D.Double(x - w / 2, y + h / 2);
        pts[REC_PTS.UPPER_RIGHT.ordinal()] =
            new Point2D.Double(x + w / 2, y + h / 2);
        pts[REC_PTS.LOWER_RIGHT.ordinal()] =
            new Point2D.Double(x + w / 2, y - h / 2);
        pts[REC_PTS.LOWER_LEFT.ordinal()] =
            new Point2D.Double(x - w / 2, y - h / 2);

        return pts;
    }

    // The planes correspond to the TOP_DOWN, SIDE, and ROBOT
    // perspectives respectively.
    private enum Plane { XZ, YZ, XY };

    private Plane intToPlane(int i) {
        if (i == Plane.XZ.ordinal())
            return Plane.XZ;
        else if (i == Plane.YZ.ordinal())
            return Plane.YZ;
        else if (i == Plane.XY.ordinal())
            return Plane.XY;

        // The code should blow up
        return null;
    }
        
    // Return a 2D array containing the upper-left, upper-right,
    // lower-left, and lower-right points of the 'rectangle' in
    // all three perspectives.
    private double[][] calcAngles(Robot robot, Point2D cornerPts[]) {
        // Calculate the angles
        double angles[][] =
            new double[Plane.values().length][cornerPts.length];

        for (int i = 0; i < angles.length; i++) {
            // We calculate the distance from the robot to the corner points
            // to give us the hypotenuse of the right triangle.
            Plane p = intToPlane(i);

            // Generate the robot's 2D position based on the current
            // perspective.
            Point2D robotPt = null;
            switch (p) {
            case XZ:
                robotPt = new Point2D.Double(robot.getXOffset(),
                                             robot.getZOffset());
                break;

            case YZ:
                robotPt = new Point2D.Double(robot.getZOffset(),
                                             robot.getYOffset());
                break;

            case XY:
                robotPt = new Point2D.Double(robot.getXOffset(),
                                             robot.getYOffset());
                break;
            }

            // Go through each point and calculate the hypotenuse distance.
            for (int j = 0; j < cornerPts.length; j++) {
                // The adjacent length for angle calculations.
                double adjLen = 1.0;
                switch (p) {
                case XZ:
                case YZ:
                    // from the robot to the wall, or ZOffset.
                    adjLen = robot.getZOffset();
                    break;

                case XY:
                    // Height of the point above the robot
                    adjLen = Math.abs(cornerPts[j].getY() - robot.getYOffset());
                    break;
                }

                // Next, calculate the triangle's opposite leg's length.
                double oppLen = 1.0;
                switch (p) {
                case XZ:
                case XY:
                    // From the top and robot, the length is the
                    // difference between the robot and the backboard
                    // (delta X)
                    oppLen = Math.abs(robot.getXOffset() - cornerPts[j].getX());
                    break;
                case YZ:
                    // From the side, the length is the difference between
                    // the robot's height and the backboard (delta Y)
                    oppLen = Math.abs(robot.getYOffset() - cornerPts[j].getY());
                    break;
                }

                // Calculate the angle
                angles[i][j] = Math.toDegrees(Math.atan(oppLen / adjLen));

                // Determine if the angle is positive or negative based on
                // the perspective.
                switch (p) {
                case XZ:
                case XY:
                    // if the corner point is left of the robot, the
                    // angle is negative.
                    if (cornerPts[j].getX() < robotPt.getX())
                        angles[i][j] *= -1.0;
                    // the robot and the backboard (delta X)
                    oppLen = Math.abs(robot.getXOffset() - cornerPts[i].getX());
                    break;

                case YZ:
                    // if the corner point is above of the robot, the
                    // angle is negative.  In Java y pixel values
                    // increase by moving down the screen, and for our
                    // perspective, increasing y is higher, so use a
                    // negative angle.
                    if (cornerPts[j].getY() > robotPt.getY())
                        angles[i][j] *= -1.0;
                    break;
                }
            }
        }
        return angles;
    }

    private double[][] correctAngles(int angle, Point2D centerPt,
                                     Point2D pts[], double angles[][]) {
        double corrected[][] = new double[angles.length][angles[0].length];

        for (int i = 0; i < angles.length; i++) {
            for (int j = 0; j < angles[i].length; j++) {
                corrected[i][j] = angles[i][j];

                // We only correct in the (XZ plane) perspective
                if (i != Plane.XZ.ordinal())
                    continue;

                if (centerPt.getX() < pts[i].getX())
                    corrected[i][j] += angle;
                else
                    corrected[i][j] -= angle;
            }
        }
        return corrected;
    }

    private Polygon createPoly(double horizPixelScale, double vertPixelScale,
                               double[][] angles) {
        Polygon poly = new Polygon();

        // Iterate through the points!
        for (int j = 0; j < angles[0].length; j++) {
            int x = 0;
            int y = 0;
            // Next, grab the angle/perspectives for this point.
            for (int i = 0; i < angles.length; i++) {
                switch (intToPlane(i)) {
                case XZ:
                    x = (int)(angles[i][j] * horizPixelScale);
                    break;

                case YZ:
                    y = (int)(angles[i][j] * vertPixelScale);
                    break;

                case XY:
                    // XXX - Unused for now
                    break;
                }
            }
            poly.addPoint(x, y);
        }
        return poly;
    }
}
