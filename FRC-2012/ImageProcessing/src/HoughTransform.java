import java.awt.image.BufferedImage; 
import java.awt.*; 
import java.util.Vector; 
import java.io.File; 
import javax.imageio.ImageIO;
import java.io.IOException; 
import java.awt.image.Raster;
import java.awt.image.WritableRaster;
import java.awt.geom.*;

/** 
 * <p/> 
 * Java Implementation of the Hough Transform.<br /> 
 * Used for finding straight lines in an image.<br /> 
 * by Olly Oechsle 
 * </p> 
 * <p/> 
 * Note: This class is based on original code from:<br /> 
 * <a href="http://homepages.inf.ed.ac.uk/rbf/HIPR2/hough.htm">http://homepages.inf.ed.ac.uk/rbf/HIPR2/hough.htm</a> 
 * </p> 
 * <p/> 
 * If you represent a line as:<br /> 
 * x cos(theta) + y sin (theta) = r 
 * </p> 
 * <p/> 
 * ... and you know values of x and y, you can calculate all the values of r by going through 
 * all the possible values of theta. If you plot the values of r on a graph for every value of 
 * theta you get a sinusoidal curve. This is the Hough transformation. 
 * </p> 
 * <p/> 
 * The hough tranform works by looking at a number of such x,y coordinates, which are usually 
 * found by some kind of edge detection. Each of these coordinates is transformed into 
 * an r, theta curve. This curve is discretised so we actually only look at a certain discrete 
 * number of theta values. "Accumulator" cells in a hough array along this curve are incremented 
 * for X and Y coordinate. 
 * </p> 
 * <p/> 
 * The accumulator space is plotted rectangularly with theta on one axis and r on the other. 
 * Each point in the array represents an (r, theta) value which can be used to represent a line 
 * using the formula above. 
 * </p> 
 * <p/> 
 * Once all the points have been added should be full of curves. The algorithm then searches for 
 * local peaks in the array. The higher the peak the more values of x and y crossed along that curve, 
 * so high peaks give good indications of a line.  * </p> 
 * 
 * @author Olly Oechsle, University of Essex 
 */ 
public class HoughTransform extends Thread { 
 
    public static void main(String[] args) throws Exception { 
        String filename = args[0];
 
        // load the file using Java's imageIO library 
        BufferedImage image = javax.imageio.ImageIO.read(new File(filename)); 
 
        // create a hough transform object with the right dimensions 
        HoughTransform h = new HoughTransform(image.getWidth(), image.getHeight()); 
 
        // add the points from the image (or call the addPoint method separately if your points are not in an image 
        h.addPoints(image); 

	// ImageIO.write(h.enhance(7), "jpg", new File("blurredHT.jpg"));
 
        // get the lines out 
	int thresh = 30;
	Vector<HoughLine> lines = h.getLines(thresh);  // XXX thresh was 30 

	System.out.println("Threshold = " + thresh);
 
	System.out.println("Found " + lines.size() + "lines above threshold " + thresh);


	// Create and write out hough space image (theta, r)
	BufferedImage himage = h.getHoughArrayImage();
	ImageIO.write(himage, "jpg", new File("houghspace.jpg"));

	// Create a color image from the hough results, and highlight the peaks in red
	BufferedImage chimage = new BufferedImage(h.maxTheta, h.doubleHeight,  BufferedImage.TYPE_INT_RGB);
	Graphics gh = chimage.getGraphics();  
	gh.drawImage(himage, 0, 0, null);
	gh.dispose(); 

	// Also create color image from original input image.  We'll overlay HT lines and line segments.
	BufferedImage cimage = new BufferedImage(image.getWidth(), image.getHeight(),  BufferedImage.TYPE_INT_RGB);
	Graphics g = cimage.getGraphics();  
	g.drawImage(image, 0, 0, null);
	g.dispose(); 

	Vector<Line2D.Double> totalSegments = new Vector<Line2D.Double>(30);

        // segment lines, draw the lines back onto the image
        for (int j = 0; j < lines.size(); j++) { 
            HoughLine line = lines.elementAt(j); 

	    // Segment hough line into visible components:
	    Vector<Line2D.Double> segments = line.segment(image, 15, 9);  // window of 15, of which 9 pixels must be 'lit'

	    totalSegments.addAll(segments);

            line.draw(cimage, Color.BLUE.getRGB());

	    System.out.print(segments.size() + " segments found " + j + ": ");
	    for (int k = 0; k < segments.size(); k++) {
	        Line2D.Double seg = segments.elementAt(k);
		System.out.print(k + ": [{ " + seg.x1 + "," + seg.y1 + "}, {" + seg.x2 + "," + seg.y2 + "}] ");
		//g.draw(cimage, (int)seg.x1, (int)seg.y1, (int)seg.x2, (int)seg.y2);
		HoughLine.drawsegment(cimage, seg, Color.RED.getRGB());
	    }
	    System.out.println();


	    // draw out line on image (for debugging and presentation)

	    System.out.println(j + ": " + line.peak + " [" + (int)((180/Math.PI)*line.theta) + ", " + line.r + "]");
	    System.out.println("Neighborhood:");
	    for (int t = -2; t <=2; t++) {
		for (int r = -2; r <= 2; r++)
		    {
			int it = t + line.itheta;
			int ir = r + line.r;
			if (it < 0) it = h.maxTheta + it;
			if (it >= h.maxTheta) it = it - h.maxTheta;
			if (ir < 0) ir = h.doubleHeight + r;
			if (ir >= h.doubleHeight) ir = ir - h.doubleHeight;
			chimage.setRGB(it, ir, Color.RED.getRGB());
			System.out.print(h.houghArray[it][ir] + " ");
		    }
		System.out.println("");
	    }
	    System.out.println("");
	}

	// Try organizing segments into boxes:
	System.out.println("Organizing " + totalSegments.size() + " segments into boxes");
	Boxes boxes = new Boxes(totalSegments);
	System.out.println("");
	System.out.println(boxes.size() + " boxes found:");
	System.out.println("");
	System.out.println(boxes);
	System.out.println("");

	///  boxes.drawCorners(cimage);     // Draws all the corners, not just those assigned to boxes
	boxes.draw(cimage);     // Draws corners assigned to completed boxes (hoops)

	// Write out markup'd images
	ImageIO.write(cimage, "jpg", new File("houghout.jpg"));
	ImageIO.write(chimage, "jpg", new File("houghspaceoverlay.jpg"));
    }

 
    // The size of the neighbourhood in which to search for other local maxima 
    final int neighbourhoodSize = 5; 
 
