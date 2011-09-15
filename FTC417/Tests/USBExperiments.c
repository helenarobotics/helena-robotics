//
// USBExperiments.c
//
#include "..\MainPrelude.h"
void InitializeMotors(IN OUT STICKYFAILURE& fOverallSuccess)    {}
void InitializeServos(IN OUT STICKYFAILURE& fOverallSuccess)    {}
void InitializeSensors(IN OUT STICKYFAILURE& fOverallSuccess)   {}
#include "..\MainPostlude.h"

//--------------------------------------------------------------------
// A slower trace so we don't lose entries when we're using Bluetooth:
// RobotC would seem to transmit the debug stream to the PC only periodically.
#define TRACEWAIT(x)        { TRACE(x); wait1Msec(20); }


//--------------------------------------------------------------------
// Some tracing helpers

#define DumpSize(s, cb)                 \
    {                                   \
    long l = (long)cb;                  \
    TRACEWAIT(("sizeof(%s)=%d", s, l));       \
    }

#define DumpFieldOffset(s, ib)          \
    {                                   \
    long l = (long)(ib);                \
    TRACEWAIT(("ib%s=%d", s, l));       \
    }

//--------------------------------------------------------------------
// Code that gets the compiler to do field offset calculations. Note that
// the code generated in Foo() is buggy, in that it does extraneous
// dereferences and copying, but at least one can look at the decompiled
// code and see what field offsets it's using; these are correct.

void Bar(BDDEVICETABLE& data) {}
void Bar(BDCONNECTTABLE& data) {}
void Bar(BRICKDATA& data) { }
void Bar(BTBUF& buf) { }
void Bar(HSBUF& buf) { }
void Bar(USBBUF& buf) { }
void Bar(byte& b) { }

void Foo(IOMAPCOMM& map)
    {
    Bar(map.BtDeviceTable[0]);
    Bar(map.BtConnectTable[0]);
    Bar(map.BrickData);
    Bar(map.BtInBuf);
    Bar(map.BtOutBuf);
    Bar(map.HsInBuf);
    Bar(map.HsOutBuf);
    Bar(map.UsbInBuf);
    Bar(map.UsbOutBuf);
    Bar(map.UsbPollBuf);
    Bar(map.BtDeviceCnt);
    Bar(map.BtDeviceNameCnt);
    Bar(map.HsFlags);
    Bar(map.HsSpeed);
    Bar(map.HsState);
    Bar(map.UsbState);
    }

//--------------------------------------------------------------------
// Manually calculate and print various struct sizes and offsets

#if 0
int ibBtDeviceTable  = 0;
int ibBtConnectTable = 0;
int ibBrickData      = 0;
int ibBtInBuf        = 0;
int ibBtOutBuf       = 0;
int ibHsInBuf        = 0;
int ibHsOutBuf       = 0;
int ibUsbInBuf       = 0;
int ibUsbOutBuf      = 0;
int ibUsbPollBuf     = 0;
int ibBtDeviceCnt    = 0;
int cbIOMAPCOMM      = 0;

void ComputeFieldOffsets()
    {
    ibBtDeviceTable  = 8;
    ibBtConnectTable = (ibBtDeviceTable + (sizeof(BDDEVICETABLE)) * SIZE_OF_BT_DEVICE_TABLE);
    ibBrickData      = (ibBtConnectTable + (sizeof(BDCONNECTTABLE)) * SIZE_OF_BT_CONNECT_TABLE);
    ibBtInBuf        = (ibBrickData + cbBRICKDATA);
    ibBtOutBuf       = (ibBtInBuf + cbBTBUF);
    ibHsInBuf        = (ibBtOutBuf + cbBTBUF);
    ibHsOutBuf       = (ibHsInBuf + cbHSBUF);
    ibUsbInBuf       = (ibHsOutBuf + cbHSBUF);
    ibUsbOutBuf      = (ibUsbInBuf + cbUSBBUF);
    ibUsbPollBuf     = (ibUsbOutBuf + cbUSBBUF);
    ibBtDeviceCnt    = (ibUsbPollBuf + cbUSBBUF);
    cbIOMAPCOMM      = (ibBtDeviceCnt + 6);
    }

