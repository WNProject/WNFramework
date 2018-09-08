// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "executable_test/inc/WNTestHarness.h"
#include "WNFunctional/inc/WNFunction.h"

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

TEST(function, construct) {
  wn::testing::allocator allocator;

  {
    // default
    const wn::functional::function<void()> function1;
    const wn::functional::function<void(int32_t)> function2;
    const wn::functional::function<void(int32_t, int32_t)> function3;
    const wn::functional::function<int32_t()> function4;
    const wn::functional::function<int32_t(int32_t)> function5;
    const wn::functional::function<int32_t(int32_t, int32_t)> function6;

    EXPECT_FALSE(function1);
    EXPECT_FALSE(function2);
    EXPECT_FALSE(function3);
    EXPECT_FALSE(function4);
    EXPECT_FALSE(function5);
    EXPECT_FALSE(function6);

    // lambda
    const wn::functional::function<void()> function7(
        &allocator, []() -> void {});
    const wn::functional::function<void(int32_t)> function8(
        &allocator, [](int32_t) -> void {});
    const wn::functional::function<void(int32_t, int32_t)> function9(
        &allocator, [](int32_t, int32_t) -> void {});
    const wn::functional::function<int32_t()> function10(
        &allocator, []() -> int32_t { return 1; });
    const wn::functional::function<int32_t(int32_t)> function11(
        &allocator, [](int32_t) -> int32_t { return 1; });
    const wn::functional::function<int32_t(int32_t, int32_t)> function12(
        &allocator, [](int32_t, int32_t) -> int32_t { return 1; });

    EXPECT_TRUE(function7);
    EXPECT_TRUE(function8);
    EXPECT_TRUE(function9);
    EXPECT_TRUE(function10);
    EXPECT_TRUE(function11);
    EXPECT_TRUE(function12);

    // lambda with capture
    const int32_t value = 1;
    const wn::functional::function<void()> function13(
        &allocator, [value]() -> void { (void)value; });
    const wn::functional::function<void(int32_t)> function14(
        &allocator, [value](int32_t) -> void { (void)value; });
    const wn::functional::function<void(int32_t, int32_t)> function15(
        &allocator, [value](int32_t, int32_t) -> void { (void)value; });
    const wn::functional::function<int32_t()> function16(
        &allocator, [value]() -> int32_t { return value; });
    const wn::functional::function<int32_t(int32_t)> function17(
        &allocator, [value](int32_t) -> int32_t { return value; });
    const wn::functional::function<int32_t(int32_t, int32_t)> function18(
        &allocator, [value](int32_t, int32_t) -> int32_t { return value; });

    EXPECT_TRUE(function13);
    EXPECT_TRUE(function14);
    EXPECT_TRUE(function15);
    EXPECT_TRUE(function16);
    EXPECT_TRUE(function17);
    EXPECT_TRUE(function18);

    // functions
    const wn::functional::function<void()> function19(
        &allocator, &test_function1);
    const wn::functional::function<void(int32_t)> function20(
        &allocator, &test_function2);
    const wn::functional::function<void(int32_t, int32_t)> function21(
        &allocator, &test_function3);
    const wn::functional::function<int32_t()> function22(
        &allocator, &test_function4);
    const wn::functional::function<int32_t(int32_t)> function23(
        &allocator, &test_function5);
    const wn::functional::function<int32_t(int32_t, int32_t)> function24(
        &allocator, &test_function6);

    EXPECT_TRUE(function19);
    EXPECT_TRUE(function20);
    EXPECT_TRUE(function21);
    EXPECT_TRUE(function22);
    EXPECT_TRUE(function23);
    EXPECT_TRUE(function24);
  }
}

TEST(function, move) {
  wn::testing::allocator allocator;

  {
    int32_t value = 1;
    wn::functional::function<int32_t()> function1(
        &allocator, [value]() -> int32_t { return value; });

    EXPECT_TRUE(function1);
    EXPECT_EQ(function1(), 1);

    wn::functional::function<int32_t()> function2(wn::core::move(function1));

    EXPECT_FALSE(function1);
    EXPECT_TRUE(function2);
    EXPECT_EQ(function2(), 1);

    function1 = wn::core::move(function2);

    EXPECT_TRUE(function1);
    EXPECT_FALSE(function2);
    EXPECT_EQ(function1(), 1);
  }
}

