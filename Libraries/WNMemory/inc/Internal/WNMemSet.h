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
        namespace __WNInternal {
            WN_FORCE_INLINE WN_VOID* __WNMemSet(WN_VOID* _memory, const WN_UINT8 _value, const WN_SIZE_T _size) {
                return(memset(_memory, _value, _size));
            }
        }
    }
#endif

#endif // __WN_MEMEORY_INTERNAL_MEMSET_H__