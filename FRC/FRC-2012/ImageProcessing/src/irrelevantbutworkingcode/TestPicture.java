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


public class TestPicture {

    static int width = 320; 
    static int height = 240;
    static BufferedImage img;

    public static void main(String args[]) {


	img = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY); 

	int rw = 40;
	int rh = (rw*3)/4;

	//	drawline(100, height/2, rw);
	//drawline(200, height/2, rw);
	//drawline(300, height/2, rw);
	//drawline(400, height/2, rw);

	//drawline(120, height/2+30, rw);
	//	drawline(180, height/2+30, rw);

	drawline(180, 50, 0.2, rw);
	drawline(180, 125, 0.3, rw);
	drawline(180, 200, 0.3, rw);

	drawline(20, 200, 0.8, rw);
	drawline(20, 50, 0.7, rw);

	// Top-center rectangle
	//	drawrect((width - rw)/2, rh/2, rw, rh);


	// lower-center rectangle
        //drawrect((width - rw)/2, height - rh - rh/2, rw, rh);

	// left-middle rectangle
        //drawrect(rw, (height - rh)/2, rw, rh);

	// right-middle rectangle
        //drawrect(width - 2*rw, (height - rh)/2, rw, rh);

	try {
	    ImageIO.write(img, "jpg", new File("testfile.jpg"));
	} catch (IOException e) {
	    e.printStackTrace();
	    System.exit(-1);
	}
	System.out.println("wrote test file");
    }


    static void drawline(int x0, int y0, double yslant, int width) {
	WritableRaster o = img.getRaster();

	// vertical lines

	for (int x = x0; x < x0+width; x++) {
	    int y = y0 + (int)(((double)(x - x0)) * yslant);
	    writePixel(o, x, y, 255);
	    System.out.print("{" + x + ", " + y + "} ");
	}
    }


    static void drawrect(int x0, int y0, int width, int height) {

	WritableRaster o = img.getRaster();

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
