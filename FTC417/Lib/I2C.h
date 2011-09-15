//
// I2C.h
//
//-------------------------------------------------------------------------------------
// I2C bus
//-------------------------------------------------------------------------------------

// Constants that name the ports outside of the back of the NXT
// in such a way that the I2C messaging primatives can talk to them.
//
// Note that these need to be macros so they can be used as the value
// of #defines which connote the presence and absence of sensors, allowing
// code to conditionally compile depending on whether a particular sensor is
// configured to be present or not.

#define I2CLINK         int
//
#define I2CLINK_1       0
#define I2CLINK_2       1
#define I2CLINK_3       2
#define I2CLINK_4       3

// A data structure defining a buffer used to send or receive I2C message packets
typedef struct
    {
    // In write requests,
    //      rgb[0] is message length
    //      rgb[1] is i2c address
    //      rgb[2] is offset to write
    //      remainder is data to send
    // In read requests,
    //      rgb[0] is message length
    //      rgb[1] is i2c address
    //      rgb[2] is offset to read
    // In replies,
    //      reply data starts at rgb[0]
    //
    ubyte rgb[3+16];    // 16 bytes is max message size + 3 bytes max overhead
    } I2CBUFFER;

// What's the destination address in an request packet
#define i2cCbOf(req)                req.rgb[0]
#define i2cAddrOf(req)              req.rgb[1]
#define i2cOffsetOf(req)            req.rgb[2]
#define IbI2CPayload(i)             (3+(i))
#define CbI2CPayload(cbPayload)     ((cbPayload) + 2)

// Initialize an I2C buffer to zero
#define ZeroI2Buffer(buf)           memset(buf, 0, sizeof(buf))

#define FormatI2CPrim(req, addr, ibOffset, cbPayload)                               \
    {                                                                               \
    ZeroI2Buffer(req);                                                              \
    i2cCbOf(req)     = CbI2CPayload(cbPayload); /* message size */                  \
    i2cAddrOf(req)   = (addr);                  /* I2C address  */                  \
    i2cOffsetOf(req) = (ibOffset);              /* offset */                        \
    }

// Format a I2C message that requests data from a certain I2C address and offset
#define FormatI2CReq(req, addr, ib)                                                 \
    FormatI2CPrim(req, addr, ib, 0)


//-------------------------------------------------------------------------------------

// Wait for the i2c bus to become available. Return true the previous bus transaction
// was successful, false otherwise.
#define i2cBusWait(fSuccess, link)                                                  \
    {                                                                               \
    fSuccess = true;                                                                \
    BOOL fDone = false;                                                             \
    while (!fDone)                                                                  \
        {                                                                           \
        switch ((short)nI2CStatus[link])                                            \
            {                                                                       \
        case NO_ERR:                    /* success */                               \
            fSuccess = true;                                                        \
            fDone = true;                                                           \
            break;                                                                  \
                                                                                    \
        case ERR_COMM_CHAN_INVALID:     /* not a valid link # */                    \
        case ERR_COMM_CHAN_NOT_READY:   /* wrong kind of link */                    \
        case ERR_COMM_BUS_ERR:          /* error occured in prev bus transaction */ \
            fSuccess = false;                                                       \
            fDone = true;                                                           \
            break;                                                                  \
                                                                                    \
        case STAT_COMM_PENDING:         /* i2c transaction not yet complete */      \
        default:                                                                    \
            EndTimeSlice();             /* don't hog the CPU */                     \
            break;                                                                  \
            }                                                                       \
        }                                                                           \
    }

// Try to clear the i2c bus error state by sending dummy packets
#define i2cClearError(link, i2cAddr)                                                \
    {                                                                               \
    byte rgb[2];                                                                    \
    rgb[0] = 1; /* cb */                                                            \
    rgb[1] = i2cAddr;                                                               \
    for (int i = 0; i < 5; i++)                                                     \
        {                                                                           \
        sendI2CMsg((tSensors)link, rgb[0], 0);                                      \
        }                                                                           \
    }

