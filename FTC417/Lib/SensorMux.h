//
// SensorMux.h
//
// Reference:
//      http://www.breigh.com/xander/HTSMUX-Tutorial-LATEST.pdf

//-----------------------------------------------------------------------------
// Legacy
//-----------------------------------------------------------------------------

#define HTSMUX_I2C_ADDR         0x10    /*!< HTSMUX I2C device address */
#define HTSMUX_COMMAND          0x20    /*!< Command register */
#define HTSMUX_STATUS           0x21    /*!< Status register */

// Registers
#define HTSMUX_MODE             0x00    /*!< Sensor mode register */
#define HTSMUX_TYPE             0x01    /*!< Sensor type register */
#define HTSMUX_I2C_COUNT        0x02    /*!< I2C byte count register */
#define HTSMUX_I2C_DADDR        0x03    /*!< I2C device address register */
#define HTSMUX_I2C_MADDR        0x04    /*!< I2C memory address register */
#define HTSMUX_CH_OFFSET        0x22    /*!< Channel register offset */
#define HTSMUX_CH_ENTRY_SIZE    0x05    /*!< Number of registers per sensor channel */

#define HTSMUX_ANALOG           0x36    /*!< Analogue upper 8 bits register */
#define HTSMUX_AN_ENTRY_SIZE    0x02    /*!< Number of registers per analogue channel */

#define HTSMUX_I2C_BUF          0x40    /*!< I2C buffer register offset */
#define HTSMUX_BF_ENTRY_SIZE    0x10    /*!< Number of registers per buffer */

typedef enumWord {
    SMUXCMD_HALT = 0x00,
    SMUXCMD_AUTODETECT = 0x01,
    SMUXCMD_RUN = 0x02,
} SMUXCMD;

/* nb: actually a bit field. careful! */
typedef enumWord {
    SMUXSTAT_NORMAL = 0x00,
    SMUXSTAT_BATT = 0x01,
    SMUXSTAT_BUSY = 0x02,
    SMUXSTAT_HALT = 0x04,
    SMUXSTAT_ERROR = 0x08,
    /*!< Status hasn't really been set yet */
    SMUXSTAT_NOTHING = 0xFF,
} SMUXSTAT;

// Channel modes
typedef enumWord {
    SMUXCHMODE_NONE = 0,
    SMUXCHMODE_I2C = 0x01,  /*!< I2C channel present channel mode */
    SMUXCHMODE_9V = 0x02,   /*!< Enable 9v supply on analogue pin channel mode */
    SMUXCHMODE_DIG0_HIGH = 0x04,    /*!< Drive pin 0 high channel mode */
    SMUXCHMODE_DIG1_HIGH = 0x08,    /*!< Drive pin 1 high channel mode */
    SMUXCHMODE_I2C_SLOW = 0x10,     /*!< Set slow I2C rate channel mode */
} SMUXCHMODE;

// Constants denoting the type of attached sensors
// as reported by a sensor mux
typedef enumWord {
    MUXSENST_ANALOG = 0,
    MUXSENST_SONIC = 1,
    MUXSENST_COMPASS = 2,
    MUXSENST_COLOROLD = 3,
    MUXSENST_ACCEL = 4,
    MUXSENST_IRSEEKOLD = 5,
    MUXSENST_PROTO = 6,
    MUXSENST_COLOR = 7,
    MUXSENST_ANGLE = 8,
    MUXSENST_IRSEEK = 9,
    // never reported by sensor mux (?); we use internally to denote
    // absense of sensor
    MUXSENST_NONE = 0xFF,
} MUXSENST;

typedef enum {
    SENSORONMUX_DUMMY,
} SENSORONMUX;

//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------

typedef struct {
    BOOL fActive;
    SMUXSTAT status;
    MUXSENST rgtype[4];
} SENSORMUX;

