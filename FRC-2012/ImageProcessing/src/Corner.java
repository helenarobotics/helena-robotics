import java.util.Vector;
import java.awt.geom.*;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.awt.Polygon;

public class Corner extends Thread {

    // for testing
    public static void main(String[] args) { 
 
	Line2D.Double line1 = new Line2D.Double(1.0, 1.0, 1.0, 10.0);   // vertical line
	Line2D.Double line2 = new Line2D.Double(1.0, 11.0, 20.0, 11.0); // horizontal line


	System.out.println("1st line: " + lineToString(line1));
	System.out.println("2nd line: " + lineToString(line2));

        Corner corner = new Corner(line2, line1, 5.0);

        System.out.println(corner.toString());


	line1 = new Line2D.Double(22.0, 2.0, 22.9, 11.0);   // vertical-ish line
	line2 = new Line2D.Double(1.0, 1.0, 20.0, 0.0); // horizontal-ish line


	System.out.println("1st line: " + lineToString(line1));
	System.out.println("2nd line: " + lineToString(line2));

	corner = new Corner(line1, line2, 5.0);

	System.out.println(corner.toString());

	// parallel lines
	line1 = new Line2D.Double(1.0, 1.0, 2.0, 10.0);  
	line2 = new Line2D.Double(2.0, 1.0, 3.0, 10.0); 

	System.out.println("1st line: " + lineToString(line1));
	System.out.println("2nd line: " + lineToString(line2));

	corner = new Corner(line1, line2, 5.0);

	System.out.println(corner.toString());
    
    }

    public enum CornerLocation { none, upperLeft, upperRight, lowerLeft, lowerRight };

    Point2D.Double intersection;            // {x, y} location of intersection
    CornerLocation cornerType;              // describes configuration of lines, relative to role in "box-like" polygon
    Line2D.Double verticalLine;             // first point in Line is closest to point of intersection
    Line2D.Double horizontalLine;     
    double verticalError, horizontalError;      // distance between intersection and end ofrespective lines

    public Corner(Line2D.Double line1, Line2D.Double line2, double maxError) {

	intersection = intersectingPoint(line1, line2);

	if (intersection == null) {
	    cornerType = CornerLocation.none;    // does not intersect;
	}
	else {
	    if (Math.atan2(Math.abs(line1.y2 - line1.y1), Math.abs(line1.x2 - line1.x1)) >
		Math.atan2(Math.abs(line2.y2 - line2.y1), Math.abs(line2.x2 - line2.x1))) {
		verticalLine = OrderPoints(line1, intersection);
		horizontalLine = OrderPoints(line2, intersection);
	    }
	    else {
		verticalLine = OrderPoints(line2, intersection);
		horizontalLine = OrderPoints(line1, intersection);
	    }

	    verticalError = distance(new Point2D.Double(verticalLine.x1, verticalLine.y1), intersection);
	    horizontalError = distance(new Point2D.Double(horizontalLine.x1, horizontalLine.y1), intersection);

	    if ((verticalError > maxError) || (horizontalError > maxError))
		cornerType = CornerLocation.none;
	    else {
		// Now figure out the orientation of the lines to determine CornerType

		boolean top, left;

		if (verticalLine.y1 > verticalLine.y2)    // If true, we're a top corner
		    top = true;
		else top = false;
		if (horizontalLine.x1 < horizontalLine.x2)  // if true, we're a left corner
		    left = true;
		else left = false;

		if (top & left)
		    cornerType = CornerLocation.upperLeft;
		else if (top & !left)
		    cornerType = CornerLocation.upperRight;
		else if (!top & left)
		    cornerType = CornerLocation.lowerLeft;
		else 
		    cornerType = CornerLocation.lowerRight;
	    }
	}
    }


    static  String lineToString(Line2D.Double line) {
	return ("{" + (int)line.x1 + ", " + (int)line.y1 + "} to {" + 
		(int)line.x2 + ", " + (int)line.y2 + "}");
    }


    public String toString() {
	if (cornerType == CornerLocation.none)
	    return ("Not an intersection");
	else
	    return (cornerType + " intersection at {" + (int) intersection.x + ", " + (int) intersection.y + "}; " + 
		    "horiz = " + lineToString(horizontalLine) + ", " +
		    "err " + (int) horizontalError + "; " +
		    " vert = " + lineToString(verticalLine) + ", " +
		    "err " + (int) verticalError);
    }

    // Order a line so that the first {x, y} point refers to the point nearest intersection
		
    static Line2D.Double OrderPoints(Line2D.Double line, Point2D.Double pt) {
	double d1 = distance(new Point2D.Double(line.x1, line.y1), pt);
	double d2 = distance(new Point2D.Double(line.x2, line.y2), pt);
	
	if (d1 < d2)
	    return new Line2D.Double(line.x1, line.y1, line.x2, line.y2);  // Copy line; ordering already correct
	else
	    return new Line2D.Double(line.x2, line.y2, line.x1, line.y1);  // "flip" line
    }
	
		
	
