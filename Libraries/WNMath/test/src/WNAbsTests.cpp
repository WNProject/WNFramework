#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNMath/inc/WNBasic.h"

    #include <math.h>
    #include <float.h>
#endif

using namespace WNMath;

TEST(WNAbsUINT8, ZeroTest) {
    const WN_UINT8 result = WNAbs(static_cast<WN_UINT8>(0));

    ASSERT_EQ(result, 0);
}

TEST(WNAbsUINT16, ZeroTest) {
    const WN_UINT16 result = WNAbs(static_cast<WN_UINT16>(0));

    ASSERT_EQ(result, 0);
}

TEST(WNAbsUINT32, ZeroTest) {
    const WN_UINT32 result = WNAbs(static_cast<WN_UINT32>(0));

    ASSERT_EQ(result, 0);
}

TEST(WNAbsUINT64, ZeroTest) {
    const WN_UINT64 result = WNAbs(static_cast<WN_UINT64>(0));

    ASSERT_EQ(result, 0);
}

TEST(WNAbsINT8, ZeroTest) {
    const WN_INT8 result = WNAbs(static_cast<WN_INT8>(0));

    ASSERT_EQ(result, 0);
}

TEST(WNAbsINT8, NegativeTest) {
    const WN_INT8 result = WNAbs(static_cast<WN_INT8>(-1));

    ASSERT_EQ(result, 1);
}

TEST(WNAbsINT16, ZeroTest) {
    const WN_INT16 result = WNAbs(static_cast<WN_INT16>(0));

    ASSERT_EQ(result, 0);
}

TEST(WNAbsINT16, NegativeTest) {
    const WN_INT16 result = WNAbs(static_cast<WN_INT16>(-1));

    ASSERT_EQ(result, 1);
}

TEST(WNAbsINT32, ZeroTest) {
    const WN_INT32 result = WNAbs(static_cast<WN_INT32>(0));

    ASSERT_EQ(result, 0);
}

TEST(WNAbsINT32, NegativeTest) {
    const WN_INT32 result = WNAbs(static_cast<WN_INT32>(-1));

    ASSERT_EQ(result, 1);
}

TEST(WNAbsINT64, ZeroTest) {
    const WN_INT64 result = WNAbs(static_cast<WN_INT64>(0));

    ASSERT_EQ(result, 0);
}

TEST(WNAbsINT64, NegativeTest) {
    const WN_INT64 result = WNAbs(static_cast<WN_INT64>(-1));

    ASSERT_EQ(result, 1);
}

TEST(WNAbsFLOAT32, ZeroTest) {
    const WN_FLOAT32 result = WNAbs(0.0f);

    ASSERT_EQ(result, 0.0f);
}

TEST(WNAbsFLOAT32, NegativeTest) {
    const WN_FLOAT32 result = WNAbs(-1.0f);

    ASSERT_EQ(result, 1.0f);
}

TEST(WNAbsFLOAT64, ZeroTest) {
    const WN_FLOAT64 result = WNAbs(0.0);

    ASSERT_EQ(result, 0.0);
}

TEST(WNAbsFLOAT64, NegativeTest) {
    const WN_FLOAT64 result = WNAbs(-1.0);

    ASSERT_EQ(result, 1.0);
}
