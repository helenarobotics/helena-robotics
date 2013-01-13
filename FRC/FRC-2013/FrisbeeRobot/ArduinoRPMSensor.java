package robotics.helena;

import edu.wpi.first.wpilibj.DigitalModule;
import edu.wpi.first.wpilibj.I2C;
import edu.wpi.first.wpilibj.parsing.ISensor;

/**
 * This Class communicates with Chris Ching's Arduino RPM Sensor
 *
 * @author Nathan Williams, March 2012
 * @version 0.2
 */
public class ArduinoRPMSensor extends I2C implements ISensor {
    // The two RPM sensors
    public static final byte BOTH_MOTORS = 0;
    public static final byte FIRST_MOTOR = 1;
    public static final byte SECOND_MOTOR = 2;

    // The I2C address
    private static final int I2C_ADDR = 0xCA;

    // Data register
    private static final byte DATA_REG = 0x00;

    // Class variable to keep track of the data
    private byte inBuf[] = new byte[6];

    /**
     * Instantiates the I2C sensor
     *
     * @param port the port the sensor is attached to
     */
    public ArduinoRPMSensor(int slot) {
        super(DigitalModule.getInstance(slot), I2C_ADDR);
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
    public synchronized int getRPM(int motorNum) {
        if (motorNum >= 1 && motorNum <= 2) {
            // The stock Arduino 1.0 I2C slave code does not handle
            // extended transactions where the Master keeps the bus
            // locked for the entire transaction.  Therefore, we must do
            // the request as two completely separate transactions, a
            // 'W'rite (indicating which counter we wish to fetch),
            // followed by a 'R'ead, which *CAN NOT* be send with the
            // register to read (a 'W'rite request) due to the
            // aformentioned bugs.
            //
            // A bugfix has been submitted to have this modified
            // http://code.google.com/p/arduino/issues/detail?id=848&q=Wire
            // Fix:
            // https://github.com/arduino/Arduino/pull/66
            byte sndBuf[] = new byte[] { DATA_REG, (byte)motorNum };
            if (!this.transaction(sndBuf, sndBuf.length, inBuf, 2))
//            if (!write(DATA_REG, (byte)motorNum) &&
//                !transaction(null, 0, inBuf, 2))
                return (int)decodeShortLE(inBuf, 0);
        }
        return 0;
    }

    /**
     * Fetches all the RPM values
     *
     * @return the array of RPM values
     */
    public synchronized int[] getRPMs() {
        int retVal[] = new int[2];

//        byte sndBuf[] = new byte[] { DATA_REG, BOTH_MOTORS };
//        if (!this.transaction(sndBuf, sndBuf.length, inBuf, 4)) {
        if (!write(DATA_REG, BOTH_MOTORS) &&
            !transaction(null, 0, inBuf, 4)) {
            retVal[0] = (int)decodeShortLE(inBuf, 0);
            retVal[1] = (int)decodeShortLE(inBuf, 2);
        }
        return retVal;
    }

    private short decodeShortLE(byte buff[], int offset) {
        return (short)((buff[offset] & 0xFF) | (buff[offset + 1] << 8));
    }
}
