public class CameraModel {    // We might move the camera model to its own class:
    public static double cameraHeight = 49.5;   // inches (an estimate for now)
    public static double cameraTilt = 11.0 * Math.PI/180.0;     // estimated from data set of March 2; we need to nail this down
    public static final double cameraHorizontalView = 54.0 * Math.PI/180.0;
    public static final double cameraVerticalView = 40.5 * Math.PI/180.0;
    public static int cameraXpixels = 320;        // this needs to come from actual image size after desampling
    public static int cameraYpixels = 240;

    public CameraModel() {
    }

    public String toString() {
	return ("Camera " + cameraXpixels + " (" +  cameraHorizontalView + " deg) x" + cameraYpixels + " (" + cameraVerticalView + " deg), tilt " + cameraTilt + ", ht " + cameraHeight + "inches");
    }

}

