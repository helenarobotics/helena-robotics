package robotics.helena;

import edu.wpi.first.wpilibj.DigitalModule;
import edu.wpi.first.wpilibj.I2C;
import edu.wpi.first.wpilibj.parsing.ISensor;

/**
 * This Class communicates with Chris Ching's Arduino RPM Sensor
 * 
 * @author Nathan Williams, February 2012
 * @version 0.1
 */
public class ArduinoRPMSensor extends I2C implements ISensor {
    // The axis for the gyro 
//    public static final byte BOTH_MOTORS = 0;
    public static final byte TOP_MOTOR = 1;
    public static final byte BOTTOM_MOTOR = 2;

    // The I2C address
    private static final int RPM_I2C_ADDR = 0xC4;

    // RPM data register
    private static final byte RPM_DATA = 0x00;

    // Global variable to keep track of the data
    private byte inBuf[] = new byte[6];

    /**
     * Instantiates the Accel sensor in the dIMU.
     *
     * @param port the port the sensor is attached to
     */
    public ArduinoRPMSensor(int slot) {
        super(DigitalModule.getInstance(slot), RPM_I2C_ADDR);
        setCompatabilityMode(true);

        // Note, the return values for the devices return true/false based
        // on the abort status of the method.  So, a write that returns true
        // 'aborted', so we want false for all our successes.  Weird..
        if (addressOnly())
            throw new RuntimeException("No device found");
    }

    /**
     * Gets the RPM of the specified motor
     *
     * @param motorNum (1 for TOP_MOTOR, 2 for BOTTOM_MOTOR)
     * @return the current motor RPM
     */
    public int getRPM(int motorNum) {
        if (motorNum >= 1 && motorNum <= 2) {
            if (!read(RPM_DATA, 2, inBuf)) {
                return (int)decodeShortLE(inBuf, 0);
            }
        }
        return 0;
    }

    public short decodeShortLE(byte buff[], int offset) {
        return (short)((buff[offset] & 0xFF) | (buff[offset + 1] << 8));
    }
}
