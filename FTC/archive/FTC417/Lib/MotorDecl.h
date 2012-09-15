//
// MotorDecl.h
//

// Constants for specifying motor properties in InitializeMotor()
typedef enum {
    // Use either this:
    MOTORPROP_NONE = 0x00,

    // Or a combination of these:
    MOTORPROP_REFLECTED = 0x01,
    MOTORPROP_ENCODER = 0x02,
    MOTORPROP_NOSTALLCHECK = 0x04,
} MOTORPROP;

typedef struct {
    BOOL fActive;
    int imotor;                 // index of this motor in rgmotor
    int imotorPaired;           // if there is a motor which is paired with this one for added oomph, this is its index in rgmotor
    BOOL fReflected;            // is this motor manually reflected / reversed?
    BOOL fHasEncoder;

    int imotorctlr;             // the controller with which this motor is associated
    int imtr;                   // the index of this controller on that controller (zero-based)
    string displayName;         // a user-comprehensible name for this motor

    // Stall-related state
    BOOL fStallCheck;           // should we check this motor for stall?
    ENCOD encStallPrev;         // encoder value at last stall check on this motor
    MILLI msLastOkStall;        // time at which stall check was last carried out on this motor
    MILLI msLastEncStall;       // last time we read the encoders in stall logic
} MOTOR;

MOTOR rgmotor[8];

void InitializeMotor(IN OUT STICKYFAILURE &fOverallSuccess, MOTOR &motor,
    MOTORCONTROLLER &controllerParam, ubyte jmtr /*one based */ ,
    string displayNameParam, int prop);
