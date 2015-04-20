// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMORY_ALLOCATION_H__
#define __WN_MEMORY_ALLOCATION_H__

#include "WNCore/inc/WNAssert.h"
#include "WNCore/inc/WNTypeTraits.h"

#ifndef _WN_WINDOWS
    #include "WNMemory/inc/WNManipulation.h"
#endif

#include <new>
#include <cstdlib>
#include <utility>

#ifdef _WN_WINDOWS
    #include <malloc.h>

    #define WN_STACK_ALLOC(_size) ::_alloca(_size)
#elif defined _WN_LINUX || defined _WN_ANDROID
    #include <alloca.h>

    #define WN_STACK_ALLOC(_size) ::alloca(_size)
#endif

namespace wn {
    namespace memory {
        WN_FORCE_INLINE wn_void* malloc(const wn_size_t _size) {
            WN_RELEASE_ASSERT_DESC(_size != 0, "attempting to allocate 0 bytes");

            return(std::malloc(_size));
        }

        WN_FORCE_INLINE wn_void* realloc(wn_void* _ptr, const wn_size_t _new_size) {
            return(std::realloc(_ptr, _new_size));
        }

        WN_FORCE_INLINE wn_void free(wn_void* _ptr) {
            std::free(_ptr);
        }

        WN_FORCE_INLINE wn_void* aligned_malloc(const wn_size_t _size, const wn_size_t _alignment) {
            WN_RELEASE_ASSERT_DESC(_size != 0, "attempting to allocate 0 bytes");
            WN_RELEASE_ASSERT_DESC(_alignment > 0 && !(_alignment & (_alignment - 1)),
                                   "alignment is not a positive power of 2");

            #ifdef _WN_WINDOWS
                return(::_aligned_malloc(_size, _alignment));
            #elif defined _WN_ANDROID
                return(::memalign(_alignment, _size));
            #elif defined _WN_POSIX
                wn_void* ptr;

                if (::posix_memalign(&ptr, _alignment, _size) != 0) {
                    return(wn_nullptr);
                }

                return(ptr);
            #else
                wn_void* temp_ptr = malloc((_size + _alignment - 1) + sizeof(wn_void*));

                if (temp_ptr == wn_nullptr) {
                    return(wn_nullptr);
                }

                wn_void* ptr = reinterpret_cast<wn_void*>(((reinterpret_cast<wn_size_t>(temp_ptr) +
                                                               sizeof(wn_void*) + _alignment - 1) & ~(_alignment - 1)));

                *(reinterpret_cast<wn_void**>(ptr) - 1) = temp_ptr;

                return(ptr);
            #endif
        }

        #ifndef _WN_WINDOWS
            wn_void aligned_free(wn_void* _ptr);
        #endif

        WN_FORCE_INLINE wn_void* aligned_realloc(wn_void* _ptr, const wn_size_t _new_size, const wn_size_t _alignment) {
            WN_RELEASE_ASSERT_DESC(_new_size != 0, "attempting to allocate 0 bytes");
            WN_RELEASE_ASSERT_DESC(_alignment > 0 && !(_alignment & (_alignment - 1)),
                                   "alignment is not a positive power of 2");

            #ifdef _WN_WINDOWS
                return(::_aligned_realloc(_ptr, _new_size, _alignment));
            #else
                wn_void* ptr = aligned_malloc(_new_size, _alignment);

                ptr = memcpy(ptr, _ptr, _new_size);

                aligned_free(_ptr);

                return(ptr);
            #endif
        }

        WN_FORCE_INLINE wn_void aligned_free(wn_void* _ptr) {
            #ifdef _WN_WINDOWS
                ::_aligned_free(_ptr);
            #elif defined _WN_POSIX
                free(_ptr);
            #else
                free(*(reinterpret_cast<wn_void**>(_ptr) - 1));
            #endif
        }

        template <typename _Type>
        WN_FORCE_INLINE _Type* heap_allocate() {
            static_assert(!std::is_void<_Type>::value,
                          "you must specify a size in bytes when trying to allocate memory of type void");

            return(static_cast<_Type*>(malloc(sizeof(_Type))));
        }

