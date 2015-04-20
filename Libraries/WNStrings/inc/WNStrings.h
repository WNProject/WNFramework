// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_STRINGS_H__
#define __WN_STRINGS_H__

#include "WNCore/inc/WNBase.h"
#include "WNCore/inc/WNTypes.h"

#ifdef _WN_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4275)  // non – DLL-interface used as base for DLL-interface
#endif

#include <string>

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

typedef std::string WNString;

namespace WNStrings {
    WN_FORCE_INLINE wn_size_t WNStrLen(const wn_char* _string);
    WN_FORCE_INLINE wn_char* WNStrCpy(wn_char* _destination, const wn_char* _source);
    WN_FORCE_INLINE wn_char* WNStrCat(wn_char* _destination, const wn_char* _source);
    WN_FORCE_INLINE wn_uint32 WNStrCmp(const wn_char* _string1, const wn_char* _string2);
    WN_FORCE_INLINE wn_char* WNStrNCpy(wn_char* _destination, const wn_char* _source, wn_size_t _count);
    WN_FORCE_INLINE wn_char* WNStrNCat(wn_char* _destination, const wn_char* _source, wn_size_t _count);
    WN_FORCE_INLINE wn_uint32 WNStrNCmp(const wn_char* _string1, const wn_char* _string2, wn_size_t _count);
    WN_FORCE_INLINE wn_char* WNStrNDup(const wn_char* _string, wn_size_t _length);
    WN_FORCE_INLINE wn_char* WNStrChr(wn_char* _string, wn_uint32 _char);
    WN_FORCE_INLINE const wn_char* WNStrChr(const wn_char* _string, wn_uint32 _char);
    WN_FORCE_INLINE wn_char* WNStrStr(wn_char* _string1, const wn_char* _string2);
    WN_FORCE_INLINE const wn_char* WNStrStr(const wn_char* _string1, const wn_char* _string2);
    WN_FORCE_INLINE wn_size_t WNStrHash(const wn_char* _string);
    WN_FORCE_INLINE wn_uint32 WNStrToInt(const wn_char* _string);
    WN_FORCE_INLINE wn_int64 WNStrToLL(const wn_char* _string);
    WN_FORCE_INLINE wn_float32 WNStrToFlt(wn_char* _string);

    WN_FORCE_INLINE wn_wchar* WNStrToWStr(const wn_char* _string);
    WN_FORCE_INLINE wn_char* WNWStrToStr(const wn_wchar* _string);

    WN_FORCE_INLINE wn_size_t WNWStrLen(const wn_wchar* _string);
    WN_FORCE_INLINE wn_int32  WNSnPrintf(wn_char* _buffer, wn_size_t _count, const wn_char* _formatter, ...);
    WN_FORCE_INLINE wn_int32  WNWSnPrintf(wn_wchar* _buffer, wn_size_t _count, const wn_wchar* _formatter, ...);
    WN_FORCE_INLINE wn_char   WNToLower(wn_char);
    WN_FORCE_INLINE wn_char   WNToUpper(wn_char);

    template<typename T>
    WN_INLINE wn_int32  WNTSnPrintf(T* _buffer, wn_size_t _count, const T* _formatter, ...);

    // Since a float is only precise to about 7 decimal digits, we have a simple float to write out
    // certain floats without needing an exponent (easier to read)
    WN_FORCE_INLINE wn_size_t WNWriteSimpleFloat32(wn_char* _buff, wn_float32 _number, wn_size_t _maxSize);
    WN_FORCE_INLINE wn_size_t WNWriteInt32(wn_char* _buff, wn_int32 _number, wn_size_t _maxSize);
    WN_FORCE_INLINE wn_size_t WNWriteUInt32(wn_char* _buff, wn_uint32 _number, wn_size_t _maxSize);
    WN_FORCE_INLINE wn_size_t WNWriteFloat32(wn_char* _buff, wn_float32 _number, wn_size_t _maxSize);
    WN_FORCE_INLINE wn_size_t WNWriteInt64(wn_char* _buff, wn_int64 _number, wn_size_t _maxSize);
    WN_FORCE_INLINE wn_size_t WNWriteUInt64(wn_char* _buff, wn_uint64 _number, wn_size_t _maxSize);
    WN_FORCE_INLINE wn_size_t WNReadInt32(wn_char* _buff, wn_int32& _number, wn_size_t _maxSize);
    WN_FORCE_INLINE wn_size_t WNReadUInt32(wn_char* _buff, wn_uint32& _number, wn_size_t _maxSize);
    WN_FORCE_INLINE wn_size_t WNReadFloat32(wn_char* _buff, wn_float32& _number, wn_size_t _maxSize);
    WN_FORCE_INLINE wn_size_t WNReadInt64(wn_char* _buff, wn_int64& _number, wn_size_t _maxSize);
    WN_FORCE_INLINE wn_size_t WNReadUInt64(wn_char* _buff, wn_uint64& _number, wn_size_t _maxSize);
}

#include "WNStrings/inc/WNStrings.inl"

#endif // __WN_STRINGS_H__
