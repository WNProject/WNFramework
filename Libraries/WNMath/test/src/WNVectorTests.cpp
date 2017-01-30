// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNMath/inc/WNVector.h"
#include "WNTesting/inc/WNTestHarness.h"

WN_ENABLE_TYPED_TEST(vector);
WN_ENABLE_TYPED_TEST(vector_real);
WN_ENABLE_TYPED_TEST(packed_vector);

template <typename T, const size_t Dimension>
struct vector_test_type final {
  enum { dimension = Dimension };

  using type = T;
};

using vector_testing_types =
    ::testing::Types<vector_test_type<int32_t, 1>, vector_test_type<int64_t, 1>,
        vector_test_type<float, 1>, vector_test_type<double, 1>,
        vector_test_type<int32_t, 2>, vector_test_type<int64_t, 2>,
        vector_test_type<float, 2>, vector_test_type<double, 2>,
        vector_test_type<int32_t, 3>, vector_test_type<int64_t, 3>,
        vector_test_type<float, 3>, vector_test_type<double, 3>,
        vector_test_type<int32_t, 4>, vector_test_type<int64_t, 4>,
        vector_test_type<float, 4>, vector_test_type<double, 4>,
        vector_test_type<int32_t, 9>, vector_test_type<int64_t, 9>,
        vector_test_type<float, 9>, vector_test_type<double, 9>,
        vector_test_type<int32_t, 10>, vector_test_type<int64_t, 10>,
        vector_test_type<float, 10>, vector_test_type<double, 10>,
        vector_test_type<int32_t, 17>, vector_test_type<int64_t, 17>,
        vector_test_type<float, 17>, vector_test_type<double, 17>>;

using vector_real_testing_types =
    ::testing::Types<vector_test_type<float, 2>, vector_test_type<double, 2>,
        vector_test_type<float, 3>, vector_test_type<double, 3>,
        vector_test_type<float, 4>, vector_test_type<double, 4>,
        vector_test_type<float, 9>, vector_test_type<double, 9>,
        vector_test_type<float, 10>, vector_test_type<double, 10>,
        vector_test_type<float, 17>, vector_test_type<double, 17>>;

TYPED_TEST_CASE(vector, vector_testing_types);
TYPED_TEST_CASE(vector_real, vector_real_testing_types);
TYPED_TEST_CASE(packed_vector, vector_testing_types);

TYPED_TEST(vector, construction) {
  using test_vector =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  test_vector vector(typename TypeParam::type(1));

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(1));
  }

  const test_vector vector1(vector);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(1));
    EXPECT_EQ(vector1[i], typename TypeParam::type(1));
  }

  const test_vector vector2(wn::core::move(vector));

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector2[i], typename TypeParam::type(1));
  }
}

TYPED_TEST(vector, assignment) {
  using test_vector =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  const test_vector vector(typename TypeParam::type(1));
  test_vector vector1;

  vector1 = vector;

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(1));
    EXPECT_EQ(vector1[i], typename TypeParam::type(1));
  }

  test_vector vector2(typename TypeParam::type(2));
  test_vector vector3;

  vector3 = wn::core::move(vector2);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector3[i], typename TypeParam::type(2));
  }

  vector3 = typename TypeParam::type(3);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector3[i], typename TypeParam::type(3));
  }

  vector3.assign(vector);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector3[i], typename TypeParam::type(1));
  }

  vector2 = typename TypeParam::type(2);

  vector3.assign(std::move(vector2));

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector3[i], typename TypeParam::type(2));
  }

  vector3.assign(typename TypeParam::type(4));

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector3[i], typename TypeParam::type(4));
  }
}

TYPED_TEST(vector, element_access) {
  using test_vector =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  const test_vector vector(typename TypeParam::type(1));

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(1));
    EXPECT_EQ(vector.at(i), typename TypeParam::type(1));
    EXPECT_EQ(*(vector.data() + i), typename TypeParam::type(1));
  }
}

