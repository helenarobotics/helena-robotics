package model;

import java.util.Observable;

// This class keeps track of the state of the camera on the robot
public class Camera extends Observable {
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
