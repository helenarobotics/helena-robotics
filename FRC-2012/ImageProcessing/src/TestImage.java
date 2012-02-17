
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

import javax.imageio.ImageIO;


public class TestImage {

    public static void main(String args[]) {

	int width = 320; 
	int height = 240;

	BufferedImage img = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY); 

	WritableRaster o = img.getRaster();
	int [] buffer = new int [1];
	int [] pixel = new int [1];

	// Draw a box.
	int xleft = width/2 - 40;
	int xright = width/2 + 40;
	int ytop = height/2 + 30;
	int ybottom = height/2 - 30;
	//	double slant = (double) (xend - xstart) / (double) (yend - ystart);

	// draw sides of a rectangle
	for (int y = ybottom; y <= ytop; y++){
	    o.setSample(xleft, y, 0, 255);
	    o.setSample(xleft+1, y, 0, 255);
	    o.setSample(xright, y, 0, 255);
	    o.setSample(xright+1, y, 0, 255);
	}

	for (int x = xleft; x < xright; x++){
	    o.setSample(x, ybottom, 0, 255);
	    o.setSample(x, ybottom+1, 0, 255);
	    o.setSample(x, ytop, 0, 255);
	    o.setSample(x, ytop+1, 0, 255);
	}

	try {
	    ImageIO.write(img, "jpg", new File("testfile.jpg"));
	} catch (IOException e) {
	    e.printStackTrace();
	    System.exit(-1);
	}
	System.out.println("wrote test file");
    }
}
