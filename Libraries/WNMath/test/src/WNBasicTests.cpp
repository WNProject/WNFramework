// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNMath/inc/WNBasic.h"
#include "core/inc/types.h"
#include "executable_test/inc/WNTestHarness.h"

TEST(basic, abs) {
  ASSERT_EQ(wn::math::abs(int8_t(-1)), int8_t(1));
  ASSERT_EQ(wn::math::abs(int16_t(-1)), int16_t(1));
  ASSERT_EQ(wn::math::abs(int32_t(-1)), int32_t(1));
  ASSERT_EQ(wn::math::abs(int64_t(-1)), int64_t(1));
  ASSERT_EQ(wn::math::abs(float(-1.0f)), float(1.0f));
  ASSERT_EQ(wn::math::abs(double(-1.0)), double(1.0));
}

TEST(basic, ceil) {
  ASSERT_EQ(wn::math::ceil(float(1.346f)), float(2));
  ASSERT_EQ(wn::math::ceil(double(1.346)), double(2));
}

TEST(basic, floor) {
  ASSERT_EQ(wn::math::floor(float(1.346f)), float(1.0f));
  ASSERT_EQ(wn::math::floor(double(1.346f)), double(1.0));
}

TEST(basic, round) {
  ASSERT_EQ(wn::math::round(float(1.346f)), float(1.0f));
  ASSERT_EQ(wn::math::round(double(1.746)), double(2.0));
}

TEST(basic, min) {
  ASSERT_EQ(wn::math::min(float(1.346f), float(-0.345f)), float(-0.345f));
  ASSERT_EQ(wn::math::min(double(1.346), double(-0.345)), double(-0.345));
}
