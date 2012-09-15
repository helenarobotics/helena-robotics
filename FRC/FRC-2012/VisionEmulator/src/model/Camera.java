package model;

import java.util.Observable;

// This class keeps track of the state of the camera on the robot
public class Camera extends Observable {
    // Global constants

    // Field of view.  Since the camera has a 4:3 perspective, the
    // vertical FOV is reduced by the inverse, or 3/4.
    public static final double HORIZONTAL_FOV = 54.0;
    public static final double VERTICAL_FOV = 54.0 * 3.0 / 4.0;

    // Camera pixels
    public static final double HORIZONTAL_PIXELS = 640;
    public static final double VERTICAL_PIXELS = 480;

    // We convert angles to pixels to view them on the screen.
    public static final double HORIZONTAL_ANGLE_TO_PIXELS =
        HORIZONTAL_PIXELS / HORIZONTAL_FOV;
    public static final double VERTICAL_ANGLE_TO_PIXELS =
        VERTICAL_PIXELS / VERTICAL_FOV;

    // Limit how far the camera can tilt
    private static final int MAX_ANGLE_AZIMUTH = 45;
    private static final int MIN_ANGLE_AZIMUTH = -45;
    private static final int MAX_ANGLE_INCLINE = 30;
    private static final int MIN_ANGLE_INCLINE = -30;

    // The camera information on the robot
    private int azimuthAngle;
    private int inclineAngle;

    public Camera() {
        // Set the camera startup angles
        resetAngle();
    }

    public int getAzimuthAngle() {
        return azimuthAngle;
    }

    public int getInclineAngle() {
        return inclineAngle;
    }

    // Reset the camera back to 'zero' position
    public void resetAngle() {
        azimuthAngle = 0;
        inclineAngle = 0;
        setChanged();
        notifyObservers(azimuthAngle);
    }

    // Update the camera information!
    public void panUp() {
        if (inclineAngle < MAX_ANGLE_INCLINE) {
            inclineAngle++;
            setChanged();
            notifyObservers(inclineAngle);
        }
    }
    
    // Update the camera information!
    public void panDown() {
        if (inclineAngle > MIN_ANGLE_INCLINE) {
            inclineAngle--;
            setChanged();
            notifyObservers(inclineAngle);
        }
    }

    public void panLeft() {
        if (azimuthAngle > MIN_ANGLE_AZIMUTH) {
            azimuthAngle--;
            setChanged();
            notifyObservers(azimuthAngle);
        }
    }

    public void panRight() {
        if (azimuthAngle < MAX_ANGLE_AZIMUTH) {
            azimuthAngle++;
            setChanged();
            notifyObservers(azimuthAngle);
        }
    }
}
