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


public class EdgeDetect {
    public BufferedImage image;
    public int width, height;
    public BufferedImage thresholdedImage;
    public BufferedImage edgesImage;

    public EdgeDetect(BufferedImage _image, int thresh) {

	image = _image;
	width =  image.getWidth();
	height = image.getHeight();
    

        // Convolve to find the edges

	Kernel sobelXKernR = new Kernel(3, 3, new float[] {
		-1.0f, 0.0f, 0.0f, 
		-2.0f, 0.0f, 2.0f, 
		-1.0f, 0.0f, 1.0f
	    });

        BufferedImageOp xopR = new ConvolveOp(sobelXKernR);

        Kernel sobelYKernD = new Kernel(3, 3, new float[] {
		-1.0f, -2.0f, -1.0f,
		0.0f, 0.0f, 0.0f, 
		1.0f, 2.0f, 1.0f, 
	    });
	BufferedImageOp yopD = new ConvolveOp(sobelYKernD);

	Kernel sobelXKernL = new Kernel(3, 3, new float[] {
		0.0f, 0.0f, -1.0f, 
		2.0f, 0.0f, -2.0f, 
		1.0f, 0.0f, -1.0f
	    });
        BufferedImageOp xopL = new ConvolveOp(sobelXKernL);

        Kernel sobelYKernU = new Kernel(3, 3, new float[] {
		1.0f, 2.0f, 1.0f, 
		0.0f, 0.0f, 0.0f, 
		-1.0f, -2.0f, -1.0f,
	    });
	BufferedImageOp yopU = new ConvolveOp(sobelYKernU);

	// convert to probability-of-backboard function (simplified for now):
	
	BufferedImage img = detectGreen(image, thresh);

	int h[] = histogram(image, 1);

	for (int i  = 0; i <= 255; i++) {
	    if ((i % 10) == 0)
		System.out.print(i + ": ");
	    System.out.print(h[i]);
	    if ((i % 10) != 9)
		System.out.print(", ");
	    else System.out.println();
	}
	System.out.println();

	System.out.println("Histogram in 32 overlapping buckets: ");
	for (int i  = 0; i < 32; i++) {
	    int jstart = Math.max(0, (i-1)*8);
	    int jstop = Math.min(256, (i+1)*8);
	    int sum = 0;
	    for (int j = jstart; j < jstop; j++) {
		sum = sum + h[j];
	    }
	    if ((i % 10) == 0) {
		System.out.println();
		System.out.print(" " + jstart + ": ");
	    }
	    System.out.print(sum/(jstop - jstart));
	    if ((i % 10) != 9)
		System.out.print(", ");
	}

	// Create a new image to store the convolved data.
	BufferedImage xImgR = deepCopy(img);
	BufferedImage yImgD = deepCopy(img);
	BufferedImage xImgL = deepCopy(img);
	BufferedImage yImgU = deepCopy(img);

	xopL.filter(img, xImgL);
	yopU.filter(img, yImgU);
	xopR.filter(img, xImgR);
	yopD.filter(img, yImgD);


	this.edgesImage = addGrayImages(xImgL, yImgU, thresh);

	/*
	this.detected = addAndthresholdImage(xImgL, yImgU, thresh);	
	BufferedImage raw = CreateEdgeImage();
	*/

	BufferedImage r = getColor(image, 0);
	BufferedImage g = getColor(image, 1);
	BufferedImage b = getColor(image, 2);

	this.thresholdedImage = img;    // save for public access

	try {
	    ImageIO.write(edgesImage, "jpg", new File("SumLU.jpg"));
	    ImageIO.write(img, "jpg", new File("Grayscale.jpg"));
    	    ImageIO.write(r, "jpg", new File("red.jpg"));
	    ImageIO.write(g, "jpg", new File("green.jpg"));
	    ImageIO.write(b, "jpg", new File("blue.jpg"));

	    // xopR.filter(img, xImgR);
	    //	yopD.filter(image, yImgD);

	    // Write out new image set
	    ImageIO.write(yImgU, "jpg", new File("yEdgeU.jpg"));
	    ImageIO.write(yImgD, "jpg", new File("yEdgeD.jpg"));
	    ImageIO.write(xImgL, "jpg", new File("xEdgeL.jpg"));
	    ImageIO.write(xImgR, "jpg", new File("xEdgeR.jpg"));
	} catch (IOException e) {
	    e.printStackTrace();
	}
    }

    

    private static BufferedImage detectGreen(BufferedImage img, int thresh) {

	 BufferedImage green = new BufferedImage(img.getWidth(), img.getHeight(), BufferedImage.TYPE_BYTE_GRAY);
	 WritableRaster raster = green.getRaster();

	 for (int x = 0; x < img.getWidth(); x++){
	     for (int y = 0; y < img.getHeight(); y++){
		 int rgb[] = getPixelData(img, x, y);
		 int pixelValue = rgb[1] - (rgb[0] + rgb[2])/2;
		 //		 if (pixelValue > 255) pixelValue = 255;
		 // else if (pixelValue < 0) pixelValue = 0;
		 if (pixelValue >= thresh)
		     pixelValue = 255;
		 else pixelValue = 0;
		 raster.setSample(x, y, 0, pixelValue);
	     }
	 }
	 return green;
    }


