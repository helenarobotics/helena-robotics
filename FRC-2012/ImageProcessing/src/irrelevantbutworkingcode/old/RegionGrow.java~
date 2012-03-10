import java.io.File;
import java.io.IOException;
import java.util.Vector;
import java.awt.geom.*;
import java.awt.image.BufferedImage;
import java.awt.image.ColorModel;
import java.awt.image.WritableRaster;
import java.awt.image.Raster;


public class RegionGrow {
    public Vector<Region> regions;
    BufferedImage image;
    int minSize;
    int minNeighbors;

    public RegionGrow(BufferedImage _image, int _minNeighbors, int _minSize) {
	image = _image;
	minSize = _minSize;
	minNeighbors = _minNeighbors;
	regions = new Vector<Region> (10);

	// Copy the image file -- this algorithm will trash the original
	BufferedImage tmp = deepCopyGrayscale(image);
	Region region;

	while ((region = grow(tmp, minNeighbors, minSize)) != null) {
	    regions.add(region);
	    //	    System.out.println("Found region " + region);
	}
    }


    // works only on thresholded grayscale
    public Region grow(BufferedImage img, int minNeighhors, int minSize) {

	int [][] dirs = {{-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}};
	WritableRaster raster = img.getRaster();
	Region region = new Region();

	for (int x = 0; x < img.getWidth(); x++) {
	    for (int y = 0; y < img.getHeight(); y++) {
		if ((img.getRGB(x, y) & 0x000000ff) > 0) {
		    int ix = x, iy = y;
		    boolean stuck = false;
		    int dirIndex = 0;
		    int npoints = 0;
		    //		    System.out.print('\n' + "Region from {" + x + ", " + y + "}");
		    while (!stuck) {
			if (npoints > 100000)  // XXX hack to avoid 1 infiite loop.
			    return region;
			if ((ix >= 0) && (ix < img.getWidth()) && (iy >= 0) && (iy < img.getHeight()) &&
			    ((img.getRGB(ix, iy) & 0x000000ff) > 0)) {
			    region.add(new Point(ix, iy));
			    raster.setSample(ix, iy, 0, 0);
			    dirIndex = 0;      // always begin looking in NW direction
			    npoints++;
			    //			    if ((npoints % 10) == 0)
			    //			System.out.println();
			    //			    System.out.print(" {" + ix + ", " + iy + "}");
			    ix += dirs[dirIndex][0];
			    iy += dirs[dirIndex][1];
			}
			else {  // back up and try moving in a new direction
			    if (dirIndex < 7) {
				ix -= dirs[dirIndex][0];
				iy -= dirs[dirIndex][1];
				dirIndex++;
				ix += dirs[dirIndex][0];
				iy += dirs[dirIndex][1];
			    }
			    else { // find a new start point
				//	System.out.print("\nStuck at (" + ix + ", " + iy + ") ");
				stuck = true;
				dirIndex = 0;
				for (int ir = 0; ir < region.size(); ir++) {
				    Point p = region.elementAt(ir);
				    //				    System.out.print("(" + p.x + ", " + p.y + ") ");
				    Point lit = litNeighbor(img, p.x, p.y);
				    if (lit != null) {
					//System.out.println("[UNSTUCK AT (" + lit.x + ", " + lit.y + ")] ");
					ix = lit.x;
					iy = lit.y;
					stuck = false;
					break;
				    }
				}
			    }
			}
		    }
		    System.out.println("Found new region that ends at (" + ix + ", " + iy + ") npoints = " + npoints);
		    if (region.size() >= minSize)
			return region;
		    else return null;
		}
	    }
	}

	return null;      // for situation where we find no lit pixels at all.
    }

    static int neighborCount(BufferedImage img, int ix, int iy) {

	int count = 0;

	for (int dx = -1; dx <= 1; dx++) {
	    int x = ix + dx;
	    if ((x >= 0) && (x < img.getWidth())) {
		for (int dy = -1; dy <= 1; dy++) {
		    int y = iy + dy;
		    if ((y >= 0) && (y < img.getHeight())) {
			if (!((x == ix) && (y == iy)))        // don't count center pixl
			    if ((img.getRGB(x, y) & 0x000000ff) > 0) {
				//				System.out.print("(" + x + "," + y + ")");
				count++;
			    }
		    }
		}
	    }
	}

	return count;
    }


    static Point litNeighbor(BufferedImage img, int ix, int iy) {
	for (int dx = -1; dx <= 1; dx++) {
	    int x = ix + dx;
	    if ((x >= 0) && (x < img.getWidth())) {
		for (int dy = -1; dy <= 1; dy++) {
		    int y = iy + dy;
		    if ((y >= 0) && (y < img.getHeight())) {
			if (!((x == ix) && (y == iy)))        // don't count center pixl
			    if ((img.getRGB(x, y) & 0x000000ff) > 0) {
				return new Point(x, y);
			    }
		    }
		}
	    }
	}
	return null;
    }

    private static BufferedImage deepCopyGrayscale(BufferedImage img) {

	 BufferedImage out = new BufferedImage(img.getWidth(), img.getHeight(), BufferedImage.TYPE_BYTE_GRAY);

	 WritableRaster o = out.getRaster();
	 Raster r = img.getData();

	 int [] buffer  = new int [1];
	 int [] pixel = new int [1];

	 for (int x = 0; x < out.getWidth(); x++){
	     for (int y = 0; y < out.getHeight(); y++){
		 pixel = r.getPixel(x, y, buffer);
		 int val = pixel[0];
		 o.setSample(x, y, 0, val);
	     }
	 }
	 return out;
    }

    public String toString() {
	String str = "Region enclosing rectangles: ";
	for (int i = 0; i < regions.size(); i++) {
	    str = str + '\n' + "  " + i + ": " + regions.elementAt(i);
	}
	return str;
    }
}
