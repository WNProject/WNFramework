// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_STRINGS_INL__
#define __WN_STRINGS_INL__

#ifndef __WN_STRINGS_H__
    #error "WNStrings.inl should never be included directly. Please include WNStrings.h instead"
#endif

#include "WNCore/inc/WNBase.h"
#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNAssert.h"
#include "WNMemory/inc/WNMemory.h"

#ifdef _WN_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4996)  // Function was declared deprecated

    #if _WN_MSVC_MAJOR < 16
        #pragma warning (disable: 4985)  // Attributes not present on previous declaration
    #endif
#endif

#ifdef _WN_WINDOWS
    #include <windows.h>
#else
    #include <stdarg.h>
    #include <wchar.h>
#endif

#include <math.h>
#include <string.h>

namespace WNStrings {
    WN_FORCE_INLINE wn_size_t WNStrLen(const wn_char* _string) {
        WN_DEBUG_ASSERT(_string != wn_nullptr);

        return(strlen(_string));
    }

    WN_FORCE_INLINE wn_char* WNStrCpy(wn_char* _destination, const wn_char* _source) {
        WN_DEBUG_ASSERT(_destination != wn_nullptr);
        WN_DEBUG_ASSERT(_source != wn_nullptr);
        WN_DEBUG_ASSERT(_destination != _source);

        return(strcpy(_destination, _source));
    }

    WN_FORCE_INLINE wn_char* WNStrCat(wn_char* _destination, const wn_char* _source) {
        WN_DEBUG_ASSERT(_destination != wn_nullptr);
        WN_DEBUG_ASSERT(_source != wn_nullptr);
        WN_DEBUG_ASSERT(_destination != _source);
        return(strcat(_destination, _source));
    }

    WN_FORCE_INLINE wn_uint32 WNStrCmp(const wn_char* _string1, const wn_char* _string2) {
        WN_DEBUG_ASSERT(_string1 != wn_nullptr);
        WN_DEBUG_ASSERT(_string2 != wn_nullptr);
        if(_string1 == _string2) {
            return(0);
        }

        return(strcmp(_string1, _string2));
    }

    WN_FORCE_INLINE wn_char* WNStrNCpy(wn_char* _destination, const wn_char* _source, wn_size_t _count) {
        WN_DEBUG_ASSERT(_destination != wn_nullptr);
        WN_DEBUG_ASSERT(_source != wn_nullptr);
        WN_DEBUG_ASSERT(_destination != _source);
        WN_DEBUG_ASSERT(_count != 0);

        return(strncpy(_destination, _source, _count));
    }

    WN_FORCE_INLINE wn_char* WNStrNCat(wn_char* _destination, const wn_char* _source, wn_size_t _count) {
        WN_DEBUG_ASSERT(_destination != wn_nullptr);
        WN_DEBUG_ASSERT(_source != wn_nullptr);
        WN_DEBUG_ASSERT(_destination != _source);
        WN_DEBUG_ASSERT(_count != 0);

        return(strncat(_destination, _source, _count));
    }

    WN_FORCE_INLINE wn_uint32 WNStrNCmp(const wn_char* _string1, const wn_char* _string2, wn_size_t _count) {
        WN_DEBUG_ASSERT(_string1 != wn_nullptr);
        WN_DEBUG_ASSERT(_string2 != wn_nullptr);
        WN_DEBUG_ASSERT(_string1 != _string2);
        WN_DEBUG_ASSERT(_count != 0);

        return(strncmp(_string1, _string2, _count));
    }

    WN_FORCE_INLINE wn_char* WNStrNDup(const wn_char* _string, wn_size_t _length) {
        WN_DEBUG_ASSERT(_string != NULL);
        WN_DEBUG_ASSERT(_length != 0);

        wn_uint32 size = 0;

        for (; size < (_length - 1) && _string[size] != '\0'; size++);

        wn_char* copy = wn::malloc<wn_char>(size + 1);

        if (copy != wn_nullptr) {
            if(size > 0) {
                WNMemory::WNMemCpy(copy, _string, size);
            }

            copy[size] = '\0';
        }

        return(copy);
    }

