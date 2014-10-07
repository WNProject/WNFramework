#pragma once

#ifndef __WN_MEMORY_INTERNAL_MEMSET_HELPER_H__
#define __WN_MEMORY_INTERNAL_MEMSET_HELPER_H__

#include "WNCore/inc/WNTypes.h"

namespace WNMemory {
    namespace __WNInternal {
        template <WN_SIZE_T Alignment>
        WN_FORCE_INLINE WN_VOID __WNMemSetHelperSetUnaligned(WN_UINT8*& _memory, const WN_UINT8& _value, WN_SIZE_T& _count) {
            while (_count > 0 && (reinterpret_cast<WN_SIZE_T>(_memory) & (Alignment - 1))) {
                *(_memory++) = _value;

                _count--;
            }
        }

        WN_FORCE_INLINE WN_VOID __WNMemSetHelperSetRemaining(WN_UINT8*& _memory, const WN_UINT8& _value, WN_SIZE_T& _count) {
            while (_count > 0) {
                *(_memory++) = _value;

                _count--;
            }
        }
    }
}

#endif // __WN_MEMORY_INTERNAL_MEMSET_HELPER_H__