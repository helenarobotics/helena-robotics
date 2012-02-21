import java.awt.image.BufferedImage; 
import java.util.Vector;  
import java.awt.geom.*;
import java.awt.image.Raster;

/** 
 * Represents a linear line as detected by the hough transform. 
 * This line is represented by an angle theta and a radius from the centre. 
 * 
 * @author Olly Oechsle, University of Essex, Date: 13-Mar-2008 
 * @version 1.0 
 */ 
public class HoughLine { 
 
    protected double theta; 
    protected int itheta;
    protected int r; 
    protected int peak;
 
    /** 
     * Initialises the hough line 
     */ 
    /*    public HoughLine(double theta, int itheta, double r) { 
        this.theta = theta; 
        this.r = r; 
	peak = 0;
    } 
    */

    public HoughLine(double _theta, int _itheta, int _r, int _peak) { 
        this.theta = _theta; 
	this.itheta = _itheta;
        this.r = _r; 
	this.peak = _peak;
    } 

    /** 
     * Segments hough lines based on image pixel intensity 
     * (we'll want to also write a version that works with pixel int array, rather than image, to avoid jpeg loss-induced errors)
     * 
     * image = original (or grayscale) camera image
     * w = window width
     * d = # cells within window that most be 'lit' to continue line segment
     */ 

    public Vector<Line2D.Double> segment(BufferedImage image, int w, int d) { 
 
        int height = image.getHeight(); 
        int width = image.getWidth(); 

	Raster raster = image.getData();

       // We return a list of line segments associated with this hough line.
	Vector<Line2D.Double> segments = new Vector<Line2D.Double>(2);

        // During processing h_h is doubled so that -ve r values 
        int houghHeight = (int) (Math.sqrt(2) * Math.max(height, width)) / 2; 
 
        // Find edge points and vote in array 
        float centerX = width / 2; 
        float centerY = height / 2; 
 
        // Draw edges in output array 
        double tsin = Math.sin(theta); 
        double tcos = Math.cos(theta); 

	// image read memory (we'll get rid of this once we move to standard java arrays)
	int buffer [] = new int [1];
	int pixel [] = new int [1];

	// moving window equipment:
	boolean window [] = new boolean [w+1];

	boolean started = false;
	Line2D.Double l = new Line2D.Double();    // line segment
 
	int x = 0, y = 0;

        if (theta < Math.PI * 0.25 || theta > Math.PI * 0.75) { 
            //  follow vvertical-ish lines 
            for (y = 0; y < height; y++) { 
                 x = (int) ((((r - houghHeight) - ((y - centerY) * tsin)) / tcos) + centerX); 
		//		System.out.print("[" + x + ", " + y + "] =");
                if (x < width && x >= 0) { 
		    pixel = raster.getPixel(x, y, buffer);
		    //		    System.out.print("=" + (pixel[0] & 0x000000ff) + " ");
		    if ((pixel[0] & 0x000000ff) > 200) {
			window[w] = true;
		    }
		    else {
			window[w] = false;
		    }
		}
		else window[w] = false;

		// Slide window, count 'lit' pixels, and start or stop line segment growth depending on whether we meet
		// minimum criteria (at least d of w pixels lit in moving window)
		int count = 0;
		for (int i = 0; i < w; i++) {
		    window[i] = window[i+1];
		    if (window[i])
			count = count + 1;
		    }
		if (count >= d) {
		    if (!started) {
			started = true;
			// we'll extend the end points when we find the end of the line.
			l.x1 = x; l.y1 = y;
			l.x2 = x; l.y2 = y;
			//			System.out.print("New segment at {" + x + ", " + y + "}");
		    }
		}
		else {
		    if (started) {
			started = false;
			segments.add(new Line2D.Double(l.x1, l.y1, x, y));
			// System.out.println("... ended at {" + x + ", " + y + "}");
		    }
		}
            }
        } else { 
            // Draw horizontal-sh lines 
            for (x = 0; x < width; x++) { 
                y = (int) ((((r - houghHeight) - ((x - centerX) * tcos)) / tsin) + centerY); 
		//		System.out.print("[" + x + ", " + y + "]");
                if (y < height && y >= 0) { 
		    pixel = raster.getPixel(x, y, buffer);
		    //		    System.out.print("=" + (pixel[0] & 0x000000ff) + " ");
		    if ((pixel[0] & 0x000000ff) > 200) {
			window[w] = true;
		    }
		    else {
			window[w] = false;
		    }
		}
		else window[w] = false;

		// Slide window, count 'lit' pixels, and start or stop line segment growth depending on whether we meet
		// minimum criteria (at least d of w pixels lit in moving window)
		int count = 0;
		for (int i = 0; i < w; i++) {
		    window[i] = window[i+1];
		    if (window[i])
			count = count + 1;
		    }
		if (count >= d) {
		    if (!started) {
			started = true;
			// we'll extend the end points when we find the end of the line.
			l.x1 = x; l.y1 = y;
			l.x2 = x; l.y2 = y;
			// System.out.print("New segment at {" + x + ", " + y + "}");
		    }
		}
		else {
		    if (started) {
			started = false;
			segments.add(new Line2D.Double(l.x1, l.y1, x, y));
			// System.out.println("... ended at {" + x + ", " + y + "}");
		    }
		}
            }
	}

	// in case we ran to the edge of the image w/o closing off an open line segment:
	if (started) {
	    System.out.println("Line ran off image: [{" + l.x1 + "," + l.y1 + "}, {" + l.x2 + "," + l.y2 + "}]");
	    segments.add(new Line2D.Double(l.x1, l.y1, x, y));
	}

	return segments;
    }

 
    /** 
     * Draws the line on the image of your choice with the RGB colour of your choice. 
     */ 
    public void draw(BufferedImage image, int color) { 
 
        int height = image.getHeight(); 
        int width = image.getWidth(); 
 
        // During processing h_h is doubled so that -ve r values 
        int houghHeight = (int) (Math.sqrt(2) * Math.max(height, width)) / 2; 
 
        // Find edge points and vote in array 
        float centerX = width / 2; 
        float centerY = height / 2; 
 
        // Draw edges in output array 
        double tsin = Math.sin(theta); 
        double tcos = Math.cos(theta); 
 
        if (theta < Math.PI * 0.25 || theta > Math.PI * 0.75) { 
            // Draw vertical-ish lines 
            for (int y = 0; y < height; y++) { 
                int x = (int) ((((r - houghHeight) - ((y - centerY) * tsin)) / tcos) + centerX); 
                if (x < width && x >= 0) { 
                    image.setRGB(x, y, color); 
                } 
            } 
        } else { 
            // Draw horizontal-sh lines 
            for (int x = 0; x < width; x++) { 
                int y = (int) ((((r - houghHeight) - ((x - centerX) * tcos)) / tsin) + centerY); 
                if (y < height && y >= 0) { 
                    image.setRGB(x, y, color); 
                } 
            } 
        } 
    }
} 

