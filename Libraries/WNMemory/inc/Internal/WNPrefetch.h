// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_INTERNAL_PREFETCH_H__
#define __WN_CORE_INTERNAL_PREFETCH_H__

#include "WNCore/inc/WNTypeTraits.h"

namespace WNCore {
    namespace internal {
        template <typename Type, typename = wn::enable_if<wn_true>::Value>
        class __WNPrefetch {
        public:
            static WN_FORCE_INLINE wn_void Execute(const Type* _address) {
                WN_UNUSED_ARGUMENT(_address);
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