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
    // origin.  yOffset would be the distance from the backboard, but
    // since we're the backboar, we only have an X (left and right of
    // center) and Z (height from the floor) offset.
    private final double xOffset;
    private final double zOffset;

    // Backboard points
    private Point2D bbPts[];

    // Outside reflective tape points
    private Point2D ortPts[];

    // Inside reflective tape points
    private Point2D irtPts[];
    
    // Degree (angle) to pixel calculations.
    private final double degreeHorizPixels;
    private final double degreeVertPixels;

    DistortHoop(double _xOffset, double _zOffset) {
        xOffset = _xOffset;

        // We place the offsets at the center of the hoop, so since
        // the hoop is on the bottom of the backboard, increase the
        // zOffset by half the height of the backboard.
        zOffset = _zOffset + backboardOutsideHeight / 2;

        // We have three rectangular 'objects' here.
        // Backboard, Outside tape, and inside tape.  For each of the
        // objects, we'll calculate all four corners.
        // - UpperLeft
        // - UpperRight
        // - LowerLeft
        // - LowerRight

        // Backboard
        bbPts = cornerPoints(
            xOffset, zOffset, backboardOutsideWidth, backboardOutsideHeight);
        
        // Outside Reflective Tape border
        ortPts = cornerPoints(
            xOffset, zOffset, tapeOutsideWidth, tapeOutsideHeight);

        // Inside Reflective Tape border
        irtPts = cornerPoints(
            xOffset, zOffset, tapeInsideWidth, tapeInsideHeight);

        // Finally, calculate the degree to pixel value.  The camera has
        // a 54 degree Field Of View (FOV), and generates a 640x480
        // picture.  We calculate the pixel to degree value here.
        degreeHorizPixels = 640.0 / 54.0;
        degreeVertPixels = 480.0 / 40.5;
    }

    // XXX - We don't yet take into account the height distortion that
    // occurs from the different distances.
    void paint(Graphics g, Dimension screenDimension,
               Robot robot, Camera camera) {
        // Calculate the horizontal angle from the robot to the four
        // corners.
        double bbHorizAngles[] = calcAngles(robot, bbPts, true);
        double ortHorizAngles[] = calcAngles(robot, ortPts, true);
        double irtHorizAngles[] = calcAngles(robot, irtPts, true);

        // Calculate the vertical angles from the robot to the four
        // corners.
        double bbVertAngles[] = calcAngles(robot, bbPts, false);
        double ortVertAngles[] = calcAngles(robot, ortPts, false);
        double irtVertAngles[] = calcAngles(robot, irtPts, false);

        // Determine where the robot's camera is pointing along the
        // backboard, which will help determine where we draw the pixels
        // (left or right of the camera's center point).
        //
        // The robot rotations only changes the XOffset, we can easily
        // determine this location since we know one leg of a right
        // triangle (YOffset), and the angle (rotation), which can give
        // us the additional XOffset.
        double rotXOffset = robot.getYOffset() *
                            Math.cos(Math.toRadians(robot.getRotation()));

        // The back wall is by convention at Y location 0.
        Point2D cameraCenter =
            new Point2D.Double(robot.getXOffset() + rotXOffset, 0);

        // Go through each of the angles to see if we subtract or add to
        // them based on whether or not the point is left/right to the
        // cameraCenter.
        bbHorizAngles = correctHorizAngles(robot.getRotation(), cameraCenter,
                                           bbPts, bbHorizAngles);
        ortHorizAngles = correctHorizAngles(robot.getRotation(), cameraCenter,
                                            ortPts, ortHorizAngles);
        irtHorizAngles = correctHorizAngles(robot.getRotation(), cameraCenter,
                                            irtPts, irtHorizAngles);

        // At this point, we can map the angles to the pixel lengths, but
        // the pixels we display on the screen may be smaller/larger
        // than the camera view, so scale the visual screen to what is
        // being shown.
        double scaledHorizPixels =
            degreeHorizPixels * screenDimension.getWidth() / 640.0;
        double scaledVertPixels =
            degreeVertPixels * screenDimension.getHeight() / 480.0;

        // Create the 3 Polygons that make up the 4 points of the
        // 'rectangle'.
        Polygon bbPoly = createPoly(scaledHorizPixels, bbHorizAngles,
                                    scaledVertPixels, bbVertAngles);
        Polygon ortPoly = createPoly(scaledHorizPixels, ortHorizAngles,
                                     scaledVertPixels, ortVertAngles);
        Polygon irtPoly = createPoly(scaledHorizPixels, irtHorizAngles,
                                     scaledVertPixels, irtVertAngles);

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

    // Return a array containing the upper-left, upper-right,
    // lower-left, and lower-right points of the 'rectangle'.
    private double[] calcAngles(Robot robot, Point2D cornerPts[],
                                boolean isHorizontalAngle) {
        // We calculate the distance from the robot to the corner point
        // to give us the hypotenuse of the right triangle.
        double angles[] = new double[cornerPts.length];

        // The perspective needs to change depending on whether or not
        // we're measuing horizontal or vertical angles.  When
        // calculating horizontal values, we're looking at the field
        // from above, so we need both the standard X and Y offsets in
        // the field.  However, when we are calculating the vertical
        // angle, we are looking at the field from the side, so we need
        // the Y offset (translated into X value), and height of the
        // robot, which is our Y offset.
        Point2D robotPt;
        if (isHorizontalAngle)
            robotPt = new Point2D.Double(robot.getXOffset(),
                                         robot.getYOffset());
        else
            robotPt = new Point2D.Double(robot.getYOffset(),
                                         robot.getHeight());

        // The adjacent angle for both calculations includes the length
        // from the robot to the wall, or YOffset.
        double adjLen = robot.getYOffset();

        // Go through each point and calculate the hypotenuse distance.
        for (int i = 0; i < cornerPts.length; i++) {
            // Calculate the distance from the robot to the corner point
            // to give us the hypotenuse of the right triangle.
            Point2D translatedPt;
            if (isHorizontalAngle) {
                // From above, the corner point's Y offset is always
                // zero, since it's at the backboard.
                translatedPt = new Point2D.Double(cornerPts[i].getX(), 0);
            } else {
                // From the side, the X offset is always zero since it's
                // at the backboard.
                translatedPt = new Point2D.Double(0, cornerPts[i].getY());
            }

            // Calculate the distance (hypotenuse), and then generate
            // the angle.
            double hypLen = translatedPt.distance(robotPt);
            angles[i] = Math.toDegrees(Math.acos(adjLen / hypLen));

            // Determine if the angle is positive or negative based on
            // if the corner point is left/right or above/below the
            // robot.
            if (isHorizontalAngle && cornerPts[i].getX() < robotPt.getX())
                angles[i] *= -1;
            else if (!isHorizontalAngle && cornerPts[i].getY() > robotPt.getY())
                // Positive Y == down in Java, so we reverse it!
                angles[i] *= -1;
        }
        return angles;
    }

    private double[] correctHorizAngles(int angle, Point2D centerPt,
                                        Point2D pts[], double angles[]) {
        double corrected[] = new double[angles.length];

        for (int i = 0; i < angles.length; i++) {
            corrected[i] = angles[i];
            if (centerPt.getX() < pts[i].getX())
                corrected[i] += angle;
            else
                corrected[i] -= angle;
        }
        return corrected;
    }

    private Polygon createPoly(double horizPixelScale, double horizAngles[],
                               double vertPixelScale, double vertAngles[]) {
        Polygon poly = new Polygon();

        for (int i = 0; i < horizAngles.length; i++) {
            // Calculate the point!
            int x = (int)(horizAngles[i] * horizPixelScale);
            int y = (int)(vertAngles[i] * vertPixelScale);
            poly.addPoint(x, y);
        }
        return poly;
    }
}