void DumpSizes()
    {
    DumpSize("BDDEVICETABLE", sizeof(BDDEVICETABLE));
    DumpSize("BDCONNECTTABLE", sizeof(BDCONNECTTABLE));
    DumpSize("BRICKDATA", sizeof(BRICKDATA));
    DumpSize("BTBUF", sizeof(BTBUF));
    DumpSize("HSBUF", sizeof(HSBUF));
    DumpSize("USBBUF", sizeof(USBBUF));
    DumpSize("IOMAPCOMM", sizeof(IOMAPCOMM));

    ComputeFieldOffsets();

    TRACE((""));
    DumpFieldOffset("BtDeviceTable", ibBtDeviceTable);
    DumpFieldOffset("BtConnectTable", ibBtConnectTable);
    DumpFieldOffset("BrickData", ibBrickData);
    DumpFieldOffset("BtInBuf", ibBtInBuf);
    DumpFieldOffset("BtOutBuf", ibBtOutBuf);
    DumpFieldOffset("HsInBuf", ibHsInBuf);
    DumpFieldOffset("HsOutBuf", ibHsOutBuf);
    DumpFieldOffset("UsbInBuf", ibUsbInBuf);
    DumpFieldOffset("UsbOutBuf", ibUsbOutBuf);
    DumpFieldOffset("UsbPollBuf", ibUsbPollBuf);
    DumpFieldOffset("BtDeviceCnt", ibBtDeviceCnt);
    DumpFieldOffset("Max", cbIOMAPCOMM);

    TRACE((""));
    }
#endif

//--------------------------------------------------------------------
// Dump the contents of the communications IOMAP in the firmware

#define Cleanse(b,ibMax,ib)  (((ib) < (ibMax) && 32 <= b && b <= 127) ? b : ' ')

void DumpCommMod(int ibFirst=0, int cbMax=cbIOMAPCOMM)
    {
    short ioResult = 0;
    byte rgb[8];       // a max 64 bytes (apparently) allowed in nxtReadIOMap

    int ibMax = Min(cbIOMAPCOMM, ibFirst + cbMax);
    for (int ib = ibFirst; ib < ibMax; ib += sizeof(rgb))
        {
        int cb = Min(sizeof(rgb), ibMax - ib);
        memset(rgb[0], '.', sizeof(rgb));
        nxtReadIOMap(strCommMap, ioResult, rgb[0], ib, cb);

        for (int i = 0; i < cb; i += 8)
            {
            int b0 = rgb[i] & 0xFF;
            int b1 = rgb[i+1] & 0xFF;
            int b2 = rgb[i+2] & 0xFF;
            int b3 = rgb[i+3] & 0xFF;
            int b4 = rgb[i+4] & 0xFF;
            int b5 = rgb[i+5] & 0xFF;
            int b6 = rgb[i+6] & 0xFF;
            int b7 = rgb[i+7] & 0xFF;

            string s0, s1, s2, s3, s4, s5, s6, s7;
            StringFormat(s0, "%02x", b0);
            StringFormat(s1, "%02x", b1);
            StringFormat(s2, "%02x", b2);
            StringFormat(s3, "%02x", b3);
            StringFormat(s4, "%02x", b4);
            StringFormat(s5, "%02x", b5);
            StringFormat(s6, "%02x", b6);
            StringFormat(s7, "%02x", b7);

            if (ib+i+1 >= ibMax)   s1 = "  ";
            if (ib+i+2 >= ibMax)   s2 = "  ";
            if (ib+i+3 >= ibMax)   s3 = "  ";
            if (ib+i+4 >= ibMax)   s4 = "  ";
            if (ib+i+5 >= ibMax)   s5 = "  ";
            if (ib+i+6 >= ibMax)   s6 = "  ";
            if (ib+i+7 >= ibMax)   s7 = "  ";

            string sBytesLow, sBytesHigh, sCharsLow, sCharsHigh;
            StringFormat(sBytesLow,  "%s%s%s%s", s0, s1, s2, s3);
            StringFormat(sBytesHigh, "%s%s%s%s", s4, s5, s6, s7);
            StringFormat(sCharsLow,  "%c%c%c%c", Cleanse(b0,ibMax,ib+i+0),Cleanse(b1,ibMax,ib+i+1),Cleanse(b2,ibMax,ib+i+2),Cleanse(b3,ibMax,ib+i+3));
            StringFormat(sCharsHigh, "%c%c%c%c", Cleanse(b4,ibMax,ib+i+4),Cleanse(b5,ibMax,ib+i+5),Cleanse(b6,ibMax,ib+i+6),Cleanse(b7,ibMax,ib+i+7));

            TRACEWAIT(("%d %d: %s|%s %s%s", cb, ib+i, sBytesLow, sBytesHigh, sCharsLow, sCharsHigh));
            }
        }
    }