#define GetSensorType(link,channel)          (rgsensormux[link].rgtype[channel])
#define GetSensorTypeMux(muxsensor)          GetSensorType(LINK_SID(muxsensor),CHANNEL_SID(muxsensor))
#define CheckSensorType(link,channel,type)   (GetSensorType(link,channel) == (type))
#define CheckSensorTypeMux(muxsensor,type)   CheckSensorType(LINK_SID(muxsensor),CHANNEL_SID(muxsensor),type)

//-----------------------------------------------------------------------------
// Sensor mux status and commands
//-----------------------------------------------------------------------------

// Both of these need the blackboard lock in order to use
I2CBUFFER i2cSmuxReq, i2cSmuxRep;       // for internal manipuliation of / commands to the sensor mux
I2CBUFFER i2cSensorReq, i2cSensorRep;   // for sending and receiving data from the sensors

// What we know about each of the possible sensor muxes
#define isensormuxMax 4
SENSORMUX rgsensormux[isensormuxMax];

// Run the indicated sensor mux if it's not already running
#define FSMUXrun(link)                              \
    ((rgsensormux[link].status == SMUXSTAT_NORMAL)  \
     ? true                                         \
     : SMUXsendCommand(link, SMUXCMD_RUN))

// Explicitly read the status of the indicated sensor mux
#define SMUXreadStatus(link, result)                            \
{                                                               \
    result = SMUXSTAT_NOTHING;                                  \
    FormatI2CReq(i2cSmuxReq, HTSMUX_I2C_ADDR, HTSMUX_STATUS);   \
    if (i2cSendReceiveSensor(link, i2cSmuxReq, i2cSmuxRep, 1))  \
        result = i2cSmuxRep.rgb[0];                             \
}

// Send a command to a sensor mux, and record for our own sake the effects
// of that command on its state. Wait until the command completes, if
// necessary.
BOOL
SMUXsendCommand(I2CLINK link, SMUXCMD command) {
    int msWait = 0;
    BOOL fSuccess = false;
    FormatI2CCmd(i2cSmuxReq, HTSMUX_I2C_ADDR, HTSMUX_COMMAND, command);
    if (i2cSendSensor(link, i2cSmuxReq, 0)) {
        // Update our record of the state of the mux
        switch (command) {
        case SMUXCMD_HALT:
            rgsensormux[link].status = SMUXSTAT_HALT;
            msWait = 50;
            break;
        case SMUXCMD_AUTODETECT:
            rgsensormux[link].status = SMUXSTAT_BUSY;
            msWait = 500;
            break;
        case SMUXCMD_RUN:
            rgsensormux[link].status = SMUXSTAT_NORMAL;
            break;
        }
        fSuccess = true;
    } else {
        // Try to figure out what happened
        SMUXreadStatus(link, rgsensormux[link].status);
    }

    // Wait for the command to complete
    if (msWait > 0) {
        wait1Msec(msWait);

        // Upon completion, an auto-detect command goes into the halt state
        if (SMUXCMD_AUTODETECT == command)
            rgsensormux[link].status = SMUXSTAT_HALT;
    }

    return fSuccess;
}

//-----------------------------------------------------------------------------
// Sensor stuff
//-----------------------------------------------------------------------------

// Set a mode for a sensor attached to a mux. Note that the mux will be
// halted to do so if necessary, but we'll return it to it's previous
// state on exit.
void
SMUXsetModeChannel(OUT BOOL & fSuccess, I2CLINK link, byte channel,
                   SMUXCHMODE mode) {
    // Can't do mode changes when we're in the middle of a scan
    SMUXSTAT statusOnEntry = rgsensormux[link].status;
    fSuccess = (statusOnEntry != SMUXSTAT_BUSY);
    if (fSuccess) {
        // Halt the mux if necessary
        if (rgsensormux[link].status != SMUXSTAT_HALT) {
            fSuccess = SMUXsendCommand(link, SMUXCMD_HALT);
        }
        if (fSuccess) {
            // Set the channel's mode as requested
            FormatI2CCmd(i2cSmuxReq, HTSMUX_I2C_ADDR,
                         HTSMUX_CH_OFFSET + HTSMUX_MODE +
                         (HTSMUX_CH_ENTRY_SIZE * channel), mode);
            fSuccess = i2cSendSensor(link, i2cSmuxReq, 0);

            // Restore the mode of the mux
            if (SMUXSTAT_NORMAL == statusOnEntry) {
                if (!SMUXsendCommand(link, SMUXCMD_RUN))
                    fSuccess = false;
            }
        }
    }
}

