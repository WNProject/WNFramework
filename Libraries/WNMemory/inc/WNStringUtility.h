// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMORY_STRING_UTILITIES_H__
#define __WN_MEMORY_STRING_UTILITIES_H__

#include "WNCore/inc/WNAssert.h"
#include "WNMemory/inc/WNBasic.h"

#include <algorithm>
#include <cstring>
#include <cstdarg>
#include <cmath>

namespace wn {
namespace memory {

WN_FORCE_INLINE wn_size_t strlen(const wn_char* str) {
  WN_DEBUG_ASSERT_DESC(str, "string must not be nullptr");

  return(std::strlen(str));
}

WN_FORCE_INLINE wn_size_t strnlen(const wn_char* str, const wn_size_t count) {
  WN_DEBUG_ASSERT_DESC(str, "string must not be nullptr");

  wn_size_t size = 0;

  for (; size < count && str[size] != '\0'; ++size) {}

  return(size);
}

WN_FORCE_INLINE wn_char* strcpy(wn_char* dest, const wn_char* src) {
  WN_DEBUG_ASSERT_DESC(dest, "destination string must not be nullptr");
  WN_DEBUG_ASSERT_DESC(src, "source string must not be nullptr");
  WN_DEBUG_ASSERT_DESC(dest != src,
    "destination and source must not be the same");

  return(std::strcpy(dest, src));
}

WN_FORCE_INLINE wn_char* strncpy(wn_char* dest, const wn_char* src,
                                 const wn_size_t count) {
  WN_DEBUG_ASSERT_DESC(dest, "destination string must not be nullptr");
  WN_DEBUG_ASSERT_DESC(src, "source string must not be nullptr");
  WN_DEBUG_ASSERT_DESC(dest != src,
    "destination and source must not be the same");

  return(std::strncpy(dest, src, count));
}

WN_FORCE_INLINE wn_char* strcat(wn_char* dest, const wn_char* src) {
  WN_DEBUG_ASSERT_DESC(dest, "destination string must not be nullptr");
  WN_DEBUG_ASSERT_DESC(src, "source string must not be nullptr");
  WN_DEBUG_ASSERT_DESC(dest != src,
    "destination and source must not be the same");

  return(std::strcat(dest, src));
}

WN_FORCE_INLINE wn_char* strncat(wn_char* dest, const wn_char* src,
                                 const wn_size_t count) {
  WN_DEBUG_ASSERT_DESC(dest, "destination string must not be nullptr");
  WN_DEBUG_ASSERT_DESC(src, "source string must not be nullptr");
  WN_DEBUG_ASSERT_DESC(dest != src,
    "destination and source must not be the same");

  return(std::strncat(dest, src, count));
}

WN_FORCE_INLINE wn_int32 strcmp(const wn_char* lhs, const wn_char* rhs) {
  WN_DEBUG_ASSERT_DESC(lhs, "string must not be nullptr");
  WN_DEBUG_ASSERT_DESC(rhs, "string must not be nullptr");

  return(std::strcmp(lhs, rhs));
}

WN_FORCE_INLINE wn_int32 strncmp(const wn_char* lhs, const wn_char* rhs,
                                  const wn_size_t count) {
  WN_DEBUG_ASSERT_DESC(lhs, "string must not be nullptr");
  WN_DEBUG_ASSERT_DESC(rhs, "string must not be nullptr");

  return(std::strncmp(lhs, rhs, count));
}

WN_FORCE_INLINE wn_char* strdup(const wn_char* str) {
  WN_DEBUG_ASSERT_DESC(str, "string must not be nullptr");

  const wn_size_t size = strlen(str);
  wn_char* copy = heap_allocate<wn_char>(size + 1);

  if (copy) {
    if (size > 0) {
      memcpy(copy, str, size);
    }

    copy[size] = '\0';
  }

  return(copy);
}

WN_FORCE_INLINE wn_char* strndup(const wn_char* str, const wn_size_t count) {
  WN_DEBUG_ASSERT_DESC(str, "string must not be nullptr");

  const wn_size_t size = strnlen(str, count);
  wn_char* copy = heap_allocate<wn_char>(size + 1);

  if (copy) {
    if (size > 0) {
      memcpy(copy, str, size);
    }

    copy[size] = '\0';
  }

  return(copy);
}

WN_FORCE_INLINE wn_char* strchr(wn_char* str, const wn_char c) {
  WN_DEBUG_ASSERT_DESC(str, "string must not be nullptr");

  return(std::strchr(str, c));
}

WN_FORCE_INLINE const wn_char* strchr(const wn_char* str, const wn_char c) {
  WN_DEBUG_ASSERT_DESC(str, "string must not be nullptr");

  return(std::strchr(str, c));
}

WN_FORCE_INLINE wn_char* strnchr(wn_char* str, const wn_char c,
                                 const wn_size_t count) {
  WN_DEBUG_ASSERT_DESC(str, "string must not be nullptr");

  return(static_cast<wn_char*>(std::memchr(str, c, count)));
}

WN_FORCE_INLINE const wn_char* strnchr(const wn_char* str, const wn_char c,
                                       const wn_size_t count) {
  WN_DEBUG_ASSERT_DESC(str, "string must not be nullptr");

  return(static_cast<const wn_char*>(std::memchr(str, c, count)));
}

WN_FORCE_INLINE wn_char* strstr(wn_char* lhs, const wn_char* rhs) {
  WN_DEBUG_ASSERT_DESC(lhs, "string must not be nullptr");
  WN_DEBUG_ASSERT_DESC(rhs, "string must not be nullptr");

  return(std::strstr(lhs, rhs));
}

WN_FORCE_INLINE const wn_char* strstr(const wn_char* lhs, const wn_char* rhs) {
  WN_DEBUG_ASSERT_DESC(lhs, "string must not be nullptr");
  WN_DEBUG_ASSERT_DESC(rhs, "string must not be nullptr");

  return(std::strstr(lhs, rhs));
}

WN_FORCE_INLINE wn_size_t strhash(const wn_char* str) {
  WN_DEBUG_ASSERT_DESC(str, "string must not be nullptr");

  wn_size_t hash = 0;
  wn_size_t c;

  while ((c = static_cast<wn_size_t>(*(str++))) != 0) {
    hash = c + (hash << 6) + (hash << 16) - hash;
  }

  return(hash);
}

WN_FORCE_INLINE wn_char* strlwr(wn_char* str) {
  WN_DEBUG_ASSERT_DESC(str, "string must not be nullptr");

  std::transform(str, str + strlen(str), str, ::tolower);

  return(str);
}

WN_FORCE_INLINE wn_char* strnlwr(wn_char* str, const wn_size_t count) {
  WN_DEBUG_ASSERT_DESC(str, "string must not be nullptr");

  std::transform(str, str + strnlen(str, count), str, ::tolower);

  return(str);
}

WN_FORCE_INLINE wn_char* strupr(wn_char* str) {
  WN_DEBUG_ASSERT_DESC(str, "string must not be nullptr");

  std::transform(str, str + strlen(str), str, ::toupper);

  return(str);
}

WN_FORCE_INLINE wn_char* strnupr(wn_char* str, const wn_size_t count) {
  WN_DEBUG_ASSERT_DESC(str, "string must not be nullptr");

  std::transform(str, str + strnlen(str, count), str, ::toupper);

  return(str);
}

WN_INLINE wn_int32 snprintf(wn_char* buffer, const wn_size_t count,
                            const wn_char* format, ...) {
  va_list argptr;
  va_start(argptr, format);

  #ifdef _WN_WINDOWS
    return(::_vsnprintf(buffer, count, format, argptr));
  #else
    return(::vsnprintf(buffer, count, format, argptr));
  #endif
}

WN_FORCE_INLINE wn_size_t writeuint32(wn_char* _buffer, wn_uint32 _int, wn_size_t _maxSize) {
  WN_RELEASE_ASSERT_DESC(_maxSize > 0, "Must have at least room for 1 character to write out an integer");

  wn_size_t counter = 0;

  for (;;) {
    if (counter >= _maxSize) {
      return(0);
    }

    const wn_int8 test = _int % 10;

    _int -= test;
    _int /= 10;
    _buffer[counter++] = '0' + test;

    if (_int == 0) {
      break;
    }
  }

  //Right now the integer is in swapped order, fix that
  for (wn_size_t i = 0; i < counter / 2; ++i) {
    _buffer[i] ^= _buffer[counter - i - 1];
    _buffer[counter - i - 1] ^= _buffer[i];
    _buffer[i] ^= _buffer[counter - i - 1];
  }

  return(counter);
}

WN_FORCE_INLINE wn_size_t writeint32(wn_char* _buffer, wn_int32 _int, const wn_size_t _maxSize) {
  WN_RELEASE_ASSERT_DESC(_maxSize > 0, "Must have at least 1 characters of space for a signed int");

  wn_size_t counter = 0;

  if (_int < 0) {
    _buffer[counter] = '-';
    _int *= -1;
    _buffer++;
    counter++;
  }

  const wn_size_t actualInt = writeuint32(_buffer, static_cast<wn_uint32>(_int), _maxSize - counter);

  if (actualInt == 0) {
    return(0);
  }

  counter += actualInt;

  return(counter);
}

WN_FORCE_INLINE wn_size_t WNWriteSimpleFloat32(wn_char* _buffer, wn_float32 _flt, wn_size_t _maxSize) {
  WN_RELEASE_ASSERT_DESC(_maxSize > 0, "Can not fill an empty buffer");
  if (_flt == 0.f) {
    _buffer[0] = '0';
    return(1);
  }
  wn_float32 absFloat = _flt > 0.f ? _flt : -_flt;
  WN_RELEASE_ASSERT_DESC(absFloat < 10000000.f && absFloat > 0.0001, "Floating point writing should be limited");
  wn_size_t numsLeft = 7;
  wn_bool losingprecision = wn_false;
  wn_size_t counter = 0;
  if (_flt < 0) {
    _buffer[counter++] = '-';
  }

  wn_uint32 wholePart = static_cast<wn_uint32>(absFloat - fmodf(absFloat, 1.0f));
  wn_size_t wholeChars = static_cast<wn_size_t>(writeuint32(_buffer + counter, wholePart, _maxSize - counter));
  if (wholeChars == 0) {
    return(0);
  }
  counter += wholeChars;
  if (wholePart != 0) {
    losingprecision = wn_true;
    numsLeft -= wholeChars;
  }
  wn_float32 floatPart = fmodf(absFloat, 1.0f);
  if (floatPart == 0.f || numsLeft <= 0) {
    return(counter);
  }

  if (counter >= _maxSize) {
    return(0);
  }
  _buffer[counter++] = '.';
  while (floatPart > 0.f && numsLeft > 0) {
    if (counter > _maxSize) {
      return(0);
    }
    floatPart *= 10.0f;
    wn_byte bPart = static_cast<wn_byte>(floatPart - fmodf(floatPart, 1.0f));
    floatPart = fmodf(floatPart, 1.0f);
    _buffer[counter++] = '0' + bPart;
    if (bPart != 0) {
      losingprecision = wn_true;
    }
    if (losingprecision) {
      numsLeft -= 1;
    }
  }
  return(counter);
}

WN_FORCE_INLINE wn_size_t WNWriteFloat32(wn_char* _buffer, wn_float32 _flt, wn_size_t _maxSize) {
  WN_RELEASE_ASSERT_DESC(_maxSize > 0, "Can not fill an empty buffer");
  if (fabs(_flt) < 10000000.f && fabs(_flt) > 0.0001) {
    return(WNWriteSimpleFloat32(_buffer, _flt, _maxSize));
  }

  const wn_float32 LOG10_LOG2 = 3.321928095f;
  wn_int32 exp = 0;
  wn_bool neg = (_flt < 0.0f);
  _flt *= (neg ? -1.0f : 1.0f);
  wn_float32 base = frexpf(_flt, &exp);
  wn_bool negExp = (exp < 0);
  exp *= (negExp ? -1 : 1);
  wn_float32 exp10 = static_cast<wn_float32>(exp) / LOG10_LOG2;
  wn_float32 multiplier = fmodf(exp10, 1.0f) * (negExp ? -1.0f : 1.0f);
  base *= powf(10.0f, multiplier);
  exp = static_cast<wn_uint32>(exp10 - fmodf(exp10, 1.0));
  wn_int32 dir = (base > 10.f ? -1 :
    (base < 1.0f ? 1 :
    0));

  base *= (dir > 0 ? 10.f : (dir < 0 ? 0.1f : 1.f));

  dir *= negExp ? -1 : 1;
  exp -= dir;

  if (exp < 0) {
    exp *= -1;
    negExp = !negExp;
  }

  if (exp == 0) {
    negExp = wn_false;
  }
  wn_size_t counter = 0;
  if (neg) {
    _buffer[counter++] = '-';
  }

  if (counter >= _maxSize) {
    return(0);
  }

  wn_byte wholePart = static_cast<wn_byte>(base - fmodf(base, 1.0f));
  base = base - static_cast<wn_float32>(wholePart);
  _buffer[counter++] = '0' + wholePart;

  if (counter >= _maxSize) {
    return(0);
  }
  _buffer[counter++] = '.';

  //We lose all of our float precision after 7 decimal numbers
  for (int j = 0; j < 7; ++j) {
    if (counter >= _maxSize) {
      return(0);
    }
    if (base == 0.f) {
      break;
    }
    base = base * 10.0f;
    wholePart = static_cast<wn_byte>(base - fmodf(base, 1.0f));
    base = fmodf(base, 1.0f);
    _buffer[counter++] = '0' + wholePart;
  }
  if (exp == 0) {
    return(counter);
  }

  if (counter > _maxSize - 2) {
    return(0);
  }
  _buffer[counter++] = 'e';
  if (negExp) {
    _buffer[counter++] = '-';
  }
  wn_size_t expPart = writeuint32(_buffer + counter, exp, _maxSize - counter);
  if (expPart == 0) {
    return(0);
  }
  counter += expPart;
  return(counter);
}

WN_FORCE_INLINE wn_size_t writeuint64(wn_char* _buffer, wn_uint64 _int, wn_size_t _maxSize) {
  WN_RELEASE_ASSERT_DESC(_maxSize > 0, "Must have at least room for 1 character to write out an integer");

  wn_size_t counter = 0;

  for (;;) {
    if (counter >= _maxSize) {
      return(0);
    }

    const wn_int8 test = _int % 10;

    _int -= test;
    _int /= 10;
    _buffer[counter++] = '0' + test;

    if (_int == 0) {
      break;
    }
  }

  //Right now the integer is in swapped order, fix that
  for (wn_size_t i = 0; i < counter / 2; ++i) {
    _buffer[i] ^= _buffer[counter - i - 1];
    _buffer[counter - i - 1] ^= _buffer[i];
    _buffer[i] ^= _buffer[counter - i - 1];
  }

  return(counter);
}

WN_FORCE_INLINE wn_size_t writeint64(wn_char* _buffer, wn_int64 _int, wn_size_t _maxSize) {
  WN_RELEASE_ASSERT_DESC(_maxSize > 0, "Must have at least 1 characters of space for a signed int");

  wn_size_t counter = 0;

  if (_int < 0) {
    _buffer[counter] = '-';
    _int *= -1;
    _buffer++;
    counter++;
  }

  const wn_size_t actualInt = writeuint64(_buffer, static_cast<wn_uint32>(_int), _maxSize - counter);

  if (actualInt == 0) {
    return(0);
  }

  counter += actualInt;

  return(counter);
}

WN_FORCE_INLINE wn_size_t WNReadUInt32(wn_char* _buff, wn_uint32& _number, wn_size_t _maxSize) {
  WN_RELEASE_ASSERT_DESC(_maxSize > 0, "You cannot store a number in less than 1 byte");

  wn_size_t counter = 0;

  _number = 0;

  while (_buff[counter] >= '0' && _buff[counter] <= '9' && counter < _maxSize) {
    _number = _number * 10;
    _number = _number + _buff[counter++] - '0';
  }

  return(counter);
}

WN_FORCE_INLINE wn_size_t WNReadInt32(wn_char* _buffer, wn_int32& _number, wn_size_t _maxSize) {
  WN_RELEASE_ASSERT_DESC(_maxSize > 0, "You cannot store a number in less than 1 byte");

  wn_size_t counter = 0;
  wn_bool negative = wn_false;

  if (_buffer[counter] == '-') {
    negative = wn_true;
    counter++;
  }

  wn_uint32 num = 0;
  const wn_size_t readNum = WNReadUInt32(_buffer + counter, num, _maxSize);

  if (readNum == 0) {
    return(0);
  }

  counter += readNum;
  _number = num * (negative ? -1 : 1);

  return(counter);
}

WN_FORCE_INLINE wn_size_t WNReadFloat32(wn_char* _buff, wn_float32& _number, wn_size_t _maxSize) {
  WN_RELEASE_ASSERT_DESC(_maxSize > 0, "You cannot store a number in less than 1 byte");
  const wn_float32 LOG10_LOG2 = 3.321928095f;
  wn_size_t counter = 0;
  wn_bool negative = wn_false;
  wn_uint32 wholePart = 0;
  if (_buff[counter] == '-') {
    counter++;
    negative = wn_true;
  }
  while (_buff[counter] >= '0' && _buff[counter] <= '9' && counter < _maxSize) {
    wholePart = wholePart * 10;
    wholePart = wholePart + _buff[counter++] - '0';
  }

  if (counter >= _maxSize || _buff[counter] != '.') {
    _number = static_cast<wn_float32>(wholePart)* (negative ? -1.0f : 1.0f);
    return(counter);
  }
  counter++;
  wn_float32 floatPart = 0;
  wn_float32 floatMult = 0.1f;
  while (_buff[counter] >= '0' && _buff[counter] <= '9' && counter < _maxSize) {
    floatPart = floatPart + static_cast<wn_float32>(_buff[counter++] - '0') * floatMult;
    floatMult /= 10.0f;
  }
  if (counter >= _maxSize || _buff[counter] != 'e') {
    _number = (static_cast<wn_float32>(wholePart)+floatPart) * (negative ? -1.0f : 1.0f);
    return(counter);
  }
  counter++;
  wn_bool negExp = wn_false;
  wn_int32 exp = 0;
  if (_buff[counter] == '-') {
    negExp = wn_true;
  }

  while (_buff[counter] >= '0' && _buff[counter] <= '9' && counter < _maxSize) {
    exp = exp * 10;
    exp = exp + _buff[counter++] - '0';
  }

  exp = exp * (negExp ? -1 : 1);
  while (wholePart > 0) {
    wn_byte lastDigit = wholePart % 10;
    floatPart /= 10.0f;
    floatPart += wn_float32(lastDigit) / 10.0f;
    exp += 1;
    wholePart -= lastDigit;
    wholePart /= 10;
  }
  negExp = exp < 0;
  exp = exp * (negExp ? -1 : 1);

  wn_float32 exp2 = exp * LOG10_LOG2;
  exp = static_cast<wn_int32>(exp2 - fmodf(exp2, 1.0f));
  exp2 = fmodf(exp2, 1.0f);
  floatPart *= powf(2.0f, exp2);
  exp = exp * (negExp ? -1 : 1);
  _number = ldexpf(floatPart, exp);
  return(counter);
}

WN_FORCE_INLINE wn_size_t WNReadUInt64(wn_char* _buff, wn_uint64& _number, wn_size_t _maxSize) {
  WN_RELEASE_ASSERT_DESC(_maxSize > 0, "You cannot store a number in less than 1 byte");

  wn_size_t counter = 0;

  _number = 0;

  while (_buff[counter] >= '0' && _buff[counter] <= '9') {
    _number = _number * 10;
    _number = _number + _buff[counter++] - '0';
  }

  return(counter);
}

WN_FORCE_INLINE wn_size_t WNReadInt64(wn_char* _buffer, wn_int64& _number, wn_size_t _maxSize) {
  WN_RELEASE_ASSERT_DESC(_maxSize > 0, "You cannot store a number in less than 1 byte");

  wn_size_t counter = 0;
  wn_bool negative = wn_false;

  if (_buffer[counter] == '-') {
    negative = wn_true;
    counter++;
  }

  wn_uint64 num = 0;
  const wn_size_t readNum = WNReadUInt64(_buffer + counter, num, _maxSize);

  if (readNum == 0) {
    return(0);
  }

  counter += readNum;
  _number = num * (negative ? -1 : 1);

  return(counter);
}

} // namespace memory
} // namespace wn

#endif // __WN_MEMORY_STRING_UTILITIES_H__