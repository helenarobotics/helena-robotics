package robotics.helena;

// This class defines where all the motors are in a single location
public class Configuration {
    // Joysticks - USB slots on the driver station
    public static final int JOYSTICK_DRIVER = 1;
    public static final int JOYSTICK_SHOOTER = 2;

    // PWN slots on the Digital Sidecar
    public static final int VICTOR_LEFT_DRIVE = 1;
    public static final int VICTOR_RIGHT_DRIVE = 2;
    public static final int VICTOR_UPPER_SHOOTER = 3;
    public static final int VICTOR_LOWER_SHOOTER = 4;
    public static final int JAGUAR_SHOOTER_ROTATION = 6;
    public static final int SERVO_SHOOTER_FEEDER = 8;
    public static final int BRIDGE_ARM_MOTOR = 10;

    // I2C bus
    public static final int I2C_ARDUINO = 1;

    // Analog channels
    public static final int GYRO_ANALOG_CHANNEL = 1;

    // XXX - Solenoid slot?
}
