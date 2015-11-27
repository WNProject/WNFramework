#include "WNMath/test/inc/Common.h"
#include "WNTesting/inc/WNTestHarness.h"

TEST(vector, construction) {
    const wn::vector<wn_int32, 10> vector1;
    const wn::vector<wn_int32, 10> vector2(1);

    for (wn_size_t i = 0; i < vector2.dimension(); ++i) {
        ASSERT_EQ(vector2[i], 1);
    }

    const wn::vector<wn_int32, 10> vector3(vector2);

    for (wn_size_t i = 0; i < vector3.dimension(); ++i) {
        ASSERT_EQ(vector3[i], 1);
    }

    const wn::vector<wn_int32, 10> vector4(std::move(vector3));

    for (wn_size_t i = 0; i < vector4.dimension(); ++i) {
        ASSERT_EQ(vector4[i], 1);
    }

    const wn::vector<wn_int32, 10> vector5(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

    for (wn_size_t i = 0; i < vector5.dimension(); ++i) {
        ASSERT_EQ(vector5[i], (i + 1));
    }

    const std::array<wn_int32, 10> array1 {{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }};
    const wn::vector<wn_int32, 10> vector6(array1);

    for (wn_size_t i = 0; i < vector6.dimension(); ++i) {
        ASSERT_EQ(vector6[i], (i + 1));
    }

    const wn::vector<wn_int32, 10> vector7 { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    for (wn_size_t i = 0; i < vector7.dimension(); ++i) {
        ASSERT_EQ(vector7[i], (i + 1));
    }
}

TEST(vector, addition) {
    wn::vector<wn_int32, 10> vector1(0);

    vector1 += 1;

    for (wn_size_t i = 0; i < vector1.dimension(); ++i) {
        ASSERT_EQ(vector1[i], 1);
    }

    wn::vector<wn_int32, 10> vector2 = vector1 + 1;

    for (wn_size_t i = 0; i < vector2.dimension(); ++i) {
        ASSERT_EQ(vector2[i], 2);
    }

    for (wn_size_t i = 0; i < vector1.dimension(); ++i) {
        ASSERT_EQ(vector1[i], 1);
    }

    vector1 += vector2;

    for (wn_size_t i = 0; i < vector1.dimension(); ++i) {
        ASSERT_EQ(vector1[i], 3);
    }

    wn::vector<wn_int32, 10> vector4 = vector1 + vector2;

    for (wn_size_t i = 0; i < vector4.dimension(); ++i) {
        ASSERT_EQ(vector4[i], 5);
    }

    for (wn_size_t i = 0; i < vector2.dimension(); ++i) {
        ASSERT_EQ(vector2[i], 2);
    }
}

TEST(vector, subtraction) {
    wn::vector<wn_int32, 10> vector1(5);

    vector1 -= 1;

    for (const wn_int32 i : vector1) {
        ASSERT_EQ(i, 4);
    }

    wn::vector<wn_int32, 10> vector2 = vector1 - 1;

    for (const wn_int32 i : vector2) {
        ASSERT_EQ(i, 3);
    }

    for (const wn_int32 i : vector1) {
        ASSERT_EQ(i, 4);
    }

    vector1 -= vector2;

    for (const wn_int32 i : vector1) {
        ASSERT_EQ(i, 1);
    }

    wn::vector<wn_int32, 10> vector4 = vector1 - vector2;

    for (const wn_int32 i : vector4) {
        ASSERT_EQ(i, -2);
    }

    for (const wn_int32 i : vector2) {
        ASSERT_EQ(i, 3);
    }
}

TEST(vector, multiplication) {
    wn::vector<wn_int32, 10> vector1(1);

    vector1 *= 2;

    for (wn_size_t i = 0; i < vector1.dimension(); ++i) {
        ASSERT_EQ(vector1[i], 2);
    }
}

TEST(vector, division) {
    wn::vector<wn_int32, 10> vector1(2);

    vector1 /= 2;

    for (wn_size_t i = 0; i < vector1.dimension(); ++i) {
        ASSERT_EQ(vector1[i], 1);
    }
}