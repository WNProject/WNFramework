// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_TESTING_TEST_HARNESS_H__
#define __WN_TESTING_TEST_HARNESS_H__

#include "WNCore/inc/WNTypes.h"
#include "WNMemory/inc/WNAllocator.h"

#define GTEST_HAS_RTTI 0

#ifdef _WN_MSVC
#pragma warning(push)
#pragma warning(disable : 4275)
#pragma warning(disable : 4702)  // disable unreachable code warning.
                                 // Work-around for xtree issue when exceptions
                                 // disabled
#endif

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#ifdef _WN_MSVC
#pragma warning(pop)
#endif

#ifdef _WN_ANDROID
#define WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(expression, regexp)
#else
#define HAS_DEATH_TEST
#ifdef _WN_DEBUG
#define WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(expression, regexp)                 \
  EXPECT_DEATH(expression, regexp)
#else
#define WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(expression, regexp)
#endif
#endif

namespace wn {
namespace testing {

class allocator : public memory::allocation_tracker {
private:
  using base = memory::allocation_tracker;

public:
  WN_FORCE_INLINE allocator() : base(&m_allocator) {}

  WN_FORCE_INLINE virtual ~allocator() override {
    EXPECT_TRUE(empty());
  }

  WN_FORCE_INLINE virtual void* allocate(const size_t _size) override {
    return base::allocate(_size);
  }

  WN_FORCE_INLINE virtual void* reallocate(
      void* _ptr, const size_t _size) override {
    return base::reallocate(_ptr, _size);
  }

  WN_FORCE_INLINE virtual void deallocate(void* _ptr) override {
    base::deallocate(_ptr);
  }

private:
  memory::basic_allocator m_allocator;
};

}  // namespace testing
}  // namespace wn

#define WN_ENABLE_TYPED_TEST(_name)                                            \
  template <typename T>                                                        \
  struct _name : ::testing::Test {}

#endif  // __WN_TESTING_TEST_HARNESS_H__
