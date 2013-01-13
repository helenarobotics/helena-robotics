package robotics.helena;

// This class defines where all the motors are in a single location
public class Configuration {
    // Joysticks - USB slots on the driver station
    public static final int JOYSTICK_DRIVER = 1;
    public static final int JOYSTICK_THROWER = 2;

    // PWN slots on the Digital Sidecar
    public static final int VICTOR_LEFT_DRIVE = 1;
    public static final int VICTOR_RIGHT_DRIVE = 2;
    public static final int VICTOR_FRISBEE_SHOOTER = 3;

    // I2C bus
    public static final int I2C_ARDUINO = 1;

    // Shifter solenoid
    public static final int SHIFTER_SOLENOID = 8;
}
