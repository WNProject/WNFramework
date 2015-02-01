#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNMath/inc/WNBasic.h"

    #include <math.h>
    #include <float.h>
#endif

using namespace WNMath;

TEST(WNCeilFLOAT32, ZeroTest) {
    const WN_FLOAT32 result = WNCeil(0.0f);

    ASSERT_EQ(result, 0.0f);
}

TEST(WNCeilFLOAT64, ZeroTest) {
    const WN_FLOAT64 result = WNCeil(0.0);

    ASSERT_EQ(result, 0.0);
}