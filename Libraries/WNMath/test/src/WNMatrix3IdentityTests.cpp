#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNMatrix3.h"
#endif

using namespace WNMath;

TEST(WNMatrix3ValidationUINT8, IdentityTest) {
    WNMatrix3<WN_UINT8> matrix = WNMatrix3<WN_UINT8>(1);

    matrix.Identity();

    for (WN_UINT32 i = 0; i < 3; ++i) {
        for (WN_UINT32 j = 0; j < 3; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 3 + j], 1);
            } else {
                ASSERT_EQ(matrix[i * 3 + j], 0);
            }
        }
    }
}

TEST(WNMatrix3ValidationUINT16, IdentityTest) {
    WNMatrix3<WN_UINT16> matrix = WNMatrix3<WN_UINT16>(1);

    matrix.Identity();

    for (WN_UINT32 i = 0; i < 3; ++i) {
        for (WN_UINT32 j = 0; j < 3; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 3 + j], 1);
            } else {
                ASSERT_EQ(matrix[i * 3 + j], 0);
            }
        }
    }
}

TEST(WNMatrix3ValidationUINT32, IdentityTest) {
    WNMatrix3<WN_UINT32> matrix = WNMatrix3<WN_UINT32>(1);

    matrix.Identity();

    for (WN_UINT32 i = 0; i < 3; ++i) {
        for (WN_UINT32 j = 0; j < 3; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 3 + j], 1);
            } else {
                ASSERT_EQ(matrix[i * 3 + j], 0);
            }
        }
    }
}

TEST(WNMatrix3ValidationUINT64, IdentityTest) {
    WNMatrix3<WN_UINT64> matrix = WNMatrix3<WN_UINT64>(1);

    matrix.Identity();

    for (WN_UINT32 i = 0; i < 3; ++i) {
        for (WN_UINT32 j = 0; j < 3; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 3 + j], 1);
            } else {
                ASSERT_EQ(matrix[i * 3 + j], 0);
            }
        }
    }
}

TEST(WNMatrix3ValidationINT8, IdentityTest) {
    WNMatrix3<WN_INT8> matrix = WNMatrix3<WN_INT8>(1);

    matrix.Identity();

    for (WN_UINT32 i = 0; i < 3; ++i) {
        for (WN_UINT32 j = 0; j < 3; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 3 + j], 1);
            } else {
                ASSERT_EQ(matrix[i * 3 + j], 0);
            }
        }
    }
}

TEST(WNMatrix3ValidationINT16, IdentityTest) {
    WNMatrix3<WN_INT16> matrix = WNMatrix3<WN_INT16>(1);

    matrix.Identity();

    for (WN_UINT32 i = 0; i < 3; ++i) {
        for (WN_UINT32 j = 0; j < 3; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 3 + j], 1);
            } else {
                ASSERT_EQ(matrix[i * 3 + j], 0);
            }
        }
    }
}

TEST(WNMatrix3ValidationINT32, IdentityTest) {
    WNMatrix3<WN_INT32> matrix = WNMatrix3<WN_INT32>(1);

    matrix.Identity();

    for (WN_UINT32 i = 0; i < 3; ++i) {
        for (WN_UINT32 j = 0; j < 3; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 3 + j], 1);
            } else {
                ASSERT_EQ(matrix[i * 3 + j], 0);
            }
        }
    }
}

TEST(WNMatrix3ValidationINT64, IdentityTest) {
    WNMatrix3<WN_INT64> matrix = WNMatrix3<WN_INT64>(1);

    matrix.Identity();

    for (WN_UINT32 i = 0; i < 3; ++i) {
        for (WN_UINT32 j = 0; j < 3; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 3 + j], 1);
            } else {
                ASSERT_EQ(matrix[i * 3 + j], 0);
            }
        }
    }
}

TEST(WNMatrix3ValidationFLOAT16, IdentityTest) {
    WNMatrix3<WN_FLOAT16> matrix = WNMatrix3<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f));

    matrix.Identity();

    for (WN_UINT32 i = 0; i < 3; ++i) {
        for (WN_UINT32 j = 0; j < 3; ++j) {
            if (i == j) {
                ASSERT_EQ(static_cast<WN_FLOAT32>(matrix[i * 3 + j]), 1.0f);
            } else {
                ASSERT_EQ(static_cast<WN_FLOAT32>(matrix[i * 3 + j]), 0.0f);
            }
        }
    }
}

TEST(WNMatrix3ValidationFLOAT32, IdentityTest) {
    WNMatrix3<WN_FLOAT32> matrix = WNMatrix3<WN_FLOAT32>(1.0f);

    matrix.Identity();

    for (WN_UINT32 i = 0; i < 3; ++i) {
        for (WN_UINT32 j = 0; j < 3; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 3 + j], 1.0f);
            } else {
                ASSERT_EQ(matrix[i * 3 + j], 0.0f);
            }
        }
    }
}

TEST(WNMatrix3ValidationFLOAT64, IdentityTest) {
    WNMatrix3<WN_FLOAT64> matrix = WNMatrix3<WN_FLOAT64>(1.0);

    matrix.Identity();

    for (WN_UINT32 i = 0; i < 3; ++i) {
        for (WN_UINT32 j = 0; j < 3; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 3 + j], 1.0);
            } else {
                ASSERT_EQ(matrix[i * 3 + j], 0.0);
            }
        }
    }
}
