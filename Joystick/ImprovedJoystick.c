////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                    HiTechnic Servo/Motor Controller Device Driver  - UPDATED 1/08/2009//
//
// With the TETRIX system, the PC Controller Station sends messages over Bluetooth to the NXT containing
// current settings of a PC joystick. The joystick settings arrive using the standard NXT BLuetooth
// "message mailbox" facility.
//
// This is a short ROBOTC program to extract the joystick data from a mailbox message and format it
// into data structure that can be easily acccessed by end user programs.
//
// The driver resides in a separate file that can be simply added to any user program with a
// "#include" preprocessor directive. End users should not have to modify this program and can use
// it as is.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////

#if (defined(NXT) || defined(TETRIX)) && (_TARGET == "Robot") && !defined(NaturalLanguage)
// Automatically start this task when the main user program starts.
#pragma autoStartTasks
#elif (defined(VEX2) || defined(NXT) || defined(TETRIX)) && (_TARGET == "VirtWorld")
//Virtual Worlds - No need to use most of this driver, so don't start the ReadMsgFromPC task.
#elif defined(NaturalLanguage)
//Manually Start for Natural Language - Otherwise, ReadMsgFromPC will never let the NL program end.
#elif (defined(NXT) || defined(TETRIX)) && (_TARGET == "Emulator")
  #warning "This driver may not work with 'Emulator'."
#else
  #error "This driver is not supported on this platform."
#endif

#pragma systemFile

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                    Joystick Information Structure
//
//
// Structure containing info from Joystick.
//
// "short" variables are used to prevent conversion errors. For example, negating a byte variable with
// value -128 results in -128 because -128 does not fit in a byte!
//
////////////////////////////////////////////////////////////////////////////////////////////////////////

#if (_TARGET == "Robot")
typedef struct
{
    bool UserMode;          // Autonomous or Telep-Operated mode
    bool StopPgm;           // Stop program

    short joy1_x1;           // -128 to +127
    short joy1_y1;           // -128 to +127
    short joy1_x2;           // -128 to +127
    short joy1_y2;           // -128 to +127
    short joy1_Buttons;      // Bit map for 12-buttons
    short joy1_TopHat;       // value -1 = not pressed, otherwise 0 to 7 for selected "octant".

    short joy2_x1;           // -128 to +127
    short joy2_y1;           // -128 to +127
    short joy2_x2;           // -128 to +127
    short joy2_y2;           // -128 to +127
    short joy2_Buttons;      // Bit map for 12-buttons
    short joy2_TopHat;       // value -1 = not pressed, otherwise 0 to 7 for selected "octant".
} TJoystick;
TJoystick joystick;      // User defined variable access
#else
TPCJoystick joystick;
#endif

#if defined(hasJoystickMessageOpcodes)
intrinsic void getJoystickSettings(TJoystick &joystick)
  asm(opcdSystemFunctions, byte(sysFuncGetJoysticks),
      variableRefRAM(joystick));
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// "Macro" to get a non-volatile copy of the last joystick settings so
//
////////////////////////////////////////////////////////////////////////////////////////////////////////

#if (_TARGET == "Robot")

#define getJoystickSettings(joystick) 	memcpy(joystick, joystickCopy, sizeof(joystick))

short joy1Btn(int btn)
{
   return ((joystick.joy1_Buttons & (1 << (btn - 1))) != 0);
}

short joy2Btn(int btn)
{
   return ((joystick.joy2_Buttons & (1 << (btn - 1))) != 0);
}

#else

#define getJoystickSettings getPCJoystickSettings
bool joy1Btn(int btn)
{
    return ((joystick.joy1_Buttons & (1 << (btn - 1))) != 0);
}

#endif

// Code Below Does Not Apply to Virtual/Emulator Robots
#if (defined(NXT) || defined(TETRIX)) && (_TARGET == "Robot")
const TMailboxIDs kJoystickQueueID = mailbox1;
TJoystick joystickCopy;  // Internal buffer to hold the last received message from the PC. Do not use

long ntotalMessageCount = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                        Receive Messages Task
//
// Dedicated task that continuously polls for a Bluetooth message from the PC containing the joystick
// values. Operaton of this task is nearly transparent to the end user as the earlier "#pragma autoStartTasks"
// statement above will cause it to start running as soon as the user program is started.
//
// The task is very simple. It's an endless loop that continuously looks for a Bluetooth mailbox
// message from the PC. When one is found, it reformats and copies the contents into the internal
// "joystickCopy" buffer.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
#if (_TARGET == "Robot")
bool bDisconnected = false;
bool bOverrideJoystickDisabling = false;
long nNoMessageCounterLimit = 750;
long nNoMessageCounter = 0;

