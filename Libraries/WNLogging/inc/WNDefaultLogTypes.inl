// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_DEFAULT_LOG_TYPES_H__
#define __WN_DEFAULT_LOG_TYPES_H__

#include "WNCore/inc/WNTypes.h"
#include "WNMemory/inc/WNStringUtility.h"

#include <cinttypes>

namespace WNLogging {
#define DEFINE_DEFAULT_LOG(type, encoding) \
    template<typename T> \
    struct _Enc##type { static const wn_char* GetVal(){return encoding;} }; \
    template<> struct _Enc##type<wn_wchar> { static const wn_wchar* GetVal(){return L##encoding;} }; \
    template<typename BuffType> \
    struct LogTypeHelper<type, BuffType> { \
        WN_FORCE_INLINE static wn_bool DoLog(const type& _0, BuffType* _buffer, wn_size_t& _bufferLeft) { \
            int printed = wn::memory::snprintf(_buffer, _bufferLeft, _Enc##type<BuffType>::GetVal(), _0); \
            if(printed < 0 || static_cast<wn_size_t>(printed) >= _bufferLeft) { \
                return(wn_false); \
            } \
            _bufferLeft -= printed; \
            return(wn_true); \
        } \
    };

#ifdef _WN_WINDOWS
    template<typename T>
    struct _EncWNChar {};
    template<> struct _EncWNChar<wn_char> { static const wn_char* GetVal(){return "%s";} };
    template<> struct _EncWNChar<wn_wchar> { static const wn_wchar* GetVal(){return L"%S";} };
#else
    template<typename T>
    struct _EncWNChar {};
    template<> struct _EncWNChar<wn_char> { static const wn_char* GetVal(){return "%s";} };
    template<> struct _EncWNChar<wn_wchar> { static const wn_wchar* GetVal(){return L"%ls";} };
#endif
    template<typename BuffType>
    struct LogTypeHelper<wn_char*,  BuffType> {
        WN_FORCE_INLINE static wn_bool DoLog(wn_char*const & _0, BuffType* _buffer, wn_size_t& _bufferLeft) {
            int printed = wn::memory::snprintf(_buffer, _bufferLeft, _EncWNChar<BuffType>::GetVal(), _0);
            if(printed < 0 || static_cast<wn_size_t>(printed) >= _bufferLeft) {
                return(wn_false);
            }
            _bufferLeft -= printed;
            return(wn_true);
        }
    };

    template<typename BuffType>
    struct LogTypeHelper<const wn_char*,  BuffType> {
        WN_FORCE_INLINE static wn_bool DoLog(const wn_char*const & _0, BuffType* _buffer, wn_size_t& _bufferLeft) {
            int printed = wn::memory::snprintf(_buffer, _bufferLeft, _EncWNChar<BuffType>::GetVal(), _0);
            if(printed < 0 || static_cast<wn_size_t>(printed) >= _bufferLeft) {
                return(wn_false);
            }
            _bufferLeft -= printed;
            return(wn_true);
        }
    };

    DEFINE_DEFAULT_LOG(wn_int32, "%d");
    DEFINE_DEFAULT_LOG(wn_uint32, "%d");
#ifdef _WN_WINDOWS
    DEFINE_DEFAULT_LOG(wn_int64, "%I64d");
    DEFINE_DEFAULT_LOG(wn_uint64, "%I64u");
#else
    DEFINE_DEFAULT_LOG(wn_int64, "%lld");
    DEFINE_DEFAULT_LOG(wn_uint64, "%llu");
#endif

}

#endif//__WN_DEFAULT_LOG_TYPES_H__