        template <typename _Type>
        WN_FORCE_INLINE _Type* heap_allocate(const wn_size_t _size) {
            return(static_cast<_Type*>(malloc(_size * sizeof(_Type))));
        }

        template <>
        WN_FORCE_INLINE wn_void* heap_allocate(const wn_size_t _size) {
            return(malloc(_size));
        }

        template <typename _Type>
        WN_FORCE_INLINE _Type* heap_reallocate(_Type* _ptr, const wn_size_t _new_size) {
            wn_void* ptr = static_cast<wn_void*>(_ptr);

            return(static_cast<_Type*>(realloc(ptr, _new_size * sizeof(_Type))));
        }

        template <>
        WN_FORCE_INLINE wn_void* heap_reallocate(wn_void* _ptr, const wn_size_t _new_size) {
            WN_RELEASE_ASSERT_DESC(_new_size != 0, "attempting to allocate 0 bytes");

            return(realloc(_ptr, _new_size));
        }

        template <typename _Type>
        WN_FORCE_INLINE wn_void heap_free(_Type* _ptr) {
            free(static_cast<wn_void*>(_ptr));
        }

        template <>
        WN_FORCE_INLINE wn_void heap_free(wn_void* _ptr) {
            free(_ptr);
        }

        template <typename _Type>
        WN_FORCE_INLINE _Type* heap_aligned_allocate(const wn_size_t _alignment) {
            return(static_cast<_Type*>(aligned_malloc(sizeof(_Type), _alignment)));
        }

        template <typename _Type>
        WN_FORCE_INLINE _Type* heap_aligned_allocate(const wn_size_t _size, const wn_size_t _alignment) {
            return(static_cast<_Type*>(aligned_malloc(_size * sizeof(_Type), _alignment)));
        }

        template <>
        WN_FORCE_INLINE wn_void* heap_aligned_allocate(const wn_size_t _size, const wn_size_t _alignment) {
            return(aligned_malloc(_size, _alignment));
        }

        template <typename _Type>
        WN_FORCE_INLINE _Type* heap_aligned_reallocate(_Type* _ptr, const wn_size_t _new_size, const wn_size_t _alignment) {
            wn_void* ptr = static_cast<wn_void*>(_ptr);

            return(static_cast<_Type*>(aligned_realloc(ptr, _new_size * sizeof(_Type), _alignment)));
        }

        template <>
        WN_FORCE_INLINE wn_void* heap_aligned_reallocate(wn_void* _ptr, const wn_size_t _new_size, const wn_size_t _alignment) {
            return(aligned_realloc(_ptr, _new_size, _alignment));
        }

        template <typename _Type>
        WN_FORCE_INLINE wn_void heap_aligned_free(_Type* _ptr) {
            aligned_free(static_cast<wn_void*>(_ptr));
        }

        template <>
        WN_FORCE_INLINE wn_void heap_aligned_free(wn_void* _ptr) {
            aligned_free(_ptr);
        }

        template <typename _Type, typename... _Args>
        WN_FORCE_INLINE _Type* construct(_Args&&... _args) {
            static_assert(!std::is_void<_Type>::value, "you cannot construct an object of type void");

            return(new(std::nothrow) _Type(std::forward<_Args>(_args)...));
        }

        template <typename _Type, typename... _Args>
        WN_FORCE_INLINE _Type* construct_at(_Type* _ptr, _Args&&... _args) {
            static_assert(!std::is_void<_Type>::value, "you cannot construct an object of type void");

            return(new(_ptr) _Type(std::forward<_Args>(_args)...));
        }

        template <typename _Type, typename... _Args>
        WN_FORCE_INLINE _Type* construct_at(wn_void* _ptr, _Args&&... _args) {
            static_assert(!std::is_void<_Type>::value, "you cannot construct an object of type void");

            return(construct_at(static_cast<_Type*>(_ptr), std::forward<_Args>(_args)...));
        }

        template <typename _Type>
        WN_FORCE_INLINE wn_void destroy(_Type* _ptr) {
            delete(_ptr);
        }
    }
}

#endif // __WN_MEMORY_ALLOCATION_H__