    static double distance(Point2D.Double p1, Point2D.Double p2) {
	return (Math.sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)));
	}
	

    // determines whether two line segments intersect
    static boolean segmentsIntersect(Line2D.Double line1, Line2D.Double line2) {
	Point2D.Double inter = intersectingPoint(line1, line2);
	if (inter != null)
	    return ((inter.x >= line1.x1) && (inter.x <= line1.x2) && (inter.x >= line2.x1) && (inter.x <= line2.x2));
	else return false;
    }



    /*
     * Calculates intersection of two lines as a Point2D.Double class.  Returns 'null' if the lines prove parallel.
     * NOTE that the point of intersection may prove outside the line segment (i.e., at some extrapolated point);
     * See boolean method "segmentsIntersect", above, to test for that condition.
     */

    static Point2D.Double intersectingPoint(Line2D.Double line1, Line2D.Double line2) {

	    double m1, m2, b1, b2;

	    double dx1 = line1.x2 - line1.x1;
	    double dy1 = line1.y2 - line1.y1;
	    double dx2 = line2.x2 - line2.x1;
	    double dy2 = line2.y2 - line2.y1;


	    // Test for parallel lines: two cases: non-vertical, and vertical
	    if ((Math.abs(dx1) > 0.0) && (Math.abs(dx2) > 0.0)) {   // non-vertical lines may or may not
		m1 = dy1/dx1;
		m2 = dy2/dx2;
		if (Math.abs(m1 - m2) < 0.00001)
		    return null;
	    }
	    else if (dx1 == 0.0 && dx2 == 0.0)    // two vertical lines never interset
		return null;

	    // Made it this far, so we know that the lines intersect (somwehere):
		
	    Point2D.Double intersect = new Point2D.Double();

	    // Handle the special cases for vertical lines
	    if (line1.x1 == line1.x2) {          // line1 vertical case
		m2 = dy2 / dx2;
		b2 = line2.y1 - line2.x1 * m2;
		intersect.x = line1.x1;
		intersect.y = intersect.x * m2 + b2;
	    }
	    else if (line2.x1 == line2.x2) {     // line2 vertical case
		m1 = dy1 / dx1;
		b1 = line1.y1 - line1.x1 * m1;
		intersect.x = line2.x1;
		intersect.y = intersect.x * m1 + b1;
	    }
	    else {                               // general case (neither line vertical)
		m1 = dy1 / dx1;
	        b1 = line1.y1 - line1.x1 * m1;

		m2 = dy2 / dx2;
		b2 = line2.y1 - line2.x1 * m2;
		   
		intersect.x = (b2 - b1) / (m1 - m2);
		intersect.y = intersect.x * m1 + b1;
	    }

	    return intersect;
    }

    public boolean shareLine(Corner c2) {

	if (linesEqual(this.horizontalLine, c2.horizontalLine) ||
	    linesEqual(this.verticalLine, c2.horizontalLine) ||
	    linesEqual(this.horizontalLine, c2.verticalLine) ||
	    linesEqual(this.verticalLine, c2.verticalLine))
	    return true;
	else return false;
    }


	private static boolean linesEqual(Line2D.Double l1, Line2D.Double l2) {

	    if ((l1.x1 == l2.x1) && (l1.y1 == l2.y1) && (l1.x2 == l2.x2) && (l1.y2 == l2.y2))
		return true;
	    else if ((l1.x2 == l2.x1) && (l1.y2 == l2.y1) && (l1.x1 == l2.x2) && (l1.y1 == l2.y2))
		return true;
	    else return false;
	}


    public void draw(BufferedImage image) {
	Graphics2D g = image.createGraphics();

	Polygon p = new Polygon();
	int x1 = (int) intersection.x; 
	int y1 = (int) intersection.y;
	int x0 = 0, y0 = 0, x2 = 0, y2 = 0;
	
	switch (cornerType) {
	case none:
	    break;
	case upperLeft: 
	    x0 = x1;
	    y0 = y1 - 20;
	    x2 = x1 + 20;
	    y2 = y1;
	    break;
	case upperRight:
	    x0 = x1 - 20;
	    y0 = y1;
	    x2 = x1;
	    y2 = y1 - 20;
	    break;
	case lowerLeft:
	    x0 = x1;
	    y0 = y1 + 20;
	    x2 = x1 + 20;
	    y2 = y1;
	    break;
	case lowerRight:
	    x0 = x1;
	    y0 = y1 + 20;
	    x2 = x1 - 20;
	    y2 = y1;
	    break;
	}
	System.out.println("Corner.draw: " + x0 + ", " + y0);
	p.addPoint(x0, y0);
	p.addPoint(x1, y1);
	p.addPoint(x2, y2);
	g.draw(p);
    }
}