    WN_FORCE_INLINE wn_char* WNStrChr(wn_char* _string, wn_uint32 _char) {
        WN_DEBUG_ASSERT(_string != NULL);

        return(strchr(_string, _char));
    }

    WN_FORCE_INLINE const wn_char* WNStrChr(const wn_char* _string, wn_uint32 _char) {
        WN_DEBUG_ASSERT(_string != NULL);

        return(strchr(_string, _char));
    }

    WN_FORCE_INLINE wn_char* WNStrStr(wn_char* _string1, const wn_char* _string2) {
        WN_DEBUG_ASSERT(_string1 != NULL);
        WN_DEBUG_ASSERT(_string2 != NULL);

        return(strstr(_string1, _string2));
    }

    WN_FORCE_INLINE const wn_char* WNStrStr(const wn_char* _string1, const wn_char* _string2) {
        WN_DEBUG_ASSERT(_string1 != NULL);
        WN_DEBUG_ASSERT(_string2 != NULL);

        return(strstr(_string1, _string2));
    }

    WN_FORCE_INLINE wn_char* WNStrNChr(wn_char* _string, wn_size_t _length, wn_char _char) {
        WN_DEBUG_ASSERT(_string != NULL);
        wn_size_t cnt = 0;
        for(wn_char* first = _string; *first != '\0' && cnt < _length; ++first) {
            if(*first == _char) {
                return(first);
            }
        }

        if(cnt == _length) {
            return(_string + cnt);
        }
        return(wn_nullptr);
    }

    WN_FORCE_INLINE const wn_char* WNStrNChr(const wn_char* _string, wn_size_t _length, wn_char _char) {
        WN_DEBUG_ASSERT(_string != NULL);
        wn_size_t cnt = 0;
        for(const wn_char* first = _string; *first != '\0' && cnt < _length; ++first) {
            if(*first == _char) {
                return(first);
            }
        }

        if(cnt == _length) {
            return(_string + cnt);
        }
        return(wn_nullptr);
    }

    WN_FORCE_INLINE wn_size_t WNStrHash(const wn_char* _string) {
        WN_DEBUG_ASSERT(_string != NULL);

        wn_size_t hash = 0;
        wn_size_t c;

        while ((c = static_cast<wn_size_t>(*(_string++)))) {
            hash = c + (hash << 6) + (hash << 16) - hash;
        }

        return(hash);
    }

    WN_FORCE_INLINE wn_uint32 WNStrToInt(const wn_char* _string) {
        WN_DEBUG_ASSERT(_string != wn_nullptr);

        return(atoi(_string));
    }

    WN_FORCE_INLINE wn_int64 WNStrToLL(const wn_char* _string, wn_bool& _consumedString) {
#ifdef _WN_WINDOWS
        wn_char* outVal;
        wn_int64 val = _strtoi64(_string, &outVal, 10);
        _consumedString = (*outVal)=='\0';
        return(val);
#else
        wn_char* outVal;
        wn_int64 val = strtoll(_string, &outVal, 10);
        _consumedString = (*outVal) == '\0';
        return(val);
#endif
    }

    WN_FORCE_INLINE wn_float32 WNStrToFlt(const wn_char* _string) {
#ifdef _WN_WINDOWS
        wn_char* flt = const_cast<wn_char*>(_string); //Do not like this but _atoflt should not be
                                                      //modifying our string anyway
        _CRT_FLOAT f;
        _atoflt(&f, flt);
        return(f.f);
#else
        double d = atof(_string);
        return(d);
#endif
    }

    wn_wchar* WNStrToWStr(const wn_char* _string) {
        WN_DEBUG_ASSERT(_string != NULL);

        wn_wchar* output = wn_nullptr;

        #ifdef _WN_WINDOWS
            const wn_uint32 length = static_cast<wn_uint32>(WNStrLen(_string));

            output = wn::malloc<wn_wchar>(length);

            if (output != wn_nullptr) {
                MultiByteToWideChar(CP_ACP, 0, _string, length, output, length);
            }
        #endif

        return(output);
    }

