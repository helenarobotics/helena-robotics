import java.io.*;
import java.io.IOException;
import java.awt.image.BufferedImage;
import java.awt.image.BufferedImageOp;
import java.awt.image.ColorModel;
import java.awt.image.ConvolveOp;
import java.awt.image.Kernel;
import java.awt.image.WritableRaster;
import java.awt.image.Raster;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.util.Vector; 
import javax.imageio.ImageIO;
import java.awt.geom.*;
import java.awt.Color;
import java.awt.*;



public class testIU {
    public static void main(String args[]) {
        if (args.length != 2) {
            System.err.println("requires two arguments: inputdirname outputdirname");
            System.exit(-1);
        }

	// Get the input directory, list the files it contains
	String indir = args[0];
	File dir = new File(indir);
	String[] children = dir.list();

	if (children == null) {
	    System.err.println("Can't access input directory " + args[0]);
	    System.exit (-2);       // punt
	} else {

	    // Filter files on *.jpg file extension
	    FilenameFilter filter = new FilenameFilter() {
		    public boolean accept(File dir, String name) {
			return name.endsWith(".jpg");
		    }
		};
	    children = dir.list(filter);
	    String outdir = args[1];
	    String slash = System.getProperty("file.separator");

	    // Set up simple queues for sending images, and returning results (RegionGrow data) from image understanding thread
	    ImageQueue iq = new ImageQueue();
	    DataQueue dq = new DataQueue();

	    // Create image understanding object, and begin its async processing thread; last param, 2, = desampling factor
	    ImageUnderstanding iu = new ImageUnderstanding(iq, dq, 1);

	    Thread t = new Thread(iu);
	    t.start();

	    for (int i=0; i<children.length; i++) {
		// Get filename of file or directory
		String filename = children[i];
		String filepath = indir+ slash + filename;
		try {
		    System.out.println("processing file '" + filename + "'");

		    BufferedImage image = ImageIO.read(new File(filepath));
		    iq.put(image);

		    // get the results.  Returns null if not ready (from any previous image, not just most recent)

		    ImageResults results = null;

		    int n = 0;

		    while ((results = dq.get()) == null) {
			try {
			    Thread.currentThread().sleep(10);  // sleep 10 msec
			} catch (InterruptedException e) {
			    e.printStackTrace();
			}
		    }

		    System.out.println(results);    // debug: report out regions we found

		    //draw bounding rectangles, polygons
		    results.drawRegions(image);

		    // draw crosshairs -- location we expect ball to hit at current thrower wheel speed and distance
		    // (just for demonstration purposes now -- we need to calculate actual distance and thrower speed)
		    drawCrosshairs(image, image.getWidth()/2.0, image.getHeight() * 0.30);

		    // Write image with box overlays
		    ImageIO.write(image, "jpg", new File(outdir + slash + filename.substring(0, filename.length()-4) + "-overlay.jpg"));

		    // just for debugging, also look at thresholded image
		    ImageIO.write(results.thresholdedImage, "jpg", new File(outdir + slash + filename.substring(0, filename.length()-4) + "-thresholded.jpg"));

		} catch (IOException e) {
		    e.printStackTrace();
		}
	    }

	    iu.stop();        // set a flag to stop at the next appropriate moment.
	}
    }

    static void drawCrosshairs(BufferedImage image, double x, double y) {
	double scale = Math.min(image.getWidth(), image.getHeight()) / 15.0;
	double d = scale * 1.5;

	Graphics2D g2 = image.createGraphics();
	g2.setColor(Color.red);

	Shape circle = new Ellipse2D.Double(x - scale, y - scale, 2.0*scale, 2.0*scale);
	g2.draw(circle);
	Line2D lin1 = new Line2D.Double(x - d, y, x + d, y);
        g2.draw(lin1);
	Line2D lin2 = new Line2D.Double(x, y-d, x, y+d);
        g2.draw(lin2);
    }
}