TEST(function, copy) {
  wn::testing::allocator allocator1;
  wn::testing::allocator allocator2;

  {
    int32_t value = 1;
    const wn::functional::function<int32_t()> function1(
        &allocator1, [value]() -> int32_t { return value; });

    EXPECT_TRUE(function1);
    EXPECT_EQ(function1(), 1);

    const wn::functional::function<int32_t()> function2(function1);

    EXPECT_TRUE(function1);
    EXPECT_TRUE(function2);
    EXPECT_EQ(function1(), 1);
    EXPECT_EQ(function2(), 1);

    wn::functional::function<int32_t()> function3;

    EXPECT_FALSE(function3);

    function3 = function2;

    EXPECT_TRUE(function1);
    EXPECT_TRUE(function2);
    EXPECT_TRUE(function3);
    EXPECT_EQ(function1(), 1);
    EXPECT_EQ(function2(), 1);
    EXPECT_EQ(function3(), 1);

    const wn::functional::function<int32_t()> function4(&allocator2, function1);

    EXPECT_TRUE(function1);
    EXPECT_TRUE(function2);
    EXPECT_TRUE(function3);
    EXPECT_TRUE(function4);
    EXPECT_EQ(function1(), 1);
    EXPECT_EQ(function2(), 1);
    EXPECT_EQ(function3(), 1);
    EXPECT_EQ(function4(), 1);
  }
}

TEST(function, reset) {
  wn::testing::allocator allocator;

  {
    wn::functional::function<int32_t()> function1;

    EXPECT_FALSE(function1);

    int32_t value = 1;

    function1.reset(&allocator, [value]() -> int32_t { return value; });

    EXPECT_TRUE(function1);
    EXPECT_EQ(function1(), 1);
  }
}

TEST(function, swap) {
  wn::testing::allocator allocator;

  {
    int32_t value = 1;
    wn::functional::function<int32_t()> function1(
        &allocator, [value]() -> int32_t { return value; });
    wn::functional::function<int32_t()> function2(
        &allocator, [value]() -> int32_t { return (value + 1); });

    EXPECT_TRUE(function1);
    EXPECT_TRUE(function2);
    EXPECT_EQ(function1.get_allocator(), function2.get_allocator());
    EXPECT_EQ(function1(), 1);
    EXPECT_EQ(function2(), 2);

    function1.swap(function2);

    EXPECT_TRUE(function1);
    EXPECT_TRUE(function2);
    EXPECT_EQ(function1.get_allocator(), function2.get_allocator());
    EXPECT_EQ(function1(), 2);
    EXPECT_EQ(function2(), 1);

    wn::functional::function<int32_t()> function3;

    EXPECT_FALSE(function3);
    EXPECT_EQ(function3.get_allocator(), nullptr);

    function1.swap(function3);

    EXPECT_FALSE(function1);
    EXPECT_EQ(function1.get_allocator(), nullptr);
    EXPECT_EQ(function3.get_allocator(), &allocator);
    EXPECT_TRUE(function3);
    EXPECT_EQ(function3(), 2);
  }
}

TEST(function, large_parameter_count) {
  wn::testing::allocator allocator;

  {
    wn::functional::function<int32_t(float, double, int8_t, int16_t, int32_t,
        int64_t, uint8_t, uint16_t, uint32_t, uint64_t, size_t, signed_t)>
        function1(&allocator);

    EXPECT_FALSE(function1);

    function1 = [](float a, double b, int8_t c, int16_t d, int32_t e, int64_t f,
                    uint8_t g, uint16_t h, uint32_t i, uint64_t j, size_t k,
                    signed_t l) -> int32_t {
      return static_cast<int32_t>(
          (a * b) + c - d + e - f + g - h + i - j + k - l);
    };

    EXPECT_TRUE(function1);
    EXPECT_EQ(function1(1.0f, 2.0, 3, 4, 5, 6, 7u, 8u, 9u, 10u, 11u, 12), -3);
  }
}

TEST(function, non_copyable_parameter_test) {
  class test final : wn::core::non_copyable {
  public:
    test(int32_t _value) : m_value(_value) {}

    test(test&& _other) : m_value(_other.m_value) {}

    int32_t value() const {
      return m_value;
    }

  private:
    int32_t m_value;
  };

  wn::testing::allocator allocator;

  {
    wn::functional::function<int32_t(test)> function1(
        &allocator, [](test _test) { return _test.value(); });

    EXPECT_EQ(function1(test(5)), 5);

    wn::functional::function<int32_t(test)> function2(function1);

    EXPECT_EQ(function2(test(6)), 6);

    const auto lambda = [&function1]() { return function1(test(9)); };

    EXPECT_EQ(lambda(), 9);
  }
}
