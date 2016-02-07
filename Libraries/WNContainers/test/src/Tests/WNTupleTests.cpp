// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNTuple.h"
#include "WNTesting/inc/WNTestHarness.h"

WN_ENABLE_TYPED_TEST(tuple);

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

template <typename T, typename U = T, typename V = U>
struct multi_type final {
  using first_type = T;
  using second_type = U;
  using third_type = V;
};

using tuple_testing_types =
    ::testing::Types<multi_type<uint8_t, int64_t, int16_t>,
        multi_type<uint16_t, dummy, float>, multi_type<uint32_t, float, int8_t>,
        multi_type<uint64_t, double, dummy>,
        multi_type<dummy, int8_t, uint32_t>>;

TYPED_TEST_CASE(tuple, tuple_testing_types);

TYPED_TEST(tuple, construction) {
  using first_type = typename TypeParam::first_type;
  using second_type = typename TypeParam::second_type;
  using third_type = typename TypeParam::third_type;

  const wn::containers::tuple<first_type, second_type, third_type> tuple1;

  WN_UNUSED_ARGUMENT(tuple1);

  const wn::containers::tuple<first_type, second_type, third_type> tuple2(
      first_type(1), second_type(2), third_type(3));

  EXPECT_EQ(wn::containers::get<0>(tuple2), first_type(1));
  EXPECT_EQ(wn::containers::get<1>(tuple2), second_type(2));
  EXPECT_EQ(wn::containers::get<2>(tuple2), third_type(3));

  wn::containers::tuple<first_type, second_type, third_type> tuple3(tuple2);

  EXPECT_EQ(wn::containers::get<0>(tuple3), first_type(1));
  EXPECT_EQ(wn::containers::get<1>(tuple3), second_type(2));
  EXPECT_EQ(wn::containers::get<2>(tuple3), third_type(3));

  wn::containers::tuple<first_type, second_type, third_type> tuple4(
      wn::core::move(tuple3));

  EXPECT_EQ(wn::containers::get<0>(tuple4), first_type(1));
  EXPECT_EQ(wn::containers::get<1>(tuple4), second_type(2));
  EXPECT_EQ(wn::containers::get<2>(tuple4), third_type(3));

  const wn::containers::tuple<first_type, first_type, first_type> tuple5(
      first_type(1));

  EXPECT_EQ(wn::containers::get<0>(tuple5), first_type(1));
  EXPECT_EQ(wn::containers::get<1>(tuple5), first_type(1));
  EXPECT_EQ(wn::containers::get<2>(tuple5), first_type(1));
}

TYPED_TEST(tuple, assignment) {
  using first_type = typename TypeParam::first_type;
  using second_type = typename TypeParam::second_type;
  using third_type = typename TypeParam::third_type;

  wn::containers::tuple<first_type, second_type, third_type> tuple1;
  const wn::containers::tuple<first_type, second_type, third_type> tuple2(
      first_type(1), second_type(2), third_type(3));

  tuple1 = tuple2;

  EXPECT_EQ(wn::containers::get<0>(tuple1), first_type(1));
  EXPECT_EQ(wn::containers::get<1>(tuple1), second_type(2));
  EXPECT_EQ(wn::containers::get<2>(tuple1), third_type(3));

  tuple1 =
      wn::containers::make_tuple(first_type(4), second_type(5), third_type(6));

  EXPECT_EQ(wn::containers::get<0>(tuple1), first_type(4));
  EXPECT_EQ(wn::containers::get<1>(tuple1), second_type(5));
  EXPECT_EQ(wn::containers::get<2>(tuple1), third_type(6));

  const wn::containers::tuple<uint8_t, uint8_t, uint8_t> tuple3(
      uint8_t(7), uint8_t(8), uint8_t(9));

  tuple1 = tuple3;

  EXPECT_EQ(wn::containers::get<0>(tuple1), first_type(7));
  EXPECT_EQ(wn::containers::get<1>(tuple1), second_type(8));
  EXPECT_EQ(wn::containers::get<2>(tuple1), third_type(9));

  tuple1 = wn::containers::make_tuple(uint8_t(10), uint8_t(11), uint8_t(12));

  EXPECT_EQ(wn::containers::get<0>(tuple1), first_type(10));
  EXPECT_EQ(wn::containers::get<1>(tuple1), second_type(11));
  EXPECT_EQ(wn::containers::get<2>(tuple1), third_type(12));
}