//--------------------------------------------------------------------
// Try to get something in an outbound message queue

void PostMessage(string str)
    {
    _TelemetryAddString_(telemetry,str);
    short ioResult = 0;
    if (true)
        {
        ioResult = cCmdMessageAddToQueue(telemetry.mailbox, telemetry.rgbMsg[0], telemetry.cbMsg);
        }
    else
        {
        while (bBTBusy)
            {
            EndTimeSlice();
            }
        ioResult = cCmdMessageWriteToBluetooth(telemetry.rgbMsg[0], telemetry.cbMsg, telemetry.mailbox);
        }
    TRACE(("ioResult=0x%04x mb=%d cb=%d", ioResult, telemetry.mailbox, telemetry.cbMsg));
    _TelemetryReset_(telemetry);

    for (int i = 0; false && i < 20; i++)
        {
            int cb = cCmdMessageGetSize(telemetry.mailbox);

            byte rgb[30];
            ioResult = cCmdMessageRead(rgb, 30, telemetry.mailbox);
            TRACE(("i=%d cb=%d io=%d", i, cb, ioResult));
            }
    }

//--------------------------------------------------------------------

#ifndef _CircularWriteChunk_
#define _CircularWriteChunk_(rgbData, dibSrc, dibDest, cbToWrite, ioResult)                                     \
    {                                                                                                           \
    const int cbQuantum = 64;                                                                                   \
    for (int cbWritten = 0; cbWritten < (cbToWrite); cbWritten += cbQuantum)                                    \
        {                                                                                                       \
        int cbQuantumWrite = Min(cbQuantum, cbToWrite - cbWritten);                                             \
        nxtWriteIOMap(strCommMap, ioResult, rgbData[(dibSrc)+cbWritten], (dibDest)+cbWritten, cbQuantumWrite);  \
        }                                                                                                       \
    }
#endif