    // How many discrete values of theta shall we check? 
    final int maxTheta = 180;
 
    // Using maxTheta, work out the step 
    final double thetaStep = Math.PI / maxTheta; 
 
    // the width and height of the image 
    protected int width, height; 
 
    // the hough array 
    protected int[][] houghArray; 
 
    // the coordinates of the centre of the image 
    protected int centerX, centerY; 
    //    protected float centerX, centerY; 
 
    // the height of the hough array 
    protected int houghHeight; 
 
    // double the hough height (allows for negative numbers) 
    protected int doubleHeight; 
 
    // the number of points that have been added 
    protected int numPoints; 
 
    // cache of values of sin and cos for different theta values. Has a significant performance improvement. 
    private double[] sinCache; 
    private double[] cosCache; 
 
    /** 
     * Initialises the hough transform. The dimensions of the input image are needed 
     * in order to initialise the hough array. 
     * 
     * @param width  The width of the input image 
     * @param height The height of the input image 
     */ 
    public HoughTransform(int width, int height) { 
 
        this.width = width; 
        this.height = height; 
 
        initialise(); 
 
    } 
 
    /** 
     * Initialises the hough array. Called by the constructor so you don't need to call it 
     * yourself, however you can use it to reset the transform if you want to plug in another 
     * image (although that image must have the same width and height) 
     */ 
    public void initialise() { 
 
        // Calculate the maximum height the hough array needs to have 
        houghHeight = (int) (Math.sqrt(2) * Math.max(height, width)) / 2; 
 
        // Double the height of the hough array to cope with negative r values 
        doubleHeight = 2 * houghHeight; 
 
        // Create the hough array 
        houghArray = new int[maxTheta][doubleHeight]; 
 
        // Find edge points and vote in array 
        centerX = width / 2; 
        centerY = height / 2; 
 
        // Count how many points there are 
        numPoints = 0; 
 
        // cache the values of sin and cos for faster processing 
        sinCache = new double[maxTheta]; 
        cosCache = new double[maxTheta];
        for (int t = 0; t < maxTheta; t++) { 
            double realTheta = t * thetaStep; 
            sinCache[t] = Math.sin(realTheta); 
            cosCache[t] = Math.cos(realTheta); 
        } 
    } 
 
    /** 
     * Adds points from an image. The image is assumed to be greyscale black and white, so all pixels that are 
     * not black are counted as edges. The image should have the same dimensions as the one passed to the constructor. 
     */ 
    public void addPoints(BufferedImage image) { 
 
	Raster r = image.getData();

        int [] buffer = new int [1];
	int [] pixel = new int [1];

        // Now find edge points and update the hough array 
        for (int x = 0; x < image.getWidth(); x++) { 
            for (int y = 0; y < image.getHeight(); y++) { 
		// avoid the JPEG error-filled pixels (we anticipate thresholded values of 255)
		//		pixel = r.getPixel(x, y, buffer);
		//if ((pixel[0] & 0x000000ff) == 255)
		if ((image.getRGB(x, y) & 0x000000ff) > 240) {      
                    addPoint(x, y);
		    //System.out.println("point [" + x + ", " + y + "] = " + (image.getRGB(x, y) & 0x000000ff));
		}
	    }
	}
    } 
 
