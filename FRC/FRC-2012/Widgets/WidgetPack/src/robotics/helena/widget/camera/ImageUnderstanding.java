package robotics.helena.widget.camera;

import java.awt.image.BufferedImage;

public class ImageUnderstanding implements Runnable {
    private ImageQueue iq;
    private DataQueue dq;
    private int downsample;
    private volatile boolean stop;

    public ImageUnderstanding(ImageQueue _iq, DataQueue _dq, int _downsample) {
        iq = _iq;
        dq = _dq;
        downsample = _downsample;

        // we set a flag in above access function to stop the thread
        stop = false;
    }

    public void stop() {
        stop = true;
    }

    public void run() {
        while (!stop) {
            // wait until the next image is available
            BufferedImage image = iq.get();

            // This constructor does all the image processing work:
            // color match, threshold, region growing, and region
            // boundary analysis downsampling,
            // 60 = image threshold, 200 = min region size (in pixels)
            ImageResults results =
                new ImageResults(image, downsample, 60, 200);

            // put the results back onto the queue where main thread
            // can see them
            dq.put(results);

            // debug: report out results we found
            System.out.println(results);
        }
    }
}