TYPED_TEST(vector, iteration) {
  using test_vector =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  const test_vector vector(typename TypeParam::type(1));
  int c = 0;

  for (const typename TypeParam::type& value : vector) {
    EXPECT_EQ(value, typename TypeParam::type(1));

    c++;
  }

  EXPECT_EQ(c, TypeParam::dimension);

  c = 0;

  for (auto i = vector.cbegin(); i != vector.cend(); ++i) {
    EXPECT_EQ(*i, typename TypeParam::type(1));

    c++;
  }

  EXPECT_EQ(c, TypeParam::dimension);

  c = 0;

  for (auto i = vector.crbegin(); i != vector.crend(); ++i) {
    EXPECT_EQ(*i, typename TypeParam::type(1));

    c++;
  }

  EXPECT_EQ(c, TypeParam::dimension);
}

TYPED_TEST(vector, unary_plus) {
  using test_vector =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  const test_vector vector(typename TypeParam::type(1));
  const test_vector vector1(+vector);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(1));
    EXPECT_EQ(vector1[i], typename TypeParam::type(1));
  }
}

TYPED_TEST(vector, unary_minus) {
  using test_vector =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  const test_vector vector(typename TypeParam::type(2));
  const test_vector vector1(-vector);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(2));
    EXPECT_EQ(vector1[i], typename TypeParam::type(-2));
  }
}

TYPED_TEST(vector, addition) {
  using test_vector =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  test_vector vector(typename TypeParam::type(1));
  const test_vector vector1(typename TypeParam::type(2));

  vector += vector1;

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(3));
    EXPECT_EQ(vector1[i], typename TypeParam::type(2));
  }

  const test_vector vector2(vector + vector1);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(3));
    EXPECT_EQ(vector1[i], typename TypeParam::type(2));
    EXPECT_EQ(vector2[i], typename TypeParam::type(5));
  }

  test_vector vector3(typename TypeParam::type(1));

  vector3 += typename TypeParam::type(1);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector3[i], typename TypeParam::type(2));
  }

  const test_vector vector4(vector3 + typename TypeParam::type(1));

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector3[i], typename TypeParam::type(2));
    EXPECT_EQ(vector4[i], typename TypeParam::type(3));
  }
}

TYPED_TEST(vector, subtraction) {
  using test_vector =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  test_vector vector(typename TypeParam::type(5));
  test_vector vector1(typename TypeParam::type(3));

  vector -= vector1;

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(2));
    EXPECT_EQ(vector1[i], typename TypeParam::type(3));
  }

  test_vector vector2(vector - vector1);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(2));
    EXPECT_EQ(vector1[i], typename TypeParam::type(3));
    EXPECT_EQ(vector2[i], typename TypeParam::type(-1));
  }

  test_vector vector3(typename TypeParam::type(3));

  vector3 -= typename TypeParam::type(1);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector3[i], typename TypeParam::type(2));
  }

  const test_vector vector4(vector3 - typename TypeParam::type(1));

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector3[i], typename TypeParam::type(2));
    EXPECT_EQ(vector4[i], typename TypeParam::type(1));
  }
}

TYPED_TEST(vector, multiply) {
  using test_vector =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  test_vector vector(typename TypeParam::type(1));
  const test_vector vector1(typename TypeParam::type(2));

  vector *= vector1;

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(2));
    EXPECT_EQ(vector1[i], typename TypeParam::type(2));
  }

  const test_vector vector2(vector * vector1);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(2));
    EXPECT_EQ(vector1[i], typename TypeParam::type(2));
    EXPECT_EQ(vector2[i], typename TypeParam::type(4));
  }

  test_vector vector3(typename TypeParam::type(1));

  vector3 *= typename TypeParam::type(2);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector3[i], typename TypeParam::type(2));
  }

  const test_vector vector4(vector3 * typename TypeParam::type(2));

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector3[i], typename TypeParam::type(2));
    EXPECT_EQ(vector4[i], typename TypeParam::type(4));
  }
}

