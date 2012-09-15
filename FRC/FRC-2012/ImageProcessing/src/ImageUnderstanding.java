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

	stop = false;     // we set a flag in above access function to stop the thread

	BufferedImage image;

	while (!stop) {

	    // wait until the next image is available
	    while (((image = iq.get()) == null) && !stop){  
		try {
		    Thread.currentThread().sleep(10);  // sleep 10msec
		} catch (InterruptedException e) {
		    e.printStackTrace();
		}
	    }

	    // This constructor does all the image processing work: color match, threshold, region growing, and region boundary analysis
	    //  downsampling, 80 = image threshold, 200 = min region size (in pixels)
	    if (image != null) {          // image is null if 'stop' was set true, so avoid crashing this thread (untidy ending!)
		ImageResults results = new ImageResults(image, downsample, 80, 200);  // constructor does it all
		dq.put(results);                // put the results back onto the queue where main thread can see them
		System.out.println(results);    // debug: report out results we found
	    }
	}
    }
}

