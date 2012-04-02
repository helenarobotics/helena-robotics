import java.io.*;
import java.io.IOException;
import java.awt.image.BufferedImage;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.util.Vector;

public class ImageUnderstanding implements Runnable {
    ImageQueue iq;
    DataQueue dq;
    boolean stop;
    int downsample;

    public ImageUnderstanding(ImageQueue _iq, DataQueue _dq, int _downsample) {
        iq = _iq;
        dq = _dq;
        downsample = _downsample;
    }

    public void stop() {
        stop = true;
    }

    public void run() {
        // we set a flag in above access function to stop the thread
        stop = false;

        while (!stop) {
            // wait until the next image is available
            BufferedImage image = iq.get();

            // image is null if 'stop' was set true, so avoid crashing
            // this thread(untidy ending!)
            if (image != null) {
                // This constructor does all the image processing work:
                // color match, threshold, region growing, and region
                // boundary analysis downsampling,
                // 80 = image threshold, 200 = min region size (in pixels)
                ImageResults results =
                    new ImageResults(image, downsample, 80, 200);

                // put the results back onto the queue where main thread
                // can see them
                dq.put(results);
                
                // debug: report out results we found
                System.out.println(results);
            }
        }
    }
}
