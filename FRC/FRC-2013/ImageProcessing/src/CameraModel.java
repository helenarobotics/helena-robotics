public class CameraModel {    // We might move the camera model to its own class:
    public static double cameraHeight = 48.0;   // XXX  inches (an estimate for now)
    public static double cameraTilt = 20.0 * Math.PI/180.0;     // estimated from data set of March 3, 2013; we need to nail this down
    public static final double cameraHorizontalView = 54.0 * Math.PI/180.0;
    public static final double cameraVerticalView = 40.5 * Math.PI/180.0;

    public CameraModel() {
    }

    public String toString() {
	return ("Camera view " +  cameraHorizontalView + " deg x" + " (" + cameraVerticalView + " deg), tilt " + cameraTilt + ", ht " + cameraHeight + "inches");
    }
}

