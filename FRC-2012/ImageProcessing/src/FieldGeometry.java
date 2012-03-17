import java.awt.geom.*;
import java.awt.Rectangle;
import java.awt.Polygon;
import javax.vecmath.Point3d;

//     public enum HoopLocation { unknown, left, top, right, bottom };

public class FieldGeometry {
    // rectangles beginning with top left corner of each hoop, moving clockwise.
    public static Rectangle2D.Double leftHoop = new Rectangle2D.Double(-39.375, 81.0, 24.0, 18.0);
    public static Rectangle2D.Double rightHoop = new Rectangle2D.Double(39.375, 81.0, 24.0, 18.0);
    public static Rectangle2D.Double topHoop = new Rectangle2D.Double(-12.0, 118.0, 24.0, 18.0);
    public static Rectangle2D.Double bottomHoop = new Rectangle2D.Double(-12.0, 48.0, 24.0, 18.0);

    public static double cameraHeight = 37.0;   // inches (an estimate for now)
    public static double cameraTilt = 11.0 * Math.PI/180.0;     // estimated from data set of March 2; we need to nail this down
    public static final double cameraHorizontalView = 54.0 * Math.PI/180.0;
    public static final double cameraVerticalView = 40.5 * Math.PI/180.0;
    public static int cameraXpixels = 320;
    public static int cameraYpixels = 240;

    // leftTop, rightTop, leftBottom, rightBottom;
    static void  estimateRange(Region hoop) {
	
	double top = 0.0, bot = 0.0, left = 0.0, right = 0.0;

	Line2D.Double edge = null;

	switch (hoop.hoopLocation) {
	case unknown:
	    return;
	case left:
	    top = leftHoop.y;
	    bot = top - leftHoop.height;
	    left = leftHoop.x;
	    right = leftHoop.x + leftHoop.width;
	    break;
	case right:
	    top = rightHoop.y;
	    bot = top - rightHoop.height;    
	    left = rightHoop.x;
	    right = rightHoop.x + rightHoop.width;
	    break;
	case top:
	    top = topHoop.y;
	    bot = top - topHoop.height;
	    left = topHoop.x;
	    right = topHoop.x + topHoop.width;
	    break;
	case bottom:
	    top = bottomHoop.y;
	    bot = top - bottomHoop.height;    
	    left = bottomHoop.x;
	    right = bottomHoop.x + bottomHoop.width;
     
	    break;
	}

	// Estimate range to 4 corners, as possible

	double x, y, h;

	if (hoop.leftTop != null) {
	    h = top  - cameraHeight;
	    y = hoop.leftTop.y;
	    x = hoop.leftTop.x;
	    HoopEstimate hr = new HoopEstimate();
	    hr.elevation = (cameraYpixels/2 - y) * cameraVerticalView / cameraYpixels + cameraTilt;
	    hr.azimuth = x * cameraHorizontalView / cameraXpixels;
	    hr.rangePoint = new Point3d(left, 0.0, top);      // We need to confirm consistent use of 3D axis: x, y, z
	    hr.range = Math.abs(h / Math.sin(hr.elevation));   // this might even be correct.
	    hr.error = Math.abs(h * Math.cos(hr.elevation) * cameraVerticalView / cameraYpixels / (Math.sin(hr.elevation) * Math.sin(hr.elevation)));
	    hoop.estimates.add(hr);                            // Save for later
	}

	if (hoop.rightTop != null) {
	    y = hoop.rightTop.y;
	    x = hoop.rightTop.x;
	    h = top - cameraHeight;
	    HoopEstimate hr = new HoopEstimate();
	    hr.elevation = (cameraYpixels/2 - y) * cameraVerticalView / cameraYpixels + cameraTilt;
	    hr.azimuth = x * cameraHorizontalView / cameraXpixels;
	    hr.rangePoint = new Point3d(right, 0.0, top);      // We need to confirm consistent use of 3D axis: x, y, z
	    hr.range = Math.abs(h / Math.sin(hr.elevation));   // this might even be correct.
	    hr.error = Math.abs(h * Math.cos(hr.elevation) * cameraVerticalView / cameraYpixels / (Math.sin(hr.elevation) * Math.sin(hr.elevation)));
	    hoop.estimates.add(hr);                            // Save for later
	}
	// Now estimate range to bottom edge, if possible
	if (hoop.leftBottom != null) {
	    y = hoop.leftBottom.y;
	    x = hoop.leftBottom.x;
	    h = bot - cameraHeight;
	    HoopEstimate hr = new HoopEstimate();
	    hr.elevation = (cameraYpixels/2 - y) * cameraVerticalView / cameraYpixels + cameraTilt;
	    hr.azimuth = x * cameraHorizontalView / cameraXpixels;
	    hr.rangePoint = new Point3d(left, 0.0, bot);      // We need to confirm consistent use of 3D axis: x, y, z
	    hr.range = Math.abs(h / Math.sin(hr.elevation));   // this might even be correct.
	    hr.error = Math.abs(h * Math.cos(hr.elevation) * cameraVerticalView / cameraYpixels / (Math.sin(hr.elevation) * Math.sin(hr.elevation)));
	    hoop.estimates.add(hr);                            // Save for later
	}
	// Now estimate range to bottom right corner, if possible
	if (hoop.rightBottom != null) {
	    y = hoop.rightBottom.y;
	    x = hoop.rightBottom.x;
	    h = bot - cameraHeight;
	    HoopEstimate hr = new HoopEstimate();
	    hr.elevation = (cameraYpixels/2 - y) * cameraVerticalView / cameraYpixels + cameraTilt;
	    hr.azimuth = x * cameraHorizontalView / cameraXpixels;
	    hr.rangePoint = new Point3d(right, 0.0, bot);      // We need to confirm consistent use of 3D axis: x, y, z
	    hr.range = Math.abs(h / Math.sin(hr.elevation));   // this might even be correct.
	    hr.error = Math.abs(h * Math.cos(hr.elevation) * cameraVerticalView / cameraYpixels / (Math.sin(hr.elevation) * Math.sin(hr.elevation)));
	    hoop.estimates.add(hr);                            // Save for later
	}
    }
}
