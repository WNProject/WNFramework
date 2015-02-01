#include "WNPlatform/test/inc/Common.h"

WN_INT32 WNMain(WN_INT32 _argc, WN_CHAR* _argv[]) {
    testing::GTEST_FLAG(print_time) = false;
    testing::InitGoogleTest(&_argc, _argv);

    const WN_UINT32 result = RUN_ALL_TESTS();

    return(result);
}
