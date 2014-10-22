// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_INTERNAL_ARM_BASE_GCC_H__
#define __WN_CORE_INTERNAL_ARM_BASE_GCC_H__

#if _WN_ARM >= 7
    #define __WN_FPU_AVAILABLE

    #ifdef __ARM_NEON__
        #define __WN_NEON_AVAILABLE
    #endif
#endif

#endif // __WN_CORE_INTERNAL_ARM_BASE_GCC_H__