    /** 
     * Adds a single point to the hough transform. You can use this method directly 
     * if your data isn't represented as a buffered image. 
     */ 
    public void addPoint(int x, int y) { 
        // Go through each value of theta 
        for (int t = 0; t < maxTheta; t++) { 
 
            //Work out the r values for each theta step 
	    //int r = (int) (((x - centerX) * cosCache[t]) + ((y - centerY) * sinCache[t]));
	    int r = (int) Math.round((((x - centerX) * cosCache[t]) + ((y - centerY) * sinCache[t])));
 
	    // XXX DEBUG
	    if (t == 0)
		System.out.println("{" + x + ", " + y + "} r = " + r);
            
	    // this copes with negative values of r 
            r += houghHeight; 
 
            if (r < 0 || r >= doubleHeight) continue; 
 
            // Increment the hough array 
            houghArray[t][r]++; 
	    numPoints++; 
	}
    }
 
    /*
    public void addPoint(int u, int v) {
	int x = u - centerX;
	int y = v - centerY;
	double dAng = Math.PI / maxTheta;

	for (int ai = 0; ai < maxTheta; ai++) {
	    double theta = dAng * ai;
	    double r = x * Math.cos(theta) + y * Math.sin(theta);
	    int ri =  (int) ((double)houghHeight + r);
	    if (ri >= 0 && ri < doubleHeight) {
		houghArray[ai][ri]++;
		numPoints++; 
	    }
	}
    }
    */


 
    /** 
     * Once points have been added in some way this method extracts the lines and returns them as a Vector 
     * of HoughLine objects, which can be used to draw on the 
     * 
     * @param percentageThreshold The percentage threshold above which lines are determined from the hough array 
     */ 


    public Vector<HoughLine> getLines(int threshold) { 

        // Initialise the vector of lines that we'll return 
        Vector<HoughLine> lines = new Vector<HoughLine>(20); 
 
        // Only proceed if the hough array is not empty 
        if (numPoints == 0) return lines; 
 
        // Search for local peaks above threshold to draw 
        for (int t = 0; t < maxTheta; t++) { 
        loop: 
            for (int r = neighbourhoodSize; r < doubleHeight - neighbourhoodSize; r++) { 
 
                // Only consider points above threshold 
                if (houghArray[t][r] > threshold) { 
 
                    int peak = houghArray[t][r]; 
 
                    // Check that this peak is indeed the local maxima 
                    for (int dx = -neighbourhoodSize; dx <= neighbourhoodSize; dx++) { 
                        for (int dy = -neighbourhoodSize; dy <= neighbourhoodSize; dy++) { 
                            int dt = t + dx; 
                            int dr = r + dy; 
                            if (dt < 0) {
				dt = dt + maxTheta;   // roll over to negative theta, and flip 'r' symmetrically over center point
				dr = doubleHeight - dr;
			    }
                            else if (dt >= maxTheta) {
				dt = dt - maxTheta; 
				dr = doubleHeight - dr;
			    }
			    if (dr >= doubleHeight) dr = dr - doubleHeight;
			    else if (dr < 0) dr = dr + doubleHeight;
			    int v = houghArray[dt][dr];
			    // Some ugly logic to avoid multiple detections of equal peaks in neighborhood
                            if (v > peak)
				    // found a bigger point nearby, skip 
				continue loop; 
			     else if (v == peak)   // avoid multiple detection of equal peaks in neighborhood; just take first
				 if ((dx < 0) | ((dx == 0) & (dy < 0)))
				    continue loop;
			}
		    } 
 
                    // calculate the true value of theta 
                    double theta = t * thetaStep; 
 
                    // add the line to the vector 
                    lines.add(new HoughLine(theta, t, r, peak)); 
                 } 
            } 
        } 
 
        return lines; 
    } 
 
    /** 
     * Gets the highest value in the hough array 
     */ 
    public int getHighestValue() { 
        int max = 0; 
        for (int t = 0; t < maxTheta; t++) { 
            for (int r = 0; r < doubleHeight; r++) { 
                if (houghArray[t][r] > max) { 
                    max = houghArray[t][r]; 
                } 
            } 
        } 
        return max; 
    } 
 
    /** 
     * Gets the hough array as an image, in case you want to have a look at it. 
     */ 
    public BufferedImage getHoughArrayImage() { 


        BufferedImage image = new BufferedImage(maxTheta, doubleHeight, BufferedImage.TYPE_BYTE_GRAY); 
	WritableRaster raster = image.getRaster();

        int max = getHighestValue();   // scale image to highest value
	System.out.println("creating " + maxTheta + " (theta) X" + doubleHeight + " (r) hough space image ");

        for (int t = 0; t < maxTheta; t++) { 
            for (int r = 0; r < doubleHeight; r++) { 
                double value = 254 * ((double) houghArray[t][r]) / max; 
                int v = 255 - (int) value; 
		raster.setSample(t, r, 0, v);
            } 
        } 
        return image;
    }


