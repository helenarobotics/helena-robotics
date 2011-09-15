//
// AutoTele.c
//
#include "lib\MainPrelude.h"
#include "config\RobotConfig.h"
#include "lib\MainPostlude.h"

#include "Autonomous.h"
#include "TeleOp.h"

task main() {
    if (InitializeMain(true, true)) {
        // Configure our telemetry
        TelemetryUse(true,false);       // Bluetooth, but no USB; change if necessary
        TelemetryEnable(false);         // *Note* disabled for now.

        // Put feedback on the screen during initialization
        fDisplaySonic = true;
        fDisplayEopdFront = true;

        // Wait until the FCS tells us to go
        waitForStart();

        // During game play avoid unecessary delays in reading sonic
        fDisplaySonic = false;
        fDisplayEopdFront = false;

        // Off to the races!
        if (PROGRAM_FLAVOR_TELEOP == programFlavor) {
            DoTeleOp();
        } else {
            DoAutonomous();
        }
    } else {
        // Just sit there and update the display
        InfiniteIdleLoop();
    }
}
