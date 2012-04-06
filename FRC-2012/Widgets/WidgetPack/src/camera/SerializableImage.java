package robotics.helena.widget.camera;
import java.awt.image.*;

import java.io.Serializable;
import java.util.Hashtable;

public class SerializableImage extends BufferedImage implements Serializable{
    public SerializableImage(int width, int height, int imageType) {
        super(width, height, imageType);
    }

    public SerializableImage(int width, int height, int imageType, IndexColorModel cm) {
        super(width, height, imageType, cm);
    }

    public SerializableImage(ColorModel cm, WritableRaster raster, boolean isRasterPremultiplied, Hashtable<?, ?> properties) {
        super(cm, raster, isRasterPremultiplied, properties);
    }
}
