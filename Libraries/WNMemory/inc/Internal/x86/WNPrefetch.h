// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_INTERNAL_X86_PREFETCH_H__
#define __WN_CORE_INTERNAL_X86_PREFETCH_H__

#ifdef __WN_SSE_AVAILABLE
    #include "WNCore/inc/Internal/x86/WNPrefetch_SSE.h"
#elif defined __WN_3DNOW_AVAILABLE
    #include "WNCore/inc/Internal/x86/WNPrefetch_3DNow.h"
#endif

#endif // __WN_CORE_INTERNAL_X86_PREFETCH_H__