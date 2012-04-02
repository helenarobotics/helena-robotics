import java.awt.image.BufferedImage;

public class ImageQueue {
    private Object syncObject = new Object();
    private BufferedImage image;

    public ImageQueue() {
        image = null;
    }

    public BufferedImage get() {
        BufferedImage tmp;
        synchronized (syncObject) {
            // Block until we have a new image!
            while (image == null) {
                try {
                    syncObject.wait();
                } catch (InterruptedException ignored) {
                }
            }
            // once consumed, remove it from our 1 - slot queue
            tmp = image;
            image = null;
        }
        return tmp;
    }

    public void put(BufferedImage _image) {
        synchronized (syncObject) {
            image = _image;
            syncObject.notifyAll();
        }
    }
}
