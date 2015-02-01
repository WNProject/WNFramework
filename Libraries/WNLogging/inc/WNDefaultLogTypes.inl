// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_DEFAULT_LOG_TYPES_H__
#define __WN_DEFAULT_LOG_TYPES_H__

#include "WNCore/inc/WNBase.h"
#include "WNCore/inc/WNMacros.h"
#ifdef _WN_WINDOWS
#ifndef PRId64
#define PRId64 "lld"
#endif
#ifndef PRId64
#define PRIu64 "llu"
#endif
#else
    #include <cinttypes>    
#endif
namespace WNLogging {
#define DEFINE_DEFAULT_LOG(Type, encoding) \
    template<typename T> \
    struct _Enc##Type { static const WN_CHAR* GetVal(){return encoding;} }; \
    template<> struct _Enc##Type<WN_WCHAR> { static const WN_WCHAR* GetVal(){return L##encoding;} }; \
    template<typename BuffType> \
    struct LogTypeHelper<Type, BuffType> { \
        WN_FORCE_INLINE static WN_BOOL DoLog(const Type& _0, BuffType* _buffer, WN_SIZE_T& _bufferLeft) { \
            int printed = WNStrings::WNTSnPrintf(_buffer, _bufferLeft, _Enc##Type<BuffType>::GetVal(), _0); \
            if(printed < 0 || static_cast<WN_SIZE_T>(printed) >= _bufferLeft) { \
                return(WN_FALSE); \
            } \
            _bufferLeft -= printed; \
            return(WN_TRUE); \
        } \
    }; 

#ifdef _WN_WINDOWS
    template<typename T>
    struct _EncWNChar {};
    template<> struct _EncWNChar<WN_CHAR> { static const WN_CHAR* GetVal(){return "%s";} };
    template<> struct _EncWNChar<WN_WCHAR> { static const WN_WCHAR* GetVal(){return L"%S";} };
#else
    template<typename T>
    struct _EncWNChar {};
    template<> struct _EncWNChar<WN_CHAR> { static const WN_CHAR* GetVal(){return "%s";} };
    template<> struct _EncWNChar<WN_WCHAR> { static const WN_WCHAR* GetVal(){return L"%ls";} };
#endif
    template<typename BuffType>
    struct LogTypeHelper<WN_CHAR*,  BuffType> {
        WN_FORCE_INLINE static WN_BOOL DoLog(WN_CHAR*const & _0, BuffType* _buffer, WN_SIZE_T& _bufferLeft) {
            int printed = WNStrings::WNTSnPrintf(_buffer, _bufferLeft, _EncWNChar<BuffType>::GetVal(), _0);
            if(printed < 0 || static_cast<WN_SIZE_T>(printed) >= _bufferLeft) {
                return(WN_FALSE);
            }
            _bufferLeft -= printed;
            return(WN_TRUE);
        }
    };

    template<typename BuffType>
    struct LogTypeHelper<const WN_CHAR*,  BuffType> {
        WN_FORCE_INLINE static WN_BOOL DoLog(const WN_CHAR*const & _0, BuffType* _buffer, WN_SIZE_T& _bufferLeft) {
            int printed = WNStrings::WNTSnPrintf(_buffer, _bufferLeft, _EncWNChar<BuffType>::GetVal(), _0);
            if(printed < 0 || static_cast<WN_SIZE_T>(printed) >= _bufferLeft) {
                return(WN_FALSE);
            }
            _bufferLeft -= printed;
            return(WN_TRUE);
        }
    };

    DEFINE_DEFAULT_LOG(WN_INT32, "%d");
    DEFINE_DEFAULT_LOG(WN_UINT32, "%d");
#ifdef _WN_WINDOWS
    DEFINE_DEFAULT_LOG(WN_INT64, "%I64d");
    DEFINE_DEFAULT_LOG(WN_UINT64, "%I64u");
#else
    DEFINE_DEFAULT_LOG(WN_INT64, "%lld");
    DEFINE_DEFAULT_LOG(WN_UINT64, "%llu");
#endif

}

#endif//__WN_DEFAULT_LOG_TYPES_H__
