import java.awt.geom.*;
import java.awt.Rectangle;
import java.awt.Polygon;
import javax.vecmath.Point3d;

//     public enum HoopLocation { unknown, left, top, right };

public class FieldGeometry {
    // Known locations of the outside corners of the four hoops:
    // XXX These are just estimates on the 'x' positions - I can't find that documented on FRC site.
    // NOTE THAT "topHoop" refers to middle hoop in 2013.
    public static Rectangle2D.Double leftHoop = new Rectangle2D.Double(-96.0, 88.625+21.0, 54.0, 21.0);
    public static Rectangle2D.Double topHoop = new Rectangle2D.Double(-27.0, 104.125+12.0, 54.0, 12.0);
    public static Rectangle2D.Double rightHoop = new Rectangle2D.Double(42.0, 88.625+21.0, 54.0, 21.0);

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
	}

	// Estimate range to 4 corners, as possible

	if (hoop.leftTop != null) {
	    HoopEstimate hr = new HoopEstimate(hoop.leftTop.x, hoop.leftTop.y, left, top, 
					       hoop.xPixels, hoop.yPixels);
	    hoop.estimates.add(hr);                            // Save for later.  We'll combine the various range estimates to calc robot position etc.
	}

	if (hoop.rightTop != null) {
	    HoopEstimate hr = new HoopEstimate(hoop.rightTop.x, hoop.rightTop.y, right, top,
					       hoop.xPixels, hoop.yPixels);
	    hoop.estimates.add(hr);                            // Save for later
	}

	if (hoop.leftBottom != null) {
	    HoopEstimate hr = new HoopEstimate(hoop.leftBottom.x, hoop.leftBottom.y, left, bot,
					       hoop.xPixels, hoop.yPixels);
	    hoop.estimates.add(hr);                            // Save for later
	}

	if (hoop.rightBottom != null) {
	    HoopEstimate hr = new HoopEstimate(hoop.rightBottom.x, hoop.rightBottom.y, right, bot,
					       hoop.xPixels, hoop.yPixels);
	    hoop.estimates.add(hr);                            // Save for later
	}

	// Calculate range to center of hoop (approx)

	if (hoop.estimates.size() == 0) {
	    hoop.range = -1.0;  // this should never happen
	}
	else {
	    double sum = 0.0;
	    for (int i = 0; i < hoop.estimates.size(); i++) {
		HoopEstimate he = hoop.estimates.elementAt(i);
		sum += he.range;
	    }
	    hoop.range = sum / hoop.estimates.size();
	}
    }
}
