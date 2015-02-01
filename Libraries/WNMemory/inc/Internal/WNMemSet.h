#pragma once

#ifndef __WN_MEMEORY_INTERNAL_MEMSET_H__
#define __WN_MEMEORY_INTERNAL_MEMSET_H__

#include "WNMemory/inc/WNConfig.h"

#include <string.h>

#ifdef __WN_MEMORY_HAS_PLATFORM_SPECIALIZED_MEMSET
    #ifdef _WN_X86
        #include "WNMemory/inc/Internal/x86/WNMemSet.h"
    #endif
#else
    namespace WNMemory {
        namespace internal {
            WN_FORCE_INLINE wn_void* __WNMemSet(wn_void* _memory, const wn_uint8 _value, const wn_size_t _size) {
                return(memset(_memory, _value, _size));
            }
        }
    }
#endif

#endif // __WN_MEMEORY_INTERNAL_MEMSET_H__