// Send the indicated I2C request packet. cbReply is the number of bytes
// (possibly zero) expected in the corresponding reply.
#define i2cSend(fSuccess, link, req, cbReply)                                       \
    {                                                                               \
    fSuccess = true;                                                                \
                                                                                    \
    /* Get access to the bus */                                                     \
    i2cBusWait(fSuccess, link);                                                     \
    if (!fSuccess)                                                                  \
        {                                                                           \
        i2cClearError(link, i2cAddrOf(req));                                        \
        i2cBusWait(fSuccess, link);                                                 \
        }                                                                           \
                                                                                    \
    if (fSuccess)                                                                   \
        {                                                                           \
        /* Send the request */                                                      \
        sendI2CMsg((tSensors)link, req.rgb[0], cbReply);                            \
                                                                                    \
        /* Wait until it went out, or send again if failure */                      \
        i2cBusWait(fSuccess, link);                                                 \
        if (!fSuccess)                                                              \
            {                                                                       \
            i2cClearError(link, i2cAddrOf(req));                                    \
            sendI2CMsg((tSensors)link, req.rgb[0], cbReply);                        \
            i2cBusWait(fSuccess, link);                                             \
            }                                                                       \
        }                                                                           \
    }

BOOL i2cSendSensor(I2CLINK link, I2CBUFFER& req, int cbReply)
    {
    CheckLockHeld(lockBlackboard);
    BOOL fSuccess;
    i2cSend(fSuccess, link, req, cbReply);
    return fSuccess;
    }
BOOL i2cSendDaisy(I2CLINK link, I2CBUFFER& req, int cbReply)
    {
    CheckLockHeld(lockDaisy);
    BOOL fSuccess;
    i2cSend(fSuccess, link, req, cbReply);
    return fSuccess;
    }

BOOL i2cSendReceiveSensor(I2CLINK link, I2CBUFFER& req, I2CBUFFER& rep, int cbReply)
    {
    if (i2cSendSensor(link, req, cbReply))
        {
        // We needn't wait for the bus here, as i2cSend did that for us
        ZeroI2Buffer(rep);
        readI2CReply((tSensors)link, rep.rgb[0], cbReply);
        return true;
        }
    else
        return false;
    }
BOOL i2cSendReceiveDaisy(I2CLINK link, I2CBUFFER& req, I2CBUFFER& rep, int cbReply)
    {
    if (i2cSendDaisy(link, req, cbReply))
        {
        // We needn't wait for the bus here, as i2cSend did that for us
        ZeroI2Buffer(rep);
        readI2CReply((tSensors)link, rep.rgb[0], cbReply);
        return true;
        }
    else
        return false;
    }

//-------------------------------------------------------------------------------------

#define FormatI2CCmd(req, addr, reg, cmd)                               \
    {                                                                   \
    FormatI2CPrim(req, addr, reg, 1);                                   \
    req.rgb[IbI2CPayload(0)]= (cmd);             /* command */          \
    }

//-------------------------------------------------------------------------------------

#define ShiftedCast(value, type, shift)                         ((type)(value) << (shift))
#define AssembleBytes2(rep, type, shft0, shft1)                 (ShiftedCast(rep.rgb[0], type, shft0) + ShiftedCast(rep.rgb[1], type, shft1))
#define AssembleBytes4(rep, type, shft0, shft1, shft2, shft3)   (ShiftedCast(rep.rgb[0], type, shft0) + ShiftedCast(rep.rgb[1], type, shft1) + ShiftedCast(rep.rgb[2], type, shft2) + ShiftedCast(rep.rgb[3], type, shft3))

#define Unpack10BitAnalog(rgb, ib)                              (ShiftedCast(rgb[ib],int,2) + (rgb[ib+1]&0x03))

#define UnpackInt(rgb,ib)                                   \
        (ShiftedCast(rgb[0+(ib)], int, 8) +                 \
         ShiftedCast(rgb[1+(ib)], int, 0))

#define UnpackLong(rgb,ib)                                  \
        (ShiftedCast(rgb[0+(ib)], long, 24) +               \
         ShiftedCast(rgb[1+(ib)], long, 16) +               \
         ShiftedCast(rgb[2+(ib)], long,  8) +               \
         ShiftedCast(rgb[3+(ib)], long,  0))

#define PackLong(rgb, ib, valParam)                 \
    {                                               \
    long value = (long)(valParam);                  \
    rgb[(ib)]   = ((( value ) >> 24) & 0xff);       \
    rgb[(ib)+1] = ((( value ) >> 16) & 0xff);       \
    rgb[(ib)+2] = ((( value ) >>  8) & 0xff);       \
    rgb[(ib)+3] = ((( value )      ) & 0xff);       \
    }
