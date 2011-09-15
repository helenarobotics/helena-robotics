//
// FileTest.c
//

#define ComputeFileName(s, i)  { StringFormat(s, "file%d.txt", i); }


TFileHandle Open(int i)
    {
    TFileHandle hFile = 0;
    TFileIOResult nIoResult;
    string sName; ComputeFileName(sName, i);
    int cbFile = 0;
    OpenWrite(hFile, nIoResult, sname, cbFile);
    return hFile;
    }

task main()
    {
    TFileIOResult nIoResult;

    for (int i =0; i < 10; i++)
        {
        TFileHandle hFile = Open(i);
        Close(hFile, nIoResult);
        }
    }
