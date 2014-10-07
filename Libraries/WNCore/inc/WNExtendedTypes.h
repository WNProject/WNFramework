////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CORE_EXTENDED_TYPES_H__
#define __WN_CORE_EXTENDED_TYPES_H__

#include "WNCore/inc/WNTypes.h"

class __WNFloat16;
class __WNFixed8;
class __WNFixed16;
class __WNFixed32;
class __WNFixed64;

class __WNFloat8 {
public:
    WN_FORCE_INLINE __WNFloat8() {}
    explicit __WNFloat8(WN_INT8 _integer8);
    explicit __WNFloat8(WN_INT16 _integer16);
    explicit __WNFloat8(WN_INT32 _integer32);
    explicit __WNFloat8(WN_INT64 _integer64);
    explicit __WNFloat8(WN_UINT8 _integer8);
    explicit __WNFloat8(WN_UINT16 _integer16);
    explicit __WNFloat8(WN_UINT32 _integer32);
    explicit __WNFloat8(WN_UINT64 _integer64);
    explicit __WNFloat8(const __WNFixed8& _fixed8);
    explicit __WNFloat8(const __WNFixed16& _fixed16);
    explicit __WNFloat8(const __WNFixed32& _fixed32);
    explicit __WNFloat8(const __WNFixed64& _fixed64);
    __WNFloat8(const __WNFloat8& _float8);
    __WNFloat8(const __WNFloat16& _float16);
    __WNFloat8(WN_FLOAT32 _float32);
    __WNFloat8(WN_FLOAT64 _float64);

    operator WN_INT8 () const;
    operator WN_INT16 () const;
    operator WN_INT32 () const;
    operator WN_INT64 () const;
    operator WN_UINT8 () const;
    operator WN_UINT16 () const;
    operator WN_UINT32 () const;
    operator WN_UINT64 () const;
    operator WN_FLOAT32 () const;
    operator WN_FLOAT64 () const;

    __WNFloat8& operator = (WN_INT8 _integer8);
    __WNFloat8& operator = (WN_INT16 _integer16);
    __WNFloat8& operator = (WN_INT32 _integer32);
    __WNFloat8& operator = (WN_INT64 _integer64);
    __WNFloat8& operator = (WN_UINT8 _integer8);
    __WNFloat8& operator = (WN_UINT16 _integer16);
    __WNFloat8& operator = (WN_UINT32 _integer32);
    __WNFloat8& operator = (WN_UINT64 _integer64);
    __WNFloat8& operator = (const __WNFixed8& _fixed8);
    __WNFloat8& operator = (const __WNFixed16& _fixed16);
    __WNFloat8& operator = (const __WNFixed32& _fixed32);
    __WNFloat8& operator = (const __WNFixed64& _fixed64);
    __WNFloat8& operator = (const __WNFloat8& _float8);
    __WNFloat8& operator = (const __WNFloat16& _float16);
    __WNFloat8& operator = (WN_FLOAT32 _float32);
    __WNFloat8& operator = (WN_FLOAT64 _float64);

    __WNFloat8 operator + () const;
    __WNFloat8 operator - () const;

    __WNFloat8& operator ++ ();
    __WNFloat8 operator ++ (WN_INT32 _integer32);
    __WNFloat8& operator -- ();
    __WNFloat8 operator -- (WN_INT32 _integer32);

    WN_BOOL operator ! () const;

    WN_BOOL operator == (WN_INT8 _integer8) const;
    WN_BOOL operator == (WN_INT16 _integer16) const;
    WN_BOOL operator == (WN_INT32 _integer32) const;
    WN_BOOL operator == (WN_INT64 _integer64) const;
    WN_BOOL operator == (WN_UINT8 _integer8) const;
    WN_BOOL operator == (WN_UINT16 _integer16) const;
    WN_BOOL operator == (WN_UINT32 _integer32) const;
    WN_BOOL operator == (WN_UINT64 _integer64) const;
    WN_BOOL operator == (const __WNFixed8& _fixed8) const;
    WN_BOOL operator == (const __WNFixed16& _fixed16) const;
    WN_BOOL operator == (const __WNFixed32& _fixed32) const;
    WN_BOOL operator == (const __WNFixed64& _fixed64) const;
    WN_BOOL operator == (const __WNFloat8& _float8) const;
    WN_BOOL operator == (const __WNFloat16& _float16) const;
    WN_BOOL operator == (WN_FLOAT32 _float32) const;
    WN_BOOL operator == (WN_FLOAT64 _float64) const;

    WN_BOOL operator != (WN_INT8 _integer8) const;
    WN_BOOL operator != (WN_INT16 _integer16) const;
    WN_BOOL operator != (WN_INT32 _integer32) const;
    WN_BOOL operator != (WN_INT64 _integer64) const;
    WN_BOOL operator != (WN_UINT8 _integer8) const;
    WN_BOOL operator != (WN_UINT16 _integer16) const;
    WN_BOOL operator != (WN_UINT32 _integer32) const;
    WN_BOOL operator != (WN_UINT64 _integer64) const;
    WN_BOOL operator != (const __WNFixed8& _fixed8) const;
    WN_BOOL operator != (const __WNFixed16& _fixed16) const;
    WN_BOOL operator != (const __WNFixed32& _fixed32) const;
    WN_BOOL operator != (const __WNFixed64& _fixed64) const;
    WN_BOOL operator != (const __WNFloat8& _float8) const;
    WN_BOOL operator != (const __WNFloat16& _float16) const;
    WN_BOOL operator != (WN_FLOAT32 _float32) const;
    WN_BOOL operator != (WN_FLOAT64 _float64) const;

    WN_BOOL operator <= (WN_INT8 _integer8) const;
    WN_BOOL operator <= (WN_INT16 _integer16) const;
    WN_BOOL operator <= (WN_INT32 _integer32) const;
    WN_BOOL operator <= (WN_INT64 _integer64) const;
    WN_BOOL operator <= (WN_UINT8 _integer8) const;
    WN_BOOL operator <= (WN_UINT16 _integer16) const;
    WN_BOOL operator <= (WN_UINT32 _integer32) const;
    WN_BOOL operator <= (WN_UINT64 _integer64) const;
    WN_BOOL operator <= (const __WNFixed8& _fixed8) const;
    WN_BOOL operator <= (const __WNFixed16& _fixed16) const;
    WN_BOOL operator <= (const __WNFixed32& _fixed32) const;
    WN_BOOL operator <= (const __WNFixed64& _fixed64) const;
    WN_BOOL operator <= (const __WNFloat8& _float8) const;
    WN_BOOL operator <= (const __WNFloat16& _float16) const;
    WN_BOOL operator <= (WN_FLOAT32 _float32) const;
    WN_BOOL operator <= (WN_FLOAT64 _float64) const;

    WN_BOOL operator >= (WN_INT8 _integer8) const;
    WN_BOOL operator >= (WN_INT16 _integer16) const;
    WN_BOOL operator >= (WN_INT32 _integer32) const;
    WN_BOOL operator >= (WN_INT64 _integer64) const;
    WN_BOOL operator >= (WN_UINT8 _integer8) const;
    WN_BOOL operator >= (WN_UINT16 _integer16) const;
    WN_BOOL operator >= (WN_UINT32 _integer32) const;
    WN_BOOL operator >= (WN_UINT64 _integer64) const;
    WN_BOOL operator >= (const __WNFixed8& _fixed8) const;
    WN_BOOL operator >= (const __WNFixed16& _fixed16) const;
    WN_BOOL operator >= (const __WNFixed32& _fixed32) const;
    WN_BOOL operator >= (const __WNFixed64& _fixed64) const;
    WN_BOOL operator >= (const __WNFloat8& _float8) const;
    WN_BOOL operator >= (const __WNFloat16& _float16) const;
    WN_BOOL operator >= (WN_FLOAT32 _float32) const;
    WN_BOOL operator >= (WN_FLOAT64 _float64) const;

    WN_BOOL operator < (WN_INT8 _integer8) const;
    WN_BOOL operator < (WN_INT16 _integer16) const;
    WN_BOOL operator < (WN_INT32 _integer32) const;
    WN_BOOL operator < (WN_INT64 _integer64) const;
    WN_BOOL operator < (WN_UINT8 _integer8) const;
    WN_BOOL operator < (WN_UINT16 _integer16) const;
    WN_BOOL operator < (WN_UINT32 _integer32) const;
    WN_BOOL operator < (WN_UINT64 _integer64) const;
    WN_BOOL operator < (const __WNFixed8& _fixed8) const;
    WN_BOOL operator < (const __WNFixed16& _fixed16) const;
    WN_BOOL operator < (const __WNFixed32& _fixed32) const;
    WN_BOOL operator < (const __WNFixed64& _fixed64) const;
    WN_BOOL operator < (const __WNFloat8& _float8) const;
    WN_BOOL operator < (const __WNFloat16& _float16) const;
    WN_BOOL operator < (WN_FLOAT32 _float32) const;
    WN_BOOL operator < (WN_FLOAT64 _float64) const;

    WN_BOOL operator > (WN_INT8 _integer8) const;
    WN_BOOL operator > (WN_INT16 _integer16) const;
    WN_BOOL operator > (WN_INT32 _integer32) const;
    WN_BOOL operator > (WN_INT64 _integer64) const;
    WN_BOOL operator > (WN_UINT8 _integer8) const;
    WN_BOOL operator > (WN_UINT16 _integer16) const;
    WN_BOOL operator > (WN_UINT32 _integer32) const;
    WN_BOOL operator > (WN_UINT64 _integer64) const;
    WN_BOOL operator > (const __WNFixed8& _fixed8) const;
    WN_BOOL operator > (const __WNFixed16& _fixed16) const;
    WN_BOOL operator > (const __WNFixed32& _fixed32) const;
    WN_BOOL operator > (const __WNFixed64& _fixed64) const;
    WN_BOOL operator > (const __WNFloat8& _float8) const;
    WN_BOOL operator > (const __WNFloat16& _float16) const;
    WN_BOOL operator > (WN_FLOAT32 _float32) const;
    WN_BOOL operator > (WN_FLOAT64 _float64) const;

    __WNFloat8& operator += (WN_INT8 _integer8);
    __WNFloat8& operator += (WN_INT16 _integer16);
    __WNFloat8& operator += (WN_INT32 _integer32);
    __WNFloat8& operator += (WN_INT64 _integer64);
    __WNFloat8& operator += (WN_UINT8 _integer8);
    __WNFloat8& operator += (WN_UINT16 _integer16);
    __WNFloat8& operator += (WN_UINT32 _integer32);
    __WNFloat8& operator += (WN_UINT64 _integer64);
    __WNFloat8& operator += (const __WNFixed8& _fixed8);
    __WNFloat8& operator += (const __WNFixed16& _fixed16);
    __WNFloat8& operator += (const __WNFixed32& _fixed32);
    __WNFloat8& operator += (const __WNFixed64& _fixed64);
    __WNFloat8& operator += (const __WNFloat8& _float8);
    __WNFloat8& operator += (const __WNFloat16& _float16);
    __WNFloat8& operator += (WN_FLOAT32 _float32);
    __WNFloat8& operator += (WN_FLOAT64 _float64);

    __WNFloat8& operator -= (WN_INT8 _integer8);
    __WNFloat8& operator -= (WN_INT16 _integer16);
    __WNFloat8& operator -= (WN_INT32 _integer32);
    __WNFloat8& operator -= (WN_INT64 _integer64);
    __WNFloat8& operator -= (WN_UINT8 _integer8);
    __WNFloat8& operator -= (WN_UINT16 _integer16);
    __WNFloat8& operator -= (WN_UINT32 _integer32);
    __WNFloat8& operator -= (WN_UINT64 _integer64);
    __WNFloat8& operator -= (const __WNFixed8& _fixed8);
    __WNFloat8& operator -= (const __WNFixed16& _fixed16);
    __WNFloat8& operator -= (const __WNFixed32& _fixed32);
    __WNFloat8& operator -= (const __WNFixed64& _fixed64);
    __WNFloat8& operator -= (const __WNFloat8& _float8);
    __WNFloat8& operator -= (const __WNFloat16& _float16);
    __WNFloat8& operator -= (WN_FLOAT32 _float32);
    __WNFloat8& operator -= (WN_FLOAT64 _float64);

    __WNFloat8& operator *= (WN_INT8 _integer8);
    __WNFloat8& operator *= (WN_INT16 _integer16);
    __WNFloat8& operator *= (WN_INT32 _integer32);
    __WNFloat8& operator *= (WN_INT64 _integer64);
    __WNFloat8& operator *= (WN_UINT8 _integer8);
    __WNFloat8& operator *= (WN_UINT16 _integer16);
    __WNFloat8& operator *= (WN_UINT32 _integer32);
    __WNFloat8& operator *= (WN_UINT64 _integer64);
    __WNFloat8& operator *= (const __WNFixed8& _fixed8);
    __WNFloat8& operator *= (const __WNFixed16& _fixed16);
    __WNFloat8& operator *= (const __WNFixed32& _fixed32);
    __WNFloat8& operator *= (const __WNFixed64& _fixed64);
    __WNFloat8& operator *= (const __WNFloat8& _float8);
    __WNFloat8& operator *= (const __WNFloat16& _float16);
    __WNFloat8& operator *= (WN_FLOAT32 _float32);
    __WNFloat8& operator *= (WN_FLOAT64 _float64);

    __WNFloat8& operator /= (WN_INT8 _integer8);
    __WNFloat8& operator /= (WN_INT16 _integer16);
    __WNFloat8& operator /= (WN_INT32 _integer32);
    __WNFloat8& operator /= (WN_INT64 _integer64);
    __WNFloat8& operator /= (WN_UINT8 _integer8);
    __WNFloat8& operator /= (WN_UINT16 _integer16);
    __WNFloat8& operator /= (WN_UINT32 _integer32);
    __WNFloat8& operator /= (WN_UINT64 _integer64);
    __WNFloat8& operator /= (const __WNFixed8& _fixed8);
    __WNFloat8& operator /= (const __WNFixed16& _fixed16);
    __WNFloat8& operator /= (const __WNFixed32& _fixed32);
    __WNFloat8& operator /= (const __WNFixed64& _fixed64);
    __WNFloat8& operator /= (const __WNFloat8& _float8);
    __WNFloat8& operator /= (const __WNFloat16& _float16);
    __WNFloat8& operator /= (WN_FLOAT32 _float32);
    __WNFloat8& operator /= (WN_FLOAT64 _float64);

    __WNFloat8 operator + (WN_INT8 _integer8) const;
    __WNFloat8 operator + (WN_INT16 _integer16) const;
    __WNFloat8 operator + (WN_INT32 _integer32) const;
    __WNFloat8 operator + (WN_INT64 _integer64) const;
    __WNFloat8 operator + (WN_UINT8 _integer8) const;
    __WNFloat8 operator + (WN_UINT16 _integer16) const;
    __WNFloat8 operator + (WN_UINT32 _integer32) const;
    __WNFloat8 operator + (WN_UINT64 _integer64) const;
    __WNFloat8 operator + (const __WNFixed8& _fixed8) const;
    __WNFloat8 operator + (const __WNFixed16& _fixed16) const;
    __WNFloat8 operator + (const __WNFixed32& _fixed32) const;
    __WNFloat8 operator + (const __WNFixed64& _fixed64) const;
    __WNFloat8 operator + (const __WNFloat8& _float8) const;
    __WNFloat16 operator + (const __WNFloat16& _float16) const;
    WN_FLOAT32 operator + (WN_FLOAT32 _float32) const;
    WN_FLOAT64 operator + (WN_FLOAT64 _float64) const;

    __WNFloat8 operator - (WN_INT8 _integer8) const;
    __WNFloat8 operator - (WN_INT16 _integer16) const;
    __WNFloat8 operator - (WN_INT32 _integer32) const;
    __WNFloat8 operator - (WN_INT64 _integer64) const;
    __WNFloat8 operator - (WN_UINT8 _integer8) const;
    __WNFloat8 operator - (WN_UINT16 _integer16) const;
    __WNFloat8 operator - (WN_UINT32 _integer32) const;
    __WNFloat8 operator - (WN_UINT64 _integer64) const;
    __WNFloat8 operator - (const __WNFixed8& _fixed8) const;
    __WNFloat8 operator - (const __WNFixed16& _fixed16) const;
    __WNFloat8 operator - (const __WNFixed32& _fixed32) const;
    __WNFloat8 operator - (const __WNFixed64& _fixed64) const;
    __WNFloat8 operator - (const __WNFloat8& _float8) const;
    __WNFloat16 operator - (const __WNFloat16& _float16) const;
    WN_FLOAT32 operator - (WN_FLOAT32 _float32) const;
    WN_FLOAT64 operator - (WN_FLOAT64 _float64) const;

    __WNFloat8 operator * (WN_INT8 _integer8) const;
    __WNFloat8 operator * (WN_INT16 _integer16) const;
    __WNFloat8 operator * (WN_INT32 _integer32) const;
    __WNFloat8 operator * (WN_INT64 _integer64) const;
    __WNFloat8 operator * (WN_UINT8 _integer8) const;
    __WNFloat8 operator * (WN_UINT16 _integer16) const;
    __WNFloat8 operator * (WN_UINT32 _integer32) const;
    __WNFloat8 operator * (WN_UINT64 _integer64) const;
    __WNFloat8 operator * (const __WNFixed8& _fixed8) const;
    __WNFloat8 operator * (const __WNFixed16& _fixed16) const;
    __WNFloat8 operator * (const __WNFixed32& _fixed32) const;
    __WNFloat8 operator * (const __WNFixed64& _fixed64) const;
    __WNFloat8 operator * (const __WNFloat8& _float8) const;
    __WNFloat16 operator * (const __WNFloat16& _float16) const;
    WN_FLOAT32 operator * (WN_FLOAT32 _float32) const;
    WN_FLOAT64 operator * (WN_FLOAT64 _float64) const;

    __WNFloat8 operator / (WN_INT8 _integer8) const;
    __WNFloat8 operator / (WN_INT16 _integer16) const;
    __WNFloat8 operator / (WN_INT32 _integer32) const;
    __WNFloat8 operator / (WN_INT64 _integer64) const;
    __WNFloat8 operator / (WN_UINT8 _integer8) const;
    __WNFloat8 operator / (WN_UINT16 _integer16) const;
    __WNFloat8 operator / (WN_UINT32 _integer32) const;
    __WNFloat8 operator / (WN_UINT64 _integer64) const;
    __WNFloat8 operator / (const __WNFixed8& _fixed8) const;
    __WNFloat8 operator / (const __WNFixed16& _fixed16) const;
    __WNFloat8 operator / (const __WNFixed32& _fixed32) const;
    __WNFloat8 operator / (const __WNFixed64& _fixed64) const;
    __WNFloat8 operator / (const __WNFloat8& _float8) const;
    __WNFloat16 operator / (const __WNFloat16& _float16) const;
    WN_FLOAT32 operator / (WN_FLOAT32 _float32) const;
    WN_FLOAT64 operator / (WN_FLOAT64 _float64) const;

private:
    static WN_INT8 F32ToF8(WN_FLOAT32 _float32);
    static WN_FLOAT32 F8ToF32(WN_INT8 _float8);

public:
    WN_INT8 mBits; // stores 8 bit floating point http://en.wikipedia.org/wiki/Minifloat
};

WN_BOOL operator == (WN_INT8 _integer8, const __WNFloat8& _float8);
WN_BOOL operator == (WN_INT16 _integer16, const __WNFloat8& _float8);
WN_BOOL operator == (WN_INT32 _integer32, const __WNFloat8& _float8);
WN_BOOL operator == (WN_INT64 _integer64, const __WNFloat8& _float8);
WN_BOOL operator == (WN_UINT8 _integer8, const __WNFloat8& _float8);
WN_BOOL operator == (WN_UINT16 _integer16, const __WNFloat8& _float8);
WN_BOOL operator == (WN_UINT32 _integer32, const __WNFloat8& _float8);
WN_BOOL operator == (WN_UINT64 _integer64, const __WNFloat8& _float8);
WN_BOOL operator == (WN_FLOAT32 _float32, const __WNFloat8& _float8);
WN_BOOL operator == (WN_FLOAT64 _float64, const __WNFloat8& _float8);

WN_BOOL operator != (WN_INT8 _integer8, const __WNFloat8& _float8);
WN_BOOL operator != (WN_INT16 _integer16, const __WNFloat8& _float8);
WN_BOOL operator != (WN_INT32 _integer32, const __WNFloat8& _float8);
WN_BOOL operator != (WN_INT64 _integer64, const __WNFloat8& _float8);
WN_BOOL operator != (WN_UINT8 _integer8, const __WNFloat8& _float8);
WN_BOOL operator != (WN_UINT16 _integer16, const __WNFloat8& _float8);
WN_BOOL operator != (WN_UINT32 _integer32, const __WNFloat8& _float8);
WN_BOOL operator != (WN_UINT64 _integer64, const __WNFloat8& _float8);
WN_BOOL operator != (WN_FLOAT32 _float32, const __WNFloat8& _float8);
WN_BOOL operator != (WN_FLOAT64 _float64, const __WNFloat8& _float8);

WN_BOOL operator <= (WN_INT8 _integer8, const __WNFloat8& _float8);
WN_BOOL operator <= (WN_INT16 _integer16, const __WNFloat8& _float8);
WN_BOOL operator <= (WN_INT32 _integer32, const __WNFloat8& _float8);
WN_BOOL operator <= (WN_INT64 _integer64, const __WNFloat8& _float8);
WN_BOOL operator <= (WN_UINT8 _integer8, const __WNFloat8& _float8);
WN_BOOL operator <= (WN_UINT16 _integer16, const __WNFloat8& _float8);
WN_BOOL operator <= (WN_UINT32 _integer32, const __WNFloat8& _float8);
WN_BOOL operator <= (WN_UINT64 _integer64, const __WNFloat8& _float8);
WN_BOOL operator <= (WN_FLOAT32 _float32, const __WNFloat8& _float8);
WN_BOOL operator <= (WN_FLOAT64 _float64, const __WNFloat8& _float8);

WN_BOOL operator >= (WN_INT8 _integer8, const __WNFloat8& _float8);
WN_BOOL operator >= (WN_INT16 _integer16, const __WNFloat8& _float8);
WN_BOOL operator >= (WN_INT32 _integer32, const __WNFloat8& _float8);
WN_BOOL operator >= (WN_INT64 _integer64, const __WNFloat8& _float8);
WN_BOOL operator >= (WN_UINT8 _integer8, const __WNFloat8& _float8);
WN_BOOL operator >= (WN_UINT16 _integer16, const __WNFloat8& _float8);
WN_BOOL operator >= (WN_UINT32 _integer32, const __WNFloat8& _float8);
WN_BOOL operator >= (WN_UINT64 _integer64, const __WNFloat8& _float8);
WN_BOOL operator >= (WN_FLOAT32 _float32, const __WNFloat8& _float8);
WN_BOOL operator >= (WN_FLOAT64 _float64, const __WNFloat8& _float8);

WN_BOOL operator < (WN_INT8 _integer8, const __WNFloat8& _float8);
WN_BOOL operator < (WN_INT16 _integer16, const __WNFloat8& _float8);
WN_BOOL operator < (WN_INT32 _integer32, const __WNFloat8& _float8);
WN_BOOL operator < (WN_INT64 _integer64, const __WNFloat8& _float8);
WN_BOOL operator < (WN_UINT8 _integer8, const __WNFloat8& _float8);
WN_BOOL operator < (WN_UINT16 _integer16, const __WNFloat8& _float8);
WN_BOOL operator < (WN_UINT32 _integer32, const __WNFloat8& _float8);
WN_BOOL operator < (WN_UINT64 _integer64, const __WNFloat8& _float8);
WN_BOOL operator < (WN_FLOAT32 _float32, const __WNFloat8& _float8);
WN_BOOL operator < (WN_FLOAT64 _float64, const __WNFloat8& _float8);

WN_BOOL operator > (WN_INT8 _integer8, const __WNFloat8& _float8);
WN_BOOL operator > (WN_INT16 _integer16, const __WNFloat8& _float8);
WN_BOOL operator > (WN_INT32 _integer32, const __WNFloat8& _float8);
WN_BOOL operator > (WN_INT64 _integer64, const __WNFloat8& _float8);
WN_BOOL operator > (WN_UINT8 _integer8, const __WNFloat8& _float8);
WN_BOOL operator > (WN_UINT16 _integer16, const __WNFloat8& _float8);
WN_BOOL operator > (WN_UINT32 _integer32, const __WNFloat8& _float8);
WN_BOOL operator > (WN_UINT64 _integer64, const __WNFloat8& _float8);
WN_BOOL operator > (WN_FLOAT32 _float32, const __WNFloat8& _float8);
WN_BOOL operator > (WN_FLOAT64 _float64, const __WNFloat8& _float8);

