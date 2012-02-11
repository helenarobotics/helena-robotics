package model;

import java.util.Observable;

// This class keeps track of the state of the camera on the robot
public class Robot extends Observable {
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

    public Robot() {
        // Set the starting position of the robot!
        resetPosition();
    }

    public int getXOffset() {
        return xOffset;
    }

    public int getYOffset() {
        return yOffset;
    }

    public int getRotation() {
        return rotation;
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

    // Move the robot!
    public void moveIn() {
        if (yOffset > 0) {
            yOffset -= MOVE_AMT;
            // Fender limits us getting any closer than 39"
            if (yOffset < MIN_Y_OFFSET)
                yOffset = MIN_Y_OFFSET;
            setChanged();
            notifyObservers(yOffset);
        }
    }

    // Move the robot!
    public void moveOut() {
        if (yOffset < MAX_Y_OFFSET) {
            yOffset += MOVE_AMT;
            // Since we can move in MOVE_AMT increments, limit us to
            // being inside the field.
            if (yOffset > MAX_Y_OFFSET)
                yOffset = MAX_Y_OFFSET;
            setChanged();
            notifyObservers(yOffset);
        }
    }

    // Move the robot!
    public void strafeRight() {
        if (xOffset < MAX_X_OFFSET) {
            xOffset += MOVE_AMT;
            // Since we can move in MOVE_AMT increments, limit us to
            // being inside the field.
            if (xOffset > MAX_X_OFFSET)
                xOffset = MAX_X_OFFSET;
            setChanged();
            notifyObservers(xOffset);
        }
    }

    public void strafeLeft() {
        if (xOffset > MIN_X_OFFSET) {
            xOffset -= MOVE_AMT;
            // Since we can move in MOVE_AMT increments, limit us to
            // being inside the field.
            if (xOffset < MIN_X_OFFSET)
                xOffset = MIN_X_OFFSET;
            setChanged();
            notifyObservers(xOffset);
        }
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
