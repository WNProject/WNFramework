// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/test/inc/Common.h"
#include "WNContainers/inc/WNFunction.h"

wn_void test_function1() {}

wn_void test_function2(wn_int32) {}

wn_void test_function3(wn_int32, wn_int32) {}

wn_int32 test_function4() {
  return(1);
}

wn_int32 test_function5(wn_int32) {
  return(1);
}

wn_int32 test_function6(wn_int32, wn_int32) {
  return(1);
}

struct test_object final {
  wn_void test_member_function1() {}

  wn_void test_member_function2(wn_int32) {}

  wn_void test_member_function3(wn_int32, wn_int32) {}

  wn_int32 test_member_function4() {
    return(m_value);
  }

  wn_int32 test_member_function5(wn_int32) {
    return(m_value);
  }

  wn_int32 test_member_function6(wn_int32, wn_int32) {
    return(m_value);
  }

  wn_int32 m_value = 1;
};

TEST(function, construct) {
  // default
  const wn::containers::function<wn_void()> function1;
  const wn::containers::function<wn_void(wn_int32)> function2;
  const wn::containers::function<wn_void(wn_int32, wn_int32)> function3;
  const wn::containers::function<wn_int32()> function4;
  const wn::containers::function<wn_int32(wn_int32)> function5;
  const wn::containers::function<wn_int32(wn_int32, wn_int32)> function6;

  EXPECT_FALSE(function1);
  EXPECT_FALSE(function2);
  EXPECT_FALSE(function3);
  EXPECT_FALSE(function4);
  EXPECT_FALSE(function5);
  EXPECT_FALSE(function6);

  // lambda
  const wn::containers::function<wn_void()> function7([]() -> wn_void {});
  const wn::containers::function<wn_void(wn_int32)> function8(
      [](wn_int32) -> wn_void {});
  const wn::containers::function<wn_void(wn_int32, wn_int32)> function9(
      [](wn_int32, wn_int32) -> wn_void {});
  const wn::containers::function<wn_int32()> function10(
      []() -> wn_int32 { return(1); });
  const wn::containers::function<wn_int32(wn_int32)> function11(
      [](wn_int32) -> wn_int32 { return(1); });
  const wn::containers::function<wn_int32(wn_int32, wn_int32)> function12(
      [](wn_int32, wn_int32) -> wn_int32 { return(1); });

  EXPECT_TRUE(function7);
  EXPECT_TRUE(function8);
  EXPECT_TRUE(function9);
  EXPECT_TRUE(function10);
  EXPECT_TRUE(function11);
  EXPECT_TRUE(function12);

  // lambda with capture
  const wn_int32 value = 1;
  const wn::containers::function<wn_void()> function13([value]() -> wn_void {});
  const wn::containers::function<wn_void(wn_int32)> function14(
      [value](wn_int32) -> wn_void {});
  const wn::containers::function<wn_void(wn_int32, wn_int32)> function15(
      [value](wn_int32, wn_int32) -> wn_void {});
  const wn::containers::function<wn_int32()> function16(
      [value]() -> wn_int32 { return(1); });
  const wn::containers::function<wn_int32(wn_int32)> function17(
      [value](wn_int32) -> wn_int32 { return(1); });
  const wn::containers::function<wn_int32(wn_int32, wn_int32)> function18(
      [value](wn_int32, wn_int32) -> wn_int32 { return(1); });

  EXPECT_TRUE(function13);
  EXPECT_TRUE(function14);
  EXPECT_TRUE(function15);
  EXPECT_TRUE(function16);
  EXPECT_TRUE(function17);
  EXPECT_TRUE(function18);

  // functions
  const wn::containers::function<wn_void()> function19(&test_function1);
  const wn::containers::function<wn_void(wn_int32)> function20(&test_function2);
  const wn::containers::function<wn_void(wn_int32, wn_int32)> function21(
      &test_function3);
  const wn::containers::function<wn_int32()> function22(&test_function4);
  const wn::containers::function<wn_int32(wn_int32)> function23(
      &test_function5);
  const wn::containers::function<wn_int32(wn_int32, wn_int32)> function24(
      &test_function6);

  EXPECT_TRUE(function19);
  EXPECT_TRUE(function20);
  EXPECT_TRUE(function21);
  EXPECT_TRUE(function22);
  EXPECT_TRUE(function23);
  EXPECT_TRUE(function24);

  // member functions
  test_object object;
  const wn::containers::function<wn_void()> function25(
      &test_object::test_member_function1, &object);
  const wn::containers::function<wn_void(wn_int32)> function26(
      &test_object::test_member_function2, &object);
  const wn::containers::function<wn_void(wn_int32, wn_int32)> function27(
      &test_object::test_member_function3, &object);
  const wn::containers::function<wn_int32()> function28(
      &test_object::test_member_function4, &object);
  const wn::containers::function<wn_int32(wn_int32)> function29(
      &test_object::test_member_function5, &object);
  const wn::containers::function<wn_int32(wn_int32, wn_int32)> function30(
      &test_object::test_member_function6, &object);

  EXPECT_TRUE(function25);
  EXPECT_TRUE(function26);
  EXPECT_TRUE(function27);
  EXPECT_TRUE(function28);
  EXPECT_TRUE(function29);
  EXPECT_TRUE(function30);
}