TYPED_TEST(vector, divide) {
  using test_vector =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  test_vector vector(typename TypeParam::type(4));
  const test_vector vector1(typename TypeParam::type(2));

  vector /= vector1;

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(2));
    EXPECT_EQ(vector1[i], typename TypeParam::type(2));
  }

  const test_vector vector2(vector / vector1);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(2));
    EXPECT_EQ(vector1[i], typename TypeParam::type(2));
    EXPECT_EQ(vector2[i], typename TypeParam::type(1));
  }

  test_vector vector3(typename TypeParam::type(6));

  vector3 /= typename TypeParam::type(2);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector3[i], typename TypeParam::type(3));
  }

  const test_vector vector4(vector3 / typename TypeParam::type(3));

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector3[i], typename TypeParam::type(3));
    EXPECT_EQ(vector4[i], typename TypeParam::type(1));
  }
}

TYPED_TEST(vector, equality) {
  using test_vector =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  const test_vector vector(typename TypeParam::type(1));
  const test_vector vector1(vector);

  EXPECT_EQ(vector, vector1);
  EXPECT_TRUE(vector == vector1);
  EXPECT_FALSE(vector != vector1);
}

TYPED_TEST(vector, inequality) {
  using test_vector =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  const test_vector vector(typename TypeParam::type(1));
  const test_vector vector1(typename TypeParam::type(2));

  EXPECT_NE(vector, vector1);
  EXPECT_FALSE(vector == vector1);
  EXPECT_TRUE(vector != vector1);
}

TYPED_TEST(vector, multiply_add) {
  using test_vector =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  test_vector vector(typename TypeParam::type(2));
  const test_vector vector1(typename TypeParam::type(3));
  const test_vector vector2(typename TypeParam::type(4));

  vector.multiply_add(vector1, vector2);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(10));
  }

  vector.multiply_add(typename TypeParam::type(2), typename TypeParam::type(3));

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(23));
  }
}

TYPED_TEST(vector, multiply_subtract) {
  using test_vector =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  test_vector vector(typename TypeParam::type(2));
  const test_vector vector1(typename TypeParam::type(3));
  const test_vector vector2(typename TypeParam::type(4));

  vector.multiply_subtract(vector1, vector2);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(2));
  }

  vector.multiply_subtract(
      typename TypeParam::type(2), typename TypeParam::type(3));

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(1));
  }
}

TYPED_TEST(vector, translate) {
  using test_vector =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  test_vector vector(typename TypeParam::type(1));
  const test_vector vector1(typename TypeParam::type(2));

  vector.translate(vector1);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(3));
    EXPECT_EQ(vector1[i], typename TypeParam::type(2));
  }

  test_vector vector2(typename TypeParam::type(1));

  vector2.translate(typename TypeParam::type(1));

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector2[i], typename TypeParam::type(2));
  }
}

TYPED_TEST(vector, scale) {
  using test_vector =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  test_vector vector(typename TypeParam::type(1));
  const test_vector vector1(typename TypeParam::type(2));

  vector.scale(vector1);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(2));
    EXPECT_EQ(vector1[i], typename TypeParam::type(2));
  }

  test_vector vector2(typename TypeParam::type(1));

  vector2.scale(typename TypeParam::type(2));

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector2[i], typename TypeParam::type(2));
  }
}

TYPED_TEST(vector, reverse) {
  using test_vector =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  test_vector vector;

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    vector[i] = typename TypeParam::type(i);
  }

  vector.reverse();

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(
        vector[i], typename TypeParam::type((TypeParam::dimension - 1) - i));
  }

  vector.reverse();

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(i));
  }
}

TYPED_TEST(vector, clamp) {
  using test_vector =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  test_vector vector(typename TypeParam::type(4));
  const test_vector vector1(typename TypeParam::type(3));
  const test_vector vector2(typename TypeParam::type(5));

  vector.clamp(vector1, vector2);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(4));
    EXPECT_EQ(vector1[i], typename TypeParam::type(3));
    EXPECT_EQ(vector2[i], typename TypeParam::type(5));
  }

  vector = typename TypeParam::type(1);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(1));
  }

  vector.clamp(vector1, vector2);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(3));
    EXPECT_EQ(vector1[i], typename TypeParam::type(3));
    EXPECT_EQ(vector2[i], typename TypeParam::type(5));
  }

  vector = typename TypeParam::type(4);

  vector.clamp(typename TypeParam::type(3), typename TypeParam::type(5));

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(4));
  }

  vector = typename TypeParam::type(1);

  vector.clamp(typename TypeParam::type(3), typename TypeParam::type(5));

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(3));
  }
}

