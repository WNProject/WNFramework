// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNExecutableTest/inc/WNTestHarness.h"
#include "WNMultiTasking/inc/thread.h"

namespace {

void test_function1(bool* _run) {
  *_run = true;
}

}  // namespace

TEST(thread, basic_usage) {
  wn::testing::allocator allocator;

  {
    bool run1 = false;
    wn::multi_tasking::thread thread1(&allocator, &test_function1, &run1);

    thread1.join();

    EXPECT_TRUE(run1);

    class test_object final {
    public:
      test_object() : m_run(false) {}

      void test_function1() {
        m_run = true;
      }

      bool run() const {
        return m_run;
      }

    private:
      bool m_run;
    };

    test_object object1;
    wn::multi_tasking::thread thread2(
        &allocator, &test_object::test_function1, &object1);

    thread2.join();

    EXPECT_TRUE(object1.run());

    bool run2 = false;
    wn::multi_tasking::thread thread3(
        &allocator, [](bool* _run) { *_run = true; }, &run2);

    thread3.join();

    EXPECT_TRUE(run2);
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

    thread.join();

    ASSERT_TRUE(executed);
    ASSERT_FALSE(thread.joinable());
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

  const wn::multi_tasking::thread::id id3(
      wn::multi_tasking::this_thread::get_id());
  const wn::multi_tasking::thread::id id4(
      wn::multi_tasking::this_thread::get_id());

  EXPECT_EQ(id3, id4);
  EXPECT_FALSE(id3 != id4);
  EXPECT_FALSE(id3 < id4);
  EXPECT_FALSE(id3 > id4);
  EXPECT_LE(id3, id4);
  EXPECT_GE(id3, id4);
  EXPECT_NE(id1, id3);

  wn::testing::allocator allocator;

  {
    int value1 = 0;
    int value2 = 0;
    wn::multi_tasking::thread thread1(
        &allocator, [](int& _value) { _value = 5; }, std::ref(value1));
    wn::multi_tasking::thread thread2(
        &allocator, [](int& _value) { _value = 10; }, std::ref(value2));
    const wn::multi_tasking::thread::id id5(thread1.get_id());
    const wn::multi_tasking::thread::id id6(thread2.get_id());

    EXPECT_NE(id5, id6);
    EXPECT_FALSE(id5 == id6);

    thread1.join();
    thread2.join();

    const wn::multi_tasking::thread::id id7(thread1.get_id());
    const wn::multi_tasking::thread::id id8(thread2.get_id());

    EXPECT_EQ(value1, 5);
    EXPECT_EQ(value2, 10);
    EXPECT_EQ(id7, id8);
    EXPECT_EQ(id7, id1);
    EXPECT_EQ(id8, id1);
  }
}
