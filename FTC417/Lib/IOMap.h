//===================================================================================
//
// IOMap.h
//
// Definitions relating to various 'IOMaps' available on the NXT. IOMaps
// are a means by which particular parts of the RAM memory of the NXT are
// exposed to RobotC programs for reading and writing (using the nxtReadIOMap
// and nxtWriteIOMap APIs). By this means, access to various particular
// low-level parts of the firmware state is possible.

//--------------------------------------------------------------------
// A place holder as big as a void* in the firmware
typedef long _INTPTR;

//--------------------------------------------------------------------
// Size constants taken from Lego Firmware 1.29
#define   _SIZE_OF_BT_NAME               16
#define   _SIZE_OF_BRICK_NAME            8
#define   _SIZE_OF_CLASS_OF_DEVICE       4
#define   _SIZE_OF_BT_PINCODE            16
#define   _SIZE_OF_BDADDR                7     // NOTE: real address is only six bytes. But extra character for NULL termination char
                                               //       But also note that address bytes can contain zeroes. So can't use standard
                                               //       'string' manipulation functions because they assum strings do not contain
                                               //       zeroes in the value bytes!

#define   _SIZE_OF_USBBUF                64
#define   _SIZE_OF_HSBUF                 128
#define   _SIZE_OF_BTBUF                 128
#define   _SIZE_OF_BT_DEVICE_TABLE       30
#define   _SIZE_OF_BT_CONNECT_TABLE      4      /* Index 0 is alway incomming connections */

//--------------------------------------------------------------------
// Struct definitions, with additional padding added
// as necessary so that IOMAPCOMM lays out correctly

typedef struct
    {
    byte        Name[_SIZE_OF_BT_NAME];
    byte        ClassOfDevice[_SIZE_OF_CLASS_OF_DEVICE];
    byte        BdAddr[_SIZE_OF_BDADDR];
    byte        DeviceStatus;
    byte        Spare1;
    byte        Spare2;
    byte        Spare3;

    byte        padding0;
    } _BDDEVICETABLE;

typedef struct
    {
    byte        Name[_SIZE_OF_BT_NAME];
    byte        ClassOfDevice[_SIZE_OF_CLASS_OF_DEVICE];
    byte        PinCode[_SIZE_OF_BT_PINCODE];
    byte        BdAddr[_SIZE_OF_BDADDR];
    byte        HandleNr;
    byte        StreamStatus;
    byte        LinkQuality;
    byte        Spare;

    byte        padding0;
    } _BDCONNECTTABLE;

typedef struct
    {
    byte        Name[_SIZE_OF_BT_NAME];     // 16
    byte        BluecoreVersion[2];         // 2
    byte        BdAddr[_SIZE_OF_BDADDR];    // 7
    byte        BtStateStatus;
    byte        BtHwStatus;
    byte        TimeOutValue;
    byte        Spare1;
    byte        Spare2;
    byte        Spare3;
    } _BRICKDATA;

#define _cbBRICKDATA (_SIZE_OF_BT_NAME + 2 + _SIZE_OF_BDADDR + 6)

typedef struct
    {
    byte        Buf[_SIZE_OF_BTBUF];
    byte        InPtr;
    byte        OutPtr;
    byte        Spare1;
    byte        Spare2;
    } _BTBUF;

#define _cbBTBUF (_SIZE_OF_BTBUF + 4)

typedef struct
    {
    byte        Buf[_SIZE_OF_HSBUF];
    byte        InPtr;
    byte        OutPtr;
    byte        Spare1;
    byte        Spare2;
    } _HSBUF;

#define _dibHSBUFInPtr       _SIZE_OF_HSBUF
#define _dibHSBUFOutPtr      (_SIZE_OF_HSBUF + 1)
#define _cbHSBUF             (_SIZE_OF_HSBUF + 4)

typedef struct
    {
    byte        Buf[_SIZE_OF_USBBUF];
    byte        InPtr;
    byte        OutPtr;     // in UsbOutBuf, this is #bytes in Buf that should be xmitd. c_comm.c(278) (at least in the *lego* firmware; robotc is seemingly different)
    byte        Spare1;
    byte        Spare2;
    } _USBBUF;

#define _dibUSBBUFInPtr      _SIZE_OF_USBBUF
#define _dibUSBBUFOutPtr     (_SIZE_OF_USBBUF + 1)
#define _cbUSBBUF            (_SIZE_OF_USBBUF + 4)

typedef struct
    {
    _INTPTR        pFunc;                                        // 0      // UWORD          (*pFunc)(byte , byte , byte , byte , byte *, UWORD*);
    _INTPTR        pFunc2;                                       // 4      // void           (*pFunc2)(byte *);

    // BT related entries
    _BDDEVICETABLE  BtDeviceTable[_SIZE_OF_BT_DEVICE_TABLE];    // 8
    _BDCONNECTTABLE BtConnectTable[_SIZE_OF_BT_CONNECT_TABLE];  // 968

    //General brick data
    _BRICKDATA      BrickData;                                  // 1160

    _BTBUF          BtInBuf;                                    // 1191
    _BTBUF          BtOutBuf;                                   // 1323

    // HI Speed related entries
    _HSBUF          HsInBuf;                                    // 1455
    _HSBUF          HsOutBuf;                                   // 1587

    // USB related entries
    _USBBUF         UsbInBuf;                                   // 1719
    _USBBUF         UsbOutBuf;                                  // 1787
    _USBBUF         UsbPollBuf;                                 // 1855

    byte           BtDeviceCnt;                                 // 1923
    byte           BtDeviceNameCnt;                             // 1924
    byte           HsFlags;                                     // 1925
    byte           HsSpeed;                                     // 1926
    byte           HsState;                                     // 1927
    byte           UsbState;                                    // 1928
    } _IOMAPCOMM; // see c_comm.iom in Lego firmware


#define _dibBtDeviceTable  8
#define _dibBtConnectTable 968
#define _dibBrickData      1160
#define _dibBtInBuf        1191
#define _dibBtOutBuf       1323
#define _dibHsInBuf        1455
#define _dibHsOutBuf       1587
#define _dibUsbInBuf       1719
#define _dibUsbOutBuf      1787
#define _dibUsbPollBuf     1855
#define _dibBtDeviceCnt    1923
#define _cbIOMAPCOMM       1929

const string _strCommMap = "Comm.mod";

//===================================================================================
