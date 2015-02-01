#include "WNStrings/test/inc/Common.h"

wn_int32 wn_main(wn_int32 _argc, wn_char* _argv[]) {
    testing::GTEST_FLAG(print_time) = false;
    testing::InitGoogleTest(&_argc, _argv);

    const wn_uint32 result = RUN_ALL_TESTS();

    return(result);
}
