////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    WN_FORCE_INLINE WN_SIZE_T WNStrLen(const WN_CHAR* _string);
    WN_FORCE_INLINE WN_CHAR* WNStrCpy(WN_CHAR* _destination, const WN_CHAR* _source);
    WN_FORCE_INLINE WN_CHAR* WNStrCat(WN_CHAR* _destination, const WN_CHAR* _source);
    WN_FORCE_INLINE WN_UINT32 WNStrCmp(const WN_CHAR* _string1, const WN_CHAR* _string2);
    WN_FORCE_INLINE WN_CHAR* WNStrNCpy(WN_CHAR* _destination, const WN_CHAR* _source, WN_SIZE_T _count);
    WN_FORCE_INLINE WN_CHAR* WNStrNCat(WN_CHAR* _destination, const WN_CHAR* _source, WN_SIZE_T _count);
    WN_FORCE_INLINE WN_UINT32 WNStrNCmp(const WN_CHAR* _string1, const WN_CHAR* _string2, WN_SIZE_T _count);
    WN_FORCE_INLINE WN_CHAR* WNStrNDup(const WN_CHAR* _string, WN_SIZE_T _length);
    WN_FORCE_INLINE WN_CHAR* WNStrChr(WN_CHAR* _string, WN_UINT32 _char);
    WN_FORCE_INLINE const WN_CHAR* WNStrChr(const WN_CHAR* _string, WN_UINT32 _char);
    WN_FORCE_INLINE WN_CHAR* WNStrStr(WN_CHAR* _string1, const WN_CHAR* _string2);
    WN_FORCE_INLINE const WN_CHAR* WNStrStr(const WN_CHAR* _string1, const WN_CHAR* _string2);
    WN_FORCE_INLINE WN_SIZE_T WNStrHash(const WN_CHAR* _string);
    WN_FORCE_INLINE WN_UINT32 WNStrToInt(const WN_CHAR* _string);
    WN_FORCE_INLINE WN_INT64 WNStrToLL(const WN_CHAR* _string);
    WN_FORCE_INLINE WN_FLOAT32 WNStrToFlt(WN_CHAR* _string);

    WN_FORCE_INLINE WN_WCHAR* WNStrToWStr(const WN_CHAR* _string);
    WN_FORCE_INLINE WN_CHAR* WNWStrToStr(const WN_WCHAR* _string);

    WN_FORCE_INLINE WN_SIZE_T WNWStrLen(const WN_WCHAR* _string);
    WN_FORCE_INLINE WN_INT32  WNSnPrintf(WN_CHAR* _buffer, WN_SIZE_T _count, const WN_CHAR* _formatter, ...);
    WN_FORCE_INLINE WN_INT32  WNWSnPrintf(WN_WCHAR* _buffer, WN_SIZE_T _count, const WN_WCHAR* _formatter, ...);

    template<typename T>
    WN_INLINE WN_INT32  WNTSnPrintf(T* _buffer, WN_SIZE_T _count, const T* _formatter, ...);
    
    // Since a float is only precise to about 7 decimal digits, we have a simple float to write out
    // certain floats without needing an exponent (easier to read)
    WN_FORCE_INLINE WN_SIZE_T WNWriteSimpleFloat32(WN_CHAR* _buff, WN_FLOAT32 _number, WN_SIZE_T _maxSize);
    WN_FORCE_INLINE WN_SIZE_T WNWriteInt32(WN_CHAR* _buff, WN_INT32 _number, WN_SIZE_T _maxSize);
    WN_FORCE_INLINE WN_SIZE_T WNWriteUInt32(WN_CHAR* _buff, WN_UINT32 _number, WN_SIZE_T _maxSize);
    WN_FORCE_INLINE WN_SIZE_T WNWriteFloat32(WN_CHAR* _buff, WN_FLOAT32 _number, WN_SIZE_T _maxSize);
    WN_FORCE_INLINE WN_SIZE_T WNWriteInt64(WN_CHAR* _buff, WN_INT64 _number, WN_SIZE_T _maxSize);
    WN_FORCE_INLINE WN_SIZE_T WNWriteUInt64(WN_CHAR* _buff, WN_UINT64 _number, WN_SIZE_T _maxSize);
    WN_FORCE_INLINE WN_SIZE_T WNReadInt32(WN_CHAR* _buff, WN_INT32& _number, WN_SIZE_T _maxSize);
    WN_FORCE_INLINE WN_SIZE_T WNReadUInt32(WN_CHAR* _buff, WN_UINT32& _number, WN_SIZE_T _maxSize);
    WN_FORCE_INLINE WN_SIZE_T WNReadFloat32(WN_CHAR* _buff, WN_FLOAT32& _number, WN_SIZE_T _maxSize);
    WN_FORCE_INLINE WN_SIZE_T WNReadInt64(WN_CHAR* _buff, WN_INT64& _number, WN_SIZE_T _maxSize);
    WN_FORCE_INLINE WN_SIZE_T WNReadUInt64(WN_CHAR* _buff, WN_UINT64& _number, WN_SIZE_T _maxSize);
}

#include "WNStrings/inc/WNStrings.inl"

#endif // __WN_STRINGS_H__
