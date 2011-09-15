//
// MotorEMF.c
//
// Test for measuring the back EMF on a Tetrix motor. For use only in the corresponding test jig.

#define ROBOT_NAME ROBOT_NAME_TWO_MOTORS_WITH_ENCODERS
#include "lib\MainPrelude.h"
#include "config\RobotConfig.h"
#include "lib\MainPostlude.h"

task main()
    {
    if (InitializeMain(true, true))
        {
	    int power = 50;
	    MILLI msDuration = 3000;

	    SetMotorPower(motorLeft, power);
	    SetMotorPower(motorRight, 0);
	    SendMotorPowers();

	    MILLI msStart = nSysTime;

	    TelemetryUseBluetooth(true);

	    TelemetryAddString("seq");
	    TelemetryAddString("ms");
	    TelemetryAddString("encLeft");
	    TelemetryAddString("encRight");
	    TelemetrySend();

	    for (;;)
	        {
	        MILLI msNow = nSysTime;
	        MILLI dms   = msNow - msStart;

	        if (dms > msDuration)
	            break;

	        ENCOD encLeft, encRight;
	        ReadEncoders(encLeft, encRight);

	        TelemetryAddInt16(telemetry.serialNumber);
	        TelemetryAddInt32(dms);
	        TelemetryAddInt32(encLeft);
	        TelemetryAddInt32(encRight);
	        TelemetrySend();

	        telemetry.serialNumber++;
	        }

	    StopRobot();
	    }
    else
        {
	    InfiniteIdleLoop(); // Just sit there and update the display
	    }
    }
