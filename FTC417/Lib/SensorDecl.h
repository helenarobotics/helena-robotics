//
// SensorDecl.h
//
// Sensor information etc made visible to each robot-specific configuration .h.
//
// Kudos and gracious thanks to Xander Soldaat (mightor_at_gmail.com) for his
// original 3rd party driver framework, without the guidance and instruction
// of which the code herein would not have been possible.

//------------------------------------------------------------------------------------
// Sensors (general)
//------------------------------------------------------------------------------------

// Member variables to be found in every sensor
#define COMMON_SENSOR_DATA          \
    int sensnm;                     \
    int cReadRequest;               \
    int cDetectRequest;             \
    BOOL fDetected;                 \
    string displayName;

// 'Base class' for all the various kinds of sensors
typedef struct
    {
    COMMON_SENSOR_DATA
    } SENSOR;

// Macros for naming sensor connections.
// Note: both mux and channel in SensorOnMux and SensorDirect are 1-based, not zero-based
#define SensorOnMux(mux,channel)    (((((mux)-1)*4 + (channel) -1)) | 0x8000)
#define SensorDirect(channel)       ((channel)-1)
#define sensnmNone                  0x7FFF
#define SensorIsDefined(sensnm)     (((sensnm) & sensnmNone) != sensnmNone)

// We remember if the sensor is on a mux or not
#define FMux(sensor)        FMuxSensnm(sensor.sensnm)
#define FDirect(sensor)     (!FMux(sensor))
#define FMuxSensnm(sensnm)  (((sensnm) & 0x8000) != 0)

// Macros for accessing the id of the sensor (SID) in mux and direct versions
#define SID_MUX(sensor)    ((SENSORONMUX)(sensor.sensnm))  // given that FMux(sensor) is true, return the identifier used for talking to that sensor on that mux
#define SID_DIRECT(sensor) ((tSensors)(sensor.sensnm))     // given that FDirect(sensor) is true, return the identifier used for talking to that sensor

// Disassemblying sensor names
#define LINK_DIRECT(sensor)  ((I2CLINK)(sensor.sensnm))         // given that FDirect(sensor) is true, return the I2CLINK on which to talk to it
#define LINK_MUX(sensor)     LINK_SID(sensor.sensnm)            // given that FMux(sensor) is true, return the I2CLINK on which to talk to its mux
#define CHANNEL_MUX(sensor)  CHANNEL_SID(sensor.sensnm)         // given that FMux(sensor) is true, return the (zero-based) channel on which it lives on its mux

#define LINK_SID(sid)       ((I2CLINK)(((sid) & 0x7FFF) / 4))
#define CHANNEL_SID(sid)    (((sid) & 0x7FFF) % 4)

// The actual InitializeSensor() work is done in a function to save space.
// The parameter to that function needs to see a sensor structure of type 'SENSOR'
// instead of ACCELSENSOR, ANGLESENSOR, etc. So we do a C++-like base-class cast
// on the parameter.
#define InitializeSensor(fOverallSuccess, nm, sensor, sensnmVal, type)                 \
    InitializeSensor_(fOverallSuccess, nm, BASE_CAST(SENSOR, sensor), sensnmVal, type)

void InitializeSensor_(IN OUT STICKYFAILURE& fOverallSuccess, string nm, SENSOR& sensor, int sensnmVal, MUXSENST type);

#define TraceSensorInitializationResult(nm, fValue)     TraceInitializationResult1("sensor[%s]", nm, fValue)

//------------------------------------------------------------------------------------
// Acceleration sensor
//------------------------------------------------------------------------------------

typedef struct
    {
    COMMON_SENSOR_DATA
    int x, y, z;
    } ACCELSENSOR;

#define InitializeAccelSensor(fOverallSuccess, nm, sensor, sensnmVal)                   \
    {                                                                                   \
    InitializeSensor(fOverallSuccess, nm, sensor, sensnmVal, MUXSENST_ACCEL);           \
    sensor.x = sensor.y = sensor.z = 0;                                                 \
    TraceInit("InitializeAccelSensor", fOverallSuccess);                                \
    }

//------------------------------------------------------------------------------------
// Angle sensor
//------------------------------------------------------------------------------------

