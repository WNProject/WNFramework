// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_PLATFORM_SYSTEM_H__
#define __WN_PLATFORM_SYSTEM_H__

#include "WNCore/inc/WNTypes.h"

namespace WNPlatform {
    namespace WNSystem {
        WN_VOID WNPopulateValues();

        WN_UINT8 WNGetPhysicalCoreCount();
        WN_UINT8 WNGetLogicalThreadCount();

        WN_UINT64 WNGetTotalPhysicalMemory();
        WN_UINT64 WNGetTotalVirtualMemory();

        WN_UINT64 WNGetAvailablePhysicalMemory();
        WN_UINT64 WNGetAvailableVirtualMemory();

        WN_BOOL WNIsHyperThreaded();

        WN_BOOL WNIsLittleEndian();
        WN_BOOL WNIsBigEndian();

        WN_BOOL WNHasFPU();

        // x86 specific feature detection functions. These will all return WN_FALSE if architecture is not x86

        WN_BOOL WNHasCMPXCHG8();
        WN_BOOL WNHasCMPXCHG16();
        WN_BOOL WNHasMMX();
        WN_BOOL WNHasEMMI();
        WN_BOOL WNHas3DNow();
        WN_BOOL WNHas3DNowPlus();
        WN_BOOL WNHasSSE();
        WN_BOOL WNHasSSE2();
        WN_BOOL WNHasSSE3();
        WN_BOOL WNHasSSSE3();
        WN_BOOL WNHasSSE41();
        WN_BOOL WNHasSSE42();
        WN_BOOL WNHasSSE4a();
        WN_BOOL WNHasF16C();
        WN_BOOL WNHasVEXPrefix();
        WN_BOOL WNHasAES();
        WN_BOOL WNHasCLMUL();
        WN_BOOL WNHasFMA3();
        WN_BOOL WNHasFMA4();
        WN_BOOL WNHasXOP();
        WN_BOOL WNHasAVX();
        WN_BOOL WNHasAVX2();

        // ARM specific feature detection functions. These will all return WN_FALSE if architecture is not ARM

        WN_BOOL WNHasNEON();
    }
}

#include "WNPlatform/inc/Internal/WNSystem.inl"

#endif // __WN_PLATFORM_SYSTEM_H__