WN_INT8& operator += (WN_INT8& _integer8, const __WNFloat8& _float8);
WN_INT16& operator += (WN_INT16& _integer16, const __WNFloat8& _float8);
WN_INT32& operator += (WN_INT32& _integer32, const __WNFloat8& _float8);
WN_INT64& operator += (WN_INT64& _integer64, const __WNFloat8& _float8);
WN_UINT8& operator += (WN_UINT8& _integer8, const __WNFloat8& _float8);
WN_UINT16& operator += (WN_UINT16& _integer16, const __WNFloat8& _float8);
WN_UINT32& operator += (WN_UINT32& _integer32, const __WNFloat8& _float8);
WN_UINT64& operator += (WN_UINT64& _integer64, const __WNFloat8& _float8);
WN_FLOAT32& operator += (WN_FLOAT32& _float32, const __WNFloat8& _float8);
WN_FLOAT64& operator += (WN_FLOAT64& _float64, const __WNFloat8& _float8);

WN_INT8& operator -= (WN_INT8& _integer8, const __WNFloat8& _float8);
WN_INT16& operator -= (WN_INT16& _integer16, const __WNFloat8& _float8);
WN_INT32& operator -= (WN_INT32& _integer32, const __WNFloat8& _float8);
WN_INT64& operator -= (WN_INT64& _integer64, const __WNFloat8& _float8);
WN_UINT8& operator -= (WN_UINT8& _integer8, const __WNFloat8& _float8);
WN_UINT16& operator -= (WN_UINT16& _integer16, const __WNFloat8& _float8);
WN_UINT32& operator -= (WN_UINT32& _integer32, const __WNFloat8& _float8);
WN_UINT64& operator -= (WN_UINT64& _integer64, const __WNFloat8& _float8);
WN_FLOAT32& operator -= (WN_FLOAT32& _float32, const __WNFloat8& _float8);
WN_FLOAT64& operator -= (WN_FLOAT64& _float64, const __WNFloat8& _float8);

WN_INT8& operator *= (WN_INT8& _integer8, const __WNFloat8& _float8);
WN_INT16& operator *= (WN_INT16& _integer16, const __WNFloat8& _float8);
WN_INT32& operator *= (WN_INT32& _integer32, const __WNFloat8& _float8);
WN_INT64& operator *= (WN_INT64& _integer64, const __WNFloat8& _float8);
WN_UINT8& operator *= (WN_UINT8& _integer8, const __WNFloat8& _float8);
WN_UINT16& operator *= (WN_UINT16& _integer16, const __WNFloat8& _float8);
WN_UINT32& operator *= (WN_UINT32& _integer32, const __WNFloat8& _float8);
WN_UINT64& operator *= (WN_UINT64& _integer64, const __WNFloat8& _float8);
WN_FLOAT32& operator *= (WN_FLOAT32& _float32, const __WNFloat8& _float8);
WN_FLOAT64& operator *= (WN_FLOAT64& _float64, const __WNFloat8& _float8);

WN_INT8& operator /= (WN_INT8& _integer8, const __WNFloat8& _float8);
WN_INT16& operator /= (WN_INT16& _integer16, const __WNFloat8& _float8);
WN_INT32& operator /= (WN_INT32& _integer32, const __WNFloat8& _float8);
WN_INT64& operator /= (WN_INT64& _integer64, const __WNFloat8& _float8);
WN_UINT8& operator /= (WN_UINT8& _integer8, const __WNFloat8& _float8);
WN_UINT16& operator /= (WN_UINT16& _integer16, const __WNFloat8& _float8);
WN_UINT32& operator /= (WN_UINT32& _integer32, const __WNFloat8& _float8);
WN_UINT64& operator /= (WN_UINT64& _integer64, const __WNFloat8& _float8);
WN_FLOAT32& operator /= (WN_FLOAT32& _float32, const __WNFloat8& _float8);
WN_FLOAT64& operator /= (WN_FLOAT64& _float64, const __WNFloat8& _float8);

__WNFloat8 operator + (WN_INT8 _integer8, const __WNFloat8& _float8);
__WNFloat8 operator + (WN_INT16 _integer16, const __WNFloat8& _float8);
__WNFloat8 operator + (WN_INT32 _integer32, const __WNFloat8& _float8);
__WNFloat8 operator + (WN_INT64 _integer64, const __WNFloat8& _float8);
__WNFloat8 operator + (WN_UINT8 _integer8, const __WNFloat8& _float8);
__WNFloat8 operator + (WN_UINT16 _integer16, const __WNFloat8& _float8);
__WNFloat8 operator + (WN_UINT32 _integer32, const __WNFloat8& _float8);
__WNFloat8 operator + (WN_UINT64 _integer64, const __WNFloat8& _float8);
WN_FLOAT32 operator + (WN_FLOAT32 _float32, const __WNFloat8& _float8);
WN_FLOAT64 operator + (WN_FLOAT64 _float64, const __WNFloat8& _float8);

__WNFloat8 operator - (WN_INT8 _integer8, const __WNFloat8& _float8);
__WNFloat8 operator - (WN_INT16 _integer16, const __WNFloat8& _float8);
__WNFloat8 operator - (WN_INT32 _integer32, const __WNFloat8& _float8);
__WNFloat8 operator - (WN_INT64 _integer64, const __WNFloat8& _float8);
__WNFloat8 operator - (WN_UINT8 _integer8, const __WNFloat8& _float8);
__WNFloat8 operator - (WN_UINT16 _integer16, const __WNFloat8& _float8);
__WNFloat8 operator - (WN_UINT32 _integer32, const __WNFloat8& _float8);
__WNFloat8 operator - (WN_UINT64 _integer64, const __WNFloat8& _float8);
WN_FLOAT32 operator - (WN_FLOAT32 _float32, const __WNFloat8& _float8);
WN_FLOAT64 operator - (WN_FLOAT64 _float64, const __WNFloat8& _float8);

__WNFloat8 operator * (WN_INT8 _integer8, const __WNFloat8& _float8);
__WNFloat8 operator * (WN_INT16 _integer16, const __WNFloat8& _float8);
__WNFloat8 operator * (WN_INT32 _integer32, const __WNFloat8& _float8);
__WNFloat8 operator * (WN_INT64 _integer64, const __WNFloat8& _float8);
__WNFloat8 operator * (WN_UINT8 _integer8, const __WNFloat8& _float8);
__WNFloat8 operator * (WN_UINT16 _integer16, const __WNFloat8& _float8);
__WNFloat8 operator * (WN_UINT32 _integer32, const __WNFloat8& _float8);
__WNFloat8 operator * (WN_UINT64 _integer64, const __WNFloat8& _float8);
WN_FLOAT32 operator * (WN_FLOAT32 _float32, const __WNFloat8& _float8);
WN_FLOAT64 operator * (WN_FLOAT64 _float64, const __WNFloat8& _float8);

__WNFloat8 operator / (WN_INT8 _integer8, const __WNFloat8& _float8);
__WNFloat8 operator / (WN_INT16 _integer16, const __WNFloat8& _float8);
__WNFloat8 operator / (WN_INT32 _integer32, const __WNFloat8& _float8);
__WNFloat8 operator / (WN_INT64 _integer64, const __WNFloat8& _float8);
__WNFloat8 operator / (WN_UINT8 _integer8, const __WNFloat8& _float8);
__WNFloat8 operator / (WN_UINT16 _integer16, const __WNFloat8& _float8);
__WNFloat8 operator / (WN_UINT32 _integer32, const __WNFloat8& _float8);
__WNFloat8 operator / (WN_UINT64 _integer64, const __WNFloat8& _float8);
WN_FLOAT32 operator / (WN_FLOAT32 _float32, const __WNFloat8& _float8);
WN_FLOAT64 operator / (WN_FLOAT64 _float64, const __WNFloat8& _float8);

class __WNFloat16 {
public:
    WN_FORCE_INLINE __WNFloat16() {}
    explicit __WNFloat16(WN_INT8 _integer8);
    explicit __WNFloat16(WN_INT16 _integer16);
    explicit __WNFloat16(WN_INT32 _integer32);
    explicit __WNFloat16(WN_INT64 _integer64);
    explicit __WNFloat16(WN_UINT8 _integer8);
    explicit __WNFloat16(WN_UINT16 _integer16);
    explicit __WNFloat16(WN_UINT32 _integer32);
    explicit __WNFloat16(WN_UINT64 _integer64);
    explicit __WNFloat16(const __WNFixed8& _fixed8);
    explicit __WNFloat16(const __WNFixed16& _fixed16);
    explicit __WNFloat16(const __WNFixed32& _fixed32);
    explicit __WNFloat16(const __WNFixed64& _fixed64);
    __WNFloat16(const __WNFloat8& _float8);
    __WNFloat16(const __WNFloat16& _float16);
    __WNFloat16(WN_FLOAT32 _float32);
    __WNFloat16(WN_FLOAT64 _float64);

    operator WN_INT8 () const;
    operator WN_INT16 () const;
    operator WN_INT32 () const;
    operator WN_INT64 () const;
    operator WN_UINT8 () const;
    operator WN_UINT16 () const;
    operator WN_UINT32 () const;
    operator WN_UINT64 () const;
    operator WN_FLOAT32 () const;
    operator WN_FLOAT64 () const;

    __WNFloat16& operator = (WN_INT8 _integer8);
    __WNFloat16& operator = (WN_INT16 _integer16);
    __WNFloat16& operator = (WN_INT32 _integer32);
    __WNFloat16& operator = (WN_INT64 _integer64);
    __WNFloat16& operator = (WN_UINT8 _integer8);
    __WNFloat16& operator = (WN_UINT16 _integer16);
    __WNFloat16& operator = (WN_UINT32 _integer32);
    __WNFloat16& operator = (WN_UINT64 _integer64);
    __WNFloat16& operator = (const __WNFixed8& _fixed8);
    __WNFloat16& operator = (const __WNFixed16& _fixed16);
    __WNFloat16& operator = (const __WNFixed32& _fixed32);
    __WNFloat16& operator = (const __WNFixed64& _fixed64);
    __WNFloat16& operator = (const __WNFloat8& _float8);
    __WNFloat16& operator = (const __WNFloat16& _float16);
    __WNFloat16& operator = (WN_FLOAT32 _float32);
    __WNFloat16& operator = (WN_FLOAT64 _float64);

    __WNFloat16 operator + () const;
    __WNFloat16 operator - () const;

    __WNFloat16& operator ++ ();
    __WNFloat16 operator ++ (WN_INT32 _integer32);
    __WNFloat16& operator -- ();
    __WNFloat16 operator -- (WN_INT32 _integer32);

    WN_BOOL operator ! () const;

    WN_BOOL operator == (WN_INT8 _integer8) const;
    WN_BOOL operator == (WN_INT16 _integer16) const;
    WN_BOOL operator == (WN_INT32 _integer32) const;
    WN_BOOL operator == (WN_INT64 _integer64) const;
    WN_BOOL operator == (WN_UINT8 _integer8) const;
    WN_BOOL operator == (WN_UINT16 _integer16) const;
    WN_BOOL operator == (WN_UINT32 _integer32) const;
    WN_BOOL operator == (WN_UINT64 _integer64) const;
    WN_BOOL operator == (const __WNFixed8& _fixed8) const;
    WN_BOOL operator == (const __WNFixed16& _fixed16) const;
    WN_BOOL operator == (const __WNFixed32& _fixed32) const;
    WN_BOOL operator == (const __WNFixed64& _fixed64) const;
    WN_BOOL operator == (const __WNFloat8& _float8) const;
    WN_BOOL operator == (const __WNFloat16& _float16) const;
    WN_BOOL operator == (WN_FLOAT32 _float32) const;
    WN_BOOL operator == (WN_FLOAT64 _float64) const;

    WN_BOOL operator != (WN_INT8 _integer8) const;
    WN_BOOL operator != (WN_INT16 _integer16) const;
    WN_BOOL operator != (WN_INT32 _integer32) const;
    WN_BOOL operator != (WN_INT64 _integer64) const;
    WN_BOOL operator != (WN_UINT8 _integer8) const;
    WN_BOOL operator != (WN_UINT16 _integer16) const;
    WN_BOOL operator != (WN_UINT32 _integer32) const;
    WN_BOOL operator != (WN_UINT64 _integer64) const;
    WN_BOOL operator != (const __WNFixed8& _fixed8) const;
    WN_BOOL operator != (const __WNFixed16& _fixed16) const;
    WN_BOOL operator != (const __WNFixed32& _fixed32) const;
    WN_BOOL operator != (const __WNFixed64& _fixed64) const;
    WN_BOOL operator != (const __WNFloat8& _float8) const;
    WN_BOOL operator != (const __WNFloat16& _float16) const;
    WN_BOOL operator != (WN_FLOAT32 _float32) const;
    WN_BOOL operator != (WN_FLOAT64 _float64) const;

    WN_BOOL operator <= (WN_INT8 _integer8) const;
    WN_BOOL operator <= (WN_INT16 _integer16) const;
    WN_BOOL operator <= (WN_INT32 _integer32) const;
    WN_BOOL operator <= (WN_INT64 _integer64) const;
    WN_BOOL operator <= (WN_UINT8 _integer8) const;
    WN_BOOL operator <= (WN_UINT16 _integer16) const;
    WN_BOOL operator <= (WN_UINT32 _integer32) const;
    WN_BOOL operator <= (WN_UINT64 _integer64) const;
    WN_BOOL operator <= (const __WNFixed8& _fixed8) const;
    WN_BOOL operator <= (const __WNFixed16& _fixed16) const;
    WN_BOOL operator <= (const __WNFixed32& _fixed32) const;
    WN_BOOL operator <= (const __WNFixed64& _fixed64) const;
    WN_BOOL operator <= (const __WNFloat8& _float8) const;
    WN_BOOL operator <= (const __WNFloat16& _float16) const;
    WN_BOOL operator <= (WN_FLOAT32 _float32) const;
    WN_BOOL operator <= (WN_FLOAT64 _float64) const;

    WN_BOOL operator >= (WN_INT8 _integer8) const;
    WN_BOOL operator >= (WN_INT16 _integer16) const;
    WN_BOOL operator >= (WN_INT32 _integer32) const;
    WN_BOOL operator >= (WN_INT64 _integer64) const;
    WN_BOOL operator >= (WN_UINT8 _integer8) const;
    WN_BOOL operator >= (WN_UINT16 _integer16) const;
    WN_BOOL operator >= (WN_UINT32 _integer32) const;
    WN_BOOL operator >= (WN_UINT64 _integer64) const;
    WN_BOOL operator >= (const __WNFixed8& _fixed8) const;
    WN_BOOL operator >= (const __WNFixed16& _fixed16) const;
    WN_BOOL operator >= (const __WNFixed32& _fixed32) const;
    WN_BOOL operator >= (const __WNFixed64& _fixed64) const;
    WN_BOOL operator >= (const __WNFloat8& _float8) const;
    WN_BOOL operator >= (const __WNFloat16& _float16) const;
    WN_BOOL operator >= (WN_FLOAT32 _float32) const;
    WN_BOOL operator >= (WN_FLOAT64 _float64) const;

    WN_BOOL operator < (WN_INT8 _integer8) const;
    WN_BOOL operator < (WN_INT16 _integer16) const;
    WN_BOOL operator < (WN_INT32 _integer32) const;
    WN_BOOL operator < (WN_INT64 _integer64) const;
    WN_BOOL operator < (WN_UINT8 _integer8) const;
    WN_BOOL operator < (WN_UINT16 _integer16) const;
    WN_BOOL operator < (WN_UINT32 _integer32) const;
    WN_BOOL operator < (WN_UINT64 _integer64) const;
    WN_BOOL operator < (const __WNFixed8& _fixed8) const;
    WN_BOOL operator < (const __WNFixed16& _fixed16) const;
    WN_BOOL operator < (const __WNFixed32& _fixed32) const;
    WN_BOOL operator < (const __WNFixed64& _fixed64) const;
    WN_BOOL operator < (const __WNFloat8& _float8) const;
    WN_BOOL operator < (const __WNFloat16& _float16) const;
    WN_BOOL operator < (WN_FLOAT32 _float32) const;
    WN_BOOL operator < (WN_FLOAT64 _float64) const;

    WN_BOOL operator > (WN_INT8 _integer8) const;
    WN_BOOL operator > (WN_INT16 _integer16) const;
    WN_BOOL operator > (WN_INT32 _integer32) const;
    WN_BOOL operator > (WN_INT64 _integer64) const;
    WN_BOOL operator > (WN_UINT8 _integer8) const;
    WN_BOOL operator > (WN_UINT16 _integer16) const;
    WN_BOOL operator > (WN_UINT32 _integer32) const;
    WN_BOOL operator > (WN_UINT64 _integer64) const;
    WN_BOOL operator > (const __WNFixed8& _fixed8) const;
    WN_BOOL operator > (const __WNFixed16& _fixed16) const;
    WN_BOOL operator > (const __WNFixed32& _fixed32) const;
    WN_BOOL operator > (const __WNFixed64& _fixed64) const;
    WN_BOOL operator > (const __WNFloat8& _float8) const;
    WN_BOOL operator > (const __WNFloat16& _float16) const;
    WN_BOOL operator > (WN_FLOAT32 _float32) const;
    WN_BOOL operator > (WN_FLOAT64 _float64) const;

    __WNFloat16& operator += (WN_INT8 _integer8);
    __WNFloat16& operator += (WN_INT16 _integer16);
    __WNFloat16& operator += (WN_INT32 _integer32);
    __WNFloat16& operator += (WN_INT64 _integer64);
    __WNFloat16& operator += (WN_UINT8 _integer8);
    __WNFloat16& operator += (WN_UINT16 _integer16);
    __WNFloat16& operator += (WN_UINT32 _integer32);
    __WNFloat16& operator += (WN_UINT64 _integer64);
    __WNFloat16& operator += (const __WNFixed8& _fixed8);
    __WNFloat16& operator += (const __WNFixed16& _fixed16);
    __WNFloat16& operator += (const __WNFixed32& _fixed32);
    __WNFloat16& operator += (const __WNFixed64& _fixed64);
    __WNFloat16& operator += (const __WNFloat8& _float8);
    __WNFloat16& operator += (const __WNFloat16& _float16);
    __WNFloat16& operator += (WN_FLOAT32 _float32);
    __WNFloat16& operator += (WN_FLOAT64 _float64);

    __WNFloat16& operator -= (WN_INT8 _integer8);
    __WNFloat16& operator -= (WN_INT16 _integer16);
    __WNFloat16& operator -= (WN_INT32 _integer32);
    __WNFloat16& operator -= (WN_INT64 _integer64);
    __WNFloat16& operator -= (WN_UINT8 _integer8);
    __WNFloat16& operator -= (WN_UINT16 _integer16);
    __WNFloat16& operator -= (WN_UINT32 _integer32);
    __WNFloat16& operator -= (WN_UINT64 _integer64);
    __WNFloat16& operator -= (const __WNFixed8& _fixed8);
    __WNFloat16& operator -= (const __WNFixed16& _fixed16);
    __WNFloat16& operator -= (const __WNFixed32& _fixed32);
    __WNFloat16& operator -= (const __WNFixed64& _fixed64);
    __WNFloat16& operator -= (const __WNFloat8& _float8);
    __WNFloat16& operator -= (const __WNFloat16& _float16);
    __WNFloat16& operator -= (WN_FLOAT32 _float32);
    __WNFloat16& operator -= (WN_FLOAT64 _float64);

    __WNFloat16& operator *= (WN_INT8 _integer8);
    __WNFloat16& operator *= (WN_INT16 _integer16);
    __WNFloat16& operator *= (WN_INT32 _integer32);
    __WNFloat16& operator *= (WN_INT64 _integer64);
    __WNFloat16& operator *= (WN_UINT8 _integer8);
    __WNFloat16& operator *= (WN_UINT16 _integer16);
    __WNFloat16& operator *= (WN_UINT32 _integer32);
    __WNFloat16& operator *= (WN_UINT64 _integer64);
    __WNFloat16& operator *= (const __WNFixed8& _fixed8);
    __WNFloat16& operator *= (const __WNFixed16& _fixed16);
    __WNFloat16& operator *= (const __WNFixed32& _fixed32);
    __WNFloat16& operator *= (const __WNFixed64& _fixed64);
    __WNFloat16& operator *= (const __WNFloat8& _float8);
    __WNFloat16& operator *= (const __WNFloat16& _float16);
    __WNFloat16& operator *= (WN_FLOAT32 _float32);
    __WNFloat16& operator *= (WN_FLOAT64 _float64);

    __WNFloat16& operator /= (WN_INT8 _integer8);
    __WNFloat16& operator /= (WN_INT16 _integer16);
    __WNFloat16& operator /= (WN_INT32 _integer32);
    __WNFloat16& operator /= (WN_INT64 _integer64);
    __WNFloat16& operator /= (WN_UINT8 _integer8);
    __WNFloat16& operator /= (WN_UINT16 _integer16);
    __WNFloat16& operator /= (WN_UINT32 _integer32);
    __WNFloat16& operator /= (WN_UINT64 _integer64);
    __WNFloat16& operator /= (const __WNFixed8& _fixed8);
    __WNFloat16& operator /= (const __WNFixed16& _fixed16);
    __WNFloat16& operator /= (const __WNFixed32& _fixed32);
    __WNFloat16& operator /= (const __WNFixed64& _fixed64);
    __WNFloat16& operator /= (const __WNFloat8& _float8);
    __WNFloat16& operator /= (const __WNFloat16& _float16);
    __WNFloat16& operator /= (WN_FLOAT32 _float32);
    __WNFloat16& operator /= (WN_FLOAT64 _float64);

    __WNFloat16 operator + (WN_INT8 _integer8) const;
    __WNFloat16 operator + (WN_INT16 _integer16) const;
    __WNFloat16 operator + (WN_INT32 _integer32) const;
    __WNFloat16 operator + (WN_INT64 _integer64) const;
    __WNFloat16 operator + (WN_UINT8 _integer8) const;
    __WNFloat16 operator + (WN_UINT16 _integer16) const;
    __WNFloat16 operator + (WN_UINT32 _integer32) const;
    __WNFloat16 operator + (WN_UINT64 _integer64) const;
    __WNFloat16 operator + (const __WNFixed8& _fixed8) const;
    __WNFloat16 operator + (const __WNFixed16& _fixed16) const;
    __WNFloat16 operator + (const __WNFixed32& _fixed32) const;
    __WNFloat16 operator + (const __WNFixed64& _fixed64) const;
    __WNFloat16 operator + (const __WNFloat8& _float8) const;
    __WNFloat16 operator + (const __WNFloat16& _float16) const;
    WN_FLOAT32 operator + (WN_FLOAT32 _float32) const;
    WN_FLOAT64 operator + (WN_FLOAT64 _float64) const;

    __WNFloat16 operator - (WN_INT8 _integer8) const;
    __WNFloat16 operator - (WN_INT16 _integer16) const;
    __WNFloat16 operator - (WN_INT32 _integer32) const;
    __WNFloat16 operator - (WN_INT64 _integer64) const;
    __WNFloat16 operator - (WN_UINT8 _integer8) const;
    __WNFloat16 operator - (WN_UINT16 _integer16) const;
    __WNFloat16 operator - (WN_UINT32 _integer32) const;
    __WNFloat16 operator - (WN_UINT64 _integer64) const;
    __WNFloat16 operator - (const __WNFixed8& _fixed8) const;
    __WNFloat16 operator - (const __WNFixed16& _fixed16) const;
    __WNFloat16 operator - (const __WNFixed32& _fixed32) const;
    __WNFloat16 operator - (const __WNFixed64& _fixed64) const;
    __WNFloat16 operator - (const __WNFloat8& _float8) const;
    __WNFloat16 operator - (const __WNFloat16& _float16) const;
    WN_FLOAT32 operator - (WN_FLOAT32 _float32) const;
    WN_FLOAT64 operator - (WN_FLOAT64 _float64) const;