TYPED_TEST(tuple, type_access) {
  using first_type = typename TypeParam::first_type;
  using second_type = typename TypeParam::second_type;
  using third_type = typename TypeParam::third_type;

  wn::containers::tuple<first_type, second_type, third_type> tuple;

  EXPECT_TRUE(
      (wn::core::is_same<wn::containers::tuple_element_t<0, decltype(tuple)>,
          first_type>::value));
  EXPECT_TRUE(
      (wn::core::is_same<wn::containers::tuple_element_t<1, decltype(tuple)>,
          second_type>::value));
  EXPECT_TRUE(
      (wn::core::is_same<wn::containers::tuple_element_t<2, decltype(tuple)>,
          third_type>::value));
  EXPECT_TRUE(
      (wn::core::is_same<wn::containers::tuple_element_t<0, decltype(tuple)>,
          first_type>::value));
  EXPECT_TRUE(
      (wn::core::is_same<wn::containers::tuple_element_t<1, decltype(tuple)>,
          second_type>::value));
  EXPECT_TRUE(
      (wn::core::is_same<wn::containers::tuple_element_t<2, decltype(tuple)>,
          third_type>::value));
}

TYPED_TEST(tuple, element_access) {
  using first_type = typename TypeParam::first_type;
  using second_type = typename TypeParam::second_type;
  using third_type = typename TypeParam::third_type;

  const wn::containers::tuple<first_type, second_type, third_type> tuple(
      first_type(1), second_type(2), third_type(3));

  EXPECT_EQ(wn::containers::get<0>(tuple), first_type(1));
  EXPECT_EQ(wn::containers::get<1>(tuple), second_type(2));
  EXPECT_EQ(wn::containers::get<2>(tuple), third_type(3));
}

TYPED_TEST(tuple, size) {
  using first_type = typename TypeParam::first_type;
  using second_type = typename TypeParam::second_type;
  using third_type = typename TypeParam::third_type;

  wn::containers::tuple<first_type, second_type, third_type> tuple1;
  wn::containers::tuple<first_type, second_type, third_type, first_type,
      second_type, third_type>
      tuple2;

  EXPECT_EQ(wn::containers::tuple_size<decltype(tuple1)>::value, 3);
  EXPECT_EQ(wn::containers::tuple_size<decltype(tuple2)>::value, 6);
}

TYPED_TEST(tuple, swap) {
  using first_type = typename TypeParam::first_type;
  using second_type = typename TypeParam::second_type;
  using third_type = typename TypeParam::third_type;

  wn::containers::tuple<first_type, second_type, third_type> tuple1(
      first_type(1), second_type(2), third_type(3));
  wn::containers::tuple<first_type, second_type, third_type> tuple2(
      first_type(4), second_type(5), third_type(6));

  EXPECT_EQ(wn::containers::get<0>(tuple1), first_type(1));
  EXPECT_EQ(wn::containers::get<1>(tuple1), second_type(2));
  EXPECT_EQ(wn::containers::get<2>(tuple1), third_type(3));
  EXPECT_EQ(wn::containers::get<0>(tuple2), first_type(4));
  EXPECT_EQ(wn::containers::get<1>(tuple2), second_type(5));
  EXPECT_EQ(wn::containers::get<2>(tuple2), third_type(6));

  tuple1.swap(tuple2);

  EXPECT_EQ(wn::containers::get<0>(tuple2), first_type(1));
  EXPECT_EQ(wn::containers::get<1>(tuple2), second_type(2));
  EXPECT_EQ(wn::containers::get<2>(tuple2), third_type(3));
  EXPECT_EQ(wn::containers::get<0>(tuple1), first_type(4));
  EXPECT_EQ(wn::containers::get<1>(tuple1), second_type(5));
  EXPECT_EQ(wn::containers::get<2>(tuple1), third_type(6));
}