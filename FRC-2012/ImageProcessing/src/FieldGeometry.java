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

    public static double cameraHeight = 37.0;   // inches (an estimate for now)
    public static double cameraTilt = 11.0 * Math.PI/180.0;     // estimated from data set of March 2; we need to nail this down
    public static final double cameraHorizontalView = 54.0 * Math.PI/180.0;
    public static final double cameraVerticalView = 40.5 * Math.PI/180.0;
    public static int cameraXixels = 320;
    public static int cameraYpixels = 240;


    static double estimateRange(Region hoop) {
	// At this point we can only estimate range at known hoop locations
	if ((hoop == null) || (hoop.hoopLocation == Region.HoopLocation.unknown))
	    return -1.0;
	
	double h = 0.0, mid = 0.0, top = 0.0, bot = 0.0;

	switch (hoop.hoopLocation) {
	case left:
	    top = leftHoop.y - cameraHeight;     // height of left hoop top relative to camera
	    bot = top - leftHoop.height;         // height of left hoop bottom relative to camera
	    break;
	case right:
	    top = rightHoop.y - cameraHeight;   // etc.
	    bot = top - rightHoop.height;    
	    break;
	case top:
	    top = topHoop.y - cameraHeight;  
	    bot = top - topHoop.height;     
	    break;
	case bottom:
	    top = bottomHoop.y - cameraHeight;
	    bot = top - bottomHoop.height;    
	    break;
	}


	// Select the hoop edge (top vs. bottom) based first on which is visible, and 
	// 2nd, to maximize range estimation accuracy,  on which sits at the greatest
	// distance from the camera height

	if ((hoop.topEdge != null) && (hoop.bottomEdge != null)) {
	    if (Math.abs(top) > Math.abs(bot)) {
		mid = (hoop.topEdge.y1 + hoop.topEdge.y2) / 2.0;
		h = top; 
	    } else {
		mid = (hoop.bottomEdge.y1 + hoop.bottomEdge.y2) / 2.0;
		h = bot;
	    }
	} else if ((hoop.topEdge != null) && (hoop.bottomEdge == null)) {
	    mid = (hoop.topEdge.y1 + hoop.topEdge.y2) / 2.0;
	    h = top; 
	}
	else if ((hoop.topEdge == null) && (hoop.bottomEdge != null)) {
		mid = (hoop.bottomEdge.y1 + hoop.bottomEdge.y2) / 2.0;
		h = bot;
	}
	else return -1.0;     // no visible edges -- punt.

    
	// angle between center of camera view and hoop top.  NB we take the
	// midppoint of the top line in the enclosing polygon

	double theta = (cameraYpixels/2 - mid) * cameraVerticalView / cameraYpixels;
	
	// debug
	//	System.out.println("estimateRange: " + hoop.hoopLocation + " mid = " + (int)mid + ", h = " + (int)h + ", theta = " + theta);

	return (Math.abs(h / Math.sin(theta + cameraTilt)));  // this might even be correct.
    }


    static double estimateRange(RegionGrow RG) {
	Region hoop = RG.getHighestVisibleHoop();
	if (hoop == null)
	    return -1.0;       // no known hopes, no estimate
	else return (estimateRange(hoop));
    }
}