    __WNFloat16 operator * (WN_INT8 _integer8) const;
    __WNFloat16 operator * (WN_INT16 _integer16) const;
    __WNFloat16 operator * (WN_INT32 _integer32) const;
    __WNFloat16 operator * (WN_INT64 _integer64) const;
    __WNFloat16 operator * (WN_UINT8 _integer8) const;
    __WNFloat16 operator * (WN_UINT16 _integer16) const;
    __WNFloat16 operator * (WN_UINT32 _integer32) const;
    __WNFloat16 operator * (WN_UINT64 _integer64) const;
    __WNFloat16 operator * (const __WNFixed8& _fixed8) const;
    __WNFloat16 operator * (const __WNFixed16& _fixed16) const;
    __WNFloat16 operator * (const __WNFixed32& _fixed32) const;
    __WNFloat16 operator * (const __WNFixed64& _fixed64) const;
    __WNFloat16 operator * (const __WNFloat8& _float8) const;
    __WNFloat16 operator * (const __WNFloat16& _float16) const;
    WN_FLOAT32 operator * (WN_FLOAT32 _float32) const;
    WN_FLOAT64 operator * (WN_FLOAT64 _float64) const;

    __WNFloat16 operator / (WN_INT8 _integer8) const;
    __WNFloat16 operator / (WN_INT16 _integer16) const;
    __WNFloat16 operator / (WN_INT32 _integer32) const;
    __WNFloat16 operator / (WN_INT64 _integer64) const;
    __WNFloat16 operator / (WN_UINT8 _integer8) const;
    __WNFloat16 operator / (WN_UINT16 _integer16) const;
    __WNFloat16 operator / (WN_UINT32 _integer32) const;
    __WNFloat16 operator / (WN_UINT64 _integer64) const;
    __WNFloat16 operator / (const __WNFixed8& _fixed8) const;
    __WNFloat16 operator / (const __WNFixed16& _fixed16) const;
    __WNFloat16 operator / (const __WNFixed32& _fixed32) const;
    __WNFloat16 operator / (const __WNFixed64& _fixed64) const;
    __WNFloat16 operator / (const __WNFloat8& _float8) const;
    __WNFloat16 operator / (const __WNFloat16& _float16) const;
    WN_FLOAT32 operator / (WN_FLOAT32 _float32) const;
    WN_FLOAT64 operator / (WN_FLOAT64 _float64) const;

private:
    static WN_INT16 F32ToF16(WN_FLOAT32 _float32);
    static WN_FLOAT32 F16ToF32(WN_INT16 _float16);

public:
    WN_INT16 mBits; // stores 16 bit floating point http://en.wikipedia.org/wiki/Half-precision_floating-point_format
};

WN_BOOL operator == (WN_INT8 _integer8, const __WNFloat16& _float16);
WN_BOOL operator == (WN_INT16 _integer16, const __WNFloat16& _float16);
WN_BOOL operator == (WN_INT32 _integer32, const __WNFloat16& _float16);
WN_BOOL operator == (WN_INT64 _integer64, const __WNFloat16& _float16);
WN_BOOL operator == (WN_UINT8 _integer8, const __WNFloat16& _float16);
WN_BOOL operator == (WN_UINT16 _integer16, const __WNFloat16& _float16);
WN_BOOL operator == (WN_UINT32 _integer32, const __WNFloat16& _float16);
WN_BOOL operator == (WN_UINT64 _integer64, const __WNFloat16& _float16);
WN_BOOL operator == (WN_FLOAT32 _float32, const __WNFloat16& _float16);
WN_BOOL operator == (WN_FLOAT64 _float64, const __WNFloat16& _float16);

WN_BOOL operator != (WN_INT8 _integer8, const __WNFloat16& _float16);
WN_BOOL operator != (WN_INT16 _integer16, const __WNFloat16& _float16);
WN_BOOL operator != (WN_INT32 _integer32, const __WNFloat16& _float16);
WN_BOOL operator != (WN_INT64 _integer64, const __WNFloat16& _float16);
WN_BOOL operator != (WN_UINT8 _integer8, const __WNFloat16& _float16);
WN_BOOL operator != (WN_UINT16 _integer16, const __WNFloat16& _float16);
WN_BOOL operator != (WN_UINT32 _integer32, const __WNFloat16& _float16);
WN_BOOL operator != (WN_UINT64 _integer64, const __WNFloat16& _float16);
WN_BOOL operator != (WN_FLOAT32 _float32, const __WNFloat16& _float16);
WN_BOOL operator != (WN_FLOAT64 _float64, const __WNFloat16& _float16);

WN_BOOL operator <= (WN_INT8 _integer8, const __WNFloat16& _float16);
WN_BOOL operator <= (WN_INT16 _integer16, const __WNFloat16& _float16);
WN_BOOL operator <= (WN_INT32 _integer32, const __WNFloat16& _float16);
WN_BOOL operator <= (WN_INT64 _integer64, const __WNFloat16& _float16);
WN_BOOL operator <= (WN_UINT8 _integer8, const __WNFloat16& _float16);
WN_BOOL operator <= (WN_UINT16 _integer16, const __WNFloat16& _float16);
WN_BOOL operator <= (WN_UINT32 _integer32, const __WNFloat16& _float16);
WN_BOOL operator <= (WN_UINT64 _integer64, const __WNFloat16& _float16);
WN_BOOL operator <= (WN_FLOAT32 _float32, const __WNFloat16& _float16);
WN_BOOL operator <= (WN_FLOAT64 _float64, const __WNFloat16& _float16);

WN_BOOL operator >= (WN_INT8 _integer8, const __WNFloat16& _float16);
WN_BOOL operator >= (WN_INT16 _integer16, const __WNFloat16& _float16);
WN_BOOL operator >= (WN_INT32 _integer32, const __WNFloat16& _float16);
WN_BOOL operator >= (WN_INT64 _integer64, const __WNFloat16& _float16);
WN_BOOL operator >= (WN_UINT8 _integer8, const __WNFloat16& _float16);
WN_BOOL operator >= (WN_UINT16 _integer16, const __WNFloat16& _float16);
WN_BOOL operator >= (WN_UINT32 _integer32, const __WNFloat16& _float16);
WN_BOOL operator >= (WN_UINT64 _integer64, const __WNFloat16& _float16);
WN_BOOL operator >= (WN_FLOAT32 _float32, const __WNFloat16& _float16);
WN_BOOL operator >= (WN_FLOAT64 _float64, const __WNFloat16& _float16);

WN_BOOL operator < (WN_INT8 _integer8, const __WNFloat16& _float16);
WN_BOOL operator < (WN_INT16 _integer16, const __WNFloat16& _float16);
WN_BOOL operator < (WN_INT32 _integer32, const __WNFloat16& _float16);
WN_BOOL operator < (WN_INT64 _integer64, const __WNFloat16& _float16);
WN_BOOL operator < (WN_UINT8 _integer8, const __WNFloat16& _float16);
WN_BOOL operator < (WN_UINT16 _integer16, const __WNFloat16& _float16);
WN_BOOL operator < (WN_UINT32 _integer32, const __WNFloat16& _float16);
WN_BOOL operator < (WN_UINT64 _integer64, const __WNFloat16& _float16);
WN_BOOL operator < (WN_FLOAT32 _float32, const __WNFloat16& _float16);
WN_BOOL operator < (WN_FLOAT64 _float64, const __WNFloat16& _float16);

WN_BOOL operator > (WN_INT8 _integer8, const __WNFloat16& _float16);
WN_BOOL operator > (WN_INT16 _integer16, const __WNFloat16& _float16);
WN_BOOL operator > (WN_INT32 _integer32, const __WNFloat16& _float16);
WN_BOOL operator > (WN_INT64 _integer64, const __WNFloat16& _float16);
WN_BOOL operator > (WN_UINT8 _integer8, const __WNFloat16& _float16);
WN_BOOL operator > (WN_UINT16 _integer16, const __WNFloat16& _float16);
WN_BOOL operator > (WN_UINT32 _integer32, const __WNFloat16& _float16);
WN_BOOL operator > (WN_UINT64 _integer64, const __WNFloat16& _float16);
WN_BOOL operator > (WN_FLOAT32 _float32, const __WNFloat16& _float16);
WN_BOOL operator > (WN_FLOAT64 _float64, const __WNFloat16& _float16);

WN_INT8& operator += (WN_INT8& _integer8, const __WNFloat16& _float16);
WN_INT16& operator += (WN_INT16& _integer16, const __WNFloat16& _float16);
WN_INT32& operator += (WN_INT32& _integer32, const __WNFloat16& _float16);
WN_INT64& operator += (WN_INT64& _integer64, const __WNFloat16& _float16);
WN_UINT8& operator += (WN_UINT8& _integer8, const __WNFloat16& _float16);
WN_UINT16& operator += (WN_UINT16& _integer16, const __WNFloat16& _float16);
WN_UINT32& operator += (WN_UINT32& _integer32, const __WNFloat16& _float16);
WN_UINT64& operator += (WN_UINT64& _integer64, const __WNFloat16& _float16);
WN_FLOAT32& operator += (WN_FLOAT32& _float32, const __WNFloat16& _float16);
WN_FLOAT64& operator += (WN_FLOAT64& _float64, const __WNFloat16& _float16);

WN_INT8& operator -= (WN_INT8& _integer8, const __WNFloat16& _float16);
WN_INT16& operator -= (WN_INT16& _integer16, const __WNFloat16& _float16);
WN_INT32& operator -= (WN_INT32& _integer32, const __WNFloat16& _float16);
WN_INT64& operator -= (WN_INT64& _integer64, const __WNFloat16& _float16);
WN_UINT8& operator -= (WN_UINT8& _integer8, const __WNFloat16& _float16);
WN_UINT16& operator -= (WN_UINT16& _integer16, const __WNFloat16& _float16);
WN_UINT32& operator -= (WN_UINT32& _integer32, const __WNFloat16& _float16);
WN_UINT64& operator -= (WN_UINT64& _integer64, const __WNFloat16& _float16);
WN_FLOAT32& operator -= (WN_FLOAT32& _float32, const __WNFloat16& _float16);
WN_FLOAT64& operator -= (WN_FLOAT64& _float64, const __WNFloat16& _float16);

WN_INT8& operator *= (WN_INT8& _integer8, const __WNFloat16& _float16);
WN_INT16& operator *= (WN_INT16& _integer16, const __WNFloat16& _float16);
WN_INT32& operator *= (WN_INT32& _integer32, const __WNFloat16& _float16);
WN_INT64& operator *= (WN_INT64& _integer64, const __WNFloat16& _float16);
WN_UINT8& operator *= (WN_UINT8& _integer8, const __WNFloat16& _float16);
WN_UINT16& operator *= (WN_UINT16& _integer16, const __WNFloat16& _float16);
WN_UINT32& operator *= (WN_UINT32& _integer32, const __WNFloat16& _float16);
WN_UINT64& operator *= (WN_UINT64& _integer64, const __WNFloat16& _float16);
WN_FLOAT32& operator *= (WN_FLOAT32& _float32, const __WNFloat16& _float16);
WN_FLOAT64& operator *= (WN_FLOAT64& _float64, const __WNFloat16& _float16);

WN_INT8& operator /= (WN_INT8& _integer8, const __WNFloat16& _float16);
WN_INT16& operator /= (WN_INT16& _integer16, const __WNFloat16& _float16);
WN_INT32& operator /= (WN_INT32& _integer32, const __WNFloat16& _float16);
WN_INT64& operator /= (WN_INT64& _integer64, const __WNFloat16& _float16);
WN_UINT8& operator /= (WN_UINT8& _integer8, const __WNFloat16& _float16);
WN_UINT16& operator /= (WN_UINT16& _integer16, const __WNFloat16& _float16);
WN_UINT32& operator /= (WN_UINT32& _integer32, const __WNFloat16& _float16);
WN_UINT64& operator /= (WN_UINT64& _integer64, const __WNFloat16& _float16);
WN_FLOAT32& operator /= (WN_FLOAT32& _float32, const __WNFloat16& _float16);
WN_FLOAT64& operator /= (WN_FLOAT64& _float64, const __WNFloat16& _float16);

__WNFloat16 operator + (WN_INT8 _integer8, const __WNFloat16& _float16);
__WNFloat16 operator + (WN_INT16 _integer16, const __WNFloat16& _float16);
__WNFloat16 operator + (WN_INT32 _integer32, const __WNFloat16& _float16);
__WNFloat16 operator + (WN_INT64 _integer64, const __WNFloat16& _float16);
__WNFloat16 operator + (WN_UINT8 _integer8, const __WNFloat16& _float16);
__WNFloat16 operator + (WN_UINT16 _integer16, const __WNFloat16& _float16);
__WNFloat16 operator + (WN_UINT32 _integer32, const __WNFloat16& _float16);
__WNFloat16 operator + (WN_UINT64 _integer64, const __WNFloat16& _float16);
WN_FLOAT32 operator + (WN_FLOAT32 _float32, const __WNFloat16& _float16);
WN_FLOAT64 operator + (WN_FLOAT64 _float64, const __WNFloat16& _float16);

__WNFloat16 operator - (WN_INT8 _integer8, const __WNFloat16& _float16);
__WNFloat16 operator - (WN_INT16 _integer16, const __WNFloat16& _float16);
__WNFloat16 operator - (WN_INT32 _integer32, const __WNFloat16& _float16);
__WNFloat16 operator - (WN_INT64 _integer64, const __WNFloat16& _float16);
__WNFloat16 operator - (WN_UINT8 _integer8, const __WNFloat16& _float16);
__WNFloat16 operator - (WN_UINT16 _integer16, const __WNFloat16& _float16);
__WNFloat16 operator - (WN_UINT32 _integer32, const __WNFloat16& _float16);
__WNFloat16 operator - (WN_UINT64 _integer64, const __WNFloat16& _float16);
WN_FLOAT32 operator - (WN_FLOAT32 _float32, const __WNFloat16& _float16);
WN_FLOAT64 operator - (WN_FLOAT64 _float64, const __WNFloat16& _float16);

__WNFloat16 operator * (WN_INT8 _integer8, const __WNFloat16& _float16);
__WNFloat16 operator * (WN_INT16 _integer16, const __WNFloat16& _float16);
__WNFloat16 operator * (WN_INT32 _integer32, const __WNFloat16& _float16);
__WNFloat16 operator * (WN_INT64 _integer64, const __WNFloat16& _float16);
__WNFloat16 operator * (WN_UINT8 _integer8, const __WNFloat16& _float16);
__WNFloat16 operator * (WN_UINT16 _integer16, const __WNFloat16& _float16);
__WNFloat16 operator * (WN_UINT32 _integer32, const __WNFloat16& _float16);
__WNFloat16 operator * (WN_UINT64 _integer64, const __WNFloat16& _float16);
WN_FLOAT32 operator * (WN_FLOAT32 _float32, const __WNFloat16& _float16);
WN_FLOAT64 operator * (WN_FLOAT64 _float64, const __WNFloat16& _float16);

__WNFloat16 operator / (WN_INT8 _integer8, const __WNFloat16& _float16);
__WNFloat16 operator / (WN_INT16 _integer16, const __WNFloat16& _float16);
__WNFloat16 operator / (WN_INT32 _integer32, const __WNFloat16& _float16);
__WNFloat16 operator / (WN_INT64 _integer64, const __WNFloat16& _float16);
__WNFloat16 operator / (WN_UINT8 _integer8, const __WNFloat16& _float16);
__WNFloat16 operator / (WN_UINT16 _integer16, const __WNFloat16& _float16);
__WNFloat16 operator / (WN_UINT32 _integer32, const __WNFloat16& _float16);
__WNFloat16 operator / (WN_UINT64 _integer64, const __WNFloat16& _float16);
WN_FLOAT32 operator / (WN_FLOAT32 _float32, const __WNFloat16& _float16);
WN_FLOAT64 operator / (WN_FLOAT64 _float64, const __WNFloat16& _float16);

const __WNFloat8 __WNFloat8Epsilon = 4.8875809e-4f;
const __WNFloat8 __WNFloat8Max = 6.5504000e+4f;
const __WNFloat8 __WNFloat8Min = 6.1035156e-5f;

const __WNFloat16 __WNFloat16Epsilon = 4.8875809e-4f;
const __WNFloat16 __WNFloat16Max = 6.5504000e+4f;
const __WNFloat16 __WNFloat16Min = 6.1035156e-5f;

typedef __WNFloat8 WN_FLOAT8;
typedef __WNFloat16 WN_FLOAT16;

#define WN_FLOAT8_EPSILON __WNFloat8Epsilon
#define WN_FLOAT8_MAX __WNFloat8Max
#define WN_FLOAT8_MIN __WNFloat8Min

#define WN_FLOAT16_EPSILON __WNFloat16Epsilon
#define WN_FLOAT16_MAX __WNFloat16Max
#define WN_FLOAT16_MIN __WNFloat16Min

class __WNFixed8 {
public:
    WN_FORCE_INLINE __WNFixed8() {}
    explicit __WNFixed8(WN_INT8 _integer8);
    explicit __WNFixed8(WN_INT16 _integer16);
    explicit __WNFixed8(WN_INT32 _integer32);
    explicit __WNFixed8(WN_INT64 _integer64);
    explicit __WNFixed8(WN_UINT8 _integer8);
    explicit __WNFixed8(WN_UINT16 _integer16);
    explicit __WNFixed8(WN_UINT32 _integer32);
    explicit __WNFixed8(WN_UINT64 _integer64);
    __WNFixed8(const __WNFixed8& _fixed8);
    __WNFixed8(const __WNFixed16& _fixed16);
    __WNFixed8(const __WNFixed32& _fixed32);
    __WNFixed8(const __WNFixed64& _fixed64);
    explicit __WNFixed8(const __WNFloat8& _float8);
    explicit __WNFixed8(const __WNFloat16& _float16);
    explicit __WNFixed8(WN_FLOAT32 _float32);
    explicit __WNFixed8(WN_FLOAT64 _float64);

    operator WN_INT8 () const;
    operator WN_INT16 () const;
    operator WN_INT32 () const;
    operator WN_INT64 () const;
    operator WN_UINT8 () const;
    operator WN_UINT16 () const;
    operator WN_UINT32 () const;
    operator WN_UINT64 () const;
    operator WN_FLOAT32 () const;
    operator WN_FLOAT64 () const;

    __WNFixed8& operator = (WN_INT8 _integer8);
    __WNFixed8& operator = (WN_INT16 _integer16);
    __WNFixed8& operator = (WN_INT32 _integer32);
    __WNFixed8& operator = (WN_INT64 _integer64);
    __WNFixed8& operator = (WN_UINT8 _integer8);
    __WNFixed8& operator = (WN_UINT16 _integer16);
    __WNFixed8& operator = (WN_UINT32 _integer32);
    __WNFixed8& operator = (WN_UINT64 _integer64);
    __WNFixed8& operator = (const __WNFixed8& _fixed8);
    __WNFixed8& operator = (const __WNFixed16& _fixed16);
    __WNFixed8& operator = (const __WNFixed32& _fixed32);
    __WNFixed8& operator = (const __WNFixed64& _fixed64);
    __WNFixed8& operator = (const __WNFloat8& _float8);
    __WNFixed8& operator = (const __WNFloat16& _float16);
    __WNFixed8& operator = (WN_FLOAT32 _float32);
    __WNFixed8& operator = (WN_FLOAT64 _float64);

    __WNFixed8 operator + () const;
    __WNFixed8 operator - () const;

    __WNFixed8& operator ++ ();
    __WNFixed8 operator ++ (WN_INT32 _integer32);
    __WNFixed8& operator -- ();
    __WNFixed8 operator -- (WN_INT32 _integer32);

    WN_BOOL operator ! () const;

    WN_BOOL operator == (WN_INT8 _integer8) const;
    WN_BOOL operator == (WN_INT16 _integer16) const;
    WN_BOOL operator == (WN_INT32 _integer32) const;
    WN_BOOL operator == (WN_INT64 _integer64) const;
    WN_BOOL operator == (WN_UINT8 _integer8) const;
    WN_BOOL operator == (WN_UINT16 _integer16) const;
    WN_BOOL operator == (WN_UINT32 _integer32) const;
    WN_BOOL operator == (WN_UINT64 _integer64) const;
    WN_BOOL operator == (const __WNFixed8& _fixed8) const;
    WN_BOOL operator == (const __WNFixed16& _fixed16) const;
    WN_BOOL operator == (const __WNFixed32& _fixed32) const;
    WN_BOOL operator == (const __WNFixed64& _fixed64) const;
    WN_BOOL operator == (const __WNFloat8& _float8) const;
    WN_BOOL operator == (const __WNFloat16& _float16) const;
    WN_BOOL operator == (WN_FLOAT32 _float32) const;
    WN_BOOL operator == (WN_FLOAT64 _float64) const;

    WN_BOOL operator != (WN_INT8 _integer8) const;
    WN_BOOL operator != (WN_INT16 _integer16) const;
    WN_BOOL operator != (WN_INT32 _integer32) const;
    WN_BOOL operator != (WN_INT64 _integer64) const;
    WN_BOOL operator != (WN_UINT8 _integer8) const;
    WN_BOOL operator != (WN_UINT16 _integer16) const;
    WN_BOOL operator != (WN_UINT32 _integer32) const;
    WN_BOOL operator != (WN_UINT64 _integer64) const;
    WN_BOOL operator != (const __WNFixed8& _fixed8) const;
    WN_BOOL operator != (const __WNFixed16& _fixed16) const;
    WN_BOOL operator != (const __WNFixed32& _fixed32) const;
    WN_BOOL operator != (const __WNFixed64& _fixed64) const;
    WN_BOOL operator != (const __WNFloat8& _float8) const;
    WN_BOOL operator != (const __WNFloat16& _float16) const;
    WN_BOOL operator != (WN_FLOAT32 _float32) const;
    WN_BOOL operator != (WN_FLOAT64 _float64) const;

    WN_BOOL operator <= (WN_INT8 _integer8) const;
    WN_BOOL operator <= (WN_INT16 _integer16) const;
    WN_BOOL operator <= (WN_INT32 _integer32) const;
    WN_BOOL operator <= (WN_INT64 _integer64) const;
    WN_BOOL operator <= (WN_UINT8 _integer8) const;
    WN_BOOL operator <= (WN_UINT16 _integer16) const;
    WN_BOOL operator <= (WN_UINT32 _integer32) const;
    WN_BOOL operator <= (WN_UINT64 _integer64) const;
    WN_BOOL operator <= (const __WNFixed8& _fixed8) const;
    WN_BOOL operator <= (const __WNFixed16& _fixed16) const;
    WN_BOOL operator <= (const __WNFixed32& _fixed32) const;
    WN_BOOL operator <= (const __WNFixed64& _fixed64) const;
    WN_BOOL operator <= (const __WNFloat8& _float8) const;
    WN_BOOL operator <= (const __WNFloat16& _float16) const;
    WN_BOOL operator <= (WN_FLOAT32 _float32) const;
    WN_BOOL operator <= (WN_FLOAT64 _float64) const;

    WN_BOOL operator >= (WN_INT8 _integer8) const;
    WN_BOOL operator >= (WN_INT16 _integer16) const;
    WN_BOOL operator >= (WN_INT32 _integer32) const;
    WN_BOOL operator >= (WN_INT64 _integer64) const;
    WN_BOOL operator >= (WN_UINT8 _integer8) const;
    WN_BOOL operator >= (WN_UINT16 _integer16) const;
    WN_BOOL operator >= (WN_UINT32 _integer32) const;
    WN_BOOL operator >= (WN_UINT64 _integer64) const;
    WN_BOOL operator >= (const __WNFixed8& _fixed8) const;
    WN_BOOL operator >= (const __WNFixed16& _fixed16) const;
    WN_BOOL operator >= (const __WNFixed32& _fixed32) const;
    WN_BOOL operator >= (const __WNFixed64& _fixed64) const;
    WN_BOOL operator >= (const __WNFloat8& _float8) const;
    WN_BOOL operator >= (const __WNFloat16& _float16) const;
    WN_BOOL operator >= (WN_FLOAT32 _float32) const;
    WN_BOOL operator >= (WN_FLOAT64 _float64) const;

    WN_BOOL operator < (WN_INT8 _integer8) const;
    WN_BOOL operator < (WN_INT16 _integer16) const;
    WN_BOOL operator < (WN_INT32 _integer32) const;
    WN_BOOL operator < (WN_INT64 _integer64) const;
    WN_BOOL operator < (WN_UINT8 _integer8) const;
    WN_BOOL operator < (WN_UINT16 _integer16) const;
    WN_BOOL operator < (WN_UINT32 _integer32) const;
    WN_BOOL operator < (WN_UINT64 _integer64) const;
    WN_BOOL operator < (const __WNFixed8& _fixed8) const;
    WN_BOOL operator < (const __WNFixed16& _fixed16) const;
    WN_BOOL operator < (const __WNFixed32& _fixed32) const;
    WN_BOOL operator < (const __WNFixed64& _fixed64) const;
    WN_BOOL operator < (const __WNFloat8& _float8) const;
    WN_BOOL operator < (const __WNFloat16& _float16) const;
    WN_BOOL operator < (WN_FLOAT32 _float32) const;
    WN_BOOL operator < (WN_FLOAT64 _float64) const;

