// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNMultiTasking/inc/WNJobPool.h"
#include "WNMemory/inc/WNUniquePtr.h"
#include "WNTesting/inc/WNTestHarness.h"

void job(int32_t* _val) {
  *_val += 1;
}

TEST(job_pool, simple_job) {
  wn::memory::basic_allocator m_allocator;
  int32_t x = 32;
  {  // The only way to make sure a job_pool has finished all jobs
    // is to destroy it.
    wn::multi_tasking::thread_job_pool pool(&m_allocator, 1u);
    pool.add_job(wn::multi_tasking::make_job(&m_allocator, &job, &x));
  }
  EXPECT_EQ(33, x);
}

TEST(job_pool, simple_co_operative_jobs) {
  wn::memory::basic_allocator m_allocator;
  int32_t x = 32;
  {  // The only way to make sure a job_pool has finished all jobs
    // is to destroy it.
    wn::multi_tasking::thread_job_pool pool(&m_allocator, 1u);
    pool.add_job(wn::multi_tasking::make_job(&m_allocator, [&]() {
      x = 33;
      wn::multi_tasking::job_signal my_signal(0);
      pool.add_job(wn::multi_tasking::make_job(&m_allocator, [&]() {
        x = 34;
        my_signal.set(1);
      }));
      my_signal.wait_until(1);
      EXPECT_EQ(34, x);
    }));
  }
  EXPECT_EQ(34, x);
}

TEST(job_pool, multiple_base_threads) {
  wn::memory::basic_allocator m_allocator;
  std::atomic<uint32_t> x(32);
  {  // The only way to make sure a job_pool has finished all jobs
    // is to destroy it.
    // Run 100 tasks on 4 threads
    wn::multi_tasking::thread_job_pool pool(&m_allocator, 4u);
    pool.add_job(wn::multi_tasking::make_job(&m_allocator, [&]() {
      x.fetch_add(1);
      wn::multi_tasking::job_signal my_signal(0);
      for (size_t i = 0; i < 100; ++i) {
        pool.add_job(wn::multi_tasking::make_job(&m_allocator, [&]() {
          x.fetch_add(1);
          my_signal.increment(1);
        }));
      }
      my_signal.wait_until(100);

      EXPECT_EQ(133, x);
    }));
  }
  EXPECT_EQ(133, x);
}

TEST(job_pool, too_many_base_threads) {
  wn::memory::basic_allocator m_allocator;
  std::atomic<uint32_t> x(32);
  {  // The only way to make sure a job_pool has finished all jobs
    // is to destroy it.
    // Run 100 tasks on 120 threads
    wn::multi_tasking::thread_job_pool pool(&m_allocator, 120);
    pool.add_job(wn::multi_tasking::make_job(&m_allocator, [&]() {
      x.fetch_add(1);
      wn::multi_tasking::job_signal my_signal(0);
      for (size_t i = 0; i < 100; ++i) {
        pool.add_job(wn::multi_tasking::make_job(&m_allocator, [&]() {
          x.fetch_add(1);
          my_signal.increment(1);
        }));
      }
      my_signal.wait_until(100);

      EXPECT_EQ(133, x);
    }));
  }
  EXPECT_EQ(133, x);
}

TEST(job_pool, many_blocked_threads) {
  wn::memory::basic_allocator m_allocator;
  std::atomic<uint32_t> x(32);
  {  // The only way to make sure a job_pool has finished all jobs
    // is to destroy it.
    wn::multi_tasking::thread_job_pool pool(&m_allocator, 2);
    pool.add_job(wn::multi_tasking::make_job(&m_allocator, [&]() {
      x.fetch_add(1);
      wn::multi_tasking::job_signal my_signal(0);
      wn::multi_tasking::job_signal my_signal2(0);
      for (size_t i = 0; i < 100; ++i) {
        pool.add_job(wn::multi_tasking::make_job(&m_allocator, [&]() {
          x.fetch_add(1);
          my_signal.increment(1);
          my_signal.wait_until(200);  // All threads will block here.
          x.fetch_add(1);
          my_signal2.increment(1);
        }));
      }

      my_signal.wait_until(100);
      EXPECT_EQ(133, x);
      my_signal.set(200);
      my_signal2.wait_until(100);
      EXPECT_EQ(233, x);
    }));
  }
  EXPECT_EQ(233, x);
}

