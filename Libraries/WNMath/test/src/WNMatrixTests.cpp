// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNExecutableTest/inc/WNTestHarness.h"
#include "WNMath/inc/WNMatrix.h"

template <typename T, size_t Rows, size_t Columns>
struct matrix_test_type final {
  enum { rows = Rows, columns = Columns };

  using value_type = T;
};

WN_ENABLE_TYPED_TEST(matrix);
WN_ENABLE_TYPED_TEST(matrix_square);

using matrix_testing_types = ::testing::Types<
    // square matricies
    matrix_test_type<int32_t, 1, 1>, matrix_test_type<int64_t, 1, 1>,
    matrix_test_type<float, 1, 1>, matrix_test_type<double, 1, 1>,
    matrix_test_type<int32_t, 2, 2>, matrix_test_type<int64_t, 2, 2>,
    matrix_test_type<float, 2, 2>, matrix_test_type<double, 2, 2>,
    matrix_test_type<int32_t, 3, 3>, matrix_test_type<int64_t, 3, 3>,
    matrix_test_type<float, 3, 3>, matrix_test_type<double, 3, 3>,
    matrix_test_type<int32_t, 4, 4>, matrix_test_type<int64_t, 4, 4>,
    matrix_test_type<float, 4, 4>, matrix_test_type<double, 4, 4>,
    matrix_test_type<int32_t, 9, 9>, matrix_test_type<int64_t, 9, 9>,
    matrix_test_type<float, 9, 9>, matrix_test_type<double, 9, 9>,
    matrix_test_type<int32_t, 10, 10>, matrix_test_type<int64_t, 10, 10>,
    matrix_test_type<float, 10, 10>, matrix_test_type<double, 10, 10>,
    matrix_test_type<int32_t, 17, 17>, matrix_test_type<int64_t, 17, 17>,
    matrix_test_type<float, 17, 17>, matrix_test_type<double, 17, 17>,
    // non-square matricies
    matrix_test_type<int32_t, 1, 2>, matrix_test_type<int64_t, 1, 2>,
    matrix_test_type<float, 1, 2>, matrix_test_type<double, 1, 2>,
    matrix_test_type<int32_t, 2, 1>, matrix_test_type<int64_t, 2, 1>,
    matrix_test_type<float, 2, 1>, matrix_test_type<double, 2, 1>,
    matrix_test_type<int32_t, 3, 4>, matrix_test_type<int64_t, 3, 4>,
    matrix_test_type<float, 3, 4>, matrix_test_type<double, 3, 4>,
    matrix_test_type<int32_t, 8, 10>, matrix_test_type<int64_t, 8, 10>,
    matrix_test_type<float, 8, 10>, matrix_test_type<double, 8, 10>>;

using matrix_square_testing_types = ::testing::Types<
    // square matricies only
    matrix_test_type<int32_t, 1, 1>, matrix_test_type<int64_t, 1, 1>,
    matrix_test_type<float, 1, 1>, matrix_test_type<double, 1, 1>,
    matrix_test_type<int32_t, 2, 2>, matrix_test_type<int64_t, 2, 2>,
    matrix_test_type<float, 2, 2>, matrix_test_type<double, 2, 2>,
    matrix_test_type<int32_t, 3, 3>, matrix_test_type<int64_t, 3, 3>,
    matrix_test_type<float, 3, 3>, matrix_test_type<double, 3, 3>,
    matrix_test_type<int32_t, 4, 4>, matrix_test_type<int64_t, 4, 4>,
    matrix_test_type<float, 4, 4>, matrix_test_type<double, 4, 4>,
    matrix_test_type<int32_t, 9, 9>, matrix_test_type<int64_t, 9, 9>,
    matrix_test_type<float, 9, 9>, matrix_test_type<double, 9, 9>,
    matrix_test_type<int32_t, 10, 10>, matrix_test_type<int64_t, 10, 10>,
    matrix_test_type<float, 10, 10>, matrix_test_type<double, 10, 10>,
    matrix_test_type<int32_t, 17, 17>, matrix_test_type<int64_t, 17, 17>,
    matrix_test_type<float, 17, 17>, matrix_test_type<double, 17, 17>>;

TYPED_TEST_CASE(matrix, matrix_testing_types);
TYPED_TEST_CASE(matrix_square, matrix_square_testing_types);

// construction ///////////////////////////////////////////////////////////////

