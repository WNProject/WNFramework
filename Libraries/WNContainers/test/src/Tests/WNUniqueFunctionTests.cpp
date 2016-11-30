// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNFunction.h"

#include "WNContainers/inc/WNUniqueFunction.h"
#include "WNMemory/inc/WNUniquePtr.h"
#include "WNTesting/inc/WNTestHarness.h"

void test_function1() {}

void test_function2(int32_t) {}

void test_function3(int32_t, int32_t) {}

int32_t test_function4() {
  return (1);
}

int32_t test_function5(int32_t) {
  return (1);
}

int32_t test_function6(int32_t, int32_t) {
  return (1);
}

struct test_object final {
  void test_member_function1() {}

  void test_member_function2(int32_t) {}

  void test_member_function3(int32_t, int32_t) {}

  int32_t test_member_function4() {
    return (m_value);
  }

  int32_t test_member_function5(int32_t) {
    return (m_value);
  }

  int32_t test_member_function6(int32_t, int32_t) {
    return (m_value);
  }

  int32_t m_value = 1;
};

TEST(unique_function, construct) {
  // default
  const wn::containers::unique_function<void()> function1;
  const wn::containers::unique_function<void(int32_t)> function2;
  const wn::containers::unique_function<void(int32_t, int32_t)> function3;
  const wn::containers::unique_function<int32_t()> function4;
  const wn::containers::unique_function<int32_t(int32_t)> function5;
  const wn::containers::unique_function<int32_t(int32_t, int32_t)> function6;

  EXPECT_FALSE(function1);
  EXPECT_FALSE(function2);
  EXPECT_FALSE(function3);
  EXPECT_FALSE(function4);
  EXPECT_FALSE(function5);
  EXPECT_FALSE(function6);

  // lambda
  const wn::containers::unique_function<void()> function7([]() -> void {});
  const wn::containers::unique_function<void(int32_t)> function8(
      [](int32_t) -> void {});
  const wn::containers::unique_function<void(int32_t, int32_t)> function9(
      [](int32_t, int32_t) -> void {});
  const wn::containers::unique_function<int32_t()> function10(
      []() -> int32_t { return (1); });
  const wn::containers::unique_function<int32_t(int32_t)> function11(
      [](int32_t) -> int32_t { return (1); });
  const wn::containers::unique_function<int32_t(int32_t, int32_t)> function12(
      [](int32_t, int32_t) -> int32_t { return (1); });

  EXPECT_TRUE(function7);
  EXPECT_TRUE(function8);
  EXPECT_TRUE(function9);
  EXPECT_TRUE(function10);
  EXPECT_TRUE(function11);
  EXPECT_TRUE(function12);

  // lambda with capture
  const int32_t value = 1;
  const wn::containers::unique_function<void()> function13(
      [value]() -> void {});
  const wn::containers::unique_function<void(int32_t)> function14(
      [value](int32_t) -> void {});
  const wn::containers::unique_function<void(int32_t, int32_t)> function15(
      [value](int32_t, int32_t) -> void {});
  const wn::containers::unique_function<int32_t()> function16(
      [value]() -> int32_t { return (1); });
  const wn::containers::unique_function<int32_t(int32_t)> function17(
      [value](int32_t) -> int32_t { return (1); });
  const wn::containers::unique_function<int32_t(int32_t, int32_t)> function18(
      [value](int32_t, int32_t) -> int32_t { return (1); });

  EXPECT_TRUE(function13);
  EXPECT_TRUE(function14);
  EXPECT_TRUE(function15);
  EXPECT_TRUE(function16);
  EXPECT_TRUE(function17);
  EXPECT_TRUE(function18);

  // functions
  const wn::containers::unique_function<void()> function19(&test_function1);
  const wn::containers::unique_function<void(int32_t)> function20(
      &test_function2);
  const wn::containers::unique_function<void(int32_t, int32_t)> function21(
      &test_function3);
  const wn::containers::unique_function<int32_t()> function22(&test_function4);
  const wn::containers::unique_function<int32_t(int32_t)> function23(
      &test_function5);
  const wn::containers::unique_function<int32_t(int32_t, int32_t)> function24(
      &test_function6);

  EXPECT_TRUE(function19);
  EXPECT_TRUE(function20);
  EXPECT_TRUE(function21);
  EXPECT_TRUE(function22);
  EXPECT_TRUE(function23);
  EXPECT_TRUE(function24);

  // member functions
  test_object object;
  const wn::containers::unique_function<void()> function25(
      &test_object::test_member_function1, &object);
  const wn::containers::unique_function<void(int32_t)> function26(
      &test_object::test_member_function2, &object);
  const wn::containers::unique_function<void(int32_t, int32_t)> function27(
      &test_object::test_member_function3, &object);
  const wn::containers::unique_function<int32_t()> function28(
      &test_object::test_member_function4, &object);
  const wn::containers::unique_function<int32_t(int32_t)> function29(
      &test_object::test_member_function5, &object);
  const wn::containers::unique_function<int32_t(int32_t, int32_t)> function30(
      &test_object::test_member_function6, &object);

  EXPECT_TRUE(function25);
  EXPECT_TRUE(function26);
  EXPECT_TRUE(function27);
  EXPECT_TRUE(function28);
  EXPECT_TRUE(function29);
  EXPECT_TRUE(function30);
}

