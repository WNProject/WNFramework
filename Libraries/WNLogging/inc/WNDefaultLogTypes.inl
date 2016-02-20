// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_DEFAULT_LOG_TYPES_H__
#define __WN_DEFAULT_LOG_TYPES_H__

#include "WNContainers/inc/WNStringView.h"
#include "WNCore/inc/WNTypes.h"
#include "WNMemory/inc/WNStringUtility.h"

#include <cinttypes>

namespace WNLogging {
#define DEFINE_DEFAULT_LOG(type, encoding)                                     \
  template <typename T>                                                        \
  struct _Enc##type {                                                          \
    static const char* GetVal() {                                              \
      return encoding;                                                         \
    }                                                                          \
  };                                                                           \
  template <>                                                                  \
  struct _Enc##type<wchar_t> {                                                 \
    static const wchar_t* GetVal() {                                           \
      return L##encoding;                                                      \
    }                                                                          \
  };                                                                           \
  template <typename BuffType>                                                 \
  struct LogTypeHelper<type, BuffType> {                                       \
    WN_FORCE_INLINE static bool DoLog(                                         \
        const type& _0, BuffType* _buffer, size_t& _bufferLeft) {              \
      int printed = wn::memory::snprintf(                                      \
          _buffer, _bufferLeft, _Enc##type<BuffType>::GetVal(), _0);           \
      if (printed < 0 || static_cast<size_t>(printed) >= _bufferLeft) {        \
        return (false);                                                        \
      }                                                                        \
      _bufferLeft -= printed;                                                  \
      return (true);                                                           \
    }                                                                          \
  };

#ifdef _WN_WINDOWS
template <typename T>
struct _EncWNChar {};
template <>
struct _EncWNChar<char> {
  static const char* GetVal() {
    return "%s";
  }
};
template <>
struct _EncWNChar<wchar_t> {
  static const wchar_t* GetVal() {
    return L"%S";
  }
};
#else
template <typename T>
struct _EncWNChar {};
template <>
struct _EncWNChar<char> {
  static const char* GetVal() {
    return "%s";
  }
};
template <>
struct _EncWNChar<wchar_t> {
  static const wchar_t* GetVal() {
    return L"%ls";
  }
};
#endif
template <typename BuffType>
struct LogTypeHelper<char*, BuffType> {
  WN_FORCE_INLINE static bool DoLog(
      char* const& _0, BuffType* _buffer, size_t& _bufferLeft) {
    int printed = wn::memory::snprintf(
        _buffer, _bufferLeft, _EncWNChar<BuffType>::GetVal(), _0);
    if (printed < 0 || static_cast<size_t>(printed) >= _bufferLeft) {
      return (false);
    }
    _bufferLeft -= printed;
    return (true);
  }
};

template <typename BuffType>
struct LogTypeHelper<const char*, BuffType> {
  WN_FORCE_INLINE static bool DoLog(
      const char* const& _0, BuffType* _buffer, size_t& _bufferLeft) {
    int printed = wn::memory::snprintf(
        _buffer, _bufferLeft, _EncWNChar<BuffType>::GetVal(), _0);
    if (printed < 0 || static_cast<size_t>(printed) >= _bufferLeft) {
      return (false);
    }
    _bufferLeft -= printed;
    return (true);
  }
};

template <typename BuffType>
struct LogTypeHelper<wn::containers::string_view, BuffType> {
  WN_FORCE_INLINE static bool DoLog(const wn::containers::string_view& _0,
      BuffType* _buffer, size_t& _bufferLeft) {
    int printed = wn::memory::snprintf(
        _buffer, _bufferLeft, "%.*s", (uint32_t)_0.size(), (void*)_0.data());
    if (printed < 0 || static_cast<size_t>(printed) >= _bufferLeft) {
      return (false);
    }
    _bufferLeft -= printed;
    return (true);
  }
};

DEFINE_DEFAULT_LOG(int32_t, "%d");
DEFINE_DEFAULT_LOG(uint32_t, "%d");
#ifdef _WN_WINDOWS
DEFINE_DEFAULT_LOG(int64_t, "%I64d");
DEFINE_DEFAULT_LOG(uint64_t, "%I64u");
#else
DEFINE_DEFAULT_LOG(int64_t, "%lld");
DEFINE_DEFAULT_LOG(uint64_t, "%llu");
#endif
}

#endif  //__WN_DEFAULT_LOG_TYPES_H__
