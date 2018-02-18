// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CORE_TYPES_H__
#define __WN_CORE_TYPES_H__

#include "WNCore/inc/WNBase.h"

#include <cfloat>
#include <cstddef>
#include <cstdint>

#ifdef _WN_64_BIT
using signed_t = int64_t;
#else
using signed_t = int32_t;
#endif

using nullptr_t = std::nullptr_t;

#endif  // __WN_CORE_TYPES_H__
