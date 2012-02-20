package model;

import java.util.Observable;

// This class keeps track of the state of the camera on the robot
public class Robot extends Observable {
    // Height of the robot (really, the height of the camera on the
    // robot).
    private static final int CAMERA_HEIGHT = 39;

    // The field length in inches
    private static final int MAX_Y_OFFSET = 54 * 12;

    // The fender limits how close we can get
    private static final int MIN_Y_OFFSET = 39;

    // The field width is half of 27'
    private static final int MAX_X_OFFSET = 27 * 12 / 2;
    private static final int MIN_X_OFFSET = -27 * 12 / 2;

    // XXX - Arbitrarily chose to move 1 foot at a time.
    private static final int MOVE_AMT = 12;
    
    // Robot location!  By default, the robot faces the basket
    // xOffset = 0 would be right under the basket
    // xOffset = 36 would be 3 feet to the right of the basket
    //
    // yOffset = 0 would be in the center of the court
    // yOffset = 36 would be 3 feet behind (in front of) the basket
    //
    // rotation = 0 would face the basket directly (assuming yOffset = 0)
    // rotation = 45 would rotate the robot 45 degrees clockwise, so the
    //   basket would be to the left of the robot (assuming xOffset = 0)
    private int xOffset;
    private int yOffset;
    private int rotation;
    private int height;

    public Robot() {
        // Height is a constant
        height = CAMERA_HEIGHT;

        // Set the starting position of the robot!
        resetPosition();
    }

    // Reset the camera back to 'zero' position
    public void resetPosition() {
        // Center court, 20 feet away, facing the basket
        xOffset = 0;
        yOffset = 20 * 12;
        rotation = 0;
        setChanged();
        notifyObservers(rotation);
    }

    public int getHeight() {
        return height;
    }

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

    public int getYOffset() {
        return yOffset;
    }

    public void setYOffset(int _yOffset) {
        // Fender limits us getting any closer than 39"
        if (_yOffset < MIN_Y_OFFSET)
            _yOffset = MIN_Y_OFFSET;
        else if (_yOffset > MAX_Y_OFFSET)
            _yOffset = MAX_Y_OFFSET;

        if (yOffset != _yOffset) {
            yOffset = _yOffset;
            setChanged();
            notifyObservers(yOffset);
        }
    }

    public int getRotation() {
        return rotation;
    }

    public void moveIn() {
        setYOffset(yOffset - MOVE_AMT);
    }

    public void moveOut() {
        setYOffset(yOffset + MOVE_AMT);
    }

    public void strafeLeft() {
        setXOffset(xOffset - MOVE_AMT);
    }

    public void strafeRight() {
        setXOffset(xOffset + MOVE_AMT);
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
