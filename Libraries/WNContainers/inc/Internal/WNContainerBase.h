// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_INTERNAL_CONTAINER_BASE_H__
#define __WN_CONTAINERS_INTERNAL_CONTAINER_BASE_H__

#include "WNcore/inc/WNTypes.h"

namespace wn {
    namespace containers {
        namespace internal {
            template <typename _Type, typename _Allocator>
            struct container_base {
                typedef _Type value_type;
                typedef wn_size_t size_type;
                typedef wn_signed_t difference_type;
                typedef _Allocator allocator_type;
                typedef value_type& reference;
                typedef const value_type& const_reference;
            };
        }
    }
}
#endif // __WN_CONTAINERS_INTERNAL_CONTAINER_BASE_H__