BOOL
SMUXreadPortChannel(I2CLINK link, byte channel, I2CBUFFER & result, int cb,
                    int dib) {
    BOOL fSuccess = false;
    if (FSMUXrun(link)) {
        FormatI2CReq(i2cSensorReq, HTSMUX_I2C_ADDR,
                     HTSMUX_I2C_BUF + (HTSMUX_BF_ENTRY_SIZE * channel) + dib);
        if (i2cSendReceiveSensor(link, i2cSensorReq, i2cSensorRep, cb))
            fSuccess = true;
    }
    return fSuccess;
}

#define SMUXReadPort(muxsensor, result, cb, dib) SMUXreadPortChannel(LINK_SID(muxsensor), CHANNEL_SID(muxsensor), result, cb, dib)

//-----------------------------------------------------------------------------
// Analog sensor on muxes
//-----------------------------------------------------------------------------

#define SMUXreadAnalog(muxsensor, result) SMUXreadAnalogChannel(LINK_SID(muxsensor), CHANNEL_SID(muxsensor), result)

void
SMUXreadAnalogChannel(I2CLINK link, byte channel, int &result) {
    result = -1;
    if (FSMUXrun(link)) {
        FormatI2CReq(i2cSensorReq, HTSMUX_I2C_ADDR,
                     HTSMUX_ANALOG + (HTSMUX_AN_ENTRY_SIZE * channel));
        if (i2cSendReceiveSensor(link, i2cSensorReq, i2cSensorRep, 2)) {
            // The analog value is 10 bits. The upper 8 bits are in the
            // first byte and the lower two bits are in the second byte.
            result = Unpack10BitAnalog(i2cSensorRep.rgb, 0);
        }
    }
}

#define SMUXsetAnalogActive(fSuccess, muxsensor)      SMUXsetAnalog(fSuccess, LINK_SID(muxsensor), CHANNEL_SID(muxsensor), SMUXCHMODE_DIG0_HIGH)
#define SMUXsetAnalogInactive(fSuccess, muxsensor)    SMUXsetAnalog(fSuccess, LINK_SID(muxsensor), CHANNEL_SID(muxsensor), SMUXCHMODE_NONE)

#define SMUXsetAnalog(fSuccess, link, channel, mode)        \
{                                                           \
    fSuccess = false;                                       \
    if (CheckSensorType(link, channel, MUXSENST_ANALOG)) {  \
        SMUXsetModeChannel(fSuccess, link, channel, mode);  \
    }                                                       \
}

//-----------------------------------------------------------------------------
// Initialization
//-----------------------------------------------------------------------------

