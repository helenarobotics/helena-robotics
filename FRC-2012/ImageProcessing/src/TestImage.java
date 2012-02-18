
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

    static int width = 320; 
    static int height = 240;
    static BufferedImage img;

    public static void main(String args[]) {


	img = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY); 

	// Top rectangle
	drawrect(width/2 - 40, 20, 80, 60);

	// lower rectangle
	//	drawrect(width/2 - 40, height - 20 - 60, 80, 60);

	try {
	    ImageIO.write(img, "jpg", new File("testfile.jpg"));
	} catch (IOException e) {
	    e.printStackTrace();
	    System.exit(-1);
	}
	System.out.println("wrote test file");
    }


    static void drawrect(int x0, int y0, int width, int height) {

	WritableRaster o = img.getRaster();

	// draw sides of a rectangle
	// vertical lines
	for (int y = y0; y < y0 + height; y++){
	    writePixel(o, x0, y, 255);
	    writePixel(o, x0 + width, y, 255);
	}

	// horizontal lines
	for (int x = x0; x < x0 + width; x++){
	    writePixel(o, x, y0, 255);
	    writePixel(o, x, y0+ height, 255);
	}
    }
    
    static void writePixel(WritableRaster o, int x, int y, int val) {
	if ((x >=0) && (x < width) && (y >= 0) && (y < height))
	    o.setSample(x, y, 0, 255);
    }
}