    WN_BOOL operator > (WN_INT8 _integer8) const;
    WN_BOOL operator > (WN_INT16 _integer16) const;
    WN_BOOL operator > (WN_INT32 _integer32) const;
    WN_BOOL operator > (WN_INT64 _integer64) const;
    WN_BOOL operator > (WN_UINT8 _integer8) const;
    WN_BOOL operator > (WN_UINT16 _integer16) const;
    WN_BOOL operator > (WN_UINT32 _integer32) const;
    WN_BOOL operator > (WN_UINT64 _integer64) const;
    WN_BOOL operator > (const __WNFixed8& _fixed8) const;
    WN_BOOL operator > (const __WNFixed16& _fixed16) const;
    WN_BOOL operator > (const __WNFixed32& _fixed32) const;
    WN_BOOL operator > (const __WNFixed64& _fixed64) const;
    WN_BOOL operator > (const __WNFloat8& _float8) const;
    WN_BOOL operator > (const __WNFloat16& _float16) const;
    WN_BOOL operator > (WN_FLOAT32 _float32) const;
    WN_BOOL operator > (WN_FLOAT64 _float64) const;

    __WNFixed8& operator += (WN_INT8 _integer8);
    __WNFixed8& operator += (WN_INT16 _integer16);
    __WNFixed8& operator += (WN_INT32 _integer32);
    __WNFixed8& operator += (WN_INT64 _integer64);
    __WNFixed8& operator += (WN_UINT8 _integer8);
    __WNFixed8& operator += (WN_UINT16 _integer16);
    __WNFixed8& operator += (WN_UINT32 _integer32);
    __WNFixed8& operator += (WN_UINT64 _integer64);
    __WNFixed8& operator += (const __WNFixed8& _fixed8);
    __WNFixed8& operator += (const __WNFixed16& _fixed16);
    __WNFixed8& operator += (const __WNFixed32& _fixed32);
    __WNFixed8& operator += (const __WNFixed64& _fixed64);
    __WNFixed8& operator += (const __WNFloat8& _float8);
    __WNFixed8& operator += (const __WNFloat16& _float16);
    __WNFixed8& operator += (WN_FLOAT32 _float32);
    __WNFixed8& operator += (WN_FLOAT64 _float64);

    __WNFixed8& operator -= (WN_INT8 _integer8);
    __WNFixed8& operator -= (WN_INT16 _integer16);
    __WNFixed8& operator -= (WN_INT32 _integer32);
    __WNFixed8& operator -= (WN_INT64 _integer64);
    __WNFixed8& operator -= (WN_UINT8 _integer8);
    __WNFixed8& operator -= (WN_UINT16 _integer16);
    __WNFixed8& operator -= (WN_UINT32 _integer32);
    __WNFixed8& operator -= (WN_UINT64 _integer64);
    __WNFixed8& operator -= (const __WNFixed8& _fixed8);
    __WNFixed8& operator -= (const __WNFixed16& _fixed16);
    __WNFixed8& operator -= (const __WNFixed32& _fixed32);
    __WNFixed8& operator -= (const __WNFixed64& _fixed64);
    __WNFixed8& operator -= (const __WNFloat8& _float8);
    __WNFixed8& operator -= (const __WNFloat16& _float16);
    __WNFixed8& operator -= (WN_FLOAT32 _float32);
    __WNFixed8& operator -= (WN_FLOAT64 _float64);

    __WNFixed8& operator *= (WN_INT8 _integer8);
    __WNFixed8& operator *= (WN_INT16 _integer16);
    __WNFixed8& operator *= (WN_INT32 _integer32);
    __WNFixed8& operator *= (WN_INT64 _integer64);
    __WNFixed8& operator *= (WN_UINT8 _integer8);
    __WNFixed8& operator *= (WN_UINT16 _integer16);
    __WNFixed8& operator *= (WN_UINT32 _integer32);
    __WNFixed8& operator *= (WN_UINT64 _integer64);
    __WNFixed8& operator *= (const __WNFixed8& _fixed8);
    __WNFixed8& operator *= (const __WNFixed16& _fixed16);
    __WNFixed8& operator *= (const __WNFixed32& _fixed32);
    __WNFixed8& operator *= (const __WNFixed64& _fixed64);
    __WNFixed8& operator *= (const __WNFloat8& _float8);
    __WNFixed8& operator *= (const __WNFloat16& _float16);
    __WNFixed8& operator *= (WN_FLOAT32 _float32);
    __WNFixed8& operator *= (WN_FLOAT64 _float64);

    __WNFixed8& operator /= (WN_INT8 _integer8);
    __WNFixed8& operator /= (WN_INT16 _integer16);
    __WNFixed8& operator /= (WN_INT32 _integer32);
    __WNFixed8& operator /= (WN_INT64 _integer64);
    __WNFixed8& operator /= (WN_UINT8 _integer8);
    __WNFixed8& operator /= (WN_UINT16 _integer16);
    __WNFixed8& operator /= (WN_UINT32 _integer32);
    __WNFixed8& operator /= (WN_UINT64 _integer64);
    __WNFixed8& operator /= (const __WNFixed8& _fixed8);
    __WNFixed8& operator /= (const __WNFixed16& _fixed16);
    __WNFixed8& operator /= (const __WNFixed32& _fixed32);
    __WNFixed8& operator /= (const __WNFixed64& _fixed64);
    __WNFixed8& operator /= (const __WNFloat8& _float8);
    __WNFixed8& operator /= (const __WNFloat16& _float16);
    __WNFixed8& operator /= (WN_FLOAT32 _float32);
    __WNFixed8& operator /= (WN_FLOAT64 _float64);

    __WNFixed8 operator + (WN_INT8 _integer8) const;
    __WNFixed8 operator + (WN_INT16 _integer16) const;
    __WNFixed8 operator + (WN_INT32 _integer32) const;
    __WNFixed8 operator + (WN_INT64 _integer64) const;
    __WNFixed8 operator + (WN_UINT8 _integer8) const;
    __WNFixed8 operator + (WN_UINT16 _integer16) const;
    __WNFixed8 operator + (WN_UINT32 _integer32) const;
    __WNFixed8 operator + (WN_UINT64 _integer64) const;
    __WNFixed8 operator + (const __WNFixed8& _fixed8) const;
    __WNFixed16 operator + (const __WNFixed16& _fixed16) const;
    __WNFixed32 operator + (const __WNFixed32& _fixed32) const;
    __WNFixed64 operator + (const __WNFixed64& _fixed64) const;
    __WNFloat8 operator + (const __WNFloat8& _float8) const;
    __WNFloat16 operator + (const __WNFloat16& _float16) const;
    WN_FLOAT32 operator + (WN_FLOAT32 _float32) const;
    WN_FLOAT64 operator + (WN_FLOAT64 _float64) const;

    __WNFixed8 operator - (WN_INT8 _integer8) const;
    __WNFixed8 operator - (WN_INT16 _integer16) const;
    __WNFixed8 operator - (WN_INT32 _integer32) const;
    __WNFixed8 operator - (WN_INT64 _integer64) const;
    __WNFixed8 operator - (WN_UINT8 _integer8) const;
    __WNFixed8 operator - (WN_UINT16 _integer16) const;
    __WNFixed8 operator - (WN_UINT32 _integer32) const;
    __WNFixed8 operator - (WN_UINT64 _integer64) const;
    __WNFixed8 operator - (const __WNFixed8& _fixed8) const;
    __WNFixed16 operator - (const __WNFixed16& _fixed16) const;
    __WNFixed32 operator - (const __WNFixed32& _fixed32) const;
    __WNFixed64 operator - (const __WNFixed64& _fixed64) const;
    __WNFloat8 operator - (const __WNFloat8& _float8) const;
    __WNFloat16 operator - (const __WNFloat16& _float16) const;
    WN_FLOAT32 operator - (WN_FLOAT32 _float32) const;
    WN_FLOAT64 operator - (WN_FLOAT64 _float64) const;

    __WNFixed8 operator * (WN_INT8 _integer8) const;
    __WNFixed8 operator * (WN_INT16 _integer16) const;
    __WNFixed8 operator * (WN_INT32 _integer32) const;
    __WNFixed8 operator * (WN_INT64 _integer64) const;
    __WNFixed8 operator * (WN_UINT8 _integer8) const;
    __WNFixed8 operator * (WN_UINT16 _integer16) const;
    __WNFixed8 operator * (WN_UINT32 _integer32) const;
    __WNFixed8 operator * (WN_UINT64 _integer64) const;
    __WNFixed8 operator * (const __WNFixed8& _fixed8) const;
    __WNFixed16 operator * (const __WNFixed16& _fixed16) const;
    __WNFixed32 operator * (const __WNFixed32& _fixed32) const;
    __WNFixed64 operator * (const __WNFixed64& _fixed64) const;
    __WNFloat8 operator * (const __WNFloat8& _float8) const;
    __WNFloat16 operator * (const __WNFloat16& _float16) const;
    WN_FLOAT32 operator * (WN_FLOAT32 _float32) const;
    WN_FLOAT64 operator * (WN_FLOAT64 _float64) const;

    __WNFixed8 operator / (WN_INT8 _integer8) const;
    __WNFixed8 operator / (WN_INT16 _integer16) const;
    __WNFixed8 operator / (WN_INT32 _integer32) const;
    __WNFixed8 operator / (WN_INT64 _integer64) const;
    __WNFixed8 operator / (WN_UINT8 _integer8) const;
    __WNFixed8 operator / (WN_UINT16 _integer16) const;
    __WNFixed8 operator / (WN_UINT32 _integer32) const;
    __WNFixed8 operator / (WN_UINT64 _integer64) const;
    __WNFixed8 operator / (const __WNFixed8& _fixed8) const;
    __WNFixed16 operator / (const __WNFixed16& _fixed16) const;
    __WNFixed32 operator / (const __WNFixed32& _fixed32) const;
    __WNFixed64 operator / (const __WNFixed64& _fixed64) const;
    __WNFloat8 operator / (const __WNFloat8& _float8) const;
    __WNFloat16 operator / (const __WNFloat16& _float16) const;
    WN_FLOAT32 operator / (WN_FLOAT32 _float32) const;
    WN_FLOAT64 operator / (WN_FLOAT64 _float64) const;

public:
    WN_INT8 mBits; // stores 8 bit fixed point number in form 1:3:4
};

WN_BOOL operator == (WN_INT8 _integer8, const __WNFixed8& _fixed8);
WN_BOOL operator == (WN_INT16 _integer16, const __WNFixed8& _fixed8);
WN_BOOL operator == (WN_INT32 _integer32, const __WNFixed8& _fixed8);
WN_BOOL operator == (WN_INT64 _integer64, const __WNFixed8& _fixed8);
WN_BOOL operator == (WN_UINT8 _integer8, const __WNFixed8& _fixed8);
WN_BOOL operator == (WN_UINT16 _integer16, const __WNFixed8& _fixed8);
WN_BOOL operator == (WN_UINT32 _integer32, const __WNFixed8& _fixed8);
WN_BOOL operator == (WN_UINT64 _integer64, const __WNFixed8& _fixed8);
WN_BOOL operator == (WN_FLOAT32 _float32, const __WNFixed8& _fixed8);
WN_BOOL operator == (WN_FLOAT64 _float64, const __WNFixed8& _fixed8);

WN_BOOL operator != (WN_INT8 _integer8, const __WNFixed8& _fixed8);
WN_BOOL operator != (WN_INT16 _integer16, const __WNFixed8& _fixed8);
WN_BOOL operator != (WN_INT32 _integer32, const __WNFixed8& _fixed8);
WN_BOOL operator != (WN_INT64 _integer64, const __WNFixed8& _fixed8);
WN_BOOL operator != (WN_UINT8 _integer8, const __WNFixed8& _fixed8);
WN_BOOL operator != (WN_UINT16 _integer16, const __WNFixed8& _fixed8);
WN_BOOL operator != (WN_UINT32 _integer32, const __WNFixed8& _fixed8);
WN_BOOL operator != (WN_UINT64 _integer64, const __WNFixed8& _fixed8);
WN_BOOL operator != (WN_FLOAT32 _float32, const __WNFixed8& _fixed8);
WN_BOOL operator != (WN_FLOAT64 _float64, const __WNFixed8& _fixed8);

WN_BOOL operator <= (WN_INT8 _integer8, const __WNFixed8& _fixed8);
WN_BOOL operator <= (WN_INT16 _integer16, const __WNFixed8& _fixed8);
WN_BOOL operator <= (WN_INT32 _integer32, const __WNFixed8& _fixed8);
WN_BOOL operator <= (WN_INT64 _integer64, const __WNFixed8& _fixed8);
WN_BOOL operator <= (WN_UINT8 _integer8, const __WNFixed8& _fixed8);
WN_BOOL operator <= (WN_UINT16 _integer16, const __WNFixed8& _fixed8);
WN_BOOL operator <= (WN_UINT32 _integer32, const __WNFixed8& _fixed8);
WN_BOOL operator <= (WN_UINT64 _integer64, const __WNFixed8& _fixed8);
WN_BOOL operator <= (WN_FLOAT32 _float32, const __WNFixed8& _fixed8);
WN_BOOL operator <= (WN_FLOAT64 _float64, const __WNFixed8& _fixed8);

WN_BOOL operator >= (WN_INT8 _integer8, const __WNFixed8& _fixed8);
WN_BOOL operator >= (WN_INT16 _integer16, const __WNFixed8& _fixed8);
WN_BOOL operator >= (WN_INT32 _integer32, const __WNFixed8& _fixed8);
WN_BOOL operator >= (WN_INT64 _integer64, const __WNFixed8& _fixed8);
WN_BOOL operator >= (WN_UINT8 _integer8, const __WNFixed8& _fixed8);
WN_BOOL operator >= (WN_UINT16 _integer16, const __WNFixed8& _fixed8);
WN_BOOL operator >= (WN_UINT32 _integer32, const __WNFixed8& _fixed8);
WN_BOOL operator >= (WN_UINT64 _integer64, const __WNFixed8& _fixed8);
WN_BOOL operator >= (WN_FLOAT32 _float32, const __WNFixed8& _fixed8);
WN_BOOL operator >= (WN_FLOAT64 _float64, const __WNFixed8& _fixed8);

WN_BOOL operator < (WN_INT8 _integer8, const __WNFixed8& _fixed8);
WN_BOOL operator < (WN_INT16 _integer16, const __WNFixed8& _fixed8);
WN_BOOL operator < (WN_INT32 _integer32, const __WNFixed8& _fixed8);
WN_BOOL operator < (WN_INT64 _integer64, const __WNFixed8& _fixed8);
WN_BOOL operator < (WN_UINT8 _integer8, const __WNFixed8& _fixed8);
WN_BOOL operator < (WN_UINT16 _integer16, const __WNFixed8& _fixed8);
WN_BOOL operator < (WN_UINT32 _integer32, const __WNFixed8& _fixed8);
WN_BOOL operator < (WN_UINT64 _integer64, const __WNFixed8& _fixed8);
WN_BOOL operator < (WN_FLOAT32 _float32, const __WNFixed8& _fixed8);
WN_BOOL operator < (WN_FLOAT64 _float64, const __WNFixed8& _fixed8);

WN_BOOL operator > (WN_INT8 _integer8, const __WNFixed8& _fixed8);
WN_BOOL operator > (WN_INT16 _integer16, const __WNFixed8& _fixed8);
WN_BOOL operator > (WN_INT32 _integer32, const __WNFixed8& _fixed8);
WN_BOOL operator > (WN_INT64 _integer64, const __WNFixed8& _fixed8);
WN_BOOL operator > (WN_UINT8 _integer8, const __WNFixed8& _fixed8);
WN_BOOL operator > (WN_UINT16 _integer16, const __WNFixed8& _fixed8);
WN_BOOL operator > (WN_UINT32 _integer32, const __WNFixed8& _fixed8);
WN_BOOL operator > (WN_UINT64 _integer64, const __WNFixed8& _fixed8);
WN_BOOL operator > (WN_FLOAT32 _float32, const __WNFixed8& _fixed8);
WN_BOOL operator > (WN_FLOAT64 _float64, const __WNFixed8& _fixed8);

WN_INT8& operator += (WN_INT8& _integer8, const __WNFixed8& _fixed8);
WN_INT16& operator += (WN_INT16& _integer16, const __WNFixed8& _fixed8);
WN_INT32& operator += (WN_INT32& _integer32, const __WNFixed8& _fixed8);
WN_INT64& operator += (WN_INT64& _integer64, const __WNFixed8& _fixed8);
WN_UINT8& operator += (WN_UINT8& _integer8, const __WNFixed8& _fixed8);
WN_UINT16& operator += (WN_UINT16& _integer16, const __WNFixed8& _fixed8);
WN_UINT32& operator += (WN_UINT32& _integer32, const __WNFixed8& _fixed8);
WN_UINT64& operator += (WN_UINT64& _integer64, const __WNFixed8& _fixed8);
WN_FLOAT32& operator += (WN_FLOAT32& _float32, const __WNFixed8& _fixed8);
WN_FLOAT64& operator += (WN_FLOAT64& _float64, const __WNFixed8& _fixed8);

WN_INT8& operator -= (WN_INT8& _integer8, const __WNFixed8& _fixed8);
WN_INT16& operator -= (WN_INT16& _integer16, const __WNFixed8& _fixed8);
WN_INT32& operator -= (WN_INT32& _integer32, const __WNFixed8& _fixed8);
WN_INT64& operator -= (WN_INT64& _integer64, const __WNFixed8& _fixed8);
WN_UINT8& operator -= (WN_UINT8& _integer8, const __WNFixed8& _fixed8);
WN_UINT16& operator -= (WN_UINT16& _integer16, const __WNFixed8& _fixed8);
WN_UINT32& operator -= (WN_UINT32& _integer32, const __WNFixed8& _fixed8);
WN_UINT64& operator -= (WN_UINT64& _integer64, const __WNFixed8& _fixed8);
WN_FLOAT32& operator -= (WN_FLOAT32& _float32, const __WNFixed8& _fixed8);
WN_FLOAT64& operator -= (WN_FLOAT64& _float64, const __WNFixed8& _fixed8);

WN_INT8& operator *= (WN_INT8& _integer8, const __WNFixed8& _fixed8);
WN_INT16& operator *= (WN_INT16& _integer16, const __WNFixed8& _fixed8);
WN_INT32& operator *= (WN_INT32& _integer32, const __WNFixed8& _fixed8);
WN_INT64& operator *= (WN_INT64& _integer64, const __WNFixed8& _fixed8);
WN_UINT8& operator *= (WN_UINT8& _integer8, const __WNFixed8& _fixed8);
WN_UINT16& operator *= (WN_UINT16& _integer16, const __WNFixed8& _fixed8);
WN_UINT32& operator *= (WN_UINT32& _integer32, const __WNFixed8& _fixed8);
WN_UINT64& operator *= (WN_UINT64& _integer64, const __WNFixed8& _fixed8);
WN_FLOAT32& operator *= (WN_FLOAT32& _float32, const __WNFixed8& _fixed8);
WN_FLOAT64& operator *= (WN_FLOAT64& _float64, const __WNFixed8& _fixed8);

WN_INT8& operator /= (WN_INT8& _integer8, const __WNFixed8& _fixed8);
WN_INT16& operator /= (WN_INT16& _integer16, const __WNFixed8& _fixed8);
WN_INT32& operator /= (WN_INT32& _integer32, const __WNFixed8& _fixed8);
WN_INT64& operator /= (WN_INT64& _integer64, const __WNFixed8& _fixed8);
WN_UINT8& operator /= (WN_UINT8& _integer8, const __WNFixed8& _fixed8);
WN_UINT16& operator /= (WN_UINT16& _integer16, const __WNFixed8& _fixed8);
WN_UINT32& operator /= (WN_UINT32& _integer32, const __WNFixed8& _fixed8);
WN_UINT64& operator /= (WN_UINT64& _integer64, const __WNFixed8& _fixed8);
WN_FLOAT32& operator /= (WN_FLOAT32& _float32, const __WNFixed8& _fixed8);
WN_FLOAT64& operator /= (WN_FLOAT64& _float64, const __WNFixed8& _fixed8);

__WNFixed8 operator + (WN_INT8 _integer8, const __WNFixed8& _fixed8);
__WNFixed8 operator + (WN_INT16 _integer16, const __WNFixed8& _fixed8);
__WNFixed8 operator + (WN_INT32 _integer32, const __WNFixed8& _fixed8);
__WNFixed8 operator + (WN_INT64 _integer64, const __WNFixed8& _fixed8);
__WNFixed8 operator + (WN_UINT8 _integer8, const __WNFixed8& _fixed8);
__WNFixed8 operator + (WN_UINT16 _integer16, const __WNFixed8& _fixed8);
__WNFixed8 operator + (WN_UINT32 _integer32, const __WNFixed8& _fixed8);
__WNFixed8 operator + (WN_UINT64 _integer64, const __WNFixed8& _fixed8);
WN_FLOAT32 operator + (WN_FLOAT32 _float32, const __WNFixed8& _fixed8);
WN_FLOAT64 operator + (WN_FLOAT64 _float64, const __WNFixed8& _fixed8);

__WNFixed8 operator - (WN_INT8 _integer8, const __WNFixed8& _fixed8);
__WNFixed8 operator - (WN_INT16 _integer16, const __WNFixed8& _fixed8);
__WNFixed8 operator - (WN_INT32 _integer32, const __WNFixed8& _fixed8);
__WNFixed8 operator - (WN_INT64 _integer64, const __WNFixed8& _fixed8);
__WNFixed8 operator - (WN_UINT8 _integer8, const __WNFixed8& _fixed8);
__WNFixed8 operator - (WN_UINT16 _integer16, const __WNFixed8& _fixed8);
__WNFixed8 operator - (WN_UINT32 _integer32, const __WNFixed8& _fixed8);
__WNFixed8 operator - (WN_UINT64 _integer64, const __WNFixed8& _fixed8);
WN_FLOAT32 operator - (WN_FLOAT32 _float32, const __WNFixed8& _fixed8);
WN_FLOAT64 operator - (WN_FLOAT64 _float64, const __WNFixed8& _fixed8);

__WNFixed8 operator * (WN_INT8 _integer8, const __WNFixed8& _fixed8);
__WNFixed8 operator * (WN_INT16 _integer16, const __WNFixed8& _fixed8);
__WNFixed8 operator * (WN_INT32 _integer32, const __WNFixed8& _fixed8);
__WNFixed8 operator * (WN_INT64 _integer64, const __WNFixed8& _fixed8);
__WNFixed8 operator * (WN_UINT8 _integer8, const __WNFixed8& _fixed8);
__WNFixed8 operator * (WN_UINT16 _integer16, const __WNFixed8& _fixed8);
__WNFixed8 operator * (WN_UINT32 _integer32, const __WNFixed8& _fixed8);
__WNFixed8 operator * (WN_UINT64 _integer64, const __WNFixed8& _fixed8);
WN_FLOAT32 operator * (WN_FLOAT32 _float32, const __WNFixed8& _fixed8);
WN_FLOAT64 operator * (WN_FLOAT64 _float64, const __WNFixed8& _fixed8);

__WNFixed8 operator / (WN_INT8 _integer8, const __WNFixed8& _fixed8);
__WNFixed8 operator / (WN_INT16 _integer16, const __WNFixed8& _fixed8);
__WNFixed8 operator / (WN_INT32 _integer32, const __WNFixed8& _fixed8);
__WNFixed8 operator / (WN_INT64 _integer64, const __WNFixed8& _fixed8);
__WNFixed8 operator / (WN_UINT8 _integer8, const __WNFixed8& _fixed8);
__WNFixed8 operator / (WN_UINT16 _integer16, const __WNFixed8& _fixed8);
__WNFixed8 operator / (WN_UINT32 _integer32, const __WNFixed8& _fixed8);
__WNFixed8 operator / (WN_UINT64 _integer64, const __WNFixed8& _fixed8);
WN_FLOAT32 operator / (WN_FLOAT32 _float32, const __WNFixed8& _fixed8);
WN_FLOAT64 operator / (WN_FLOAT64 _float64, const __WNFixed8& _fixed8);

