// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMORY_ALLOCATION_H__
#define __WN_MEMORY_ALLOCATION_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNAssert.h"
#include "WNCore/inc/WNTypeTraits.h"

#ifndef _WN_WINDOWS
    #include "WNMemory/inc/WNManipulation.h"
#endif

#include <new>
#include <stdlib.h>

#ifdef _WN_WINDOWS
    #include <malloc.h>

    #define WN_STACK_ALLOC(_size) _alloca(_size)
#elif defined _WN_LINUX || defined _WN_ANDROID
    #include <alloca.h>

    #define WN_STACK_ALLOC(_size) alloca(_size)
#endif

#define WN_PLACEMENT_NEW(_location) new(_location)
#define WN_NEW new

#ifdef _WN_DEBUG
    #define WN_DELETE(_object) delete _object; _object = wn_nullptr
    #define WN_ARRAY_DELETE(_object) delete[] _object; _object = wn_nullptr
#else
    #define WN_DELETE(_object) delete _object
    #define WN_ARRAY_DELETE(_object) delete[] _object
#endif

namespace wn {
    WN_FORCE_INLINE wn_void* malloc(const wn_size_t _size) {
        WN_DEBUG_ASSERT_DESC(_size != 0, "Attempting to allocate 0 bytes");

        return(::malloc(_size));
    }

    template <typename type>
    WN_FORCE_INLINE type* malloc(const wn_size_t _amount = 1) {
        return(static_cast<type*>(malloc<wn_void>(_amount * sizeof(type))));
    }

    template <>
    WN_FORCE_INLINE wn_void* malloc(const wn_size_t _size) {
        return(malloc(_size));
    }

    template <const wn_size_t size>
    WN_FORCE_INLINE wn_void* malloc() {
        static_assert(size != 0, "Attempting to allocate 0 elements");

        return(malloc<wn_void>(size));
    }

    template <const wn_size_t amount, typename type>
    WN_FORCE_INLINE typename std::enable_if<!is_same_decayed<type, wn_void>::value, type>::type* malloc() {
        static_assert(amount != 0, "Attempting to allocate 0 elements");

        return(static_cast<type*>(malloc<wn_void>(amount * sizeof(type))));
    }

    template <const wn_size_t size, typename type>
    WN_FORCE_INLINE typename std::enable_if<is_same_decayed<type, wn_void>::value, type>::type* malloc() {
        static_assert(size != 0, "Attempting to allocate 0 bytes");

        return(malloc<wn_void>(size));
    }

    /*// calloc
    WN_FORCE_INLINE wn_void* calloc(const wn_size_t _count, const wn_size_t _size) {
        WN_DEBUG_ASSERT_DESC(_count != 0, "Attempting to allocate 0 elements");
        WN_DEBUG_ASSERT_DESC(_size != 0, "Attempting to allocate 0 bytes");

        return(::calloc(_count, _size));
    }

    template <typename type>
    type* calloc(const wn_size_t _count = 1) {
        return(static_cast<type*>(calloc(_count, sizeof(type))));
    }

    template <>
    WN_FORCE_INLINE wn_void* calloc(const wn_size_t _size) {
        return(calloc(1, _size));
    }

    template <typename type, const wn_size_t count>
    typename std::enable_if<!is_same_decayed<type, wn_void>::value, type>::type* calloc() {
        static_assert(count != 0, "Attempting to allocate 0 elements");

        return(static_cast<type*>(calloc(sizeof(type), count)));
    }

    template <typename type, const wn_size_t count>
    typename std::enable_if<is_same_decayed<type, wn_void>::value, type>::type* calloc() {
        static_assert(count != 0, "Attempting to allocate 0 elements");

        return(calloc(1, count));
    }*/

    template <typename type>
    WN_FORCE_INLINE type* realloc(type* _memory, const wn_size_t _amount = 1) {
        return(static_cast<type*>(realloc<wn_void>(static_cast<wn_void*>(_memory), _amount * sizeof(type))));
    }

    template <>
    WN_FORCE_INLINE wn_void* realloc(wn_void* _memory, const wn_size_t _size) {
        WN_DEBUG_ASSERT_DESC(_size != 0, "Attempting to allocate 0 bytes");

        return(::realloc(_memory, _size));
    }

    template <const wn_size_t amount, typename type>
    WN_FORCE_INLINE typename std::enable_if<!is_same_decayed<type, wn_void>::value, type>::type* realloc(type* _memory) {
        static_assert(amount != 0, "Attempting to allocate 0 elements");

        return(static_cast<type*>(realloc<wn_void>(static_cast<wn_void*>(_memory), amount * sizeof(type))));
    }

    template <const wn_size_t size, typename type>
    WN_FORCE_INLINE typename std::enable_if<is_same_decayed<type, wn_void>::value, type>::type* realloc(type* _memory) {
        static_assert(size != 0, "Attempting to allocate 0 bytes");

        return(realloc<wn_void>(_memory, size));
    }

