import java.util.Vector;
import java.awt.geom.*;

public class Box {
    public Vector<Corner> corners;

    Box() {
	corners = new Vector<Corner>(4);
    }

    Box(Corner firstCorner) {
	corners = new Vector<Corner>(4);
	addCorner(firstCorner);
    }

    public int size() {
	return corners.size();
    }

    public void addCorner(Corner corner) {
	corners.add(corner);
    }
    public void removeCorner(Corner corner) {
	corners.remove(corner);
    }
    public Corner getCorner(int index) {
	return corners.get(index);
    }

    // measures the overall fit of lines to corners that form this box
    public double fitError() {
	if (corners.size() < 4)
	    return 10000.0;
	else {
	    double error = 0.0;
	    for (int j = 0; j < corners.size(); j++) {
		Corner c = corners.elementAt(j); 
		error = error + c.horizontalError + c.verticalError;
	    }
	    return error;
	}
    }

    public String toString() {
	String str = "";
	for (int i = 0; i < corners.size(); i++) {
	    Corner c = corners.elementAt(i);
	    str = str + c + '\n';
	}

	return str;
    }
}


