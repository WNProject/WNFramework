// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNMultiTasking/inc/WNThread.h"
#include "WNTesting/inc/WNTestHarness.h"

static void test_function1() {}

static uint32_t test_function2() {
  return (1);
}

static void test_function3(void* _input) {
  WN_UNUSED_ARGUMENT(_input);
}

static void* test_function4(void* _input) {
  return (_input);
}

static void test_function5(uint32_t _input) {
  WN_UNUSED_ARGUMENT(_input);
}

static uint32_t test_function6(uint32_t _input) {
  return (_input);
}

static uint32_t test_function7(uint32_t* _input) {
  return (*_input);
}

static uint32_t* test_function8(uint32_t* _input) {
  return (_input);
}

static void test_function9(uint32_t& _input) {
  _input = 1;
}

class test_object {
public:
  test_object() : mTestVar(0) {}

  void test_function1() {
    mTestVar++;
  }

  uint32_t test_function2() {
    return (mTestVar + 1);
  }

public:
  uint32_t mTestVar;
};

TEST(thread, basic_usage) {
  wn::testing::allocator allocator;

  {
    void* temp;
    void* pointer_test = &temp;
    uint32_t number_test1 = 5;
    uint32_t number_test2 = 0;

    wn::multi_tasking::thread<void> test1(&allocator, &test_function1);
    wn::multi_tasking::thread<uint32_t> test2(&allocator, &test_function2);
    wn::multi_tasking::thread<void> test3(
        &allocator, &test_function3, pointer_test);
    wn::multi_tasking::thread<void*> test4(
        &allocator, &test_function4, pointer_test);
    wn::multi_tasking::thread<void> test5(
        &allocator, &test_function5, number_test1);
    wn::multi_tasking::thread<uint32_t> test6(
        &allocator, &test_function6, number_test1);
    wn::multi_tasking::thread<uint32_t> test7(
        &allocator, &test_function7, &number_test1);
    wn::multi_tasking::thread<uint32_t*> test8(
        &allocator, &test_function8, &number_test1);
    wn::multi_tasking::thread<void> test9(
        &allocator, &test_function9, std::ref(number_test2));

    EXPECT_TRUE(test1.join());
    EXPECT_TRUE(test3.join());
    EXPECT_TRUE(test5.join());
    EXPECT_TRUE(test9.join());

    uint32_t test2_result;
    void* test4_result;
    uint32_t test6_result;
    uint32_t test7_result;
    uint32_t* test8_result;

    EXPECT_TRUE(test2.join(test2_result));
    EXPECT_TRUE(test4.join(test4_result));
    EXPECT_TRUE(test6.join(test6_result));
    EXPECT_TRUE(test7.join(test7_result));
    EXPECT_TRUE(test8.join(test8_result));
    EXPECT_EQ(test2_result, 1);
    EXPECT_EQ(test4_result, pointer_test);
    EXPECT_EQ(test6_result, number_test1);
    EXPECT_EQ(test7_result, number_test1);
    EXPECT_EQ(test8_result, &number_test1);
    EXPECT_EQ(number_test2, 1);

    test_object test_object1;
    test_object test_object2;

    wn::multi_tasking::thread<void> test10(
        &allocator, &test_object::test_function1, &test_object1);
    wn::multi_tasking::thread<uint32_t> test11(
        &allocator, &test_object::test_function2, &test_object2);

    EXPECT_TRUE(test10.join());

    uint32_t test11_result;

    EXPECT_TRUE(test11.join(test11_result));
    EXPECT_EQ(test_object1.mTestVar, 1);
    EXPECT_EQ(test_object2.mTestVar, 0);
    EXPECT_EQ(test11_result, 1);
  }
}

TEST(thread, joinable) {
  wn::testing::allocator allocator;

  {
    wn::multi_tasking::thread<void> thread;

    ASSERT_FALSE(thread.joinable());

    bool executed = false;

    thread = wn::multi_tasking::thread<void>(
        &allocator, [](bool& flag) { flag = true; }, std::ref(executed));

    ASSERT_TRUE(thread.joinable());
    ASSERT_TRUE(thread.join());
    ASSERT_TRUE(executed);
    ASSERT_FALSE(thread.joinable());
  }
}

TEST(thread, detach) {
  wn::testing::allocator allocator;

  {
    wn::multi_tasking::thread<void> thread;

    ASSERT_FALSE(thread.joinable());

    bool executed = false;

    thread = wn::multi_tasking::thread<void>(
        &allocator, [](bool& flag) { flag = true; }, std::ref(executed));

    ASSERT_TRUE(thread.joinable());
    ASSERT_TRUE(thread.join());
    ASSERT_TRUE(executed);

    thread.detach();

    ASSERT_FALSE(thread.joinable());
    ASSERT_FALSE(thread.join());
  }
}
