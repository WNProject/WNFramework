// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_INTERNAL_BASE_CLANG_H__
#define __WN_CORE_INTERNAL_BASE_CLANG_H__

#if __clang_major__ < 3 || (__clang_major__ == 3 && __clang_minor__ < 3)
#error "Compiler not supported: Clang 3.3 or higher must be used"
#endif

#ifdef __clang_patchlevel__
#define _WN_CLANG                                                              \
  (__clang_major__ * 100000 + __clang_minor__ * 100 + __clang_patchlevel__)
#define _WN_CLANG_MAJOR __clang_major__
#define _WN_CLANG_MINOR __clang_minor__
#define _WN_CLANG_BUILD __clang_patchlevel__
#else
#define _WN_CLANG (__clang_major__ * 100000 + __clang_minor__ * 100)
#define _WN_CLANG_MAJOR __clang_major__
#define _WN_CLANG_MINOR __clang_minor__
#define _WN_CLANG_BUILD 0
#endif

#include "core/inc/Internal/WNBase_Clang_GCC.h"

#endif  // __WN_CORE_INTERNAL_BASE_CLANG_H__