typedef struct
    {
    COMMON_SENSOR_DATA
    float       deg;              // the actual, gearing-compensated value of the sensor (NB: NOT normalized to +-180 deg)
    float       degTarget;        // target value to detect
    COMPARISON  comparison;       // how to compare deg to degTarget
                                  //
    float       degUnwrapped;     // the un-gearing-compensated value, but with rotational wraps accounted for.
    float       degRaw;           // current value of the angle sensor. In units of deg CCW.
    float       degRawPrev;       // previous reading of the sensor. Used for detecting wraps.
    float       gearing;          // ratio between actual angle and measured rotation angle of interest
    float       degStallPrev;     // used for stall detection
    MILLI       msLastOkStall;    // used for stall detection
    } ANGLESENSOR;

#define InitializeAngleSensor(fOverallSuccess, nm, sensor, sensnmVal)                           \
    {                                                                                           \
    InitializeSensor(fOverallSuccess, nm, sensor, sensnmVal, MUXSENST_ANGLE);                   \
    sensor.deg        = 0;                                                                      \
    sensor.degTarget  = 0;                                                                      \
    sensor.comparison = COMPARE_GT;                                                             \
    sensor.degUnwrapped = 0;                                                                    \
    sensor.degRaw     = 0;                                                                      \
    sensor.degRawPrev = 0;                                                                      \
    sensor.gearing    = 1.0;                                                                    \
    sensor.degStallPrev = 0;                                                                    \
    sensor.msLastOkStall = 0;                                                                   \
    TraceSensorInitializationResult(nm, fOverallSuccess);                                       \
    }

//------------------------------------------------------------------------------------
// Color sensor
//------------------------------------------------------------------------------------

typedef struct
    {
    COMMON_SENSOR_DATA
    int     red, green, blue;                     // current values
    int     targetRed, targetGreen, targetBlue;   // targets
    int     tolerance;                            // how good of a match the target readings have to be
    } COLORSENSOR;

#define InitializeColorSensor(fOverallSuccess, nm, sensor, sensnmVal)                       \
    {                                                                                       \
    InitializeSensor(fOverallSuccess, nm, sensor, sensnmVal, MUXSENST_COLOR);               \
    sensor.red       = sensor.green       = sensor.blue       = 0;                          \
    sensor.targetRed = sensor.targetGreen = sensor.targetBlue = 0;                          \
    sensor.tolerance = 20;                                                                  \
    TraceSensorInitializationResult(nm, fOverallSuccess);                                   \
    }

//------------------------------------------------------------------------------------
// Compass sensor
//------------------------------------------------------------------------------------

typedef struct
    {
    COMMON_SENSOR_DATA
    ANGLE         value;       // current reading of the compass
    ANGLE         bias;   // value to subtract from raw readings to get value
    } COMPASSSENSOR;

#define InitializeCompassSensor(fOverallSuccess, nm, sensor, sensnmVal)                     \
    {                                                                                       \
    InitializeSensor(fOverallSuccess, nm, sensor, sensnmVal, MUXSENST_COMPASS);             \
    sensor.bias = sensor.value = 0.0;                                                       \
    TraceSensorInitializationResult(nm, fOverallSuccess);                                   \
    }

//------------------------------------------------------------------------------------
// EOPD sensor
//------------------------------------------------------------------------------------

typedef enum { EOPD_RAW, EOPD_PROCESSED } EOPD_CONFIG;
typedef enum { EOPD_SHORT, EOPD_LONG    } EOPD_RANGE;

typedef struct
    {
    COMMON_SENSOR_DATA
    BOOL        fReadProcessed;   // if true, read processed values; false raw values
    int         value;            // current value of the sensor when last we looked
    int         target;           // target value that we are trying to detect
    COMPARISON  comparison;       // how to compare the current to the target value
    int         countDetected;    // how many times has the comparison fired?
    int         countRequired;    // how many times the comparison must fire in order to be 'detected'
    MILLI       msDetectDelay;    // how many ms after drive start we should wait before arming
    MILLI       msDetectThreshold; // system time after which we are allowed to detect
    } EOPDSENSOR;

void InitializeEopdSensor(STICKYFAILURE& fOverallSuccess, string nm, EOPDSENSOR& sensor, int sensnmVal, EOPD_CONFIG processed, EOPD_RANGE range);


//------------------------------------------------------------------------------------
// Gryo sensor
//------------------------------------------------------------------------------------

