#include "WNContainers/inc/WNSerializer.h"
#include "WNCore/inc/WNTypes.h"
#include "WNFileSystem/inc/WNReadFileBuffer.h"
#include "WNFileSystem/inc/WNWriteFileBuffer.h"

#ifdef _WN_ANDROID
#define FILE_PREFIX "/sdcard/"
#else
#define FILE_PREFIX
#endif

struct TestStruct {
  wn_uint32 blah;
  wn_uint64 blah2;
  wn_uint8 blah3;
  wn_int64 blah4;
  wn_float32 blah5;
  wn_float32 blah6;
  wn_float32 blah7;
  wn_float32 blah8;
};

wn_void testSerialize(
    wn::containers::data_buffer& _buffer, TestStruct& _tester) {
  _buffer.serialize(wn::containers::serializer<wn_uint32>(_tester.blah), 0);
  _buffer.serialize(wn::containers::serializer<wn_uint64>(_tester.blah2), 0);
  _buffer.serialize(wn::containers::serializer<wn_uint8>(_tester.blah3), 0);
  _buffer.serialize(wn::containers::serializer<wn_int64>(_tester.blah4), 0);
  _buffer.serialize(wn::containers::serializer<wn_float32>(_tester.blah5), 0);
  _buffer.serialize(wn::containers::serializer<wn_float32>(_tester.blah6), 0);
  _buffer.serialize(wn::containers::serializer<wn_float32>(_tester.blah7), 0);
  _buffer.serialize(wn::containers::serializer<wn_float32>(_tester.blah8), 0);
}

wn_void writeStruct(TestStruct& tester) {
  WNFileSystem::WNWriteTextFileBuffer fileWrite(WNFileSystem::eWNText);

  fileWrite.SetFile(FILE_PREFIX "testFile1.txt");

  testSerialize(fileWrite, tester);
}

wn_void readStruct(TestStruct& tester) {
  WNFileSystem::WNReadTextFileBuffer fileRead(WNFileSystem::eWNText);

  fileRead.SetFile(FILE_PREFIX "testFile1.txt");

  testSerialize(fileRead, tester);
}

wn_int32 wn_main(wn_int32 _argc, wn_char* _argv[]) {
  WN_UNUSED_ARGUMENT(_argc);
  WN_UNUSED_ARGUMENT(_argv);

  TestStruct tester = {25, 51123, 152, -1553223, 0.00024324f, -32.2253f,
      1.30234234234f, 12355235342.f};
  TestStruct tester2 = {0, 0, 0, 0, 0.f, 0.f, 0.f, 0.f};
  writeStruct(tester);
  readStruct(tester2);
  WNFileSystem::WNFile::DeleteFile(FILE_PREFIX "testFile1.txt");
  if (tester.blah != tester2.blah || tester.blah2 != tester2.blah2 ||
      tester.blah3 != tester2.blah3 ||
      tester.blah4 !=
          tester2
              .blah4) {  // Can't check the rest since they are float and change
    return (-1);
  }
  return (0);
}
