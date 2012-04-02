import java.util.Vector;
import java.awt.Rectangle;
import java.awt.image.BufferedImage;
import java.awt.geom.*;
import java.awt.Polygon;
import java.awt.image.ColorModel;
import java.awt.image.WritableRaster;
import java.awt.image.Raster;
import java.awt.Graphics;
import java.awt.Graphics2D;
import javax.imageio.ImageIO;
import javax.vecmath.Point3d;
import java.io.File;
import java.io.IOException;
import java.awt.*;


public class Region {

    public enum HoopLocation { unknown, left, top, right, bottom };

    Vector<Point> points;      // temporary space used while building the region.
    public HoopLocation hoopLocation;
    public Rectangle enclosingRectangle;
    double range;                 // range = dist from camera to a 3D point at center of the backboard (above the rim)
    Vector<HoopEstimate> estimates;
    public Line2D.Double leftEdge;
    public Line2D.Double rightEdge;
    public Line2D.Double topEdge;
    public Line2D.Double bottomEdge;
    Point2D.Double leftTop, rightTop, leftBottom, rightBottom;
    int xPixels, yPixels;   // size of processed imaged (note that it may be downsampled from original)

    public Region() {
	points = new Vector<Point> (2000);
	hoopLocation = HoopLocation.unknown;
	enclosingRectangle = null;
	leftEdge = rightEdge = topEdge = bottomEdge = null;
	leftTop = rightTop = leftBottom = rightBottom = null;
	estimates = new Vector<HoopEstimate>(4);
	xPixels = yPixels = 0;
    }


    public void finish(BufferedImage image) {
	calculateEnclosingRectangle();
	calculateHoopEdges(image);
	xPixels = image.getWidth();
	yPixels = image.getHeight();
	points = null;      // free memory
    }

    public void add(Point p) {
	points.add(p);
    }

    public int size() {
	return points.size();
    }

    public Point elementAt(int index) {
	return points.elementAt(index);
    }

    public Rectangle getEnclosingRectangle() {
	if (enclosingRectangle == null)
	    calculateEnclosingRectangle();
	return enclosingRectangle;
    }

    private void calculateEnclosingRectangle() {
	int minX = 1000000, minY = 1000000, maxX = -1, maxY = -1;

	for (int i = 0; i < size(); i++) {
	    Point p = elementAt(i);
	    if (p.x < minX) minX = p.x;
	    if (p.x > maxX) maxX = p.x;
	    if (p.y < minY) minY = p.y;
	    if (p.y > maxY) maxY = p.y;
	}

	this.enclosingRectangle = new Rectangle(minX, minY, (maxX - minX), (maxY - minY));

	// debug
	//	System.out.println("calculateEnclosingRectangle: x = " + enclosingRectangle.x + ", y = " + enclosingRectangle.y + ", h = " + enclosingRectangle.height + ", w = " + enclosingRectangle.width);

    }


    // Returns area of bounding rectangle, measured in pixels^2.  
    // Note that we rely on 'get' function rather than accessing the class value
    // directly.  This accommodates possible 'lazy instantiation' of that value,
    // as enclosure calculations can be relatively expensive, and may not be called
    // unless necessary

    public double getArea() {
	Rectangle r = getEnclosingRectangle();
	return (r.width * r.height);
    }


    /* 
     * Method to calculate a tight enclosing polygon for each hoop.
     * Input: a thresholded grayscale image, ostensibly optimized to highlight pixels occupied with 
     *  FRC reflective tape when illuminated with LED right.  
     * Applies a two-pass least mean squares line fit: the first pass fits to all leading points, the 2nd
     *  after outlier removal; a qsort algorithm orders the points by resulting error, and a large fraction 
     *  (currently 1/2) of the data points - those with the lowest fit errors -- are used for the 2nd pass.
     *  This makes it much more robust to gaps, including those created at top, bottom, and sides from loose
     *  fitting of enclosing rectangle from which the algorithm begins.
     *
     *  Calcuates a set of four Point2D.Double's representing the four hoop corners, as visible (or extrapolated from 
     *   visible edges)
     * 
     *   Note that any pixel > 0 is assumed "on" -- a general grayscale scene would likely create disasterous results.
     * 
     */

