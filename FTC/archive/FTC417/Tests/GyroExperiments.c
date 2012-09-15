//
// GyroExperiments.c
//
// Designed to work with a gyro on a sensor multiplexor. Initializes said gyro,
// then starts streaming out telemetry containing the gyro state. A simple
// little thing.

#include "..\lib\MainPrelude.h"

#define sensnmGyroHorz SensorOnMux(2,1)

GYROSENSOR sensGyroHorz;

void
InitializeMotors(IN OUT STICKYFAILURE &fOverallSuccess) {
}
void
InitializeServos(IN OUT STICKYFAILURE &fOverallSuccess) {
}
void
InitializeSensors(IN OUT STICKYFAILURE &fOverallSuccess) {
    InitializeGyroSensor(fOverallSuccess, "gyro", sensGyroHorz, sensnmGyroHorz,
        2000, 0);
}

#include "..\lib\MainPostlude.h"


void
DoGyroDisplay() {
    eraseDisplay();

    StartReadingGyroSensor(sensGyroHorz);
    for (;;) {
        LockBlackboard();
        float deg = sensGyroHorz.deg;
        ReleaseBlackboard();
        //
        nxtDisplayTextLine(3, "gyro=%.2f", deg);
        //
        EndTimeSlice();
    }
}

void
DoGyroTelemetry() {
    SuspendBlackboard();
    SuspendDisplayTask();

    StartReadingGyroSensor(sensGyroHorz);

    // Bluetooth, but not USB
    TelemetryUse(true, false);

    // Put out our bias settings
    TelemetryAddString("bias");
    TelemetryAddFloat(sensGyroHorz.bias);
    TelemetrySend();

    // Emit column headers
    TelemetryAddString("serial#");
    TelemetryAddString("ms");
    TelemetryAddString("degCWPerS");
    TelemetryAddString("deg");
    TelemetrySend();

    // Emit the data
    MILLI msStart = nSysTime;
    for (;;) {
        LockBlackboard();
        MILLI msNow = nSysTime;
        ReadGyroSensor(sensGyroHorz, msNow);
        float degCWPerS = sensGyroHorz.degCWPerS;
        float deg = sensGyroHorz.deg;
        ReleaseBlackboard();

        TelemetryAddInt32(telemetry.serialNumber);
        TelemetryAddInt32(msNow - msStart);
        TelemetryAddFloat(degCWPerS);
        TelemetryAddFloat(deg);
        TelemetrySend();

        telemetry.serialNumber++;

        EndTimeSlice();
    }
}

task
main() {
    if (!InitializeMain(true, true))
        return;
    DoGyroTelemetry();
}