    private static BufferedImage getColor(BufferedImage img, int c) {
	
	BufferedImage out = new BufferedImage(img.getWidth(), img.getHeight(), BufferedImage.TYPE_BYTE_GRAY);
	WritableRaster raster = out.getRaster();
	
	for (int x = 0; x < img.getWidth(); x++){
	    for (int y = 0; y < img.getHeight(); y++){
		int rgb[] = getPixelData(img, x, y);
		int pixelValue = rgb[c];
		raster.setSample(x, y, 0, pixelValue);
	    }
	}
	return out;
    }


    private static short [][] addAndthresholdImage(BufferedImage img1, BufferedImage img2, int threshold) {

	if ((img1.getWidth() != img2.getWidth()) || (img1.getHeight() != img2.getHeight())) {
	    System.err.println("attempt to add unequal images");
	    return null;
	}

	//	System.out.println("width = " + img1.getWidth() + ", height = " + img1.getHeight() + "; threshold = " + threshold);
	    
       short [][]  out = new short [img1.getWidth()][img1.getHeight()];

       Raster r1 = img1.getData();
       Raster r2 = img2.getData();

       int [] buffer1  = new int [1];
       int [] buffer2 = new int [1];
       int [] pixel1 = new int [1];
       int [] pixel2 = new int [1];

       for (int x = 0; x < img1.getWidth(); x++){
	   //	   System.out.println();
	   for (int y = 0; y < img2.getHeight(); y++){
	       pixel1 = r1.getPixel(x, y, buffer1);
	       pixel2 = r2.getPixel(x, y, buffer2);
	       int sum = pixel1[0] + pixel2[0];
	       if (sum >= threshold) sum = 255;
	       out[x][y] = (short)sum;
	   }
       }
       return out;
    }


    private static int[] getPixelData(BufferedImage img, int x, int y) {
        int argb = img.getRGB(x, y);

        int rgb[] = new int[] {
            (argb >> 16) & 0xff, //red
            (argb >>  8) & 0xff, //green
            (argb      ) & 0xff  //blue
        };
//        System.out.println("rgb: " + rgb[0] + " " + rgb[1] + " " + rgb[2]);
        return rgb;
    }

    private static BufferedImage deepCopy(BufferedImage bi) {
        ColorModel cm = bi.getColorModel();
        boolean isAlphaPremultiplied = cm.isAlphaPremultiplied();
        WritableRaster raster = bi.copyData(null);
        return new BufferedImage(cm, raster, isAlphaPremultiplied, null);
    }


    private static BufferedImage addGrayImages(BufferedImage img1, BufferedImage img2, int thresh) {

	 BufferedImage out = new BufferedImage(img1.getWidth(), img1.getHeight(), BufferedImage.TYPE_BYTE_GRAY);

	 WritableRaster o = out.getRaster();
	 Raster r1 = img1.getData();
	 Raster r2 = img2.getData();

	 int [] buffer1  = new int [1];
	 int [] buffer2 = new int [1];
	 int [] pixel1 = new int [1];
	 int [] pixel2 = new int [1];

	 for (int x = 0; x < out.getWidth(); x++){
	     for (int y = 0; y < out.getHeight(); y++){
		 pixel1 = r1.getPixel(x, y, buffer1);
		 pixel2 = r2.getPixel(x, y, buffer2);
		 int sum = pixel1[0] + pixel2[0];
		 if (sum >= thresh) sum = 255;
		 else sum = 0;
		 o.setSample(x, y, 0, sum);
	     }
	 }
	 return out;
    }
    /*
    BufferedImage CreateEdgeImage() {

	BufferedImage raw = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY); 

	WritableRaster o = raw.getRaster();

	// vertical lines

	for (int x = 0; x < width; x++) {
	    for (int y = 0; y < height; y++)
		o.setSample(x, y, 0, (int)detected[x][y]);
	}

	return raw;
    }

*/
    private static BufferedImage detectRed(BufferedImage img) {

	 BufferedImage red = new BufferedImage(img.getWidth(), img.getHeight(), BufferedImage.TYPE_BYTE_GRAY);

	 WritableRaster raster = red.getRaster();

	 for (int x = 0; x < img.getWidth(); x++){
	     for (int y = 0; y < img.getHeight(); y++){
		 int rgb[] = getPixelData(img, x, y);
		 //		 int pixelValue = rgb[0] - Mat˜πππh.abs(rgb[1] - rgb[2]) - (rgb[1] + rgb[2])/4;
		 int pixelValue = rgb[0] - (rgb[1] + rgb[2])/2;
		 if (pixelValue > 255) pixelValue = 255;
		 else if (pixelValue < 0) pixelValue = 0;
		 raster.setSample(x, y, 0, pixelValue);
	     }
	 }
	 return red;
    }

    static int [] histogram(BufferedImage img, int colorIndex) {

	 int h[] = new int [256];

	 for (int x = 0; x < img.getWidth(); x++){
	     for (int y = 0; y < img.getHeight(); y++){
		 int rgb[] = getPixelData(img, x, y);
		 int pixelValue = rgb[colorIndex];
		 if (pixelValue > 255) pixelValue = 255;
		 else if (pixelValue < 0) pixelValue = 0;
		 h[pixelValue]++;
	     }
	 }
	 return h;
     }
}