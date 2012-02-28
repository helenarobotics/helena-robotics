import java.io.File;
import java.io.IOException;
import java.util.Vector;
import java.awt.geom.*;
import java.awt.image.BufferedImage;

public class Boxes extends Thread {

    public void main(String args[]) {

    }

    double maxError = 40.0;
    Vector<Box> boxes;
    Vector<Corner> bcorners;     // for debugging only -- we can kill this later

    public Boxes(Vector<Line2D.Double> segments) {

	// Create a local copy of segments vector -- we'll modify it locally
	Vector<Line2D.Double> tmp = new Vector<Line2D.Double>(segments);

	// First find all the feasible corners by pairwise inspection of the line segments
	// and sort by morphological role (upper-left, lower-right, etc)
	Vector<Corner> ul = new Vector<Corner>(20);
	Vector<Corner> ur = new Vector<Corner>(20);
	Vector<Corner> lr = new Vector<Corner>(20);
	Vector<Corner> ll = new Vector<Corner>(20);

	this.bcorners = new Vector<Corner>(20);   // for debugging only - can kill later

	for (int j = 0; j < tmp.size(); j++) {
	    Line2D.Double seg1 = tmp.elementAt(j);
	    for (int i = j+1; i < tmp.size(); i++) {
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
		if (c.cornerType != Corner.CornerLocation.none) {
		    System.out.println("Corner: " + c);
		    this.bcorners.add(c);
		}
	    }
	}

	// Now try to combine corners into collection of boxes that achieve lowest total error
	// (starting with simple "greedy algorithm")

	this.boxes = new Vector<Box>(20);
	Corner urc = null, ulc = null, lrc = null, llc = null;

	System.out.println("Corner counts: " + ul.size() + " UL, " + ur.size() + " UR, " + lr.size() + " LR, " + ll.size() + " LL" + '\n');

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
		    System.out.println("Starting with UR " + b);

		    for (int j = 0; j < lr.size(); j++) {
			lrc = lr.elementAt(j);
			//  System.out.println("Looking at LR box " + lrc);
			if (urc.shareLine(lrc)) {
			    b.addCorner(lrc);
			    System.out.println("addingd LR " + b);
			    
			    for (int k = 0; k < ll.size(); k++) {
				llc = ll.elementAt(k);
				// System.out.println("Looking at LL box " + ulc);
				if (lrc.shareLine(llc) && ulc.shareLine(llc)) {
				    b.addCorner(llc);
				    System.out.println("finishing with  LL " + b);
				    found = true;
				    break;
				}
			    }
			    break;
			}
		    }
		    break;
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
   

    public void drawCorners(BufferedImage image) {

	for (int i = 0; i < bcorners.size(); i++) {
	    Corner c = bcorners.elementAt(i);
	    c.draw(image);
	}
    }

    public void draw(BufferedImage image) {
	for (int i = 0; i < boxes.size(); i++) {
	    Box b = boxes.elementAt(i);
	    b.draw(image);
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



