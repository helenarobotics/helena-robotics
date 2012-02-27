import java.io.File;
import java.io.IOException;
import java.util.Vector;
import java.awt.geom.*;

public class Boxes extends Thread {

    public void main(String args[]) {

    }

    double maxError = 15.0;
    Vector<Box> boxes;

    public Boxes(Vector<Line2D.Double> segments) {

	// Create a local copy of segments vector -- we'll modify it locally
	Vector<Line2D.Double> tmp = new Vector<Line2D.Double>(segments); 

	// First find all the feasible corners by pairwise inspection of the line segments

	Vector<Corner> corners = new Vector<Corner>(20);

	for (int j = 0; j < tmp.size(); j++) {
	    Line2D.Double seg1 = tmp.elementAt(j);

	    for (int i = 1; i < tmp.size(); i++) {
	    Line2D.Double seg2 = tmp.elementAt(i);
	    Corner c = new Corner(seg1, seg2, maxError);
	    if (c.cornerType != Corner.CornerLocation.none)
		corners.add(c);
	    }
	}

	// Now try to combine corners into collection of boxes that achieve lowest total error
	// (starting with simple "greedy algorithm")

	this.boxes = new Vector<Box>(20);

	for (int k = 0; k < corners.size(); k++) {
	    Corner c1 = corners.elementAt(k);
	    Box b = new Box(c1);
	    for (int n = 1; n < corners.size(); n++) {
		Corner c2 = corners.elementAt(n);
		if ((c1.cornerType != c2.cornerType) && c1.shareLine(c2)) {
		    b.addCorner(c2);
		    corners.remove(c2);
		}
	    }
	    if (b.size() > 1)         // if this corner found others, keep the box, else discard
		boxes.add(b);
	}

	System.out.println("Boxes: ");
	System.out.println(boxes);
    }

    public int size() {
	return boxes.size();
    }

public    String toString() {

	String str = "";

	for (int i = 0; i < boxes.size(); i++) {
	    Box b = boxes.elementAt(i);
	    str = str + b;
	}

	return str;
    }
}



