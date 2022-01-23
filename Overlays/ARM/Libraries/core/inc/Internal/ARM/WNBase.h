// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_INTERNAL_ARM_BASE_H__
#define __WN_CORE_INTERNAL_ARM_BASE_H__

#ifdef _WN_MSVC
#include "core/inc/Internal/ARM/WNBase_MSVC.h"
#else
#include "core/inc/Internal/ARM/WNBase_GCC_Clang.h"
#endif

#endif  // __WN_CORE_INTERNAL_ARM_BASE_H__
