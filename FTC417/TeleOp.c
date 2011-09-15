//
// TeleOp.c
//
#define USE_FTCFIELD    0

#include "lib\MainPrelude.h"
#include "config\RobotConfig.h"
#include "lib\MainPostlude.h"

#include "TeleOp.h"

task main() {
    if (InitializeMain(true, true)) {
        // Put feedback on the screen during initialization
        fDisplaySonic = true;
        fDisplayEopdFront = true;

        // Wait until the FCS tells us to start autonomous
        waitForStart();

        // During game play avoid unecessary delays in reading sonic
        fDisplaySonic = false;
        fDisplayEopdFront = false;

        DoTeleOp();
    } else {
        InfiniteIdleLoop();
    }
}
