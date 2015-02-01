#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNMatrix2.h"
#endif

using namespace WNMath;

TEST(WNMatrix2ValidationUINT8, IdentityTest) {
    WNMatrix2<WN_UINT8> matrix = WNMatrix2<WN_UINT8>(1);

    matrix.Identity();

    for (WN_UINT32 i = 0; i < 2; ++i) {
        for (WN_UINT32 j = 0; j < 2; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 2 + j], 1);
            } else {
                ASSERT_EQ(matrix[i * 2 + j], 0);
            }
        }
    }
}

TEST(WNMatrix2ValidationUINT16, IdentityTest) {
    WNMatrix2<WN_UINT16> matrix = WNMatrix2<WN_UINT16>(1);

    matrix.Identity();

    for (WN_UINT32 i = 0; i < 2; ++i) {
        for (WN_UINT32 j = 0; j < 2; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 2 + j], 1);
            } else {
                ASSERT_EQ(matrix[i * 2 + j], 0);
            }
        }
    }
}

TEST(WNMatrix2ValidationUINT32, IdentityTest) {
    WNMatrix2<WN_UINT32> matrix = WNMatrix2<WN_UINT32>(1);

    matrix.Identity();

    for (WN_UINT32 i = 0; i < 2; ++i) {
        for (WN_UINT32 j = 0; j < 2; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 2 + j], 1);
            } else {
                ASSERT_EQ(matrix[i * 2 + j], 0);
            }
        }
    }
}

TEST(WNMatrix2ValidationUINT64, IdentityTest) {
    WNMatrix2<WN_UINT64> matrix = WNMatrix2<WN_UINT64>(1);

    matrix.Identity();

    for (WN_UINT32 i = 0; i < 2; ++i) {
        for (WN_UINT32 j = 0; j < 2; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 2 + j], 1);
            } else {
                ASSERT_EQ(matrix[i * 2 + j], 0);
            }
        }
    }
}

TEST(WNMatrix2ValidationINT8, IdentityTest) {
    WNMatrix2<WN_INT8> matrix = WNMatrix2<WN_INT8>(1);

    matrix.Identity();

    for (WN_UINT32 i = 0; i < 2; ++i) {
        for (WN_UINT32 j = 0; j < 2; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 2 + j], 1);
            } else {
                ASSERT_EQ(matrix[i * 2 + j], 0);
            }
        }
    }
}

TEST(WNMatrix2ValidationINT16, IdentityTest) {
    WNMatrix2<WN_INT16> matrix = WNMatrix2<WN_INT16>(1);

    matrix.Identity();

    for (WN_UINT32 i = 0; i < 2; ++i) {
        for (WN_UINT32 j = 0; j < 2; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 2 + j], 1);
            } else {
                ASSERT_EQ(matrix[i * 2 + j], 0);
            }
        }
    }
}

TEST(WNMatrix2ValidationINT32, IdentityTest) {
    WNMatrix2<WN_INT32> matrix = WNMatrix2<WN_INT32>(1);

    matrix.Identity();

    for (WN_UINT32 i = 0; i < 2; ++i) {
        for (WN_UINT32 j = 0; j < 2; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 2 + j], 1);
            } else {
                ASSERT_EQ(matrix[i * 2 + j], 0);
            }
        }
    }
}

TEST(WNMatrix2ValidationINT64, IdentityTest) {
    WNMatrix2<WN_INT64> matrix = WNMatrix2<WN_INT64>(1);

    matrix.Identity();

    for (WN_UINT32 i = 0; i < 2; ++i) {
        for (WN_UINT32 j = 0; j < 2; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 2 + j], 1);
            } else {
                ASSERT_EQ(matrix[i * 2 + j], 0);
            }
        }
    }
}

TEST(WNMatrix2ValidationFLOAT16, IdentityTest) {
    WNMatrix2<WN_FLOAT16> matrix = WNMatrix2<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f));

    matrix.Identity();

    for (WN_UINT32 i = 0; i < 2; ++i) {
        for (WN_UINT32 j = 0; j < 2; ++j) {
            if (i == j) {
                ASSERT_EQ(static_cast<WN_FLOAT32>(matrix[i * 2 + j]), 1.0f);
            } else {
                ASSERT_EQ(static_cast<WN_FLOAT32>(matrix[i * 2 + j]), 0.0f);
            }
        }
    }
}

TEST(WNMatrix2ValidationFLOAT32, IdentityTest) {
    WNMatrix2<WN_FLOAT32> matrix = WNMatrix2<WN_FLOAT32>(1.0f);

    matrix.Identity();

    for (WN_UINT32 i = 0; i < 2; ++i) {
        for (WN_UINT32 j = 0; j < 2; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 2 + j], 1.0f);
            } else {
                ASSERT_EQ(matrix[i * 2 + j], 0.0f);
            }
        }
    }
}

TEST(WNMatrix2ValidationFLOAT64, IdentityTest) {
    WNMatrix2<WN_FLOAT64> matrix = WNMatrix2<WN_FLOAT64>(1.0);

    matrix.Identity();

    for (WN_UINT32 i = 0; i < 2; ++i) {
        for (WN_UINT32 j = 0; j < 2; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 2 + j], 1.0);
            } else {
                ASSERT_EQ(matrix[i * 2 + j], 0.0);
            }
        }
    }
}
