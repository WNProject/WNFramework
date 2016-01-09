// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CORE_INTERNAL_ARM_BASE_H__
#define __WN_CORE_INTERNAL_ARM_BASE_H__

#ifdef _WN_MSVC
    #include "WNCore/inc/Internal/ARM/WNBase_MSVC.h"
#elif defined _WN_GCC
    #include "WNCore/inc/Internal/ARM/WNBase_GCC.h"
#elif defined _WN_CLANG
    #include "WNCore/inc/Internal/ARM/WNBase_Clang.h"
#endif

#endif // __WN_CORE_INTERNAL_ARM_BASE_H__