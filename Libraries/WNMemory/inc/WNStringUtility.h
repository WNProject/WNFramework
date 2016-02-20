// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MEMORY_STRING_UTILITIES_H__
#define __WN_MEMORY_STRING_UTILITIES_H__

#include "WNCore/inc/WNAssert.h"
#include "WNMemory/inc/WNBasic.h"

#include <algorithm>
#include <cmath>
#include <cstdarg>
#include <cstring>

namespace wn {
namespace memory {

WN_FORCE_INLINE size_t strlen(const char* str) {
  WN_DEBUG_ASSERT_DESC(str, "string must not be nullptr");

  return (std::strlen(str));
}

WN_FORCE_INLINE size_t strnlen(const char* str, const size_t count) {
  WN_DEBUG_ASSERT_DESC(str, "string must not be nullptr");

  size_t size = 0;

  for (; size < count && str[size] != '\0'; ++size) {
  }

  return (size);
}

WN_FORCE_INLINE char* strcpy(char* dest, const char* src) {
  WN_DEBUG_ASSERT_DESC(dest, "destination string must not be nullptr");
  WN_DEBUG_ASSERT_DESC(src, "source string must not be nullptr");
  WN_DEBUG_ASSERT_DESC(
      dest != src, "destination and source must not be the same");

  return (std::strcpy(dest, src));
}

WN_FORCE_INLINE char* strncpy(char* dest, const char* src, const size_t count) {
  WN_DEBUG_ASSERT_DESC(dest, "destination string must not be nullptr");
  WN_DEBUG_ASSERT_DESC(src, "source string must not be nullptr");
  WN_DEBUG_ASSERT_DESC(
      dest != src, "destination and source must not be the same");

  return (std::strncpy(dest, src, count));
}

WN_FORCE_INLINE char* strcat(char* dest, const char* src) {
  WN_DEBUG_ASSERT_DESC(dest, "destination string must not be nullptr");
  WN_DEBUG_ASSERT_DESC(src, "source string must not be nullptr");
  WN_DEBUG_ASSERT_DESC(
      dest != src, "destination and source must not be the same");

  return (std::strcat(dest, src));
}

WN_FORCE_INLINE char* strncat(char* dest, const char* src, const size_t count) {
  WN_DEBUG_ASSERT_DESC(dest, "destination string must not be nullptr");
  WN_DEBUG_ASSERT_DESC(src, "source string must not be nullptr");
  WN_DEBUG_ASSERT_DESC(
      dest != src, "destination and source must not be the same");

  return (std::strncat(dest, src, count));
}

WN_FORCE_INLINE int32_t strcmp(const char* lhs, const char* rhs) {
  WN_DEBUG_ASSERT_DESC(lhs, "string must not be nullptr");
  WN_DEBUG_ASSERT_DESC(rhs, "string must not be nullptr");

  return (std::strcmp(lhs, rhs));
}

WN_FORCE_INLINE int32_t strncmp(
    const char* lhs, const char* rhs, const size_t count) {
  WN_DEBUG_ASSERT_DESC(lhs, "string must not be nullptr");
  WN_DEBUG_ASSERT_DESC(rhs, "string must not be nullptr");

  return (std::strncmp(lhs, rhs, count));
}

WN_FORCE_INLINE char* strdup(const char* str) {
  WN_DEBUG_ASSERT_DESC(str, "string must not be nullptr");

  const size_t size = strlen(str);
  char* copy = heap_allocate<char>(size + 1);

  if (copy) {
    if (size > 0) {
      memcpy(copy, str, size);
    }

    copy[size] = '\0';
  }

  return (copy);
}

WN_FORCE_INLINE char* strndup(const char* str, const size_t count) {
  WN_DEBUG_ASSERT_DESC(str, "string must not be nullptr");

  const size_t size = strnlen(str, count);
  char* copy = heap_allocate<char>(size + 1);

  if (copy) {
    if (size > 0) {
      memcpy(copy, str, size);
    }

    copy[size] = '\0';
  }

  return (copy);
}

WN_FORCE_INLINE char* strchr(char* str, const char c) {
  WN_DEBUG_ASSERT_DESC(str, "string must not be nullptr");

  return (std::strchr(str, c));
}

WN_FORCE_INLINE const char* strchr(const char* str, const char c) {
  WN_DEBUG_ASSERT_DESC(str, "string must not be nullptr");

  return (std::strchr(str, c));
}

WN_FORCE_INLINE char* strnchr(char* str, const char c, const size_t count) {
  WN_DEBUG_ASSERT_DESC(str, "string must not be nullptr");
  for (size_t i = 0; i < count; ++i) {
    if (str[i] == '\0') {
      return nullptr;
    } else if (str[i] == c) {
      return &str[i];
    }
  }
  return nullptr;
}

WN_FORCE_INLINE const char* strnchr(
    const char* str, const char c, const size_t count) {
  WN_DEBUG_ASSERT_DESC(str, "string must not be nullptr");
  for (size_t i = 0; i < count; ++i) {
    if (str[i] == '\0') {
      return nullptr;
    } else if (str[i] == c) {
      return &str[i];
    }
  }
  return nullptr;
}

WN_FORCE_INLINE char* strstr(char* lhs, const char* rhs) {
  WN_DEBUG_ASSERT_DESC(lhs, "string must not be nullptr");
  WN_DEBUG_ASSERT_DESC(rhs, "string must not be nullptr");

  return (std::strstr(lhs, rhs));
}

WN_FORCE_INLINE const char* strstr(const char* lhs, const char* rhs) {
  WN_DEBUG_ASSERT_DESC(lhs, "string must not be nullptr");
  WN_DEBUG_ASSERT_DESC(rhs, "string must not be nullptr");

  return (std::strstr(lhs, rhs));
}

WN_FORCE_INLINE size_t strnhash(const char* _str, size_t _length) {
  WN_DEBUG_ASSERT_DESC(_str, "string must not be nullptr");
  size_t hash = 0;
  for (size_t i = 0; i < _length && _str[i] != '\0'; ++i) {
    hash = _str[i] + (hash << 6) + (hash << 16) - hash;
  }

  return (hash);
}

WN_FORCE_INLINE size_t strhash(const char* str) {
  WN_DEBUG_ASSERT_DESC(str, "string must not be nullptr");

  size_t hash = 0;
  size_t c;

  while ((c = static_cast<size_t>(*(str++))) != 0) {
    hash = c + (hash << 6) + (hash << 16) - hash;
  }

  return (hash);
}

WN_FORCE_INLINE char* strlwr(char* str) {
  WN_DEBUG_ASSERT_DESC(str, "string must not be nullptr");

  std::transform(str, str + strlen(str), str, ::tolower);

  return (str);
}

WN_FORCE_INLINE char* strnlwr(char* str, const size_t count) {
  WN_DEBUG_ASSERT_DESC(str, "string must not be nullptr");

  std::transform(str, str + strnlen(str, count), str, ::tolower);

  return (str);
}

WN_FORCE_INLINE char* strupr(char* str) {
  WN_DEBUG_ASSERT_DESC(str, "string must not be nullptr");

  std::transform(str, str + strlen(str), str, ::toupper);

  return (str);
}

WN_FORCE_INLINE char* strnupr(char* str, const size_t count) {
  WN_DEBUG_ASSERT_DESC(str, "string must not be nullptr");

  std::transform(str, str + strnlen(str, count), str, ::toupper);

  return (str);
}

WN_INLINE int32_t snprintf(
    char* buffer, const size_t count, const char* format, ...) {
  va_list argptr;
  va_start(argptr, format);

#ifdef _WN_WINDOWS
  int32_t ret_val = ::_vsnprintf(buffer, count, format, argptr);
#else
  int32_t ret_val = ::vsnprintf(buffer, count, format, argptr);
#endif
  va_end(argptr);
  return ret_val;
}

WN_FORCE_INLINE size_t writeuint32(
    char* _buffer, uint32_t _int, size_t _maxSize) {
  WN_RELEASE_ASSERT_DESC(_maxSize > 0,
      "Must have at least room for 1 character to write out an integer");

  size_t counter = 0;

  for (;;) {
    if (counter >= _maxSize) {
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

WN_FORCE_INLINE size_t writeint32(
    char* _buffer, int32_t _int, const size_t _maxSize) {
  WN_RELEASE_ASSERT_DESC(_maxSize > 0,
      "Must have at least 1 characters of space for a signed int");

  size_t counter = 0;

  if (_int < 0) {
    _buffer[counter] = '-';
    _int *= -1;
    _buffer++;
    counter++;
  }

  const size_t actualInt =
      writeuint32(_buffer, static_cast<uint32_t>(_int), _maxSize - counter);

  if (actualInt == 0) {
    return (0);
  }

  counter += actualInt;

  return (counter);
}

WN_FORCE_INLINE size_t WNWriteSimpleFloat32(
    char* _buffer, float _flt, size_t _maxSize) {
  WN_RELEASE_ASSERT_DESC(_maxSize > 0, "Can not fill an empty buffer");
  if (_flt == 0.f) {
    _buffer[0] = '0';
    return (1);
  }
  float absFloat = _flt > 0.f ? _flt : -_flt;
  WN_RELEASE_ASSERT_DESC(absFloat < 10000000.f && absFloat > 0.0001,
      "Floating point writing should be limited");
  size_t numsLeft = 7;
  bool losingprecision = false;
  size_t counter = 0;
  if (_flt < 0) {
    _buffer[counter++] = '-';
  }

  uint32_t wholePart = static_cast<uint32_t>(absFloat - fmodf(absFloat, 1.0f));
  size_t wholeChars = static_cast<size_t>(
      writeuint32(_buffer + counter, wholePart, _maxSize - counter));
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

  if (counter >= _maxSize) {
    return (0);
  }
  _buffer[counter++] = '.';
  while (floatPart > 0.f && numsLeft > 0) {
    if (counter > _maxSize) {
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

WN_FORCE_INLINE size_t WNWriteFloat32(
    char* _buffer, float _flt, size_t _maxSize) {
  WN_RELEASE_ASSERT_DESC(_maxSize > 0, "Can not fill an empty buffer");
  if (fabs(_flt) < 10000000.f && fabs(_flt) > 0.0001) {
    return (WNWriteSimpleFloat32(_buffer, _flt, _maxSize));
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

  if (counter >= _maxSize) {
    return (0);
  }

  uint8_t wholePart = static_cast<uint8_t>(base - fmodf(base, 1.0f));
  base = base - static_cast<float>(wholePart);
  _buffer[counter++] = '0' + wholePart;

  if (counter >= _maxSize) {
    return (0);
  }
  _buffer[counter++] = '.';

  // We lose all of our float precision after 7 decimal numbers
  for (int j = 0; j < 7; ++j) {
    if (counter >= _maxSize) {
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

  if (counter > _maxSize - 2) {
    return (0);
  }
  _buffer[counter++] = 'e';
  if (negExp) {
    _buffer[counter++] = '-';
  }
  size_t expPart = writeuint32(_buffer + counter, exp, _maxSize - counter);
  if (expPart == 0) {
    return (0);
  }
  counter += expPart;
  return (counter);
}

WN_FORCE_INLINE size_t writeuint64(
    char* _buffer, uint64_t _int, size_t _maxSize) {
  WN_RELEASE_ASSERT_DESC(_maxSize > 0,
      "Must have at least room for 1 character to write out an integer");

  size_t counter = 0;

  for (;;) {
    if (counter >= _maxSize) {
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

WN_FORCE_INLINE size_t writeint64(
    char* _buffer, int64_t _int, size_t _maxSize) {
  WN_RELEASE_ASSERT_DESC(_maxSize > 0,
      "Must have at least 1 characters of space for a signed int");

  size_t counter = 0;

  if (_int < 0) {
    _buffer[counter] = '-';
    _int *= -1;
    _buffer++;
    counter++;
  }

  const size_t actualInt =
      writeuint64(_buffer, static_cast<uint32_t>(_int), _maxSize - counter);

  if (actualInt == 0) {
    return (0);
  }

  counter += actualInt;

  return (counter);
}

WN_FORCE_INLINE size_t WNReadUInt32(
    char* _buff, uint32_t& _number, size_t _maxSize) {
  WN_RELEASE_ASSERT_DESC(
      _maxSize > 0, "You cannot store a number in less than 1 byte");

  size_t counter = 0;

  _number = 0;

  while (_buff[counter] >= '0' && _buff[counter] <= '9' && counter < _maxSize) {
    _number = _number * 10;
    _number = _number + _buff[counter++] - '0';
  }

  return (counter);
}

WN_FORCE_INLINE size_t WNReadInt32(
    char* _buffer, int32_t& _number, size_t _maxSize) {
  WN_RELEASE_ASSERT_DESC(
      _maxSize > 0, "You cannot store a number in less than 1 byte");

  size_t counter = 0;
  bool negative = false;

  if (_buffer[counter] == '-') {
    negative = true;
    counter++;
  }

  uint32_t num = 0;
  const size_t readNum = WNReadUInt32(_buffer + counter, num, _maxSize);

  if (readNum == 0) {
    return (0);
  }

  counter += readNum;
  _number = num * (negative ? -1 : 1);

  return (counter);
}

WN_FORCE_INLINE size_t WNReadFloat32(
    char* _buff, float& _number, size_t _maxSize) {
  WN_RELEASE_ASSERT_DESC(
      _maxSize > 0, "You cannot store a number in less than 1 byte");
  const float LOG10_LOG2 = 3.321928095f;
  size_t counter = 0;
  bool negative = false;
  uint32_t wholePart = 0;
  if (_buff[counter] == '-') {
    counter++;
    negative = true;
  }
  while (_buff[counter] >= '0' && _buff[counter] <= '9' && counter < _maxSize) {
    wholePart = wholePart * 10;
    wholePart = wholePart + _buff[counter++] - '0';
  }

  if (counter >= _maxSize || _buff[counter] != '.') {
    _number = static_cast<float>(wholePart) * (negative ? -1.0f : 1.0f);
    return (counter);
  }
  counter++;
  float floatPart = 0;
  float floatMult = 0.1f;
  while (_buff[counter] >= '0' && _buff[counter] <= '9' && counter < _maxSize) {
    floatPart =
        floatPart + static_cast<float>(_buff[counter++] - '0') * floatMult;
    floatMult /= 10.0f;
  }
  if (counter >= _maxSize || _buff[counter] != 'e') {
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

  while (_buff[counter] >= '0' && _buff[counter] <= '9' && counter < _maxSize) {
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

WN_FORCE_INLINE size_t WNReadUInt64(
    char* _buff, uint64_t& _number, size_t _maxSize) {
  WN_RELEASE_ASSERT_DESC(
      _maxSize > 0, "You cannot store a number in less than 1 byte");

  size_t counter = 0;

  _number = 0;

  while (_buff[counter] >= '0' && _buff[counter] <= '9') {
    _number = _number * 10;
    _number = _number + _buff[counter++] - '0';
  }

  return (counter);
}

WN_FORCE_INLINE size_t WNReadInt64(
    char* _buffer, int64_t& _number, size_t _maxSize) {
  WN_RELEASE_ASSERT_DESC(
      _maxSize > 0, "You cannot store a number in less than 1 byte");

  size_t counter = 0;
  bool negative = false;

  if (_buffer[counter] == '-') {
    negative = true;
    counter++;
  }

  uint64_t num = 0;
  const size_t readNum = WNReadUInt64(_buffer + counter, num, _maxSize);

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