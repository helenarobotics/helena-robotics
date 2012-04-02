import javax.vecmath.Point3d;

public class HoopEstimate {
    // vertical angle of this estimate relative to camera orientation(in
    // radians)
    public double elevation;

    // horizontal angle of this estimate relative to camera
    // orientation(radians)
    public double azimuth;

    // distance from camera to point where range estimate was taken (in
    // inches, of all things)
    public double range;

    // approx std we can expect from this estimate; used for combining
    // estimates(inches)
    public double error;

    // distance between camera and wall directly beneath hoop(ignores
    // vertical component--just takes x &z)
    public double floorRange;

    // Absolute coord system 3D point where range estimate was taken(inches)
    public Point3d rangePoint;

    public HoopEstimate(double cornerX, double cornerY, double hoopX, double hoopY, int nx, int ny) {
        // estimate angle parameters
        this.elevation = (ny / 2 - cornerY) * CameraModel.cameraVerticalView / ny + CameraModel.cameraTilt;
        this.azimuth = cornerX * CameraModel.cameraHorizontalView / nx;

        // estimate range parameters
        double h = hoopY - CameraModel.cameraHeight;
        // total air distance between camera and measured corner of hoop
        this.range = Math.abs(h / Math.sin(elevation));

        // distance along the floor between point directly below camera,
        // and directly below hoop(y = 0)
        this.floorRange = range * Math.cos(elevation);

        // Calculate range error parameter for later use as weighting
        // factor across multiple estimates.
        this.error = Math.abs(h * Math.cos(elevation) * CameraModel.cameraVerticalView / ny / (Math.sin(elevation) * Math.sin(elevation)));

        // save actual hoop corner location for later reference.  NB we
        // set z to 0, because hoop is on wall (z = 0 plane)
        this.rangePoint = new Point3d(hoopX, hoopY, 0.0);
    }

    public String toString() {
        return ("HoopEstimate: elev = " + Math.toDegrees(elevation) + "deg, " +
                "azi = " + (int)Math.toDegrees(azimuth) + "deg, " +
                "range = " + (int)range +
                "at point {" +
                (int)rangePoint.x + ", " +
                (int)rangePoint.y + ", " +
                (int)rangePoint.z + "}");
    }
}