    wn_char* WNWStrToStr(const wn_wchar* _string) {
        wn_char* output = wn_nullptr;

        WN_DEBUG_ASSERT(_string != NULL);

        #ifdef _WN_WINDOWS
            const wn_uint32 length = static_cast<wn_uint32>(WNWStrLen(_string));

            output = wn::malloc<wn_char>(length);

            if (output != wn_nullptr) {
                WideCharToMultiByte(CP_ACP, 0, _string, length, output, length, wn_nullptr, wn_nullptr);
            }
        #endif

        return(output);
    }

    WN_FORCE_INLINE wn_size_t WNWStrLen(const wn_wchar* _string) {
        WN_DEBUG_ASSERT(_string != NULL);

        return(wcslen(_string));
    }

    WN_FORCE_INLINE wn_int32  WNSnPrintf(wn_char* _buffer, wn_size_t _count, const wn_char* _formatter, ...) {
        va_list argptr;
        va_start(argptr, _formatter);
#ifdef _WN_WINDOWS
        return(_vsnprintf(_buffer, _count, _formatter, argptr));
#else
        return(vsnprintf(_buffer, _count, _formatter, argptr));
#endif
    }

    WN_FORCE_INLINE wn_int32  WNWSnPrintf(wn_wchar* _buffer, wn_size_t _count, const wn_wchar* _formatter, ...) {
        va_list argptr;
        va_start(argptr, _formatter);
#ifdef _WN_WINDOWS
        return(_vsnwprintf(_buffer, _count, _formatter, argptr));
#else
        return(vswprintf(_buffer, _count, _formatter, argptr));
#endif
    }

    template<>
    WN_INLINE wn_int32 WNTSnPrintf<wn_char>(wn_char* _buffer, wn_size_t _count, const wn_char* _formatter, ...) {
        va_list argptr;
        va_start(argptr, _formatter);
        #ifdef _WN_WINDOWS
        return(_vsnprintf(_buffer, _count, _formatter, argptr));
#else
        return(vsnprintf(_buffer, _count, _formatter, argptr));
#endif
    }

    template<>
    WN_INLINE wn_int32 WNTSnPrintf<wn_wchar>(wn_wchar* _buffer, wn_size_t _count, const wn_wchar* _formatter, ...) {
        va_list argptr;
        va_start(argptr, _formatter);
        #ifdef _WN_WINDOWS
        return(_vsnwprintf(_buffer, _count, _formatter, argptr));
#else
        return(vswprintf(_buffer, _count, _formatter, argptr));
#endif
    }


    template<typename T>
    WN_INLINE wn_int32  WNTSnPrintf(T _buffer, wn_size_t _count, const T _formatter, ...) {
        WN_STATIC_ASSERT(WN_STATIC_ASSERT_DEPENDENT_FAIL<T>::Value);
    }

    WN_FORCE_INLINE wn_size_t WNWriteInt32(wn_char* _buffer, wn_int32 _int, wn_size_t _maxSize) {
        WN_RELEASE_ASSERT_DESC(_maxSize > 0, "Must have at least 1 characters of space for a signed int");

        wn_size_t counter = 0;

        if (_int < 0) {
            _buffer[counter] = '-';
            _int *= -1;
            _buffer++;
            counter++;
        }

        const wn_size_t actualInt = WNWriteUInt32(_buffer, static_cast<wn_uint32>(_int), _maxSize - counter);

        if (actualInt == 0) {
            return(0);
        }

        counter += actualInt;

        return(counter);
    }

