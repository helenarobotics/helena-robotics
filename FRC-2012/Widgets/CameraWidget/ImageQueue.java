import java.awt.image.BufferedImage;

public class ImageQueue {
    BufferedImage image;

    public ImageQueue() {
        image = null;
    }

    public synchronized BufferedImage get() {
        BufferedImage tmp = image;
        image = null;     // once consumed, remove it from our 1 - slot queue

        return tmp;
    }

    public synchronized void put(BufferedImage _image) {
        image = _image;
    }
}
