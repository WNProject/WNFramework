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
        wn_void* memset(wn_void* _dest, const wn_uint8 _value, const wn_size_t _count);

        WN_FORCE_INLINE wn_void* memzero(wn_void* _dest, const wn_size_t _count) {
            return(memset(_dest, 0, _count));
        }

        WN_FORCE_INLINE wn_void* memset(wn_void* _dest, const wn_uint8 _value, const wn_size_t _count) {
            return(std::memset(_dest, _value, _count));
        }

        WN_FORCE_INLINE wn_void* memcpy(wn_void* _dest, const wn_void* _src, const wn_size_t _count) {
            return(std::memcpy(_dest, _src, _count));
        }

        WN_FORCE_INLINE wn_void* memmove(wn_void* _dest, const wn_void* _src, const wn_size_t _count) {
            return(std::memmove(_dest, _src, _count));
        }

        WN_FORCE_INLINE wn_int32 memcmp(const wn_void* _lhs, const wn_void* _rhs, const wn_size_t _count) {
            return(std::memcmp(_lhs, _rhs, _count));
        }

        template <typename _Type>
        WN_FORCE_INLINE _Type* memory_zero(_Type* _dest) {
            static_assert(!std::is_void<_Type>::value,
                          "you must specify a size in bytes when trying to zero memory of type void");

            return(static_cast<_Type*>(memzero(_dest, sizeof(_Type))));
        }

        template <typename _Type>
        WN_FORCE_INLINE _Type* memory_zero(_Type* _dest, const wn_size_t _count) {
            return(static_cast<_Type*>(memzero(_dest, _count * sizeof(_Type))));
        }

        template <>
        WN_FORCE_INLINE wn_void* memory_zero(wn_void* _dest, const wn_size_t _count) {
            return(memzero(_dest, _count));
        }

        template <typename _Type>
        WN_FORCE_INLINE _Type* memory_set(_Type* _dest, const wn_uint8 _value) {
            static_assert(!std::is_void<_Type>::value,
                          "you must specify a size in bytes when trying to set memory of type void");

            return(static_cast<_Type*>(memset(_dest, _value, sizeof(_Type))));
        }

        template <typename _Type>
        WN_FORCE_INLINE _Type* memory_set(_Type* _dest, const wn_uint8 _value, const wn_size_t _count) {
            return(static_cast<_Type*>(memset(_dest, _value, _count * sizeof(_Type))));
        }

        template <>
        WN_FORCE_INLINE wn_void* memory_set(wn_void* _dest, const wn_uint8 _value, const wn_size_t _count) {
            return(memset(_dest, _value, _count));
        }

        template <typename _Type>
        WN_FORCE_INLINE _Type* memory_copy(_Type* _dest, const _Type* _src) {
            static_assert(!std::is_void<_Type>::value,
                          "you must specify a size in bytes when trying to copy memory of type void");

            return(static_cast<_Type*>(memcpy(_dest, _src, sizeof(_Type))));
        }

        template <typename _Type>
        WN_FORCE_INLINE _Type* memory_copy(_Type* _dest, const _Type* _src, const wn_size_t _count) {
            return(static_cast<_Type*>(memcpy(_dest, _src, _count * sizeof(_Type))));
        }

        template <>
        WN_FORCE_INLINE wn_void* memory_copy(wn_void* _dest, const wn_void* _src, const wn_size_t _count) {
            return(memcpy(_dest, _src, _count));
        }

        template <typename _Type>
        WN_FORCE_INLINE _Type* memory_move(_Type* _dest, const _Type* _src) {
            static_assert(!std::is_void<_Type>::value,
                          "you must specify a size in bytes when trying to move memory of type void");

            return(static_cast<_Type*>(memmove(_dest, _src, sizeof(_Type))));
        }

        template <typename _Type>
        WN_FORCE_INLINE _Type* memory_move(_Type* _dest, const _Type* _src, const wn_size_t _count) {
            return(static_cast<_Type*>(memmove(_dest, _src, _count * sizeof(_Type))));
        }

        template <>
        WN_FORCE_INLINE wn_void* memory_move(wn_void* _dest, const wn_void* _src, const wn_size_t _count) {
            return(memmove(_dest, _src, _count));
        }

        template <typename _Type>
        WN_FORCE_INLINE wn_int32 memory_compare(const _Type* _lhs, const _Type* _rhs) {
            static_assert(!std::is_void<_Type>::value,
                          "you must specify a size in bytes when trying to compare memory of type void");

            return(memcmp(_lhs, _rhs, sizeof(_Type)));
        }

        template <typename _Type>
        WN_FORCE_INLINE wn_int32 memory_compare(const _Type* _lhs, const _Type* _rhs, const wn_size_t _count) {
            return(memcmp(_lhs, _rhs, _count * sizeof(_Type)));
        }

        template <>
        WN_FORCE_INLINE wn_int32 memory_compare(const wn_void* _lhs, const wn_void* _rhs, const wn_size_t _count) {
            return(memcmp(_lhs, _rhs, _count));
        }
    }
}

#endif // __WN_MEMORY_MANIPULATION_H__