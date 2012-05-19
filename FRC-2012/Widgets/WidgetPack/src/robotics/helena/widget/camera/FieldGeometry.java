package robotics.helena.widget.camera;

import java.awt.geom.Rectangle2D;

public class FieldGeometry {
    // Known locations of the outside corners of the four hoops:
    public static Rectangle2D.Double leftHoop =
        new Rectangle2D.Double(-39.375, 81.0, 24.0, 18.0);
    public static Rectangle2D.Double rightHoop =
        new Rectangle2D.Double(39.375, 81.0, 24.0, 18.0);
    public static Rectangle2D.Double topHoop =
        new Rectangle2D.Double(-12.0, 118.0, 24.0, 18.0);
    public static Rectangle2D.Double bottomHoop =
        new Rectangle2D.Double(-12.0, 48.0, 24.0, 18.0);

    // leftTop, rightTop, leftBottom, rightBottom;
    static void estimateRange(Region hoop) {
        double top = 0.0, bot = 0.0, left = 0.0, right = 0.0;
	top = topHoop.y;
	bot = top - topHoop.height;
	left = topHoop.x;
	right = topHoop.x + topHoop.width;

        // Estimate range to 4 corners, as possible
        if (hoop.leftTop != null) {
            HoopEstimate hr = new HoopEstimate(
                hoop.leftTop.x, hoop.leftTop.y, left, top,
                hoop.xPixels, hoop.yPixels);
            // Save for later.  We'll combine the various range
            // estimates to calc robot position etc.
            hoop.estimates.add(hr);
        }

        if (hoop.rightTop != null) {
            HoopEstimate hr = new HoopEstimate(
                hoop.rightTop.x, hoop.rightTop.y, right, top,
                hoop.xPixels, hoop.yPixels);
            // Save for later
            hoop.estimates.add(hr);
        }

        if (hoop.leftBottom != null) {
            HoopEstimate hr = new HoopEstimate(
                hoop.leftBottom.x, hoop.leftBottom.y, left, bot,
                hoop.xPixels, hoop.yPixels);
            // Save for later
            hoop.estimates.add(hr);
        }

        if (hoop.rightBottom != null) {
            HoopEstimate hr = new HoopEstimate(
                hoop.rightBottom.x, hoop.rightBottom.y, right, bot,
                hoop.xPixels, hoop.yPixels);
            // Save for later
            hoop.estimates.add(hr);
        }

        // Calculate range to center of hoop (approx)
        if (hoop.estimates.size() == 0) {
            hoop.range = -1.0;  // this should never happen
        } else {
            double rangeSum = 0.0;
            double azimuthSum = 0.0;
            for (HoopEstimate he: hoop.estimates) {
                rangeSum += he.range;
                azimuthSum += he.azimuth;
            }
            hoop.range = rangeSum / hoop.estimates.size();
            hoop.azimuth = azimuthSum / hoop.estimates.size();
        }
    }
}
