// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file intentionally has no header guards.
// It is intended for multiple inclusion.
#ifdef _WINDOWS_
#error Due to a bug in llvm it must be included before windows.h
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4100)
#pragma warning(disable: 4127)
#pragma warning(disable: 4152)
#pragma warning(disable: 4244)
#pragma warning(disable: 4245)
#pragma warning(disable: 4267)
#pragma warning(disable: 4355)
#pragma warning(disable: 4512)
#pragma warning(disable: 4800)
#if _MSC_VER >= 1900
#pragma warning(disable: 4310)
#pragma warning(disable: 4458)
#pragma warning(disable: 4624)
#pragma warning(disable: 4996)
#pragma warning(disable: 4146)
#endif
#endif