TYPED_TEST(vector, saturate) {
  using test_vector =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  test_vector vector(typename TypeParam::type(4));

  vector.saturate();

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(1));
  }

  vector = typename TypeParam::type(-5);

  vector.saturate();

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(0));
  }

  vector = typename TypeParam::type(0);

  vector.saturate();

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(0));
  }
}

TYPED_TEST(vector, minimum) {
  using test_vector =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  test_vector vector(typename TypeParam::type(4));
  const test_vector vector1(typename TypeParam::type(3));

  vector.minimum(vector1);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(3));
    EXPECT_EQ(vector1[i], typename TypeParam::type(3));
  }

  vector = typename TypeParam::type(2);

  vector.minimum(vector1);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(2));
    EXPECT_EQ(vector1[i], typename TypeParam::type(3));
  }
}

TYPED_TEST(vector, maximum) {
  using test_vector =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  test_vector vector(typename TypeParam::type(4));
  const test_vector vector1(typename TypeParam::type(5));

  vector.maximum(vector1);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(5));
    EXPECT_EQ(vector1[i], typename TypeParam::type(5));
  }

  vector = typename TypeParam::type(6);

  vector.maximum(vector1);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(6));
    EXPECT_EQ(vector1[i], typename TypeParam::type(5));
  }
}

TYPED_TEST(vector, average) {
  using test_vector =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  test_vector vector(typename TypeParam::type(3));
  const test_vector vector1(typename TypeParam::type(5));

  vector.average(vector1);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(4));
    EXPECT_EQ(vector1[i], typename TypeParam::type(5));
  }
}

TYPED_TEST(vector, swap) {
  using test_vector =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  test_vector vector(typename TypeParam::type(4));
  test_vector vector1(typename TypeParam::type(3));

  vector.swap(vector1);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(3));
    EXPECT_EQ(vector1[i], typename TypeParam::type(4));
  }
}

TYPED_TEST(vector_real, recipricol) {
  using test_vector =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  test_vector vector(typename TypeParam::type(4));

  vector.recipricol();

  const typename TypeParam::type value(
      typename TypeParam::type(1) / typename TypeParam::type(4));

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_NEAR(vector[i], value, (typename TypeParam::type(0.01) * value));
  }
}

TEST(vector, more_construction) {
  const wn::math::vector<int, 6> vector({0, 1, 2, 3, 4, 5});

  EXPECT_EQ(vector[0], 0);
  EXPECT_EQ(vector[1], 1);
  EXPECT_EQ(vector[2], 2);
  EXPECT_EQ(vector[3], 3);
  EXPECT_EQ(vector[4], 4);
  EXPECT_EQ(vector[5], 5);

  const wn::math::vector<int, 2> vector1({6, 7});

  EXPECT_EQ(vector1[0], 6);
  EXPECT_EQ(vector1[1], 7);

  const wn::math::vector<int, 15> vector2(
      {8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22});

  for (int i = 0; i < 15; ++i) {
    EXPECT_EQ(vector2[i], (i + 8));
  }

  const wn::math::vector<int, 2> vector3(vector2);

  for (int i = 0; i < 2; ++i) {
    EXPECT_EQ(vector3[i], (i + 8));
  }

  const wn::math::vector<int, 5> vector4(vector3);

  for (int i = 0; i < 5; ++i) {
    if (i < 2) {
      EXPECT_EQ(vector4[i], (i + 8));
    } else {
      EXPECT_EQ(vector4[i], 0);
    }
  }
}

TEST(vector, more_element_access) {
  const wn::math::vector<int, 6> vector({0, 1, 2, 3, 4, 5});

  for (size_t i = 0; i < 6; ++i) {
    EXPECT_EQ(vector[i], i);
    EXPECT_EQ(vector.at(i), i);
    EXPECT_EQ(*(vector.data() + i), i);
  }
}

