import java.io.File;
import java.io.IOException;

import java.awt.image.BufferedImage;
import java.awt.image.BufferedImageOp;
import java.awt.image.ColorModel;
import java.awt.image.ConvolveOp;
import java.awt.image.Kernel;
import java.awt.image.WritableRaster;

import javax.imageio.ImageIO;

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
        Kernel sobelXKern = new Kernel(3, 3, new float[] {
                                           -1.0f, 0.0f, 1.0f,
                                           -2.0f, 0.0f, 2.0f,
                                           -1.0f, 0.0f, 1.0f
                                       });
        Kernel sobelYKern = new Kernel(3, 3, new float[] {
                                           1.0f, 2.0f, 1.0f,
                                           0.0f, 0.0f, 0.0f,
                                           -1.0f, -2.0f, -1.0f
                                       });
        BufferedImageOp xop = new ConvolveOp(sobelXKern);
        BufferedImageOp yop = new ConvolveOp(sobelYKern);

        long totalTime = 0;
        
        // Iterate through all the provided images!
        boolean success = true;
        for (int fileNum = 0; success && fileNum < args.length; fileNum++) {
            String imageFile = args[fileNum];
//            for (int r = 0; r < 100; r++) {
            long startTime = System.currentTimeMillis();
            try {
                BufferedImage img = ImageIO.read(new File(imageFile));

                // Create a new image to store the convolved data.
//                long startTime = System.currentTimeMillis();
                BufferedImage xImg = deepCopy(img);
                BufferedImage edgeImg = deepCopy(img);
                xop.filter(img, xImg);
                yop.filter(xImg, edgeImg);
                totalTime += (System.currentTimeMillis() - startTime);

                // Write out new image!
                String edgeFile = "Edge_" + fileNum + ".jpg";
                ImageIO.write(edgeImg, "jpg", new File(edgeFile));
                if (true)
                continue;

                int pixelData[][] = new int[img.getHeight() * img.getWidth()][3];

                int counter = 0;
                for (int x = 0; x < img.getWidth(); x++){
                    for (int y = 0; y < img.getHeight(); y++){
                        int rgb[] = getPixelData(img, x, y);
                        for (int k = 0; k < rgb.length; k++)
                            pixelData[counter][k] = rgb[k];

                        counter++;
                    }
                }
            } catch (IOException e) {
                e.printStackTrace();
                success = false;
            }
//            }
        }
        System.out.println("Total time = '" + totalTime / 100 + "'");
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
