// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_USE_PRECOMPILED_HEADER
    #pragma once
#endif

#ifndef __WN_CONCURRENCY_TESTS_COMMON_H__
#define __WN_CONCURRENCY_TESTS_COMMON_H__

#include "WNCore/inc/WNTypes.h"

#define GTEST_HAS_RTTI 0

#ifdef _WN_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4275)
#endif

#include <gtest/gtest.h>

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

#define RANDOM_TIME(_high, _low) (rand() % ((_high) - (_low) + 1) + (_low))

#include "WNConcurrency/inc/WNConcurrency.h"
#include "WNPlatform/inc/WNTime.h"

#include <vector>
#include <memory>

#endif // __WN_CONCURRENCY_TESTS_COMMON_H__