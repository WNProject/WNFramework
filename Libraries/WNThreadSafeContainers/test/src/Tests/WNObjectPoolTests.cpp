// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNThreadSafeContainers/inc/WNObjectPool.h"
#include "WNTesting/inc/WNTestHarness.h"

struct counted_object {
  counted_object(const counted_object&) {
    num_calls = 100;
  }
  counted_object(counted_object&& _other) {
    num_calls = _other.num_calls;
  }
  counted_object& operator=(const counted_object&) {
    num_calls = 99;
    return *this;
  }
  counted_object(int32_t val) { num_calls = val; }
  int32_t num_calls;
};

TEST(object_pool, simple_construction) {
  wn::testing::allocator alloc;
  wn::threadsafe_containers::object_pool<counted_object> pool(&alloc,
      []() { return counted_object(32); },
      [](counted_object* _obj) { _obj->num_calls = 42; });

  {
    auto num_allocated = alloc.current_allocations().size();
    auto obj = pool.get_object();
    EXPECT_EQ(32, (*obj).num_calls);
    EXPECT_EQ(32, obj->num_calls);
    EXPECT_EQ(num_allocated + 1, alloc.current_allocations().size());
  }
  {
    // We should have reset the object but not
    // created any new memory allocations.
    auto allocate = alloc.allocated();
    auto obj = pool.get_object();
    EXPECT_EQ(42, (*obj).num_calls);
    EXPECT_EQ(42, obj->num_calls);
    EXPECT_EQ(allocate, alloc.allocated());
  }
}

TEST(object_pool, copy_construction) {
  wn::testing::allocator alloc;
  wn::threadsafe_containers::object_pool<counted_object> pool(
      &alloc, counted_object(201));

  {
    // They should be copy-constructed for creation
    auto obj = pool.get_object();
    EXPECT_EQ(100, (*obj).num_calls);
    EXPECT_EQ(100, obj->num_calls);
  }
  {
    // They should be copy-assigned on re-aquisition
    auto obj = pool.get_object();
    EXPECT_EQ(99, (*obj).num_calls);
    EXPECT_EQ(99, obj->num_calls);
  }
}
