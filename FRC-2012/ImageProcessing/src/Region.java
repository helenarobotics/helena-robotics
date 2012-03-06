import java.util.Vector;
import java.awt.Rectangle;

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
	    

    public String toString() {
	Rectangle e = enclosingRectangle();
	int xc = e.x + e.width/2;
	int yc = e.y + e.height/2;
	return "Centers on {" + xc + ", " + yc + "} width = " + e.width + ", height = " + e.height;
    }
}


    
    
