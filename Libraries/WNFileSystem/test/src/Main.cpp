#include "WNCore/inc/WNTypes.h"
#include "WNFileSystem/inc/WNWriteFileBuffer.h"
#include "WNFileSystem/inc/WNReadFileBuffer.h"
#include "WNContainers/inc/WNSerializer.h"

#ifdef _WN_ANDROID
    #define FILE_PREFIX "/sdcard/"
#else
    #define FILE_PREFIX 
#endif

struct TestStruct {
    WN_UINT32 blah;
    WN_UINT64 blah2;
    WN_UINT8 blah3;
    WN_INT64 blah4;
    WN_FLOAT32 blah5;
    WN_FLOAT32 blah6;
    WN_FLOAT32 blah7;
    WN_FLOAT32 blah8;
};

WN_VOID testSerialize(WNContainers::WNDataBuffer& _buffer, TestStruct& _tester){
    _buffer.Serialize(0, WNContainers::WNSerializer<WN_UINT32>(_tester.blah));
    _buffer.Serialize(0, WNContainers::WNSerializer<WN_UINT64>(_tester.blah2));
    _buffer.Serialize(0, WNContainers::WNSerializer<WN_UINT8>(_tester.blah3));
    _buffer.Serialize(0, WNContainers::WNSerializer<WN_INT64>(_tester.blah4));
    _buffer.Serialize(0, WNContainers::WNSerializer<WN_FLOAT32>(_tester.blah5));
    _buffer.Serialize(0, WNContainers::WNSerializer<WN_FLOAT32>(_tester.blah6));
    _buffer.Serialize(0, WNContainers::WNSerializer<WN_FLOAT32>(_tester.blah7));
    _buffer.Serialize(0, WNContainers::WNSerializer<WN_FLOAT32>(_tester.blah8));
}

WN_VOID writeStruct(TestStruct& tester) {
    WNFileSystem::WNWriteTextFileBuffer fileWrite(WNFileSystem::eWNText);

    fileWrite.SetFile(FILE_PREFIX "testFile1.txt");

    testSerialize(fileWrite, tester);
}

WN_VOID readStruct(TestStruct& tester){
    WNFileSystem::WNReadTextFileBuffer fileRead(WNFileSystem::eWNText);

    fileRead.SetFile(FILE_PREFIX "testFile1.txt");

    testSerialize(fileRead, tester);
}

WN_INT32 WNMain(WN_INT32 _argc, WN_CHAR* _argv[]) {
    WN_UNUSED_ARG(_argc);
    WN_UNUSED_ARG(_argv);

    TestStruct tester = { 25, 51123, 152, -1553223,  0.00024324f, -32.2253f, 1.30234234234f, 12355235342.f};
    TestStruct tester2 = {0, 0, 0, 0, 0.f, 0.f, 0.f, 0.f };
    writeStruct(tester);
    readStruct(tester2);
    WNFileSystem::WNFile::DeleteFile(FILE_PREFIX "testFile1.txt");
    if(
        tester.blah != tester2.blah ||
        tester.blah2 != tester2.blah2 ||
        tester.blah3 != tester2.blah3 ||
        tester.blah4 != tester2.blah4 ) { //Can't check the rest since they are float and change
        return(-1);
    }
    return(0);
}
