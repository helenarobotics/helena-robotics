import java.awt.geom.*;
import java.awt.Rectangle;
import java.awt.Polygon;

//     public enum HoopLocation { unknown, left, top, right, bottom };

public class FieldGeometry {
    // rectangles beginning with top left corner of each hoop, moving clockwise.
    public static Rectangle2D.Double leftHoop = new Rectangle2D.Double(-39.375, 81.0, 24.0, 18.0);
    public static Rectangle2D.Double rightHoop = new Rectangle2D.Double(39.375, 81.0, 24.0, 18.0);
    public static Rectangle2D.Double topHoop = new Rectangle2D.Double(-12.0, 118.0, 24.0, 18.0);
    public static Rectangle2D.Double bottomHoop = new Rectangle2D.Double(-12.0, 48.0, 24.0, 18.0);

    public static double cameraHeight = 36.0;   // inches (an estimate for now)
    public static double cameraTilt = 11.0 * Math.PI/180.0;     // estimated from data set of March 2; we need to nail this down
    public static final double cameraHorizontalView = 54.0 * Math.PI/180.0;
    public static final double cameraVerticalView = 40.5 * Math.PI/180.0;
    public static int cameraXixels = 320;
    public static int cameraYpixels = 240;


    static double estimateRange(Region hoop) {
	if (hoop == null) 
	    return -1.0;
	
	Polygon p = hoop.getEnclosingPolygon();
	if (p == null)
	    return -1.0;

	double h = 0.0, mid = 0.0;

	switch (hoop.hoopLocation) {
	case left:
	    h = leftHoop.y - cameraHeight;     // height of this hoop above camera
	    mid = (p.ypoints[0] + p.ypoints[1])/2.0;
	    break;
	case right:
	    h = rightHoop.y - cameraHeight;    // height of this hoop above camera
	    mid = (p.ypoints[0] + p.ypoints[1])/2.0;
	    break;
	case top:
	    h = topHoop.y - cameraHeight;      // height of this hoop above camera
	    mid = (p.ypoints[0] + p.ypoints[1])/2.0;
	    break;
	case bottom:
	    double mid1 = (p.ypoints[0] + p.ypoints[1])/2.0;
	    double mid2 = (p.ypoints[2] + p.ypoints[3])/2.0;
	    if (Math.abs(cameraYpixels/2 - mid1) > Math.abs(cameraYpixels/2 - mid2)) {
		mid = mid1;
		h = bottomHoop.y - cameraHeight;
	    }
	    else {
		mid = mid2;
		h = bottomHoop.y - bottomHoop.height - cameraHeight;
	    }
	    break;
	}

	// angle between center of camera view and hoop top.  NB we take the
	// midppoint of the top line in the enclosing polygon
	double theta = (cameraYpixels/2 - mid) * cameraVerticalView / cameraYpixels;
	return (Math.abs(h / Math.sin(theta + cameraTilt)));  // this might even be correct.
    }


    static double estimateRange(RegionGrow RG) {
	Region hoop = RG.getHighestVisibleHoop();
	if (hoop == null)
	    return -1.0;       // no known hopes, no estimate
	else return (estimateRange(hoop));
    }
}