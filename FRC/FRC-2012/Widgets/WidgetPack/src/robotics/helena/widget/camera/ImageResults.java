package robotics.helena.widget.camera;

import java.util.ArrayList;

import java.awt.Graphics2D;
import java.awt.Point;

import java.awt.image.BufferedImage;
import java.awt.image.WritableRaster;
import java.awt.image.Raster;
import javax.vecmath.Point3d;

//XXX This code has been modified to only use the single largest region.
public class ImageResults {
	ArrayList<Region> regions;
	
    // each Region corresponds to a detected hoop, and contains estimated location, range, etc
    Region regionRes = null;

    // robot {x, y, z} location, when available(depends on view of hoops)
    Point3d robotPosition = null;

    BufferedImage thresholdedImage = null;

    // input processing parameters (set by constructor)
    int downsample;                    // 1 = no desampling, 2 = 1 / 2 size image, etc
    int nx, ny;                        // image size(after downsampling)
    int threshold;                     // pixel threshold for on / off after color detection
    int minSize;                       // minimum acceptable pixel count in grown regions(remove and ignore anything smaller)

    public ImageResults(BufferedImage image, int _downsample, int _threshold, int _minSize) {
        downsample = _downsample;
        minSize = _minSize;
        regions = new ArrayList<Region>(10);
        threshold = _threshold;

        nx = image.getWidth() / downsample;
        ny = image.getHeight() / downsample;
        
        {
        	System.out.println("hi");
        }

        // Create downsampled image (we can check for downsample == 1
        // condition, but checking the code now)
        BufferedImage cimage =
            new BufferedImage(nx, ny,  BufferedImage.TYPE_INT_RGB);

        Graphics2D g = cimage.createGraphics();
        g.drawImage(image, 0, 0, nx, ny, null);
        g.dispose();

        thresholdedImage = detectGreen(cimage, threshold);

        // Note that 'grow' zero's out pixels after they've been
        // grouped. We'll need the original later for geometry
        // calculations (polygon in particular), so create and use a
        // throw-away copy
        BufferedImage tmp = deepCopyGrayscale(thresholdedImage);

        Region region;
        while ((region = grow(tmp)) != null) {
            // calculates and saves rect and tight polygon boundaries, etc.
            region.finish(thresholdedImage);
            regions.add(region);
//            System.out.println("Found region " + region);
        }
        labelHoops();
        estimateRange();
        estimateRobotPosition();
        adjustForDownsample();
    }

    // rescales results data to fit original image (prior to desampling)
    private void adjustForDownsample() {
        for (Region r: regions) {
            // adjust the closing rectangle, if available
            if (r.enclosingRectangle != null) {
                r.enclosingRectangle.height *= downsample;
                r.enclosingRectangle.width  *= downsample;
                r.enclosingRectangle.x *= downsample;
                r.enclosingRectangle.y *= downsample;
            }

            // adjust the four sides, as available:
            if (r.leftEdge != null) {
                r.leftEdge.x1 *= downsample;
                r.leftEdge.y1 *= downsample;
                r.leftEdge.x2 *= downsample;
                r.leftEdge.y2 *= downsample;
            }

            if (r.rightEdge != null) {
                r.rightEdge.x1 *= downsample;
                r.rightEdge.y1 *= downsample;
                r.rightEdge.x2 *= downsample;
                r.rightEdge.y2 *= downsample;
            }

            if (r.topEdge != null) {
                r.topEdge.x1 *= downsample;
                r.topEdge.y1 *= downsample;
                r.topEdge.x2 *= downsample;
                r.topEdge.y2 *= downsample;
            }

            if (r.bottomEdge != null) {
                r.bottomEdge.x1 *= downsample;
                r.bottomEdge.y1 *= downsample;
                r.bottomEdge.x2 *= downsample;
                r.bottomEdge.y2 *= downsample;
            }

            // Adjust the four corners, as available
            if (r.leftTop != null) {
                r.leftTop.x *= downsample;
                r.leftTop.y *= downsample;
            }

            if (r.rightTop != null) {
                r.rightTop.x *= downsample;
                r.rightTop.y *= downsample;
            }

            if (r.leftBottom != null) {
                r.leftBottom.x *= downsample;
                r.leftBottom.y *= downsample;
            }

            if (r.rightBottom != null) {
                r.rightBottom.x *= downsample;
                r.rightBottom.y *= downsample;
            }
        }
    }

    public void estimateRange() {
	FieldGeometry.estimateRange(regionRes);
    }

