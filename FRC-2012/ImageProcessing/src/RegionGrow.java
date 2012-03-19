import java.io.File;
import java.io.IOException;
import java.util.Vector;
import java.awt.geom.*;
import java.awt.image.BufferedImage;
import java.awt.image.ColorModel;
import java.awt.image.WritableRaster;
import java.awt.image.Raster;
import java.awt.Rectangle;
import java.awt.Polygon;
import javax.vecmath.Point3d;

public class RegionGrow {
    public Vector<Region> regions;
    BufferedImage thresholdedImage;    // color-detected and thresholded image we keep.
    int threshold;                     // pixel threshold for on/off after color detection
    int minSize;                       // minimum acceptable pixel count in grown regions (remove and ignore anything smaller)

    public RegionGrow(BufferedImage image, int _threshold, int _minSize) {
        minSize = _minSize;
	regions = new Vector<Region> (10);
	threshold = _threshold;
	thresholdedImage = detectGreen(image, threshold);

	Region region;

	// Note that 'grow' zero's out pixels after they've been grouped. We'll need the original
	// later for geometry calculations (polygon in particular), so create and use a throw-away copy

	BufferedImage tmp = deepCopyGrayscale(thresholdedImage);

	while ((region = grow(tmp)) != null) {
	    region.finish(thresholdedImage);       // calculates and saves rect and tight polygon boundaries, etc.
	    regions.add(region);
	    //	    System.out.println("Found region " + region);
	}
	labelHoops();
	estimateRanges();
	estimateRobotPosition();
    }

    public void estimateRanges() {
	for (int i = 0; i < regions.size(); i++) {
	    Region r = regions.elementAt(i);
	    if (r.hoopLocation != Region.HoopLocation.unknown)
		FieldGeometry.estimateRange(r);
	}
    }


    public Point3d estimateRobotPosition() {
	double maxBaseline = 0.0;
	HoopEstimate e1 = null, e2 = null;

	// find the wides available baseline

	for (int i = 0; i < regions.size(); i++) {
	    Region ri = regions.elementAt(i);
	    for (int j = i; j < regions.size(); j++) {
		Region rj = regions.elementAt(j);
		for (int rii = 0; rii < ri.estimates.size(); rii++) {
		    HoopEstimate eii = ri.estimates.elementAt(rii);
		    for (int rjj = rii; rjj < rj.estimates.size(); rjj++) {
			HoopEstimate ejj = rj.estimates.elementAt(rjj);
			if (Math.abs(ejj.rangePoint.z - eii.rangePoint.z) < 1) {
			    double baseline = Math.abs(ejj.rangePoint.x - eii.rangePoint.x);
			    if (baseline > maxBaseline) {
				e1 = eii;
				e2 = ejj;
				maxBaseline = baseline;
			    }
			}
		    }
		}
	    }
	}
	System.out.println("e1 = " + e1);
	System.out.println("e2 = " + e2);

	if ((e1 != null) && (e2 != null)) {
	    double phi = Math.abs(e1.azimuth - e2.azimuth);
	    double lambda = Math.asin((e1.range * Math.sin(phi)) / maxBaseline);
	    if (e1.range > e2.range)
		lambda = Math.PI - lambda;
	    double xloc = e2.rangePoint.x - e2.range * Math.cos(lambda);
	    double yloc = e2.rangePoint.y + e2.range * Math.sin(lambda);
	    System.out.println("Robot located at {" + (int)xloc + ", " + (int)yloc + "}; phi=" + (int) Math.toDegrees(phi) 
			       + ", lambda=" + (int)Math.toDegrees(lambda));
	    return new Point3d(xloc, yloc, FieldGeometry.cameraHeight);
	}
	else return null;
    }


    private static BufferedImage detectGreen(BufferedImage img, int thresh) {

	 BufferedImage green = new BufferedImage(img.getWidth(), img.getHeight(), BufferedImage.TYPE_BYTE_GRAY);
	 WritableRaster raster = green.getRaster();

	 for (int x = 0; x < img.getWidth(); x++){
	     for (int y = 0; y < img.getHeight(); y++){
		 int rgb[] = getPixelData(img, x, y);
		 int pixelValue = rgb[1] - (rgb[0] + rgb[2])/2;
		 //   if (pixelValue > 255) pixelValue = 255;
		 // else if (pixelValue < 0) pixelValue = 0;
		 if (pixelValue >= thresh)
		     pixelValue = 255;
		 else pixelValue = 0;
		 raster.setSample(x, y, 0, pixelValue);
	     }
	 }
	 return (green);
    }


