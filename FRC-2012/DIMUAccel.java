package robotics.helena;

import edu.wpi.first.wpilibj.DigitalModule;
import edu.wpi.first.wpilibj.I2C;
import edu.wpi.first.wpilibj.parsing.ISensor;

/**
 * This Class manages the Dexter Inertial Motion Sensor
 *
 * @author Nathan Williams, February 2012
 * @version 0.1
 */
public class DIMUAccel extends I2C implements ISensor {
    /*
     * Documentation can be obtained here:
     * http://dexterindustries.com/manual/imu-sensor/
     */

    // The axis for the gyro
    public static final byte X_AXIS = 0;
    public static final byte Y_AXIS = 1;
    public static final byte Z_AXIS = 2;

    // The I2C address
    private static final int ACCEL_I2C_ADDR = 0x3A;

    // Control register
    private static final byte CTRL_REG = 0x16;

    // Data registers
    private static final byte ACCEL_AXIS_DATA = 0x00;

    // Range
    private static final byte ACCEL_RANGE_2G = 0x04;
    private static final byte ACCEL_RANGE_4G = 0x08;
    private static final byte ACCEL_RANGE_8G = 0x00;

    // Measurement mode
    private static final byte ACCEL_MEASURE = 0x01;

    // Global variable to keep track of the data
    private byte inBuf[] = new byte[6];

    // calibration data
    private int calibration[] = new int[3];

    /**
     * Instantiates the Accel sensor in the dIMU.
     *
     * @param port the port the sensor is attached to
     */
    public DIMUAccel(int slot) {
        super(DigitalModule.getInstance(slot), ACCEL_I2C_ADDR);
        setCompatabilityMode(true);

        // Note, the return values for the devices return true/false based
        // on the abort status of the method.  So, a write that returns true
        // 'aborted', so we want false for all our successes.  Weird..
        if (addressOnly())
            throw new RuntimeException("No device found");

        //
        // Initialize the accel
        //

        // measurement mode with +-2G range
        if (write(CTRL_REG, (ACCEL_RANGE_2G | ACCEL_MEASURE)))
            throw new RuntimeException("Unable to enable accel");

        // Make sure we can read the data.
        if (read(CTRL_REG, 1, inBuf))
            throw new RuntimeException("Unable to read accel data");

        // Finally, calibrate the accelerometer by populating the
        // calibration array.  Note, there is a better way of doing
        // this.
        final int NUM_CALIBRATIONS = 200;

        long calTotal[] = new long[3];
        for (int j = 0; j < 3; j++)
            calTotal[j] = 0;

        int numCals = 0;
        for (int i = 0; i < NUM_CALIBRATIONS; i++) {
            if (!read(ACCEL_AXIS_DATA, 6, inBuf)) {
                // First 6 bytes are 10-bit X access
                calTotal[0] += decodeShortLE(inBuf, 0);
                calTotal[1] += decodeShortLE(inBuf, 2);
                calTotal[2] += decodeShortLE(inBuf, 4);
                numCals++;
            }
            // Delay a bit between reads
            try {
                Thread.sleep(1);
            } catch (InterruptedException ignored) {
            }
        }
        for (int j = 0; j < 3; j++)
            calibration[j] = (int)(calTotal[j] / numCals);
    }

    /**
     * Gets the current acceleration for the specified axis.
     *
     * @param axis the axis (0 for Y, 1 for X, 2 for Z)
     * @return the current acceleration
     */
    public int getCurrentAccel(int axis) {
        if (axis >=0 && axis <= 2) {
            if (!read(ACCEL_AXIS_DATA, 6, inBuf)) {
                return (int)decodeShortLE(inBuf, axis * 2) - calibration[axis];
            }
        }
        return 0;
    }

    public short decodeShortLE(byte buff[], int offset) {
        return (short)((buff[offset] & 0xFF) | (buff[offset + 1] << 8));
    }
}
