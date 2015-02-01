#pragma once

#ifndef __WN_MEMORY_INTERNAL_MEMSET_HELPER_H__
#define __WN_MEMORY_INTERNAL_MEMSET_HELPER_H__

#include "WNCore/inc/WNTypes.h"

namespace WNMemory {
    namespace internal {
        template <wn_size_t Alignment>
        WN_FORCE_INLINE wn_void __WNMemSetHelperSetUnaligned(wn_uint8*& _memory, const wn_uint8& _value, wn_size_t& _count) {
            while (_count > 0 && (reinterpret_cast<wn_size_t>(_memory) & (Alignment - 1))) {
                *(_memory++) = _value;

                _count--;
            }
        }

        WN_FORCE_INLINE wn_void __WNMemSetHelperSetRemaining(wn_uint8*& _memory, const wn_uint8& _value, wn_size_t& _count) {
            while (_count > 0) {
                *(_memory++) = _value;

                _count--;
            }
        }
    }
}

#endif // __WN_MEMORY_INTERNAL_MEMSET_HELPER_H__