    private static int[] getPixelData(BufferedImage img, int x, int y) {
        int argb = img.getRGB(x, y);

        int rgb[] = new int[] {
            (argb >> 16) & 0xff, //red
            (argb >>  8) & 0xff, //green
            (argb      ) & 0xff  //blue
        };
        return rgb;
    }


    // works only on thresholded grayscale
    public Region grow(BufferedImage img) {

	int [][] dirs = {{-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}};
	WritableRaster raster = img.getRaster();

	for (int x = 0; x < img.getWidth(); x++) {
	    for (int y = 0; y < img.getHeight(); y++) {
		if ((img.getRGB(x, y) & 0x000000ff) > 0) {
		    Region region = new Region();           // start a new region
		    int ix = x, iy = y;
		    boolean stuck = false;
		    int dirIndex = 0;
		    int npoints = 0;
		    //		    System.out.print('\n' + "Region starting at {" + x + ", " + y + "}");
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
		    //		    System.out.println("Found new region that ends at (" + ix + ", " + iy + ") npoints = " + npoints);
		    if (region.size() >= minSize)           // if it's big enough 'to keep', return it; else keep looking
			return region;
		}
	    }
	}

	return null;      // we reach this point iff the thresholded image is blank -- this is, all regions found & reported (and erased)
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

    //    public enum HoopLocation { unknown, left, top, right, bottom };

    public void labelHoops() {

	Region highest = null, lowest = null, rightmost = null, leftmost = null;

	// Pick out the top 4 (largest, in this case) regions as candidate hoops.
	// Looking for equality of size might be better; dunno.

	Region [] sorted = new Region [this.regions.size()];

	// fill array with regions for analysis.  We need to add logic to pick "the best 4", rather than the first.


	for (int i = 0; i < this.regions.size(); i++) {
	    sorted[i] = this.regions.elementAt(i);
	}

	// Pick largest four regions (measured by area of their enclosig rectangle)
	quickSortByArea(sorted, 0, this.regions.size() - 1);

	int nregions = Math.min(4, this.regions.size());

	// note that y dimension is flipped; {0, 0} pixel index sits in upper left corner of the image

	for (int i = 0; i < nregions; i++) {
	    Region region = sorted[i];
	    if ((highest == null) || (region.enclosingRectangle.y < highest.enclosingRectangle.y))
		highest = region;
	    if ((lowest == null) || (region.enclosingRectangle.y + region.enclosingRectangle.height > lowest.enclosingRectangle.y + lowest.enclosingRectangle.height))   
	        lowest = region;
	    if ((rightmost == null) || (region.enclosingRectangle.x + region.enclosingRectangle.width > rightmost.enclosingRectangle.x + rightmost.enclosingRectangle.width))
		rightmost = region;
	    if ((leftmost == null) || (region.enclosingRectangle.x < leftmost.enclosingRectangle.x))
		leftmost = region;
	}

	//	System.out.println("labelRegiones: rightmost = " + rightmost + "leftmost = " + leftmost + " highest = " + highest + " lowest " + lowest);

	// This could use some add'l logic to sort out bad regions, and insure that we're looking at no more than 4 hoops
	// (in case of false detection)

	switch (nregions) {

	case 1:             
	    // if we see only one region, we have no way to know which it is
	    sorted[0].hoopLocation = Region.HoopLocation.unknown;
	    break;

	case 2:
	    // if same height, must be side-by-side.  Determine which is left v right.
	    if (sorted[0].sameElevation(sorted[1])) {
		leftmost.hoopLocation = Region.HoopLocation.left;
		rightmost.hoopLocation = Region.HoopLocation.right;
	    } else if (sorted[0].sameAzimuth(sorted[1])) {
		highest.hoopLocation =  Region.HoopLocation.top;
		lowest.hoopLocation =  Region.HoopLocation.bottom;
	    }
	    else { // we can't tell -- ambiguous between (left & bottom) and (top and right)
		sorted[0].hoopLocation = sorted[1].hoopLocation = Region.HoopLocation.unknown;
	    }
	    break;

	case 3:
	    if (highest.isAbove(leftmost) && highest.isAbove(rightmost)) {
		highest.hoopLocation = Region.HoopLocation.top;
		if (leftmost.isLeftOf(highest))
		    leftmost.hoopLocation = Region.HoopLocation.left;
		else if (leftmost.sameAzimuth(highest))
		    leftmost.hoopLocation = Region.HoopLocation.bottom;
		if (rightmost.isRightOf(highest))
		    rightmost.hoopLocation = Region.HoopLocation.right;
		else if (rightmost.sameAzimuth(highest))
		    rightmost.hoopLocation = Region.HoopLocation.bottom;
	    }
	    else if (highest.isLeftOf(lowest)) {
		highest.hoopLocation = Region.HoopLocation.left;
		lowest.hoopLocation = Region.HoopLocation.bottom;
		rightmost.hoopLocation = Region.HoopLocation.right;
	    }
	    else if (highest.isRightOf(lowest)) {
		highest.hoopLocation = Region.HoopLocation.right;
		lowest.hoopLocation = Region.HoopLocation.bottom;
		leftmost.hoopLocation = Region.HoopLocation.left;
	    }
	    else if (highest.sameAzimuth(lowest)) {
		highest.hoopLocation = Region.HoopLocation.top;
		lowest.hoopLocation = Region.HoopLocation.bottom;
		if (rightmost.isRightOf(highest)) 
		    rightmost.hoopLocation = Region.HoopLocation.right;
		else if (leftmost.isLeftOf(highest))
		    leftmost.hoopLocation = Region.HoopLocation.left;
	    }
	    else System.err.println("Bad labeling logic!");
	    break;

	case 4:
	    highest.hoopLocation = Region.HoopLocation.top;
	    lowest.hoopLocation = Region.HoopLocation.bottom;
	    leftmost.hoopLocation = Region.HoopLocation.left;
	    rightmost.hoopLocation = Region.HoopLocation.right;
	    break;
	}
    }


    //Quick Sort code for JAVA, by Yash Gupta
    // (Rebuilt by Arnold to sort class type dataPoint

private int partition(Region arr[], int left, int right){
	int i = left, j = right;

	Region tmp;
	double pivot = arr[(left + right) / 2].getArea();
	while (i <= j)
	    {
		while (arr[i].getArea() > pivot)
		    i++;
		while (arr[j].getArea() < pivot)
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


    // Sorts Regions by area by area, descending.

    private void quickSortByArea(Region arr[], int left, int right)  {

	if (right <= left)        //  watch out for  case of zero-length vector
	    return;

	int index = partition(arr, left, right);
	if (left < index - 1)
            quickSortByArea(arr, left, index - 1);
	if (index < right)
            quickSortByArea(arr, index, right);
    }

    public void drawRegions(BufferedImage cimage) {

	for (int ir = 0; ir < this.regions.size(); ir++) {
	    this.regions.elementAt(ir).drawEnclosingPolygon(cimage);
	    this.regions.elementAt(ir).drawEnclosingRectangle(cimage);
	}
    }

    public Region getHoop(Region.HoopLocation hl) {
	for (int i = 0; i < regions.size(); i++) {
	    Region r = regions.elementAt(i);
	    if (r.hoopLocation == hl)
		return r;
	}
	return null;
    }

    // find the best hoop for estimating range (highest).  At this point we rely on hoops that
    // are completely visible -- no occlusions, and thus with enclosing polygons.  We can relax
    // this to include the visible tops or bottoms of hoops (FOR LATER)

    public Region getHighestVisibleHoop() {
	Region hoop = getTopHoop();
	if ((hoop != null) && (hoop.topEdge != null))
	    return (hoop);
	hoop = getRightHoop();
	if ((hoop != null) && (hoop.topEdge != null))
	    return (hoop);
	hoop = getLeftHoop();
	if ((hoop != null) && (hoop.topEdge != null))
	    return (hoop);
	hoop = getBottomHoop();
	if ((hoop != null) && (hoop.topEdge != null))
	    return (hoop);

	return null;
    }


    public Region getTopHoop()	{
	return (getHoop(Region.HoopLocation.top));
    }

    public Region getLeftHoop()	{
	return (getHoop(Region.HoopLocation.left));
    }

    public Region getRightHoop()	{
	return (getHoop(Region.HoopLocation.right));
    }

    public Region getBottomHoop()	{
	return (getHoop(Region.HoopLocation.bottom));
    }

    public Region getUnknownHoop()	{
	return (getHoop(Region.HoopLocation.unknown));
    }



    public String toString() {
	String str = "";
	for (int i = 0; i < regions.size(); i++) {
	    str = str + " " + i + ":  " + regions.elementAt(i) + '\n';
	}
	return str;
    }
}
