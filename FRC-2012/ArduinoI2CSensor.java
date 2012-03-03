package robotics.helena;

import edu.wpi.first.wpilibj.DigitalModule;
import edu.wpi.first.wpilibj.I2C;
import edu.wpi.first.wpilibj.parsing.ISensor;

/**
 * This Class communicates with the example Arduino I2C Sensor
 *
 * @author Nathan Williams, March 2012
 * @version 0.1
 */
public class ArduinoI2CSensor extends I2C implements ISensor {
    // The counters on the sensor
    public static final byte BOTH_COUNTERS = 0;
    public static final byte FIRST_COUNTER = 1;
    public static final byte SECOND_COUNTER = 2;

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
    public ArduinoI2CSensor(int slot) {
        super(DigitalModule.getInstance(slot), I2C_ADDR);
        setCompatabilityMode(true);

        // Note, the return values for the devices return true/false based
        // on the abort status of the method.  So, a write that returns true
        // 'aborted', so we want false for all our successes.  Weird..
        if (addressOnly())
            throw new RuntimeException("No device found");
    }

    /**
     * Fetches an individual sensor count
     *
     * @param counterNum (1 for FIRST_COUNTER, 2 for SECOND_COUNTER)
     * @return the sensor's counter value
     */
    public int getCount(int counterNum) {
        if (counterNum >= 1 && counterNum <= 2) {
            // The Arduino I2C slave code does not handle extended
            // transactions where the Master keeps the bus locked
            // for the entire transaction.  Therefore, we must
            // do the request as two completely separate transactions,
            // a 'W'rite (indicating which counter we wish to fetch),
            // followed by a 'R'ead, which *CAN NOT* be send
            // with the register to read (a 'W'rite request) due to
            // the aformentioned bugs.

            // Indicate which counter we're reading, and then read
            // the counter value with a raw 'R'ead request.
//            byte sndBuf[] = new byte[] { DATA_REG, (byte)counterNum };
//            if (!this.transaction(sndBuf, sndBuf.length, inBuf, 2))
            if (!write(DATA_REG, (byte)counterNum) &&
                !transaction(null, 0, inBuf, 2))
                return (int)decodeShortLE(inBuf, 0);
        }
        return 0;
    }

    /**
     * Fetches all the sensor counts
     *
     * @return the array of sensor counter values
     */
    public int[] getCounts() {
        int retVal[] = new int[2];

//        byte sndBuf[] = new byte[] { DATA_REG, BOTH_COUNTERS };
//        if (!this.transaction(sndBuf, sndBuf.length, inBuf, 4)) {
        if (!write(DATA_REG, BOTH_COUNTERS) &&
            !transaction(null, 0, inBuf, 4)) {
            retVal[0] = (int)decodeShortLE(inBuf, 0);
            retVal[1] = (int)decodeShortLE(inBuf, 2);
        }
        return retVal;
    }

    public short decodeShortLE(byte buff[], int offset) {
        return (short)((buff[offset] & 0xFF) | (buff[offset + 1] << 8));
    }
}
