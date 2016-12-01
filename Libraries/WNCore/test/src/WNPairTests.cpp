// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNCore/inc/WNPair.h"
#include "WNTesting/inc/WNTestHarness.h"

WN_ENABLE_TYPED_TEST(pair);

// 72 bit struct
struct dummy {
  WN_FORCE_INLINE dummy() : dummy(0) {}

  WN_FORCE_INLINE dummy(const uint8_t value)
    : m_value1(value), m_value2(value) {}

  uint8_t m_value1;
  uint64_t m_value2;
};

WN_FORCE_INLINE bool operator==(const dummy& dummy1, const dummy& dummy2) {
  return dummy1.m_value1 == dummy2.m_value1 &&
         dummy1.m_value2 == dummy2.m_value2;
}

template <typename T, typename U = T>
struct multi_type final {
  using first_type = T;
  using second_type = U;
};

using pair_testing_types = ::testing::Types<multi_type<uint8_t, int64_t>,
    multi_type<uint16_t, dummy>, multi_type<uint32_t, float>,
    multi_type<uint64_t, double>, multi_type<dummy, int8_t>>;

TYPED_TEST_CASE(pair, pair_testing_types);

TYPED_TEST(pair, construction) {
  using first_type = typename TypeParam::first_type;
  using second_type = typename TypeParam::second_type;

  const wn::core::pair<first_type, second_type> pair1;

  WN_UNUSED_ARGUMENT(pair1);

  const wn::core::pair<first_type, second_type> pair2(
      first_type(1), second_type(2));

  EXPECT_EQ(pair2.first, first_type(1));
  EXPECT_EQ(pair2.second, second_type(2));

  wn::core::pair<first_type, second_type> pair3(pair2);

  EXPECT_EQ(pair3.first, first_type(1));
  EXPECT_EQ(pair3.second, second_type(2));

  const wn::core::pair<first_type, second_type> pair4(wn::core::move(pair3));

  EXPECT_EQ(pair4.first, first_type(1));
  EXPECT_EQ(pair4.second, second_type(2));

  const wn::core::pair<first_type> pair5(first_type(1));

  EXPECT_EQ(pair5.first, first_type(1));
  EXPECT_EQ(pair5.second, first_type(1));

  const wn::core::pair<first_type, first_type> pair6(first_type(1));

  EXPECT_EQ(pair6.first, first_type(1));
  EXPECT_EQ(pair6.second, first_type(1));
}

TYPED_TEST(pair, assignment) {
  using first_type = typename TypeParam::first_type;
  using second_type = typename TypeParam::second_type;

  wn::core::pair<first_type, second_type> pair1;
  const wn::core::pair<first_type, second_type> pair2(
      first_type(1), second_type(2));

  pair1 = pair2;

  EXPECT_EQ(pair1.first, pair2.first);
  EXPECT_EQ(pair1.second, pair2.second);

  pair1 = wn::core::make_pair(first_type(3), second_type(4));

  EXPECT_EQ(pair1.first, first_type(3));
  EXPECT_EQ(pair1.second, second_type(4));

  const wn::core::pair<uint8_t, uint8_t> pair3(uint8_t(5), uint8_t(6));

  pair1 = pair3;

  EXPECT_EQ(pair1.first, first_type(pair3.first));
  EXPECT_EQ(pair1.second, second_type(pair3.second));

  pair1 = wn::core::make_pair(uint8_t(7), uint8_t(8));

  EXPECT_EQ(pair1.first, first_type(7));
  EXPECT_EQ(pair1.second, second_type(8));
}

TYPED_TEST(pair, type_access) {
  using first_type = typename TypeParam::first_type;
  using second_type = typename TypeParam::second_type;

  EXPECT_TRUE((wn::core::is_same<first_type,
      typename wn::core::pair<first_type, second_type>::first_type>::value));
  EXPECT_TRUE((wn::core::is_same<second_type,
      typename wn::core::pair<first_type, second_type>::second_type>::value));
}

TYPED_TEST(pair, element_access) {
  using first_type = typename TypeParam::first_type;
  using second_type = typename TypeParam::second_type;

  const wn::core::pair<first_type, second_type> pair(
      first_type(1), second_type(2));

  EXPECT_EQ(pair.first, first_type(1));
  EXPECT_EQ(pair.second, second_type(2));
}

TYPED_TEST(pair, swap) {
  using first_type = typename TypeParam::first_type;
  using second_type = typename TypeParam::second_type;

  wn::core::pair<first_type, second_type> pair1(first_type(1), second_type(2));
  wn::core::pair<first_type, second_type> pair2(first_type(3), second_type(4));

  EXPECT_EQ(pair1.first, first_type(1));
  EXPECT_EQ(pair1.second, second_type(2));
  EXPECT_EQ(pair2.first, first_type(3));
  EXPECT_EQ(pair2.second, second_type(4));

  pair1.swap(pair2);

  EXPECT_EQ(pair2.first, first_type(1));
  EXPECT_EQ(pair2.second, second_type(2));
  EXPECT_EQ(pair1.first, first_type(3));
  EXPECT_EQ(pair1.second, second_type(4));
}