TEST(vector, more_iteration) {
  const wn::math::vector<int, 6> vector({0, 1, 2, 3, 4, 5});
  int c = 0;

  for (const int& value : vector) {
    EXPECT_EQ(value, c++);
  }

  EXPECT_EQ(c, 6);

  c = 0;

  for (auto i = vector.cbegin(); i != vector.cend(); ++i) {
    EXPECT_EQ(*i, c++);
  }

  EXPECT_EQ(c, 6);

  c = 0;

  for (auto i = vector.crbegin(); i != vector.crend(); ++i) {
    EXPECT_EQ(*i, (5 - (c++)));
  }

  EXPECT_EQ(c, 6);
}

TEST(vector, length) {
  const wn::math::vector<float, 6> vector({0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f});
  const float value(wn::math::sqrt(1.0f + 4.0f + 9.0f + 16.0f + 25.0f));

  EXPECT_NEAR(vector.length(), value, (0.01f * value));
}

TEST(vector, inverse_length) {
  const wn::math::vector<float, 6> vector({0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f});
  const float value(1.0f / wn::math::sqrt(1.0f + 4.0f + 9.0f + 16.0f + 25.0f));

  EXPECT_NEAR(vector.inverse_length(), value, (0.01f * value));
}

TEST(vector, length_squared) {
  const wn::math::vector<float, 6> vector({0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f});
  const float value(55.0f);

  EXPECT_NEAR(vector.length_squared(), value, (0.01f * value));
}

TEST(vector, dot) {
  const wn::math::vector<float, 6> vector({0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f});
  const float value(55.0f);

  EXPECT_NEAR(vector.dot(vector), value, (0.01f * value));

  const wn::math::vector<float, 6> vector1(
      {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f});
  const float value1(70.0f);

  EXPECT_NEAR(vector.dot(vector1), value1, (0.01f * value));
}

TEST(vector, cross) {
  const wn::math::vector<float, 3> vector({0.0f, 1.0f, 2.0f});
  const wn::math::vector<float, 3> vector1({0.0f, 1.0f, 2.0f});
  wn::math::vector<float, 3> vector2(vector.cross(vector1));

  for (const float& value : vector2) {
    EXPECT_NEAR(value, 0.0f, 0.01f);
  }
}

TEST(vector, normalize) {
  wn::math::vector<float, 6> vector({0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f});

  vector.normalize();

  const float value(1.0f);

  for (size_t i = 0; i < 6; ++i) {
    EXPECT_LT(vector[i], value);
  }

  EXPECT_NEAR(vector.length(), value, (0.01f * value));

  wn::math::vector<float, 2> vector1({0.234f, 21.0f});

  vector1.normalize();

  for (size_t i = 0; i < 2; ++i) {
    EXPECT_LT(vector1[i], value);
  }

  EXPECT_NEAR(vector1.length(), value, (0.01f * value));
}

TEST(vector, truncate) {
  wn::math::vector<float, 6> vector({0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f});
  const float value(4.0f);

  vector.truncate(value);

  EXPECT_NEAR(vector.length(), value, (0.01f * value));
}

TYPED_TEST(packed_vector, construction) {
  using test_vector =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;
  using test_packed_vector =
      wn::math::packed_vector<typename TypeParam::type, TypeParam::dimension>;

  const test_vector vector(typename TypeParam::type(1));

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(1));
  }

  const test_packed_vector packed_vector(vector);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(packed_vector[i], typename TypeParam::type(1));
  }

  const test_vector vector1(packed_vector);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector1[i], typename TypeParam::type(1));
  }
}

TYPED_TEST(packed_vector, assignment) {
  using test_vector =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;
  using test_packed_vector =
      wn::math::packed_vector<typename TypeParam::type, TypeParam::dimension>;

  const test_vector vector(typename TypeParam::type(1));
  test_packed_vector packed_vector(vector);

  packed_vector = vector;

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(packed_vector[i], typename TypeParam::type(1));
  }

  const test_vector vector1(typename TypeParam::type(2));

  packed_vector.assign(vector1);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(packed_vector[i], typename TypeParam::type(2));
  }
}