class __WNFixed16 {
public:
    WN_FORCE_INLINE __WNFixed16() {}
    explicit __WNFixed16(WN_INT8 _integer8);
    explicit __WNFixed16(WN_INT16 _integer16);
    explicit __WNFixed16(WN_INT32 _integer32);
    explicit __WNFixed16(WN_INT64 _integer64);
    explicit __WNFixed16(WN_UINT8 _integer8);
    explicit __WNFixed16(WN_UINT16 _integer16);
    explicit __WNFixed16(WN_UINT32 _integer32);
    explicit __WNFixed16(WN_UINT64 _integer64);
    __WNFixed16(const __WNFixed8& _fixed8);
    __WNFixed16(const __WNFixed16& _fixed16);
    __WNFixed16(const __WNFixed32& _fixed32);
    __WNFixed16(const __WNFixed64& _fixed64);
    explicit __WNFixed16(const __WNFloat8& _float8);
    explicit __WNFixed16(const __WNFloat16& _float16);
    explicit __WNFixed16(WN_FLOAT32 _float32);
    explicit __WNFixed16(WN_FLOAT64 _float64);

    operator WN_INT8 () const;
    operator WN_INT16 () const;
    operator WN_INT32 () const;
    operator WN_INT64 () const;
    operator WN_UINT8 () const;
    operator WN_UINT16 () const;
    operator WN_UINT32 () const;
    operator WN_UINT64 () const;
    operator WN_FLOAT32 () const;
    operator WN_FLOAT64 () const;

    __WNFixed16& operator = (WN_INT8 _integer8);
    __WNFixed16& operator = (WN_INT16 _integer16);
    __WNFixed16& operator = (WN_INT32 _integer32);
    __WNFixed16& operator = (WN_INT64 _integer64);
    __WNFixed16& operator = (WN_UINT8 _integer8);
    __WNFixed16& operator = (WN_UINT16 _integer16);
    __WNFixed16& operator = (WN_UINT32 _integer32);
    __WNFixed16& operator = (WN_UINT64 _integer64);
    __WNFixed16& operator = (const __WNFixed8& _fixed8);
    __WNFixed16& operator = (const __WNFixed16& _fixed16);
    __WNFixed16& operator = (const __WNFixed32& _fixed32);
    __WNFixed16& operator = (const __WNFixed64& _fixed64);
    __WNFixed16& operator = (const __WNFloat8& _float8);
    __WNFixed16& operator = (const __WNFloat16& _float16);
    __WNFixed16& operator = (WN_FLOAT32 _float32);
    __WNFixed16& operator = (WN_FLOAT64 _float64);

    __WNFixed16 operator + () const;
    __WNFixed16 operator - () const;

    __WNFixed16& operator ++ ();
    __WNFixed16 operator ++ (WN_INT32 _integer32);
    __WNFixed16& operator -- ();
    __WNFixed16 operator -- (WN_INT32 _integer32);

    WN_BOOL operator ! () const;

    WN_BOOL operator == (WN_INT8 _integer8) const;
    WN_BOOL operator == (WN_INT16 _integer16) const;
    WN_BOOL operator == (WN_INT32 _integer32) const;
    WN_BOOL operator == (WN_INT64 _integer64) const;
    WN_BOOL operator == (WN_UINT8 _integer8) const;
    WN_BOOL operator == (WN_UINT16 _integer16) const;
    WN_BOOL operator == (WN_UINT32 _integer32) const;
    WN_BOOL operator == (WN_UINT64 _integer64) const;
    WN_BOOL operator == (const __WNFixed8& _fixed8) const;
    WN_BOOL operator == (const __WNFixed16& _fixed16) const;
    WN_BOOL operator == (const __WNFixed32& _fixed32) const;
    WN_BOOL operator == (const __WNFixed64& _fixed64) const;
    WN_BOOL operator == (const __WNFloat8& _float8) const;
    WN_BOOL operator == (const __WNFloat16& _float16) const;
    WN_BOOL operator == (WN_FLOAT32 _float32) const;
    WN_BOOL operator == (WN_FLOAT64 _float64) const;

    WN_BOOL operator != (WN_INT8 _integer8) const;
    WN_BOOL operator != (WN_INT16 _integer16) const;
    WN_BOOL operator != (WN_INT32 _integer32) const;
    WN_BOOL operator != (WN_INT64 _integer64) const;
    WN_BOOL operator != (WN_UINT8 _integer8) const;
    WN_BOOL operator != (WN_UINT16 _integer16) const;
    WN_BOOL operator != (WN_UINT32 _integer32) const;
    WN_BOOL operator != (WN_UINT64 _integer64) const;
    WN_BOOL operator != (const __WNFixed8& _fixed8) const;
    WN_BOOL operator != (const __WNFixed16& _fixed16) const;
    WN_BOOL operator != (const __WNFixed32& _fixed32) const;
    WN_BOOL operator != (const __WNFixed64& _fixed64) const;
    WN_BOOL operator != (const __WNFloat8& _float8) const;
    WN_BOOL operator != (const __WNFloat16& _float16) const;
    WN_BOOL operator != (WN_FLOAT32 _float32) const;
    WN_BOOL operator != (WN_FLOAT64 _float64) const;

    WN_BOOL operator <= (WN_INT8 _integer8) const;
    WN_BOOL operator <= (WN_INT16 _integer16) const;
    WN_BOOL operator <= (WN_INT32 _integer32) const;
    WN_BOOL operator <= (WN_INT64 _integer64) const;
    WN_BOOL operator <= (WN_UINT8 _integer8) const;
    WN_BOOL operator <= (WN_UINT16 _integer16) const;
    WN_BOOL operator <= (WN_UINT32 _integer32) const;
    WN_BOOL operator <= (WN_UINT64 _integer64) const;
    WN_BOOL operator <= (const __WNFixed8& _fixed8) const;
    WN_BOOL operator <= (const __WNFixed16& _fixed16) const;
    WN_BOOL operator <= (const __WNFixed32& _fixed32) const;
    WN_BOOL operator <= (const __WNFixed64& _fixed64) const;
    WN_BOOL operator <= (const __WNFloat8& _float8) const;
    WN_BOOL operator <= (const __WNFloat16& _float16) const;
    WN_BOOL operator <= (WN_FLOAT32 _float32) const;
    WN_BOOL operator <= (WN_FLOAT64 _float64) const;

    WN_BOOL operator >= (WN_INT8 _integer8) const;
    WN_BOOL operator >= (WN_INT16 _integer16) const;
    WN_BOOL operator >= (WN_INT32 _integer32) const;
    WN_BOOL operator >= (WN_INT64 _integer64) const;
    WN_BOOL operator >= (WN_UINT8 _integer8) const;
    WN_BOOL operator >= (WN_UINT16 _integer16) const;
    WN_BOOL operator >= (WN_UINT32 _integer32) const;
    WN_BOOL operator >= (WN_UINT64 _integer64) const;
    WN_BOOL operator >= (const __WNFixed8& _fixed8) const;
    WN_BOOL operator >= (const __WNFixed16& _fixed16) const;
    WN_BOOL operator >= (const __WNFixed32& _fixed32) const;
    WN_BOOL operator >= (const __WNFixed64& _fixed64) const;
    WN_BOOL operator >= (const __WNFloat8& _float8) const;
    WN_BOOL operator >= (const __WNFloat16& _float16) const;
    WN_BOOL operator >= (WN_FLOAT32 _float32) const;
    WN_BOOL operator >= (WN_FLOAT64 _float64) const;

    WN_BOOL operator < (WN_INT8 _integer8) const;
    WN_BOOL operator < (WN_INT16 _integer16) const;
    WN_BOOL operator < (WN_INT32 _integer32) const;
    WN_BOOL operator < (WN_INT64 _integer64) const;
    WN_BOOL operator < (WN_UINT8 _integer8) const;
    WN_BOOL operator < (WN_UINT16 _integer16) const;
    WN_BOOL operator < (WN_UINT32 _integer32) const;
    WN_BOOL operator < (WN_UINT64 _integer64) const;
    WN_BOOL operator < (const __WNFixed8& _fixed8) const;
    WN_BOOL operator < (const __WNFixed16& _fixed16) const;
    WN_BOOL operator < (const __WNFixed32& _fixed32) const;
    WN_BOOL operator < (const __WNFixed64& _fixed64) const;
    WN_BOOL operator < (const __WNFloat8& _float8) const;
    WN_BOOL operator < (const __WNFloat16& _float16) const;
    WN_BOOL operator < (WN_FLOAT32 _float32) const;
    WN_BOOL operator < (WN_FLOAT64 _float64) const;

    WN_BOOL operator > (WN_INT8 _integer8) const;
    WN_BOOL operator > (WN_INT16 _integer16) const;
    WN_BOOL operator > (WN_INT32 _integer32) const;
    WN_BOOL operator > (WN_INT64 _integer64) const;
    WN_BOOL operator > (WN_UINT8 _integer8) const;
    WN_BOOL operator > (WN_UINT16 _integer16) const;
    WN_BOOL operator > (WN_UINT32 _integer32) const;
    WN_BOOL operator > (WN_UINT64 _integer64) const;
    WN_BOOL operator > (const __WNFixed8& _fixed8) const;
    WN_BOOL operator > (const __WNFixed16& _fixed16) const;
    WN_BOOL operator > (const __WNFixed32& _fixed32) const;
    WN_BOOL operator > (const __WNFixed64& _fixed64) const;
    WN_BOOL operator > (const __WNFloat8& _float8) const;
    WN_BOOL operator > (const __WNFloat16& _float16) const;
    WN_BOOL operator > (WN_FLOAT32 _float32) const;
    WN_BOOL operator > (WN_FLOAT64 _float64) const;

    __WNFixed16& operator += (WN_INT8 _integer8);
    __WNFixed16& operator += (WN_INT16 _integer16);
    __WNFixed16& operator += (WN_INT32 _integer32);
    __WNFixed16& operator += (WN_INT64 _integer64);
    __WNFixed16& operator += (WN_UINT8 _integer8);
    __WNFixed16& operator += (WN_UINT16 _integer16);
    __WNFixed16& operator += (WN_UINT32 _integer32);
    __WNFixed16& operator += (WN_UINT64 _integer64);
    __WNFixed16& operator += (const __WNFixed8& _fixed8);
    __WNFixed16& operator += (const __WNFixed16& _fixed16);
    __WNFixed16& operator += (const __WNFixed32& _fixed32);
    __WNFixed16& operator += (const __WNFixed64& _fixed64);
    __WNFixed16& operator += (const __WNFloat8& _float8);
    __WNFixed16& operator += (const __WNFloat16& _float16);
    __WNFixed16& operator += (WN_FLOAT32 _float32);
    __WNFixed16& operator += (WN_FLOAT64 _float64);

    __WNFixed16& operator -= (WN_INT8 _integer8);
    __WNFixed16& operator -= (WN_INT16 _integer16);
    __WNFixed16& operator -= (WN_INT32 _integer32);
    __WNFixed16& operator -= (WN_INT64 _integer64);
    __WNFixed16& operator -= (WN_UINT8 _integer8);
    __WNFixed16& operator -= (WN_UINT16 _integer16);
    __WNFixed16& operator -= (WN_UINT32 _integer32);
    __WNFixed16& operator -= (WN_UINT64 _integer64);
    __WNFixed16& operator -= (const __WNFixed8& _fixed8);
    __WNFixed16& operator -= (const __WNFixed16& _fixed16);
    __WNFixed16& operator -= (const __WNFixed32& _fixed32);
    __WNFixed16& operator -= (const __WNFixed64& _fixed64);
    __WNFixed16& operator -= (const __WNFloat8& _float8);
    __WNFixed16& operator -= (const __WNFloat16& _float16);
    __WNFixed16& operator -= (WN_FLOAT32 _float32);
    __WNFixed16& operator -= (WN_FLOAT64 _float64);

    __WNFixed16& operator *= (WN_INT8 _integer8);
    __WNFixed16& operator *= (WN_INT16 _integer16);
    __WNFixed16& operator *= (WN_INT32 _integer32);
    __WNFixed16& operator *= (WN_INT64 _integer64);
    __WNFixed16& operator *= (WN_UINT8 _integer8);
    __WNFixed16& operator *= (WN_UINT16 _integer16);
    __WNFixed16& operator *= (WN_UINT32 _integer32);
    __WNFixed16& operator *= (WN_UINT64 _integer64);
    __WNFixed16& operator *= (const __WNFixed8& _fixed8);
    __WNFixed16& operator *= (const __WNFixed16& _fixed16);
    __WNFixed16& operator *= (const __WNFixed32& _fixed32);
    __WNFixed16& operator *= (const __WNFixed64& _fixed64);
    __WNFixed16& operator *= (const __WNFloat8& _float8);
    __WNFixed16& operator *= (const __WNFloat16& _float16);
    __WNFixed16& operator *= (WN_FLOAT32 _float32);
    __WNFixed16& operator *= (WN_FLOAT64 _float64);

    __WNFixed16& operator /= (WN_INT8 _integer8);
    __WNFixed16& operator /= (WN_INT16 _integer16);
    __WNFixed16& operator /= (WN_INT32 _integer32);
    __WNFixed16& operator /= (WN_INT64 _integer64);
    __WNFixed16& operator /= (WN_UINT8 _integer8);
    __WNFixed16& operator /= (WN_UINT16 _integer16);
    __WNFixed16& operator /= (WN_UINT32 _integer32);
    __WNFixed16& operator /= (WN_UINT64 _integer64);
    __WNFixed16& operator /= (const __WNFixed8& _fixed8);
    __WNFixed16& operator /= (const __WNFixed16& _fixed16);
    __WNFixed16& operator /= (const __WNFixed32& _fixed32);
    __WNFixed16& operator /= (const __WNFixed64& _fixed64);
    __WNFixed16& operator /= (const __WNFloat8& _float8);
    __WNFixed16& operator /= (const __WNFloat16& _float16);
    __WNFixed16& operator /= (WN_FLOAT32 _float32);
    __WNFixed16& operator /= (WN_FLOAT64 _float64);

    __WNFixed16 operator + (WN_INT8 _integer8) const;
    __WNFixed16 operator + (WN_INT16 _integer16) const;
    __WNFixed16 operator + (WN_INT32 _integer32) const;
    __WNFixed16 operator + (WN_INT64 _integer64) const;
    __WNFixed16 operator + (WN_UINT8 _integer8) const;
    __WNFixed16 operator + (WN_UINT16 _integer16) const;
    __WNFixed16 operator + (WN_UINT32 _integer32) const;
    __WNFixed16 operator + (WN_UINT64 _integer64) const;
    __WNFixed16 operator + (const __WNFixed8& _fixed8) const;
    __WNFixed16 operator + (const __WNFixed16& _fixed16) const;
    __WNFixed32 operator + (const __WNFixed32& _fixed32) const;
    __WNFixed64 operator + (const __WNFixed64& _fixed64) const;
    __WNFloat8 operator + (const __WNFloat8& _float8) const;
    __WNFloat16 operator + (const __WNFloat16& _float16) const;
    WN_FLOAT32 operator + (WN_FLOAT32 _float32) const;
    WN_FLOAT64 operator + (WN_FLOAT64 _float64) const;

    __WNFixed16 operator - (WN_INT8 _integer8) const;
    __WNFixed16 operator - (WN_INT16 _integer16) const;
    __WNFixed16 operator - (WN_INT32 _integer32) const;
    __WNFixed16 operator - (WN_INT64 _integer64) const;
    __WNFixed16 operator - (WN_UINT8 _integer8) const;
    __WNFixed16 operator - (WN_UINT16 _integer16) const;
    __WNFixed16 operator - (WN_UINT32 _integer32) const;
    __WNFixed16 operator - (WN_UINT64 _integer64) const;
    __WNFixed16 operator - (const __WNFixed8& _fixed8) const;
    __WNFixed16 operator - (const __WNFixed16& _fixed16) const;
    __WNFixed32 operator - (const __WNFixed32& _fixed32) const;
    __WNFixed64 operator - (const __WNFixed64& _fixed64) const;
    __WNFloat8 operator - (const __WNFloat8& _float8) const;
    __WNFloat16 operator - (const __WNFloat16& _float16) const;
    WN_FLOAT32 operator - (WN_FLOAT32 _float32) const;
    WN_FLOAT64 operator - (WN_FLOAT64 _float64) const;

    __WNFixed16 operator * (WN_INT8 _integer8) const;
    __WNFixed16 operator * (WN_INT16 _integer16) const;
    __WNFixed16 operator * (WN_INT32 _integer32) const;
    __WNFixed16 operator * (WN_INT64 _integer64) const;
    __WNFixed16 operator * (WN_UINT8 _integer8) const;
    __WNFixed16 operator * (WN_UINT16 _integer16) const;
    __WNFixed16 operator * (WN_UINT32 _integer32) const;
    __WNFixed16 operator * (WN_UINT64 _integer64) const;
    __WNFixed16 operator * (const __WNFixed8& _fixed8) const;
    __WNFixed16 operator * (const __WNFixed16& _fixed16) const;
    __WNFixed32 operator * (const __WNFixed32& _fixed32) const;
    __WNFixed64 operator * (const __WNFixed64& _fixed64) const;
    __WNFloat8 operator * (const __WNFloat8& _float8) const;
    __WNFloat16 operator * (const __WNFloat16& _float16) const;
    WN_FLOAT32 operator * (WN_FLOAT32 _float32) const;
    WN_FLOAT64 operator * (WN_FLOAT64 _float64) const;

    __WNFixed16 operator / (WN_INT8 _integer8) const;
    __WNFixed16 operator / (WN_INT16 _integer16) const;
    __WNFixed16 operator / (WN_INT32 _integer32) const;
    __WNFixed16 operator / (WN_INT64 _integer64) const;
    __WNFixed16 operator / (WN_UINT8 _integer8) const;
    __WNFixed16 operator / (WN_UINT16 _integer16) const;
    __WNFixed16 operator / (WN_UINT32 _integer32) const;
    __WNFixed16 operator / (WN_UINT64 _integer64) const;
    __WNFixed16 operator / (const __WNFixed8& _fixed8) const;
    __WNFixed16 operator / (const __WNFixed16& _fixed16) const;
    __WNFixed32 operator / (const __WNFixed32& _fixed32) const;
    __WNFixed64 operator / (const __WNFixed64& _fixed64) const;
    __WNFloat8 operator / (const __WNFloat8& _float8) const;
    __WNFloat16 operator / (const __WNFloat16& _float16) const;
    WN_FLOAT32 operator / (WN_FLOAT32 _float32) const;
    WN_FLOAT64 operator / (WN_FLOAT64 _float64) const;

public:
    WN_INT16 mBits; // stores 16 bit fixed point number in form 1:7:8
};

WN_BOOL operator == (WN_INT8 _integer8, const __WNFixed16& _fixed16);
WN_BOOL operator == (WN_INT16 _integer16, const __WNFixed16& _fixed16);
WN_BOOL operator == (WN_INT32 _integer32, const __WNFixed16& _fixed16);
WN_BOOL operator == (WN_INT64 _integer64, const __WNFixed16& _fixed16);
WN_BOOL operator == (WN_UINT8 _integer8, const __WNFixed16& _fixed16);
WN_BOOL operator == (WN_UINT16 _integer16, const __WNFixed16& _fixed16);
WN_BOOL operator == (WN_UINT32 _integer32, const __WNFixed16& _fixed16);
WN_BOOL operator == (WN_UINT64 _integer64, const __WNFixed16& _fixed16);
WN_BOOL operator == (WN_FLOAT32 _float32, const __WNFixed16& _fixed16);
WN_BOOL operator == (WN_FLOAT64 _float64, const __WNFixed16& _fixed16);

WN_BOOL operator != (WN_INT8 _integer8, const __WNFixed16& _fixed16);
WN_BOOL operator != (WN_INT16 _integer16, const __WNFixed16& _fixed16);
WN_BOOL operator != (WN_INT32 _integer32, const __WNFixed16& _fixed16);
WN_BOOL operator != (WN_INT64 _integer64, const __WNFixed16& _fixed16);
WN_BOOL operator != (WN_UINT8 _integer8, const __WNFixed16& _fixed16);
WN_BOOL operator != (WN_UINT16 _integer16, const __WNFixed16& _fixed16);
WN_BOOL operator != (WN_UINT32 _integer32, const __WNFixed16& _fixed16);
WN_BOOL operator != (WN_UINT64 _integer64, const __WNFixed16& _fixed16);
WN_BOOL operator != (WN_FLOAT32 _float32, const __WNFixed16& _fixed16);
WN_BOOL operator != (WN_FLOAT64 _float64, const __WNFixed16& _fixed16);

WN_BOOL operator <= (WN_INT8 _integer8, const __WNFixed16& _fixed16);
WN_BOOL operator <= (WN_INT16 _integer16, const __WNFixed16& _fixed16);
WN_BOOL operator <= (WN_INT32 _integer32, const __WNFixed16& _fixed16);
WN_BOOL operator <= (WN_INT64 _integer64, const __WNFixed16& _fixed16);
WN_BOOL operator <= (WN_UINT8 _integer8, const __WNFixed16& _fixed16);
WN_BOOL operator <= (WN_UINT16 _integer16, const __WNFixed16& _fixed16);
WN_BOOL operator <= (WN_UINT32 _integer32, const __WNFixed16& _fixed16);
WN_BOOL operator <= (WN_UINT64 _integer64, const __WNFixed16& _fixed16);
WN_BOOL operator <= (WN_FLOAT32 _float32, const __WNFixed16& _fixed16);
WN_BOOL operator <= (WN_FLOAT64 _float64, const __WNFixed16& _fixed16);

WN_BOOL operator >= (WN_INT8 _integer8, const __WNFixed16& _fixed16);
WN_BOOL operator >= (WN_INT16 _integer16, const __WNFixed16& _fixed16);
WN_BOOL operator >= (WN_INT32 _integer32, const __WNFixed16& _fixed16);
WN_BOOL operator >= (WN_INT64 _integer64, const __WNFixed16& _fixed16);
WN_BOOL operator >= (WN_UINT8 _integer8, const __WNFixed16& _fixed16);
WN_BOOL operator >= (WN_UINT16 _integer16, const __WNFixed16& _fixed16);
WN_BOOL operator >= (WN_UINT32 _integer32, const __WNFixed16& _fixed16);
WN_BOOL operator >= (WN_UINT64 _integer64, const __WNFixed16& _fixed16);
WN_BOOL operator >= (WN_FLOAT32 _float32, const __WNFixed16& _fixed16);
WN_BOOL operator >= (WN_FLOAT64 _float64, const __WNFixed16& _fixed16);

WN_BOOL operator < (WN_INT8 _integer8, const __WNFixed16& _fixed16);
WN_BOOL operator < (WN_INT16 _integer16, const __WNFixed16& _fixed16);
WN_BOOL operator < (WN_INT32 _integer32, const __WNFixed16& _fixed16);
WN_BOOL operator < (WN_INT64 _integer64, const __WNFixed16& _fixed16);
WN_BOOL operator < (WN_UINT8 _integer8, const __WNFixed16& _fixed16);
WN_BOOL operator < (WN_UINT16 _integer16, const __WNFixed16& _fixed16);
WN_BOOL operator < (WN_UINT32 _integer32, const __WNFixed16& _fixed16);
WN_BOOL operator < (WN_UINT64 _integer64, const __WNFixed16& _fixed16);
WN_BOOL operator < (WN_FLOAT32 _float32, const __WNFixed16& _fixed16);
WN_BOOL operator < (WN_FLOAT64 _float64, const __WNFixed16& _fixed16);

WN_BOOL operator > (WN_INT8 _integer8, const __WNFixed16& _fixed16);
WN_BOOL operator > (WN_INT16 _integer16, const __WNFixed16& _fixed16);
WN_BOOL operator > (WN_INT32 _integer32, const __WNFixed16& _fixed16);
WN_BOOL operator > (WN_INT64 _integer64, const __WNFixed16& _fixed16);
WN_BOOL operator > (WN_UINT8 _integer8, const __WNFixed16& _fixed16);
WN_BOOL operator > (WN_UINT16 _integer16, const __WNFixed16& _fixed16);
WN_BOOL operator > (WN_UINT32 _integer32, const __WNFixed16& _fixed16);
WN_BOOL operator > (WN_UINT64 _integer64, const __WNFixed16& _fixed16);
WN_BOOL operator > (WN_FLOAT32 _float32, const __WNFixed16& _fixed16);
WN_BOOL operator > (WN_FLOAT64 _float64, const __WNFixed16& _fixed16);

