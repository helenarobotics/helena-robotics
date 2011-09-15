//
// CalibrateCompass.h
//
#include "lib\MainPrelude.h"
#include "config\RobotConfig.h"
#include "lib\MainPostlude.h"

task main()
    {
    if (InitializeMain(false,true))
        {
        BOOL fSuccess = true;
        I2CLINK link = I2CLINK_1;
        //
        StartCompassCalibration(link, OUT fSuccess);
        if (fSuccess)
            {
            // TO DO: rotate the bot a little more than 360 degrees in the
            // horizontal in NO LESS THAN 20 seconds.
            //
            // Not Yet Implemented

            StopCompassCalibration(link, OUT fSuccess);
            }
        //
        if (fSuccess)
            PlayHappy();
        else
            PlaySad();
        }
    }
