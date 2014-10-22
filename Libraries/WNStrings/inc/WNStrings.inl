////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    WN_FORCE_INLINE WN_SIZE_T WNStrLen(const WN_CHAR* _string) {
        WN_DEBUG_ASSERT(_string != WN_NULL);

        return(strlen(_string));
    }

    WN_FORCE_INLINE WN_CHAR* WNStrCpy(WN_CHAR* _destination, const WN_CHAR* _source) {
        WN_DEBUG_ASSERT(_destination != WN_NULL);
        WN_DEBUG_ASSERT(_source != WN_NULL);
        WN_DEBUG_ASSERT(_destination != _source);

        return(strcpy(_destination, _source));
    }

    WN_FORCE_INLINE WN_CHAR* WNStrCat(WN_CHAR* _destination, const WN_CHAR* _source) {
        WN_DEBUG_ASSERT(_destination != WN_NULL);
        WN_DEBUG_ASSERT(_source != WN_NULL);
        WN_DEBUG_ASSERT(_destination != _source);
        return(strcat(_destination, _source));
    }

    WN_FORCE_INLINE WN_UINT32 WNStrCmp(const WN_CHAR* _string1, const WN_CHAR* _string2) {
        WN_DEBUG_ASSERT(_string1 != WN_NULL);
        WN_DEBUG_ASSERT(_string2 != WN_NULL);
        if(_string1 == _string2) {
            return(0);
        }

        return(strcmp(_string1, _string2));
    }

    WN_FORCE_INLINE WN_CHAR* WNStrNCpy(WN_CHAR* _destination, const WN_CHAR* _source, WN_SIZE_T _count) {
        WN_DEBUG_ASSERT(_destination != WN_NULL);
        WN_DEBUG_ASSERT(_source != WN_NULL);
        WN_DEBUG_ASSERT(_destination != _source);
        WN_DEBUG_ASSERT(_count != 0);

        return(strncpy(_destination, _source, _count));
    }

    WN_FORCE_INLINE WN_CHAR* WNStrNCat(WN_CHAR* _destination, const WN_CHAR* _source, WN_SIZE_T _count) {
        WN_DEBUG_ASSERT(_destination != WN_NULL);
        WN_DEBUG_ASSERT(_source != WN_NULL);
        WN_DEBUG_ASSERT(_destination != _source);
        WN_DEBUG_ASSERT(_count != 0);

        return(strncat(_destination, _source, _count));
    }

    WN_FORCE_INLINE WN_UINT32 WNStrNCmp(const WN_CHAR* _string1, const WN_CHAR* _string2, WN_SIZE_T _count) {
        WN_DEBUG_ASSERT(_string1 != WN_NULL);
        WN_DEBUG_ASSERT(_string2 != WN_NULL);
        WN_DEBUG_ASSERT(_string1 != _string2);
        WN_DEBUG_ASSERT(_count != 0);

        return(strncmp(_string1, _string2, _count));
    }

    WN_FORCE_INLINE WN_CHAR* WNStrNDup(const WN_CHAR* _string, WN_SIZE_T _length) {
        WN_DEBUG_ASSERT(_string != NULL);
        WN_DEBUG_ASSERT(_length != 0);

        WN_UINT32 size = 0;

        for (; size < (_length - 1) && _string[size] != '\0'; size++);

        WN_CHAR* copy = WNMemory::WNMallocT<WN_CHAR>(size + 1);

        if (copy != WN_NULL) {
            if(size > 0) {
                WNMemory::WNMemCpy(copy, _string, size);
            }

            copy[size] = '\0';
        }

        return(copy);
    }

    WN_FORCE_INLINE WN_CHAR* WNStrChr(WN_CHAR* _string, WN_UINT32 _char) {
        WN_DEBUG_ASSERT(_string != NULL);

        return(strchr(_string, _char));
    }

    WN_FORCE_INLINE const WN_CHAR* WNStrChr(const WN_CHAR* _string, WN_UINT32 _char) {
        WN_DEBUG_ASSERT(_string != NULL);

        return(strchr(_string, _char));
    }

    WN_FORCE_INLINE WN_CHAR* WNStrStr(WN_CHAR* _string1, const WN_CHAR* _string2) {
        WN_DEBUG_ASSERT(_string1 != NULL);
        WN_DEBUG_ASSERT(_string2 != NULL);

        return(strstr(_string1, _string2));
    }

    WN_FORCE_INLINE const WN_CHAR* WNStrStr(const WN_CHAR* _string1, const WN_CHAR* _string2) {
        WN_DEBUG_ASSERT(_string1 != NULL);
        WN_DEBUG_ASSERT(_string2 != NULL);

        return(strstr(_string1, _string2));
    }

    WN_FORCE_INLINE WN_CHAR* WNStrNChr(WN_CHAR* _string, WN_SIZE_T _length, WN_CHAR _char) {
        WN_DEBUG_ASSERT(_string != NULL);
        WN_SIZE_T cnt = 0;
        for(WN_CHAR* first = _string; *first != '\0' && cnt < _length; ++first) {
            if(*first == _char) {
                return(first);
            }
        }

        if(cnt == _length) {
            return(_string + cnt);
        }
        return(WN_NULL);
    }

    WN_FORCE_INLINE const WN_CHAR* WNStrNChr(const WN_CHAR* _string, WN_SIZE_T _length, WN_CHAR _char) {
        WN_DEBUG_ASSERT(_string != NULL);
        WN_SIZE_T cnt = 0;
        for(const WN_CHAR* first = _string; *first != '\0' && cnt < _length; ++first) {
            if(*first == _char) {
                return(first);
            }
        }

        if(cnt == _length) {
            return(_string + cnt);
        }
        return(WN_NULL);
    }

    WN_FORCE_INLINE WN_SIZE_T WNStrHash(const WN_CHAR* _string) {
        WN_DEBUG_ASSERT(_string != NULL);

        WN_SIZE_T hash = 0;
        WN_SIZE_T c;

        while ((c = static_cast<WN_SIZE_T>(*(_string++)))) {
            hash = c + (hash << 6) + (hash << 16) - hash;
        }

        return(hash);
    }

    WN_FORCE_INLINE WN_UINT32 WNStrToInt(const WN_CHAR* _string) {
        WN_DEBUG_ASSERT(_string != WN_NULL);

        return(atoi(_string));
    }

    WN_FORCE_INLINE WN_INT64 WNStrToLL(const WN_CHAR* _string, WN_BOOL& _consumedString) {
#ifdef _WN_WINDOWS
        WN_CHAR* outVal;
        WN_INT64 val = _strtoi64(_string, &outVal, 10);
        _consumedString = (*outVal)=='\0';
        return(val);
#else
        WN_CHAR* outVal;
        WN_INT64 val = strtoll(_string, &outVal, 10);
        _consumedString = (*outVal) == '\0';
        return(val);
#endif
    }

    WN_FORCE_INLINE WN_FLOAT32 WNStrToFlt(const WN_CHAR* _string) {
#ifdef _WN_WINDOWS 
        WN_CHAR* flt = const_cast<WN_CHAR*>(_string); //Do not like this but _atoflt should not be
                                                      //modifying our string anyway
        _CRT_FLOAT f;
        _atoflt(&f, flt);
        return(f.f);
#else
        double d = atof(_string);
        return(d);
#endif
    }

    WN_WCHAR* WNStrToWStr(const WN_CHAR* _string) {
        WN_DEBUG_ASSERT(_string != NULL);

        WN_WCHAR* output = WN_NULL;

        #ifdef _WN_WINDOWS
            const WN_UINT32 length = static_cast<WN_UINT32>(WNStrLen(_string));

            output = WNMemory::WNMallocT<WN_WCHAR>(length);

            if (output != WN_NULL) {
                MultiByteToWideChar(CP_ACP, 0, _string, length, output, length);
            }
        #endif

        return(output);
    }

    WN_CHAR* WNWStrToStr(const WN_WCHAR* _string) {
        WN_CHAR* output = WN_NULL;

        WN_DEBUG_ASSERT(_string != NULL);

        #ifdef _WN_WINDOWS
            const WN_UINT32 length = static_cast<WN_UINT32>(WNWStrLen(_string));

            output = WNMemory::WNMallocT<WN_CHAR>(length);

            if (output != WN_NULL) {
                WideCharToMultiByte(CP_ACP, 0, _string, length, output, length, WN_NULL, WN_NULL);
            }
        #endif

        return(output);
    }

    WN_FORCE_INLINE WN_SIZE_T WNWStrLen(const WN_WCHAR* _string) {
        WN_DEBUG_ASSERT(_string != NULL);

        return(wcslen(_string));
    }

    WN_FORCE_INLINE WN_INT32  WNSnPrintf(WN_CHAR* _buffer, WN_SIZE_T _count, const WN_CHAR* _formatter, ...) {
        va_list argptr;
        va_start(argptr, _formatter);
#ifdef _WN_WINDOWS
        return(_vsnprintf(_buffer, _count, _formatter, argptr));
#else
        return(vsnprintf(_buffer, _count, _formatter, argptr));
#endif
    }

    WN_FORCE_INLINE WN_INT32  WNWSnPrintf(WN_WCHAR* _buffer, WN_SIZE_T _count, const WN_WCHAR* _formatter, ...) {
        va_list argptr;
        va_start(argptr, _formatter);
#ifdef _WN_WINDOWS
        return(_vsnwprintf(_buffer, _count, _formatter, argptr));
#else
        return(vswprintf(_buffer, _count, _formatter, argptr));
#endif
    }
    
    template<>
    WN_INLINE WN_INT32 WNTSnPrintf<WN_CHAR>(WN_CHAR* _buffer, WN_SIZE_T _count, const WN_CHAR* _formatter, ...) {
        va_list argptr;
        va_start(argptr, _formatter);
        #ifdef _WN_WINDOWS
        return(_vsnprintf(_buffer, _count, _formatter, argptr));
#else
        return(vsnprintf(_buffer, _count, _formatter, argptr));
#endif
    }

    template<>
    WN_INLINE WN_INT32 WNTSnPrintf<WN_WCHAR>(WN_WCHAR* _buffer, WN_SIZE_T _count, const WN_WCHAR* _formatter, ...) {
        va_list argptr;
        va_start(argptr, _formatter);
        #ifdef _WN_WINDOWS
        return(_vsnwprintf(_buffer, _count, _formatter, argptr));
#else
        return(vswprintf(_buffer, _count, _formatter, argptr));
#endif
    }


    template<typename T>
    WN_INLINE WN_INT32  WNTSnPrintf(T _buffer, WN_SIZE_T _count, const T _formatter, ...) {
        WN_STATIC_ASSERT(WN_STATIC_ASSERT_DEPENDENT_FAIL<T>::Value);
    }

    WN_FORCE_INLINE WN_SIZE_T WNWriteInt32(WN_CHAR* _buffer, WN_INT32 _int, WN_SIZE_T _maxSize) {
        WN_RELEASE_ASSERT_DESC(_maxSize > 0, "Must have at least 1 characters of space for a signed int");

        WN_SIZE_T counter = 0;

        if (_int < 0) {
            _buffer[counter] = '-';
            _int *= -1;
            _buffer++;
            counter++;
        }

        const WN_SIZE_T actualInt = WNWriteUInt32(_buffer, static_cast<WN_UINT32>(_int), _maxSize - counter);

        if (actualInt == 0) {
            return(0);
        }

        counter += actualInt; 

        return(counter);
    }

    WN_FORCE_INLINE WN_SIZE_T WNWriteUInt32(WN_CHAR* _buffer, WN_UINT32 _int, WN_SIZE_T _maxSize) {
        WN_RELEASE_ASSERT_DESC(_maxSize > 0, "Must have at least room for 1 character to write out an integer");

        WN_SIZE_T counter = 0;

        for (;;) {
            if(counter >= _maxSize) {
                return(0);
            }

            const WN_INT8 test = _int % 10;

            _int -= test;
            _int /= 10;
            _buffer[counter++] = '0' + test;

            if (_int == 0) {
                break;
            }
        }
    
        //Right now the integer is in swapped order, fix that
        for (WN_SIZE_T i = 0; i < counter / 2; ++i) {
            _buffer[i] ^= _buffer[counter - i - 1];
            _buffer[counter - i - 1] ^= _buffer[i];
            _buffer[i] ^= _buffer[counter - i - 1];
        }

        return(counter);
    }

    WN_FORCE_INLINE WN_SIZE_T WNWriteSimpleFloat32(WN_CHAR* _buffer, WN_FLOAT32 _flt, WN_SIZE_T _maxSize) {
        WN_RELEASE_ASSERT_DESC(_maxSize > 0, "Can not fill an empty buffer");
        if(_flt == 0.f) {
            _buffer[0] = '0';
            return(1);
        }
        WN_FLOAT32 absFloat = _flt > 0.f? _flt : -_flt;
        WN_RELEASE_ASSERT_DESC(absFloat < 10000000.f && absFloat > 0.0001, "Floating point writing should be limited");
        WN_SIZE_T numsLeft = 7;
        WN_BOOL losingprecision = WN_FALSE;
        WN_SIZE_T counter = 0;
        if(_flt < 0){
            _buffer[counter++] = '-';
        }

        WN_UINT32 wholePart = static_cast<WN_UINT32>(absFloat - fmodf(absFloat, 1.0f));
        WN_SIZE_T wholeChars = static_cast<WN_SIZE_T>(WNWriteUInt32(_buffer + counter, wholePart, _maxSize - counter));
        if(wholeChars == 0) {
            return(0);
        }
        counter += wholeChars;
        if(wholePart != 0) {
            losingprecision = WN_TRUE;
            numsLeft -= wholeChars;
        }
        WN_FLOAT32 floatPart = fmodf(absFloat, 1.0f);
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
            WN_BYTE bPart = static_cast<WN_BYTE>(floatPart - fmodf(floatPart, 1.0f));
            floatPart = fmodf(floatPart, 1.0f);
            _buffer[counter++] = '0' + bPart;
            if(bPart != 0){
                losingprecision = WN_TRUE;
            }
            if(losingprecision){
                numsLeft -= 1;
            }
        }
        return(counter);
    }

    WN_FORCE_INLINE WN_SIZE_T WNWriteFloat32(WN_CHAR* _buffer, WN_FLOAT32 _flt, WN_SIZE_T _maxSize) {
        WN_RELEASE_ASSERT_DESC(_maxSize > 0, "Can not fill an empty buffer");
        if(fabs(_flt) < 10000000.f && fabs(_flt) > 0.0001){
            return(WNWriteSimpleFloat32(_buffer, _flt, _maxSize));
        }
    
        const WN_FLOAT32 LOG10_LOG2 = 3.321928095f;
        WN_INT32 exp = 0;
        WN_BOOL neg = (_flt < 0.0f);
        _flt *= (neg? -1.0f: 1.0f);
        WN_FLOAT32 base = frexpf(_flt, &exp);
        WN_BOOL negExp = (exp < 0);
        exp *= (negExp? -1: 1);
        WN_FLOAT32 exp10 = static_cast<WN_FLOAT32>(exp) / LOG10_LOG2;
        WN_FLOAT32 multiplier = fmodf(exp10, 1.0f) * (negExp? -1.0f: 1.0f);
        base *= powf(10.0f, multiplier);
        exp = static_cast<WN_UINT32>(exp10 - fmodf(exp10, 1.0));
        WN_INT32 dir = (base > 10.f? -1:
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
            negExp = WN_FALSE;
        }
        WN_SIZE_T counter = 0;
        if(neg) {
            _buffer[counter++] = '-';
        }

        if(counter >= _maxSize) {
            return(0);
        }
    
        WN_BYTE wholePart = static_cast<WN_BYTE>(base - fmodf(base, 1.0f));
        base = base - static_cast<WN_FLOAT32>(wholePart);
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
            WN_BYTE wholePart = static_cast<WN_BYTE>(base - fmodf(base, 1.0f));
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
        WN_SIZE_T expPart = WNWriteUInt32(_buffer + counter, exp, _maxSize - counter);
        if(expPart == 0) {
            return(0);
        }
        counter += expPart;
        return(counter);
    }

    WN_FORCE_INLINE WN_SIZE_T WNWriteInt64(WN_CHAR* _buffer, WN_INT64 _int, WN_SIZE_T _maxSize) {
        WN_RELEASE_ASSERT_DESC(_maxSize > 0, "Must have at least 1 characters of space for a signed int");
        
        WN_SIZE_T counter = 0;
        
        if (_int < 0) {
            _buffer[counter] = '-';
            _int *= -1;
            _buffer++;
            counter++;
        }

        const WN_SIZE_T actualInt = WNWriteUInt64(_buffer, static_cast<WN_UINT32>(_int), _maxSize - counter);
        
        if (actualInt == 0) {
            return(0);
        }

        counter += actualInt;

        return(counter);
    }

    WN_FORCE_INLINE WN_SIZE_T WNWriteUInt64(WN_CHAR* _buffer, WN_UINT64 _int, WN_SIZE_T _maxSize) {
        WN_RELEASE_ASSERT_DESC(_maxSize > 0, "Must have at least room for 1 character to write out an integer");
        
        WN_SIZE_T counter = 0;
        
        for (;;) {
            if (counter >= _maxSize) {
                return(0);
            }

            const WN_INT8 test = _int % 10;

            _int -= test;
            _int /= 10;
            _buffer[counter++] = '0' + test;

            if (_int == 0) {
                break;
            }
        }
    
        //Right now the integer is in swapped order, fix that
        for (WN_SIZE_T i = 0; i < counter / 2; ++i) {
            _buffer[i] ^= _buffer[counter - i - 1];
            _buffer[counter - i - 1] ^= _buffer[i];
            _buffer[i] ^= _buffer[counter - i - 1];
        }

        return(counter);
    }

    WN_FORCE_INLINE WN_SIZE_T WNReadInt32(WN_CHAR* _buffer, WN_INT32& _number, WN_SIZE_T _maxSize) {
        WN_RELEASE_ASSERT_DESC(_maxSize > 0, "You cannot store a number in less than 1 byte");
        
        WN_SIZE_T counter = 0;
        WN_BOOL negative = WN_FALSE;
        
        if (_buffer[counter] == '-') {
            negative = WN_TRUE;
            counter++;
        }

        WN_UINT32 num = 0;
        const WN_SIZE_T readNum = WNReadUInt32(_buffer + counter, num, _maxSize);
        
        if (readNum == 0) {
            return(0);
        }

        counter += readNum;
        _number = num * (negative? -1: 1);

        return(counter);
    }

    WN_FORCE_INLINE WN_SIZE_T WNReadUInt32(WN_CHAR* _buff, WN_UINT32& _number, WN_SIZE_T _maxSize) {
        WN_RELEASE_ASSERT_DESC(_maxSize > 0, "You cannot store a number in less than 1 byte");
        
        WN_SIZE_T counter = 0;

        _number = 0;
        
        while (_buff[counter] >= '0' && _buff[counter] <= '9' && counter < _maxSize) {
            _number = _number * 10;
            _number = _number + _buff[counter++] - '0';
        }

        return(counter);
    }

    WN_FORCE_INLINE WN_SIZE_T WNReadFloat32(WN_CHAR* _buff, WN_FLOAT32& _number, WN_SIZE_T _maxSize) {
        WN_RELEASE_ASSERT_DESC(_maxSize > 0, "You cannot store a number in less than 1 byte");
        const WN_FLOAT32 LOG10_LOG2 = 3.321928095f;
        WN_SIZE_T counter = 0;
        WN_BOOL negative = WN_FALSE;
        WN_UINT32 wholePart = 0;
        if(_buff[counter] == '-') {
            counter++;
            negative = WN_TRUE;
        }
        while(_buff[counter] >= '0' && _buff[counter] <= '9' && counter < _maxSize){
            wholePart = wholePart * 10;
            wholePart = wholePart + _buff[counter++] - '0';
        }

        if(counter >= _maxSize || _buff[counter] != '.') {
            _number = static_cast<WN_FLOAT32>(wholePart) * (negative ? -1.0f: 1.0f);
            return(counter);
        }
        counter++;
        WN_FLOAT32 floatPart = 0;
        WN_FLOAT32 floatMult = 0.1f;
        while(_buff[counter] >= '0' && _buff[counter] <= '9' && counter < _maxSize){
            floatPart = floatPart + static_cast<WN_FLOAT32>(_buff[counter++] - '0') * floatMult;
            floatMult /= 10.0f;
        }
        if(counter >= _maxSize || _buff[counter] != 'e') {
            _number = (static_cast<WN_FLOAT32>(wholePart) + floatPart) * (negative ? -1.0f: 1.0f);
            return(counter);
        }
        counter++;
        WN_BOOL negExp = WN_FALSE;
        WN_INT32 exp = 0;
        if(_buff[counter] == '-') {
            negExp = WN_TRUE;
        }

        while(_buff[counter] >= '0' && _buff[counter] <= '9' && counter < _maxSize) {
            exp = exp * 10;
            exp = exp + _buff[counter++] - '0';
        }

        exp = exp * (negExp? -1: 1);
        while(wholePart > 0){
            WN_BYTE lastDigit = wholePart % 10;
            floatPart /= 10.0f;
            floatPart += WN_FLOAT32(lastDigit) / 10.0f;
            exp += 1;
            wholePart -= lastDigit;
            wholePart /= 10;
        }
        negExp = exp < 0;
        exp = exp * (negExp? -1: 1);

        WN_FLOAT32 exp2 = exp * LOG10_LOG2;
        exp = static_cast<WN_INT32>(exp2 - fmodf(exp2, 1.0f));
        exp2 = fmodf(exp2, 1.0f);
        floatPart *= powf(2.0f, exp2);
        exp = exp * (negExp? -1: 1);
        _number = ldexpf(floatPart, exp);
        return(counter);
    }

    WN_FORCE_INLINE WN_SIZE_T WNReadInt64(WN_CHAR* _buffer, WN_INT64& _number, WN_SIZE_T _maxSize) {
        WN_RELEASE_ASSERT_DESC(_maxSize > 0, "You cannot store a number in less than 1 byte");
        
        WN_SIZE_T counter = 0;
        WN_BOOL negative = WN_FALSE;

        if (_buffer[counter] == '-') {
            negative = WN_TRUE;
            counter++;
        }

        WN_UINT64 num = 0;
        const WN_SIZE_T readNum = WNReadUInt64(_buffer + counter, num, _maxSize);

        if (readNum == 0) {
            return(0);
        }

        counter += readNum;
        _number = num * (negative? -1 : 1);

        return(counter);
    }

    WN_FORCE_INLINE WN_SIZE_T WNReadUInt64(WN_CHAR* _buff, WN_UINT64& _number, WN_SIZE_T _maxSize) {
        WN_RELEASE_ASSERT_DESC(_maxSize > 0, "You cannot store a number in less than 1 byte");

        WN_SIZE_T counter = 0;

        _number = 0;

        while (_buff[counter] >= '0' && _buff[counter] <= '9') {
            _number = _number * 10;
            _number = _number + _buff[counter++] - '0';
        }

        return(counter);
    }
}

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

#endif // __WN_STRINGS_INL__
