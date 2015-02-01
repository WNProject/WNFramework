// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_INTERNAL_PREFETCH_H__
#define __WN_CORE_INTERNAL_PREFETCH_H__

#include "WNCore/inc/WNTypeTraits.h"

namespace WNCore {
    namespace __WNInternal {
        template <typename Type, typename = WNCore::WNEnableWhen<WN_TRUE>::Value>
        class __WNPrefetch {
        public:
            static WN_FORCE_INLINE WN_VOID Execute(const Type* _address) {
                WN_UNUSED_ARG(_address);
            }

        private:
            WN_FORCE_INLINE __WNPrefetch() {}
        };
    }
}

#ifdef __WN_CORE_EXTENSIONS_ENABLED
    #ifdef _WN_X86
        #include "WNCore/inc/Internal/x86/WNPrefetch.h"
    #endif
#endif

#endif // __WN_CORE_INTERNAL_PREFETCH_H__