     BufferedImage enhance(int w) {

	int [][] blurred = new int[maxTheta][doubleHeight]; 

	 for (int t = 0; t < maxTheta; t++) {
	     for (int r = 0; r < doubleHeight; r++) {
		 int sum = 0;
		 for (int dx = -w; dx <= w; dx++) { 
		     for (int dy = -w; dy <= w; dy++) { 
			 int dt = t + dx; 
			 int dr = r + dy; 
			 if (dt < 0) dt = dt + maxTheta; 
			 else if (dt >= maxTheta) dt = dt - maxTheta; 
			 if (dr < 0) dr = dr + doubleHeight;
			 else if (dr >= doubleHeight) dr = dr - doubleHeight;
			 sum  = sum + houghArray[dt][dr];
		     }
		 }
		 blurred[t][r] = sum;
	     }
	 }
 
	 // Now adjust the individual cells according to neighborhood average:

	 int area2 = (2 * w + 1) * (2 * w + 1);
	 for (int t = 0; t < maxTheta; t++) {
	     for (int r = 0; r < doubleHeight; r++) {
		 int bl = blurred[t][r];
		 if (bl > 0) {
		     int hl = houghArray[t][r];
 		     houghArray[t][r] =  (hl * hl * area2) / bl;
		 }
		 else if (houghArray[t][r] > 0)
		     System.err.println("bad HT blur " + houghArray[t][r] + " [" + t + "][" + r + "], blur = " + bl);
	     }
	 }

        BufferedImage image = new BufferedImage(maxTheta, doubleHeight, BufferedImage.TYPE_BYTE_GRAY); 
	WritableRaster raster = image.getRaster();

	int bmax = 0;
      
	for (int t = 0; t < maxTheta; t++)
	    for (int r = 0; r < doubleHeight; r++)
		if (blurred[t][r] > bmax)
		    bmax = blurred[t][r];

	double scale = 255.0 / bmax;

	for (int t = 0; t < maxTheta; t++)
	    for (int r = 0; r < doubleHeight; r++) {
		int v = 255 - (int) (blurred[t][r] * scale);
		raster.setSample(t, r, 0, v);
	    }

	return image;
     }




	/*

	for (int t = 0; t < maxTheta; t++) { 
	    // seed summation.  Note that we wrap around the matrix in both r & t
	    int sum = 0;
	    for (int r = 0; r < wr/2; r++)
		sum = sum + houghArray[t][r];
	    for (int r = doubleHeight - wr/2; r < doubleHeight; r++)
		sum = sum + houghArray[t][r];
	    for (int r = 0; r < doubleHeight; r++) {
		int ir1 = r - wr/2;
		if (ir1 < 0) ir1 = doubleHeight + ir1;
		int ir2 = r + wr/2;
		if (ir2 >= doubleHeight) ir2 = ir2 - doubleHeight;
		sum = sum - houghArray[t][ir1] + houghArray[t][ir2];
		blurred[t][r] = sum;
	    }
	}
    
	for (int r = 0; r < doubleHeight; r++) {
		// seed summation
		int sum = 0;
		for (int t = 0; t < wt/2; t++)
		    sum = sum + blurred[t][r];
		for (int t = maxTheta - wt/2; t < maxTheta; t++)
		    sum = sum + blurred[t][r];
		for (int t = 0; t < maxTheta; t++) {
		    int it1 = t - wt/2;
		    if (it1 < 0) it1 = maxTheta + it1;
		    int it2 = t + wt/2;
		    if (it2 >= maxTheta) it2 = it2 - maxTheta;
		    sum = sum - blurred[it1][r] + blurred[it2][r];
		    // put the enhanced value back into our hough array, scale
		    if (sum > 0)
			houghArray[t][r] = houghArray[t][r] * houghArray[t][r] * wr * wt / sum;
		}
	}
     }
	*/

    /*
    public BufferedImage getHoughArrayImage() { 
        int max = getHighestValue(); 
        BufferedImage image = new BufferedImage(maxTheta, doubleHeight, BufferedImage.TYPE_INT_ARGB); 
        for (int t = 0; t < maxTheta; t++) { 
            for (int r = 0; r < doubleHeight; r++) { 
                double value = 255 * ((double) houghArray[t][r]) / max; 
                int v = 255 - (int) value; 
                int c = new Color(v, v, v).getRGB(); 

		image.setRGB(t, r, c); 
            } 
        } 
        return image;
    }
    */
}