task readMsgFromPC()
{
    TFileIOResult nBTCmdRdErrorStatus;
    const int kMaxSizeOfMessage = 18;
    sbyte tempBuffer[kMaxSizeOfMessage];

    // Initialize setting to default values in case communications with PC is broken.

    //joystickCopy.TeamColor = false;
    joystickCopy.UserMode = false;
    joystickCopy.StopPgm  = true;

    joystickCopy.joy1_x1 = 0;
    joystickCopy.joy1_y1 = 0;
    joystickCopy.joy1_x2 = 0;
    joystickCopy.joy1_y2 = 0;
    joystickCopy.joy1_Buttons = 0;
    joystickCopy.joy1_TopHat = -1;

    joystickCopy.joy2_x1 = 0;
    joystickCopy.joy2_y1 = 0;
    joystickCopy.joy2_x2 = 0;
    joystickCopy.joy2_y2 = 0;
    joystickCopy.joy2_Buttons = 0;
    joystickCopy.joy2_TopHat = -1;

    bDisconnected = false;
    while (true)
    {
        // Check to see if a complete message is available.
        int nSizeOfMessage = cCmdMessageGetSize(kJoystickQueueID);
        if (nSizeOfMessage < sizeof(tempBuffer))
        {
            // No complete message available
            nNoMessageCounter++;
            if (nNoMessageCounter > nNoMessageCounterLimit)
            {
                // We've reached the limit
                if (!bOverrideJoystickDisabling && !bDisconnected)
                {
                    bool bTempUserMode = joystickCopy.UserMode;
                    bool bTempStopPgm = joystickCopy.StopPgm;

                    // Turn off all joystick settings, but make sure to
                    // restore the UserMode and StopPgm settings from
                    // the last message we received.

                    hogCPU();   // grab CPU for duration of critical section

                    memset(joystickCopy, 0, sizeof(joystickCopy));

                    joystickCopy.UserMode = bTempUserMode;
                    joystickCopy.StopPgm = bTempStopPgm;
                    joystickCopy.joy1_TopHat = -1;
                    joystickCopy.joy2_TopHat = -1;

                    releaseCPU(); // end of critical section
                }
                bDisconnected = true;
            }
        } else {
            // Found at least one complete message, so we're re-connected.
            nNoMessageCounter = 0;
            bDisconnected = false;

             // There may be more than one message in the queue. We want to get to the last received
             // message and discard the earlier "stale" messages. This loop simply discards all but
             // the last message.
             while (nSizeOfMessage >= sizeof(tempBuffer)) {
                 // Read the data from the queue

                 nBTCmdRdErrorStatus = cCmdMessageRead(tempBuffer, sizeof(tempBuffer), kJoystickQueueID);
                 nBTCmdRdErrorStatus = nBTCmdRdErrorStatus; //Get rid of info message

                 // Check the size of the queue again to see how much more data is in the queue
                nSizeOfMessage = cCmdMessageGetSize(kJoystickQueueID);
             }

            // tempBuffer now contains the last complete message we received.

             hogCPU();   // grab CPU for duration of critical section

             joystickCopy.UserMode           = (bool)tempBuffer[1];
             joystickCopy.StopPgm            = (bool)tempBuffer[2];

             joystickCopy.joy1_x1            = tempBuffer[3];
             joystickCopy.joy1_y1            = tempBuffer[4];
             joystickCopy.joy1_x2            = tempBuffer[5];
             joystickCopy.joy1_y2            = tempBuffer[6];
             joystickCopy.joy1_Buttons       = (tempBuffer[7] & 0x00FF) | (tempBuffer[8] << 8);
             joystickCopy.joy1_TopHat        = tempBuffer[9];

             joystickCopy.joy1_y1            = -joystickCopy.joy1_y1; // Negate to "natural" position
             joystickCopy.joy1_y2            = -joystickCopy.joy1_y2; // Negate to "natural" position

             joystickCopy.joy2_x1            = tempBuffer[10];
             joystickCopy.joy2_y1            = tempBuffer[11];
             joystickCopy.joy2_x2            = tempBuffer[12];
             joystickCopy.joy2_y2            = tempBuffer[13];
             joystickCopy.joy2_Buttons       = (tempBuffer[14] & 0x00FF) | (tempBuffer[15] << 8);
             joystickCopy.joy2_TopHat        = tempBuffer[16];

             joystickCopy.joy2_y1            = -joystickCopy.joy2_y1; // Negate to "natural" position
             joystickCopy.joy2_y2            = -joystickCopy.joy2_y2; // Negate to "natural" position

             releaseCPU(); // end of critical section

            ++ntotalMessageCount;
        }
        // Give other tasks a chance to run
//        EndTimeSlice();
        wait1Msec(4);
    }
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////
//
//                                        displayDiagnostics
//
// THis task will display diagnostic information about a TETRIX robot on the NXT LCD.
//
// If you want to use the LCD for your own debugging use, call the function
// "disableDiagnosticsDisplay()
//
///////////////////////////////////////////////////////////////////////////////////////////

bool bDisplayDiagnostics = false;  // Set to false in user program to disable diagnostic display

void getUserControlProgram(string &sFileName);

#if defined(TETRIX) && (_TARGET == "Robot")

void disableDiagnosticsDisplay()
{
    bDisplayDiagnostics = false;   // Disable diagnostic display
}

task displayDiagnostics()
{
    string sFileName;
    bNxtLCDStatusDisplay = true;
    getUserControlProgram(sFileName);

    while (true)
    {
        if (bDisplayDiagnostics)
        {
            nxtDisplayTextLine(6, "Teleop FileName:");
            nxtDisplayTextLine(7, sFileName);

            getJoystickSettings(joystick);                   //Update variables with current joystick values

            if (joystick.StopPgm)
                nxtDisplayCenteredTextLine(1, "Wait for Start");
            else if (joystick.UserMode)
                nxtDisplayCenteredTextLine(1, "TeleOp Running");
            else
                nxtDisplayCenteredTextLine(1, "Auton Running");

            if ( externalBatteryAvg < 0)
                nxtDisplayTextLine(3, "Ext Batt: OFF");       //External battery is off or not connected
            else
                nxtDisplayTextLine(3, "Ext Batt:%4.1f V", externalBatteryAvg / (float) 1000);

            nxtDisplayTextLine(4, "NXT Batt:%4.1f V", nAvgBatteryLevel / (float) 1000);   // Display NXT Battery Voltage

            nxtDisplayTextLine(5, "FMS Msgs: %d", ntotalMessageCount);   // Display Count of FMS messages
        }
        wait1Msec(200);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
//
//                                        getUserControlProgram
//
// This function returns the name of the TETRIX User Control program. It reads the file
// "FTCConfig.txt" and builds the name of the file from the contents.
//
// Note that the filename includes the ".rxe" (robot executable file) file extension.
//
///////////////////////////////////////////////////////////////////////////////////////////
#endif
const string kConfigName = "FTCConfig.txt";

void getUserControlProgram(string &sFileName)
{
    byte nParmToReadByte[2];
    int nFileSize;
    TFileIOResult nIoResult;
    TFileHandle hFileHandle;

    sFileName = "";
    nParmToReadByte[1] = 0;
    hFileHandle = 0;
    OpenRead(hFileHandle, nIoResult, kConfigName, nFileSize);
    if (nIoResult == ioRsltSuccess)
    {
        for (int index = 0; index < nFileSize; ++index)
        {
            ReadByte(hFileHandle, nIoResult,  nParmToReadByte[0]);
            strcat(sFileName, nParmToReadByte);
        }

        //
        // Delete the ".rxe" file extension
        //
        int nFileExtPosition;

        nFileExtPosition = strlen(sFileName) - 4;
        if (nFileExtPosition > 0)
            StringDelete(sFileName, nFileExtPosition, 4);
    }
    Close(hFileHandle, nIoResult);
    return;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                    waitForStart
//
// Wait for the start of either the autonomous or tele-op phase. User program is running on the NXT
// but the phase has not yet started. The FMS (Field Management System) is continually (every 50 msec)
// sending information to the NXT. This program loops getting the latest value of joystick settings.
// When it finds that the FMS has started the  phase, it immediately returns.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

void waitForStart()
{
    while (true)
    {
        getJoystickSettings(joystick);
        if (!joystick.StopPgm)
            break;
    }
    return;
}

#endif
