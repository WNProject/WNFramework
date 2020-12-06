// Copyright (_c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MEMORY_STRING_UTILITIES_H__
#define __WN_MEMORY_STRING_UTILITIES_H__

#include "WNMemory/inc/allocation.h"
#include "WNMemory/inc/manipulation.h"
#include "core/inc/assert.h"

#include <algorithm>
#include <cmath>
#include <cstdarg>
#include <cstring>

namespace wn {
namespace memory {

inline size_t strlen(const char* _str) {
  WN_DEBUG_ASSERT(_str, "string must not be nullptr");

  return std::strlen(_str);
}

inline size_t strnlen(const char* _str, size_t _count) {
  WN_DEBUG_ASSERT(_str, "string must not be nullptr");

  size_t size = 0;

  for (; size < _count && _str[size] != '\0'; ++size) {
    continue;
  }

  return size;
}

inline char* strcpy(char* _dest, const char* _src) {
  WN_DEBUG_ASSERT(_dest, "destination string must not be nullptr");
  WN_DEBUG_ASSERT(_src, "source string must not be nullptr");
  WN_DEBUG_ASSERT(_dest != _src, "destination and source must not be the same");

  return std::strcpy(_dest, _src);
}

inline char* strncpy(char* _dest, const char* _src, size_t _count) {
  WN_DEBUG_ASSERT(_dest, "destination string must not be nullptr");
  WN_DEBUG_ASSERT(_src, "source string must not be nullptr");
  WN_DEBUG_ASSERT(_dest != _src, "destination and source must not be the same");

  return std::strncpy(_dest, _src, _count);
}

inline char* strcat(char* _dest, const char* _src) {
  WN_DEBUG_ASSERT(_dest, "destination string must not be nullptr");
  WN_DEBUG_ASSERT(_src, "source string must not be nullptr");
  WN_DEBUG_ASSERT(_dest != _src, "destination and source must not be the same");

  return std::strcat(_dest, _src);
}

inline char* strncat(char* _dest, const char* _src, size_t _count) {
  WN_DEBUG_ASSERT(_dest, "destination string must not be nullptr");
  WN_DEBUG_ASSERT(_src, "source string must not be nullptr");
  WN_DEBUG_ASSERT(_dest != _src, "destination and source must not be the same");

  return std::strncat(_dest, _src, _count);
}

inline int32_t strcmp(const char* _lhs, const char* _rhs) {
  WN_DEBUG_ASSERT(_lhs, "string must not be nullptr");
  WN_DEBUG_ASSERT(_rhs, "string must not be nullptr");

  return std::strcmp(_lhs, _rhs);
}

inline int32_t strncmp(const char* _lhs, const char* _rhs, size_t _count) {
  WN_DEBUG_ASSERT(_lhs, "string must not be nullptr");
  WN_DEBUG_ASSERT(_rhs, "string must not be nullptr");

  return std::strncmp(_lhs, _rhs, _count);
}

inline char* strdup(const char* _str) {
  WN_DEBUG_ASSERT(_str, "string must not be nullptr");

  const size_t size = strlen(_str);
  char* copy = heap_allocate<char>(size + 1);

  if (copy) {
    if (size > 0) {
      memcpy(copy, _str, size);
    }

    copy[size] = '\0';
  }

  return copy;
}

inline char* strndup(const char* _str, size_t _count) {
  WN_DEBUG_ASSERT(_str, "string must not be nullptr");

  const size_t size = strnlen(_str, _count);
  char* copy = heap_allocate<char>(size + 1);

  if (copy) {
    if (size > 0) {
      memcpy(copy, _str, size);
    }

    copy[size] = '\0';
  }

  return copy;
}

inline char* strchr(char* _str, char _c) {
  WN_DEBUG_ASSERT(_str, "string must not be nullptr");

  return std::strchr(_str, _c);
}

inline const char* strchr(const char* _str, char _c) {
  WN_DEBUG_ASSERT(_str, "string must not be nullptr");

  return std::strchr(_str, _c);
}

inline char* strnchr(char* _str, char _c, size_t _count) {
  WN_DEBUG_ASSERT(_str, "string must not be nullptr");

  for (size_t i = 0; i < _count; ++i) {
    if (_str[i] == '\0') {
      return nullptr;
    } else if (_str[i] == _c) {
      return &_str[i];
    }
  }

  return nullptr;
}

inline const char* strnchr(const char* _str, char _c, size_t _count) {
  WN_DEBUG_ASSERT(_str, "string must not be nullptr");

  for (size_t i = 0; i < _count; ++i) {
    if (_str[i] == '\0') {
      return nullptr;
    } else if (_str[i] == _c) {
      return &_str[i];
    }
  }

  return nullptr;
}

inline char* strstr(char* _lhs, const char* _rhs) {
  WN_DEBUG_ASSERT(_lhs, "string must not be nullptr");
  WN_DEBUG_ASSERT(_rhs, "string must not be nullptr");

  return std::strstr(_lhs, _rhs);
}

inline const char* strstr(const char* _lhs, const char* _rhs) {
  WN_DEBUG_ASSERT(_lhs, "string must not be nullptr");
  WN_DEBUG_ASSERT(_rhs, "string must not be nullptr");

  return std::strstr(_lhs, _rhs);
}

inline size_t strnhash(const char* _str, size_t _length) {
  WN_DEBUG_ASSERT(_str, "string must not be nullptr");

  size_t hash = 0;

  for (size_t i = 0; i < _length && _str[i] != '\0'; ++i) {
    hash = _str[i] + (hash << 6) + (hash << 16) - hash;
  }

  return hash;
}

inline size_t strhash(const char* _str) {
  WN_DEBUG_ASSERT(_str, "string must not be nullptr");

  size_t hash = 0;
  size_t c;

  while ((c = static_cast<size_t>(*(_str++))) != 0) {
    hash = c + (hash << 6) + (hash << 16) - hash;
  }

  return hash;
}

inline char lower(char _c) {
  return static_cast<char>(::tolower(static_cast<int>(_c)));
}

inline char upper(char _c) {
  return static_cast<char>(::toupper(static_cast<int>(_c)));
}

inline char* strlwr(char* _str) {
  WN_DEBUG_ASSERT(_str, "string must not be nullptr");

  std::transform(_str, _str + strlen(_str), _str, &lower);

  return (_str);
}

inline char* strnlwr(char* _str, size_t _count) {
  WN_DEBUG_ASSERT(_str, "string must not be nullptr");

  std::transform(_str, _str + strnlen(_str, _count), _str, &lower);

  return (_str);
}

inline char* strupr(char* _str) {
  WN_DEBUG_ASSERT(_str, "string must not be nullptr");

  std::transform(_str, _str + strlen(_str), _str, &upper);

  return (_str);
}

inline char* strnupr(char* _str, size_t _count) {
  WN_DEBUG_ASSERT(_str, "string must not be nullptr");

  std::transform(_str, _str + strnlen(_str, _count), _str, &upper);

  return (_str);
}

inline int32_t snprintf(char* buffer, size_t _count, const char* _format, ...) {
  va_list argptr;
  va_start(argptr, _format);

#ifdef _WN_WINDOWS
  const int32_t ret_val = ::_vsnprintf(buffer, _count, _format, argptr);
#else
  const int32_t ret_val = ::vsnprintf(buffer, _count, _format, argptr);
#endif
  va_end(argptr);

  return ret_val;
}

inline size_t writeuint32(char* _buffer, uint32_t _int, size_t _max_size) {
  WN_RELEASE_ASSERT(_max_size > 0,
      "Must have at least room for 1 character to write out an integer");

  size_t counter = 0;

  for (;;) {
    if (counter >= _max_size) {
      return (0);
    }

    const int8_t test = _int % 10;

    _int -= test;
    _int /= 10;
    _buffer[counter++] = '0' + test;

    if (_int == 0) {
      break;
    }
  }
  if (counter < _max_size) {
    _buffer[counter] = '\0';
  }

  // Right now the integer is in swapped order, fix that
  for (size_t i = 0; i < counter / 2; ++i) {
    _buffer[i] ^= _buffer[counter - i - 1];
    _buffer[counter - i - 1] ^= _buffer[i];
    _buffer[i] ^= _buffer[counter - i - 1];
  }

  return (counter);
}

inline size_t writeint32(char* _buffer, int32_t _int, const size_t _max_size) {
  WN_RELEASE_ASSERT(_max_size > 0,
      "Must have at least 1 characters of space for a signed int");

  size_t counter = 0;

  if (_int < 0) {
    _buffer[counter] = '-';
    _int *= -1;
    _buffer++;
    counter++;
  }

  const size_t actualInt =
      writeuint32(_buffer, static_cast<uint32_t>(_int), _max_size - counter);

  if (actualInt == 0) {
    return (0);
  }

  counter += actualInt;

  return (counter);
}

inline size_t WNWriteSimpleFloat32(
    char* _buffer, float _flt, size_t _max_size) {
  WN_RELEASE_ASSERT(_max_size > 0, "Can not fill an empty buffer");
  if (_flt == 0.f) {
    _buffer[0] = '0';
    return (1);
  }
  float absFloat = _flt > 0.f ? _flt : -_flt;
  WN_RELEASE_ASSERT(absFloat < 10000000.f && absFloat > 0.0001,
      "Floating point writing should be limited");
  size_t numsLeft = 7;
  bool losingprecision = false;
  size_t counter = 0;
  if (_flt < 0) {
    _buffer[counter++] = '-';
  }

  uint32_t wholePart = static_cast<uint32_t>(absFloat - fmodf(absFloat, 1.0f));
  size_t wholeChars = static_cast<size_t>(
      writeuint32(_buffer + counter, wholePart, _max_size - counter));
  if (wholeChars == 0) {
    return (0);
  }
  counter += wholeChars;
  if (wholePart != 0) {
    losingprecision = true;
    numsLeft -= wholeChars;
  }
  float floatPart = fmodf(absFloat, 1.0f);
  if (floatPart == 0.f || numsLeft <= 0) {
    return (counter);
  }

  if (counter >= _max_size) {
    return (0);
  }
  _buffer[counter++] = '.';
  while (floatPart > 0.f && numsLeft > 0) {
    if (counter > _max_size) {
      return (0);
    }
    floatPart *= 10.0f;
    uint8_t bPart = static_cast<uint8_t>(floatPart - fmodf(floatPart, 1.0f));
    floatPart = fmodf(floatPart, 1.0f);
    _buffer[counter++] = '0' + bPart;
    if (bPart != 0) {
      losingprecision = true;
    }
    if (losingprecision) {
      numsLeft -= 1;
    }
  }
  return (counter);
}

inline size_t WNWriteFloat32(char* _buffer, float _flt, size_t _max_size) {
  WN_RELEASE_ASSERT(_max_size > 0, "Can not fill an empty buffer");
  if (fabs(_flt) < 10000000.f && fabs(_flt) > 0.0001) {
    return (WNWriteSimpleFloat32(_buffer, _flt, _max_size));
  }

  const float LOG10_LOG2 = 3.321928095f;
  int32_t exp = 0;
  bool neg = (_flt < 0.0f);
  _flt *= (neg ? -1.0f : 1.0f);
  float base = frexpf(_flt, &exp);
  bool negExp = (exp < 0);
  exp *= (negExp ? -1 : 1);
  float exp10 = static_cast<float>(exp) / LOG10_LOG2;
  float multiplier = fmodf(exp10, 1.0f) * (negExp ? -1.0f : 1.0f);
  base *= powf(10.0f, multiplier);
  exp = static_cast<uint32_t>(exp10 - fmodf(exp10, 1.0));
  int32_t dir = (base > 10.f ? -1 : (base < 1.0f ? 1 : 0));

  base *= (dir > 0 ? 10.f : (dir < 0 ? 0.1f : 1.f));

  dir *= negExp ? -1 : 1;
  exp -= dir;

  if (exp < 0) {
    exp *= -1;
    negExp = !negExp;
  }

  if (exp == 0) {
    negExp = false;
  }
  size_t counter = 0;
  if (neg) {
    _buffer[counter++] = '-';
  }

  if (counter >= _max_size) {
    return (0);
  }

  uint8_t wholePart = static_cast<uint8_t>(base - fmodf(base, 1.0f));
  base = base - static_cast<float>(wholePart);
  _buffer[counter++] = '0' + wholePart;

  if (counter >= _max_size) {
    return (0);
  }
  _buffer[counter++] = '.';

  // We lose all of our float precision after 7 decimal numbers
  for (int j = 0; j < 7; ++j) {
    if (counter >= _max_size) {
      return (0);
    }
    if (base == 0.f) {
      break;
    }
    base = base * 10.0f;
    wholePart = static_cast<uint8_t>(base - fmodf(base, 1.0f));
    base = fmodf(base, 1.0f);
    _buffer[counter++] = '0' + wholePart;
  }
  if (exp == 0) {
    return (counter);
  }

  if (counter > _max_size - 2) {
    return (0);
  }
  _buffer[counter++] = 'e';
  if (negExp) {
    _buffer[counter++] = '-';
  }
  size_t expPart = writeuint32(_buffer + counter, exp, _max_size - counter);
  if (expPart == 0) {
    return (0);
  }
  counter += expPart;
  return (counter);
}

inline size_t writeuint64(char* _buffer, uint64_t _int, size_t _max_size) {
  WN_RELEASE_ASSERT(_max_size > 0,
      "Must have at least room for 1 character to write out an integer");

  size_t counter = 0;

  for (;;) {
    if (counter >= _max_size) {
      return (0);
    }

    const int8_t test = _int % 10;

    _int -= test;
    _int /= 10;
    _buffer[counter++] = '0' + test;

    if (_int == 0) {
      break;
    }
  }

  // Right now the integer is in swapped order, fix that
  for (size_t i = 0; i < counter / 2; ++i) {
    _buffer[i] ^= _buffer[counter - i - 1];
    _buffer[counter - i - 1] ^= _buffer[i];
    _buffer[i] ^= _buffer[counter - i - 1];
  }

  return (counter);
}

inline size_t writeint64(char* _buffer, int64_t _int, size_t _max_size) {
  WN_RELEASE_ASSERT(_max_size > 0,
      "Must have at least 1 characters of space for a signed int");

  size_t counter = 0;

  if (_int < 0) {
    _buffer[counter] = '-';
    _int *= -1;
    _buffer++;
    counter++;
  }

  const size_t actualInt =
      writeuint64(_buffer, static_cast<uint32_t>(_int), _max_size - counter);

  if (actualInt == 0) {
    return (0);
  }

  counter += actualInt;

  return (counter);
}

inline size_t readuint32(
    const char* _buff, uint32_t& _number, size_t _max_size) {
  WN_RELEASE_ASSERT(
      _max_size > 0, "You cannot store a number in less than 1 byte");

  size_t counter = 0;

  _number = 0;

  while (
      _buff[counter] >= '0' && _buff[counter] <= '9' && counter < _max_size) {
    _number = _number * 10;
    _number = _number + _buff[counter++] - '0';
  }

  return (counter);
}

inline size_t readint32(char* _buffer, int32_t& _number, size_t _max_size) {
  WN_RELEASE_ASSERT(
      _max_size > 0, "You cannot store a number in less than 1 byte");

  size_t counter = 0;
  bool negative = false;

  if (_buffer[counter] == '-') {
    negative = true;
    counter++;
  }

  uint32_t num = 0;
  const size_t readNum = readuint32(_buffer + counter, num, _max_size);

  if (readNum == 0) {
    return (0);
  }

  counter += readNum;
  _number = num * (negative ? -1 : 1);

  return (counter);
}

inline size_t WNReadFloat32(char* _buff, float& _number, size_t _max_size) {
  WN_RELEASE_ASSERT(
      _max_size > 0, "You cannot store a number in less than 1 byte");
  const float LOG10_LOG2 = 3.321928095f;
  size_t counter = 0;
  bool negative = false;
  uint32_t wholePart = 0;
  if (_buff[counter] == '-') {
    counter++;
    negative = true;
  }
  while (
      _buff[counter] >= '0' && _buff[counter] <= '9' && counter < _max_size) {
    wholePart = wholePart * 10;
    wholePart = wholePart + _buff[counter++] - '0';
  }

  if (counter >= _max_size || _buff[counter] != '.') {
    _number = static_cast<float>(wholePart) * (negative ? -1.0f : 1.0f);
    return (counter);
  }
  counter++;
  float floatPart = 0;
  float floatMult = 0.1f;
  while (
      _buff[counter] >= '0' && _buff[counter] <= '9' && counter < _max_size) {
    floatPart =
        floatPart + static_cast<float>(_buff[counter++] - '0') * floatMult;
    floatMult /= 10.0f;
  }
  if (counter >= _max_size || _buff[counter] != 'e') {
    _number =
        (static_cast<float>(wholePart) + floatPart) * (negative ? -1.0f : 1.0f);
    return (counter);
  }
  counter++;
  bool negExp = false;
  int32_t exp = 0;
  if (_buff[counter] == '-') {
    negExp = true;
  }

  while (
      _buff[counter] >= '0' && _buff[counter] <= '9' && counter < _max_size) {
    exp = exp * 10;
    exp = exp + _buff[counter++] - '0';
  }

  exp = exp * (negExp ? -1 : 1);
  while (wholePart > 0) {
    uint8_t lastDigit = wholePart % 10;
    floatPart /= 10.0f;
    floatPart += float(lastDigit) / 10.0f;
    exp += 1;
    wholePart -= lastDigit;
    wholePart /= 10;
  }
  negExp = exp < 0;
  exp = exp * (negExp ? -1 : 1);

  float exp2 = exp * LOG10_LOG2;
  exp = static_cast<int32_t>(exp2 - fmodf(exp2, 1.0f));
  exp2 = fmodf(exp2, 1.0f);
  floatPart *= powf(2.0f, exp2);
  exp = exp * (negExp ? -1 : 1);
  _number = ldexpf(floatPart, exp);
  return (counter);
}

inline size_t WNReadUInt64(char* _buff, uint64_t& _number, size_t _max_size) {
  WN_RELEASE_ASSERT(
      _max_size > 0, "You cannot store a number in less than 1 byte");

  size_t counter = 0;

  _number = 0;

  while (_buff[counter] >= '0' && _buff[counter] <= '9') {
    _number = _number * 10;
    _number = _number + _buff[counter++] - '0';
  }

  return (counter);
}

inline size_t WNReadInt64(char* _buffer, int64_t& _number, size_t _max_size) {
  WN_RELEASE_ASSERT(
      _max_size > 0, "You cannot store a number in less than 1 byte");

  size_t counter = 0;
  bool negative = false;

  if (_buffer[counter] == '-') {
    negative = true;
    counter++;
  }

  uint64_t num = 0;
  const size_t readNum = WNReadUInt64(_buffer + counter, num, _max_size);

  if (readNum == 0) {
    return (0);
  }

  counter += readNum;
  _number = num * (negative ? -1 : 1);

  return (counter);
}

}  // namespace memory
}  // namespace wn

#endif  // __WN_MEMORY_STRING_UTILITIES_H__
