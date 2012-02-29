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
    public short [][] detected;

    public EdgeDetect(BufferedImage _image, int thresh) {

	image = _image;
	width =  image.getWidth();
	height = image.getHeight();

        // Convolve to find the edges
	/*
	Kernel sobelXKernR = new Kernel(3, 3, new float[] {
		-1.0f, 0.0f, 0.0f, 
		-2.0f, 0.0f, 2.0f, 
		-1.0f, 0.0f, 1.0f
	    });
        BufferedImageOp xopR = new ConvolveOp(sobelXKernR);
	*/

	/*
        Kernel sobelYKernD = new Kernel(3, 3, new float[] {
		-1.0f, -2.0f, -1.0f,
		0.0f, 0.0f, 0.0f, 
		1.0f, 2.0f, 1.0f, 
	    });
        BufferedImageOp yopD = new ConvolveOp(sobelYKernD);
	*/

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
	
	BufferedImage img = detectRed(image);

	// Create a new image to store the convolved data.
	// BufferedImage xImgL = deepCopy(img);
	// BufferedImage xImgR = deepCopy(img);

	BufferedImage xImgL = deepCopy(img);
	BufferedImage yImgU = deepCopy(img);

	xopL.filter(img, xImgL);
	yopU.filter(img, yImgU);
	detected = addAndthresholdImage(xImgL, yImgU, thresh);
	BufferedImage iout = addGrayImages(xImgL, yImgU);

	try {
	    ImageIO.write(iout, "jpg", new File("SumLU.jpg"));
	    ImageIO.write(img, "jpg", new File("Grayscale.jpg"));
	    ImageIO.write(xImgL, "jpg", new File("xEdge.jpg"));
	    ImageIO.write(yImgU, "jpg", new File("yEdge.jpg"));
	} catch (IOException e) {
	    e.printStackTrace();
	}


	// xopR.filter(img, xImgR);
	//	yopD.filter(image, yImgD);

	// Write out new image set
	/*                ImageIO.write(yImgU, "jpg", new File("yEdgeU" + fileNum + ".jpg"));
			  ImageIO.write(yImgD, "jpg", new File("yEdgeD" + fileNum + ".jpg"));
			  ImageIO.write(xImgL, "jpg", new File("xEdgeL" + fileNum + ".jpg"));
			  ImageIO.write(xImgR, "jpg", new File("xEdgeR" + fileNum + ".jpg"));
	*/



    }

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




    private static short [][] addAndthresholdImage(BufferedImage img1, BufferedImage img2, int threshold) {

	if ((img1.getWidth() != img2.getWidth()) || (img1.getHeight() != img2.getHeight())) {
	    System.err.println("attempt to add unequal images");
	    return null;
	}

	System.out.println("width = " + img1.getWidth() + ", height = " + img1.getHeight() + "; threshold = " + threshold);
	    
       short [][]  out = new short [img1.getWidth()][img1.getHeight()];

       Raster r1 = img1.getData();
       Raster r2 = img2.getData();

       int [] buffer1  = new int [1];
       int [] buffer2 = new int [1];
       int [] pixel1 = new int [1];
       int [] pixel2 = new int [1];

       for (int x = 0; x < img1.getWidth(); x++){
	   System.out.println();
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


    private static BufferedImage addGrayImages(BufferedImage img1, BufferedImage img2) {

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
		 if (sum > 255) sum = 255;
		 o.setSample(x, y, 0, sum);
	     }
	 }
	 return out;
    }

}