import java.io.File;
import java.io.IOException;
import java.util.Vector;
import java.awt.geom.*;

public class Boxes extends Thread {

    public void main(String args[]) {

    }

    double maxError = 25.0;
    Vector<Box> boxes;

    public Boxes(Vector<Line2D.Double> segments) {

	// Create a local copy of segments vector -- we'll modify it locally
	Vector<Line2D.Double> tmp = new Vector<Line2D.Double>(segments);

	// First find all the feasible corners by pairwise inspection of the line segments
	// and sort by morphological role (upper-left, lower-right, etc)
	Vector<Corner> ul = new Vector<Corner>(20);
	Vector<Corner> ur = new Vector<Corner>(20);
	Vector<Corner> lr = new Vector<Corner>(20);
	Vector<Corner> ll = new Vector<Corner>(20);

	for (int j = 0; j < segments.size(); j++) {
	    Line2D.Double seg1 = tmp.elementAt(j);
	    for (int i = 1; i < tmp.size(); i++) {
		Line2D.Double seg2 = tmp.elementAt(i);
		Corner c = new Corner(seg1, seg2, maxError);
		switch (c.cornerType) {
		case none:
		    break;
		case upperLeft:
		    ul.add(c);
		    break;
		case upperRight:
		    ur.add(c);
		    break;
		case lowerRight:
		    lr.add(c);
		    break;
		case lowerLeft:
		    ll.add(c);
		    break;
		}
	    }
	}

	// Now try to combine corners into collection of boxes that achieve lowest total error
	// (starting with simple "greedy algorithm")

	this.boxes = new Vector<Box>(20);
	Corner urc = null, ulc = null, lrc = null, llc = null;

	while (ul.size() > 0) {
	    ulc = ul.elementAt(0);
	    ul.remove(ulc);
	    Box b = new Box(ulc);

	    boolean found = false;

	    for (int i = 0; i < ur.size(); i++) {
		urc = ur.elementAt(i);
		//		System.out.println("Looking at UR box " + ur);
		if (ulc.shareLine(urc)) {
		    b.addCorner(urc);
		    System.out.println("Found UR " + b);

		    for (int j = 0; j < lr.size(); j++) {
			lrc = lr.elementAt(j);
			//  System.out.println("Looking at LR box " + lrc);
			if (urc.shareLine(lrc)) {
			    b.addCorner(lrc);
			    System.out.println("Found LR " + b);
			    
			    for (int k = 0; k < ll.size(); k++) {
				llc = ll.elementAt(k);
				// System.out.println("Looking at LL box " + ulc);
				if (lrc.shareLine(llc) && ulc.shareLine(llc)) {
				    b.addCorner(llc);
				    System.out.println("Found LL " + b);
				    found = true;
				}
			    }
			}
		    }
		}
	    }

	    if (found) {
		ur.remove(urc);
		ll.remove(llc);
		lr.remove(lrc);
		this.boxes.add(b);
	    }
	}
    }
   

    public int size() {
	return boxes.size();
    }

public    String toString() {

	String str = "";

	for (int i = 0; i < boxes.size(); i++) {
	    Box b = boxes.elementAt(i);
	    str = str + b + '\n';
	}

	return str + '\n';
    }
}



