import java.io.File;
import java.io.IOException;

import java.awt.image.BufferedImage;
import java.awt.image.BufferedImageOp;
import java.awt.image.ColorModel;
import java.awt.image.ConvolveOp;
import java.awt.image.Kernel;
import java.awt.image.WritableRaster;
import java.awt.image.Raster;
import java.awt.Graphics;
import java.util.Vector; 
import javax.imageio.ImageIO;
import java.awt.geom.*;

import java.awt.*;



public class IU {
    public static void main(String args[]) {
        if (args.length == 0) {
            System.err.println(
                "Provide image(s) to process as argument(s) to program");
            System.exit(-1);
        }

	int[][] thetas = {{0, 7}, {83, 97}, {173, 180}};
	int nthetas = 3;

	boolean success = true;

        for (int fileNum = 0; success && fileNum < args.length; fileNum++) {
            String imageFile = args[fileNum];
	    System.out.println("processing file '" + imageFile + "'");
            try {

		// time trial:
		for (int trial = 0; trial < 20; trial++) {

		BufferedImage image = ImageIO.read(new File(imageFile));

		//		System.out.println("Detecting edges...");
		EdgeDetect edges = new EdgeDetect(image, 150);

		// System.out.println("Creating HT...");
		HoughTransform h = new HoughTransform(image.getWidth(), image.getHeight(), thetas, nthetas);
		//		System.out.println("Transforming...");
		//		short [][] raw = image2short(image);
		//		h.addPoints(raw);
		//
		h.addPoints(edges.edgesImage);      // revert to image version of HT
		//		h.addPoints(edges.detected);      
		//		System.out.println("Segmenting...");

		// The following just for debugging:

		Vector<HoughLine> lines = h.getLines(20);  // XXX thresh was 30 
		//		BufferedImage himage = h.getHoughArrayImage();
		// ImageIO.write(himage, "jpg", new File("houghspace.jpg"));

		// Create a color image from the hough results, and highlight the peaks in red
		//BufferedImage chimage = new BufferedImage(h.maxTheta, h.doubleHeight,  BufferedImage.TYPE_INT_RGB);
		//Graphics gh = chimage.getGraphics();  
		//gh.drawImage(himage, 0, 0, null);
		//		gh.dispose(); 

		// Also create color image from original input image.  We'll overlay HT lines and line segments.
		BufferedImage cimage = new BufferedImage(image.getWidth(), image.getHeight(),  BufferedImage.TYPE_INT_RGB);
		Graphics g = cimage.getGraphics();  
		g.drawImage(image, 0, 0, null);
		g.dispose(); 
		Vector<Line2D.Double> totalSegments = new Vector<Line2D.Double>(30);

		for (int j = 0; j < lines.size(); j++) { 
		    HoughLine line = lines.elementAt(j); 

		    // Segment hough line into visible components:
		    // window of 15, of which 9 pixels must be 'lit'
		    //		    Vector<Line2D.Double> segments = line.segment(edges.detected, image.getWidth(), image.getHeight(), 15, 9, 10);  
		    //		    Vector<Line2D.Double> segments = line.segment(edges.edgesImage, 15, 9, 10);
		    Vector<Line2D.Double> segments = line.segment(edges.edgesImage, 15, 9, 10);    // Should this be performed on original image, or edges??

		    totalSegments.addAll(segments);
		    

		    line.draw(cimage, Color.BLUE.getRGB());

		    
		    //		    System.out.print(segments.size() + " segments found " + j + ": ");
		    for (int k = 0; k < segments.size(); k++) {
			Line2D.Double seg = segments.elementAt(k);
			//System.out.print(k + ": [{ " + seg.x1 + "," + seg.y1 + "}, {" + seg.x2 + "," + seg.y2 + "}] ");
			//			g.draw(edges.edgesImage, (int)seg.x1, (int)seg.y1, (int)seg.x2, (int)seg.y2);
			//			HoughLine.drawsegment(edges.edgesImage, seg, Color.RED.getRGB());
			HoughLine.drawsegment(cimage, seg, Color.RED.getRGB());
		    }


		    // draw out line on image (for debugging and presentation)

		    /*
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
		    */
		}
		// Write out markup'd images
		//ImageIO.write(cimage, "jpg", new File("houghout.jpg"));
		//		ImageIO.write(edges.edgesImage, "jpg", new File("houghoutedges.jpg"));
		//ImageIO.write(chimage, "jpg", new File("houghspaceoverlay.jpg"));
		  

		//	        Vector<Line2D.Double> segments = h.getLineSegments(edges.detected, 20);   // threshold = min # points required along a line of integration to declare detection.

		System.out.println("Forming into boxes...");
		Boxes boxes = new Boxes(totalSegments);
		// System.out.println("Drawing boxes...");
		boxes.draw(cimage);
		//System.out.println("writing overlaid image...");
		ImageIO.write(cimage, "jpg", new File("process" + fileNum + ".jpg"));
		System.out.println(boxes.size() + " boxes found:");
		//System.out.println("");
		System.out.println(boxes);
		//System.out.println("");
		}
            } catch (IOException e) {
                e.printStackTrace();
		success = false;
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
}