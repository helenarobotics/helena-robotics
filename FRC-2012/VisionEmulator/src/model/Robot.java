package model;

import java.util.Observable;

// This class keeps track of the state of the camera on the robot
public class Robot extends Observable {
    // The field width is half of 27'
    private static final int MAX_X_OFFSET = 27 * 12 / 2;
    private static final int MIN_X_OFFSET = -27 * 12 / 2;

    // Height of the robot (really, the height of the camera on the
    // robot).
    private static final int CAMERA_HEIGHT = 39;

    // The field length in inches
    private static final int MAX_Z_OFFSET = 54 * 12;

    // The fender limits how close we can get
    private static final int MIN_Z_OFFSET = 39;

    // Arbitrarily chose to move 1 foot at a time.
    private static final int MOVE_AMT = 12;
    
    // Robot location!  By default, the robot faces the basket
    // xOffset = 0 would be right under the basket
    // xOffset = 36 would be 3 feet to the right of the basket
    //
    // yOffset = CAMERA_HEIGHT, since the height of the camera on the
    // robot is effectively fixed.
    //
    // zOffset = 0 would be at the backboard
    // zOffset = 36 would be 3 feet behind (in front of) the basket
    //
    // rotation = 0 would face the basket directly (assuming yOffset = 0)
    // rotation = 45 would rotate the robot 45 degrees clockwise, so the
    //   basket would be to the left of the robot (assuming xOffset = 0)
    private int xOffset;
    private int yOffset;
    private int zOffset;
    private int rotation;

    public Robot() {
        // Height is a constant
        yOffset = CAMERA_HEIGHT;

        // Set the starting position of the robot!
        resetPosition();
    }

    // Reset the camera back to 'zero' position
    public void resetPosition() {
        // Center court, 20 feet away, facing the basket
        xOffset = 0;
        zOffset = 20 * 12;
        rotation = 0;
        setChanged();
        notifyObservers(rotation);
    }

    // Horizontal distance from the center of the backboard
    public int getXOffset() {
        return xOffset;
    }

    public void setXOffset(int _xOffset) {
        // Limit the robot to the field
        if (_xOffset < MIN_X_OFFSET)
            _xOffset = MIN_X_OFFSET;
        else if (_xOffset > MAX_X_OFFSET)
            _xOffset = MAX_X_OFFSET;

        if (xOffset != _xOffset) {
            xOffset = _xOffset;
            setChanged();
            notifyObservers(xOffset);
        }
    }

    public void strafeLeft() {
        setXOffset(xOffset - MOVE_AMT);
    }

    public void strafeRight() {
        setXOffset(xOffset + MOVE_AMT);
    }

    // Vertical distance from the floor.
    public int getYOffset() {
        return yOffset;
    }

    // Distance away from the backboard along the floor
    public int getZOffset() {
        return zOffset;
    }

    public void setZOffset(int _zOffset) {
        // Fender limits us getting any closer than 39"
        if (_zOffset < MIN_Z_OFFSET)
            _zOffset = MIN_Z_OFFSET;
        else if (_zOffset > MAX_Z_OFFSET)
            _zOffset = MAX_Z_OFFSET;

        if (zOffset != _zOffset) {
            zOffset = _zOffset;
            setChanged();
            notifyObservers(zOffset);
        }
    }

    public void moveIn() {
        setZOffset(zOffset - MOVE_AMT);
    }

    public void moveOut() {
        setZOffset(zOffset + MOVE_AMT);
    }

    // The robot's rotation in the XZ plane
    public int getRotation() {
        return rotation;
    }

    // We normalize the rotation angle to +- 180 degrees
    public void rotateLeft() {
        rotation--;
        if (rotation < -180)
            rotation = 360 + rotation;
        setChanged();
        notifyObservers(rotation);
    }

    public void rotateRight() {
        rotation++;
        if (rotation > 180)
            rotation = rotation - 360;
        setChanged();
        notifyObservers(rotation);
    }
}
