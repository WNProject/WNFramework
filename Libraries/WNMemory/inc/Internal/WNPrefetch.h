// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMORY_INTERNAL_PREFETCH_H__
#define __WN_MEMORY_INTERNAL_PREFETCH_H__

#include "WNCore/inc/WNTypeTraits.h"
#include "WNCore/inc/WNUtility.h"

namespace wn {
    namespace memory {
        namespace internal {
            struct prefetch : core::non_constructable_non_copyable {
                template <typename _Type>
                static WN_FORCE_INLINE wn_void execute(const _Type* _address) {
                    WN_UNUSED_ARGUMENT(_address);
                }
            };
        }
    }
}

#endif // __WN_MEMORY_INTERNAL_PREFETCH_H__