    // Estimates robot {x, z} location based on range estimates made on
    // multiple hoop corners (generally across different hoops).
    public void estimateRobotPosition() {
        double maxBaseline = 0.0;
        HoopEstimate e1 = null, e2 = null;

        // find the widest available baseline to maximize accuracy (NB
        // we could also combine weighted estimates from multiple
        // baselines)
        for (int i = 0; i < regions.size(); i++) {
            Region ri = regions.get(i);
            for (int j = i; j < regions.size(); j++) {
                Region rj = regions.get(j);
                for (int rii = 0; rii < ri.estimates.size(); rii++) {
                    HoopEstimate eii = ri.estimates.get(rii);
                    for (int rjj = rii; rjj < rj.estimates.size(); rjj++) {
                        HoopEstimate ejj = rj.estimates.get(rjj);
                        // same approx height? (measured in inches!)
                        if (Math.abs(ejj.rangePoint.y - eii.rangePoint.y) < 1) {
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
//        System.out.println("e1 = " + e1);
//        System.out.println("e2 = " + e2);

        // Law of sines solution
        if ((e1 != null) && (e2 != null)) {
            double phi = Math.abs(e1.azimuth - e2.azimuth);
            double lambda = Math.asin((e1.range * Math.sin(phi)) / maxBaseline);
            double zloc, yloc, xloc;
            if (e1.range > e2.range) {
                lambda = Math.PI - lambda;
                xloc = e2.rangePoint.x + e2.floorRange * Math.cos(lambda);
            } else {
                // distance left(neg) or right(pos) of center of hoop wall
                xloc = e2.rangePoint.x - e2.floorRange * Math.cos(lambda);
            }
            // distance from hoop wall
            zloc = e2.rangePoint.z + e2.floorRange * Math.sin(lambda);
            // vertical height
            yloc = CameraModel.cameraHeight;
            System.out.println(
                "Robot located at {" + (int)xloc + ", " + (int)zloc + "}; " +
                "phi=" + (int) Math.toDegrees(phi) +
                ", lambda=" + (int)Math.toDegrees(lambda) +
                ", azi to center of baseline: " +
                (int)Math.toDegrees((e1.azimuth + e2.azimuth) / 2.0));
            robotPosition = new Point3d(xloc, yloc, zloc);
        } else
            robotPosition = null;
    }

    // detects FRC reflective tape.  Works remarkably well IN OUR
    // TESTING; need to verify onsite at competition, and adjust as
    // required.
    private static BufferedImage detectGreen(BufferedImage img, int thresh) {
        BufferedImage green = new BufferedImage(img.getWidth(), img.getHeight(), BufferedImage.TYPE_BYTE_GRAY);
        WritableRaster raster = green.getRaster();
        for (int x = 0; x < img.getWidth(); x++) {
            for (int y = 0; y < img.getHeight(); y++) {
                int rgb[] = getPixelData(img, x, y);
                int pixelValue = rgb[1] - (rgb[0] + rgb[2]) / 2;
                //   if (pixelValue > 255) pixelValue = 255;
                // else if (pixelValue < 0) pixelValue = 0;
                if (pixelValue >= thresh)
                    pixelValue = 255;
                else
                    pixelValue = 0;
                raster.setSample(x, y, 0, pixelValue);
            }
        }
        return (green);
    }

    private static int[] getPixelData(BufferedImage img, int x, int y) {
        int argb = img.getRGB(x, y);

        int rgb[] = new int[] {
            (argb >> 16) &0xff, // red
            (argb >>  8) &0xff, // green
            (argb) &0xff  // blue
        };
        return rgb;
    }

    // works only on thresholded grayscale
    public Region grow(BufferedImage img) {
        int [][] dirs = {
            { -1, 1}, { 0, 1 }, { 1, 1 }, { 1, 0 },
            { 1, -1 }, { 0, -1 }, { -1, -1 }, { -1, 0 }
        };
        WritableRaster raster = img.getRaster();

        for (int x = 0; x < img.getWidth(); x++) {
            for (int y = 0; y < img.getHeight(); y++) {
                if ((img.getRGB(x, y) &0x000000ff) > 0) {
                    // start a new region
                    Region region = new Region();
                    int ix = x, iy = y;
                    boolean stuck = false;
                    int dirIndex = 0;
                    int npoints = 0;
//                    System.out.print('\n' + "Region starting at {" + x + ", " + y + "}");
                    while (!stuck) {
                        // XXX hack to avoid 1 infinite loop.
                        if (npoints > 100000)
                            return region;
                        if ((ix >= 0) && (ix < img.getWidth()) &&
                            (iy >= 0) && (iy < img.getHeight()) &&
                            ((img.getRGB(ix, iy) &0x000000ff) > 0)) {
                            region.add(new Point(ix, iy));
                            raster.setSample(ix, iy, 0, 0);
                            // always begin looking in NW direction
                            dirIndex = 0;
                            npoints++;
//                            if ((npoints % 10) == 0)
//                                System.out.println();
//                            System.out.print(" {" + ix + ", " + iy + "}");
                            ix += dirs[dirIndex][0];
                            iy += dirs[dirIndex][1];
                        } else {
                            // back up and try moving in a new direction
                            if (dirIndex < 7) {
                                ix -= dirs[dirIndex][0];
                                iy -= dirs[dirIndex][1];
                                dirIndex++;
                                ix += dirs[dirIndex][0];
                                iy += dirs[dirIndex][1];
                            } else {
                                // find a new start point
//                                System.out.print("\nStuck at (" + ix + ", " + iy + ") ");
                                stuck = true;
                                dirIndex = 0;
                                for (int ir = 0; ir < region.size(); ir++) {
                                    Point p = region.elementAt(ir);
//                                  System.out.print("(" + p.x + ", " + p.y + ") ");
                                    Point lit = litNeighbor(img, p.x, p.y);
                                    if (lit != null) {
//                                        System.out.println("[UNSTUCK AT (" + lit.x + ", " + lit.y + ")] ");
                                        ix = lit.x;
                                        iy = lit.y;
                                        stuck = false;
                                        break;
                                    }
                                }
                            }
                        }
                    }
//                    System.out.println("Found new region that ends at (" + ix + ", " + iy + ") npoints = " + npoints);
                    // if it's big enough 'to keep', return it; else
                    // keep looking
                    if (region.size() >= minSize)
                        return region;
                }
            }
        }
        // we reach this point iff the thresholded image is blank--this
        // is, all regions found &reported(and erased)
        return null;
    }

    static int neighborCount(BufferedImage img, int ix, int iy) {
        int count = 0;
        for (int dx = -1; dx <= 1; dx++) {
            int x = ix + dx;
            if ((x >= 0) && (x < img.getWidth())) {
                for (int dy = -1; dy <= 1; dy++) {
                    int y = iy + dy;
                    if ((y >= 0) && (y < img.getHeight())) {
                        // don't count center pixl
                        if (!((x == ix) && (y == iy))) {
                            if ((img.getRGB(x, y) &0x000000ff) > 0) {
//                                System.out.print("(" + x + "," + y + ")");
                                count++;
                            }
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
                        // don't count center pixel
                        if (!((x == ix) && (y == iy))) {
                            if ((img.getRGB(x, y) &0x000000ff) > 0){
                                return new Point(x, y);
                            }
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

        int [] buffer = new int [1];
        int [] pixel = new int [1];

        for (int x = 0; x < out.getWidth(); x++) {
            for (int y = 0; y < out.getHeight(); y++) {
                pixel = r.getPixel(x, y, buffer);
                int val = pixel[0];
                o.setSample(x, y, 0, val);
            }
        }
        return out;
    }

    public void labelHoops() {

        // Punt if no regions to process (the area comparison code below will
        // blow up (not an elegant solutions, but hey)
        if (this.regions.size() == 0)
            return;

        // Pick out the top (largest, in this case) region as
        // the candidate hoop.  Looking for equality of size might be
        // better; dunno.
        Region [] sorted = new Region [this.regions.size()];

        // fill array with regions for analysis.  We need to add logic
        // to pick "the best", rather than the first.
        for (int i = 0; i < this.regions.size(); i++) {
            sorted[i] = this.regions.get(i);
        }

        // Pick largest four regions (measured by area of their enclosig
        // rectangle)
        quickSortByArea(sorted, 0, this.regions.size() - 1);

        // min allowed region size
        double minAllowedArea = (nx * ny) * 2.0E-03;

		this.regions.clear();

		double h = sorted[0].getEnclosingRectangle().height;
		double w = sorted[0].getEnclosingRectangle().width;
		double aspect = w / h;

		if(sorted[0].getArea() >= minAllowedArea && aspect < 4.0 && aspect > 0.25){
			regionRes = sorted[0];
			regions.add(sorted[0]);
		}
    }

    // Quick Sort code for JAVA, by Yash Gupta
    // (Rebuilt by Arnold to sort class type dataPoint
    private int partition(Region arr[], int left, int right) {
        int i = left, j = right;

        Region tmp;
        double pivot = arr[(left + right) / 2].getArea();
        while (i <= j) {
            while (arr[i].getArea() > pivot)
                i++;
            while (arr[j].getArea() < pivot)
                j--;
            if (i <= j) {
                tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
                i++;
                j--;
            }
        }
        return i;
    }

    // Sorts Regions by area by area, descending.
    private void quickSortByArea(Region arr[], int left, int right)  {
        //  watch out for case of zero - length vector
        if (right <= left)
            return;

        int index = partition(arr, left, right);
        if (left < index - 1)
            quickSortByArea(arr, left, index - 1);
        if (index < right)
            quickSortByArea(arr, index, right);
    }

    public void drawRegions(BufferedImage cimage) {
        regionRes.drawEnclosingPolygon(cimage);
        regionRes.drawEnclosingRectangle(cimage);
    }

    public Region getHoop() {
		return regionRes;
    }
}
