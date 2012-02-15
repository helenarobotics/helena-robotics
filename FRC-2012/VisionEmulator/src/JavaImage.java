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

// following additions for HoughTransform section.  Unclear that we need the complete awt imported...
import java.awt.*; 
import java.util.Vector; 


public class JavaImage {
    public static void main(String args[]) {
        if (args.length == 0) {
            System.err.println(
                "Provide image(s) to process as argument(s) to program");
            System.exit(-1);
        }

        // Validate *ALL* the arguments before processing any of them.

        for (int fileNum = 0; fileNum < args.length; fileNum++) {
            File f = new File(args[fileNum]);
            if (!f.exists()) {
                System.err.println(
                    "Provided image '" + args[fileNum] + "' !exist");
                System.exit(-1);
            }
        }

        // Convolve to find the edges
	Kernel sobelXKernR = new Kernel(3, 3, new float[] {
		-1.0f, 0.0f, 0.0f, 
		-2.0f, 0.0f, 2.0f, 
		-1.0f, 0.0f, 1.0f
	    });
	Kernel sobelXKernL = new Kernel(3, 3, new float[] {
		0.0f, 0.0f, -1.0f, 
		2.0f, 0.0f, -2.0f, 
		1.0f, 0.0f, -1.0f
	    });
        Kernel sobelYKernD = new Kernel(3, 3, new float[] {
		-1.0f, -2.0f, -1.0f,
		0.0f, 0.0f, 0.0f, 
		1.0f, 2.0f, 1.0f, 
	    });
        Kernel sobelYKernU = new Kernel(3, 3, new float[] {
		1.0f, 2.0f, 1.0f, 
		0.0f, 0.0f, 0.0f, 
		-1.0f, -2.0f, -1.0f,
	    });

        BufferedImageOp xopR = new ConvolveOp(sobelXKernR);
        BufferedImageOp xopL = new ConvolveOp(sobelXKernL);
        BufferedImageOp yopU = new ConvolveOp(sobelYKernU);
        BufferedImageOp yopD = new ConvolveOp(sobelYKernD);

        long totalTime = 0;
        
        // Iterate through all the provided images!
        boolean success = true;
        for (int fileNum = 0; success && fileNum < args.length; fileNum++) {
            String imageFile = args[fileNum];
	    System.out.println("processing file '" + imageFile + "'");
            long startTime = System.currentTimeMillis();
            try {
                BufferedImage colorImg = ImageIO.read(new File(imageFile));

		// convert to probability-of-backboard function (simplified for now):

		BufferedImage img = detectRed(colorImg);

//                long startTime = System.currentTimeMillis();
                // Create a new image to store the convolved data.
                BufferedImage xImgL = deepCopy(img);
                BufferedImage xImgR = deepCopy(img);
                BufferedImage yImgD = deepCopy(img);
                BufferedImage yImgU = deepCopy(img);

		xopL.filter(img, xImgL);
		xopR.filter(img, xImgR);
		yopD.filter(img, yImgD);
		yopU.filter(img, yImgU);

                totalTime += (System.currentTimeMillis() - startTime);

                // Write out new image set

                ImageIO.write(yImgU, "jpg", new File("yEdgeU" + fileNum));
                ImageIO.write(yImgD, "jpg", new File("yEdgeD" + fileNum));
                ImageIO.write(xImgL, "jpg", new File("xEdgeL" + fileNum));
                ImageIO.write(xImgR, "jpg", new File("xEdgeR" + fileNum));

		BufferedImage iout = addGrayImages(xImgR, yImgD);

		ImageIO.write(iout, "jpg", new File("Sum_" + fileNum));

		/*
		BufferedImage grayscale_image = new BufferedImage(img.getWidth(), img.getHeight(), 
							BufferedImage.TYPE_BYTE_GRAY);  
		Graphics g = grayscale_image.getGraphics();  
		g.drawImage(img, 0, 0, null);  
		g.dispose();
		String gFile = "Grayscale_" + fileNum;
                ImageIO.write(grayscale_image, "jpg", new File(gFile));
		*/

		ImageIO.write(img, "jpg", new File("Grayscale_" + fileNum));

	    
            } catch (IOException e) {
                e.printStackTrace();
                success = false;
            }
        }
        System.out.println("Total time = '" + totalTime / 100 + "'");
    }


    private static BufferedImage detectRed(BufferedImage img) {

	 BufferedImage red = new BufferedImage(img.getWidth(), img.getHeight(), BufferedImage.TYPE_BYTE_GRAY);

	 WritableRaster raster = red.getRaster();

	 for (int x = 0; x < img.getWidth(); x++){
	     for (int y = 0; y < img.getHeight(); y++){
		 int rgb[] = getPixelData(img, x, y);
		 int pixelValue = rgb[0] - Math.abs(rgb[1] - rgb[2]) - rgb[1]/2 - rgb[2]/2;
		 if (pixelValue > 255) pixelValue = 255;
		 else if (pixelValue < 0) pixelValue = 0;
		 raster.setSample(x, y, 0, pixelValue);
	     }
	 }
	 return red;
    }



    private static BufferedImage addGrayImages(BufferedImage img1, BufferedImage img2, BufferedImage img3, BufferedImage img4) {

	 BufferedImage out = new BufferedImage(img1.getWidth(), img1.getHeight(), BufferedImage.TYPE_BYTE_GRAY);

	 WritableRaster o = out.getRaster();
	 Raster r1 = img1.getData();
	 Raster r2 = img2.getData();
	 Raster r3 = img3.getData();
	 Raster r4 = img4.getData();

	 int [] buffer = new int [1];
	 int [] pixel1 = new int [1];
	 int [] pixel2 = new int [1];
	 int [] pixel3 = new int [1];
	 int [] pixel4 = new int [1];

	 for (int x = 0; x < out.getWidth(); x++){
	     for (int y = 0; y < out.getHeight(); y++){
		 pixel1 = r1.getPixel(x, y, buffer);
		 pixel2 = r2.getPixel(x, y, buffer);
		 pixel3 = r3.getPixel(x, y, buffer);
		 pixel4 = r4.getPixel(x, y, buffer);
		 int sum = pixel1[0] + pixel2[0] + pixel3[0] + pixel4[0];
		 if (sum > 255) sum = 255;
		 o.setSample(x, y, 0, sum);
	     }
	 }
	 return out;
    }


    private static BufferedImage addGrayImages(BufferedImage img1, BufferedImage img2) {

	 BufferedImage out = new BufferedImage(img1.getWidth(), img1.getHeight(), BufferedImage.TYPE_BYTE_GRAY);

	 WritableRaster o = out.getRaster();
	 Raster r1 = img1.getData();
	 Raster r2 = img2.getData();

	 int [] buffer = new int [1];
	 int [] pixel1 = new int [1];
	 int [] pixel2 = new int [1];

	 for (int x = 0; x < out.getWidth(); x++){
	     for (int y = 0; y < out.getHeight(); y++){
		 pixel1 = r1.getPixel(x, y, buffer);
		 pixel2 = r2.getPixel(x, y, buffer);
		 int sum = pixel1[0] + pixel2[0];
		 if (sum > 255) sum = 255;
		 o.setSample(x, y, 0, sum);
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
}