    template <typename type>
    WN_FORCE_INLINE wn_void free(type* _memory) {
        ::free(static_cast<wn_void*>(_memory));
    }

    WN_FORCE_INLINE wn_void* aligned_malloc(const wn_size_t _size, const wn_size_t _alignment) {
        WN_DEBUG_ASSERT_DESC(_size != 0, "Attempting to allocate 0 bytes");
        WN_DEBUG_ASSERT_DESC(_alignment > 0 && !(_alignment & (_alignment - 1)), "Alignment is not a positive power of 2");

        wn_void* memory;

        #ifdef _WN_WINDOWS
            memory = ::_aligned_malloc(_size, _alignment);
        #elif defined _WN_ANDROID
            memory = ::memalign(_alignment, _size);
        #elif defined _WN_POSIX
            if (::posix_memalign(&memory, _alignment, _size) != 0) {
                memory = wn_nullptr;
            }
        #else
            if (_alignment > 0 && !(_alignment & (_alignment - 1))) {
                wn_void* temp_memory = malloc((_size + _alignment - 1) + sizeof(wn_void*));

                if (temp_memory == wn_nullptr) {
                    return(wn_nullptr);
                }

                memory = reinterpret_cast<wn_void*>(((reinterpret_cast<wn_size_t>(temp_memory) +
                                                      sizeof(wn_void*) + _alignment - 1) & ~(_alignment - 1)));

                *(reinterpret_cast<wn_void**>(memory) - 1) = temp_memory;
            } else {
                return(wn_nullptr);
            }
        #endif

        return(memory);
    }

    template <typename type>
    WN_FORCE_INLINE type* aligned_malloc(const wn_size_t _amount, const wn_size_t _alignment) {
        return(static_cast<type*>(aligned_malloc(_amount * sizeof(type), _alignment)));
    }

    template <typename type, const wn_size_t alignment>
    WN_FORCE_INLINE type* aligned_malloc(const wn_size_t _amount) {
        static_assert(alignment > 0 && !(alignment & (alignment - 1)), "Alignment is not a positive power of 2");

        return(static_cast<type*>(aligned_malloc(_amount * sizeof(type), alignment)));
    }

    template <typename type, const wn_size_t amount, const wn_size_t alignment>
    WN_FORCE_INLINE type* aligned_malloc() {
        static_assert(amount != 0, "Attempting to allocate 0 bytes");
        static_assert(alignment > 0 && !(alignment & (alignment - 1)), "Alignment is not a positive power of 2");

        return(static_cast<type*>(aligned_malloc(amount * sizeof(type), alignment)));
    }

    /*WN_FORCE_INLINE wn_void* aligned_calloc(const wn_size_t _count, const wn_size_t _size, const wn_size_t _alignment) {
        WN_DEBUG_ASSERT_DESC(_count != 0, "Attempting to allocate 0 elements");
        WN_DEBUG_ASSERT_DESC(_size != 0, "Attempting to allocate 0 bytes");
        WN_DEBUG_ASSERT_DESC(_alignment > 0 && !(_alignment & (_alignment - 1)), "Alignment is not a positive power of 2");

        return(aligned_malloc(_count * _size, _alignment));
    }

    template <typename type>
    type* aligned_calloc(const wn_size_t _count, const wn_size_t _amount, const wn_size_t _alignment) {
        return(static_cast<type*>(aligned_calloc(_count, _amount * sizeof(type), _alignment)));
    }*/

    #ifndef _WN_WINDOWS
        wn_void aligned_free(wn_void* _memory);
    #endif

    WN_FORCE_INLINE wn_void* aligned_realloc(wn_void* _memory, const wn_size_t _size, const wn_size_t _alignment) {
        WN_DEBUG_ASSERT_DESC(_size != 0, "Attempting to allocate 0 bytes");
        WN_DEBUG_ASSERT_DESC(_alignment > 0 && !(_alignment & (_alignment - 1)), "Alignment is not a positive power of 2");

        wn_void* memory;

        #ifdef _WN_WINDOWS
            memory = ::_aligned_realloc(_memory, _size, _alignment);
        #else
            if (!(_alignment > 0 && !(_alignment & (_alignment - 1)))) {
                return(wn_nullptr);
            }

            memory = aligned_malloc(_size, _alignment);

            ::memcpy(memory, _memory, _size);
            aligned_free(_memory);
        #endif

        return(memory);
    }

    template <typename type>
    WN_FORCE_INLINE type* aligned_realloc(type* _memory, const wn_size_t _amount, const wn_size_t _alignment) {
        return(static_cast<type*>(aligned_realloc(static_cast<wn_void*>(_memory), _amount * sizeof(type), _alignment)));
    }

    template <typename type>
    WN_FORCE_INLINE wn_void aligned_free(type* _memory) {
        #ifdef _WN_WINDOWS
            ::_aligned_free(_memory);
        #elif defined _WN_POSIX
            ::free(_memory);
        #else
            free(*(reinterpret_cast<wn_void**>(_memory) - 1));
        #endif
    }
}

#endif // __WN_MEMORY_ALLOCATION_H__