// Append the current contents of the telemetry buffer to IoMapComm.HsInBuf.
// Return the number of bytes we actually wrote.
int CircularWriteTelemetryToHsInBuf()
    {
    short ioResult = 0;

    // Find out the current location of the circular buffer pointers
    ubyte rgbPtr[2];
    nxtReadIOMap(strCommMap, ioResult, rgbPtr[0], dibHsInBuf + dibHSBUFInPtr, 2);
    int ibInPtr  = rgbPtr[0];
    int ibOutPtr = rgbPtr[1];   // this may become stale while we run, but that will only increase available space, so it's safe

    // Figure out how much room there is in the buffer
    const int cbBuffer      = SIZE_OF_HSBUF;
    const int cbUsed        = (ibInPtr - ibOutPtr) & (SIZE_OF_HSBUF-1);
    const int cbFree        = cbBuffer - cbUsed - 1;            // need the -1 so that ptrs don't coincide when full
    const int cbToWrite     = Min(cbFree, telemetry.cbMsg);

    TRACE(("used=%d free=%d write=%d", cbUsed, cbFree, cbToWrite));

    // Write all or nothing
    if (cbToWrite == telemetry.cbMsg)
        {
        // We might have to write the data in two chunks. How much will we write in each?
            const int cbChunkFirst  = Min(cbToWrite, cbBuffer - ibInPtr);
            const int cbChunkSecond = cbToWrite - cbChunkFirst;

            // Write each chunk. Note that we can only write in quanta of 64 bytes at a time, as that is all
            // that the nxtWriteIOMap API will allow.
            _CircularWriteChunk_(telemetry.rgbMsg, 0,            dibHsInBuf+ibInPtr, cbChunkFirst,  ioResult);
            _CircularWriteChunk_(telemetry.rgbMsg, cbChunkFirst, dibHsInBuf,         cbChunkSecond, ioResult);

            // Update the in ptr. This will, atomically, make the data available for transmission in the 'Poll' command
            rgbPtr[0] = (ibInPtr + cbToWrite) & (SIZE_OF_HSBUF-1);
            nxtWriteIOMap(strCommMap, ioResult, rgbPtr[0], dibHsInBuf + dibHSBUFInPtr, 1);

        return cbToWrite;
            }
        else
            return 0;
    }

// Write the telemetry to the USB Output Buffer
int WriteTelemetryToUsbOutBuf()
    {
    short ioResult = 0;

    // Because of the interaction of the USB output buffer with
    // USB command replies, we need to execute the write atomically,
    // with no intervening external command being serviced. But unfortunately
    // our maximum write limit is 64 bytes. Unfortunately, that means
    // that in order to include the OutPtr in the atomic write, we have
    // to leave the first two bytes in the buffer as junk.
    const int cbOmit    = 2;
    const int cbFree    = SIZE_OF_USBBUF-cbOmit;
    const int cbToWrite = Min(cbFree, telemetry.cbMsg);

    TRACE(("free=%d write=%d", (long)cbFree, (long)cbToWrite));

    // Write all or nothing
    if (cbToWrite == telemetry.cbMsg)
        {
        ubyte rgb[64]; memset(rgb, 0, sizeof(rgb));
        //
        memcpy(rgb[0], telemetry.rgbMsg[0], cbToWrite);
        rgb[62] = (ubyte)(cbOmit + cbToWrite);
        rgb[63] = 0;
        //
        nxtWriteIOMap(strCommMap, ioResult, rgb[0], dibUsbOutBuf+cbOmit, 64);
        TRACE(("ioResult=%d", (long)ioResult));
        //
        return cbToWrite;
            }
        else
            return 0;
    }

//--------------------------------------------------------------------
// Root of the world

task main()
    {
    TRACE(("-------"));
    // ComputeFieldOffsets();
    TelemetryInitialize();

    if (true)
        {
            DumpCommMod(dibHsInBuf, cbHSBUF);

            TelemetryAddString("how now brown cow");
        _TelemetryFinalizeRecord_(telemetry, 0);
            CircularWriteTelemetryToHsInBuf();

            TRACE(("-------"));
            DumpCommMod(dibHsInBuf, cbHSBUF);
            }
    else
        {
        DumpCommMod(dibUsbOutBuf, cbUSBBUF);

            TelemetryAddString("how now brown cow");
        _TelemetryFinalizeRecord_(telemetry, 0);
        WriteTelemetryToUsbOutBuf();

        wait1Msec(500);
        TRACE(("-------"));
            DumpCommMod(dibUsbOutBuf, cbUSBBUF);
        }


        wait1Msec(100);

    for (;false;)
        {
        // PostMessage("zipzipzip");
        wait1Msec(1000);
        }
    }
