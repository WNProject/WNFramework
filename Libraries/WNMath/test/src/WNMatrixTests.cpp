#include "WNMath/test/inc/Common.h"

TEST(matrix, construction) {
    const wn::matrix<wn_int32, 5> matrix1;
    const wn::matrix<wn_int32, 5> matrix2(1);

    for (wn_size_t i = 0; i < matrix2.dimension(); ++i) {
        ASSERT_EQ(matrix2[i], 1);
    }

    const wn::matrix<wn_int32, 5> matrix3(matrix2);

    for (wn_size_t i = 0; i < matrix3.dimension(); ++i) {
        ASSERT_EQ(matrix3[i], 1);
    }

    const wn::matrix<wn_int32, 5> matrix4(std::move(matrix3));

    for (wn_size_t i = 0; i < matrix4.dimension(); ++i) {
        ASSERT_EQ(matrix4[i], 1);
    }

    /*const wn::matrix<wn_int32, 5> matrix5(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

    for (wn_size_t i = 0; i < matrix5.dimension(); ++i) {
        ASSERT_EQ(matrix5[i], (i + 1));
    }

    const std::array<wn_int32, 5> array1{ { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 } };
    const wn::matrix<wn_int32, 5> matrix6(array1);

    for (wn_size_t i = 0; i < matrix6.dimension(); ++i) {
        ASSERT_EQ(matrix6[i], (i + 1));
    }

    const wn::matrix<wn_int32, 5> matrix7{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    for (wn_size_t i = 0; i < matrix7.dimension(); ++i) {
        ASSERT_EQ(matrix7[i], (i + 1));
    }*/
}