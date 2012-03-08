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
	    System.exit (-2);
	} else {

	    // Filter files on *.jpg
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
		    EdgeDetect edges = new EdgeDetect(image, 80);
		    RegionGrow RG = new RegionGrow(edges.thresholdedImage, 2, 200);
		    System.out.println(RG);

		    // Create color image from original input image.  We'll overlay HT lines and line segments.
		    BufferedImage cimage = new BufferedImage(image.getWidth(), image.getHeight(),  BufferedImage.TYPE_INT_RGB);
		    Graphics g = cimage.getGraphics();  
		    g.drawImage(image, 0, 0, null);

		    //draw bounding rectangles
		    if (RG.regions != null) {
			Graphics2D g2 = cimage.createGraphics();
			for (int ir = 0; ir < RG.regions.size(); ir++) {
			    Region region = RG.regions.elementAt(ir);
			    Polygon p = new Polygon();
			    Rectangle r = region.enclosingRectangle();
			    p.addPoint(r.x, r.y);
			    p.addPoint(r.x + r.width, r.y);
			    p.addPoint(r.x + r.width, r.y + r.height);
			    p.addPoint(r.x, r.y + r.height);
			    g2.drawPolygon(p);

			    Polygon p2 = region.enclosingPolygon(edges.thresholdedImage);
			    if (p2 != null)
				g2.drawPolygon(p2);
			    else System.out.println("Igorning substantially occluded hoop");
			}
			// Write image with box overlays
			ImageIO.write(cimage, "jpg", new File(outdir + slash + filename.substring(0, filename.length()-4) + "-overlay.jpg"));
		    }
		    //		    ImageIO.write(edges.thresholdedImage, "jpg", new File(filename + "-thresholded.jpg"));
		} catch (IOException e) {
		    e.printStackTrace();
		}
	    }
	}
    }

 static   short [][] image2short(BufferedImage image) {
	short [][] raw = new short [image.getWidth()][image.getHeight()];
	
        int [] buffer = new int [1];
	int [] pixel = new int [1];

	for (int x = 0; x < image.getWidth(); x++) {
	    for (int y = 0; y < image.getHeight(); y++) {
	        raw[x][y] = (short) (image.getRGB(x, y) & 0x000000ff);
	    }
	}
	return raw;
    }


    // End of class..
}