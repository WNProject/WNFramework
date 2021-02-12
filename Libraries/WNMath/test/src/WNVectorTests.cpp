// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNMath/inc/WNVector.h"
#include "executable_test/inc/WNTestHarness.h"

WN_ENABLE_TYPED_TEST(vector);

template <typename T, size_t Dimension>
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

TYPED_TEST_SUITE(vector, vector_testing_types);

TYPED_TEST(vector, construction) {
  using value_type = typename TypeParam::type;
  using vector_type = wn::math::vector<value_type, TypeParam::dimension>;

  const value_type values[] = {1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233};

  for (auto value : values) {
    vector_type vector(value);
    const vector_type vector1(vector);

    EXPECT_EQ(vector, vector1);

    for (size_t j = 0; j < TypeParam::dimension; ++j) {
      EXPECT_EQ(vector1[j], value);
    }

    const vector_type vector2(wn::core::move(vector));

    EXPECT_EQ(vector1, vector2);

    for (size_t j = 0; j < TypeParam::dimension; ++j) {
      EXPECT_EQ(vector2[j], value);
    }
  }

  std::vector<value_type> more_values;

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    more_values.push_back(value_type(i));
  }

  const vector_type vector(more_values.cbegin(), more_values.cend());

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], value_type(i));
  }
}

TEST(vector, more_construction) {
  const wn::math::vector<int, 6> vector({0, 1, 2, 3, 4, 5});

  for (int i = 0; i < 6; ++i) {
    EXPECT_EQ(vector[i], i);
  }

  const wn::math::vector<int, 2> vector1({6, 7});

  for (int i = 0; i < 2; ++i) {
    EXPECT_EQ(vector1[i], (i + 6));
  }

  const wn::math::vector<int, 15> vector2(
      {8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22});

  for (int i = 0; i < 15; ++i) {
    EXPECT_EQ(vector2[i], (i + 8));
  }
}

TYPED_TEST(vector, assignment) {
  using vector_type =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  const vector_type vector(typename TypeParam::type(1));
  vector_type vector1;

  vector1 = vector;

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(1));
    EXPECT_EQ(vector1[i], typename TypeParam::type(1));
  }

  vector_type vector2(typename TypeParam::type(2));
  vector_type vector3;

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

  vector3.assign(wn::core::move(vector2));

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector3[i], typename TypeParam::type(2));
  }

  vector3.assign(typename TypeParam::type(4));

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector3[i], typename TypeParam::type(4));
  }
}

TYPED_TEST(vector, element_access) {
  using vector_type =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  const vector_type vector(typename TypeParam::type(1));

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(1));
    EXPECT_EQ(vector.at(i), typename TypeParam::type(1));
  }
}

TEST(vector, more_element_access) {
  const wn::math::vector<int, 6> vector({0, 1, 2, 3, 4, 5});

  for (int i = 0; i < 6; ++i) {
    EXPECT_EQ(vector[i], i);
    EXPECT_EQ(vector.at(i), i);
  }
}

TYPED_TEST(vector, unary_plus) {
  using vector_type =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  const vector_type vector(typename TypeParam::type(1));
  const vector_type vector1(+vector);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(1));
    EXPECT_EQ(vector1[i], typename TypeParam::type(1));
  }
}

TYPED_TEST(vector, unary_minus) {
  using vector_type =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  const vector_type vector(typename TypeParam::type(2));
  const vector_type vector1(-vector);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(2));
    EXPECT_EQ(vector1[i], typename TypeParam::type(-2));
  }
}

TYPED_TEST(vector, multiply) {
  using vector_type =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  vector_type vector3(typename TypeParam::type(1));

  vector3 *= typename TypeParam::type(2);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector3[i], typename TypeParam::type(2));
  }

  const vector_type vector4(vector3 * typename TypeParam::type(2));

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector3[i], typename TypeParam::type(2));
    EXPECT_EQ(vector4[i], typename TypeParam::type(4));
  }

  const vector_type vector5(typename TypeParam::type(2) * vector4);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector4[i], typename TypeParam::type(4));
    EXPECT_EQ(vector5[i], typename TypeParam::type(8));
  }
}

TYPED_TEST(vector, divide) {
  using vector_type =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  vector_type vector3(typename TypeParam::type(12));

  vector3 /= typename TypeParam::type(2);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector3[i], typename TypeParam::type(6));
  }

  const vector_type vector4(vector3 / typename TypeParam::type(2));

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector3[i], typename TypeParam::type(6));
    EXPECT_EQ(vector4[i], typename TypeParam::type(3));
  }
}

TYPED_TEST(vector, equality) {
  using vector_type =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  const vector_type vector(typename TypeParam::type(1));
  const vector_type vector1(vector);

  EXPECT_EQ(vector, vector1);
  EXPECT_TRUE(vector == vector1);
  EXPECT_FALSE(vector != vector1);
}

TYPED_TEST(vector, inequality) {
  using vector_type =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  const vector_type vector(typename TypeParam::type(1));
  const vector_type vector1(typename TypeParam::type(2));

  EXPECT_NE(vector, vector1);
  EXPECT_FALSE(vector == vector1);
  EXPECT_TRUE(vector != vector1);
}

TYPED_TEST(vector, scale) {
  using vector_type =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  vector_type vector2(typename TypeParam::type(1));

  vector2.scale(typename TypeParam::type(2));

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector2[i], typename TypeParam::type(2));
  }
}

TYPED_TEST(vector, reverse) {
  using vector_type =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  vector_type vector;

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

TYPED_TEST(vector, swap) {
  using vector_type =
      wn::math::vector<typename TypeParam::type, TypeParam::dimension>;

  vector_type vector(typename TypeParam::type(4));
  vector_type vector1(typename TypeParam::type(3));

  vector.swap(vector1);

  for (size_t i = 0; i < TypeParam::dimension; ++i) {
    EXPECT_EQ(vector[i], typename TypeParam::type(3));
    EXPECT_EQ(vector1[i], typename TypeParam::type(4));
  }
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