TEST(function, move) {
  const wn_int32 value = 1;
  wn::containers::function<wn_int32()> function1(
      [value]() -> wn_int32 { return(value); });

  EXPECT_TRUE(function1);
  EXPECT_EQ(function1(), 1);

  wn::containers::function<wn_int32()> function2(std::move(function1));

  EXPECT_FALSE(function1);
  EXPECT_TRUE(function2);
  EXPECT_EQ(function2(), 1);

  function1 = std::move(function2);

  EXPECT_TRUE(function1);
  EXPECT_FALSE(function2);
  EXPECT_EQ(function1(), 1);
}

TEST(function, copy) {
  const wn_int32 value = 1;
  const wn::containers::function<wn_int32()> function1(
      [value]() -> wn_int32 { return(value); });

  EXPECT_TRUE(function1);
  EXPECT_EQ(function1(), 1);

  const wn::containers::function<wn_int32()> function2(function1);

  EXPECT_TRUE(function1);
  EXPECT_TRUE(function2);
  EXPECT_EQ(function2(), 1);

  wn::containers::function<wn_int32()> function3;

  EXPECT_FALSE(function3);

  function3 = function2;

  EXPECT_TRUE(function1);
  EXPECT_TRUE(function2);
  EXPECT_TRUE(function3);
  EXPECT_EQ(function3(), 1);
}

TEST(function, assign) {
  wn::containers::function<wn_int32()> function1;

  EXPECT_FALSE(function1);

  const wn_int32 value = 1;

  function1.assign([value]() -> wn_int32 { return(value); });

  EXPECT_TRUE(function1);
  EXPECT_EQ(function1(), 1);
}

TEST(function, swap) {
  const wn_int32 value = 1;
  wn::containers::function<wn_int32()> function1(
      [value]() -> wn_int32 { return(value); });
  wn::containers::function<wn_int32()> function2(
      [value]() -> wn_int32 { return(value + 1); });

  EXPECT_TRUE(function1);
  EXPECT_TRUE(function2);
  EXPECT_EQ(function1(), 1);
  EXPECT_EQ(function2(), 2);

  function1.swap(function2);

  EXPECT_TRUE(function1);
  EXPECT_TRUE(function2);
  EXPECT_EQ(function1(), 2);
  EXPECT_EQ(function2(), 1);

  wn::containers::function<wn_int32()> function3;

  EXPECT_FALSE(function3);

  function1.swap(function3);

  EXPECT_FALSE(function1);
  EXPECT_TRUE(function3);
  EXPECT_EQ(function3(), 2);
}

TEST(function, large_parameter_count) {
  wn::containers::function<wn_int32(wn_float32, wn_float64, wn_int8, wn_int16,
                                    wn_int32, wn_int64, wn_uint8, wn_uint16,
                                    wn_uint32, wn_uint64, wn_size_t,
                                    wn_signed_t)> function1;

  EXPECT_FALSE(function1);

  function1 = [](wn_float32 a, wn_float64 b, wn_int8 c, wn_int16 d, wn_int32 e,
                 wn_int64 f, wn_uint8 g, wn_uint16 h, wn_uint32 i, wn_uint64 j,
                 wn_size_t k, wn_signed_t l) -> wn_int32 {
    return(static_cast<wn_int32>(a * b + c - d + e - f + g - h + i - j + k - l));
  };

  EXPECT_TRUE(function1);
  EXPECT_EQ(function1(1.0f, 2.0, 3, 4, 5, 6, 7u, 8u, 9u, 10u, 11u, 12), -3);
}