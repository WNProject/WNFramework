#include "WNConcurrency/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNPlatform/inc/WNTime.h"
#endif

WN_INT32 WNMain(WN_INT32 _argc, WN_CHAR* _argv[]) {
    testing::GTEST_FLAG(print_time) = false;
    testing::InitGoogleTest(&_argc, _argv);

    srand(WNPlatform::WNGetTime());

    const WN_INT32 result = RUN_ALL_TESTS();

    return(result);
}
