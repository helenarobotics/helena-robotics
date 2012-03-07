import java.util.Vector;
import java.awt.Rectangle;
import java.awt.image.BufferedImage;
import java.awt.geom.*;
import java.awt.Polygon;

public class Region {

    Vector<Point> points;

    public Region() {
	points = new Vector<Point> (2000);
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

    public Rectangle enclosingRectangle() {
	int minX = 1000000, minY = 1000000, maxX = -1, maxY = -1;

	for (int i = 0; i < size(); i++) {
	    Point p = elementAt(i);
	    if (p.x < minX) minX = p.x;
	    if (p.x > maxX) maxX = p.x;
	    if (p.y < minY) minY = p.y;
	    if (p.y > maxY) maxY = p.y;
	}

	return new Rectangle(minX, minY, (maxX - minX), (maxY - minY));
    }

	    
    public Polygon enclosingPolygon(BufferedImage img) {
	Rectangle r = enclosingRectangle();
	dataPoint [] dp = new dataPoint [Math.max(r.width, r.height) + 1];

	// Calculate best line fit to left edge (using thresholded image data)
	int count = 0;
	for (int y = r.y; y < r.y + r.height; y++) {
	    for (int x = r.x; x < r.x + r.width; x++) {
		if ((img.getRGB(x, y) & 0x000000ff) > 0) {
		    dp[count] = new dataPoint(x, y);
		    count++;
		    break;
		}
	    }
	}
	double resultsLeft [] = leastSquares(dp, count, count/2);

	// Calculate best line fit to right edge (using thresholded image data)
	count = 0;
	for (int y = r.y; y < r.y + r.height; y++) {
	    for (int x = r.x + r.width; x >=  r.x; x--) {
		if ((img.getRGB(x, y) & 0x000000ff) > 0) {
		    dp[count] = new dataPoint(x, y);
		    count++;
		    break;
		}
	    }
	}
	double resultsRight [] = leastSquares(dp, count, count/2);

	// Calculate best line fit to top edge (using thresholded image data)
	count = 0;
	for (int x = r.x; x < r.x + r.width; x++) {
	    for (int y = r.y; y < r.y + r.height; y++) {
		if ((img.getRGB(x, y) & 0x000000ff) > 0) {
		    dp[count] = new dataPoint(x, y);
		    count++;
		    break;
		}
	    }
	}
	double resultsTop [] = leastSquares(dp, count, count/2);

	// Calculate best line fit to bottom edge (using thresholded image data)
	count = 0;
	for (int x = r.x; x < r.x + r.width; x++) {
	    int y;
	    for (y = r.y + r.height; y > r.y; y--) {
		if ((img.getRGB(x, y) & 0x000000ff) > 0) {
		    dp[count] = new dataPoint(x, y);
		    count++;
		    break;
		}
	    }
	}
	double resultsBottom [] = leastSquares(dp, count, count/2);


	// Now find the four points of intersection
	Point2D.Double leftTop = intersectingPoint(resultsTop[0], resultsTop[1], resultsLeft[0], resultsLeft[1]);
	Point2D.Double rightTop = intersectingPoint(resultsTop[0], resultsTop[1], resultsRight[0], resultsRight[1]);
	Point2D.Double leftBottom = intersectingPoint(resultsBottom[0], resultsBottom[1], resultsLeft[0], resultsLeft[1]);
	Point2D.Double rightBottom = intersectingPoint(resultsBottom[0], resultsBottom[1], resultsRight[0], resultsRight[1]);

	System.out.println("Top left corner at     " + (int)leftTop.x + ", " +  (int)leftTop.y);
	System.out.println("Top right corner at    " + (int)rightTop.x + ", " +  (int)rightTop.y);
	System.out.println("Bottom right corner at " + (int)rightBottom.x + ", " +  (int)rightBottom.y);
	System.out.println("Bottom left corner at " +  (int)leftBottom.x + ", " +  (int)leftBottom.y);

	Polygon p = new Polygon();

	p.addPoint((int) leftTop.x, (int) leftTop.y);
	p.addPoint((int) rightTop.x, (int) rightTop.y);
	p.addPoint((int) rightBottom.x, (int) rightBottom.y);
	p.addPoint((int) leftBottom.x, (int) leftBottom.y);

	return p;
    }

    static Point2D.Double intersectingPoint(double m1, double b1, double m2, double b2) {
	double x = (b2 - b1) / (m1 - m2);   // x at point of intersection (if they intersect!)
	return (new Point2D.Double(x, x * m1 + b1));
    }

    // Simple least squares linear fit algorithm.  Returns a 2-long array of doubles, the first (slot [0]) of which 
    // represents m, 2nd b, in the familiar line equation y = mx + b

    private double [] leastSquares(dataPoint dp [], int n, int keep) {
	leastSquares(dp, n);
	quickSort(dp, 0, n-1);
	/*	System.out.println("Errors after sorting: ");
	for (int i = 0; i < n; i++) {
	    System.out.print(" {" + (int)dp[i].x + "," + (int)dp[i].y + "}=" + dp[i].err);
	}
	*/
	return (leastSquares(dp, keep));
    }

    private double [] leastSquares(dataPoint dp [], int n) {

	double sumx = 0.0, sumx2 = 0.0, sumy = 0;

	for (int i = 0; i < n; i++) {
	    sumx  += dp[i].x;
	    sumx2 += dp[i].x * dp[i].x;
	    sumy  += dp[i].y;
	}

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

	// print results
	System.out.println("y   = " + beta1 + " * x + " + beta0 + " std err " + Math.sqrt(err2/(double)n));

	double result [] = new double [2];
	result[0] = beta1;
	result[1] = beta0;
	return (result);
    }
	

    public String toString() {
	Rectangle e = enclosingRectangle();
	int xc = e.x + e.width/2;
	int yc = e.y + e.height/2;
	return "Centers on {" + xc + ", " + yc + "} width = " + e.width + ", height = " + e.height;
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

private void quickSort(dataPoint arr[], int left, int right)
    {
	int index = partition(arr, left, right);
	if (left < index - 1)
            quickSort(arr, left, index - 1);
	if (index < right)
            quickSort(arr, index, right);
    }
}


    
    