    public void calculateHoopEdges(BufferedImage img) {

	Rectangle r = getEnclosingRectangle();
	dataPoint [] dp = new dataPoint [Math.max(r.width, r.height) + 1];
	double [] resultsRight = null, resultsLeft = null, resultsTop = null, resultsBottom = null;


	int wsearch = r.width / 4;   // search for edge w/in 25% (1/4) of enclosing box;
	int hsearch = r.height/ 4;   //   (this limit helps avoid outliers)
	double required = 0.5;       // fraction of edge cells that must be visible to consider valid

	// Calculate best line fit to left edge (using thresholded image data)
	int count = 0;
	for (int y = r.y; y < r.y + r.height; y++) {
	    for (int x = r.x; x < r.x + wsearch; x++) {
		if ((img.getRGB(x, y) & 0x000000ff) > 0) {
		    if (x > 0) {      // we only count edge cells that are off the image edge
			dp[count] = new dataPoint(y, x);
			count++;
		    }
		break;
		}
	    }
	}

	if (count >= r.height/2) {
	    resultsLeft = leastSquares(dp, count, 2*count/3);
	    //	    System.out.println("Left count = " + count + " (results calculated)");
	}
	//	else System.out.println("Left count = " + count);

	// Calculate best line fit to right edge (using thresholded image data)
	count = 0;
	for (int y = r.y; y < r.y + r.height; y++) {
	    for (int x = r.x + r.width; x >  r.x + (r.width - wsearch); x--) {
		if ((img.getRGB(x, y) & 0x000000ff) > 0) {
		    if (x < (img.getWidth() - 1)) {
			dp[count] = new dataPoint(y, x);
			count++;
		    }
		    break;
		}
	    }
	}

	if (count >= r.height/2) {
	    resultsRight = leastSquares(dp, count, 2*count/3);
	    //	    System.out.println("Right count = " + count + " (results calculated)");
	}
	//	else System.out.println("Right count = " + count);

	count = 0;
	for (int x = r.x; x < r.x + r.width; x++) {
	    for (int y = r.y; y < r.y + hsearch; y++) {
		if ((img.getRGB(x, y) & 0x000000ff) > 0) {
		    if (y > 0) {
			dp[count] = new dataPoint(x, y);
			count++;
		    }
		    break;
		}
	    }
	}
	if (count >= r.width/2) {
	    resultsTop = leastSquares(dp, count, 2*count/3);	    
	    //	    System.out.println("Top count = " + count + " (results calculated)");
	}
	//	else System.out.println("Top count = " + count);


	// Calculate best line fit to bottom edge (using thresholded image data)
	count = 0;
	for (int x = r.x; x < r.x + r.width; x++) {
	    int y;
	    for (y = r.y + r.height; y > r.y + (r.height - hsearch); y--) {
		if ((img.getRGB(x, y) & 0x000000ff) > 0) {
		    if (y < (img.getHeight() - 1)) {
			dp[count] = new dataPoint(x, y);
			count++;
		    }
		    break;
		}
	    }
	}
	if (count >= r.width/2) {
	    resultsBottom = leastSquares(dp, count, 2*count/3);
	    //	        System.out.println("Bottom count = " + count + " (results calculated)");
	}
	//       else 	System.out.println("Bottom: count = " + count);


	// Now find the points of intersection

	if ((resultsTop != null) && (resultsLeft != null)) {
	    leftTop = intersectingPoint(resultsTop[0], resultsTop[1], resultsLeft[0], resultsLeft[1]);
	    //	    System.out.println("leftTop");
	}
	else leftTop = null;

	if ((resultsTop != null) && (resultsRight != null)) {
	    rightTop = intersectingPoint(resultsTop[0], resultsTop[1], resultsRight[0], resultsRight[1]);
	    //	    System.out.println("rightTop");
	}
	else rightTop = null;

	if ((resultsBottom != null) && (resultsLeft != null)) {
	    leftBottom = intersectingPoint(resultsBottom[0], resultsBottom[1], resultsLeft[0], resultsLeft[1]);
	    //	    System.out.println("leftBottom");
	}
	else leftBottom = null;

	if ((resultsBottom != null) && (resultsRight != null)) {
	    rightBottom = intersectingPoint(resultsBottom[0], resultsBottom[1], resultsRight[0], resultsRight[1]);
	    //System.out.println("rightBottom");
	}
	else rightBottom = null;
	/*
	System.out.println("Top left corner at     " + (int)leftTop.x + ", " +  (int)leftTop.y);
	System.out.println("Top right corner at    " + (int)rightTop.x + ", " +  (int)rightTop.y);
	System.out.println("Bottom right corner at " + (int)rightBottom.x + ", " +  (int)rightBottom.y);
	System.out.println("Bottom left corner at " +  (int)leftBottom.x + ", " +  (int)leftBottom.y);
	*/
	// Save 4 lines that tightly bound the hoop, where possible.  We set edge pointers to null
	// where no line could be found (e.g., due to substantial occlusion or off camera FOV

	if ((leftBottom != null) && (rightBottom != null))
	    bottomEdge = new Line2D.Double(leftBottom, rightBottom);
	else bottomEdge = null;

	if ((rightTop != null) && (rightBottom != null))
	    rightEdge = new Line2D.Double(rightTop, rightBottom);
	else rightEdge = null;

	if ((leftBottom != null) && (rightTop != null))
	    leftEdge = new Line2D.Double(leftBottom, leftTop);
	else leftEdge = null;

	if ((leftTop != null) && (rightTop != null))
	    topEdge = new Line2D.Double(leftTop, rightTop);
	else topEdge = null;
    }


