#include "WNMath/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNCore/inc/WNExtendedTypes.h"
    #include "WNMath/inc/WNMatrix4.h"
#endif

using namespace WNMath;

TEST(WNMatrix4ValidationUINT8, CreateIdentityTest) {
    WNMatrix4<WN_UINT8> matrix = WNMatrix4<WN_UINT8>(1);

    matrix = WNMatrix4<WN_UINT8>::CreateIdentity();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        for (WN_UINT32 j = 0; j < 4; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 4 + j], 1);
            } else {
                ASSERT_EQ(matrix[i * 4 + j], 0);
            }
        }
    }
}

TEST(WNMatrix4ValidationUINT16, CreateIdentityTest) {
    WNMatrix4<WN_UINT16> matrix = WNMatrix4<WN_UINT16>(1);

    matrix = WNMatrix4<WN_UINT16>::CreateIdentity();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        for (WN_UINT32 j = 0; j < 4; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 4 + j], 1);
            } else {
                ASSERT_EQ(matrix[i * 4 + j], 0);
            }
        }
    }
}

TEST(WNMatrix4ValidationUINT32, CreateIdentityTest) {
    WNMatrix4<WN_UINT32> matrix = WNMatrix4<WN_UINT32>(1);

    matrix = WNMatrix4<WN_UINT32>::CreateIdentity();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        for (WN_UINT32 j = 0; j < 4; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 4 + j], 1);
            } else {
                ASSERT_EQ(matrix[i * 4 + j], 0);
            }
        }
    }
}

TEST(WNMatrix4ValidationUINT64, CreateIdentityTest) {
    WNMatrix4<WN_UINT64> matrix = WNMatrix4<WN_UINT64>(1);

    matrix = WNMatrix4<WN_UINT64>::CreateIdentity();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        for (WN_UINT32 j = 0; j < 4; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 4 + j], 1);
            } else {
                ASSERT_EQ(matrix[i * 4 + j], 0);
            }
        }
    }
}

TEST(WNMatrix4ValidationINT8, CreateIdentityTest) {
    WNMatrix4<WN_INT8> matrix = WNMatrix4<WN_INT8>(1);

    matrix = WNMatrix4<WN_INT8>::CreateIdentity();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        for (WN_UINT32 j = 0; j < 4; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 4 + j], 1);
            } else {
                ASSERT_EQ(matrix[i * 4 + j], 0);
            }
        }
    }
}

TEST(WNMatrix4ValidationINT16, CreateIdentityTest) {
    WNMatrix4<WN_INT16> matrix = WNMatrix4<WN_INT16>(1);

    matrix = WNMatrix4<WN_INT16>::CreateIdentity();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        for (WN_UINT32 j = 0; j < 4; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 4 + j], 1);
            } else {
                ASSERT_EQ(matrix[i * 4 + j], 0);
            }
        }
    }
}

TEST(WNMatrix4ValidationINT32, CreateIdentityTest) {
    WNMatrix4<WN_INT32> matrix = WNMatrix4<WN_INT32>(1);

    matrix = WNMatrix4<WN_INT32>::CreateIdentity();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        for (WN_UINT32 j = 0; j < 4; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 4 + j], 1);
            } else {
                ASSERT_EQ(matrix[i * 4 + j], 0);
            }
        }
    }
}

TEST(WNMatrix4ValidationINT64, CreateIdentityTest) {
    WNMatrix4<WN_INT64> matrix = WNMatrix4<WN_INT64>(1);

    matrix = WNMatrix4<WN_INT64>::CreateIdentity();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        for (WN_UINT32 j = 0; j < 4; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 4 + j], 1);
            } else {
                ASSERT_EQ(matrix[i * 4 + j], 0);
            }
        }
    }
}

TEST(WNMatrix4ValidationFLOAT16, CreateIdentityTest) {
    WNMatrix4<WN_FLOAT16> matrix = WNMatrix4<WN_FLOAT16>(static_cast<WN_FLOAT16>(1.0f));

    matrix = WNMatrix4<WN_FLOAT16>::CreateIdentity();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        for (WN_UINT32 j = 0; j < 4; ++j) {
            if (i == j) {
                ASSERT_EQ(static_cast<WN_FLOAT32>(matrix[i * 4 + j]), 1.0f);
            } else {
                ASSERT_EQ(static_cast<WN_FLOAT32>(matrix[i * 4 + j]), 0.0f);
            }
        }
    }
}

TEST(WNMatrix4ValidationFLOAT32, CreateIdentityTest) {
    WNMatrix4<WN_FLOAT32> matrix = WNMatrix4<WN_FLOAT32>(1.0f);

    matrix = WNMatrix4<WN_FLOAT32>::CreateIdentity();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        for (WN_UINT32 j = 0; j < 4; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 4 + j], 1.0f);
            } else {
                ASSERT_EQ(matrix[i * 4 + j], 0.0f);
            }
        }
    }
}

TEST(WNMatrix4ValidationFLOAT64, CreateIdentityTest) {
    WNMatrix4<WN_FLOAT64> matrix = WNMatrix4<WN_FLOAT64>(1.0);

    matrix = WNMatrix4<WN_FLOAT64>::CreateIdentity();

    for (WN_UINT32 i = 0; i < 4; ++i) {
        for (WN_UINT32 j = 0; j < 4; ++j) {
            if (i == j) {
                ASSERT_EQ(matrix[i * 4 + j], 1.0);
            } else {
                ASSERT_EQ(matrix[i * 4 + j], 0.0);
            }
        }
    }
}
