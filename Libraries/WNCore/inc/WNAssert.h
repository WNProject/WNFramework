// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_ASSERT_H__
#define __WN_CORE_ASSERT_H__

#include "WNCore/inc/WNBase.h"
#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNMacros.h"

namespace WNCore {
    namespace __WNInternal {
        WN_VOID __WNAssert(WN_INT32 _argument, const WN_CHAR* _file, WN_UINT32 _line, const WN_CHAR* _message);

        template <typename Type>
        struct __WNStaticAssertDependentFail {
            enum {
                Value = WN_FALSE
            };
        };
    }
}

#ifndef _WN_RETAIL
    #define WN_RELEASE_ASSERT_DESC(_argument, _description) WNCore::__WNInternal::__WNAssert(static_cast<WN_INT32>(_argument), __FILE__, __LINE__, _description)
#else
    #define WN_RELEASE_ASSERT_DESC(_argument, _description)
#endif

#ifdef _WN_DEBUG
    #define WN_DEBUG_ASSERT_DESC(_argument, _description) WNCore::__WNInternal::__WNAssert(static_cast<WN_INT32>(_argument), __FILE__, __LINE__, _description)
#else
    #define WN_DEBUG_ASSERT_DESC(_argument, _description)
#endif

#define WN_RELEASE_ASSERT(_argument) WN_RELEASE_ASSERT_DESC(_argument, WN_STRINGERIZE(_argument))
#define WN_DEBUG_ASSERT(_argument) WN_DEBUG_ASSERT_DESC(_argument, WN_STRINGERIZE(_argument))

#ifdef __WN_HAS_STATIC_ASSERT
    #define WN_STATIC_ASSERT_DESC(_argument, _description) static_assert(_argument, _description)
    #define WN_STATIC_ASSERT_DEPENDENT_FAIL WNCore::__WNInternal::__WNStaticAssertDependentFail
#else
    #define WN_STATIC_ASSERT_DESC(_argument, _description)
    #define WN_STATIC_ASSERT_DEPENDENT_FAIL
#endif

#define WN_STATIC_ASSERT(_argument) WN_STATIC_ASSERT_DESC((_argument), WN_STRINGERIZE(_argument))

#include "WNCore/inc/Internal/WNAssert.inl"

#endif // __WN_CORE_ASSERT_H__
