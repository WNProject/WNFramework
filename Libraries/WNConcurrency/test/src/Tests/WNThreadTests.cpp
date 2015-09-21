// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNConcurrency/test/inc/Common.h"
#include "WNConcurrency/inc/WNThread.h"

static wn_void test_function1() {}

static wn_uint32 test_function2() { return(1); }

static wn_void test_function3(wn_void* _input) { WN_UNUSED_ARGUMENT(_input); }

static wn_void* test_function4(wn_void* _input) { return(_input); }

static wn_void test_function5(wn_uint32 _input) { WN_UNUSED_ARGUMENT(_input); }

static wn_uint32 test_function6(wn_uint32 _input) { return(_input); }

static wn_uint32 test_function7(wn_uint32* _input) { return(*_input); }

static wn_uint32 *test_function8(wn_uint32* _input) { return(_input); }

static wn_void test_function9(wn_uint32& _input) { _input = 1; }

class test_object {
public:
  test_object() : mTestVar(0) {}

  wn_void test_function1() { mTestVar++; }

  wn_uint32 test_function2() { return(mTestVar + 1); }

public:
  wn_uint32 mTestVar;
};

TEST(thread, basic_usage) {
  wn_void* temp;
  wn_void* pointer_test = &temp;
  wn_uint32 number_test1 = 5;
  wn_uint32 number_test2 = 0;

  wn::concurrency::thread<wn_void> test1(&test_function1);
  wn::concurrency::thread<wn_uint32> test2(&test_function2);
  wn::concurrency::thread<wn_void> test3(&test_function3, pointer_test);
  wn::concurrency::thread<wn_void*> test4(&test_function4, pointer_test);
  wn::concurrency::thread<wn_void> test5(&test_function5, number_test1);
  wn::concurrency::thread<wn_uint32> test6(&test_function6, number_test1);
  wn::concurrency::thread<wn_uint32> test7(&test_function7, &number_test1);
  wn::concurrency::thread<wn_uint32*> test8(&test_function8, &number_test1);
  wn::concurrency::thread<wn_void> test9(&test_function9,
                                         std::ref(number_test2));

  ASSERT_TRUE(test1.join());
  ASSERT_TRUE(test3.join());
  ASSERT_TRUE(test5.join());
  ASSERT_TRUE(test9.join());

  wn_uint32 test2_result;
  wn_void* test4_result;
  wn_uint32 test6_result;
  wn_uint32 test7_result;
  wn_uint32 *test8_result;

  ASSERT_TRUE(test2.join(test2_result));
  ASSERT_TRUE(test4.join(test4_result));
  ASSERT_TRUE(test6.join(test6_result));
  ASSERT_TRUE(test7.join(test7_result));
  ASSERT_TRUE(test8.join(test8_result));
  ASSERT_EQ(test2_result, 1);
  ASSERT_EQ(test4_result, pointer_test);
  ASSERT_EQ(test6_result, number_test1);
  ASSERT_EQ(test7_result, number_test1);
  ASSERT_EQ(test8_result, &number_test1);
  ASSERT_EQ(number_test2, 1);

  test_object test_object1;
  test_object test_object2;

  wn::concurrency::thread<wn_void> test10(&test_object::test_function1,
                                          &test_object1);
  wn::concurrency::thread<wn_uint32> test11(&test_object::test_function2,
                                            &test_object2);

  ASSERT_TRUE(test10.join());

  wn_uint32 test11_result;

  ASSERT_TRUE(test11.join(test11_result));
  ASSERT_EQ(test_object1.mTestVar, 1);
  ASSERT_EQ(test_object2.mTestVar, 0);
  ASSERT_EQ(test11_result, 1);
}

TEST(thread, joinable) {
  wn::concurrency::thread<wn_void> thread;

  ASSERT_FALSE(thread.joinable());

  wn_bool executed = wn_false;

  thread = wn::concurrency::thread<wn_void>(
      [](wn_bool& flag) { flag = wn_true; }, std::ref(executed));

  ASSERT_TRUE(thread.joinable());
  ASSERT_TRUE(thread.join());
  ASSERT_TRUE(executed);
  ASSERT_FALSE(thread.joinable());
}

TEST(thread, detach) {
  wn::concurrency::thread<wn_void> thread;

  ASSERT_FALSE(thread.joinable());

  wn_bool executed = wn_false;

  thread = wn::concurrency::thread<wn_void>(
      [](wn_bool& flag) { flag = wn_true; }, std::ref(executed));

  ASSERT_TRUE(thread.joinable());
  ASSERT_TRUE(thread.join());
  ASSERT_TRUE(executed);

  thread.detach();

  ASSERT_FALSE(thread.joinable());
  ASSERT_FALSE(thread.join());
}
