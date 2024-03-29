// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_INTERNAL_BASE_GCC_H__
#define __WN_CORE_INTERNAL_BASE_GCC_H__

#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 8)
#error "Compiler not supported: GNU C 4.8 or higher must be used"
#endif

#ifdef __GNUC_PATCHLEVEL__
#define _WN_GCC (__GNUC__ * 100000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#define _WN_GCC_MAJOR __GNUC__
#define _WN_GCC_MINOR __GNUC_MINOR__
#define _WN_GCC_BUILD __GNUC_PATCHLEVEL__
#else
#define _WN_GCC (__GNUC__ * 100000 + __GNUC_MINOR__ * 100)
#define _WN_GCC_MAJOR __GNUC__
#define _WN_GCC_MINOR __GNUC_MINOR__
#define _WN_GCC_BUILD 0
#endif

#include "core/inc/Internal/WNBase_Clang_GCC.h"

#endif  // __WN_CORE_INTERNAL_BASE_GCC_H__
