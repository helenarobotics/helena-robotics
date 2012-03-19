import javax.vecmath.Point3d;

public class HoopEstimate {
    public double elevation;      // vertical angle of this estimate relative to camera orientation
    public double azimuth;        // horizontal angle of this estimate relative to camera orientation
    public double range;          // distance from camera to point where range estimate was taken
    public double error;          // approx std we can expect from this estimate; used for combining estimates
    public Point3d rangePoint;    // Absolute coord system 3D point where range estimate was taken.

    public String toString() {
	return ("HoopEstimate: elev = " + Math.toDegrees(elevation) + "deg, azi = " + (int)Math.toDegrees(azimuth) + "deg, range = " + (int)range 
		+ "at point {" + (int)rangePoint.x + ", " + (int)rangePoint.y + ", " + (int)rangePoint.z + "}");
    }
}

