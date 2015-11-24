// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CORE_TEST_TEST_DEFINITIONS_H__
#define __WN_CORE_TEST_TEST_DEFINITIONS_H__
#include "WNCore/inc/WNBase.h"

#ifdef _WN_ANDROID
  #define WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(expression, regexp)
#else
#ifdef _WN_DEBUG
#define WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(expression, regexp) \
  EXPECT_DEATH(expression, regexp)
#else
#define WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(expression, regexp)
#endif
#endif

#endif // __WN_CORE_TEST_TEST_DEFINITIONS_H__