    WN_FORCE_INLINE wn_size_t WNWriteUInt32(wn_char* _buffer, wn_uint32 _int, wn_size_t _maxSize) {
        WN_RELEASE_ASSERT_DESC(_maxSize > 0, "Must have at least room for 1 character to write out an integer");

        wn_size_t counter = 0;

        for (;;) {
            if(counter >= _maxSize) {
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

    WN_FORCE_INLINE wn_size_t WNWriteSimpleFloat32(wn_char* _buffer, wn_float32 _flt, wn_size_t _maxSize) {
        WN_RELEASE_ASSERT_DESC(_maxSize > 0, "Can not fill an empty buffer");
        if(_flt == 0.f) {
            _buffer[0] = '0';
            return(1);
        }
        wn_float32 absFloat = _flt > 0.f? _flt : -_flt;
        WN_RELEASE_ASSERT_DESC(absFloat < 10000000.f && absFloat > 0.0001, "Floating point writing should be limited");
        wn_size_t numsLeft = 7;
        wn_bool losingprecision = wn_false;
        wn_size_t counter = 0;
        if(_flt < 0){
            _buffer[counter++] = '-';
        }

        wn_uint32 wholePart = static_cast<wn_uint32>(absFloat - fmodf(absFloat, 1.0f));
        wn_size_t wholeChars = static_cast<wn_size_t>(WNWriteUInt32(_buffer + counter, wholePart, _maxSize - counter));
        if(wholeChars == 0) {
            return(0);
        }
        counter += wholeChars;
        if(wholePart != 0) {
            losingprecision = wn_true;
            numsLeft -= wholeChars;
        }
        wn_float32 floatPart = fmodf(absFloat, 1.0f);
        if(floatPart == 0.f || numsLeft <= 0) {
            return(counter);
        }

        if(counter >= _maxSize) {
            return(0);
        }
        _buffer[counter++] = '.';
        while(floatPart > 0.f && numsLeft > 0){
            if(counter > _maxSize) {
                return(0);
            }
            floatPart *= 10.0f;
            wn_byte bPart = static_cast<wn_byte>(floatPart - fmodf(floatPart, 1.0f));
            floatPart = fmodf(floatPart, 1.0f);
            _buffer[counter++] = '0' + bPart;
            if(bPart != 0){
                losingprecision = wn_true;
            }
            if(losingprecision){
                numsLeft -= 1;
            }
        }
        return(counter);
    }

    WN_FORCE_INLINE wn_size_t WNWriteFloat32(wn_char* _buffer, wn_float32 _flt, wn_size_t _maxSize) {
        WN_RELEASE_ASSERT_DESC(_maxSize > 0, "Can not fill an empty buffer");
        if(fabs(_flt) < 10000000.f && fabs(_flt) > 0.0001){
            return(WNWriteSimpleFloat32(_buffer, _flt, _maxSize));
        }

        const wn_float32 LOG10_LOG2 = 3.321928095f;
        wn_int32 exp = 0;
        wn_bool neg = (_flt < 0.0f);
        _flt *= (neg? -1.0f: 1.0f);
        wn_float32 base = frexpf(_flt, &exp);
        wn_bool negExp = (exp < 0);
        exp *= (negExp? -1: 1);
        wn_float32 exp10 = static_cast<wn_float32>(exp) / LOG10_LOG2;
        wn_float32 multiplier = fmodf(exp10, 1.0f) * (negExp? -1.0f: 1.0f);
        base *= powf(10.0f, multiplier);
        exp = static_cast<wn_uint32>(exp10 - fmodf(exp10, 1.0));
        wn_int32 dir = (base > 10.f? -1:
                     (base < 1.0f? 1:
                      0));

        base *= (dir > 0? 10.f : (dir < 0? 0.1f : 1.f));

        dir *= negExp? -1: 1;
        exp -= dir;

        if(exp < 0) {
            exp *= -1;
            negExp = !negExp;
        }

        if(exp == 0) {
            negExp = wn_false;
        }
        wn_size_t counter = 0;
        if(neg) {
            _buffer[counter++] = '-';
        }

        if(counter >= _maxSize) {
            return(0);
        }

        wn_byte wholePart = static_cast<wn_byte>(base - fmodf(base, 1.0f));
        base = base - static_cast<wn_float32>(wholePart);
        _buffer[counter++] = '0' + wholePart;

        if(counter >= _maxSize) {
            return(0);
        }
        _buffer[counter++] = '.';

        //We lose all of our float precision after 7 decimal numbers
        for(int j = 0; j < 7; ++j) {
            if(counter >= _maxSize) {
                return(0);
            }
            if(base == 0.f) {
                 break;
            }
            base = base * 10.0f;
            wn_byte wholePart = static_cast<wn_byte>(base - fmodf(base, 1.0f));
            base = fmodf(base, 1.0f);
            _buffer[counter++] = '0' + wholePart;
        }
        if(exp == 0) {
            return(counter);
        }

        if(counter > _maxSize - 2) {
            return(0);
        }
        _buffer[counter++] = 'e';
        if(negExp) {
            _buffer[counter++] = '-';
        }
        wn_size_t expPart = WNWriteUInt32(_buffer + counter, exp, _maxSize - counter);
        if(expPart == 0) {
            return(0);
        }
        counter += expPart;
        return(counter);
    }

    WN_FORCE_INLINE wn_size_t WNWriteInt64(wn_char* _buffer, wn_int64 _int, wn_size_t _maxSize) {
        WN_RELEASE_ASSERT_DESC(_maxSize > 0, "Must have at least 1 characters of space for a signed int");

        wn_size_t counter = 0;

        if (_int < 0) {
            _buffer[counter] = '-';
            _int *= -1;
            _buffer++;
            counter++;
        }

        const wn_size_t actualInt = WNWriteUInt64(_buffer, static_cast<wn_uint32>(_int), _maxSize - counter);

        if (actualInt == 0) {
            return(0);
        }

        counter += actualInt;

        return(counter);
    }

    WN_FORCE_INLINE wn_size_t WNWriteUInt64(wn_char* _buffer, wn_uint64 _int, wn_size_t _maxSize) {
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
        _number = num * (negative? -1: 1);

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

    WN_FORCE_INLINE wn_size_t WNReadFloat32(wn_char* _buff, wn_float32& _number, wn_size_t _maxSize) {
        WN_RELEASE_ASSERT_DESC(_maxSize > 0, "You cannot store a number in less than 1 byte");
        const wn_float32 LOG10_LOG2 = 3.321928095f;
        wn_size_t counter = 0;
        wn_bool negative = wn_false;
        wn_uint32 wholePart = 0;
        if(_buff[counter] == '-') {
            counter++;
            negative = wn_true;
        }
        while(_buff[counter] >= '0' && _buff[counter] <= '9' && counter < _maxSize){
            wholePart = wholePart * 10;
            wholePart = wholePart + _buff[counter++] - '0';
        }

        if(counter >= _maxSize || _buff[counter] != '.') {
            _number = static_cast<wn_float32>(wholePart) * (negative ? -1.0f: 1.0f);
            return(counter);
        }
        counter++;
        wn_float32 floatPart = 0;
        wn_float32 floatMult = 0.1f;
        while(_buff[counter] >= '0' && _buff[counter] <= '9' && counter < _maxSize){
            floatPart = floatPart + static_cast<wn_float32>(_buff[counter++] - '0') * floatMult;
            floatMult /= 10.0f;
        }
        if(counter >= _maxSize || _buff[counter] != 'e') {
            _number = (static_cast<wn_float32>(wholePart) + floatPart) * (negative ? -1.0f: 1.0f);
            return(counter);
        }
        counter++;
        wn_bool negExp = wn_false;
        wn_int32 exp = 0;
        if(_buff[counter] == '-') {
            negExp = wn_true;
        }

        while(_buff[counter] >= '0' && _buff[counter] <= '9' && counter < _maxSize) {
            exp = exp * 10;
            exp = exp + _buff[counter++] - '0';
        }

        exp = exp * (negExp? -1: 1);
        while(wholePart > 0){
            wn_byte lastDigit = wholePart % 10;
            floatPart /= 10.0f;
            floatPart += wn_float32(lastDigit) / 10.0f;
            exp += 1;
            wholePart -= lastDigit;
            wholePart /= 10;
        }
        negExp = exp < 0;
        exp = exp * (negExp? -1: 1);

        wn_float32 exp2 = exp * LOG10_LOG2;
        exp = static_cast<wn_int32>(exp2 - fmodf(exp2, 1.0f));
        exp2 = fmodf(exp2, 1.0f);
        floatPart *= powf(2.0f, exp2);
        exp = exp * (negExp? -1: 1);
        _number = ldexpf(floatPart, exp);
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
        _number = num * (negative? -1 : 1);

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

    WN_FORCE_INLINE wn_char WNToLower(wn_char _chr) {
        return(static_cast<wn_char>(tolower(_chr)));
    }

    WN_FORCE_INLINE wn_char WNToUpper(wn_char _chr) {
        return(static_cast<wn_char>(toupper(_chr)));
    }
}

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

#endif // __WN_STRINGS_INL__
