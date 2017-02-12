// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNCore/inc/WNTypeTraits.h"
#include "WNCore/inc/WNExtendedTypes.h"
#include "WNCore/inc/WNTypes.h"
#include "WNExecutableTest/inc/WNTestHarness.h"

TEST(type_traits, integral_constant) {
  EXPECT_TRUE((wn::core::integral_constant<bool, true>::value));
  EXPECT_EQ((wn::core::integral_constant<uint32_t, (10 % 10)>::value), 0);
}

TEST(type_traits, index_constant) {
  EXPECT_EQ((wn::core::index_constant<0>::value), 0);
  EXPECT_EQ((wn::core::index_constant<(10 - 5)>::value), 5);
}

TEST(type_traits, bool_constant) {
  EXPECT_TRUE(wn::core::bool_constant<true>::value);
  EXPECT_TRUE(wn::core::bool_constant<(10 % 10 == 0)>::value);
  EXPECT_FALSE(wn::core::bool_constant<false>::value);
  EXPECT_FALSE(wn::core::bool_constant<(10 % 4 == 0)>::value);
}

TEST(type_traits, is_null_pointer) {
  EXPECT_TRUE((wn::core::is_null_pointer<decltype(nullptr)>::value));
  EXPECT_FALSE((wn::core::is_null_pointer<bool>::value));
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

TEST(type_traits, are_same) {
  EXPECT_TRUE((wn::core::are_same<bool, bool, bool>::value));
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

TEST(type_traits, is_arithmetic) {
  EXPECT_TRUE(wn::core::is_arithmetic<wn_float8>::value);
  EXPECT_TRUE(wn::core::is_arithmetic<const wn_float16>::value);
  EXPECT_TRUE(wn::core::is_arithmetic<volatile float>::value);
  EXPECT_TRUE(wn::core::is_arithmetic<const volatile double>::value);
  EXPECT_TRUE(wn::core::is_arithmetic<wn_fixed8>::value);
  EXPECT_TRUE(wn::core::is_arithmetic<const wn_fixed16>::value);
  EXPECT_TRUE(wn::core::is_arithmetic<volatile wn_fixed32>::value);
  EXPECT_TRUE(wn::core::is_arithmetic<const volatile wn_fixed64>::value);
  EXPECT_FALSE(wn::core::is_arithmetic<void>::value);
  EXPECT_FALSE(wn::core::is_arithmetic<const void*>::value);
  EXPECT_FALSE(wn::core::is_arithmetic<volatile void*>::value);
  EXPECT_FALSE(wn::core::is_arithmetic<const volatile void*>::value);
}

TEST(type_traits, is_floating_point) {
  EXPECT_TRUE(wn::core::is_floating_point<wn_float8>::value);
  EXPECT_TRUE(wn::core::is_floating_point<const wn_float16>::value);
  EXPECT_TRUE(wn::core::is_floating_point<volatile float>::value);
  EXPECT_TRUE(wn::core::is_floating_point<const volatile double>::value);
  EXPECT_FALSE(wn::core::is_floating_point<bool>::value);
  EXPECT_FALSE(wn::core::is_floating_point<const int32_t>::value);
  EXPECT_FALSE(wn::core::is_floating_point<volatile uint32_t>::value);
  EXPECT_FALSE(wn::core::is_floating_point<const volatile wn_fixed32>::value);
}

TEST(type_traits, is_fixed_point) {
  EXPECT_TRUE(wn::core::is_fixed_point<wn_fixed8>::value);
  EXPECT_TRUE(wn::core::is_fixed_point<const wn_fixed16>::value);
  EXPECT_TRUE(wn::core::is_fixed_point<volatile wn_fixed32>::value);
  EXPECT_TRUE(wn::core::is_fixed_point<const volatile wn_fixed64>::value);
  EXPECT_TRUE((wn::core::is_fixed_point<wn_fixed<uint32_t, 25>>::value));
  EXPECT_FALSE(wn::core::is_fixed_point<bool>::value);
  EXPECT_FALSE(wn::core::is_fixed_point<const int32_t>::value);
  EXPECT_FALSE(wn::core::is_fixed_point<volatile uint32_t>::value);
  EXPECT_FALSE(wn::core::is_fixed_point<const volatile float>::value);
}

TEST(type_traits, is_real) {
  EXPECT_TRUE(wn::core::is_real<wn_fixed8>::value);
  EXPECT_TRUE(wn::core::is_real<const wn_fixed16>::value);
  EXPECT_TRUE(wn::core::is_real<volatile wn_fixed32>::value);
  EXPECT_TRUE(wn::core::is_real<const volatile wn_fixed64>::value);
  EXPECT_TRUE((wn::core::is_real<wn_fixed<uint32_t, 25>>::value));
  EXPECT_TRUE(wn::core::is_real<wn_float8>::value);
  EXPECT_TRUE(wn::core::is_real<const wn_float16>::value);
  EXPECT_TRUE(wn::core::is_real<volatile float>::value);
  EXPECT_TRUE(wn::core::is_real<const volatile double>::value);
  EXPECT_FALSE(wn::core::is_real<bool>::value);
  EXPECT_FALSE(wn::core::is_real<const int32_t>::value);
  EXPECT_FALSE(wn::core::is_real<volatile uint32_t>::value);
}