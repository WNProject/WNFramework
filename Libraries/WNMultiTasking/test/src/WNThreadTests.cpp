// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNExecutableTest/inc/WNTestHarness.h"
#include "WNMultiTasking/inc/thread.h"

static void test_function1() {}

class test_object {
public:
  test_object() : mTestVar(0) {}

  void test_function1() {
    mTestVar++;
  }

public:
  uint32_t mTestVar;
};

TEST(thread, basic_usage) {
  wn::testing::allocator allocator;

  {
    wn::multi_tasking::thread test1(&allocator, &test_function1);

    EXPECT_TRUE(test1.join());
  }
}

TEST(thread, joinable) {
  wn::testing::allocator allocator;

  {
    wn::multi_tasking::thread thread;

    ASSERT_FALSE(thread.joinable());

    bool executed = false;

    thread = wn::multi_tasking::thread(
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
    wn::multi_tasking::thread thread;

    ASSERT_FALSE(thread.joinable());

    bool executed = false;

    thread = wn::multi_tasking::thread(
        &allocator, [](bool& flag) { flag = true; }, std::ref(executed));

    ASSERT_TRUE(thread.joinable());
    ASSERT_TRUE(thread.join());
    ASSERT_TRUE(executed);

    thread.detach();

    ASSERT_FALSE(thread.joinable());
    ASSERT_FALSE(thread.join());
  }
}

TEST(thread_id, equality) {
  wn::multi_tasking::thread::id id1;
  wn::multi_tasking::thread::id id2;

  EXPECT_EQ(id1, id2);
  EXPECT_FALSE(id1 != id2);
  EXPECT_FALSE(id1 < id2);
  EXPECT_FALSE(id1 > id2);
  EXPECT_LE(id1, id2);
  EXPECT_GE(id1, id2);

  const wn::multi_tasking::thread::id id3 =
      wn::multi_tasking::this_thread::get_id();
  const wn::multi_tasking::thread::id id4 =
      wn::multi_tasking::this_thread::get_id();

  EXPECT_EQ(id3, id4);
  EXPECT_FALSE(id3 != id4);
  EXPECT_FALSE(id3 < id4);
  EXPECT_FALSE(id3 > id4);
  EXPECT_LE(id3, id4);
  EXPECT_GE(id3, id4);
  EXPECT_NE(id1, id3);
}
