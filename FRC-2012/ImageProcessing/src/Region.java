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

import java.io.File;
import java.io.IOException;
import java.awt.*;


public class Region {

    public enum HoopLocation { unknown, left, top, right, bottom };

    Vector<Point> points;
    public HoopLocation hoopLocation;
    public Rectangle enclosingRectangle;
    public Polygon enclosingPolygon;

    public Region() {
	points = new Vector<Point> (2000);
	hoopLocation = HoopLocation.unknown;
	enclosingRectangle = null;
	enclosingPolygon = null;
    }

    public void finish(BufferedImage image) {
	calculateEnclosingRectangle();
	calculateEnclosingPolygon(image);
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

    public Polygon getEnclosingPolygon() {
	return enclosingPolygon;
    }

    public Polygon getEnclosingPolygon(BufferedImage image) {

	if (enclosingPolygon == null)
	    calculateEnclosingPolygon(image);

	return enclosingPolygon;
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
     *  returns a java.awt.Polygon containing the four hoop corners, UNLESS a hoop edge is more than 50% occluded;
     *   in this event, a null is returned.
     * 
     *   Note that any pixel > 0 is assumed "on" -- a general grayscale scene would likely create disasterous results.
     * 
     *   (We may need to add logic to avoid misinterpreting hoops that are located along edge images.  The "50% rule"
     *    described above helps, but in some cases an edge can be partially occluded but apparently still visible.
     *    For example, we could ignore hoops with one or more of their bounding rectangle edges is along the image
     *    boundary.  TBC).
     */

    public Polygon calculateEnclosingPolygon(BufferedImage img) {

	// for debugging
	/*
	//	BufferedImage cimage = new BufferedImage(img.getWidth(), img.getHeight(),  BufferedImage.TYPE_INT_RGB);
	//	Graphics g = cimage.getGraphics();  
	//	g.drawImage(img, 0, 0, null);

	int red = new Color(255, 0, 0).getRGB(); 
	int grn = new Color(0, 255, 0).getRGB(); 
	int blu = new Color(0, 0, 255).getRGB(); 
	int pur = new Color(255, 0, 255).getRGB(); 
	*/

	Rectangle r = getEnclosingRectangle();

	dataPoint [] dp = new dataPoint [Math.max(r.width, r.height) + 1];

	int wsearch = r.width / 4;   // search for edge w/in 25% (1/4) of enclosing box;
	int hsearch = r.height/ 4;   //   (this limit helps avoid outliers)

	// Calculate best line fit to left edge (using thresholded image data)
	int count = 0;
	for (int y = r.y; y < r.y + r.height; y++) {
	    for (int x = r.x; x < r.x + wsearch; x++) {
		if ((img.getRGB(x, y) & 0x000000ff) > 0) {
		    dp[count] = new dataPoint(y, x);
		    count++;
		    break;
		}
	    }
	}
	if (count < r.height/2)
	    return null;
	double [] resultsLeft = leastSquares(dp, count, 2*count/3);
	// debug
	/*	for (int i = 0; i < 2*count/3; i++) {
	    int x = (int)dp[i].x;
	    int y = (int)dp[i].y;
	    cimage.setRGB(y, x, red);
	}
	*/

	// Calculate best line fit to right edge (using thresholded image data)
	count = 0;
	for (int y = r.y; y < r.y + r.height; y++) {
	    for (int x = r.x + r.width; x >  r.x + (r.width - wsearch); x--) {
		if ((img.getRGB(x, y) & 0x000000ff) > 0) {
		    //    dp[count] = new dataPoint(x, y);
		    dp[count] = new dataPoint(y, x);
		    count++;
		    break;
		}
	    }
	}
	if (count < r.height/2)
	    return null;
	double resultsRight [] = leastSquares(dp, count, 2*count/3);
	// debug
	/*	for (int i = 0; i < 2*count/3; i++) {
	    int x = (int)dp[i].x;
	    int y = (int)dp[i].y;
	    cimage.setRGB(y, x, red); 
	}
	*/
	// Calculate best line fit to top edge (using thresholded image data)
	count = 0;
	for (int x = r.x; x < r.x + r.width; x++) {
	    for (int y = r.y; y < r.y + hsearch; y++) {
		if ((img.getRGB(x, y) & 0x000000ff) > 0) {
		    dp[count] = new dataPoint(x, y);
		    count++;
		    break;
		}
	    }
	}
	if (count < r.width/2)
	    return null;
	double resultsTop [] = leastSquares(dp, count, 2*count/3);

	// Calculate best line fit to bottom edge (using thresholded image data)
	count = 0;
	for (int x = r.x; x < r.x + r.width; x++) {
	    int y;
	    for (y = r.y + r.height; y > r.y + (r.height - hsearch); y--) {
		if ((img.getRGB(x, y) & 0x000000ff) > 0) {
		    dp[count] = new dataPoint(x, y);
		    count++;
		    break;
		}
	    }
	}
	if (count < r.width/2)
	    return null;
	double resultsBottom [] = leastSquares(dp, count, 2*count/3);
	// debug
	/*	for (int i = 0; i < 3*count/4; i++) {
	    int x = (int)dp[i].x;
	    int y = (int)dp[i].y;
	    cimage.setRGB(x, y, red); 
	}
	*/
	/*
	try {
	    ImageIO.write(cimage, "jpg", new File("edgeoverlay.jpg"));
            } catch (IOException e) {
                e.printStackTrace();
            }
	*/

	// Now find the four points of intersection
	Point2D.Double leftTop = intersectingPoint(resultsTop[0], resultsTop[1], resultsLeft[0], resultsLeft[1]);
	Point2D.Double rightTop = intersectingPoint(resultsTop[0], resultsTop[1], resultsRight[0], resultsRight[1]);
	Point2D.Double leftBottom = intersectingPoint(resultsBottom[0], resultsBottom[1], resultsLeft[0], resultsLeft[1]);
	Point2D.Double rightBottom = intersectingPoint(resultsBottom[0], resultsBottom[1], resultsRight[0], resultsRight[1]);

	/*	System.out.println("Top left corner at     " + (int)leftTop.x + ", " +  (int)leftTop.y);
	System.out.println("Top right corner at    " + (int)rightTop.x + ", " +  (int)rightTop.y);
	System.out.println("Bottom right corner at " + (int)rightBottom.x + ", " +  (int)rightBottom.y);
	System.out.println("Bottom left corner at " +  (int)leftBottom.x + ", " +  (int)leftBottom.y);
	*/
	Polygon p = new Polygon();

	p.addPoint((int) leftTop.x, (int) leftTop.y);
	p.addPoint((int) rightTop.x, (int) rightTop.y);
	p.addPoint((int) rightBottom.x, (int) rightBottom.y);
	p.addPoint((int) leftBottom.x, (int) leftBottom.y);

	enclosingPolygon = p;     // save this result in case anyone asks again...

	double dx = leftTop.x - leftBottom.x;
	double dy = leftTop.y - leftBottom.y;
	double leftside = Math.sqrt(dx*dx + dy*dy);

	dx = rightTop.x - rightBottom.x;
	dy = rightTop.y - rightBottom.y;
	double rightside = Math.sqrt(dx*dx + dy*dy);

	/*	String str = "right";
	if (rightside < leftside)
	    str = "left";
	System.out.println("left/right ratio = " + leftside / rightside + " " + 
			   + (180.0 / Math.PI) * Math.acos(Math.min(rightside, leftside)
							   / Math.max(rightside, leftside))
			   + " degrees " + str + " from center");
	*/
	return p;
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

	// Now draw a tight polygon around the hoop, if available

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

	double ft = FieldGeometry.estimateRange(this) / 12.0;
	String distance = (int) ft + "." + (int)((ft - (int)ft) * 10.0) + "ft";

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
	Polygon p2 = this.getEnclosingPolygon();
	if (p2 != null) {
	    Graphics2D g2 = cimage.createGraphics();
	    g2.setColor(Color.magenta);
	    g2.drawPolygon(p2);
	}
	else System.out.println("Ignoring substantially occluded hoop");
    }


    public String toString() {
	Polygon  p = getEnclosingPolygon();
	String str;

	if (p == null) {
	    Rectangle r = getEnclosingRectangle();
	    str = " " + hoopLocation + " top left at {" + r.x + ", " + r.y + "} bottom right at {" + 
		+ r.x + r.width + ", " + r.x + r.height + "}";
	}
	else {
	    double ft = FieldGeometry.estimateRange(this) / 12.0;
	    int ft10 = (int)((ft - (int)ft) * 10.0);
	    
	    // , top left at {" + p.xpoints[0] + ", " + p.ypoints[0] + "} bottom right at {" + 
	    //		+ p.xpoints[2] + ", " + p.xpoints[2] + "}";
	    str = " " + hoopLocation + " Range = " + (int) ft + "." + ft10 + "ft";
	    str = str + ", yvalue of top middle = " + (p.ypoints[0] + p.ypoints[1])/2.0;
	}

	return (str);
    }
}




    
    
