// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_LOGGING_DEFAULT_LOG_TYPES_INL__
#define __WN_LOGGING_DEFAULT_LOG_TYPES_INL__

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNMemory/inc/string_utility.h"
#include "core/inc/types.h"

#include <cinttypes>

namespace wn {
namespace logging {

#define DEFINE_DEFAULT_LOG(type, encoding)                                     \
  template <typename T>                                                        \
  struct _enc##type {                                                          \
    static const char* get_val() {                                             \
      return encoding;                                                         \
    }                                                                          \
  };                                                                           \
  template <>                                                                  \
  struct _enc##type<wchar_t> {                                                 \
    static const wchar_t* get_val() {                                          \
      return L##encoding;                                                      \
    }                                                                          \
  };                                                                           \
  template <typename BuffType>                                                 \
  struct log_type_helper<type, BuffType> {                                     \
    inline static bool do_log(                                                 \
        const type& _0, BuffType* _buffer, size_t& _bufferLeft) {              \
      int printed = wn::memory::snprintf(                                      \
          _buffer, _bufferLeft, _enc##type<BuffType>::get_val(), _0);          \
      if (printed < 0 || static_cast<size_t>(printed) >= _bufferLeft) {        \
        return (false);                                                        \
      }                                                                        \
      _bufferLeft -= printed;                                                  \
      return (true);                                                           \
    }                                                                          \
  };

#ifdef _WN_WINDOWS
template <typename T>
struct _enc_wn_char {};
template <>
struct _enc_wn_char<char> {
  static const char* get_val() {
    return "%s";
  }
};
template <>
struct _enc_wn_char<wchar_t> {
  static const wchar_t* get_val() {
    return L"%S";
  }
};
#else
template <typename T>
struct _enc_wn_char {};
template <>
struct _enc_wn_char<char> {
  static const char* get_val() {
    return "%s";
  }
};
template <>
struct _enc_wn_char<wchar_t> {
  static const wchar_t* get_val() {
    return L"%ls";
  }
};
#endif
template <typename BuffType>
struct log_type_helper<char*, BuffType> {
  inline static bool do_log(
      char* const& _0, BuffType* _buffer, size_t& _buffer_left) {
    int printed = wn::memory::snprintf(
        _buffer, _buffer_left, _enc_wn_char<BuffType>::get_val(), _0);
    if (printed < 0 || static_cast<size_t>(printed) >= _buffer_left) {
      return (false);
    }
    _buffer_left -= printed;
    return (true);
  }
};

template <typename BuffType>
struct log_type_helper<const char*, BuffType> {
  inline static bool do_log(
      const char* const& _0, BuffType* _buffer, size_t& _buffer_left) {
    int printed = wn::memory::snprintf(
        _buffer, _buffer_left, _enc_wn_char<BuffType>::get_val(), _0);
    if (printed < 0 || static_cast<size_t>(printed) >= _buffer_left) {
      return (false);
    }
    _buffer_left -= printed;
    return (true);
  }
};

template <size_t N, typename BuffType>
struct log_type_helper<char[N], BuffType> {
  inline static bool do_log(
      const char (&_0)[N], BuffType* _buffer, size_t& _buffer_left) {
    int printed = wn::memory::snprintf(
        _buffer, _buffer_left, _enc_wn_char<BuffType>::get_val(), _0);
    if (printed < 0 || static_cast<size_t>(printed) >= _buffer_left) {
      return (false);
    }
    _buffer_left -= printed;
    return (true);
  }
};

template <typename BuffType>
struct log_type_helper<wn::containers::string_view, BuffType> {
  inline static bool do_log(const wn::containers::string_view& _0,
      BuffType* _buffer, size_t& _buffer_left) {
    int printed = wn::memory::snprintf(
        _buffer, _buffer_left, "%.*s", (uint32_t)_0.size(), (void*)_0.data());
    if (printed < 0 || static_cast<size_t>(printed) >= _buffer_left) {
      return (false);
    }
    _buffer_left -= printed;
    return (true);
  }
};

template <typename BuffType>
struct log_type_helper<wn::containers::string, BuffType> {
  inline static bool do_log(const wn::containers::string& _0, BuffType* _buffer,
      size_t& _buffer_left) {
    int printed = wn::memory::snprintf(
        _buffer, _buffer_left, "%.*s", (uint32_t)_0.size(), (void*)_0.data());
    if (printed < 0 || static_cast<size_t>(printed) >= _buffer_left) {
      return (false);
    }
    _buffer_left -= printed;
    return (true);
  }
};

template <typename T, size_t N, typename BuffType>
struct log_type_helper<wn::containers::dynamic_array<T, N>, BuffType> {
  inline static bool do_log(const wn::containers::dynamic_array<T, N>& _0,
      BuffType* _buffer, size_t& _buffer_left) {
    size_t last_buffer_left = _buffer_left;
    if (!log_type_helper<char[2], BuffType>::do_log(
            "[", _buffer, last_buffer_left)) {
      return false;
    }

    for (size_t i = 0; i < _0.size(); ++i) {
      if (i != 0) {
        if (!log_type_helper<char[3], BuffType>::do_log(", ",
                _buffer + (_buffer_left - last_buffer_left),
                last_buffer_left)) {
          return false;
        }
      }
      if (!log_type_helper<T, BuffType>::do_log(_0[i],
              _buffer + (_buffer_left - last_buffer_left), last_buffer_left)) {
        return false;
      }
    }

    if (!log_type_helper<char[2], BuffType>::do_log("]",
            _buffer + (_buffer_left - last_buffer_left), last_buffer_left)) {
      return false;
    }

    _buffer_left = last_buffer_left;
    return (true);
  }
};

DEFINE_DEFAULT_LOG(int32_t, "%d");
DEFINE_DEFAULT_LOG(uint32_t, "%d");
DEFINE_DEFAULT_LOG(float, "%f");
DEFINE_DEFAULT_LOG(double, "%f");
#ifdef _WN_WINDOWS
DEFINE_DEFAULT_LOG(int64_t, "%I64d");
DEFINE_DEFAULT_LOG(uint64_t, "%I64u");
#else
DEFINE_DEFAULT_LOG(int64_t, "%lld");
DEFINE_DEFAULT_LOG(uint64_t, "%llu");
#endif

}  // namespace logging
}  // namespace wn

#endif  // __WN_LOGGING_DEFAULT_LOG_TYPES_INL__
