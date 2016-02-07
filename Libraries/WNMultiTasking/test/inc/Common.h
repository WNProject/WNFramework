// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_USE_PRECOMPILED_HEADER
#pragma once
#endif

#ifndef __WN_CONCURRENCY_TESTS_COMMON_H__
#define __WN_CONCURRENCY_TESTS_COMMON_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/test/inc/TestDefinitions.h"

#define RANDOM_TIME(_high, _low) (rand() % ((_high) - (_low) + 1) + (_low))

#include "WNPlatform/inc/WNTime.h"

#include <memory>
#include <vector>

#endif  // __WN_CONCURRENCY_TESTS_COMMON_H__