TYPED_TEST(matrix, construction) {
  using value_type = typename TypeParam::value_type;
  using matrix_type = wn::math::matrix<typename TypeParam::value_type,
      TypeParam::rows, TypeParam::columns>;

  const size_t dimension = (TypeParam::rows * TypeParam::columns);
  const value_type values[] = {1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233};

  for (auto value : values) {
    matrix_type matrix(value);
    const matrix_type matrix1(matrix);

    EXPECT_EQ(matrix, matrix1);

    for (size_t j = 0; j < dimension; ++j) {
      EXPECT_EQ(matrix1[j], value);
    }

    const matrix_type matrix2(wn::core::move(matrix));

    EXPECT_EQ(matrix1, matrix2);

    for (size_t j = 0; j < dimension; ++j) {
      EXPECT_EQ(matrix2[j], value);
    }
  }
}

TEST(matrix, more_construction) {
  const wn::math::matrix<int, 3, 4> matrix(
      {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});

  for (int i = 0; i < (3 * 4); ++i) {
    EXPECT_EQ(matrix[i], (i + 1));
  }

  const wn::math::matrix<int, 2, 8> matrix1(
      {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});

  for (int i = 0; i < (2 * 8); ++i) {
    EXPECT_EQ(matrix1[i], (i + 1));
  }

  const wn::math::matrix<int, 3, 4> matrix2({
      {1, 2, 3},    // column 1
      {4, 5, 6},    // column 2
      {7, 8, 9},    // column 3
      {10, 11, 12}  // column 4
  });
  int count = 0;

  for (size_t column = 0; column < 4; ++column) {
    for (size_t row = 0; row < 3; ++row) {
      EXPECT_EQ(matrix2.at(row, column), ++count);
    }
  }

  const std::vector<int> values({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});
  const wn::math::matrix<int, 3, 4> matrix3(values.cbegin(), values.cend());

  for (int i = 0; i < (3 * 4); ++i) {
    EXPECT_EQ(matrix3[i], (i + 1));
  }

  const std::vector<int> values1({1, 2, 3, 4, 5, 6, 7, 8});
  const wn::math::matrix<int, 2, 4> matrix4(values1.cbegin(), values1.cend());

  for (int i = 0; i < (2 * 4); ++i) {
    EXPECT_EQ(matrix4[i], (i + 1));
  }
}

// assignment /////////////////////////////////////////////////////////////////

TYPED_TEST(matrix, assignment) {
  using value_type = typename TypeParam::value_type;
  using matrix_type = wn::math::matrix<typename TypeParam::value_type,
      TypeParam::rows, TypeParam::columns>;

  const value_type values[] = {1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233};

  for (auto value : values) {
    const size_t dimension = (TypeParam::rows * TypeParam::columns);
    matrix_type matrix(value);
    matrix_type matrix1;
    matrix_type matrix2;
    matrix_type matrix3;

    matrix1 = matrix;

    EXPECT_EQ(matrix, matrix1);

    for (size_t j = 0; j < dimension; ++j) {
      EXPECT_EQ(matrix1[j], value);
    }

    matrix2 = wn::core::move(matrix);

    EXPECT_EQ(matrix1, matrix2);

    for (size_t j = 0; j < dimension; ++j) {
      EXPECT_EQ(matrix2[j], value);
    }

    matrix3 = value;

    for (size_t j = 0; j < dimension; ++j) {
      EXPECT_EQ(matrix3[j], value);
    }
  }
}

TEST(matrix, more_assignment) {
  wn::math::matrix<int, 3, 4> matrix;
  wn::math::matrix<int, 2, 8> matrix1;

  matrix = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

  for (int i = 0; i < (3 * 4); ++i) {
    EXPECT_EQ(matrix[i], (i + 1));
  }

  matrix1 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

  for (int i = 0; i < (2 * 8); ++i) {
    EXPECT_EQ(matrix1[i], (i + 1));
  }

  wn::math::matrix<int, 3, 4> matrix2;

  matrix2 = {
      {1, 2, 3},    // column 1
      {4, 5, 6},    // column 2
      {7, 8, 9},    // column 3
      {10, 11, 12}  // column 4
  };

  int count = 0;

  for (size_t column = 0; column < 4; ++column) {
    for (size_t row = 0; row < 3; ++row) {
      EXPECT_EQ(matrix2.at(row, column), ++count);
    }
  }
}

// element access /////////////////////////////////////////////////////////////

