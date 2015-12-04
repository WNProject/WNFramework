// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNTesting/inc/WNTestHarness.h"
#include "WNContainers/inc/WNPair.h"

WN_ENABLE_TYPED_TEST(pair);

// 72 bit struct
struct dummy {
  WN_FORCE_INLINE dummy() : dummy(0) {}

  WN_FORCE_INLINE dummy(const wn_uint8 value)
    : m_value1(value), m_value2(value) {}

  wn_uint8 m_value1;
  wn_uint64 m_value2;
};

WN_FORCE_INLINE wn_bool operator==(const dummy& dummy1, const dummy& dummy2) {
  return dummy1.m_value1 == dummy2.m_value1 &&
         dummy1.m_value2 == dummy2.m_value2;
}

template <typename T, typename U = T>
struct multi_type final {
  using first_type = T;
  using second_type = U;
};

using pair_testing_types = ::testing::Types<multi_type<wn_uint8, wn_int64>,
    multi_type<wn_uint16, dummy>, multi_type<wn_uint32, wn_float32>,
    multi_type<wn_uint64, wn_float64>, multi_type<dummy, wn_int8>>;

TYPED_TEST_CASE(pair, pair_testing_types);

TYPED_TEST(pair, construction) {
  using first_type = typename TypeParam::first_type;
  using second_type = typename TypeParam::second_type;

  const wn::containers::pair<first_type, second_type> pair1;

  WN_UNUSED_ARGUMENT(pair1);

  const wn::containers::pair<first_type, second_type> pair2(
      first_type(1), second_type(2));

  EXPECT_EQ(pair2.first, first_type(1));
  EXPECT_EQ(pair2.second, second_type(2));

  wn::containers::pair<first_type, second_type> pair3(pair2);

  EXPECT_EQ(pair3.first, first_type(1));
  EXPECT_EQ(pair3.second, second_type(2));

  const wn::containers::pair<first_type, second_type> pair4(
      wn::core::move(pair3));

  EXPECT_EQ(pair4.first, first_type(1));
  EXPECT_EQ(pair4.second, second_type(2));

  const wn::containers::pair<first_type> pair5(first_type(1));

  EXPECT_EQ(pair5.first, first_type(1));
  EXPECT_EQ(pair5.second, first_type(1));

  const wn::containers::pair<first_type, first_type> pair6(first_type(1));

  EXPECT_EQ(pair6.first, first_type(1));
  EXPECT_EQ(pair6.second, first_type(1));
}

TYPED_TEST(pair, assignment) {
  using first_type = typename TypeParam::first_type;
  using second_type = typename TypeParam::second_type;

  wn::containers::pair<first_type, second_type> pair1;
  const wn::containers::pair<first_type, second_type> pair2(
      first_type(1), second_type(2));

  pair1 = pair2;

  EXPECT_EQ(pair1.first, pair2.first);
  EXPECT_EQ(pair1.second, pair2.second);

  pair1 = wn::containers::make_pair(first_type(3), second_type(4));

  EXPECT_EQ(pair1.first, first_type(3));
  EXPECT_EQ(pair1.second, second_type(4));

  const wn::containers::pair<wn_uint8, wn_uint8> pair3(
      wn_uint8(5), wn_uint8(6));

  pair1 = pair3;

  EXPECT_EQ(pair1.first, first_type(pair3.first));
  EXPECT_EQ(pair1.second, second_type(pair3.second));

  pair1 = wn::containers::make_pair(wn_uint8(7), wn_uint8(8));

  EXPECT_EQ(pair1.first, first_type(7));
  EXPECT_EQ(pair1.second, second_type(8));
}

TYPED_TEST(pair, type_access) {
  using first_type = typename TypeParam::first_type;
  using second_type = typename TypeParam::second_type;

  EXPECT_TRUE(
      (wn::core::is_same<first_type, typename wn::containers::pair<first_type,
                                         second_type>::first_type>::value));
  EXPECT_TRUE(
      (wn::core::is_same<second_type, typename wn::containers::pair<first_type,
                                          second_type>::second_type>::value));
}

TYPED_TEST(pair, element_access) {
  using first_type = typename TypeParam::first_type;
  using second_type = typename TypeParam::second_type;

  const wn::containers::pair<first_type, second_type> pair(
      first_type(1), second_type(2));

  EXPECT_EQ(pair.first, first_type(1));
  EXPECT_EQ(pair.second, second_type(2));
}

TYPED_TEST(pair, swap) {
  using first_type = typename TypeParam::first_type;
  using second_type = typename TypeParam::second_type;

  wn::containers::pair<first_type, second_type> pair1(
      first_type(1), second_type(2));
  wn::containers::pair<first_type, second_type> pair2(
      first_type(3), second_type(4));

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