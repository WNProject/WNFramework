// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMORY_MANIPULATION_H__
#define __WN_MEMORY_MANIPULATION_H__

#include "WNCore/inc/WNTypes.h"

#include <type_traits>
#include <cstring>

namespace wn {
    namespace memory {
        template <typename _Type>
        WN_FORCE_INLINE _Type* memset(_Type* _dest, const wn_uint8 _value) {
            static_assert(!std::is_void<_Type>::value,
                          "you must specify a size in bytes when trying to set memory of type void");

            return(memset(_dest, _value, 1));
        }

        template <typename _Type>
        WN_FORCE_INLINE _Type* memset(_Type* _dest, const wn_uint8 _value, const wn_size_t _count) {
            wn_void* dest = static_cast<wn_void*>(_dest);

            return(static_cast<_Type*>(memset<wn_void>(dest, _value, _count * sizeof(_Type))));
        }

        template <>
        WN_FORCE_INLINE wn_void* memset(wn_void* _dest, const wn_uint8 _value, const wn_size_t _count) {
            return(std::memset(_dest, _value, _count));
        }

        template <typename _Type>
        WN_FORCE_INLINE _Type* memzero(_Type* _dest) {
            static_assert(!std::is_void<_Type>::value,
                          "you must specify a size in bytes when trying to zero memory of type void");

            return(memzero(_dest, 1));
        }

        template <typename _Type>
        WN_FORCE_INLINE _Type* memzero(_Type* _dest, const wn_size_t _count) {
            wn_void* dest = static_cast<wn_void*>(_dest);

            return(static_cast<_Type*>(memzero<wn_void>(dest, _count * sizeof(_Type))));
        }

        template <>
        WN_FORCE_INLINE wn_void* memzero(wn_void* _dest, const wn_size_t _count) {
            return(memset(_dest, 0, _count));
        }

        template <typename _Type>
        WN_FORCE_INLINE _Type* memcpy(_Type* _dest, const _Type* _src) {
            static_assert(!std::is_void<_Type>::value,
                          "you must specify a size in bytes when trying to copy memory of type void");

            return(memcpy(_dest, _src, 1));
        }

        template <typename _Type>
        WN_FORCE_INLINE _Type* memcpy(_Type* _dest, const _Type* _src, const wn_size_t _count) {
            wn_void* dest = static_cast<wn_void*>(_dest);
            const wn_void* src = static_cast<const wn_void*>(_src);

            return(static_cast<_Type*>(memcpy<wn_void>(dest, src, _count * sizeof(_Type))));
        }

        template <>
        WN_FORCE_INLINE wn_void* memcpy(wn_void* _dest, const wn_void* _src, const wn_size_t _count) {
            return(std::memcpy(_dest, _src, _count));
        }

        template <typename _Type>
        WN_FORCE_INLINE _Type* memmove(_Type* _dest, const _Type* _src) {
            static_assert(!std::is_void<_Type>::value,
                          "you must specify a size in bytes when trying to move memory of type void");

            return(memmove(_dest, _src, 1));
        }

        template <typename _Type>
        WN_FORCE_INLINE _Type* memmove(_Type* _dest, const _Type* _src, const wn_size_t _count) {
            wn_void* dest = static_cast<wn_void*>(_dest);
            const wn_void* src = static_cast<const wn_void*>(_src);

            return(static_cast<_Type*>(memmove<wn_void>(dest, src, _count * sizeof(_Type))));
        }

        template <>
        WN_FORCE_INLINE wn_void* memmove(wn_void* _dest, const wn_void* _src, const wn_size_t _count) {
            return(std::memmove(_dest, _src, _count));
        }

        template <typename _Type>
        WN_FORCE_INLINE wn_int32 memcmp(const _Type* _lhs, const _Type* _rhs) {
            static_assert(!std::is_void<_Type>::value,
                          "you must specify a size in bytes when trying to compare memory of type void");

            return(memcmp(_lhs, _rhs, 1));
        }

        template <typename _Type>
        WN_FORCE_INLINE wn_int32 memcmp(const _Type* _lhs, const _Type* _rhs, const wn_size_t _count) {
            const wn_void* lhs = static_cast<const wn_void*>(_lhs);
            const wn_void* rhs = static_cast<const wn_void*>(_rhs);

            return(memcmp<wn_void>(lhs, rhs, _count * sizeof(_Type)));
        }

        template <>
        WN_FORCE_INLINE wn_int32 memcmp(const wn_void* _lhs, const wn_void* _rhs, const wn_size_t _count) {
            return(std::memcmp(_lhs, _rhs, _count));
        }
    }
}

#endif // __WN_MEMORY_MANIPULATION_H__