TYPED_TEST(matrix, element_access) {
  using value_type = typename TypeParam::value_type;
  using matrix_type = wn::math::matrix<typename TypeParam::value_type,
      TypeParam::rows, TypeParam::columns>;
  const size_t dimension = (TypeParam::rows * TypeParam::columns);

  matrix_type matrix;

  for (size_t i = 0; i < dimension; ++i) {
    matrix[i] = value_type(i);
  }

  for (size_t i = 0; i < dimension; ++i) {
    EXPECT_EQ(matrix[i], value_type(i));
    EXPECT_EQ(matrix.at(i), value_type(i));
  }

  value_type count(0);

  for (size_t column = 0; column < TypeParam::columns; ++column) {
    for (size_t row = 0; row < TypeParam::rows; ++row) {
      EXPECT_EQ(matrix.at(row, column), count++);
    }
  }
}

// transpose //////////////////////////////////////////////////////////////////

TYPED_TEST(matrix_square, transpose) {
  using value_type = typename TypeParam::value_type;
  using matrix_type =
      wn::math::matrix<value_type, TypeParam::rows, TypeParam::columns>;

  matrix_type matrix;
  value_type count(0);

  for (size_t row = 0; row < TypeParam::rows; ++row) {
    for (size_t column = 0; column < TypeParam::columns; ++column) {
      matrix.at(row, column) = count++;
    }
  }

  matrix.transpose();

  count = value_type(0);

  for (size_t column = 0; column < TypeParam::columns; ++column) {
    for (size_t row = 0; row < TypeParam::rows; ++row) {
      EXPECT_EQ(matrix.at(row, column), count++);
    }
  }

  matrix.transpose();

  count = value_type(0);

  for (size_t row = 0; row < TypeParam::rows; ++row) {
    for (size_t column = 0; column < TypeParam::columns; ++column) {
      EXPECT_EQ(matrix.at(row, column), count++);
    }
  }
}

// transposed /////////////////////////////////////////////////////////////////

TYPED_TEST(matrix, transposed) {
  using value_type = typename TypeParam::value_type;
  using matrix_type1 =
      wn::math::matrix<value_type, TypeParam::rows, TypeParam::columns>;
  using matrix_type2 =
      wn::math::matrix<value_type, TypeParam::columns, TypeParam::rows>;

  matrix_type1 matrix;
  value_type count(0);

  for (size_t row = 0; row < TypeParam::rows; ++row) {
    for (size_t column = 0; column < TypeParam::columns; ++column) {
      matrix.at(row, column) = count++;
    }
  }

  const matrix_type2 matrix1(matrix.transposed());

  for (size_t row = 0; row < TypeParam::rows; ++row) {
    for (size_t column = 0; column < TypeParam::columns; ++column) {
      EXPECT_EQ(matrix.at(row, column), matrix1.at(column, row));
    }
  }

  const matrix_type1 matrix2(matrix1.transposed());

  for (size_t row = 0; row < TypeParam::rows; ++row) {
    for (size_t column = 0; column < TypeParam::columns; ++column) {
      EXPECT_EQ(matrix2.at(row, column), matrix1.at(column, row));
      EXPECT_EQ(matrix2.at(row, column), matrix.at(row, column));
    }
  }
}

// trace //////////////////////////////////////////////////////////////////////

TYPED_TEST(matrix_square, trace) {
  using value_type = typename TypeParam::value_type;
  using matrix_type =
      wn::math::matrix<value_type, TypeParam::columns, TypeParam::rows>;

  matrix_type matrix;
  value_type trace(0);

  for (size_t i = 0; i < TypeParam::columns; ++i) {
    for (size_t j = 0; j < TypeParam::rows; ++j) {
      if (i == j) {
        matrix.at(i, j) = value_type(1);

        trace++;
      } else {
        matrix.at(i, j) = value_type(0);
      }
    }
  }

  EXPECT_EQ(matrix.trace(), trace);

  trace = value_type(0);

  value_type count(0);

  for (size_t i = 0; i < TypeParam::columns; ++i) {
    for (size_t j = 0; j < TypeParam::rows; ++j) {
      const value_type value = count++;

      matrix.at(i, j) = value;

      if (i == j) {
        trace += value;
      }
    }
  }

  EXPECT_EQ(matrix.trace(), trace);
}

// multiplication /////////////////////////////////////////////////////////////

// TODO(devpow112): Generlize this so multiple types/sizes are tested
TEST(matrix, multiplication) {
  wn::math::matrix<int, 3, 4> matrix1({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});
  wn::math::matrix<int, 4, 3> matrix2(
      {13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24});
  const wn::math::matrix<int, 3, 3> matrix3(matrix1 * matrix2);
  const wn::math::matrix<int, 3, 3> expected(
      {334, 392, 450, 422, 496, 570, 510, 600, 690});

  EXPECT_EQ(expected, matrix3);
}