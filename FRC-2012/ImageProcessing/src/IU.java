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



public class IU {
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

	    for (int i=0; i<children.length; i++) {
		// Get filename of file or directory
		String filename = children[i];
		String filepath = indir+ slash + filename;
		try {
		    System.out.println("processing file '" + filename + "'");

		    BufferedImage image = ImageIO.read(new File(filepath));

		    // This constructor does all the image processing work: color match, threshold, region growing, and region boundary analysis
		    RegionGrow RG = new RegionGrow(image, 80, 200);     // 80 = image threshold, 200 = min region size (in pixels)
		    System.out.println(RG);    // debug: report out regions we found

		    // Create color image from original input image.  We'll overlay region boundaries, crosshairs, etc
		    BufferedImage cimage = new BufferedImage(image.getWidth(), image.getHeight(),  BufferedImage.TYPE_INT_RGB);
		    Graphics g = cimage.getGraphics();  
		    g.drawImage(image, 0, 0, null);

		    //draw bounding rectangles, polygons
		    RG.drawRegions(cimage);

		    // draw crosshairs -- location we expect ball to hit at current thrower wheel speed and distance
		    // (just for demonstration purposes now -- we need to calculate actual distance and thrower speed)
		    drawCrosshairs(cimage, cimage.getWidth()/2.0, cimage.getHeight() * 0.30);

		    // Write image with box overlays
		    ImageIO.write(cimage, "jpg", new File(outdir + slash + filename.substring(0, filename.length()-4) + "-overlay.jpg"));
		    ImageIO.write(RG.thresholdedImage, "jpg", new File(filename.substring(0, filename.length()-4) + "-thresholded.jpg"));

		} catch (IOException e) {
		    e.printStackTrace();
		}
	    }
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