TEST(unique_function, move) {
  const int32_t value = 1;
  wn::containers::unique_function<int32_t()> function1(
      [value]() -> int32_t { return (value); });

  EXPECT_TRUE(function1);
  EXPECT_EQ(function1(), 1);

  wn::containers::unique_function<int32_t()> function2(std::move(function1));

  EXPECT_FALSE(function1);
  EXPECT_TRUE(function2);
  EXPECT_EQ(function2(), 1);

  function1 = std::move(function2);

  EXPECT_TRUE(function1);
  EXPECT_FALSE(function2);
  EXPECT_EQ(function1(), 1);

  wn::testing::allocator allocator;

  {
    wn::memory::unique_ptr<int32_t> int_ptr(
        wn::memory::make_unique<int32_t>(&allocator, 5));
    wn::containers::unique_function<int32_t()> function3(std::bind(
        [&](wn::memory::unique_ptr<int32_t>& _ptr) -> int32_t {
          return *(_ptr.get());
        },
        std::move(int_ptr)));

    EXPECT_EQ(function3(), 5);
  }
}

TEST(unique_function, assign) {
  wn::containers::unique_function<int32_t()> function1;

  EXPECT_FALSE(function1);

  const int32_t value = 1;

  function1.assign([value]() -> int32_t { return (value); });

  EXPECT_TRUE(function1);
  EXPECT_EQ(function1(), 1);
}

TEST(unique_function, swap) {
  const int32_t value = 1;
  wn::containers::unique_function<int32_t()> function1(
      [value]() -> int32_t { return (value); });
  wn::containers::unique_function<int32_t()> function2(
      [value]() -> int32_t { return (value + 1); });

  EXPECT_TRUE(function1);
  EXPECT_TRUE(function2);
  EXPECT_EQ(function1(), 1);
  EXPECT_EQ(function2(), 2);

  function1.swap(function2);

  EXPECT_TRUE(function1);
  EXPECT_TRUE(function2);
  EXPECT_EQ(function1(), 2);
  EXPECT_EQ(function2(), 1);

  wn::containers::unique_function<int32_t()> function3;

  EXPECT_FALSE(function3);

  function1.swap(function3);

  EXPECT_FALSE(function1);
  EXPECT_TRUE(function3);
  EXPECT_EQ(function3(), 2);
}

TEST(unique_function, large_parameter_count) {
  wn::containers::unique_function<int32_t(float, double, int8_t, int16_t,
      int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t, size_t,
      signed_t)>
      function1;

  EXPECT_FALSE(function1);

  function1 = [](float a, double b, int8_t c, int16_t d, int32_t e, int64_t f,
      uint8_t g, uint16_t h, uint32_t i, uint64_t j, size_t k,
      signed_t l) -> int32_t {
    return (
        static_cast<int32_t>(a * b + c - d + e - f + g - h + i - j + k - l));
  };

  EXPECT_TRUE(function1);
  EXPECT_EQ(function1(1.0f, 2.0, 3, 4, 5, 6, 7u, 8u, 9u, 10u, 11u, 12), -3);
}