    // Expects m2, b2 to come from eqn x = m2 * y + b2;
    //  while m1 and b1 eqn y = m1 * x + b1

    static Point2D.Double intersectingPoint(double m1, double b1, double m2, double b2) {
	double y = (m1 * b2 + b1) / (1.0 - m1 * m2);
	double x = m2 * y + b2;
	//	System.out.println("Intersection at " + (int)x + ", " + y);
	return (new Point2D.Double(x, y));
    }

    /*    static Point2D.Double intersectingPoint(double m1, double b1, double m2, double b2) {
	double x = (b2 - b1) / (m1 - m2);   // x at point of intersection (if they intersect!)
	return (new Point2D.Double(x, x * m1 + b1));
    }
    */

    /* 
     * Two-pass least squares algorith.  First pass estimates a line, sorts the results by residual error (on point-by-point basis), 
     *  removes high-error points, and re-estimates.
     */

    private double [] leastSquares(dataPoint dp [], int n, int keep) {

	leastSquares(dp, n);        // first pass -- use all data
	quickSort(dp, 0, n-1);      // sort by residual error

	return (leastSquares(dp, keep));    // recalculate with 'keep' lowest error data points; return
    }


    // Simple least squares linear fit algorithm.  Returns a 2-long array of doubles, the first (slot [0]) of which 
    // represents m, 2nd b, in the familiar line equation y = mx + b

    private double [] leastSquares(dataPoint dp [], int n) {

	double sumx = 0.0, sumx2 = 0.0, sumy = 0;

	//	System.out.println("Data into leastSquares: (" + n + " points)");
	for (int i = 0; i < n; i++) {
	    sumx  += dp[i].x;
	    sumx2 += dp[i].x * dp[i].x;
	    sumy  += dp[i].y;
	    // debug
	    //  System.out.print("{" + (int)dp[i].x + "," + (int)dp[i].y + "}:" + (int)dp[i].err + " ");
	}
	//	System.out.println();

	double xbar = sumx / n;
	double ybar = sumy / n;

	// second pass: compute summary statistics
	double xxbar = 0.0, yybar = 0.0, xybar = 0.0;
	for (int i = 0; i < n; i++) {
	    xxbar += (dp[i].x - xbar) * (dp[i].x - xbar);
	    yybar += (dp[i].y - ybar) * (dp[i].y - ybar);
	    xybar += (dp[i].x - xbar) * (dp[i].y - ybar);
	}
	double beta1 = xybar / xxbar;
	double beta0 = ybar - beta1 * xbar;

	// Calculate and store the errors
	double err2 = 0;
	for (int i = 0; i < n; i++) {
	    double err = dp[i].y - beta1 * dp[i].x - beta0; 
	    dp[i].err = err * err;
	    err2 += dp[i].err;
	}

	// debug - print results
	// System.out.println("y   = " + beta1 + " * x + " + beta0 + " std err " + Math.sqrt(err2/(double)n));

	double result [] = new double [2];
	result[0] = beta1;
	result[1] = beta0;
	return (result);
    }
	

    private class dataPoint {
	double x, y, err;

	dataPoint(double _x, double _y, double _err) {
	    x = _x;
	    y = _y;
	    err = _err;
	}

	dataPoint(double _x, double _y) {
	    x = _x;
	    y = _y;
	    err = 0.0;
	}

	dataPoint() {
	    x = y = err = 0;
	}
    }
	

    //Quick Sort code for JAVA, by Yash Gupta
    // (Rebuilt by Arnold to sort class type dataPoint

private int partition(dataPoint arr[], int left, int right){
	int i = left, j = right;
	dataPoint tmp;
	double pivot = arr[(left + right) / 2].err;
	while (i <= j)
	    {
		while (arr[i].err < pivot)
		    i++;
		while (arr[j].err > pivot)
		    j--;
		if (i <= j)
		    {
			tmp = arr[i];
			arr[i] = arr[j];
			arr[j] = tmp;
			i++;
			j--;
		    }
	    };
	return i;
    }

