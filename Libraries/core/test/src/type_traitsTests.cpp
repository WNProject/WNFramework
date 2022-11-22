// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/inc/type_traits.h"
#include "core/inc/types.h"
#include "executable_test/inc/WNTestHarness.h"

TEST(type_traits, index_constant) {
  EXPECT_EQ((wn::core::index_constant<0>::value), 0u);
  EXPECT_EQ((wn::core::index_constant<(10 - 5)>::value), 5u);
}

TEST(type_traits, is_same) {
  EXPECT_TRUE((wn::core::is_same<bool, bool>::value));
  EXPECT_TRUE((wn::core::is_same<nullptr_t, nullptr_t>::value));
  EXPECT_TRUE((wn::core::is_same<float, float>::value));
  EXPECT_TRUE((wn::core::is_same<int64_t, int64_t>::value));
  EXPECT_FALSE((wn::core::is_same<const bool, bool>::value));
  EXPECT_FALSE((wn::core::is_same<int32_t, volatile int32_t>::value));
  EXPECT_FALSE(
      (wn::core::is_same<const volatile uint32_t, const uint32_t>::value));
  EXPECT_FALSE((wn::core::is_same<volatile float, float>::value));
  EXPECT_FALSE((wn::core::is_same<const bool, uint32_t>::value));
  EXPECT_FALSE((wn::core::is_same<int32_t, volatile float>::value));
  EXPECT_FALSE((wn::core::is_same<const volatile uint32_t, const bool>::value));
  EXPECT_FALSE((wn::core::is_same<volatile float, int32_t>::value));
  EXPECT_FALSE((wn::core::is_same<uint32_t, bool>::value));
  EXPECT_FALSE((wn::core::is_same<float, int32_t>::value));
  EXPECT_FALSE((wn::core::is_same<uint32_t, int32_t>::value));
}

TEST(type_traits, is_same_decayed) {
  EXPECT_TRUE((wn::core::is_same_decayed<bool, bool>::value));
  EXPECT_TRUE((wn::core::is_same_decayed<const bool, bool>::value));
  EXPECT_TRUE((wn::core::is_same_decayed<int32_t, volatile int32_t>::value));
  EXPECT_TRUE((wn::core::is_same_decayed<const volatile uint32_t,
      const uint32_t>::value));
  EXPECT_TRUE((wn::core::is_same_decayed<volatile float, float>::value));
  EXPECT_FALSE((wn::core::is_same_decayed<const bool, uint32_t>::value));
  EXPECT_FALSE((wn::core::is_same_decayed<int32_t, volatile float>::value));
  EXPECT_FALSE(
      (wn::core::is_same_decayed<const volatile uint32_t, const bool>::value));
  EXPECT_FALSE((wn::core::is_same_decayed<volatile float, int32_t>::value));
  EXPECT_FALSE((wn::core::is_same_decayed<uint32_t, bool>::value));
  EXPECT_FALSE((wn::core::is_same_decayed<float, int32_t>::value));
  EXPECT_FALSE((wn::core::is_same_decayed<uint32_t, int32_t>::value));
}

TEST(type_traits, are_same) {
  EXPECT_TRUE((wn::core::are_same<bool, bool, bool>::value));
}