WN_INT8& operator += (WN_INT8& _integer8, const __WNFixed16& _fixed16);
WN_INT16& operator += (WN_INT16& _integer16, const __WNFixed16& _fixed16);
WN_INT32& operator += (WN_INT32& _integer32, const __WNFixed16& _fixed16);
WN_INT64& operator += (WN_INT64& _integer64, const __WNFixed16& _fixed16);
WN_UINT8& operator += (WN_UINT8& _integer8, const __WNFixed16& _fixed16);
WN_UINT16& operator += (WN_UINT16& _integer16, const __WNFixed16& _fixed16);
WN_UINT32& operator += (WN_UINT32& _integer32, const __WNFixed16& _fixed16);
WN_UINT64& operator += (WN_UINT64& _integer64, const __WNFixed16& _fixed16);
WN_FLOAT32& operator += (WN_FLOAT32& _float32, const __WNFixed16& _fixed16);
WN_FLOAT64& operator += (WN_FLOAT64& _float64, const __WNFixed16& _fixed16);

WN_INT8& operator -= (WN_INT8& _integer8, const __WNFixed16& _fixed16);
WN_INT16& operator -= (WN_INT16& _integer16, const __WNFixed16& _fixed16);
WN_INT32& operator -= (WN_INT32& _integer32, const __WNFixed16& _fixed16);
WN_INT64& operator -= (WN_INT64& _integer64, const __WNFixed16& _fixed16);
WN_UINT8& operator -= (WN_UINT8& _integer8, const __WNFixed16& _fixed16);
WN_UINT16& operator -= (WN_UINT16& _integer16, const __WNFixed16& _fixed16);
WN_UINT32& operator -= (WN_UINT32& _integer32, const __WNFixed16& _fixed16);
WN_UINT64& operator -= (WN_UINT64& _integer64, const __WNFixed16& _fixed16);
WN_FLOAT32& operator -= (WN_FLOAT32& _float32, const __WNFixed16& _fixed16);
WN_FLOAT64& operator -= (WN_FLOAT64& _float64, const __WNFixed16& _fixed16);

WN_INT8& operator *= (WN_INT8& _integer8, const __WNFixed16& _fixed16);
WN_INT16& operator *= (WN_INT16& _integer16, const __WNFixed16& _fixed16);
WN_INT32& operator *= (WN_INT32& _integer32, const __WNFixed16& _fixed16);
WN_INT64& operator *= (WN_INT64& _integer64, const __WNFixed16& _fixed16);
WN_UINT8& operator *= (WN_UINT8& _integer8, const __WNFixed16& _fixed16);
WN_UINT16& operator *= (WN_UINT16& _integer16, const __WNFixed16& _fixed16);
WN_UINT32& operator *= (WN_UINT32& _integer32, const __WNFixed16& _fixed16);
WN_UINT64& operator *= (WN_UINT64& _integer64, const __WNFixed16& _fixed16);
WN_FLOAT32& operator *= (WN_FLOAT32& _float32, const __WNFixed16& _fixed16);
WN_FLOAT64& operator *= (WN_FLOAT64& _float64, const __WNFixed16& _fixed16);

WN_INT8& operator /= (WN_INT8& _integer8, const __WNFixed16& _fixed16);
WN_INT16& operator /= (WN_INT16& _integer16, const __WNFixed16& _fixed16);
WN_INT32& operator /= (WN_INT32& _integer32, const __WNFixed16& _fixed16);
WN_INT64& operator /= (WN_INT64& _integer64, const __WNFixed16& _fixed16);
WN_UINT8& operator /= (WN_UINT8& _integer8, const __WNFixed16& _fixed16);
WN_UINT16& operator /= (WN_UINT16& _integer16, const __WNFixed16& _fixed16);
WN_UINT32& operator /= (WN_UINT32& _integer32, const __WNFixed16& _fixed16);
WN_UINT64& operator /= (WN_UINT64& _integer64, const __WNFixed16& _fixed16);
WN_FLOAT32& operator /= (WN_FLOAT32& _float32, const __WNFixed16& _fixed16);
WN_FLOAT64& operator /= (WN_FLOAT64& _float64, const __WNFixed16& _fixed16);

__WNFixed16 operator + (WN_INT8 _integer8, const __WNFixed16& _fixed16);
__WNFixed16 operator + (WN_INT16 _integer16, const __WNFixed16& _fixed16);
__WNFixed16 operator + (WN_INT32 _integer32, const __WNFixed16& _fixed16);
__WNFixed16 operator + (WN_INT64 _integer64, const __WNFixed16& _fixed16);
__WNFixed16 operator + (WN_UINT8 _integer8, const __WNFixed16& _fixed16);
__WNFixed16 operator + (WN_UINT16 _integer16, const __WNFixed16& _fixed16);
__WNFixed16 operator + (WN_UINT32 _integer32, const __WNFixed16& _fixed16);
__WNFixed16 operator + (WN_UINT64 _integer64, const __WNFixed16& _fixed16);
WN_FLOAT32 operator + (WN_FLOAT32 _float32, const __WNFixed16& _fixed16);
WN_FLOAT64 operator + (WN_FLOAT64 _float64, const __WNFixed16& _fixed16);

__WNFixed16 operator - (WN_INT8 _integer8, const __WNFixed16& _fixed16);
__WNFixed16 operator - (WN_INT16 _integer16, const __WNFixed16& _fixed16);
__WNFixed16 operator - (WN_INT32 _integer32, const __WNFixed16& _fixed16);
__WNFixed16 operator - (WN_INT64 _integer64, const __WNFixed16& _fixed16);
__WNFixed16 operator - (WN_UINT8 _integer8, const __WNFixed16& _fixed16);
__WNFixed16 operator - (WN_UINT16 _integer16, const __WNFixed16& _fixed16);
__WNFixed16 operator - (WN_UINT32 _integer32, const __WNFixed16& _fixed16);
__WNFixed16 operator - (WN_UINT64 _integer64, const __WNFixed16& _fixed16);
WN_FLOAT32 operator - (WN_FLOAT32 _float32, const __WNFixed16& _fixed16);
WN_FLOAT64 operator - (WN_FLOAT64 _float64, const __WNFixed16& _fixed16);

__WNFixed16 operator * (WN_INT8 _integer8, const __WNFixed16& _fixed16);
__WNFixed16 operator * (WN_INT16 _integer16, const __WNFixed16& _fixed16);
__WNFixed16 operator * (WN_INT32 _integer32, const __WNFixed16& _fixed16);
__WNFixed16 operator * (WN_INT64 _integer64, const __WNFixed16& _fixed16);
__WNFixed16 operator * (WN_UINT8 _integer8, const __WNFixed16& _fixed16);
__WNFixed16 operator * (WN_UINT16 _integer16, const __WNFixed16& _fixed16);
__WNFixed16 operator * (WN_UINT32 _integer32, const __WNFixed16& _fixed16);
__WNFixed16 operator * (WN_UINT64 _integer64, const __WNFixed16& _fixed16);
WN_FLOAT32 operator * (WN_FLOAT32 _float32, const __WNFixed16& _fixed16);
WN_FLOAT64 operator * (WN_FLOAT64 _float64, const __WNFixed16& _fixed16);

__WNFixed16 operator / (WN_INT8 _integer8, const __WNFixed16& _fixed16);
__WNFixed16 operator / (WN_INT16 _integer16, const __WNFixed16& _fixed16);
__WNFixed16 operator / (WN_INT32 _integer32, const __WNFixed16& _fixed16);
__WNFixed16 operator / (WN_INT64 _integer64, const __WNFixed16& _fixed16);
__WNFixed16 operator / (WN_UINT8 _integer8, const __WNFixed16& _fixed16);
__WNFixed16 operator / (WN_UINT16 _integer16, const __WNFixed16& _fixed16);
__WNFixed16 operator / (WN_UINT32 _integer32, const __WNFixed16& _fixed16);
__WNFixed16 operator / (WN_UINT64 _integer64, const __WNFixed16& _fixed16);
WN_FLOAT32 operator / (WN_FLOAT32 _float32, const __WNFixed16& _fixed16);
WN_FLOAT64 operator / (WN_FLOAT64 _float64, const __WNFixed16& _fixed16);

class __WNFixed32 {
public:
    WN_FORCE_INLINE __WNFixed32() {}
    explicit __WNFixed32(WN_INT8 _integer8);
    explicit __WNFixed32(WN_INT16 _integer16);
    explicit __WNFixed32(WN_INT32 _integer32);
    explicit __WNFixed32(WN_INT64 _integer64);
    explicit __WNFixed32(WN_UINT8 _integer8);
    explicit __WNFixed32(WN_UINT16 _integer16);
    explicit __WNFixed32(WN_UINT32 _integer32);
    explicit __WNFixed32(WN_UINT64 _integer64);
    __WNFixed32(const __WNFixed8& _fixed8);
    __WNFixed32(const __WNFixed16& _fixed16);
    __WNFixed32(const __WNFixed32& _fixed32);
    __WNFixed32(const __WNFixed64& _fixed64);
    explicit __WNFixed32(const __WNFloat8& _float8);
    explicit __WNFixed32(const __WNFloat16& _float16);
    explicit __WNFixed32(WN_FLOAT32 _float32);
    explicit __WNFixed32(WN_FLOAT64 _float64);

    operator WN_INT8 () const;
    operator WN_INT16 () const;
    operator WN_INT32 () const;
    operator WN_INT64 () const;
    operator WN_UINT8 () const;
    operator WN_UINT16 () const;
    operator WN_UINT32 () const;
    operator WN_UINT64 () const;
    operator WN_FLOAT32 () const;
    operator WN_FLOAT64 () const;

    __WNFixed32& operator = (WN_INT8 _integer8);
    __WNFixed32& operator = (WN_INT16 _integer16);
    __WNFixed32& operator = (WN_INT32 _integer32);
    __WNFixed32& operator = (WN_INT64 _integer64);
    __WNFixed32& operator = (WN_UINT8 _integer8);
    __WNFixed32& operator = (WN_UINT16 _integer16);
    __WNFixed32& operator = (WN_UINT32 _integer32);
    __WNFixed32& operator = (WN_UINT64 _integer64);
    __WNFixed32& operator = (const __WNFixed8& _fixed8);
    __WNFixed32& operator = (const __WNFixed16& _fixed16);
    __WNFixed32& operator = (const __WNFixed32& _fixed32);
    __WNFixed32& operator = (const __WNFixed64& _fixed64);
    __WNFixed32& operator = (const __WNFloat8& _float8);
    __WNFixed32& operator = (const __WNFloat16& _float16);
    __WNFixed32& operator = (WN_FLOAT32 _float32);
    __WNFixed32& operator = (WN_FLOAT64 _float64);

    __WNFixed32 operator + () const;
    __WNFixed32 operator - () const;

    __WNFixed32& operator ++ ();
    __WNFixed32 operator ++ (WN_INT32 _integer32);
    __WNFixed32& operator -- ();
    __WNFixed32 operator -- (WN_INT32 _integer32);

    WN_BOOL operator ! () const;

    WN_BOOL operator == (WN_INT8 _integer8) const;
    WN_BOOL operator == (WN_INT16 _integer16) const;
    WN_BOOL operator == (WN_INT32 _integer32) const;
    WN_BOOL operator == (WN_INT64 _integer64) const;
    WN_BOOL operator == (WN_UINT8 _integer8) const;
    WN_BOOL operator == (WN_UINT16 _integer16) const;
    WN_BOOL operator == (WN_UINT32 _integer32) const;
    WN_BOOL operator == (WN_UINT64 _integer64) const;
    WN_BOOL operator == (const __WNFixed8& _fixed8) const;
    WN_BOOL operator == (const __WNFixed16& _fixed16) const;
    WN_BOOL operator == (const __WNFixed32& _fixed32) const;
    WN_BOOL operator == (const __WNFixed64& _fixed64) const;
    WN_BOOL operator == (const __WNFloat8& _float8) const;
    WN_BOOL operator == (const __WNFloat16& _float16) const;
    WN_BOOL operator == (WN_FLOAT32 _float32) const;
    WN_BOOL operator == (WN_FLOAT64 _float64) const;

    WN_BOOL operator != (WN_INT8 _integer8) const;
    WN_BOOL operator != (WN_INT16 _integer16) const;
    WN_BOOL operator != (WN_INT32 _integer32) const;
    WN_BOOL operator != (WN_INT64 _integer64) const;
    WN_BOOL operator != (WN_UINT8 _integer8) const;
    WN_BOOL operator != (WN_UINT16 _integer16) const;
    WN_BOOL operator != (WN_UINT32 _integer32) const;
    WN_BOOL operator != (WN_UINT64 _integer64) const;
    WN_BOOL operator != (const __WNFixed8& _fixed8) const;
    WN_BOOL operator != (const __WNFixed16& _fixed16) const;
    WN_BOOL operator != (const __WNFixed32& _fixed32) const;
    WN_BOOL operator != (const __WNFixed64& _fixed64) const;
    WN_BOOL operator != (const __WNFloat8& _float8) const;
    WN_BOOL operator != (const __WNFloat16& _float16) const;
    WN_BOOL operator != (WN_FLOAT32 _float32) const;
    WN_BOOL operator != (WN_FLOAT64 _float64) const;

    WN_BOOL operator <= (WN_INT8 _integer8) const;
    WN_BOOL operator <= (WN_INT16 _integer16) const;
    WN_BOOL operator <= (WN_INT32 _integer32) const;
    WN_BOOL operator <= (WN_INT64 _integer64) const;
    WN_BOOL operator <= (WN_UINT8 _integer8) const;
    WN_BOOL operator <= (WN_UINT16 _integer16) const;
    WN_BOOL operator <= (WN_UINT32 _integer32) const;
    WN_BOOL operator <= (WN_UINT64 _integer64) const;
    WN_BOOL operator <= (const __WNFixed8& _fixed8) const;
    WN_BOOL operator <= (const __WNFixed16& _fixed16) const;
    WN_BOOL operator <= (const __WNFixed32& _fixed32) const;
    WN_BOOL operator <= (const __WNFixed64& _fixed64) const;
    WN_BOOL operator <= (const __WNFloat8& _float8) const;
    WN_BOOL operator <= (const __WNFloat16& _float16) const;
    WN_BOOL operator <= (WN_FLOAT32 _float32) const;
    WN_BOOL operator <= (WN_FLOAT64 _float64) const;

    WN_BOOL operator >= (WN_INT8 _integer8) const;
    WN_BOOL operator >= (WN_INT16 _integer16) const;
    WN_BOOL operator >= (WN_INT32 _integer32) const;
    WN_BOOL operator >= (WN_INT64 _integer64) const;
    WN_BOOL operator >= (WN_UINT8 _integer8) const;
    WN_BOOL operator >= (WN_UINT16 _integer16) const;
    WN_BOOL operator >= (WN_UINT32 _integer32) const;
    WN_BOOL operator >= (WN_UINT64 _integer64) const;
    WN_BOOL operator >= (const __WNFixed8& _fixed8) const;
    WN_BOOL operator >= (const __WNFixed16& _fixed16) const;
    WN_BOOL operator >= (const __WNFixed32& _fixed32) const;
    WN_BOOL operator >= (const __WNFixed64& _fixed64) const;
    WN_BOOL operator >= (const __WNFloat8& _float8) const;
    WN_BOOL operator >= (const __WNFloat16& _float16) const;
    WN_BOOL operator >= (WN_FLOAT32 _float32) const;
    WN_BOOL operator >= (WN_FLOAT64 _float64) const;

    WN_BOOL operator < (WN_INT8 _integer8) const;
    WN_BOOL operator < (WN_INT16 _integer16) const;
    WN_BOOL operator < (WN_INT32 _integer32) const;
    WN_BOOL operator < (WN_INT64 _integer64) const;
    WN_BOOL operator < (WN_UINT8 _integer8) const;
    WN_BOOL operator < (WN_UINT16 _integer16) const;
    WN_BOOL operator < (WN_UINT32 _integer32) const;
    WN_BOOL operator < (WN_UINT64 _integer64) const;
    WN_BOOL operator < (const __WNFixed8& _fixed8) const;
    WN_BOOL operator < (const __WNFixed16& _fixed16) const;
    WN_BOOL operator < (const __WNFixed32& _fixed32) const;
    WN_BOOL operator < (const __WNFixed64& _fixed64) const;
    WN_BOOL operator < (const __WNFloat8& _float8) const;
    WN_BOOL operator < (const __WNFloat16& _float16) const;
    WN_BOOL operator < (WN_FLOAT32 _float32) const;
    WN_BOOL operator < (WN_FLOAT64 _float64) const;

    WN_BOOL operator > (WN_INT8 _integer8) const;
    WN_BOOL operator > (WN_INT16 _integer16) const;
    WN_BOOL operator > (WN_INT32 _integer32) const;
    WN_BOOL operator > (WN_INT64 _integer64) const;
    WN_BOOL operator > (WN_UINT8 _integer8) const;
    WN_BOOL operator > (WN_UINT16 _integer16) const;
    WN_BOOL operator > (WN_UINT32 _integer32) const;
    WN_BOOL operator > (WN_UINT64 _integer64) const;
    WN_BOOL operator > (const __WNFixed8& _fixed8) const;
    WN_BOOL operator > (const __WNFixed16& _fixed16) const;
    WN_BOOL operator > (const __WNFixed32& _fixed32) const;
    WN_BOOL operator > (const __WNFixed64& _fixed64) const;
    WN_BOOL operator > (const __WNFloat8& _float8) const;
    WN_BOOL operator > (const __WNFloat16& _float16) const;
    WN_BOOL operator > (WN_FLOAT32 _float32) const;
    WN_BOOL operator > (WN_FLOAT64 _float64) const;

    __WNFixed32& operator += (WN_INT8 _integer8);
    __WNFixed32& operator += (WN_INT16 _integer16);
    __WNFixed32& operator += (WN_INT32 _integer32);
    __WNFixed32& operator += (WN_INT64 _integer64);
    __WNFixed32& operator += (WN_UINT8 _integer8);
    __WNFixed32& operator += (WN_UINT16 _integer16);
    __WNFixed32& operator += (WN_UINT32 _integer32);
    __WNFixed32& operator += (WN_UINT64 _integer64);
    __WNFixed32& operator += (const __WNFixed8& _fixed8);
    __WNFixed32& operator += (const __WNFixed16& _fixed16);
    __WNFixed32& operator += (const __WNFixed32& _fixed32);
    __WNFixed32& operator += (const __WNFixed64& _fixed64);
    __WNFixed32& operator += (const __WNFloat8& _float8);
    __WNFixed32& operator += (const __WNFloat16& _float16);
    __WNFixed32& operator += (WN_FLOAT32 _float32);
    __WNFixed32& operator += (WN_FLOAT64 _float64);

    __WNFixed32& operator -= (WN_INT8 _integer8);
    __WNFixed32& operator -= (WN_INT16 _integer16);
    __WNFixed32& operator -= (WN_INT32 _integer32);
    __WNFixed32& operator -= (WN_INT64 _integer64);
    __WNFixed32& operator -= (WN_UINT8 _integer8);
    __WNFixed32& operator -= (WN_UINT16 _integer16);
    __WNFixed32& operator -= (WN_UINT32 _integer32);
    __WNFixed32& operator -= (WN_UINT64 _integer64);
    __WNFixed32& operator -= (const __WNFixed8& _fixed8);
    __WNFixed32& operator -= (const __WNFixed16& _fixed16);
    __WNFixed32& operator -= (const __WNFixed32& _fixed32);
    __WNFixed32& operator -= (const __WNFixed64& _fixed64);
    __WNFixed32& operator -= (const __WNFloat8& _float8);
    __WNFixed32& operator -= (const __WNFloat16& _float16);
    __WNFixed32& operator -= (WN_FLOAT32 _float32);
    __WNFixed32& operator -= (WN_FLOAT64 _float64);

    __WNFixed32& operator *= (WN_INT8 _integer8);
    __WNFixed32& operator *= (WN_INT16 _integer16);
    __WNFixed32& operator *= (WN_INT32 _integer32);
    __WNFixed32& operator *= (WN_INT64 _integer64);
    __WNFixed32& operator *= (WN_UINT8 _integer8);
    __WNFixed32& operator *= (WN_UINT16 _integer16);
    __WNFixed32& operator *= (WN_UINT32 _integer32);
    __WNFixed32& operator *= (WN_UINT64 _integer64);
    __WNFixed32& operator *= (const __WNFixed8& _fixed8);
    __WNFixed32& operator *= (const __WNFixed16& _fixed16);
    __WNFixed32& operator *= (const __WNFixed32& _fixed32);
    __WNFixed32& operator *= (const __WNFixed64& _fixed64);
    __WNFixed32& operator *= (const __WNFloat8& _float8);
    __WNFixed32& operator *= (const __WNFloat16& _float16);
    __WNFixed32& operator *= (WN_FLOAT32 _float32);
    __WNFixed32& operator *= (WN_FLOAT64 _float64);

    __WNFixed32& operator /= (WN_INT8 _integer8);
    __WNFixed32& operator /= (WN_INT16 _integer16);
    __WNFixed32& operator /= (WN_INT32 _integer32);
    __WNFixed32& operator /= (WN_INT64 _integer64);
    __WNFixed32& operator /= (WN_UINT8 _integer8);
    __WNFixed32& operator /= (WN_UINT16 _integer16);
    __WNFixed32& operator /= (WN_UINT32 _integer32);
    __WNFixed32& operator /= (WN_UINT64 _integer64);
    __WNFixed32& operator /= (const __WNFixed8& _fixed8);
    __WNFixed32& operator /= (const __WNFixed16& _fixed16);
    __WNFixed32& operator /= (const __WNFixed32& _fixed32);
    __WNFixed32& operator /= (const __WNFixed64& _fixed64);
    __WNFixed32& operator /= (const __WNFloat8& _float8);
    __WNFixed32& operator /= (const __WNFloat16& _float16);
    __WNFixed32& operator /= (WN_FLOAT32 _float32);
    __WNFixed32& operator /= (WN_FLOAT64 _float64);

    __WNFixed32 operator + (WN_INT8 _integer8) const;
    __WNFixed32 operator + (WN_INT16 _integer16) const;
    __WNFixed32 operator + (WN_INT32 _integer32) const;
    __WNFixed32 operator + (WN_INT64 _integer64) const;
    __WNFixed32 operator + (WN_UINT8 _integer8) const;
    __WNFixed32 operator + (WN_UINT16 _integer16) const;
    __WNFixed32 operator + (WN_UINT32 _integer32) const;
    __WNFixed32 operator + (WN_UINT64 _integer64) const;
    __WNFixed32 operator + (const __WNFixed8& _fixed8) const;
    __WNFixed32 operator + (const __WNFixed16& _fixed16) const;
    __WNFixed32 operator + (const __WNFixed32& _fixed32) const;
    __WNFixed64 operator + (const __WNFixed64& _fixed64) const;
    __WNFloat8 operator + (const __WNFloat8& _float8) const;
    __WNFloat16 operator + (const __WNFloat16& _float16) const;
    WN_FLOAT32 operator + (WN_FLOAT32 _float32) const;
    WN_FLOAT64 operator + (WN_FLOAT64 _float64) const;

    __WNFixed32 operator - (WN_INT8 _integer8) const;
    __WNFixed32 operator - (WN_INT16 _integer16) const;
    __WNFixed32 operator - (WN_INT32 _integer32) const;
    __WNFixed32 operator - (WN_INT64 _integer64) const;
    __WNFixed32 operator - (WN_UINT8 _integer8) const;
    __WNFixed32 operator - (WN_UINT16 _integer16) const;
    __WNFixed32 operator - (WN_UINT32 _integer32) const;
    __WNFixed32 operator - (WN_UINT64 _integer64) const;
    __WNFixed32 operator - (const __WNFixed8& _fixed8) const;
    __WNFixed32 operator - (const __WNFixed16& _fixed16) const;
    __WNFixed32 operator - (const __WNFixed32& _fixed32) const;
    __WNFixed64 operator - (const __WNFixed64& _fixed64) const;
    __WNFloat8 operator - (const __WNFloat8& _float8) const;
    __WNFloat16 operator - (const __WNFloat16& _float16) const;
    WN_FLOAT32 operator - (WN_FLOAT32 _float32) const;
    WN_FLOAT64 operator - (WN_FLOAT64 _float64) const;

    __WNFixed32 operator * (WN_INT8 _integer8) const;
    __WNFixed32 operator * (WN_INT16 _integer16) const;
    __WNFixed32 operator * (WN_INT32 _integer32) const;
    __WNFixed32 operator * (WN_INT64 _integer64) const;
    __WNFixed32 operator * (WN_UINT8 _integer8) const;
    __WNFixed32 operator * (WN_UINT16 _integer16) const;
    __WNFixed32 operator * (WN_UINT32 _integer32) const;
    __WNFixed32 operator * (WN_UINT64 _integer64) const;
    __WNFixed32 operator * (const __WNFixed8& _fixed8) const;
    __WNFixed32 operator * (const __WNFixed16& _fixed16) const;
    __WNFixed32 operator * (const __WNFixed32& _fixed32) const;
    __WNFixed64 operator * (const __WNFixed64& _fixed64) const;
    __WNFloat8 operator * (const __WNFloat8& _float8) const;
    __WNFloat16 operator * (const __WNFloat16& _float16) const;
    WN_FLOAT32 operator * (WN_FLOAT32 _float32) const;
    WN_FLOAT64 operator * (WN_FLOAT64 _float64) const;

