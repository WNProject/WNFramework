// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNExecutableTest/inc/WNTestHarness.h"
#include "WNMultiTasking/inc/WNJobPool.h"
#include "WNMultiTasking/inc/synchronized.h"

using wn::multi_tasking::async_blocking_function;
using wn::multi_tasking::async_function;

template <typename T>
using sd = wn::multi_tasking::synchronized_destroy<T>;

struct sync_struct : public wn::multi_tasking::synchronized<> {
  void func(async_function) {
    volatile int y = x;
    y += 1;
    x = y;
  }
  void func2(async_function, int z) {
    volatile int y = x;
    y -= z;
    x = y;
  }

  void bfunc(async_blocking_function, int z) {
    x = wn::multi_tasking::job_pool::this_job_pool()
            ->call_blocking_function<int>(
                &sync_struct::blocking_function, this, z);
  }

  int blocking_function(int z) {
    volatile int y = x;
    y -= z;
    return y;
  }

  void move_func(async_function, wn::memory::unique_ptr<int> _x) {
    x = *_x;
  }

  int32_t x = 0;
};

struct sync_struct2 : public sync_struct {
  sync_struct2(int32_t& y) : m_y(y) {}
  ~sync_struct2() {
    m_y = x;
  }
  int32_t& m_y;
};

struct s2 {
  void func() {}
};

TEST(synchronized, all_same) {
  sync_struct my_s;
  ASSERT_EQ(true, wn::multi_tasking::is_synchronized<sync_struct>::type::value);
  ASSERT_NE(nullptr, my_s.get_synchronization_data());
}

TEST(synchronized, not_synchronized) {
  ASSERT_EQ(false, wn::multi_tasking::is_synchronized<s2>::type::value);
}

TEST(synchronized, multi_test) {
  wn::testing::allocator m_allocator;
  wn::multi_tasking::thread_job_pool pool(&m_allocator, 2);

  pool.add_unsynchronized_job(nullptr, [&]() {
    wn::multi_tasking::job_signal sig(0);
    sd<sync_struct> s;
    for (size_t i = 0; i < 12; ++i) {
      pool.add_job(&sig, &sync_struct::func, s.get());
      pool.add_job(&sig, &sync_struct::func2, s.get(), 1);
    }
    sig.wait_until(24);
    ASSERT_EQ(0, s->x);
  });
}

TEST(synchronized, destroy_test) {
  wn::testing::allocator m_allocator;
  int32_t y = 42;
  {
    wn::multi_tasking::thread_job_pool pool(&m_allocator, 2);
    pool.add_unsynchronized_job(nullptr, [&]() {
      wn::multi_tasking::job_signal sig(0);
      sd<sync_struct2> s(y);
      for (size_t i = 0; i < 12; ++i) {
        pool.add_job(&sig, &sync_struct::func, s.get());
        pool.add_job(&sig, &sync_struct::func2, s.get(), 1);
      }
    });
  }
  ASSERT_EQ(0, y);
}

TEST(synchronized, blocking_test) {
  wn::testing::allocator m_allocator;
  int32_t y = 42;
  {
    wn::multi_tasking::thread_job_pool pool(&m_allocator, 2);
    pool.add_unsynchronized_job(nullptr, [&]() {
      wn::multi_tasking::job_signal sig(0);
      sd<sync_struct2> s(y);
      for (size_t i = 0; i < 12; ++i) {
        pool.add_job(&sig, &sync_struct::func, s.get());
        pool.add_job(&sig, &sync_struct::bfunc, s.get(), 1);
      }
    });
  }
  ASSERT_EQ(0, y);
}

TEST(synchronized, move_test) {
  wn::testing::allocator m_allocator;
  wn::memory::unique_ptr<int> m_int =
      wn::memory::make_unique<int>(&m_allocator, 32);
  wn::multi_tasking::thread_job_pool pool(&m_allocator, 2);

  pool.add_unsynchronized_job(nullptr, [&]() {
    wn::multi_tasking::job_signal sig(0);
    sd<sync_struct> s;
    pool.add_job(&sig, &sync_struct::move_func, s.get(), wn::core::move(m_int));
    sig.wait_until(1);
    ASSERT_EQ(32, s->x);
  });
}
