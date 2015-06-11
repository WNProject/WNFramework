// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_MACROS_H__
#define __WN_CORE_MACROS_H__

#include "WNCore/inc/WNBase.h"
#include "WNCore/inc/WNTypes.h"

#ifdef _WN_WINDOWS
    #include <Windows.h>
#else
    #include <stdio.h>
#endif

#ifdef _WN_MSVC
    #pragma warning(disable: 4996) //snprintf

    #define WN_SNPRINTF(dest,size, format, ...) _snprintf(dest, size, format, ##__VA_ARGS__)
#else
    #define WN_SNPRINTF(dest,size, format, ...) snprintf(dest, size, format, ##__VA_ARGS__)
#endif

#endif // __WN_CORE_MACROS_H__