    __WNFixed32 operator / (WN_INT8 _integer8) const;
    __WNFixed32 operator / (WN_INT16 _integer16) const;
    __WNFixed32 operator / (WN_INT32 _integer32) const;
    __WNFixed32 operator / (WN_INT64 _integer64) const;
    __WNFixed32 operator / (WN_UINT8 _integer8) const;
    __WNFixed32 operator / (WN_UINT16 _integer16) const;
    __WNFixed32 operator / (WN_UINT32 _integer32) const;
    __WNFixed32 operator / (WN_UINT64 _integer64) const;
    __WNFixed32 operator / (const __WNFixed8& _fixed8) const;
    __WNFixed32 operator / (const __WNFixed16& _fixed16) const;
    __WNFixed32 operator / (const __WNFixed32& _fixed32) const;
    __WNFixed64 operator / (const __WNFixed64& _fixed64) const;
    __WNFloat8 operator / (const __WNFloat8& _float8) const;
    __WNFloat16 operator / (const __WNFloat16& _float16) const;
    WN_FLOAT32 operator / (WN_FLOAT32 _float32) const;
    WN_FLOAT64 operator / (WN_FLOAT64 _float64) const;

public:
    WN_INT32 mBits; // stores 32 bit fixed point number in form 1:15:16
};

WN_BOOL operator == (WN_INT8 _integer8, const __WNFixed32& _fixed32);
WN_BOOL operator == (WN_INT16 _integer16, const __WNFixed32& _fixed32);
WN_BOOL operator == (WN_INT32 _integer32, const __WNFixed32& _fixed32);
WN_BOOL operator == (WN_INT64 _integer64, const __WNFixed32& _fixed32);
WN_BOOL operator == (WN_UINT8 _integer8, const __WNFixed32& _fixed32);
WN_BOOL operator == (WN_UINT16 _integer16, const __WNFixed32& _fixed32);
WN_BOOL operator == (WN_UINT32 _integer32, const __WNFixed32& _fixed32);
WN_BOOL operator == (WN_UINT64 _integer64, const __WNFixed32& _fixed32);
WN_BOOL operator == (WN_FLOAT32 _float32, const __WNFixed32& _fixed32);
WN_BOOL operator == (WN_FLOAT64 _float64, const __WNFixed32& _fixed32);

WN_BOOL operator != (WN_INT8 _integer8, const __WNFixed32& _fixed32);
WN_BOOL operator != (WN_INT16 _integer16, const __WNFixed32& _fixed32);
WN_BOOL operator != (WN_INT32 _integer32, const __WNFixed32& _fixed32);
WN_BOOL operator != (WN_INT64 _integer64, const __WNFixed32& _fixed32);
WN_BOOL operator != (WN_UINT8 _integer8, const __WNFixed32& _fixed32);
WN_BOOL operator != (WN_UINT16 _integer16, const __WNFixed32& _fixed32);
WN_BOOL operator != (WN_UINT32 _integer32, const __WNFixed32& _fixed32);
WN_BOOL operator != (WN_UINT64 _integer64, const __WNFixed32& _fixed32);
WN_BOOL operator != (WN_FLOAT32 _float32, const __WNFixed32& _fixed32);
WN_BOOL operator != (WN_FLOAT64 _float64, const __WNFixed32& _fixed32);

WN_BOOL operator <= (WN_INT8 _integer8, const __WNFixed32& _fixed32);
WN_BOOL operator <= (WN_INT16 _integer16, const __WNFixed32& _fixed32);
WN_BOOL operator <= (WN_INT32 _integer32, const __WNFixed32& _fixed32);
WN_BOOL operator <= (WN_INT64 _integer64, const __WNFixed32& _fixed32);
WN_BOOL operator <= (WN_UINT8 _integer8, const __WNFixed32& _fixed32);
WN_BOOL operator <= (WN_UINT16 _integer16, const __WNFixed32& _fixed32);
WN_BOOL operator <= (WN_UINT32 _integer32, const __WNFixed32& _fixed32);
WN_BOOL operator <= (WN_UINT64 _integer64, const __WNFixed32& _fixed32);
WN_BOOL operator <= (WN_FLOAT32 _float32, const __WNFixed32& _fixed32);
WN_BOOL operator <= (WN_FLOAT64 _float64, const __WNFixed32& _fixed32);

WN_BOOL operator >= (WN_INT8 _integer8, const __WNFixed32& _fixed32);
WN_BOOL operator >= (WN_INT16 _integer16, const __WNFixed32& _fixed32);
WN_BOOL operator >= (WN_INT32 _integer32, const __WNFixed32& _fixed32);
WN_BOOL operator >= (WN_INT64 _integer64, const __WNFixed32& _fixed32);
WN_BOOL operator >= (WN_UINT8 _integer8, const __WNFixed32& _fixed32);
WN_BOOL operator >= (WN_UINT16 _integer16, const __WNFixed32& _fixed32);
WN_BOOL operator >= (WN_UINT32 _integer32, const __WNFixed32& _fixed32);
WN_BOOL operator >= (WN_UINT64 _integer64, const __WNFixed32& _fixed32);
WN_BOOL operator >= (WN_FLOAT32 _float32, const __WNFixed32& _fixed32);
WN_BOOL operator >= (WN_FLOAT64 _float64, const __WNFixed32& _fixed32);

WN_BOOL operator < (WN_INT8 _integer8, const __WNFixed32& _fixed32);
WN_BOOL operator < (WN_INT16 _integer16, const __WNFixed32& _fixed32);
WN_BOOL operator < (WN_INT32 _integer32, const __WNFixed32& _fixed32);
WN_BOOL operator < (WN_INT64 _integer64, const __WNFixed32& _fixed32);
WN_BOOL operator < (WN_UINT8 _integer8, const __WNFixed32& _fixed32);
WN_BOOL operator < (WN_UINT16 _integer16, const __WNFixed32& _fixed32);
WN_BOOL operator < (WN_UINT32 _integer32, const __WNFixed32& _fixed32);
WN_BOOL operator < (WN_UINT64 _integer64, const __WNFixed32& _fixed32);
WN_BOOL operator < (WN_FLOAT32 _float32, const __WNFixed32& _fixed32);
WN_BOOL operator < (WN_FLOAT64 _float64, const __WNFixed32& _fixed32);

WN_BOOL operator > (WN_INT8 _integer8, const __WNFixed32& _fixed32);
WN_BOOL operator > (WN_INT16 _integer16, const __WNFixed32& _fixed32);
WN_BOOL operator > (WN_INT32 _integer32, const __WNFixed32& _fixed32);
WN_BOOL operator > (WN_INT64 _integer64, const __WNFixed32& _fixed32);
WN_BOOL operator > (WN_UINT8 _integer8, const __WNFixed32& _fixed32);
WN_BOOL operator > (WN_UINT16 _integer16, const __WNFixed32& _fixed32);
WN_BOOL operator > (WN_UINT32 _integer32, const __WNFixed32& _fixed32);
WN_BOOL operator > (WN_UINT64 _integer64, const __WNFixed32& _fixed32);
WN_BOOL operator > (WN_FLOAT32 _float32, const __WNFixed32& _fixed32);
WN_BOOL operator > (WN_FLOAT64 _float64, const __WNFixed32& _fixed32);

WN_INT8& operator += (WN_INT8& _integer8, const __WNFixed32& _fixed32);
WN_INT16& operator += (WN_INT16& _integer16, const __WNFixed32& _fixed32);
WN_INT32& operator += (WN_INT32& _integer32, const __WNFixed32& _fixed32);
WN_INT64& operator += (WN_INT64& _integer64, const __WNFixed32& _fixed32);
WN_UINT8& operator += (WN_UINT8& _integer8, const __WNFixed32& _fixed32);
WN_UINT16& operator += (WN_UINT16& _integer16, const __WNFixed32& _fixed32);
WN_UINT32& operator += (WN_UINT32& _integer32, const __WNFixed32& _fixed32);
WN_UINT64& operator += (WN_UINT64& _integer64, const __WNFixed32& _fixed32);
WN_FLOAT32& operator += (WN_FLOAT32& _float32, const __WNFixed32& _fixed32);
WN_FLOAT64& operator += (WN_FLOAT64& _float64, const __WNFixed32& _fixed32);

WN_INT8& operator -= (WN_INT8& _integer8, const __WNFixed32& _fixed32);
WN_INT16& operator -= (WN_INT16& _integer16, const __WNFixed32& _fixed32);
WN_INT32& operator -= (WN_INT32& _integer32, const __WNFixed32& _fixed32);
WN_INT64& operator -= (WN_INT64& _integer64, const __WNFixed32& _fixed32);
WN_UINT8& operator -= (WN_UINT8& _integer8, const __WNFixed32& _fixed32);
WN_UINT16& operator -= (WN_UINT16& _integer16, const __WNFixed32& _fixed32);
WN_UINT32& operator -= (WN_UINT32& _integer32, const __WNFixed32& _fixed32);
WN_UINT64& operator -= (WN_UINT64& _integer64, const __WNFixed32& _fixed32);
WN_FLOAT32& operator -= (WN_FLOAT32& _float32, const __WNFixed32& _fixed32);
WN_FLOAT64& operator -= (WN_FLOAT64& _float64, const __WNFixed32& _fixed32);

WN_INT8& operator *= (WN_INT8& _integer8, const __WNFixed32& _fixed32);
WN_INT16& operator *= (WN_INT16& _integer16, const __WNFixed32& _fixed32);
WN_INT32& operator *= (WN_INT32& _integer32, const __WNFixed32& _fixed32);
WN_INT64& operator *= (WN_INT64& _integer64, const __WNFixed32& _fixed32);
WN_UINT8& operator *= (WN_UINT8& _integer8, const __WNFixed32& _fixed32);
WN_UINT16& operator *= (WN_UINT16& _integer16, const __WNFixed32& _fixed32);
WN_UINT32& operator *= (WN_UINT32& _integer32, const __WNFixed32& _fixed32);
WN_UINT64& operator *= (WN_UINT64& _integer64, const __WNFixed32& _fixed32);
WN_FLOAT32& operator *= (WN_FLOAT32& _float32, const __WNFixed32& _fixed32);
WN_FLOAT64& operator *= (WN_FLOAT64& _float64, const __WNFixed32& _fixed32);

WN_INT8& operator /= (WN_INT8& _integer8, const __WNFixed32& _fixed32);
WN_INT16& operator /= (WN_INT16& _integer16, const __WNFixed32& _fixed32);
WN_INT32& operator /= (WN_INT32& _integer32, const __WNFixed32& _fixed32);
WN_INT64& operator /= (WN_INT64& _integer64, const __WNFixed32& _fixed32);
WN_UINT8& operator /= (WN_UINT8& _integer8, const __WNFixed32& _fixed32);
WN_UINT16& operator /= (WN_UINT16& _integer16, const __WNFixed32& _fixed32);
WN_UINT32& operator /= (WN_UINT32& _integer32, const __WNFixed32& _fixed32);
WN_UINT64& operator /= (WN_UINT64& _integer64, const __WNFixed32& _fixed32);
WN_FLOAT32& operator /= (WN_FLOAT32& _float32, const __WNFixed32& _fixed32);
WN_FLOAT64& operator /= (WN_FLOAT64& _float64, const __WNFixed32& _fixed32);

__WNFixed32 operator + (WN_INT8 _integer8, const __WNFixed32& _fixed32);
__WNFixed32 operator + (WN_INT16 _integer16, const __WNFixed32& _fixed32);
__WNFixed32 operator + (WN_INT32 _integer32, const __WNFixed32& _fixed32);
__WNFixed32 operator + (WN_INT64 _integer64, const __WNFixed32& _fixed32);
__WNFixed32 operator + (WN_UINT8 _integer8, const __WNFixed32& _fixed32);
__WNFixed32 operator + (WN_UINT16 _integer16, const __WNFixed32& _fixed32);
__WNFixed32 operator + (WN_UINT32 _integer32, const __WNFixed32& _fixed32);
__WNFixed32 operator + (WN_UINT64 _integer64, const __WNFixed32& _fixed32);
WN_FLOAT32 operator + (WN_FLOAT32 _float32, const __WNFixed32& _fixed32);
WN_FLOAT64 operator + (WN_FLOAT64 _float64, const __WNFixed32& _fixed32);

__WNFixed32 operator - (WN_INT8 _integer8, const __WNFixed32& _fixed32);
__WNFixed32 operator - (WN_INT16 _integer16, const __WNFixed32& _fixed32);
__WNFixed32 operator - (WN_INT32 _integer32, const __WNFixed32& _fixed32);
__WNFixed32 operator - (WN_INT64 _integer64, const __WNFixed32& _fixed32);
__WNFixed32 operator - (WN_UINT8 _integer8, const __WNFixed32& _fixed32);
__WNFixed32 operator - (WN_UINT16 _integer16, const __WNFixed32& _fixed32);
__WNFixed32 operator - (WN_UINT32 _integer32, const __WNFixed32& _fixed32);
__WNFixed32 operator - (WN_UINT64 _integer64, const __WNFixed32& _fixed32);
WN_FLOAT32 operator - (WN_FLOAT32 _float32, const __WNFixed32& _fixed32);
WN_FLOAT64 operator - (WN_FLOAT64 _float64, const __WNFixed32& _fixed32);

__WNFixed32 operator * (WN_INT8 _integer8, const __WNFixed32& _fixed32);
__WNFixed32 operator * (WN_INT16 _integer16, const __WNFixed32& _fixed32);
__WNFixed32 operator * (WN_INT32 _integer32, const __WNFixed32& _fixed32);
__WNFixed32 operator * (WN_INT64 _integer64, const __WNFixed32& _fixed32);
__WNFixed32 operator * (WN_UINT8 _integer8, const __WNFixed32& _fixed32);
__WNFixed32 operator * (WN_UINT16 _integer16, const __WNFixed32& _fixed32);
__WNFixed32 operator * (WN_UINT32 _integer32, const __WNFixed32& _fixed32);
__WNFixed32 operator * (WN_UINT64 _integer64, const __WNFixed32& _fixed32);
WN_FLOAT32 operator * (WN_FLOAT32 _float32, const __WNFixed32& _fixed32);
WN_FLOAT64 operator * (WN_FLOAT64 _float64, const __WNFixed32& _fixed32);

__WNFixed32 operator / (WN_INT8 _integer8, const __WNFixed32& _fixed32);
__WNFixed32 operator / (WN_INT16 _integer16, const __WNFixed32& _fixed32);
__WNFixed32 operator / (WN_INT32 _integer32, const __WNFixed32& _fixed32);
__WNFixed32 operator / (WN_INT64 _integer64, const __WNFixed32& _fixed32);
__WNFixed32 operator / (WN_UINT8 _integer8, const __WNFixed32& _fixed32);
__WNFixed32 operator / (WN_UINT16 _integer16, const __WNFixed32& _fixed32);
__WNFixed32 operator / (WN_UINT32 _integer32, const __WNFixed32& _fixed32);
__WNFixed32 operator / (WN_UINT64 _integer64, const __WNFixed32& _fixed32);
WN_FLOAT32 operator / (WN_FLOAT32 _float32, const __WNFixed32& _fixed32);
WN_FLOAT64 operator / (WN_FLOAT64 _float64, const __WNFixed32& _fixed32);

class __WNFixed64 {
public:
    WN_FORCE_INLINE __WNFixed64() {}
    explicit __WNFixed64(WN_INT8 _integer8);
    explicit __WNFixed64(WN_INT16 _integer16);
    explicit __WNFixed64(WN_INT32 _integer32);
    explicit __WNFixed64(WN_INT64 _integer64);
    explicit __WNFixed64(WN_UINT8 _integer8);
    explicit __WNFixed64(WN_UINT16 _integer16);
    explicit __WNFixed64(WN_UINT32 _integer32);
    explicit __WNFixed64(WN_UINT64 _integer64);
    __WNFixed64(const __WNFixed8& _fixed8);
    __WNFixed64(const __WNFixed16& _fixed16);
    __WNFixed64(const __WNFixed32& _fixed32);
    __WNFixed64(const __WNFixed64& _fixed64);
    explicit __WNFixed64(const __WNFloat8& _float8);
    explicit __WNFixed64(const __WNFloat16& _float16);
    explicit __WNFixed64(WN_FLOAT32 _float32);
    explicit __WNFixed64(WN_FLOAT64 _float64);

    operator WN_INT8 () const;
    operator WN_INT16 () const;
    operator WN_INT32 () const;
    operator WN_INT64 () const;
    operator WN_UINT8 () const;
    operator WN_UINT16 () const;
    operator WN_UINT32 () const;
    operator WN_UINT64 () const;
    operator WN_FLOAT32 () const;
    operator WN_FLOAT64 () const;

    __WNFixed64& operator = (WN_INT8 _integer8);
    __WNFixed64& operator = (WN_INT16 _integer16);
    __WNFixed64& operator = (WN_INT32 _integer32);
    __WNFixed64& operator = (WN_INT64 _integer64);
    __WNFixed64& operator = (WN_UINT8 _integer8);
    __WNFixed64& operator = (WN_UINT16 _integer16);
    __WNFixed64& operator = (WN_UINT32 _integer32);
    __WNFixed64& operator = (WN_UINT64 _integer64);
    __WNFixed64& operator = (const __WNFixed8& _fixed8);
    __WNFixed64& operator = (const __WNFixed16& _fixed16);
    __WNFixed64& operator = (const __WNFixed32& _fixed32);
    __WNFixed64& operator = (const __WNFixed64& _fixed64);
    __WNFixed64& operator = (const __WNFloat8& _float8);
    __WNFixed64& operator = (const __WNFloat16& _float16);
    __WNFixed64& operator = (WN_FLOAT32 _float32);
    __WNFixed64& operator = (WN_FLOAT64 _float64);

    __WNFixed64 operator + () const;
    __WNFixed64 operator - () const;

    __WNFixed64& operator ++ ();
    __WNFixed64 operator ++ (WN_INT32 _integer32);
    __WNFixed64& operator -- ();
    __WNFixed64 operator -- (WN_INT32 _integer32);

    WN_BOOL operator ! () const;

    WN_BOOL operator == (WN_INT8 _integer8) const;
    WN_BOOL operator == (WN_INT16 _integer16) const;
    WN_BOOL operator == (WN_INT32 _integer32) const;
    WN_BOOL operator == (WN_INT64 _integer64) const;
    WN_BOOL operator == (WN_UINT8 _integer8) const;
    WN_BOOL operator == (WN_UINT16 _integer16) const;
    WN_BOOL operator == (WN_UINT32 _integer32) const;
    WN_BOOL operator == (WN_UINT64 _integer64) const;
    WN_BOOL operator == (const __WNFixed8& _fixed8) const;
    WN_BOOL operator == (const __WNFixed16& _fixed16) const;
    WN_BOOL operator == (const __WNFixed32& _fixed32) const;
    WN_BOOL operator == (const __WNFixed64& _fixed64) const;
    WN_BOOL operator == (const __WNFloat8& _float8) const;
    WN_BOOL operator == (const __WNFloat16& _float16) const;
    WN_BOOL operator == (WN_FLOAT32 _float32) const;
    WN_BOOL operator == (WN_FLOAT64 _float64) const;

    WN_BOOL operator != (WN_INT8 _integer8) const;
    WN_BOOL operator != (WN_INT16 _integer16) const;
    WN_BOOL operator != (WN_INT32 _integer32) const;
    WN_BOOL operator != (WN_INT64 _integer64) const;
    WN_BOOL operator != (WN_UINT8 _integer8) const;
    WN_BOOL operator != (WN_UINT16 _integer16) const;
    WN_BOOL operator != (WN_UINT32 _integer32) const;
    WN_BOOL operator != (WN_UINT64 _integer64) const;
    WN_BOOL operator != (const __WNFixed8& _fixed8) const;
    WN_BOOL operator != (const __WNFixed16& _fixed16) const;
    WN_BOOL operator != (const __WNFixed32& _fixed32) const;
    WN_BOOL operator != (const __WNFixed64& _fixed64) const;
    WN_BOOL operator != (const __WNFloat8& _float8) const;
    WN_BOOL operator != (const __WNFloat16& _float16) const;
    WN_BOOL operator != (WN_FLOAT32 _float32) const;
    WN_BOOL operator != (WN_FLOAT64 _float64) const;

    WN_BOOL operator <= (WN_INT8 _integer8) const;
    WN_BOOL operator <= (WN_INT16 _integer16) const;
    WN_BOOL operator <= (WN_INT32 _integer32) const;
    WN_BOOL operator <= (WN_INT64 _integer64) const;
    WN_BOOL operator <= (WN_UINT8 _integer8) const;
    WN_BOOL operator <= (WN_UINT16 _integer16) const;
    WN_BOOL operator <= (WN_UINT32 _integer32) const;
    WN_BOOL operator <= (WN_UINT64 _integer64) const;
    WN_BOOL operator <= (const __WNFixed8& _fixed8) const;
    WN_BOOL operator <= (const __WNFixed16& _fixed16) const;
    WN_BOOL operator <= (const __WNFixed32& _fixed32) const;
    WN_BOOL operator <= (const __WNFixed64& _fixed64) const;
    WN_BOOL operator <= (const __WNFloat8& _float8) const;
    WN_BOOL operator <= (const __WNFloat16& _float16) const;
    WN_BOOL operator <= (WN_FLOAT32 _float32) const;
    WN_BOOL operator <= (WN_FLOAT64 _float64) const;

    WN_BOOL operator >= (WN_INT8 _integer8) const;
    WN_BOOL operator >= (WN_INT16 _integer16) const;
    WN_BOOL operator >= (WN_INT32 _integer32) const;
    WN_BOOL operator >= (WN_INT64 _integer64) const;
    WN_BOOL operator >= (WN_UINT8 _integer8) const;
    WN_BOOL operator >= (WN_UINT16 _integer16) const;
    WN_BOOL operator >= (WN_UINT32 _integer32) const;
    WN_BOOL operator >= (WN_UINT64 _integer64) const;
    WN_BOOL operator >= (const __WNFixed8& _fixed8) const;
    WN_BOOL operator >= (const __WNFixed16& _fixed16) const;
    WN_BOOL operator >= (const __WNFixed32& _fixed32) const;
    WN_BOOL operator >= (const __WNFixed64& _fixed64) const;
    WN_BOOL operator >= (const __WNFloat8& _float8) const;
    WN_BOOL operator >= (const __WNFloat16& _float16) const;
    WN_BOOL operator >= (WN_FLOAT32 _float32) const;
    WN_BOOL operator >= (WN_FLOAT64 _float64) const;

    WN_BOOL operator < (WN_INT8 _integer8) const;
    WN_BOOL operator < (WN_INT16 _integer16) const;
    WN_BOOL operator < (WN_INT32 _integer32) const;
    WN_BOOL operator < (WN_INT64 _integer64) const;
    WN_BOOL operator < (WN_UINT8 _integer8) const;
    WN_BOOL operator < (WN_UINT16 _integer16) const;
    WN_BOOL operator < (WN_UINT32 _integer32) const;
    WN_BOOL operator < (WN_UINT64 _integer64) const;
    WN_BOOL operator < (const __WNFixed8& _fixed8) const;
    WN_BOOL operator < (const __WNFixed16& _fixed16) const;
    WN_BOOL operator < (const __WNFixed32& _fixed32) const;
    WN_BOOL operator < (const __WNFixed64& _fixed64) const;
    WN_BOOL operator < (const __WNFloat8& _float8) const;
    WN_BOOL operator < (const __WNFloat16& _float16) const;
    WN_BOOL operator < (WN_FLOAT32 _float32) const;
    WN_BOOL operator < (WN_FLOAT64 _float64) const;

    WN_BOOL operator > (WN_INT8 _integer8) const;
    WN_BOOL operator > (WN_INT16 _integer16) const;
    WN_BOOL operator > (WN_INT32 _integer32) const;
    WN_BOOL operator > (WN_INT64 _integer64) const;
    WN_BOOL operator > (WN_UINT8 _integer8) const;
    WN_BOOL operator > (WN_UINT16 _integer16) const;
    WN_BOOL operator > (WN_UINT32 _integer32) const;
    WN_BOOL operator > (WN_UINT64 _integer64) const;
    WN_BOOL operator > (const __WNFixed8& _fixed8) const;
    WN_BOOL operator > (const __WNFixed16& _fixed16) const;
    WN_BOOL operator > (const __WNFixed32& _fixed32) const;
    WN_BOOL operator > (const __WNFixed64& _fixed64) const;
    WN_BOOL operator > (const __WNFloat8& _float8) const;
    WN_BOOL operator > (const __WNFloat16& _float16) const;
    WN_BOOL operator > (WN_FLOAT32 _float32) const;
    WN_BOOL operator > (WN_FLOAT64 _float64) const;