typedef struct
    {
    COMMON_SENSOR_DATA
    float bias;                 // the offset bias to apply to raw readings from the sensor. Will be ~620ish, typically
    float deadbandMax;          // upper limit of band around the bias that's to be deemed equivalent to zero
    float deadbandMin;          // lower limit of band
    float degCWPerSScale;       // scaling constant to calibrate the rate of the gyro. Will be very close to 1.0
    float degCWPerS;            // the de-biased current reading the gyro (in degrees, CW /* note! */)
    int32 msIntegratePrev;      // when we last updated the integration; zero == never
    float degCWPerSPrev;        // previous value of degCWPerS. used in trapezoidal integration

    // public
    float deg;                  // the accumulated degrees of rotation, CCW /* note */, NOT normalized (see comment below)
    float degTarget;            // target value to detect
    COMPARISON comparison;      // how to compare the current to the target value

    // Debugging support
    float degDetected;          // value of deg when detection (first) occurs
    int32 cRead;                // number of time ReadGyroSensor has been called
    } GYROSENSOR;

void InitializeGyroSensor(STICKYFAILURE& fOverallSuccess, string nm, GYROSENSOR& sensor, int sensnmVal, int cSample=300, MILLI msWait=10);

//------------------------------------------------------------------------------------
// Magnetic sensor
//------------------------------------------------------------------------------------

#define cdegMagDetectionRecordMax  4   // in 'Get-Over-It', there are only four magnetic batons on the field, total

typedef enum
    {
    VALUECHANGE_ASCENDING   = -1,
    VALUECHANGE_STEADY      = 0,
    VALUECHANGE_DESCENDING  = 1,
    } VALUECHANGE;

typedef struct
    {
    COMMON_SENSOR_DATA
    int         value;                  // current reading if the mag, compensated by the bias
    int         valuePrev;
    int         bias;                   // the 'zero point' of the sensor
    int         threshold;              // necessary threshold above bias before detection is said to occur
    VALUECHANGE valueChange;
    VALUECHANGE valueChangePrev;

    // record of associated angle sensor when detection occured
    int         isensangleAssociated;                                   // the 'index' of an associated angle sensor; -1 if none
    int         idegDetectionRecordNext;                                // slot at which to write the next detection record
    float       rgdegDetectionRecord[cdegMagDetectionRecordMax];        // where the angle sensor was at time of detection
    BOOL        rgfDetectionRecordValid[cdegMagDetectionRecordMax];     // corresponding entry in rgdegDetectionRecord valid iff this entry true
    } MAGNETICSENSOR;

void InitializeMagneticSensor(STICKYFAILURE& fOverallSuccess, string nm, MAGNETICSENSOR& sensor, int sensnmVal);

//------------------------------------------------------------------------------------
// (Ultra)Sonic sensor
//------------------------------------------------------------------------------------

typedef struct
    {
    COMMON_SENSOR_DATA
    int     cm;                     // current reading in cm
    MILLI   msLastRead;             // when was this sensor last read (nSysTime is the clock)
    } SONICSENSOR;

#define InitializeSonicSensor(fOverallSuccess, nm, sensor, sensnmVal)                   \
    {                                                                                   \
    InitializeSensor(fOverallSuccess, nm, sensor, sensnmVal, MUXSENST_SONIC);           \
    sensor.cm = 255;                                                                    \
    sensor.msLastRead = nSysTime;                                                       \
    TraceSensorInitializationResult(nm, fOverallSuccess);                               \
    }


//------------------------------------------------------------------------------------
// Touch sensor
//------------------------------------------------------------------------------------

typedef struct
    {
    COMMON_SENSOR_DATA
    BOOL fValue;
    } TOUCHSENSOR;

#define InitializeTouchSensor(fOverallSuccess, nm, sensor, sensnmVal)                                           \
    {                                                                                                           \
    InitializeSensor(fOverallSuccess, nm, sensor, sensnmVal, MUXSENST_ANALOG);                                  \
    sensor.fValue = false;                                                                                      \
    /* change the mode in the direct case. not sure this is needed */                                           \
    if (!FMux(sensor))                                                                                          \
        {                                                                                                       \
        if ((SensorType[SID_DIRECT(sensor)] != sensorTouch) || SensorMode[SID_DIRECT(sensor)] != modeBoolean)   \
            {                                                                                                   \
            SetSensorType(SID_DIRECT(sensor), sensorTouch);                                                     \
            SetSensorMode(SID_DIRECT(sensor), modeBoolean);                                                     \
            wait1Msec(10);                                                                                      \
            }                                                                                                   \
        }                                                                                                       \
    TraceSensorInitializationResult(nm, fOverallSuccess);                               \
    }
