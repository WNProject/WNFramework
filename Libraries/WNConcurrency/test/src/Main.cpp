#include "WNConcurrency/test/inc/Common.h"
#include "WNEntryPoint/inc/WNEntry.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
#include "WNPlatform/inc/WNTime.h"
#endif

wn_int32 wn_main(wn_int32 _argc, wn_char* _argv[]) {
  wn_dummy();
  testing::InitGoogleTest(&_argc, _argv);

  srand(wn::WNGetTime());

  const wn_int32 result = RUN_ALL_TESTS();

  return (result);
}
