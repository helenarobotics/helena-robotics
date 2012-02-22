package view;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Polygon;

import javax.vecmath.Point3d;

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

    // The offset of this hoop in inches from the 'center' of the bottom
    // of the backboard.  xOffset is the left/right offset from the
    // center of the backboard, yOffset is the height from the floor,
    // and zOffset is the distance from the backboard, which by
    // definition is zero.
    private Point3d centerPt;

    // Backboard points
    private Point3d bbPts[];

    // Outside reflective tape points
    private Point3d ortPts[];

    // Inside reflective tape points
    private Point3d irtPts[];

    DistortHoop(double xOffset, double yOffset) {
        // We place the offsets at the center of the hoop's
        // backboard. Since the hoop is on the bottom of the backboard,
        // increase the zOffset by half the height of the backboard.
        yOffset += backboardOutsideHeight / 2;

        // The hoops are on the back wall, which by convention is at
        // location 0.
        centerPt = new Point3d(xOffset, yOffset, 0);

        // We have three rectangular 'objects' here, Backboard, outside
        // reflective tape, and inside reflective tape.  For each of
        // these objects, we calculate all four corners.
        // - UpperLeft
        // - UpperRight
        // - LowerRight
        // - LowerLeft

        // Clear Backboard
        bbPts = cornerPoints(backboardOutsideWidth, backboardOutsideHeight);

        // Outside Reflective Tape border
        ortPts = cornerPoints(tapeOutsideWidth, tapeOutsideHeight);

        // Inside Reflective Tape border
        irtPts = cornerPoints(tapeInsideWidth, tapeInsideHeight);
    }

    void paint(Graphics g, Dimension screenDimension,
               Robot robot, Camera camera) {
        // Create a new origin point where the robot/camera is located.
        final Point3d robotPt = new Point3d(0, 0, 0);

        // Calculate the object's points from the perspective of the
        // camera on the robot being at the origin.
        Point3d bbRobotPts[] = robotPoints(robot, bbPts);
        Point3d ortRobotPts[] = robotPoints(robot, ortPts);
        Point3d irtRobotPts[] = robotPoints(robot, irtPts);

        // Calculate the angles from the robot to the four corners in
        // both the horizontal and vertical directions.
        double bbAngles[][] = calcAngles(robotPt, bbRobotPts);
        double ortAngles[][] = calcAngles(robotPt, ortRobotPts);
        double irtAngles[][] = calcAngles(robotPt, irtRobotPts);

        // Go through each of the angles and compensate for the robot's
        // rotation.
        bbAngles = correctAngles(robot.getRotation(), bbAngles);
        ortAngles = correctAngles(robot.getRotation(), ortAngles);
        irtAngles = correctAngles(robot.getRotation(), irtAngles);

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
    private Point3d[] cornerPoints(double w, double h) {
        Point3d pts[] = new Point3d[REC_PTS.values().length];

        // Because z is always 0, we only store X/Z values.
        pts[REC_PTS.UPPER_LEFT.ordinal()] = new Point3d(
            centerPt.x - w / 2, centerPt.y + h / 2, centerPt.z);
        pts[REC_PTS.UPPER_RIGHT.ordinal()] = new Point3d(
            centerPt.x + w / 2, centerPt.y + h / 2, centerPt.z);
        pts[REC_PTS.LOWER_RIGHT.ordinal()] = new Point3d(
            centerPt.x + w / 2, centerPt.y - h / 2, centerPt.z);
        pts[REC_PTS.LOWER_LEFT.ordinal()] = new Point3d(
            centerPt.x - w / 2, centerPt.y - h / 2, centerPt.z);

        return pts;
    }

    // Return a 3d point created from the perspective of the robot.
    private Point3d[] robotPoints(Robot robot, Point3d cornerPts[]) {
        Point3d pts[] = new Point3d[cornerPts.length];

        // Simple math.  The point is the difference between the robot's
        // position and the position of the corner point in all 3
        // dimensions.
        for (int i = 0; i < pts.length; i++) {
            double x = cornerPts[i].x - robot.getXOffset();
            double y = cornerPts[i].y - robot.getYOffset();
            double z = cornerPts[i].z - robot.getZOffset();
            pts[i] = new Point3d(x, y, z);
        }
        return pts;
    }

    private enum ANGLE { HORIZ, VERT };

    // Return a 2D array containing horizontal/vertical angles to the
    // the upper-left, upper-right, lower-left, and lower-right points
    // of the triangle from the robot's perspective.
    private double[][] calcAngles(Point3d robotPt, Point3d cornerPts[]) {
        // Calculate the angles
        double angles[][] =
            new double[cornerPts.length][ANGLE.values().length];

        // Calculate the angles for each point
        for (int i = 0; i < cornerPts.length; i++) {
            // First, calculate the horizontal angle to this point.  This
            // is just the atan of the X and Z points of the
            // cornerPoints.
            angles[i][ANGLE.HORIZ.ordinal()] =
                Math.toDegrees(Math.atan(cornerPts[i].x / -cornerPts[i].z));

            // Calculate the horizontal angle to the point, but first
            // calculating the length of the adjacent side of the right
            // triangle which which exists in the XZ plane.  The length
            // of this is the length of the hypotenuse of the triangle
            // in the same XZ plane, which are known coordinates.
            double adjLen = Math.sqrt(Math.pow(cornerPts[i].x, 2) +
                                      Math.pow(cornerPts[i].z, 2));
            // The length of the opposite triangle is the height, or y
            // value.
            angles[i][ANGLE.VERT.ordinal()] =
                Math.toDegrees(Math.atan(cornerPts[i].y / adjLen));
        }
        return angles;
    }

    // Note, This is only correct if the camera is parallel with the X/Z
    // axis, otherwise the rotation of the robot will also affect the
    // HORIZONTAL angle slightly due to the change in the angle based on
    // Z/Y offsets.
    private double[][] correctAngles(int angle, double angles[][]) {
        double corrected[][] = new double[angles.length][angles[0].length];
        for (int i = 0; i < angles.length; i++) {
            // Make sure we copy the current angle.
            corrected[i][ANGLE.HORIZ.ordinal()] =
                angles[i][ANGLE.HORIZ.ordinal()];
            corrected[i][ANGLE.VERT.ordinal()] =
                angles[i][ANGLE.VERT.ordinal()];

            // Apply rotation correction
            corrected[i][ANGLE.HORIZ.ordinal()] -= angle;
        }
        return corrected;
    }

    private Polygon createPoly(double horizPixelScale, double vertPixelScale,
                               double[][] angles) {
        Polygon poly = new Polygon();

        // Iterate through the points!
        for (int i = 0; i < angles.length; i++) {
            int x = (int)(angles[i][ANGLE.HORIZ.ordinal()] * horizPixelScale);
            int y = -(int)(angles[i][ANGLE.VERT.ordinal()] * vertPixelScale);
            poly.addPoint(x, y);
        }
        return poly;
    }
}
