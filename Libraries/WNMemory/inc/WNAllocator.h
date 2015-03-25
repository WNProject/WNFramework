// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMORY_ALLOCATOR_H__
#define __WN_MEMORY_ALLOCATOR_H__

#include "WNMath/inc/WNBasic.h"
#include "WNMemory/inc/WNAllocation.h"

namespace wn {
    namespace memory {
        struct allocation_pair final {
            wn_void* m_location;
            wn_size_t m_count;
        };

        class allocator {
        public:
            virtual ~allocator() {}

            virtual allocation_pair allocate(const wn_size_t _size, const wn_size_t _count) = 0;
            virtual allocation_pair allocate_for_resize(const wn_size_t _size, const wn_size_t _count,
                                                        const wn_size_t _old_size) = 0;
            virtual allocation_pair reallocate(wn_void* _memory, const wn_size_t _number,
                                               const wn_size_t _count, const wn_size_t _old_size = 0) = 0;
            virtual wn_void deallocate(wn_void* _memory) = 0;
        };

        template <const wn_size_t _ExpandPercent, const wn_size_t _MinimumAllocationSize = 1>
        struct default_expanding_allocator : allocator {
            virtual allocation_pair allocate(const wn_size_t _size, const wn_size_t _count) {
                const wn_size_t count = wn::max(_count, _MinimumAllocationSize);
                allocation_pair pair { malloc(_size * count), count };

                return(pair);
            }

            virtual allocation_pair allocate_for_resize(const wn_size_t _size, const wn_size_t _count,
                                                        const wn_size_t _old_size) {
                const wn_size_t count = wn::max(_count, _MinimumAllocationSize);
                const wn_size_t new_size = static_cast<wn_size_t>(_old_size * (1 + (_ExpandPercent / 100.0f)));
                const wn_size_t allocated_number = wn::max(new_size, count);
                allocation_pair pair { malloc(_size * allocated_number), allocated_number };

                return(pair);
            }

            virtual allocation_pair reallocate(wn_void* _memory, const wn_size_t _size,
                                               const wn_size_t _count, const wn_size_t _old_size = 0) {
                const wn_size_t count = wn::max(_count, _MinimumAllocationSize);
                const wn_size_t new_size = static_cast<wn_size_t>(_old_size * (1 + (_ExpandPercent / 100.0f)));
                const wn_size_t allocated_number = wn::max(new_size, count);
                allocation_pair pair { realloc(_memory, _size * allocated_number), count };

                return(pair);
            }

            virtual wn_void deallocate(wn_void* _memory) {
                free(_memory);
            }
        };

        typedef default_expanding_allocator<50> default_allocator;
    }
}

#endif // __WN_MEMORY_ALLOCATOR_H__