TEST(job_pool, wake_job_from_main_thread) {
  wn::memory::basic_allocator m_allocator;
  std::atomic<uint32_t> x(32);
  {  // The only way to make sure a job_pool has finished all jobs
    // is to destroy it.
    wn::multi_tasking::job_signal my_signal;
    // my_signal needs to be created before pool so that it get
    // destroyed AFTER pool.
    wn::multi_tasking::thread_job_pool pool(&m_allocator, 1);
    my_signal.initialize_signal(&pool, 0);
    pool.add_job(wn::multi_tasking::make_job(&m_allocator, [&]() {
      my_signal.wait_until(100);
      x.fetch_add(1);
      EXPECT_EQ(33, x);
    }));
    EXPECT_EQ(32, x);
    my_signal.set(100);
  }
  EXPECT_EQ(33, x);
}

TEST(job_pool, blocking_call) {
  wn::memory::basic_allocator m_allocator;
  {  // The only way to make sure a job_pool has finished all jobs
    // is to destroy it.
    wn::multi_tasking::thread_job_pool pool(&m_allocator, 1);
    {
      wn::multi_tasking::semaphore sem;
      wn::multi_tasking::semaphore sem2;
      wn::multi_tasking::semaphore last_wait;

      pool.add_job(wn::multi_tasking::make_job(&m_allocator, [&]() {
        pool.call_blocking_function(wn::containers::function<void()>([&]() {
          sem2.notify();
          sem.wait();
          last_wait.notify();
        }));
      }));
      sem2.wait();
      pool.add_job(
          wn::multi_tasking::make_job(&m_allocator, [&]() { sem.notify(); }));
      last_wait.wait();
    }
  }
}

TEST(job_pool, blocking_call_async) {
  wn::memory::basic_allocator m_allocator;
  {  // The only way to make sure a job_pool has finished all jobs
    // is to destroy it.
    wn::multi_tasking::semaphore sem;
    wn::multi_tasking::semaphore sem2;
    wn::multi_tasking::semaphore last_wait;

    {
      wn::multi_tasking::job_signal my_signal;
      // my_signal needs to be created before pool so that it get
      // destroyed AFTER pool.
      wn::multi_tasking::thread_job_pool pool(&m_allocator, 1);
      my_signal.initialize_signal(&pool, 0);

      pool.call_blocking_function_async(wn::containers::function<void()>([&]() {
                                          sem2.notify();
                                          sem.wait();
                                        }),
          &my_signal);

      sem2.wait();

      pool.add_job(
          wn::multi_tasking::make_job(&m_allocator, [&]() { sem.notify(); }));

      pool.add_job(wn::multi_tasking::make_job(&m_allocator, [&]() {
        my_signal.wait_until(1);
        last_wait.notify();
      }));

      last_wait.wait();
    }
  }
}

struct synchronized_object : public wn::multi_tasking::synchronized<> {
  void increment_number(wn::multi_tasking::semaphore* sem) {
    volatile uint32_t n = number;
    // wn::multi_tasking::this_thread::sleep_for(std::chrono::milliseconds(1));
    number = n + 1;
    sem->notify();
  }
  uint32_t number = 0;
};

TEST(job_pool, synchronizing_test) {
  wn::memory::basic_allocator m_allocator;
  wn::multi_tasking::semaphore sem;
  const uint32_t NUM_JOBS = 1000;
  {
    wn::memory::unique_ptr<synchronized_object> obj;
    // Create a pile of threads, because in reality only one should be
    // active at any given time.
    wn::multi_tasking::thread_job_pool pool(&m_allocator, 30);
    // A bit of strangeness here, only because synchronized objects
    // are expected to be created inside jobs.
    pool.add_job(
        wn::multi_tasking::make_job(&m_allocator, [&m_allocator, &obj, &sem]() {
          obj = wn::memory::make_unique<synchronized_object>(&m_allocator);
          sem.notify();
        }));
    sem.wait();
    for (size_t i = 0; i < NUM_JOBS; ++i) {
      pool.add_job(wn::multi_tasking::make_job(&m_allocator, obj.get(),
          &synchronized_object::increment_number, &sem));
    }

    for (size_t i = 0; i < NUM_JOBS; ++i) {
      sem.wait();
    }

    pool.join();
    EXPECT_EQ(obj->number, NUM_JOBS);
  }
}