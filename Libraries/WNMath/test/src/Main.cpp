#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNPlatform/inc/WNTime.h"
#endif

wn_int32 wn_main(wn_int32 _argc, wn_char* _argv[]) {
    testing::InitGoogleTest(&_argc, _argv);

    srand(wn::WNGetTime());

    ::printf("%.1f%% error is allowed for all non-integer types\n\n", ALLOWED_PERCENTAGE_ERROR * 100.0f);

    return(RUN_ALL_TESTS());
}