    __WNFixed64& operator += (WN_INT8 _integer8);
    __WNFixed64& operator += (WN_INT16 _integer16);
    __WNFixed64& operator += (WN_INT32 _integer32);
    __WNFixed64& operator += (WN_INT64 _integer64);
    __WNFixed64& operator += (WN_UINT8 _integer8);
    __WNFixed64& operator += (WN_UINT16 _integer16);
    __WNFixed64& operator += (WN_UINT32 _integer32);
    __WNFixed64& operator += (WN_UINT64 _integer64);
    __WNFixed64& operator += (const __WNFixed8& _fixed8);
    __WNFixed64& operator += (const __WNFixed16& _fixed16);
    __WNFixed64& operator += (const __WNFixed32& _fixed32);
    __WNFixed64& operator += (const __WNFixed64& _fixed64);
    __WNFixed64& operator += (const __WNFloat8& _float8);
    __WNFixed64& operator += (const __WNFloat16& _float16);
    __WNFixed64& operator += (WN_FLOAT32 _float32);
    __WNFixed64& operator += (WN_FLOAT64 _float64);

    __WNFixed64& operator -= (WN_INT8 _integer8);
    __WNFixed64& operator -= (WN_INT16 _integer16);
    __WNFixed64& operator -= (WN_INT32 _integer32);
    __WNFixed64& operator -= (WN_INT64 _integer64);
    __WNFixed64& operator -= (WN_UINT8 _integer8);
    __WNFixed64& operator -= (WN_UINT16 _integer16);
    __WNFixed64& operator -= (WN_UINT32 _integer32);
    __WNFixed64& operator -= (WN_UINT64 _integer64);
    __WNFixed64& operator -= (const __WNFixed8& _fixed8);
    __WNFixed64& operator -= (const __WNFixed16& _fixed16);
    __WNFixed64& operator -= (const __WNFixed32& _fixed32);
    __WNFixed64& operator -= (const __WNFixed64& _fixed64);
    __WNFixed64& operator -= (const __WNFloat8& _float8);
    __WNFixed64& operator -= (const __WNFloat16& _float16);
    __WNFixed64& operator -= (WN_FLOAT32 _float32);
    __WNFixed64& operator -= (WN_FLOAT64 _float64);

    __WNFixed64& operator *= (WN_INT8 _integer8);
    __WNFixed64& operator *= (WN_INT16 _integer16);
    __WNFixed64& operator *= (WN_INT32 _integer32);
    __WNFixed64& operator *= (WN_INT64 _integer64);
    __WNFixed64& operator *= (WN_UINT8 _integer8);
    __WNFixed64& operator *= (WN_UINT16 _integer16);
    __WNFixed64& operator *= (WN_UINT32 _integer32);
    __WNFixed64& operator *= (WN_UINT64 _integer64);
    __WNFixed64& operator *= (const __WNFixed8& _fixed8);
    __WNFixed64& operator *= (const __WNFixed16& _fixed16);
    __WNFixed64& operator *= (const __WNFixed32& _fixed32);
    __WNFixed64& operator *= (const __WNFixed64& _fixed64);
    __WNFixed64& operator *= (const __WNFloat8& _float8);
    __WNFixed64& operator *= (const __WNFloat16& _float16);
    __WNFixed64& operator *= (WN_FLOAT32 _float32);
    __WNFixed64& operator *= (WN_FLOAT64 _float64);

    __WNFixed64& operator /= (WN_INT8 _integer8);
    __WNFixed64& operator /= (WN_INT16 _integer16);
    __WNFixed64& operator /= (WN_INT32 _integer32);
    __WNFixed64& operator /= (WN_INT64 _integer64);
    __WNFixed64& operator /= (WN_UINT8 _integer8);
    __WNFixed64& operator /= (WN_UINT16 _integer16);
    __WNFixed64& operator /= (WN_UINT32 _integer32);
    __WNFixed64& operator /= (WN_UINT64 _integer64);
    __WNFixed64& operator /= (const __WNFixed8& _fixed8);
    __WNFixed64& operator /= (const __WNFixed16& _fixed16);
    __WNFixed64& operator /= (const __WNFixed32& _fixed32);
    __WNFixed64& operator /= (const __WNFixed64& _fixed64);
    __WNFixed64& operator /= (const __WNFloat8& _float8);
    __WNFixed64& operator /= (const __WNFloat16& _float16);
    __WNFixed64& operator /= (WN_FLOAT32 _float32);
    __WNFixed64& operator /= (WN_FLOAT64 _float64);

    __WNFixed64 operator + (WN_INT8 _integer8) const;
    __WNFixed64 operator + (WN_INT16 _integer16) const;
    __WNFixed64 operator + (WN_INT32 _integer32) const;
    __WNFixed64 operator + (WN_INT64 _integer64) const;
    __WNFixed64 operator + (WN_UINT8 _integer8) const;
    __WNFixed64 operator + (WN_UINT16 _integer16) const;
    __WNFixed64 operator + (WN_UINT32 _integer32) const;
    __WNFixed64 operator + (WN_UINT64 _integer64) const;
    __WNFixed64 operator + (const __WNFixed8& _fixed8) const;
    __WNFixed64 operator + (const __WNFixed16& _fixed16) const;
    __WNFixed64 operator + (const __WNFixed32& _fixed32) const;
    __WNFixed64 operator + (const __WNFixed64& _fixed64) const;
    __WNFloat8 operator + (const __WNFloat8& _float8) const;
    __WNFloat16 operator + (const __WNFloat16& _float16) const;
    WN_FLOAT32 operator + (WN_FLOAT32 _float32) const;
    WN_FLOAT64 operator + (WN_FLOAT64 _float64) const;

    __WNFixed64 operator - (WN_INT8 _integer8) const;
    __WNFixed64 operator - (WN_INT16 _integer16) const;
    __WNFixed64 operator - (WN_INT32 _integer32) const;
    __WNFixed64 operator - (WN_INT64 _integer64) const;
    __WNFixed64 operator - (WN_UINT8 _integer8) const;
    __WNFixed64 operator - (WN_UINT16 _integer16) const;
    __WNFixed64 operator - (WN_UINT32 _integer32) const;
    __WNFixed64 operator - (WN_UINT64 _integer64) const;
    __WNFixed64 operator - (const __WNFixed8& _fixed8) const;
    __WNFixed64 operator - (const __WNFixed16& _fixed16) const;
    __WNFixed64 operator - (const __WNFixed32& _fixed32) const;
    __WNFixed64 operator - (const __WNFixed64& _fixed64) const;
    __WNFloat8 operator - (const __WNFloat8& _float8) const;
    __WNFloat16 operator - (const __WNFloat16& _float16) const;
    WN_FLOAT32 operator - (WN_FLOAT32 _float32) const;
    WN_FLOAT64 operator - (WN_FLOAT64 _float64) const;

    __WNFixed64 operator * (WN_INT8 _integer8) const;
    __WNFixed64 operator * (WN_INT16 _integer16) const;
    __WNFixed64 operator * (WN_INT32 _integer32) const;
    __WNFixed64 operator * (WN_INT64 _integer64) const;
    __WNFixed64 operator * (WN_UINT8 _integer8) const;
    __WNFixed64 operator * (WN_UINT16 _integer16) const;
    __WNFixed64 operator * (WN_UINT32 _integer32) const;
    __WNFixed64 operator * (WN_UINT64 _integer64) const;
    __WNFixed64 operator * (const __WNFixed8& _fixed8) const;
    __WNFixed64 operator * (const __WNFixed16& _fixed16) const;
    __WNFixed64 operator * (const __WNFixed32& _fixed32) const;
    __WNFixed64 operator * (const __WNFixed64& _fixed64) const;
    __WNFloat8 operator * (const __WNFloat8& _float8) const;
    __WNFloat16 operator * (const __WNFloat16& _float16) const;
    WN_FLOAT32 operator * (WN_FLOAT32 _float32) const;
    WN_FLOAT64 operator * (WN_FLOAT64 _float64) const;

    __WNFixed64 operator / (WN_INT8 _integer8) const;
    __WNFixed64 operator / (WN_INT16 _integer16) const;
    __WNFixed64 operator / (WN_INT32 _integer32) const;
    __WNFixed64 operator / (WN_INT64 _integer64) const;
    __WNFixed64 operator / (WN_UINT8 _integer8) const;
    __WNFixed64 operator / (WN_UINT16 _integer16) const;
    __WNFixed64 operator / (WN_UINT32 _integer32) const;
    __WNFixed64 operator / (WN_UINT64 _integer64) const;
    __WNFixed64 operator / (const __WNFixed8& _fixed8) const;
    __WNFixed64 operator / (const __WNFixed16& _fixed16) const;
    __WNFixed64 operator / (const __WNFixed32& _fixed32) const;
    __WNFixed64 operator / (const __WNFixed64& _fixed64) const;
    __WNFloat8 operator / (const __WNFloat8& _float8) const;
    __WNFloat16 operator / (const __WNFloat16& _float16) const;
    WN_FLOAT32 operator / (WN_FLOAT32 _float32) const;
    WN_FLOAT64 operator / (WN_FLOAT64 _float64) const;

public:
    WN_INT64 mBits; // stores 64 bit fixed point number in form 1:31:32
};

WN_BOOL operator == (WN_INT8 _integer8, const __WNFixed64& _fixed64);
WN_BOOL operator == (WN_INT16 _integer16, const __WNFixed64& _fixed64);
WN_BOOL operator == (WN_INT32 _integer32, const __WNFixed64& _fixed64);
WN_BOOL operator == (WN_INT64 _integer64, const __WNFixed64& _fixed64);
WN_BOOL operator == (WN_UINT8 _integer8, const __WNFixed64& _fixed64);
WN_BOOL operator == (WN_UINT16 _integer16, const __WNFixed64& _fixed64);
WN_BOOL operator == (WN_UINT32 _integer32, const __WNFixed64& _fixed64);
WN_BOOL operator == (WN_UINT64 _integer64, const __WNFixed64& _fixed64);
WN_BOOL operator == (WN_FLOAT32 _float32, const __WNFixed64& _fixed64);
WN_BOOL operator == (WN_FLOAT64 _float64, const __WNFixed64& _fixed64);

WN_BOOL operator != (WN_INT8 _integer8, const __WNFixed64& _fixed64);
WN_BOOL operator != (WN_INT16 _integer16, const __WNFixed64& _fixed64);
WN_BOOL operator != (WN_INT32 _integer32, const __WNFixed64& _fixed64);
WN_BOOL operator != (WN_INT64 _integer64, const __WNFixed64& _fixed64);
WN_BOOL operator != (WN_UINT8 _integer8, const __WNFixed64& _fixed64);
WN_BOOL operator != (WN_UINT16 _integer16, const __WNFixed64& _fixed64);
WN_BOOL operator != (WN_UINT32 _integer32, const __WNFixed64& _fixed64);
WN_BOOL operator != (WN_UINT64 _integer64, const __WNFixed64& _fixed64);
WN_BOOL operator != (WN_FLOAT32 _float32, const __WNFixed64& _fixed64);
WN_BOOL operator != (WN_FLOAT64 _float64, const __WNFixed64& _fixed64);

WN_BOOL operator <= (WN_INT8 _integer8, const __WNFixed64& _fixed64);
WN_BOOL operator <= (WN_INT16 _integer16, const __WNFixed64& _fixed64);
WN_BOOL operator <= (WN_INT32 _integer32, const __WNFixed64& _fixed64);
WN_BOOL operator <= (WN_INT64 _integer64, const __WNFixed64& _fixed64);
WN_BOOL operator <= (WN_UINT8 _integer8, const __WNFixed64& _fixed64);
WN_BOOL operator <= (WN_UINT16 _integer16, const __WNFixed64& _fixed64);
WN_BOOL operator <= (WN_UINT32 _integer32, const __WNFixed64& _fixed64);
WN_BOOL operator <= (WN_UINT64 _integer64, const __WNFixed64& _fixed64);
WN_BOOL operator <= (WN_FLOAT32 _float32, const __WNFixed64& _fixed64);
WN_BOOL operator <= (WN_FLOAT64 _float64, const __WNFixed64& _fixed64);

WN_BOOL operator >= (WN_INT8 _integer8, const __WNFixed64& _fixed64);
WN_BOOL operator >= (WN_INT16 _integer16, const __WNFixed64& _fixed64);
WN_BOOL operator >= (WN_INT32 _integer32, const __WNFixed64& _fixed64);
WN_BOOL operator >= (WN_INT64 _integer64, const __WNFixed64& _fixed64);
WN_BOOL operator >= (WN_UINT8 _integer8, const __WNFixed64& _fixed64);
WN_BOOL operator >= (WN_UINT16 _integer16, const __WNFixed64& _fixed64);
WN_BOOL operator >= (WN_UINT32 _integer32, const __WNFixed64& _fixed64);
WN_BOOL operator >= (WN_UINT64 _integer64, const __WNFixed64& _fixed64);
WN_BOOL operator >= (WN_FLOAT32 _float32, const __WNFixed64& _fixed64);
WN_BOOL operator >= (WN_FLOAT64 _float64, const __WNFixed64& _fixed64);

WN_BOOL operator < (WN_INT8 _integer8, const __WNFixed64& _fixed64);
WN_BOOL operator < (WN_INT16 _integer16, const __WNFixed64& _fixed64);
WN_BOOL operator < (WN_INT32 _integer32, const __WNFixed64& _fixed64);
WN_BOOL operator < (WN_INT64 _integer64, const __WNFixed64& _fixed64);
WN_BOOL operator < (WN_UINT8 _integer8, const __WNFixed64& _fixed64);
WN_BOOL operator < (WN_UINT16 _integer16, const __WNFixed64& _fixed64);
WN_BOOL operator < (WN_UINT32 _integer32, const __WNFixed64& _fixed64);
WN_BOOL operator < (WN_UINT64 _integer64, const __WNFixed64& _fixed64);
WN_BOOL operator < (WN_FLOAT32 _float32, const __WNFixed64& _fixed64);
WN_BOOL operator < (WN_FLOAT64 _float64, const __WNFixed64& _fixed64);

WN_BOOL operator > (WN_INT8 _integer8, const __WNFixed64& _fixed64);
WN_BOOL operator > (WN_INT16 _integer16, const __WNFixed64& _fixed64);
WN_BOOL operator > (WN_INT32 _integer32, const __WNFixed64& _fixed64);
WN_BOOL operator > (WN_INT64 _integer64, const __WNFixed64& _fixed64);
WN_BOOL operator > (WN_UINT8 _integer8, const __WNFixed64& _fixed64);
WN_BOOL operator > (WN_UINT16 _integer16, const __WNFixed64& _fixed64);
WN_BOOL operator > (WN_UINT32 _integer32, const __WNFixed64& _fixed64);
WN_BOOL operator > (WN_UINT64 _integer64, const __WNFixed64& _fixed64);
WN_BOOL operator > (WN_FLOAT32 _float32, const __WNFixed64& _fixed64);
WN_BOOL operator > (WN_FLOAT64 _float64, const __WNFixed64& _fixed64);

WN_INT8& operator += (WN_INT8& _integer8, const __WNFixed64& _fixed64);
WN_INT16& operator += (WN_INT16& _integer16, const __WNFixed64& _fixed64);
WN_INT32& operator += (WN_INT32& _integer32, const __WNFixed64& _fixed64);
WN_INT64& operator += (WN_INT64& _integer64, const __WNFixed64& _fixed64);
WN_UINT8& operator += (WN_UINT8& _integer8, const __WNFixed64& _fixed64);
WN_UINT16& operator += (WN_UINT16& _integer16, const __WNFixed64& _fixed64);
WN_UINT32& operator += (WN_UINT32& _integer32, const __WNFixed64& _fixed64);
WN_UINT64& operator += (WN_UINT64& _integer64, const __WNFixed64& _fixed64);
WN_FLOAT32& operator += (WN_FLOAT32& _float32, const __WNFixed64& _fixed64);
WN_FLOAT64& operator += (WN_FLOAT64& _float64, const __WNFixed64& _fixed64);

WN_INT8& operator -= (WN_INT8& _integer8, const __WNFixed64& _fixed64);
WN_INT16& operator -= (WN_INT16& _integer16, const __WNFixed64& _fixed64);
WN_INT32& operator -= (WN_INT32& _integer32, const __WNFixed64& _fixed64);
WN_INT64& operator -= (WN_INT64& _integer64, const __WNFixed64& _fixed64);
WN_UINT8& operator -= (WN_UINT8& _integer8, const __WNFixed64& _fixed64);
WN_UINT16& operator -= (WN_UINT16& _integer16, const __WNFixed64& _fixed64);
WN_UINT32& operator -= (WN_UINT32& _integer32, const __WNFixed64& _fixed64);
WN_UINT64& operator -= (WN_UINT64& _integer64, const __WNFixed64& _fixed64);
WN_FLOAT32& operator -= (WN_FLOAT32& _float32, const __WNFixed64& _fixed64);
WN_FLOAT64& operator -= (WN_FLOAT64& _float64, const __WNFixed64& _fixed64);

WN_INT8& operator *= (WN_INT8& _integer8, const __WNFixed64& _fixed64);
WN_INT16& operator *= (WN_INT16& _integer16, const __WNFixed64& _fixed64);
WN_INT32& operator *= (WN_INT32& _integer32, const __WNFixed64& _fixed64);
WN_INT64& operator *= (WN_INT64& _integer64, const __WNFixed64& _fixed64);
WN_UINT8& operator *= (WN_UINT8& _integer8, const __WNFixed64& _fixed64);
WN_UINT16& operator *= (WN_UINT16& _integer16, const __WNFixed64& _fixed64);
WN_UINT32& operator *= (WN_UINT32& _integer32, const __WNFixed64& _fixed64);
WN_UINT64& operator *= (WN_UINT64& _integer64, const __WNFixed64& _fixed64);
WN_FLOAT32& operator *= (WN_FLOAT32& _float32, const __WNFixed64& _fixed64);
WN_FLOAT64& operator *= (WN_FLOAT64& _float64, const __WNFixed64& _fixed64);

WN_INT8& operator /= (WN_INT8& _integer8, const __WNFixed64& _fixed64);
WN_INT16& operator /= (WN_INT16& _integer16, const __WNFixed64& _fixed64);
WN_INT32& operator /= (WN_INT32& _integer32, const __WNFixed64& _fixed64);
WN_INT64& operator /= (WN_INT64& _integer64, const __WNFixed64& _fixed64);
WN_UINT8& operator /= (WN_UINT8& _integer8, const __WNFixed64& _fixed64);
WN_UINT16& operator /= (WN_UINT16& _integer16, const __WNFixed64& _fixed64);
WN_UINT32& operator /= (WN_UINT32& _integer32, const __WNFixed64& _fixed64);
WN_UINT64& operator /= (WN_UINT64& _integer64, const __WNFixed64& _fixed64);
WN_FLOAT32& operator /= (WN_FLOAT32& _float32, const __WNFixed64& _fixed64);
WN_FLOAT64& operator /= (WN_FLOAT64& _float64, const __WNFixed64& _fixed64);

__WNFixed64 operator + (WN_INT8 _integer8, const __WNFixed64& _fixed64);
__WNFixed64 operator + (WN_INT16 _integer16, const __WNFixed64& _fixed64);
__WNFixed64 operator + (WN_INT32 _integer32, const __WNFixed64& _fixed64);
__WNFixed64 operator + (WN_INT64 _integer64, const __WNFixed64& _fixed64);
__WNFixed64 operator + (WN_UINT8 _integer8, const __WNFixed64& _fixed64);
__WNFixed64 operator + (WN_UINT16 _integer16, const __WNFixed64& _fixed64);
__WNFixed64 operator + (WN_UINT32 _integer32, const __WNFixed64& _fixed64);
__WNFixed64 operator + (WN_UINT64 _integer64, const __WNFixed64& _fixed64);
WN_FLOAT32 operator + (WN_FLOAT32 _float32, const __WNFixed64& _fixed64);
WN_FLOAT64 operator + (WN_FLOAT64 _float64, const __WNFixed64& _fixed64);

__WNFixed64 operator - (WN_INT8 _integer8, const __WNFixed64& _fixed64);
__WNFixed64 operator - (WN_INT16 _integer16, const __WNFixed64& _fixed64);
__WNFixed64 operator - (WN_INT32 _integer32, const __WNFixed64& _fixed64);
__WNFixed64 operator - (WN_INT64 _integer64, const __WNFixed64& _fixed64);
__WNFixed64 operator - (WN_UINT8 _integer8, const __WNFixed64& _fixed64);
__WNFixed64 operator - (WN_UINT16 _integer16, const __WNFixed64& _fixed64);
__WNFixed64 operator - (WN_UINT32 _integer32, const __WNFixed64& _fixed64);
__WNFixed64 operator - (WN_UINT64 _integer64, const __WNFixed64& _fixed64);
WN_FLOAT32 operator - (WN_FLOAT32 _float32, const __WNFixed64& _fixed64);
WN_FLOAT64 operator - (WN_FLOAT64 _float64, const __WNFixed64& _fixed64);

__WNFixed64 operator * (WN_INT8 _integer8, const __WNFixed64& _fixed64);
__WNFixed64 operator * (WN_INT16 _integer16, const __WNFixed64& _fixed64);
__WNFixed64 operator * (WN_INT32 _integer32, const __WNFixed64& _fixed64);
__WNFixed64 operator * (WN_INT64 _integer64, const __WNFixed64& _fixed64);
__WNFixed64 operator * (WN_UINT8 _integer8, const __WNFixed64& _fixed64);
__WNFixed64 operator * (WN_UINT16 _integer16, const __WNFixed64& _fixed64);
__WNFixed64 operator * (WN_UINT32 _integer32, const __WNFixed64& _fixed64);
__WNFixed64 operator * (WN_UINT64 _integer64, const __WNFixed64& _fixed64);
WN_FLOAT32 operator * (WN_FLOAT32 _float32, const __WNFixed64& _fixed64);
WN_FLOAT64 operator * (WN_FLOAT64 _float64, const __WNFixed64& _fixed64);

__WNFixed64 operator / (WN_INT8 _integer8, const __WNFixed64& _fixed64);
__WNFixed64 operator / (WN_INT16 _integer16, const __WNFixed64& _fixed64);
__WNFixed64 operator / (WN_INT32 _integer32, const __WNFixed64& _fixed64);
__WNFixed64 operator / (WN_INT64 _integer64, const __WNFixed64& _fixed64);
__WNFixed64 operator / (WN_UINT8 _integer8, const __WNFixed64& _fixed64);
__WNFixed64 operator / (WN_UINT16 _integer16, const __WNFixed64& _fixed64);
__WNFixed64 operator / (WN_UINT32 _integer32, const __WNFixed64& _fixed64);
__WNFixed64 operator / (WN_UINT64 _integer64, const __WNFixed64& _fixed64);
WN_FLOAT32 operator / (WN_FLOAT32 _float32, const __WNFixed64& _fixed64);
WN_FLOAT64 operator / (WN_FLOAT64 _float64, const __WNFixed64& _fixed64);

typedef __WNFixed8 WN_FIXED8;
typedef __WNFixed16 WN_FIXED16;
typedef __WNFixed32 WN_FIXED32;
typedef __WNFixed64 WN_FIXED64;

#define WN_FIXED8_BITS 4
#define WN_FIXED8_MAX 7.9375
#define WN_FIXED8_MIN -8.0

#define WN_FIXED16_BITS 8
#define WN_FIXED16_MAX 127.99609375
#define WN_FIXED16_MIN -128.0

#define WN_FIXED32_BITS 16
#define WN_FIXED32_MAX 32767.9999847412109375
#define WN_FIXED32_MIN -32768.0

#define WN_FIXED64_BITS 32
#define WN_FIXED64_MAX 2147483647.9999999997671693563461
#define WN_FIXED64_MIN -2147483648.0

template <> WN_BOOL WNIsValid(WN_FLOAT8 _value);
template <> WN_BOOL WNIsValid(WN_FLOAT16 _value);

#include "WNCore/inc/Internal/WNExtendedTypes.inl"

#endif // __WN_CORE_EXTENDED_TYPES_H__