    private void quickSort(dataPoint arr[], int left, int right)  {
	if (right <= left)        //  watch out for  case of zero-length vector.  If this happes, partition blows up
	    return;
	int index = partition(arr, left, right);
	if (left < index - 1)
            quickSort(arr, left, index - 1);
	if (index < right)
            quickSort(arr, index, right);
    }


    /*
     * Logic to support organizing of detected regions ("hoops") into hoop locations.
     */


    boolean sameElevation(Region box2) {
	return (!(isAbove(box2) || isBelow(box2)));
    }

    boolean sameAzimuth(Region box2) {
	return (!(isLeftOf(box2) || isRightOf(box2)));
    }

    // Note again that y dimension is flipped; larger y's mean lower on the image.

    boolean isAbove(Region box2) {
	Rectangle r1 = this.enclosingRectangle;
	Rectangle r2 = box2.enclosingRectangle;
	return ((r1.y + r1.height/2.0) < r2.y);
    }

    boolean isBelow(Region box2) {
	Rectangle r1 = this.enclosingRectangle;
	Rectangle r2 = box2.enclosingRectangle;
	return (r1.y > (r2.y + r2.height/2.0));
    }

    boolean isLeftOf(Region box2) {
	Rectangle r1 = this.enclosingRectangle;
	Rectangle r2 = box2.enclosingRectangle;
	return ((r1.x + r1.width/2.0) < r2.x);
    }

    boolean isRightOf(Region box2) {
	Rectangle r1 = this.enclosingRectangle;
	Rectangle r2 = box2.enclosingRectangle;
	return (r1.x > (r2.x + r2.width/2.0));
    }


    public void drawEnclosingRectangle(BufferedImage cimage) {

	Polygon p = new Polygon();
	Rectangle r = this.getEnclosingRectangle();
	p.addPoint(r.x, r.y);
	p.addPoint(r.x + r.width, r.y);
	p.addPoint(r.x + r.width, r.y + r.height);
	p.addPoint(r.x, r.y + r.height);

	// Create outter polygon (to make it thicker)
	Polygon p2 = new Polygon();
	p2.addPoint(r.x-1, r.y-1);
	p2.addPoint(r.x + r.width + 1, r.y - 1);
	p2.addPoint(r.x + r.width + 1, r.y + r.height + 1);
	p2.addPoint(r.x - 1, r.y + r.height + 1);

	
	Graphics2D g2 = cimage.createGraphics();
	g2.setColor(Color.white);
	g2.drawPolygon(p);
	g2.drawPolygon(p2);

	String distance = "";
	double ft = this.range / 12.0;
	if (ft > 0.0)
	    distance = (int) ft + "." + (int)((ft - (int)ft) * 10.0) + "ft";

	// now put a label on
	switch (this.hoopLocation) {
	case unknown:
	    break;
	case left: 
	    g2.drawString("Left " + distance, r.x + r.width/4, r.y + r.height/2);
	    break;
	case right:
	    g2.drawString("Right " + distance, r.x + r.width/4, r.y + r.height/2);
	    break;
	case top:
	    g2.drawString("Top " + distance, r.x + r.width/4, r.y + r.height/2);
	    break;
	case bottom:
	    g2.drawString("Bottom " + distance, r.x + r.width/16, r.y + r.height/2);
	    break;
	}
    
}
    public void drawEnclosingPolygon(BufferedImage cimage) {
	Graphics2D g2 = cimage.createGraphics();
	g2.setColor(Color.magenta);
	if (leftEdge != null)
	    g2.draw(leftEdge);
	if (rightEdge != null)
	    g2.draw(rightEdge);
	if (topEdge != null)
	    g2.draw(topEdge);
	if (bottomEdge != null)
	    g2.draw(bottomEdge);
    }


    public String toString() {

	String str = "";
	
	if ((topEdge == null) && (bottomEdge == null)) {
	    Rectangle r = getEnclosingRectangle();
	    str = " " + hoopLocation + " top left at {" + r.x + ", " + r.y + "} bottom right at {" + 
		+ (r.x + r.width) + ", " + (r.x + r.height) + "}";
	}
	else {
	    if (estimates.size() > 0) {
		double sum = 0.0, wgtsum = 0.0;
		for (int i = 0; i < estimates.size(); i++) {
		    HoopEstimate he = estimates.elementAt(i);
		    double wgt = 1.0/he.error;
		    sum += he.range * wgt;
		    wgtsum += wgt;
		}
		double ft = sum / (wgtsum * 12.0);
		int ft10 = (int)((ft - (int)ft) * 10.0);
		str = " " + hoopLocation + " Range = " + (int) ft + "." + ft10 + "ft";
	    }
	}
	return (str);
    }
}
