// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CORE_TYPES_H__
#define __WN_CORE_TYPES_H__

#include <cstdint>
#include <type_traits>

#include "WNCore/inc/base.h"

using signed_t = std::make_signed<size_t>::type;
using nullptr_t = decltype(nullptr);

#endif  // __WN_CORE_TYPES_H__