// Have the sensor mux poke around to find out what's connected to
// it. Then talk to it to dig out all the types of sensors connected.
#define SMUXScanPorts(fSuccess, link)                                   \
{                                                                       \
    /* Assume failure until proven otherwise */                         \
    fSuccess = false;                                                   \
                                                                        \
    /* Can't start a scan when we're in the middle of one already */    \
    if (SMUXSTAT_BUSY != rgsensormux[link].status) {                    \
        /* Caller is responsible for halting us */                      \
        if (SMUXSTAT_HALT == rgsensormux[link].status) {                \
            /* Do the actual scan */                                    \
            if (SMUXsendCommand(link, SMUXCMD_AUTODETECT)) {            \
                fSuccess = true;                                        \
                                                                        \
                /* Query the sensor types that were found */            \
                for (int i = 0; fSuccess && i < 4; i++) {               \
                    rgsensormux[link].rgtype[i] = MUXSENST_NONE;        \
                    FormatI2CReq(i2cSmuxReq, HTSMUX_I2C_ADDR, HTSMUX_CH_OFFSET + HTSMUX_TYPE + (HTSMUX_CH_ENTRY_SIZE * i)); \
                    if (i2cSendReceiveSensor(link, i2cSmuxReq,          \
                                             i2cSmuxRep, 1)) {          \
                        rgsensormux[link].rgtype[i] = i2cSmuxRep.rgb[0]; \
                        /*TRACE(("msensor_S%d_%d=%d", link+1, i+1, (int)(rgsensormux[link].rgtype[i]))); */ \
                    } else {                                            \
                        fSuccess = false;  /* send/receive error */     \
                    }                                                   \
                }                                                       \
                                                                        \
                /* Hitechnic Prototype boards apparently have issues with a 'galloping buffer problem'.     */ \
                /* What we (apparently) do here is forcibly set the number of bytes that the smux should    */ \
                /* ask of the proto board explicitly to 14 to correct for same.                             */ \
                for (int i = 0; fSuccess && i < 4; i++) {               \
                    if (MUXSENST_PROTO == rgsensormux[link].rgtype[i]) { \
                        FormatI2CCmd(i2cSmuxReq, HTSMUX_I2C_ADDR, HTSMUX_CH_OFFSET + HTSMUX_I2C_COUNT + (HTSMUX_CH_ENTRY_SIZE * i), 14); \
                        if (!i2cSendSensor(link, i2cSmuxReq, 0)) {      \
                            rgsensormux[link].rgtype[i] = MUXSENST_NONE; \
                        }                                               \
                    }                                                   \
                }                                                       \
            }                                                           \
        }                                                               \
    }                                                                   \
}

//-----------------------------------------------------------------------------
// Core routines
//-----------------------------------------------------------------------------

/* Initialize one HiTechnic sensor multiplexor */
void
InitializeSensorMux(IN OUT STICKYFAILURE &fOverallSuccess, int link) {
    /* avoid multiple initializations */
    if (!rgsensormux[link].fActive) {
        /* avoid lock not held warnings, might be recursive */
        LockBlackboard();

        rgsensormux[link].fActive = true;

        /* Tell the system about our sensor mux */
        TSensorTypes type = sensorI2CCustomFast;
        if (SensorType[link] != type) {
            SensorType[link] = type;
            /* Exact wait duration is lore and rumor */
            wait1Msec(10);
        }

        /* Put the mux in a known state. We need the halt state anyway  */
        /* in order to scan the ports.                                  */
        SMUXsendCommand(link, SMUXCMD_HALT);

        /* Tell the SMUX to scan its ports for connected sensors */
        BOOL fSuccess = true;
        SMUXScanPorts(fSuccess, link);  /* will take 500ms */

        ReleaseBlackboard();

        if (fOverallSuccess)
            fOverallSuccess = fSuccess;
        TraceInitializationResult1("sensMux(%d)", link + 1, fOverallSuccess);
    }
}

#define InitializeSensorMuxes(fOverallSuccess)      \
{                                                   \
    for (int i = 0; i < isensormuxMax; i++) {       \
        rgsensormux[i].fActive = false;             \
        rgsensormux[i].status = SMUXSTAT_NOTHING;   \
        rgsensormux[i].rgtype[0] = MUXSENST_NONE;   \
        rgsensormux[i].rgtype[1] = MUXSENST_NONE;   \
        rgsensormux[i].rgtype[2] = MUXSENST_NONE;   \
        rgsensormux[i].rgtype[3] = MUXSENST_NONE;   \
    }                                               \
}
