////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CORE_INTERNAL_EXTENDED_TYPES_INL__
#define __WN_CORE_INTERNAL_EXTENDED_TYPES_INL__

#ifndef __WN_CORE_EXTENDED_TYPES_H__
    #error "WNTypes.inl should never be included directly. Please include WNTypes.h instead"
#endif

#if defined _WN_WINDOWS && defined _WN_64_BIT
    #include <windows.h>
#endif

#include "WNCore/inc/WNConfig.h"

#define __WN_FIXED8_ONE (1 << WN_FIXED8_BITS)
#define __WN_FIXED16_ONE (1 << WN_FIXED16_BITS)
#define __WN_FIXED32_ONE (1L << WN_FIXED32_BITS)
#define __WN_FIXED64_ONE (1LL << WN_FIXED64_BITS)

// Float (8 bit) ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

WN_FORCE_INLINE __WNFloat8::__WNFloat8(WN_INT8 _integer8) {
    mBits = F32ToF8(static_cast<WN_FLOAT32>(_integer8));
}

WN_FORCE_INLINE __WNFloat8::__WNFloat8(WN_INT16 _integer16) {
    mBits = F32ToF8(static_cast<WN_FLOAT32>(_integer16));
}

WN_FORCE_INLINE __WNFloat8::__WNFloat8(WN_INT32 _integer32) {
    mBits = F32ToF8(static_cast<WN_FLOAT32>(_integer32));
}

WN_FORCE_INLINE __WNFloat8::__WNFloat8(WN_INT64 _integer64) {
    mBits = F32ToF8(static_cast<WN_FLOAT32>(_integer64));
}

WN_FORCE_INLINE __WNFloat8::__WNFloat8(WN_UINT8 _integer8) {
    mBits = F32ToF8(static_cast<WN_FLOAT32>(_integer8));
}

WN_FORCE_INLINE __WNFloat8::__WNFloat8(WN_UINT16 _integer16) {
    mBits = F32ToF8(static_cast<WN_FLOAT32>(_integer16));
}

WN_FORCE_INLINE __WNFloat8::__WNFloat8(WN_UINT32 _integer32) {
    mBits = F32ToF8(static_cast<WN_FLOAT32>(_integer32));
}

WN_FORCE_INLINE __WNFloat8::__WNFloat8(WN_UINT64 _integer64) {
    mBits = F32ToF8(static_cast<WN_FLOAT32>(_integer64));
}

WN_FORCE_INLINE __WNFloat8::__WNFloat8(const __WNFixed8& _fixed8) {
    mBits = F32ToF8(static_cast<WN_FLOAT32>(_fixed8));
}

WN_FORCE_INLINE __WNFloat8::__WNFloat8(const __WNFixed16& _fixed16) {
    mBits = F32ToF8(static_cast<WN_FLOAT32>(_fixed16));
}

WN_FORCE_INLINE __WNFloat8::__WNFloat8(const __WNFixed32& _fixed32) {
    mBits = F32ToF8(static_cast<WN_FLOAT32>(_fixed32));
}

WN_FORCE_INLINE __WNFloat8::__WNFloat8(const __WNFixed64& _fixed64) {
    mBits = F32ToF8(static_cast<WN_FLOAT32>(_fixed64));
}

WN_FORCE_INLINE __WNFloat8::__WNFloat8(const __WNFloat8& _float8) {
    mBits = _float8.mBits;
}

WN_FORCE_INLINE __WNFloat8::__WNFloat8(const __WNFloat16& _float16) {
    mBits = F32ToF8(static_cast<WN_FLOAT32>(_float16));
}

WN_FORCE_INLINE __WNFloat8::__WNFloat8(WN_FLOAT32 _float32) {
    mBits = F32ToF8(_float32);
}

WN_FORCE_INLINE __WNFloat8::__WNFloat8(WN_FLOAT64 _float64) {
    mBits = F32ToF8(static_cast<WN_FLOAT32>(_float64));
}

WN_FORCE_INLINE __WNFloat8::operator WN_INT8 () const {
    return(static_cast<WN_INT8>(static_cast<WN_FLOAT32>(*this)));
}

WN_FORCE_INLINE __WNFloat8::operator WN_INT16 () const {
    return(static_cast<WN_INT16>(static_cast<WN_FLOAT32>(*this)));
}

WN_FORCE_INLINE __WNFloat8::operator WN_INT32 () const {
    return(static_cast<WN_INT32>(static_cast<WN_FLOAT32>(*this)));
}

WN_FORCE_INLINE __WNFloat8::operator WN_INT64 () const {
    return(static_cast<WN_INT64>(static_cast<WN_FLOAT32>(*this)));
}

WN_FORCE_INLINE __WNFloat8::operator WN_UINT8 () const {
    return(static_cast<WN_UINT8>(static_cast<WN_FLOAT32>(*this)));
}

WN_FORCE_INLINE __WNFloat8::operator WN_UINT16 () const {
    return(static_cast<WN_UINT16>(static_cast<WN_FLOAT32>(*this)));
}

WN_FORCE_INLINE __WNFloat8::operator WN_UINT32 () const {
    return(static_cast<WN_UINT32>(static_cast<WN_FLOAT32>(*this)));
}

WN_FORCE_INLINE __WNFloat8::operator WN_UINT64 () const {
    return(static_cast<WN_UINT64>(static_cast<WN_FLOAT32>(*this)));
}

WN_FORCE_INLINE __WNFloat8::operator WN_FLOAT32 () const {
    return(F8ToF32(mBits));
}

WN_FORCE_INLINE __WNFloat8::operator WN_FLOAT64 () const {
    return(static_cast<WN_FLOAT64>(static_cast<WN_FLOAT32>(*this)));
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator = (WN_INT8 _integer8) {
    *this = __WNFloat8(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator = (WN_INT16 _integer16) {
    *this = __WNFloat8(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator = (WN_INT32 _integer32) {
    *this = __WNFloat8(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator = (WN_INT64 _integer64) {
    *this = __WNFloat8(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator = (WN_UINT8 _integer8) {
    *this = __WNFloat8(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator = (WN_UINT16 _integer16) {
    *this = __WNFloat8(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator = (WN_UINT32 _integer32) {
    *this = __WNFloat8(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator = (WN_UINT64 _integer64) {
    *this = __WNFloat8(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator = (const __WNFixed8& _fixed8) {
    *this = __WNFloat8(_fixed8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator = (const __WNFixed16& _fixed16) {
    *this = __WNFloat8(_fixed16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator = (const __WNFixed32& _fixed32) {
    *this = __WNFloat8(_fixed32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator = (const __WNFixed64& _fixed64) {
    *this = __WNFloat8(_fixed64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator = (const __WNFloat8& _float8) {
    mBits = _float8.mBits;

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator = (const __WNFloat16& _float16) {
    *this = __WNFloat8(_float16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator = (WN_FLOAT32 _float32) {
    *this = __WNFloat8(_float32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator = (WN_FLOAT64 _float64) {
    *this = __WNFloat8(_float64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator + () const {
    return(*this);
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator - () const {
    __WNFloat8 result;

    result.mBits = -mBits;

    return(result);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator ++ () {
    WN_FLOAT32 result = static_cast<WN_FLOAT32>(*this);

    ++result;

    *this = __WNFloat8(result);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator ++ (WN_INT32) {
    const __WNFloat8 result = *this;

    ++(*this);

    return(result);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator -- () {
    WN_FLOAT32 result = static_cast<WN_FLOAT32>(*this);

    ++result;

    *this = __WNFloat8(result);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator -- (WN_INT32) {
    const __WNFloat8 result = *this;

    --(*this);

    return(result);
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator ! () const {
    return(!mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator == (WN_INT8 _integer8) const {
    return(*this == __WNFloat8(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator == (WN_INT16 _integer16) const {
    return(*this == __WNFloat8(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator == (WN_INT32 _integer32) const {
    return(*this == __WNFloat8(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator == (WN_INT64 _integer64) const {
    return(*this == __WNFloat8(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator == (WN_UINT8 _integer8) const {
    return(*this == __WNFloat8(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator == (WN_UINT16 _integer16) const {
    return(*this == __WNFloat8(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator == (WN_UINT32 _integer32) const {
    return(*this == __WNFloat8(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator == (WN_UINT64 _integer64) const {
    return(*this == __WNFloat8(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator == (const __WNFixed8& _fixed8) const {
    return(*this == __WNFloat8(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator == (const __WNFixed16& _fixed16) const {
    return(*this == __WNFloat8(_fixed16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator == (const __WNFixed32& _fixed32) const {
    return(*this == __WNFloat8(_fixed32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator == (const __WNFixed64& _fixed64) const {
    return(*this == __WNFloat8(_fixed64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator == (const __WNFloat8& _float8) const {
    return(mBits == _float8.mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator == (const __WNFloat16& _float16) const {
    return(*this == __WNFloat8(_float16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator == (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) == _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator == (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) == _float64);
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator != (WN_INT8 _integer8) const {
    return(*this != __WNFloat8(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator != (WN_INT16 _integer16) const {
    return(*this != __WNFloat8(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator != (WN_INT32 _integer32) const {
    return(*this != __WNFloat8(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator != (WN_INT64 _integer64) const {
    return(*this != __WNFloat8(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator != (WN_UINT8 _integer8) const {
    return(*this != __WNFloat8(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator != (WN_UINT16 _integer16) const {
    return(*this != __WNFloat8(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator != (WN_UINT32 _integer32) const {
    return(*this != __WNFloat8(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator != (WN_UINT64 _integer64) const {
    return(*this != __WNFloat8(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator != (const __WNFixed8& _fixed8) const {
    return(*this != __WNFloat8(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator != (const __WNFixed16& _fixed16) const {
    return(*this != __WNFloat8(_fixed16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator != (const __WNFixed32& _fixed32) const {
    return(*this != __WNFloat8(_fixed32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator != (const __WNFixed64& _fixed64) const {
    return(*this != __WNFloat8(_fixed64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator != (const __WNFloat8& _float8) const {
    return(mBits != _float8.mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator != (const __WNFloat16& _float16) const {
    return(*this != __WNFloat8(_float16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator != (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) != _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator != (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) != _float64);
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator <= (WN_INT8 _integer8) const {
    return(*this <= __WNFloat8(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator <= (WN_INT16 _integer16) const {
    return(*this <= __WNFloat8(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator <= (WN_INT32 _integer32) const {
    return(*this <= __WNFloat8(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator <= (WN_INT64 _integer64) const {
    return(*this <= __WNFloat8(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator <= (WN_UINT8 _integer8) const {
    return(*this <= __WNFloat8(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator <= (WN_UINT16 _integer16) const {
    return(*this <= __WNFloat8(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator <= (WN_UINT32 _integer32) const {
    return(*this <= __WNFloat8(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator <= (WN_UINT64 _integer64) const {
    return(*this <= __WNFloat8(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator <= (const __WNFixed8& _fixed8) const {
    return(*this <= __WNFloat8(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator <= (const __WNFixed16& _fixed16) const {
    return(*this <= __WNFloat8(_fixed16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator <= (const __WNFixed32& _fixed32) const {
    return(*this <= __WNFloat8(_fixed32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator <= (const __WNFixed64& _fixed64) const {
    return(*this <= __WNFloat8(_fixed64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator <= (const __WNFloat8& _float8) const {
    return(F8ToF32(mBits) <= F8ToF32(_float8.mBits));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator <= (const __WNFloat16& _float16) const {
    return(*this <= __WNFloat8(_float16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator <= (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) <= _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator <= (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) <= _float64);
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator >= (WN_INT8 _integer8) const {
    return(*this >= __WNFloat8(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator >= (WN_INT16 _integer16) const {
    return(*this >= __WNFloat8(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator >= (WN_INT32 _integer32) const {
    return(*this >= __WNFloat8(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator >= (WN_INT64 _integer64) const {
    return(*this >= __WNFloat8(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator >= (WN_UINT8 _integer8) const {
    return(*this >= __WNFloat8(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator >= (WN_UINT16 _integer16) const {
    return(*this >= __WNFloat8(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator >= (WN_UINT32 _integer32) const {
    return(*this >= __WNFloat8(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator >= (WN_UINT64 _integer64) const {
    return(*this >= __WNFloat8(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator >= (const __WNFixed8& _fixed8) const {
    return(*this >= __WNFloat8(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator >= (const __WNFixed16& _fixed16) const {
    return(*this >= __WNFloat8(_fixed16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator >= (const __WNFixed32& _fixed32) const {
    return(*this >= __WNFloat8(_fixed32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator >= (const __WNFixed64& _fixed64) const {
    return(*this >= __WNFloat8(_fixed64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator >= (const __WNFloat8& _float8) const {
    return(F8ToF32(mBits) >= F8ToF32(_float8.mBits));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator >= (const __WNFloat16& _float16) const {
    return(*this >= __WNFloat8(_float16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator >= (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) >= _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator >= (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) >= _float64);
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator < (WN_INT8 _integer8) const {
    return(*this < __WNFloat8(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator < (WN_INT16 _integer16) const {
    return(*this < __WNFloat8(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator < (WN_INT32 _integer32) const {
    return(*this < __WNFloat8(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator < (WN_INT64 _integer64) const {
    return(*this < __WNFloat8(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator < (WN_UINT8 _integer8) const {
    return(*this < __WNFloat8(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator < (WN_UINT16 _integer16) const {
    return(*this < __WNFloat8(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator < (WN_UINT32 _integer32) const {
    return(*this < __WNFloat8(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator < (WN_UINT64 _integer64) const {
    return(*this < __WNFloat8(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator < (const __WNFixed8& _fixed8) const {
    return(*this < __WNFloat8(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator < (const __WNFixed16& _fixed16) const {
    return(*this < __WNFloat8(_fixed16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator < (const __WNFixed32& _fixed32) const {
    return(*this < __WNFloat8(_fixed32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator < (const __WNFixed64& _fixed64) const {
    return(*this < __WNFloat8(_fixed64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator < (const __WNFloat8& _float8) const {
    return(F8ToF32(mBits) < F8ToF32(_float8.mBits));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator < (const __WNFloat16& _float16) const {
    return(*this < __WNFloat8(_float16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator < (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) < _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator < (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) < _float64);
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator > (WN_INT8 _integer8) const {
    return(*this > __WNFloat8(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator > (WN_INT16 _integer16) const {
    return(*this > __WNFloat8(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator > (WN_INT32 _integer32) const {
    return(*this > __WNFloat8(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator > (WN_INT64 _integer64) const {
    return(*this > __WNFloat8(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator > (WN_UINT8 _integer8) const {
    return(*this > __WNFloat8(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator > (WN_UINT16 _integer16) const {
    return(*this > __WNFloat8(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator > (WN_UINT32 _integer32) const {
    return(*this > __WNFloat8(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator > (WN_UINT64 _integer64) const {
    return(*this > __WNFloat8(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator > (const __WNFixed8& _fixed8) const {
    return(*this > __WNFloat8(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator > (const __WNFixed16& _fixed16) const {
    return(*this > __WNFloat8(_fixed16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator > (const __WNFixed32& _fixed32) const {
    return(*this > __WNFloat8(_fixed32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator > (const __WNFixed64& _fixed64) const {
    return(*this > __WNFloat8(_fixed64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator > (const __WNFloat8& _float8) const {
    return(F8ToF32(mBits) > F8ToF32(_float8.mBits));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator > (const __WNFloat16& _float16) const {
    return(*this > __WNFloat8(_float16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator > (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) > _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFloat8::operator > (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) > _float64);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator += (WN_INT8 _integer8) {
    *this += __WNFloat8(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator += (WN_INT16 _integer16) {
    *this += __WNFloat8(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator += (WN_INT32 _integer32) {
    *this += __WNFloat8(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator += (WN_INT64 _integer64) {
    *this += __WNFloat8(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator += (WN_UINT8 _integer8) {
    *this += __WNFloat8(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator += (WN_UINT16 _integer16) {
    *this += __WNFloat8(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator += (WN_UINT32 _integer32) {
    *this += __WNFloat8(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator += (WN_UINT64 _integer64) {
    *this += __WNFloat8(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator += (const __WNFixed8& _fixed8) {
    *this += __WNFloat8(_fixed8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator += (const __WNFixed16& _fixed16) {
    *this += __WNFloat8(_fixed16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator += (const __WNFixed32& _fixed32) {
    *this += __WNFloat8(_fixed32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator += (const __WNFixed64& _fixed64) {
    *this += __WNFloat8(_fixed64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator += (const __WNFloat8& _float8) {
    mBits = F32ToF8(F8ToF32(mBits) + F8ToF32(_float8.mBits));

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator += (const __WNFloat16& _float16) {
    *this += __WNFloat8(_float16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator += (WN_FLOAT32 _float32) {
    *this += __WNFloat8(_float32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator += (WN_FLOAT64 _float64) {
    *this += __WNFloat8(_float64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator -= (WN_INT8 _integer8) {
    *this -= __WNFloat8(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator -= (WN_INT16 _integer16) {
    *this -= __WNFloat8(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator -= (WN_INT32 _integer32) {
    *this -= __WNFloat8(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator -= (WN_INT64 _integer64) {
    *this -= __WNFloat8(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator -= (WN_UINT8 _integer8) {
    *this -= __WNFloat8(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator -= (WN_UINT16 _integer16) {
    *this -= __WNFloat8(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator -= (WN_UINT32 _integer32) {
    *this -= __WNFloat8(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator -= (WN_UINT64 _integer64) {
    *this -= __WNFloat8(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator -= (const __WNFixed8& _fixed8) {
    *this -= __WNFloat8(_fixed8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator -= (const __WNFixed16& _fixed16) {
    *this -= __WNFloat8(_fixed16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator -= (const __WNFixed32& _fixed32) {
    *this -= __WNFloat8(_fixed32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator -= (const __WNFixed64& _fixed64) {
    *this -= __WNFloat8(_fixed64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator -= (const __WNFloat8& _float8) {
    mBits = F32ToF8(F8ToF32(mBits) - F8ToF32(_float8.mBits));

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator -= (const __WNFloat16& _float16) {
    *this -= __WNFloat8(_float16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator -= (WN_FLOAT32 _float32) {
    *this -= __WNFloat8(_float32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator -= (WN_FLOAT64 _float64) {
    *this -= __WNFloat8(_float64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator *= (WN_INT8 _integer8) {
    *this *= __WNFloat8(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator *= (WN_INT16 _integer16) {
    *this *= __WNFloat8(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator *= (WN_INT32 _integer32) {
    *this *= __WNFloat8(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator *= (WN_INT64 _integer64) {
    *this *= __WNFloat8(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator *= (WN_UINT8 _integer8) {
    *this *= __WNFloat8(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator *= (WN_UINT16 _integer16) {
    *this *= __WNFloat8(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator *= (WN_UINT32 _integer32) {
    *this *= __WNFloat8(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator *= (WN_UINT64 _integer64) {
    *this *= __WNFloat8(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator *= (const __WNFixed8& _fixed8) {
    *this *= __WNFloat8(_fixed8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator *= (const __WNFixed16& _fixed16) {
    *this *= __WNFloat8(_fixed16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator *= (const __WNFixed32& _fixed32) {
    *this *= __WNFloat8(_fixed32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator *= (const __WNFixed64& _fixed64) {
    *this *= __WNFloat8(_fixed64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator *= (const __WNFloat8& _float8) {
    mBits = F32ToF8(F8ToF32(mBits) * F8ToF32(_float8.mBits));

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator *= (const __WNFloat16& _float16) {
    *this *= __WNFloat8(_float16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator *= (WN_FLOAT32 _float32) {
    *this *= __WNFloat8(_float32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator *= (WN_FLOAT64 _float64) {
    *this *= __WNFloat8(_float64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator /= (WN_INT8 _integer8) {
    *this /= __WNFloat8(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator /= (WN_INT16 _integer16) {
    *this /= __WNFloat8(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator /= (WN_INT32 _integer32) {
    *this /= __WNFloat8(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator /= (WN_INT64 _integer64) {
    *this /= __WNFloat8(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator /= (WN_UINT8 _integer8) {
    *this /= __WNFloat8(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator /= (WN_UINT16 _integer16) {
    *this /= __WNFloat8(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator /= (WN_UINT32 _integer32) {
    *this /= __WNFloat8(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator /= (WN_UINT64 _integer64) {
    *this /= __WNFloat8(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator /= (const __WNFixed8& _fixed8) {
    *this /= __WNFloat8(_fixed8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator /= (const __WNFixed16& _fixed16) {
    *this /= __WNFloat8(_fixed16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator /= (const __WNFixed32& _fixed32) {
    *this /= __WNFloat8(_fixed32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator /= (const __WNFixed64& _fixed64) {
    *this /= __WNFloat8(_fixed64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator /= (const __WNFloat8& _float8) {
    mBits = F32ToF8(F8ToF32(mBits) / F8ToF32(_float8.mBits));

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator /= (const __WNFloat16& _float16) {
    *this /= __WNFloat8(_float16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator /= (WN_FLOAT32 _float32) {
    *this /= __WNFloat8(_float32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8& __WNFloat8::operator /= (WN_FLOAT64 _float64) {
    *this /= __WNFloat8(_float64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator + (WN_INT8 _integer8) const {
    return(*this + __WNFloat8(_integer8));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator + (WN_INT16 _integer16) const {
    return(*this + __WNFloat8(_integer16));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator + (WN_INT32 _integer32) const {
    return(*this + __WNFloat8(_integer32));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator + (WN_INT64 _integer64) const {
    return(*this + __WNFloat8(_integer64));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator + (WN_UINT8 _integer8) const {
    return(*this + __WNFloat8(_integer8));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator + (WN_UINT16 _integer16) const {
    return(*this + __WNFloat8(_integer16));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator + (WN_UINT32 _integer32) const {
    return(*this + __WNFloat8(_integer32));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator + (WN_UINT64 _integer64) const {
    return(*this + __WNFloat8(_integer64));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator + (const __WNFixed8& _fixed8) const {
    return(*this + __WNFloat8(_fixed8));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator + (const __WNFixed16& _fixed16) const {
    return(*this + __WNFloat8(_fixed16));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator + (const __WNFixed32& _fixed32) const {
    return(*this + __WNFloat8(_fixed32));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator + (const __WNFixed64& _fixed64) const {
    return(*this + __WNFloat8(_fixed64));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator + (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) += _float8);
}

WN_FORCE_INLINE __WNFloat16 __WNFloat8::operator + (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) + _float16);
}

WN_FORCE_INLINE WN_FLOAT32 __WNFloat8::operator + (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) + _float32);
}

WN_FORCE_INLINE WN_FLOAT64 __WNFloat8::operator + (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) + _float64);
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator - (WN_INT8 _integer8) const {
    return(*this - __WNFloat8(_integer8));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator - (WN_INT16 _integer16) const {
    return(*this - __WNFloat8(_integer16));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator - (WN_INT32 _integer32) const {
    return(*this - __WNFloat8(_integer32));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator - (WN_INT64 _integer64) const {
    return(*this - __WNFloat8(_integer64));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator - (WN_UINT8 _integer8) const {
    return(*this - __WNFloat8(_integer8));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator - (WN_UINT16 _integer16) const {
    return(*this - __WNFloat8(_integer16));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator - (WN_UINT32 _integer32) const {
    return(*this - __WNFloat8(_integer32));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator - (WN_UINT64 _integer64) const {
    return(*this - __WNFloat8(_integer64));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator - (const __WNFixed8& _fixed8) const {
    return(*this - __WNFloat8(_fixed8));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator - (const __WNFixed16& _fixed16) const {
    return(*this - __WNFloat8(_fixed16));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator - (const __WNFixed32& _fixed32) const {
    return(*this - __WNFloat8(_fixed32));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator - (const __WNFixed64& _fixed64) const {
    return(*this - __WNFloat8(_fixed64));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator - (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) -= _float8);
}

WN_FORCE_INLINE __WNFloat16 __WNFloat8::operator - (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) - _float16);
}

WN_FORCE_INLINE WN_FLOAT32 __WNFloat8::operator - (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) - _float32);
}

WN_FORCE_INLINE WN_FLOAT64 __WNFloat8::operator - (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) - _float64);
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator * (WN_INT8 _integer8) const {
    return(*this * __WNFloat8(_integer8));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator * (WN_INT16 _integer16) const {
    return(*this * __WNFloat8(_integer16));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator * (WN_INT32 _integer32) const {
    return(*this * __WNFloat8(_integer32));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator * (WN_INT64 _integer64) const {
    return(*this * __WNFloat8(_integer64));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator * (WN_UINT8 _integer8) const {
    return(*this * __WNFloat8(_integer8));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator * (WN_UINT16 _integer16) const {
    return(*this * __WNFloat8(_integer16));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator * (WN_UINT32 _integer32) const {
    return(*this * __WNFloat8(_integer32));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator * (WN_UINT64 _integer64) const {
    return(*this * __WNFloat8(_integer64));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator * (const __WNFixed8& _fixed8) const {
    return(*this * __WNFloat8(_fixed8));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator * (const __WNFixed16& _fixed16) const {
    return(*this * __WNFloat8(_fixed16));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator * (const __WNFixed32& _fixed32) const {
    return(*this * __WNFloat8(_fixed32));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator * (const __WNFixed64& _fixed64) const {
    return(*this * __WNFloat8(_fixed64));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator * (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) *= _float8);
}

WN_FORCE_INLINE __WNFloat16 __WNFloat8::operator * (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) * _float16);
}

WN_FORCE_INLINE WN_FLOAT32 __WNFloat8::operator * (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) * _float32);
}

WN_FORCE_INLINE WN_FLOAT64 __WNFloat8::operator * (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) * _float64);
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator / (WN_INT8 _integer8) const {
    return(*this / __WNFloat8(_integer8));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator / (WN_INT16 _integer16) const {
    return(*this / __WNFloat8(_integer16));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator / (WN_INT32 _integer32) const {
    return(*this / __WNFloat8(_integer32));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator / (WN_INT64 _integer64) const {
    return(*this / __WNFloat8(_integer64));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator / (WN_UINT8 _integer8) const {
    return(*this / __WNFloat8(_integer8));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator / (WN_UINT16 _integer16) const {
    return(*this / __WNFloat8(_integer16));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator / (WN_UINT32 _integer32) const {
    return(*this / __WNFloat8(_integer32));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator / (WN_UINT64 _integer64) const {
    return(*this / __WNFloat8(_integer64));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator / (const __WNFixed8& _fixed8) const {
    return(*this / __WNFloat8(_fixed8));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator / (const __WNFixed16& _fixed16) const {
    return(*this / __WNFloat8(_fixed16));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator / (const __WNFixed32& _fixed32) const {
    return(*this / __WNFloat8(_fixed32));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator / (const __WNFixed64& _fixed64) const {
    return(*this / __WNFloat8(_fixed64));
}

WN_FORCE_INLINE __WNFloat8 __WNFloat8::operator / (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) /= _float8);
}

WN_FORCE_INLINE __WNFloat16 __WNFloat8::operator / (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) / _float16);
}

WN_FORCE_INLINE WN_FLOAT32 __WNFloat8::operator / (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) / _float32);
}

WN_FORCE_INLINE WN_FLOAT64 __WNFloat8::operator / (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) / _float64);
}

WN_FORCE_INLINE WN_INT8 __WNFloat8::F32ToF8(WN_FLOAT32 _float32) {
    WN_UNUSED_ARG(_float32);

    return(1);
}

WN_FORCE_INLINE WN_FLOAT32 __WNFloat8::F8ToF32(WN_INT8 _float8) {
    WN_UNUSED_ARG(_float8);

    return(1.0f);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_INT8 _integer8, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer8) == _float8);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_INT16 _integer16, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer16) == _float8);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_INT32 _integer32, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer32) == _float8);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_INT64 _integer64, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer64) == _float8);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_UINT8 _integer8, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer8) == _float8);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_UINT16 _integer16, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer16) == _float8);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_UINT32 _integer32, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer32) == _float8);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_UINT64 _integer64, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer64) == _float8);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_FLOAT32 _float32, const __WNFloat8& _float8) {
    return(_float32 != static_cast<WN_FLOAT32>(_float8));
}

WN_FORCE_INLINE WN_BOOL operator == (WN_FLOAT64 _float64, const __WNFloat8& _float8) {
    return(_float64 != static_cast<WN_FLOAT64>(_float8));
}

WN_FORCE_INLINE WN_BOOL operator != (WN_INT8 _integer8, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer8) != _float8);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_INT16 _integer16, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer16) != _float8);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_INT32 _integer32, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer32) != _float8);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_INT64 _integer64, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer64) != _float8);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_UINT8 _integer8, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer8) != _float8);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_UINT16 _integer16, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer16) != _float8);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_UINT32 _integer32, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer32) != _float8);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_UINT64 _integer64, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer64) != _float8);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_FLOAT32 _float32, const __WNFloat8& _float8) {
    return(_float32 != static_cast<WN_FLOAT32>(_float8));
}

WN_FORCE_INLINE WN_BOOL operator != (WN_FLOAT64 _float64, const __WNFloat8& _float8) {
    return(_float64 != static_cast<WN_FLOAT64>(_float8));
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_INT8 _integer8, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer8) <= _float8);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_INT16 _integer16, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer16) <= _float8);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_INT32 _integer32, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer32) <= _float8);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_INT64 _integer64, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer64) <= _float8);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_UINT8 _integer8, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer8) <= _float8);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_UINT16 _integer16, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer16) <= _float8);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_UINT32 _integer32, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer32) <= _float8);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_UINT64 _integer64, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer64) <= _float8);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_FLOAT32 _float32, const __WNFloat8& _float8) {
    return(_float32 <= static_cast<WN_FLOAT32>(_float8));
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_FLOAT64 _float64, const __WNFloat8& _float8) {
    return(_float64 <= static_cast<WN_FLOAT64>(_float8));
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_INT8 _integer8, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer8) >= _float8);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_INT16 _integer16, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer16) >= _float8);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_INT32 _integer32, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer32) >= _float8);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_INT64 _integer64, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer64) >= _float8);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_UINT8 _integer8, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer8) >= _float8);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_UINT16 _integer16, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer16) >= _float8);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_UINT32 _integer32, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer32) >= _float8);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_UINT64 _integer64, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer64) >= _float8);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_FLOAT32 _float32, const __WNFloat8& _float8) {
    return(_float32 >= static_cast<WN_FLOAT32>(_float8));
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_FLOAT64 _float64, const __WNFloat8& _float8) {
    return(_float64 >= static_cast<WN_FLOAT64>(_float8));
}

WN_FORCE_INLINE WN_BOOL operator < (WN_INT8 _integer8, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer8) < _float8);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_INT16 _integer16, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer16) < _float8);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_INT32 _integer32, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer32) < _float8);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_INT64 _integer64, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer64) < _float8);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_UINT8 _integer8, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer8) < _float8);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_UINT16 _integer16, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer16) < _float8);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_UINT32 _integer32, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer32) < _float8);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_UINT64 _integer64, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer64) < _float8);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_FLOAT32 _float32, const __WNFloat8& _float8) {
    return(_float32 < static_cast<WN_FLOAT32>(_float8));
}

WN_FORCE_INLINE WN_BOOL operator < (WN_FLOAT64 _float64, const __WNFloat8& _float8) {
    return(_float64 < static_cast<WN_FLOAT64>(_float8));
}

WN_FORCE_INLINE WN_BOOL operator > (WN_INT8 _integer8, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer8) > _float8);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_INT16 _integer16, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer16) > _float8);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_INT32 _integer32, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer32) > _float8);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_INT64 _integer64, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer64) > _float8);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_UINT8 _integer8, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer8) > _float8);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_UINT16 _integer16, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer16) > _float8);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_UINT32 _integer32, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer32) > _float8);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_UINT64 _integer64, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer64) > _float8);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_FLOAT32 _float32, const __WNFloat8& _float8) {
    return(_float32 > static_cast<WN_FLOAT32>(_float8));
}

WN_FORCE_INLINE WN_BOOL operator > (WN_FLOAT64 _float64, const __WNFloat8& _float8) {
    return(_float64 > static_cast<WN_FLOAT64>(_float8));
}

WN_FORCE_INLINE WN_INT8& operator += (WN_INT8& _integer8, const __WNFloat8& _float8) {
    _integer8 += static_cast<WN_INT8>(_float8);

    return(_integer8);
}

WN_FORCE_INLINE WN_INT16& operator += (WN_INT16& _integer16, const __WNFloat8& _float8) {
    _integer16 += static_cast<WN_INT16>(_float8);

    return(_integer16);
}

WN_FORCE_INLINE WN_INT32& operator += (WN_INT32& _integer32, const __WNFloat8& _float8) {
    _integer32 += static_cast<WN_INT32>(_float8);

    return(_integer32);
}

WN_FORCE_INLINE WN_INT64& operator += (WN_INT64& _integer64, const __WNFloat8& _float8) {
    _integer64 += static_cast<WN_INT64>(_float8);

    return(_integer64);
}

WN_FORCE_INLINE WN_UINT8& operator += (WN_UINT8& _integer8, const __WNFloat8& _float8) {
    _integer8 += static_cast<WN_UINT8>(_float8);

    return(_integer8);
}

WN_FORCE_INLINE WN_UINT16& operator += (WN_UINT16& _integer16, const __WNFloat8& _float8) {
    _integer16 += static_cast<WN_UINT16>(_float8);

    return(_integer16);
}

WN_FORCE_INLINE WN_UINT32& operator += (WN_UINT32& _integer32, const __WNFloat8& _float8) {
    _integer32 += static_cast<WN_UINT32>(_float8);

    return(_integer32);
}

WN_FORCE_INLINE WN_UINT64& operator += (WN_UINT64& _integer64, const __WNFloat8& _float8) {
    _integer64 += static_cast<WN_UINT64>(_float8);

    return(_integer64);
}

WN_FORCE_INLINE WN_FLOAT32& operator += (WN_FLOAT32& _float32, const __WNFloat8& _float8) {
    _float32 += static_cast<WN_FLOAT32>(_float8);

    return(_float32);
}

WN_FORCE_INLINE WN_FLOAT64& operator += (WN_FLOAT64& _float64, const __WNFloat8& _float8) {
    _float64 += static_cast<WN_FLOAT64>(_float8);

    return(_float64);
}

WN_FORCE_INLINE WN_INT8& operator -= (WN_INT8& _integer8, const __WNFloat8& _float8) {
    _integer8 -= static_cast<WN_INT8>(_float8);

    return(_integer8);
}

WN_FORCE_INLINE WN_INT16& operator -= (WN_INT16& _integer16, const __WNFloat8& _float8) {
    _integer16 -= static_cast<WN_INT16>(_float8);

    return(_integer16);
}

WN_FORCE_INLINE WN_INT32& operator -= (WN_INT32& _integer32, const __WNFloat8& _float8) {
    _integer32 -= static_cast<WN_INT32>(_float8);

    return(_integer32);
}

WN_FORCE_INLINE WN_INT64& operator -= (WN_INT64& _integer64, const __WNFloat8& _float8) {
    _integer64 -= static_cast<WN_INT64>(_float8);

    return(_integer64);
}

WN_FORCE_INLINE WN_UINT8& operator -= (WN_UINT8& _integer8, const __WNFloat8& _float8) {
    _integer8 -= static_cast<WN_UINT8>(_float8);

    return(_integer8);
}

WN_FORCE_INLINE WN_UINT16& operator -= (WN_UINT16& _integer16, const __WNFloat8& _float8) {
    _integer16 -= static_cast<WN_UINT16>(_float8);

    return(_integer16);
}

WN_FORCE_INLINE WN_UINT32& operator -= (WN_UINT32& _integer32, const __WNFloat8& _float8) {
    _integer32 -= static_cast<WN_UINT32>(_float8);

    return(_integer32);
}

WN_FORCE_INLINE WN_UINT64& operator -= (WN_UINT64& _integer64, const __WNFloat8& _float8) {
    _integer64 -= static_cast<WN_UINT64>(_float8);

    return(_integer64);
}

WN_FORCE_INLINE WN_FLOAT32& operator -= (WN_FLOAT32& _float32, const __WNFloat8& _float8) {
    _float32 -= static_cast<WN_FLOAT32>(_float8);

    return(_float32);
}

WN_FORCE_INLINE WN_FLOAT64& operator -= (WN_FLOAT64& _float64, const __WNFloat8& _float8) {
    _float64 -= static_cast<WN_FLOAT64>(_float8);

    return(_float64);
}

WN_FORCE_INLINE WN_INT8& operator *= (WN_INT8& _integer8, const __WNFloat8& _float8) {
    _integer8 *= static_cast<WN_INT8>(_float8);

    return(_integer8);
}

WN_FORCE_INLINE WN_INT16& operator *= (WN_INT16& _integer16, const __WNFloat8& _float8) {
    _integer16 *= static_cast<WN_INT16>(_float8);

    return(_integer16);
}

WN_FORCE_INLINE WN_INT32& operator *= (WN_INT32& _integer32, const __WNFloat8& _float8) {
    _integer32 *= static_cast<WN_INT32>(_float8);

    return(_integer32);
}

WN_FORCE_INLINE WN_INT64& operator *= (WN_INT64& _integer64, const __WNFloat8& _float8) {
    _integer64 *= static_cast<WN_INT64>(_float8);

    return(_integer64);
}

WN_FORCE_INLINE WN_UINT8& operator *= (WN_UINT8& _integer8, const __WNFloat8& _float8) {
    _integer8 *= static_cast<WN_UINT8>(_float8);

    return(_integer8);
}

WN_FORCE_INLINE WN_UINT16& operator *= (WN_UINT16& _integer16, const __WNFloat8& _float8) {
    _integer16 *= static_cast<WN_UINT16>(_float8);

    return(_integer16);
}

WN_FORCE_INLINE WN_UINT32& operator *= (WN_UINT32& _integer32, const __WNFloat8& _float8) {
    _integer32 *= static_cast<WN_UINT32>(_float8);

    return(_integer32);
}

WN_FORCE_INLINE WN_UINT64& operator *= (WN_UINT64& _integer64, const __WNFloat8& _float8) {
    _integer64 *= static_cast<WN_UINT64>(_float8);

    return(_integer64);
}

WN_FORCE_INLINE WN_FLOAT32& operator *= (WN_FLOAT32& _float32, const __WNFloat8& _float8) {
    _float32 *= static_cast<WN_FLOAT32>(_float8);

    return(_float32);
}

WN_FORCE_INLINE WN_FLOAT64& operator *= (WN_FLOAT64& _float64, const __WNFloat8& _float8) {
    _float64 *= static_cast<WN_FLOAT64>(_float8);

    return(_float64);
}

WN_FORCE_INLINE WN_INT8& operator /= (WN_INT8& _integer8, const __WNFloat8& _float8) {
    _integer8 /= static_cast<WN_INT8>(_float8);

    return(_integer8);
}

WN_FORCE_INLINE WN_INT16& operator /= (WN_INT16& _integer16, const __WNFloat8& _float8) {
    _integer16 /= static_cast<WN_INT16>(_float8);

    return(_integer16);
}

WN_FORCE_INLINE WN_INT32& operator /= (WN_INT32& _integer32, const __WNFloat8& _float8) {
    _integer32 /= static_cast<WN_INT32>(_float8);

    return(_integer32);
}

WN_FORCE_INLINE WN_INT64& operator /= (WN_INT64& _integer64, const __WNFloat8& _float8) {
    _integer64 /= static_cast<WN_INT64>(_float8);

    return(_integer64);
}

WN_FORCE_INLINE WN_UINT8& operator /= (WN_UINT8& _integer8, const __WNFloat8& _float8) {
    _integer8 /= static_cast<WN_UINT8>(_float8);

    return(_integer8);
}

WN_FORCE_INLINE WN_UINT16& operator /= (WN_UINT16& _integer16, const __WNFloat8& _float8) {
    _integer16 /= static_cast<WN_UINT16>(_float8);

    return(_integer16);
}

WN_FORCE_INLINE WN_UINT32& operator /= (WN_UINT32& _integer32, const __WNFloat8& _float8) {
    _integer32 /= static_cast<WN_UINT32>(_float8);

    return(_integer32);
}

WN_FORCE_INLINE WN_UINT64& operator /= (WN_UINT64& _integer64, const __WNFloat8& _float8) {
    _integer64 /= static_cast<WN_UINT64>(_float8);

    return(_integer64);
}

WN_FORCE_INLINE WN_FLOAT32& operator /= (WN_FLOAT32& _float32, const __WNFloat8& _float8) {
    _float32 /= static_cast<WN_FLOAT32>(_float8);

    return(_float32);
}

WN_FORCE_INLINE WN_FLOAT64& operator /= (WN_FLOAT64& _float64, const __WNFloat8& _float8) {
    _float64 /= static_cast<WN_FLOAT64>(_float8);

    return(_float64);
}

WN_FORCE_INLINE __WNFloat8 operator + (WN_INT8 _integer8, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer8) + _float8);
}

WN_FORCE_INLINE __WNFloat8 operator + (WN_INT16 _integer16, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer16) + _float8);
}

WN_FORCE_INLINE __WNFloat8 operator + (WN_INT32 _integer32, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer32) + _float8);
}

WN_FORCE_INLINE __WNFloat8 operator + (WN_INT64 _integer64, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer64) + _float8);
}

WN_FORCE_INLINE __WNFloat8 operator + (WN_UINT8 _integer8, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer8) + _float8);
}

WN_FORCE_INLINE __WNFloat8 operator + (WN_UINT16 _integer16, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer16) + _float8);
}

WN_FORCE_INLINE __WNFloat8 operator + (WN_UINT32 _integer32, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer32) + _float8);
}

WN_FORCE_INLINE __WNFloat8 operator + (WN_UINT64 _integer64, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer64) + _float8);
}

WN_FORCE_INLINE WN_FLOAT32 operator + (WN_FLOAT32 _float32, const __WNFloat8& _float8) {
    return(_float32 + static_cast<WN_FLOAT32>(_float8));
}

WN_FORCE_INLINE WN_FLOAT64 operator + (WN_FLOAT64 _float64, const __WNFloat8& _float8) {
    return(_float64 + static_cast<WN_FLOAT64>(_float8));
}

WN_FORCE_INLINE __WNFloat8 operator - (WN_INT8 _integer8, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer8) - _float8);
}

WN_FORCE_INLINE __WNFloat8 operator - (WN_INT16 _integer16, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer16) - _float8);
}

WN_FORCE_INLINE __WNFloat8 operator - (WN_INT32 _integer32, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer32) - _float8);
}

WN_FORCE_INLINE __WNFloat8 operator - (WN_INT64 _integer64, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer64) - _float8);
}

WN_FORCE_INLINE __WNFloat8 operator - (WN_UINT8 _integer8, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer8) - _float8);
}

WN_FORCE_INLINE __WNFloat8 operator - (WN_UINT16 _integer16, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer16) - _float8);
}

WN_FORCE_INLINE __WNFloat8 operator - (WN_UINT32 _integer32, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer32) - _float8);
}

WN_FORCE_INLINE __WNFloat8 operator - (WN_UINT64 _integer64, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer64) - _float8);
}

WN_FORCE_INLINE WN_FLOAT32 operator - (WN_FLOAT32 _float32, const __WNFloat8& _float8) {
    return(_float32 - static_cast<WN_FLOAT32>(_float8));
}

WN_FORCE_INLINE WN_FLOAT64 operator - (WN_FLOAT64 _float64, const __WNFloat8& _float8) {
    return(_float64 - static_cast<WN_FLOAT64>(_float8));
}

WN_FORCE_INLINE __WNFloat8 operator * (WN_INT8 _integer8, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer8) * _float8);
}

WN_FORCE_INLINE __WNFloat8 operator * (WN_INT16 _integer16, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer16) * _float8);
}

WN_FORCE_INLINE __WNFloat8 operator * (WN_INT32 _integer32, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer32) * _float8);
}

WN_FORCE_INLINE __WNFloat8 operator * (WN_INT64 _integer64, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer64) * _float8);
}

WN_FORCE_INLINE __WNFloat8 operator * (WN_UINT8 _integer8, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer8) * _float8);
}

WN_FORCE_INLINE __WNFloat8 operator * (WN_UINT16 _integer16, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer16) * _float8);
}

WN_FORCE_INLINE __WNFloat8 operator * (WN_UINT32 _integer32, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer32) * _float8);
}

WN_FORCE_INLINE __WNFloat8 operator * (WN_UINT64 _integer64, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer64) * _float8);
}

WN_FORCE_INLINE WN_FLOAT32 operator * (WN_FLOAT32 _float32, const __WNFloat8& _float8) {
    return(_float32 * static_cast<WN_FLOAT32>(_float8));
}

WN_FORCE_INLINE WN_FLOAT64 operator * (WN_FLOAT64 _float64, const __WNFloat8& _float8) {
    return(_float64 * static_cast<WN_FLOAT64>(_float8));
}

WN_FORCE_INLINE __WNFloat8 operator / (WN_INT8 _integer8, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer8) / _float8);
}

WN_FORCE_INLINE __WNFloat8 operator / (WN_INT16 _integer16, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer16) / _float8);
}

WN_FORCE_INLINE __WNFloat8 operator / (WN_INT32 _integer32, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer32) / _float8);
}

WN_FORCE_INLINE __WNFloat8 operator / (WN_INT64 _integer64, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer64) / _float8);
}

WN_FORCE_INLINE __WNFloat8 operator / (WN_UINT8 _integer8, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer8) / _float8);
}

WN_FORCE_INLINE __WNFloat8 operator / (WN_UINT16 _integer16, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer16) / _float8);
}

WN_FORCE_INLINE __WNFloat8 operator / (WN_UINT32 _integer32, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer32) / _float8);
}

WN_FORCE_INLINE __WNFloat8 operator / (WN_UINT64 _integer64, const __WNFloat8& _float8) {
    return(__WNFloat8(_integer64) / _float8);
}

WN_FORCE_INLINE WN_FLOAT32 operator / (WN_FLOAT32 _float32, const __WNFloat8& _float8) {
    return(_float32 / static_cast<WN_FLOAT32>(_float8));
}

WN_FORCE_INLINE WN_FLOAT64 operator / (WN_FLOAT64 _float64, const __WNFloat8& _float8) {
    return(_float64 / static_cast<WN_FLOAT64>(_float8));
}

// Float (16 bit) //////////////////////////////////////////////////////////////////////////////////////////////////////////////

WN_FORCE_INLINE __WNFloat16::__WNFloat16(WN_INT8 _integer8) {
    mBits = F32ToF16(static_cast<WN_FLOAT32>(_integer8));
}

WN_FORCE_INLINE __WNFloat16::__WNFloat16(WN_INT16 _integer16) {
    mBits = F32ToF16(static_cast<WN_FLOAT32>(_integer16));
}

WN_FORCE_INLINE __WNFloat16::__WNFloat16(WN_INT32 _integer32) {
    mBits = F32ToF16(static_cast<WN_FLOAT32>(_integer32));
}

WN_FORCE_INLINE __WNFloat16::__WNFloat16(WN_INT64 _integer64) {
    mBits = F32ToF16(static_cast<WN_FLOAT32>(_integer64));
}

WN_FORCE_INLINE __WNFloat16::__WNFloat16(WN_UINT8 _integer8) {
    mBits = F32ToF16(static_cast<WN_FLOAT32>(_integer8));
}

WN_FORCE_INLINE __WNFloat16::__WNFloat16(WN_UINT16 _integer16) {
    mBits = F32ToF16(static_cast<WN_FLOAT32>(_integer16));
}

WN_FORCE_INLINE __WNFloat16::__WNFloat16(WN_UINT32 _integer32) {
    mBits = F32ToF16(static_cast<WN_FLOAT32>(_integer32));
}

WN_FORCE_INLINE __WNFloat16::__WNFloat16(WN_UINT64 _integer64) {
    mBits = F32ToF16(static_cast<WN_FLOAT32>(_integer64));
}

WN_FORCE_INLINE __WNFloat16::__WNFloat16(const __WNFixed8& _fixed8) {
    mBits = F32ToF16(static_cast<WN_FLOAT32>(_fixed8));
}

WN_FORCE_INLINE __WNFloat16::__WNFloat16(const __WNFixed16& _fixed16) {
    mBits = F32ToF16(static_cast<WN_FLOAT32>(_fixed16));
}

WN_FORCE_INLINE __WNFloat16::__WNFloat16(const __WNFixed32& _fixed32) {
    mBits = F32ToF16(static_cast<WN_FLOAT32>(_fixed32));
}

WN_FORCE_INLINE __WNFloat16::__WNFloat16(const __WNFixed64& _fixed64) {
    mBits = F32ToF16(static_cast<WN_FLOAT32>(_fixed64));
}

WN_FORCE_INLINE  __WNFloat16::__WNFloat16(const __WNFloat8& _float8) {
    mBits = F32ToF16(static_cast<WN_FLOAT32>(_float8));
}

WN_FORCE_INLINE  __WNFloat16::__WNFloat16(const __WNFloat16& _float16) {
    mBits = _float16.mBits;
}

WN_FORCE_INLINE __WNFloat16::__WNFloat16(WN_FLOAT32 _float32) {
    mBits = F32ToF16(_float32);
}

WN_FORCE_INLINE __WNFloat16::__WNFloat16(WN_FLOAT64 _float64) {
    mBits = F32ToF16(static_cast<WN_FLOAT32>(_float64));
}

WN_FORCE_INLINE __WNFloat16::operator WN_INT8 () const {
    return(static_cast<WN_INT8>(static_cast<WN_FLOAT32>(*this)));
}

WN_FORCE_INLINE __WNFloat16::operator WN_INT16 () const {
    return(static_cast<WN_INT16>(static_cast<WN_FLOAT32>(*this)));
}

WN_FORCE_INLINE __WNFloat16::operator WN_INT32 () const {
    return(static_cast<WN_INT32>(static_cast<WN_FLOAT32>(*this)));
}

WN_FORCE_INLINE __WNFloat16::operator WN_INT64 () const {
    return(static_cast<WN_INT64>(static_cast<WN_FLOAT32>(*this)));
}

WN_FORCE_INLINE __WNFloat16::operator WN_UINT8 () const {
    return(static_cast<WN_UINT8>(static_cast<WN_FLOAT32>(*this)));
}

WN_FORCE_INLINE __WNFloat16::operator WN_UINT16 () const {
    return(static_cast<WN_UINT16>(static_cast<WN_FLOAT32>(*this)));
}

WN_FORCE_INLINE __WNFloat16::operator WN_UINT32 () const {
    return(static_cast<WN_UINT32>(static_cast<WN_FLOAT32>(*this)));
}

WN_FORCE_INLINE __WNFloat16::operator WN_UINT64 () const {
    return(static_cast<WN_UINT64>(static_cast<WN_FLOAT32>(*this)));
}

WN_FORCE_INLINE __WNFloat16::operator WN_FLOAT32 () const {
    return(F16ToF32(mBits));
}

WN_FORCE_INLINE __WNFloat16::operator WN_FLOAT64 () const {
    return(static_cast<WN_FLOAT64>(static_cast<WN_FLOAT32>(*this)));
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator = (WN_INT8 _integer8) {
    *this = __WNFloat16(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator = (WN_INT16 _integer16) {
    *this = __WNFloat16(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator = (WN_INT32 _integer32) {
    *this = __WNFloat16(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator = (WN_INT64 _integer64) {
    *this = __WNFloat16(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator = (WN_UINT8 _integer8) {
    *this = __WNFloat16(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator = (WN_UINT16 _integer16) {
    *this = __WNFloat16(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator = (WN_UINT32 _integer32) {
    *this = __WNFloat16(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator = (WN_UINT64 _integer64) {
    *this = __WNFloat16(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator = (const __WNFixed8& _fixed8) {
    *this = __WNFloat16(_fixed8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator = (const __WNFixed16& _fixed16) {
    *this = __WNFloat16(_fixed16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator = (const __WNFixed32& _fixed32) {
    *this = __WNFloat16(_fixed32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator = (const __WNFixed64& _fixed64) {
    *this = __WNFloat16(_fixed64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator = (const __WNFloat8& _float8) {
    *this = __WNFloat16(_float8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator = (const __WNFloat16& _float16) {
    mBits = _float16.mBits;

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator = (WN_FLOAT32 _float32) {
    *this = __WNFloat16(_float32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator = (WN_FLOAT64 _float64) {
    *this = __WNFloat16(_float64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator + () const {
    return(*this);
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator - () const {
    return(__WNFloat16(-static_cast<WN_FLOAT32>(*this)));
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator ++ () {
    WN_FLOAT32 result = static_cast<WN_FLOAT32>(*this);

    ++result;

    *this = __WNFloat16(result);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator ++ (WN_INT32) {
    const __WNFloat16 result = *this;

    ++(*this);

    return(result);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator -- () {
    WN_FLOAT32 result = static_cast<WN_FLOAT32>(*this);

    ++result;

    *this = __WNFloat16(result);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator -- (WN_INT32) {
    const __WNFloat16 result = *this;

    --(*this);

    return(result);
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator ! () const {
    return(!mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator == (WN_INT8 _integer8) const {
    return(*this == __WNFloat16(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator == (WN_INT16 _integer16) const {
    return(*this == __WNFloat16(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator == (WN_INT32 _integer32) const {
    return(*this == __WNFloat16(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator == (WN_INT64 _integer64) const {
    return(*this == __WNFloat16(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator == (WN_UINT8 _integer8) const {
    return(*this == __WNFloat16(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator == (WN_UINT16 _integer16) const {
    return(*this == __WNFloat16(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator == (WN_UINT32 _integer32) const {
    return(*this == __WNFloat16(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator == (WN_UINT64 _integer64) const {
    return(*this == __WNFloat16(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator == (const __WNFixed8& _fixed8) const {
    return(*this == __WNFloat16(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator == (const __WNFixed16& _fixed16) const {
    return(*this == __WNFloat16(_fixed16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator == (const __WNFixed32& _fixed32) const {
    return(*this == __WNFloat16(_fixed32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator == (const __WNFixed64& _fixed64) const {
    return(*this == __WNFloat16(_fixed64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator == (const __WNFloat8& _float8) const {
    return(*this == __WNFloat16(_float8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator == (const __WNFloat16& _float16) const {
    return(F16ToF32(mBits) == F16ToF32(_float16.mBits));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator == (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) == _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator == (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) == _float64);
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator != (WN_INT8 _integer8) const {
    return(*this != __WNFloat16(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator != (WN_INT16 _integer16) const {
    return(*this != __WNFloat16(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator != (WN_INT32 _integer32) const {
    return(*this != __WNFloat16(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator != (WN_INT64 _integer64) const {
    return(*this != __WNFloat16(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator != (WN_UINT8 _integer8) const {
    return(*this != __WNFloat16(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator != (WN_UINT16 _integer16) const {
    return(*this != __WNFloat16(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator != (WN_UINT32 _integer32) const {
    return(*this != __WNFloat16(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator != (WN_UINT64 _integer64) const {
    return(*this != __WNFloat16(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator != (const __WNFixed8& _fixed8) const {
    return(*this != __WNFloat16(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator != (const __WNFixed16& _fixed16) const {
    return(*this != __WNFloat16(_fixed16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator != (const __WNFixed32& _fixed32) const {
    return(*this != __WNFloat16(_fixed32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator != (const __WNFixed64& _fixed64) const {
    return(*this != __WNFloat16(_fixed64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator != (const __WNFloat8& _float8) const {
    return(*this != __WNFloat16(_float8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator != (const __WNFloat16& _float16) const {
    return(F16ToF32(mBits) != F16ToF32(_float16.mBits));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator != (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) != _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator != (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) != _float64);
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator <= (WN_INT8 _integer8) const {
    return(*this <= __WNFloat16(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator <= (WN_INT16 _integer16) const {
    return(*this <= __WNFloat16(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator <= (WN_INT32 _integer32) const {
    return(*this <= __WNFloat16(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator <= (WN_INT64 _integer64) const {
    return(*this <= __WNFloat16(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator <= (WN_UINT8 _integer8) const {
    return(*this <= __WNFloat16(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator <= (WN_UINT16 _integer16) const {
    return(*this <= __WNFloat16(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator <= (WN_UINT32 _integer32) const {
    return(*this <= __WNFloat16(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator <= (WN_UINT64 _integer64) const {
    return(*this <= __WNFloat16(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator <= (const __WNFixed8& _fixed8) const {
    return(*this <= __WNFloat16(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator <= (const __WNFixed16& _fixed16) const {
    return(*this <= __WNFloat16(_fixed16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator <= (const __WNFixed32& _fixed32) const {
    return(*this <= __WNFloat16(_fixed32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator <= (const __WNFixed64& _fixed64) const {
    return(*this <= __WNFloat16(_fixed64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator <= (const __WNFloat8& _float8) const {
    return(*this <= __WNFloat16(_float8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator <= (const __WNFloat16& _float16) const {
    return(F16ToF32(mBits) <= F16ToF32(_float16.mBits));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator <= (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) <= _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator <= (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) <= _float64);
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator >= (WN_INT8 _integer8) const {
    return(*this >= __WNFloat16(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator >= (WN_INT16 _integer16) const {
    return(*this >= __WNFloat16(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator >= (WN_INT32 _integer32) const {
    return(*this >= __WNFloat16(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator >= (WN_INT64 _integer64) const {
    return(*this >= __WNFloat16(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator >= (WN_UINT8 _integer8) const {
    return(*this >= __WNFloat16(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator >= (WN_UINT16 _integer16) const {
    return(*this >= __WNFloat16(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator >= (WN_UINT32 _integer32) const {
    return(*this >= __WNFloat16(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator >= (WN_UINT64 _integer64) const {
    return(*this >= __WNFloat16(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator >= (const __WNFixed8& _fixed8) const {
    return(*this >= __WNFloat16(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator >= (const __WNFixed16& _fixed16) const {
    return(*this >= __WNFloat16(_fixed16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator >= (const __WNFixed32& _fixed32) const {
    return(*this >= __WNFloat16(_fixed32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator >= (const __WNFixed64& _fixed64) const {
    return(*this >= __WNFloat16(_fixed64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator >= (const __WNFloat8& _float8) const {
    return(*this >= __WNFloat16(_float8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator >= (const __WNFloat16& _float16) const {
    return(F16ToF32(mBits) >= F16ToF32(_float16.mBits));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator >= (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) >= _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator >= (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) >= _float64);
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator < (WN_INT8 _integer8) const {
    return(*this < __WNFloat16(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator < (WN_INT16 _integer16) const {
    return(*this < __WNFloat16(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator < (WN_INT32 _integer32) const {
    return(*this < __WNFloat16(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator < (WN_INT64 _integer64) const {
    return(*this < __WNFloat16(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator < (WN_UINT8 _integer8) const {
    return(*this < __WNFloat16(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator < (WN_UINT16 _integer16) const {
    return(*this < __WNFloat16(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator < (WN_UINT32 _integer32) const {
    return(*this < __WNFloat16(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator < (WN_UINT64 _integer64) const {
    return(*this < __WNFloat16(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator < (const __WNFixed8& _fixed8) const {
    return(*this < __WNFloat16(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator < (const __WNFixed16& _fixed16) const {
    return(*this < __WNFloat16(_fixed16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator < (const __WNFixed32& _fixed32) const {
    return(*this < __WNFloat16(_fixed32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator < (const __WNFixed64& _fixed64) const {
    return(*this < __WNFloat16(_fixed64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator < (const __WNFloat8& _float8) const {
    return(*this < __WNFloat16(_float8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator < (const __WNFloat16& _float16) const {
    return(F16ToF32(mBits) < F16ToF32(_float16.mBits));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator < (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) < _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator < (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) < _float64);
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator > (WN_INT8 _integer8) const {
    return(*this > __WNFloat16(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator > (WN_INT16 _integer16) const {
    return(*this > __WNFloat16(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator > (WN_INT32 _integer32) const {
    return(*this > __WNFloat16(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator > (WN_INT64 _integer64) const {
    return(*this > __WNFloat16(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator > (WN_UINT8 _integer8) const {
    return(*this > __WNFloat16(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator > (WN_UINT16 _integer16) const {
    return(*this > __WNFloat16(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator > (WN_UINT32 _integer32) const {
    return(*this > __WNFloat16(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator > (WN_UINT64 _integer64) const {
    return(*this > __WNFloat16(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator > (const __WNFixed8& _fixed8) const {
    return(*this > __WNFloat16(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator > (const __WNFixed16& _fixed16) const {
    return(*this > __WNFloat16(_fixed16));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator > (const __WNFixed32& _fixed32) const {
    return(*this > __WNFloat16(_fixed32));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator > (const __WNFixed64& _fixed64) const {
    return(*this > __WNFloat16(_fixed64));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator > (const __WNFloat8& _float8) const {
    return(*this > __WNFloat16(_float8));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator > (const __WNFloat16& _float16) const {
    return(F16ToF32(mBits) > F16ToF32(_float16.mBits));
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator > (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) > _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFloat16::operator > (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) > _float64);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator += (WN_INT8 _integer8) {
    *this += __WNFloat16(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator += (WN_INT16 _integer16) {
    *this += __WNFloat16(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator += (WN_INT32 _integer32) {
    *this += __WNFloat16(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator += (WN_INT64 _integer64) {
    *this += __WNFloat16(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator += (WN_UINT8 _integer8) {
    *this += __WNFloat16(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator += (WN_UINT16 _integer16) {
    *this += __WNFloat16(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator += (WN_UINT32 _integer32) {
    *this += __WNFloat16(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator += (WN_UINT64 _integer64) {
    *this += __WNFloat16(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator += (const __WNFixed8& _fixed8) {
    *this += __WNFloat16(_fixed8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator += (const __WNFixed16& _fixed16) {
    *this += __WNFloat16(_fixed16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator += (const __WNFixed32& _fixed32) {
    *this += __WNFloat16(_fixed32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator += (const __WNFixed64& _fixed64) {
    *this += __WNFloat16(_fixed64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator += (const __WNFloat8& _float8) {
    *this += __WNFloat16(_float8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator += (const __WNFloat16& _float16) {
    mBits = F32ToF16(F16ToF32(mBits) + F16ToF32(_float16.mBits));

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator += (WN_FLOAT32 _float32) {
    *this += __WNFloat16(_float32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator += (WN_FLOAT64 _float64) {
    *this += __WNFloat16(_float64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator -= (WN_INT8 _integer8) {
    *this -= __WNFloat16(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator -= (WN_INT16 _integer16) {
    *this -= __WNFloat16(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator -= (WN_INT32 _integer32) {
    *this -= __WNFloat16(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator -= (WN_INT64 _integer64) {
    *this -= __WNFloat16(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator -= (WN_UINT8 _integer8) {
    *this -= __WNFloat16(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator -= (WN_UINT16 _integer16) {
    *this -= __WNFloat16(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator -= (WN_UINT32 _integer32) {
    *this -= __WNFloat16(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator -= (WN_UINT64 _integer64) {
    *this -= __WNFloat16(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator -= (const __WNFixed8& _fixed8) {
    *this -= __WNFloat16(_fixed8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator -= (const __WNFixed16& _fixed16) {
    *this -= __WNFloat16(_fixed16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator -= (const __WNFixed32& _fixed32) {
    *this -= __WNFloat16(_fixed32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator -= (const __WNFixed64& _fixed64) {
    *this -= __WNFloat16(_fixed64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator -= (const __WNFloat8& _float8) {
    *this -= __WNFloat16(_float8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator -= (const __WNFloat16& _float16) {
    mBits = F32ToF16(F16ToF32(mBits) - F16ToF32(_float16.mBits));

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator -= (WN_FLOAT32 _float32) {
    *this -= __WNFloat16(_float32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator -= (WN_FLOAT64 _float64) {
    *this -= __WNFloat16(_float64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator *= (WN_INT8 _integer8) {
    *this *= __WNFloat16(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator *= (WN_INT16 _integer16) {
    *this *= __WNFloat16(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator *= (WN_INT32 _integer32) {
    *this *= __WNFloat16(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator *= (WN_INT64 _integer64) {
    *this *= __WNFloat16(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator *= (WN_UINT8 _integer8) {
    *this *= __WNFloat16(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator *= (WN_UINT16 _integer16) {
    *this *= __WNFloat16(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator *= (WN_UINT32 _integer32) {
    *this *= __WNFloat16(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator *= (WN_UINT64 _integer64) {
    *this *= __WNFloat16(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator *= (const __WNFixed8& _fixed8) {
    *this *= __WNFloat16(_fixed8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator *= (const __WNFixed16& _fixed16) {
    *this *= __WNFloat16(_fixed16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator *= (const __WNFixed32& _fixed32) {
    *this *= __WNFloat16(_fixed32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator *= (const __WNFixed64& _fixed64) {
    *this *= __WNFloat16(_fixed64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator *= (const __WNFloat8& _float8) {
    *this *= __WNFloat16(_float8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator *= (const __WNFloat16& _float16) {
    mBits = F32ToF16(F16ToF32(mBits) * F16ToF32(_float16.mBits));

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator *= (WN_FLOAT32 _float32) {
    *this *= __WNFloat16(_float32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator *= (WN_FLOAT64 _float64) {
    *this *= __WNFloat16(_float64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator /= (WN_INT8 _integer8) {
    *this /= __WNFloat16(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator /= (WN_INT16 _integer16) {
    *this /= __WNFloat16(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator /= (WN_INT32 _integer32) {
    *this /= __WNFloat16(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator /= (WN_INT64 _integer64) {
    *this /= __WNFloat16(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator /= (WN_UINT8 _integer8) {
    *this /= __WNFloat16(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator /= (WN_UINT16 _integer16) {
    *this /= __WNFloat16(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator /= (WN_UINT32 _integer32) {
    *this /= __WNFloat16(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator /= (WN_UINT64 _integer64) {
    *this /= __WNFloat16(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator /= (const __WNFixed8& _fixed8) {
    *this /= __WNFloat16(_fixed8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator /= (const __WNFixed16& _fixed16) {
    *this /= __WNFloat16(_fixed16);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator /= (const __WNFixed32& _fixed32) {
    *this /= __WNFloat16(_fixed32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator /= (const __WNFixed64& _fixed64) {
    *this /= __WNFloat16(_fixed64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator /= (const __WNFloat8& _float8) {
    *this /= __WNFloat16(_float8);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator /= (const __WNFloat16& _float16) {
    mBits = F32ToF16(F16ToF32(mBits) / F16ToF32(_float16.mBits));

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator /= (WN_FLOAT32 _float32) {
    *this /= __WNFloat16(_float32);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16& __WNFloat16::operator /= (WN_FLOAT64 _float64) {
    *this /= __WNFloat16(_float64);

    return(*this);
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator + (WN_INT8 _integer8) const {
    return(*this + __WNFloat16(_integer8));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator + (WN_INT16 _integer16) const {
    return(*this + __WNFloat16(_integer16));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator + (WN_INT32 _integer32) const {
    return(*this + __WNFloat16(_integer32));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator + (WN_INT64 _integer64) const {
    return(*this + __WNFloat16(_integer64));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator + (WN_UINT8 _integer8) const {
    return(*this + __WNFloat16(_integer8));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator + (WN_UINT16 _integer16) const {
    return(*this + __WNFloat16(_integer16));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator + (WN_UINT32 _integer32) const {
    return(*this + __WNFloat16(_integer32));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator + (WN_UINT64 _integer64) const {
    return(*this + __WNFloat16(_integer64));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator + (const __WNFixed8& _fixed8) const {
    return(*this + __WNFloat16(_fixed8));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator + (const __WNFixed16& _fixed16) const {
    return(*this + __WNFloat16(_fixed16));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator + (const __WNFixed32& _fixed32) const {
    return(*this + __WNFloat16(_fixed32));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator + (const __WNFixed64& _fixed64) const {
    return(*this + __WNFloat16(_fixed64));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator + (const __WNFloat8& _float8) const {
    return(*this + __WNFloat16(_float8));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator + (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) += _float16);
}

WN_FORCE_INLINE WN_FLOAT32 __WNFloat16::operator + (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) + _float32);
}

WN_FORCE_INLINE WN_FLOAT64 __WNFloat16::operator + (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) + _float64);
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator - (WN_INT8 _integer8) const {
    return(*this - __WNFloat16(_integer8));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator - (WN_INT16 _integer16) const {
    return(*this - __WNFloat16(_integer16));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator - (WN_INT32 _integer32) const {
    return(*this - __WNFloat16(_integer32));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator - (WN_INT64 _integer64) const {
    return(*this - __WNFloat16(_integer64));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator - (WN_UINT8 _integer8) const {
    return(*this - __WNFloat16(_integer8));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator - (WN_UINT16 _integer16) const {
    return(*this - __WNFloat16(_integer16));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator - (WN_UINT32 _integer32) const {
    return(*this - __WNFloat16(_integer32));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator - (WN_UINT64 _integer64) const {
    return(*this - __WNFloat16(_integer64));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator - (const __WNFixed8& _fixed8) const {
    return(*this - __WNFloat16(_fixed8));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator - (const __WNFixed16& _fixed16) const {
    return(*this - __WNFloat16(_fixed16));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator - (const __WNFixed32& _fixed32) const {
    return(*this - __WNFloat16(_fixed32));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator - (const __WNFixed64& _fixed64) const {
    return(*this - __WNFloat16(_fixed64));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator - (const __WNFloat8& _float8) const {
    return(*this - __WNFloat16(_float8));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator - (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) -= _float16);
}

WN_FORCE_INLINE WN_FLOAT32 __WNFloat16::operator - (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) - _float32);
}

WN_FORCE_INLINE WN_FLOAT64 __WNFloat16::operator - (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) - _float64);
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator * (WN_INT8 _integer8) const {
    return(*this * __WNFloat16(_integer8));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator * (WN_INT16 _integer16) const {
    return(*this * __WNFloat16(_integer16));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator * (WN_INT32 _integer32) const {
    return(*this * __WNFloat16(_integer32));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator * (WN_INT64 _integer64) const {
    return(*this * __WNFloat16(_integer64));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator * (WN_UINT8 _integer8) const {
    return(*this * __WNFloat16(_integer8));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator * (WN_UINT16 _integer16) const {
    return(*this * __WNFloat16(_integer16));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator * (WN_UINT32 _integer32) const {
    return(*this * __WNFloat16(_integer32));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator * (WN_UINT64 _integer64) const {
    return(*this * __WNFloat16(_integer64));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator * (const __WNFixed8& _fixed8) const {
    return(*this * __WNFloat16(_fixed8));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator * (const __WNFixed16& _fixed16) const {
    return(*this * __WNFloat16(_fixed16));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator * (const __WNFixed32& _fixed32) const {
    return(*this * __WNFloat16(_fixed32));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator * (const __WNFixed64& _fixed64) const {
    return(*this * __WNFloat16(_fixed64));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator * (const __WNFloat8& _float8) const {
    return(*this * __WNFloat16(_float8));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator * (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) *= _float16);
}

WN_FORCE_INLINE WN_FLOAT32 __WNFloat16::operator * (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) * _float32);
}

WN_FORCE_INLINE WN_FLOAT64 __WNFloat16::operator * (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) * _float64);
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator / (WN_INT8 _integer8) const {
    return(*this / __WNFloat16(_integer8));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator / (WN_INT16 _integer16) const {
    return(*this / __WNFloat16(_integer16));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator / (WN_INT32 _integer32) const {
    return(*this / __WNFloat16(_integer32));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator / (WN_INT64 _integer64) const {
    return(*this / __WNFloat16(_integer64));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator / (WN_UINT8 _integer8) const {
    return(*this / __WNFloat16(_integer8));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator / (WN_UINT16 _integer16) const {
    return(*this / __WNFloat16(_integer16));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator / (WN_UINT32 _integer32) const {
    return(*this / __WNFloat16(_integer32));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator / (WN_UINT64 _integer64) const {
    return(*this / __WNFloat16(_integer64));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator / (const __WNFixed8& _fixed8) const {
    return(*this / __WNFloat16(_fixed8));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator / (const __WNFixed16& _fixed16) const {
    return(*this / __WNFloat16(_fixed16));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator / (const __WNFixed32& _fixed32) const {
    return(*this / __WNFloat16(_fixed32));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator / (const __WNFixed64& _fixed64) const {
    return(*this / __WNFloat16(_fixed64));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator / (const __WNFloat8& _float8) const {
    return(*this / __WNFloat16(_float8));
}

WN_FORCE_INLINE __WNFloat16 __WNFloat16::operator / (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) /= _float16);
}

WN_FORCE_INLINE WN_FLOAT32 __WNFloat16::operator / (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) / _float32);
}

WN_FORCE_INLINE WN_FLOAT64 __WNFloat16::operator / (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) / _float64);
}

WN_FORCE_INLINE WN_INT16 __WNFloat16::F32ToF16(WN_FLOAT32 _float32) {
    WN_INT32 output = *reinterpret_cast<WN_INT32*>(&_float32);
    const WN_INT32 sign = output & 0x80000000;

    output ^= sign;

    WN_INT32 multiplier = 0x52000000;

    multiplier = static_cast<WN_INT32>(*reinterpret_cast<WN_FLOAT32*>(&multiplier) * *reinterpret_cast<WN_FLOAT32*>(&output));

    output ^= (multiplier ^ output) & -(0x38800000 > output);
    output ^= (0x7F800000 ^ output) & -((0x7F800000 > output) & (output > 0x477FE000));
    output ^= (0x7F802000 ^ output) & -((0x7F802000 > output) & (output > 0x7F800000));
    output >>= 13;
    output ^= ((output - 0x0001C000) ^ output) & -(output > 0x00023BFF);
    output ^= ((output - 0x0001C000) ^ output) & -(output > 0x000003FF);
    output |= (sign >> 16);

    return(static_cast<WN_INT16>(output));
}

WN_FORCE_INLINE WN_FLOAT32 __WNFloat16::F16ToF32(WN_INT16 _float16) {
    WN_INT32 output = static_cast<WN_INT32>(_float16);
    const WN_INT32 sign = output & 0xFFFF8000;

    output ^= sign;
    output ^= ((output + 0x0001C000) ^ output) & -(output > 0x000003FF);
    output ^= ((output + 0x0001C000) ^ output) & -(output > 0x00023BFF);

    WN_INT32 multiplier = 0x33800000;

    *reinterpret_cast<WN_FLOAT32*>(&multiplier) *= output;

    const WN_INT32 mask = -(0x00000400 > output);

    output <<= 13;
    output ^= (multiplier ^ output) & mask;
    output |= (sign << 16);

    return(*reinterpret_cast<WN_FLOAT32*>(&output));
}

WN_FORCE_INLINE WN_BOOL operator == (WN_INT8 _integer8, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer8) == _float16);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_INT16 _integer16, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer16) == _float16);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_INT32 _integer32, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer32) == _float16);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_INT64 _integer64, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer64) == _float16);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_UINT8 _integer8, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer8) == _float16);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_UINT16 _integer16, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer16) == _float16);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_UINT32 _integer32, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer32) == _float16);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_UINT64 _integer64, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer64) == _float16);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_FLOAT32 _float32, const __WNFloat16& _float16) {
    return(_float32 != static_cast<WN_FLOAT32>(_float16));
}

WN_FORCE_INLINE WN_BOOL operator == (WN_FLOAT64 _float64, const __WNFloat16& _float16) {
    return(_float64 != static_cast<WN_FLOAT64>(_float16));
}

WN_FORCE_INLINE WN_BOOL operator != (WN_INT8 _integer8, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer8) != _float16);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_INT16 _integer16, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer16) != _float16);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_INT32 _integer32, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer32) != _float16);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_INT64 _integer64, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer64) != _float16);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_UINT8 _integer8, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer8) != _float16);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_UINT16 _integer16, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer16) != _float16);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_UINT32 _integer32, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer32) != _float16);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_UINT64 _integer64, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer64) != _float16);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_FLOAT32 _float32, const __WNFloat16& _float16) {
    return(_float32 != static_cast<WN_FLOAT32>(_float16));
}

WN_FORCE_INLINE WN_BOOL operator != (WN_FLOAT64 _float64, const __WNFloat16& _float16) {
    return(_float64 != static_cast<WN_FLOAT64>(_float16));
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_INT8 _integer8, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer8) <= _float16);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_INT16 _integer16, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer16) <= _float16);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_INT32 _integer32, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer32) <= _float16);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_INT64 _integer64, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer64) <= _float16);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_UINT8 _integer8, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer8) <= _float16);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_UINT16 _integer16, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer16) <= _float16);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_UINT32 _integer32, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer32) <= _float16);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_UINT64 _integer64, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer64) <= _float16);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_FLOAT32 _float32, const __WNFloat16& _float16) {
    return(_float32 <= static_cast<WN_FLOAT32>(_float16));
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_FLOAT64 _float64, const __WNFloat16& _float16) {
    return(_float64 <= static_cast<WN_FLOAT64>(_float16));
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_INT8 _integer8, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer8) >= _float16);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_INT16 _integer16, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer16) >= _float16);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_INT32 _integer32, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer32) >= _float16);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_INT64 _integer64, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer64) >= _float16);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_UINT8 _integer8, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer8) >= _float16);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_UINT16 _integer16, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer16) >= _float16);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_UINT32 _integer32, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer32) >= _float16);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_UINT64 _integer64, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer64) >= _float16);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_FLOAT32 _float32, const __WNFloat16& _float16) {
    return(_float32 >= static_cast<WN_FLOAT32>(_float16));
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_FLOAT64 _float64, const __WNFloat16& _float16) {
    return(_float64 >= static_cast<WN_FLOAT64>(_float16));
}

WN_FORCE_INLINE WN_BOOL operator < (WN_INT8 _integer8, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer8) < _float16);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_INT16 _integer16, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer16) < _float16);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_INT32 _integer32, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer32) < _float16);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_INT64 _integer64, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer64) < _float16);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_UINT8 _integer8, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer8) < _float16);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_UINT16 _integer16, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer16) < _float16);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_UINT32 _integer32, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer32) < _float16);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_UINT64 _integer64, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer64) < _float16);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_FLOAT32 _float32, const __WNFloat16& _float16) {
    return(_float32 < static_cast<WN_FLOAT32>(_float16));
}

WN_FORCE_INLINE WN_BOOL operator < (WN_FLOAT64 _float64, const __WNFloat16& _float16) {
    return(_float64 < static_cast<WN_FLOAT64>(_float16));
}

WN_FORCE_INLINE WN_BOOL operator > (WN_INT8 _integer8, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer8) > _float16);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_INT16 _integer16, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer16) > _float16);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_INT32 _integer32, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer32) > _float16);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_INT64 _integer64, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer64) > _float16);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_UINT8 _integer8, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer8) > _float16);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_UINT16 _integer16, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer16) > _float16);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_UINT32 _integer32, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer32) > _float16);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_UINT64 _integer64, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer64) > _float16);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_FLOAT32 _float32, const __WNFloat16& _float16) {
    return(_float32 > static_cast<WN_FLOAT32>(_float16));
}

WN_FORCE_INLINE WN_BOOL operator > (WN_FLOAT64 _float64, const __WNFloat16& _float16) {
    return(_float64 > static_cast<WN_FLOAT64>(_float16));
}

WN_FORCE_INLINE WN_INT8& operator += (WN_INT8& _integer8, const __WNFloat16& _float16) {
    _integer8 += static_cast<WN_INT8>(_float16);

    return(_integer8);
}

WN_FORCE_INLINE WN_INT16& operator += (WN_INT16& _integer16, const __WNFloat16& _float16) {
    _integer16 += static_cast<WN_INT16>(_float16);

    return(_integer16);
}

WN_FORCE_INLINE WN_INT32& operator += (WN_INT32& _integer32, const __WNFloat16& _float16) {
    _integer32 += static_cast<WN_INT32>(_float16);

    return(_integer32);
}

WN_FORCE_INLINE WN_INT64& operator += (WN_INT64& _integer64, const __WNFloat16& _float16) {
    _integer64 += static_cast<WN_INT64>(_float16);

    return(_integer64);
}

WN_FORCE_INLINE WN_UINT8& operator += (WN_UINT8& _integer8, const __WNFloat16& _float16) {
    _integer8 += static_cast<WN_UINT8>(_float16);

    return(_integer8);
}

WN_FORCE_INLINE WN_UINT16& operator += (WN_UINT16& _integer16, const __WNFloat16& _float16) {
    _integer16 += static_cast<WN_UINT16>(_float16);

    return(_integer16);
}

WN_FORCE_INLINE WN_UINT32& operator += (WN_UINT32& _integer32, const __WNFloat16& _float16) {
    _integer32 += static_cast<WN_UINT32>(_float16);

    return(_integer32);
}

WN_FORCE_INLINE WN_UINT64& operator += (WN_UINT64& _integer64, const __WNFloat16& _float16) {
    _integer64 += static_cast<WN_UINT64>(_float16);

    return(_integer64);
}

WN_FORCE_INLINE WN_FLOAT32& operator += (WN_FLOAT32& _float32, const __WNFloat16& _float16) {
    _float32 += static_cast<WN_FLOAT32>(_float16);

    return(_float32);
}

WN_FORCE_INLINE WN_FLOAT64& operator += (WN_FLOAT64& _float64, const __WNFloat16& _float16) {
    _float64 += static_cast<WN_FLOAT64>(_float16);

    return(_float64);
}

WN_FORCE_INLINE WN_INT8& operator -= (WN_INT8& _integer8, const __WNFloat16& _float16) {
    _integer8 -= static_cast<WN_INT8>(_float16);

    return(_integer8);
}

WN_FORCE_INLINE WN_INT16& operator -= (WN_INT16& _integer16, const __WNFloat16& _float16) {
    _integer16 -= static_cast<WN_INT16>(_float16);

    return(_integer16);
}

WN_FORCE_INLINE WN_INT32& operator -= (WN_INT32& _integer32, const __WNFloat16& _float16) {
    _integer32 -= static_cast<WN_INT32>(_float16);

    return(_integer32);
}

WN_FORCE_INLINE WN_INT64& operator -= (WN_INT64& _integer64, const __WNFloat16& _float16) {
    _integer64 -= static_cast<WN_INT64>(_float16);

    return(_integer64);
}

WN_FORCE_INLINE WN_UINT8& operator -= (WN_UINT8& _integer8, const __WNFloat16& _float16) {
    _integer8 -= static_cast<WN_UINT8>(_float16);

    return(_integer8);
}

WN_FORCE_INLINE WN_UINT16& operator -= (WN_UINT16& _integer16, const __WNFloat16& _float16) {
    _integer16 -= static_cast<WN_UINT16>(_float16);

    return(_integer16);
}

WN_FORCE_INLINE WN_UINT32& operator -= (WN_UINT32& _integer32, const __WNFloat16& _float16) {
    _integer32 -= static_cast<WN_UINT32>(_float16);

    return(_integer32);
}

WN_FORCE_INLINE WN_UINT64& operator -= (WN_UINT64& _integer64, const __WNFloat16& _float16) {
    _integer64 -= static_cast<WN_UINT64>(_float16);

    return(_integer64);
}

WN_FORCE_INLINE WN_FLOAT32& operator -= (WN_FLOAT32& _float32, const __WNFloat16& _float16) {
    _float32 -= static_cast<WN_FLOAT32>(_float16);

    return(_float32);
}

WN_FORCE_INLINE WN_FLOAT64& operator -= (WN_FLOAT64& _float64, const __WNFloat16& _float16) {
    _float64 -= static_cast<WN_FLOAT64>(_float16);

    return(_float64);
}

WN_FORCE_INLINE WN_INT8& operator *= (WN_INT8& _integer8, const __WNFloat16& _float16) {
    _integer8 *= static_cast<WN_INT8>(_float16);

    return(_integer8);
}

WN_FORCE_INLINE WN_INT16& operator *= (WN_INT16& _integer16, const __WNFloat16& _float16) {
    _integer16 *= static_cast<WN_INT16>(_float16);

    return(_integer16);
}

WN_FORCE_INLINE WN_INT32& operator *= (WN_INT32& _integer32, const __WNFloat16& _float16) {
    _integer32 *= static_cast<WN_INT32>(_float16);

    return(_integer32);
}

WN_FORCE_INLINE WN_INT64& operator *= (WN_INT64& _integer64, const __WNFloat16& _float16) {
    _integer64 *= static_cast<WN_INT64>(_float16);

    return(_integer64);
}

WN_FORCE_INLINE WN_UINT8& operator *= (WN_UINT8& _integer8, const __WNFloat16& _float16) {
    _integer8 *= static_cast<WN_UINT8>(_float16);

    return(_integer8);
}

WN_FORCE_INLINE WN_UINT16& operator *= (WN_UINT16& _integer16, const __WNFloat16& _float16) {
    _integer16 *= static_cast<WN_UINT16>(_float16);

    return(_integer16);
}

WN_FORCE_INLINE WN_UINT32& operator *= (WN_UINT32& _integer32, const __WNFloat16& _float16) {
    _integer32 *= static_cast<WN_UINT32>(_float16);

    return(_integer32);
}

WN_FORCE_INLINE WN_UINT64& operator *= (WN_UINT64& _integer64, const __WNFloat16& _float16) {
    _integer64 *= static_cast<WN_UINT64>(_float16);

    return(_integer64);
}

WN_FORCE_INLINE WN_FLOAT32& operator *= (WN_FLOAT32& _float32, const __WNFloat16& _float16) {
    _float32 *= static_cast<WN_FLOAT32>(_float16);

    return(_float32);
}

WN_FORCE_INLINE WN_FLOAT64& operator *= (WN_FLOAT64& _float64, const __WNFloat16& _float16) {
    _float64 *= static_cast<WN_FLOAT64>(_float16);

    return(_float64);
}

WN_FORCE_INLINE WN_INT8& operator /= (WN_INT8& _integer8, const __WNFloat16& _float16) {
    _integer8 /= static_cast<WN_INT8>(_float16);

    return(_integer8);
}

WN_FORCE_INLINE WN_INT16& operator /= (WN_INT16& _integer16, const __WNFloat16& _float16) {
    _integer16 /= static_cast<WN_INT16>(_float16);

    return(_integer16);
}

WN_FORCE_INLINE WN_INT32& operator /= (WN_INT32& _integer32, const __WNFloat16& _float16) {
    _integer32 /= static_cast<WN_INT32>(_float16);

    return(_integer32);
}

WN_FORCE_INLINE WN_INT64& operator /= (WN_INT64& _integer64, const __WNFloat16& _float16) {
    _integer64 /= static_cast<WN_INT64>(_float16);

    return(_integer64);
}

WN_FORCE_INLINE WN_UINT8& operator /= (WN_UINT8& _integer8, const __WNFloat16& _float16) {
    _integer8 /= static_cast<WN_UINT8>(_float16);

    return(_integer8);
}

WN_FORCE_INLINE WN_UINT16& operator /= (WN_UINT16& _integer16, const __WNFloat16& _float16) {
    _integer16 /= static_cast<WN_UINT16>(_float16);

    return(_integer16);
}

WN_FORCE_INLINE WN_UINT32& operator /= (WN_UINT32& _integer32, const __WNFloat16& _float16) {
    _integer32 /= static_cast<WN_UINT32>(_float16);

    return(_integer32);
}

WN_FORCE_INLINE WN_UINT64& operator /= (WN_UINT64& _integer64, const __WNFloat16& _float16) {
    _integer64 /= static_cast<WN_UINT64>(_float16);

    return(_integer64);
}

WN_FORCE_INLINE WN_FLOAT32& operator /= (WN_FLOAT32& _float32, const __WNFloat16& _float16) {
    _float32 /= static_cast<WN_FLOAT32>(_float16);

    return(_float32);
}

WN_FORCE_INLINE WN_FLOAT64& operator /= (WN_FLOAT64& _float64, const __WNFloat16& _float16) {
    _float64 /= static_cast<WN_FLOAT64>(_float16);

    return(_float64);
}

WN_FORCE_INLINE __WNFloat16 operator + (WN_INT8 _integer8, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer8) + _float16);
}

WN_FORCE_INLINE __WNFloat16 operator + (WN_INT16 _integer16, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer16) + _float16);
}

WN_FORCE_INLINE __WNFloat16 operator + (WN_INT32 _integer32, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer32) + _float16);
}

WN_FORCE_INLINE __WNFloat16 operator + (WN_INT64 _integer64, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer64) + _float16);
}

WN_FORCE_INLINE __WNFloat16 operator + (WN_UINT8 _integer8, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer8) + _float16);
}

WN_FORCE_INLINE __WNFloat16 operator + (WN_UINT16 _integer16, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer16) + _float16);
}

WN_FORCE_INLINE __WNFloat16 operator + (WN_UINT32 _integer32, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer32) + _float16);
}

WN_FORCE_INLINE __WNFloat16 operator + (WN_UINT64 _integer64, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer64) + _float16);
}

WN_FORCE_INLINE WN_FLOAT32 operator + (WN_FLOAT32 _float32, const __WNFloat16& _float16) {
    return(_float32 + static_cast<WN_FLOAT32>(_float16));
}

WN_FORCE_INLINE WN_FLOAT64 operator + (WN_FLOAT64 _float64, const __WNFloat16& _float16) {
    return(_float64 + static_cast<WN_FLOAT64>(_float16));
}

WN_FORCE_INLINE __WNFloat16 operator - (WN_INT8 _integer8, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer8) - _float16);
}

WN_FORCE_INLINE __WNFloat16 operator - (WN_INT16 _integer16, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer16) - _float16);
}

WN_FORCE_INLINE __WNFloat16 operator - (WN_INT32 _integer32, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer32) - _float16);
}

WN_FORCE_INLINE __WNFloat16 operator - (WN_INT64 _integer64, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer64) - _float16);
}

WN_FORCE_INLINE __WNFloat16 operator - (WN_UINT8 _integer8, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer8) - _float16);
}

WN_FORCE_INLINE __WNFloat16 operator - (WN_UINT16 _integer16, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer16) - _float16);
}

WN_FORCE_INLINE __WNFloat16 operator - (WN_UINT32 _integer32, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer32) - _float16);
}

WN_FORCE_INLINE __WNFloat16 operator - (WN_UINT64 _integer64, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer64) - _float16);
}

WN_FORCE_INLINE WN_FLOAT32 operator - (WN_FLOAT32 _float32, const __WNFloat16& _float16) {
    return(_float32 - static_cast<WN_FLOAT32>(_float16));
}

WN_FORCE_INLINE WN_FLOAT64 operator - (WN_FLOAT64 _float64, const __WNFloat16& _float16) {
    return(_float64 - static_cast<WN_FLOAT64>(_float16));
}

WN_FORCE_INLINE __WNFloat16 operator * (WN_INT8 _integer8, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer8) * _float16);
}

WN_FORCE_INLINE __WNFloat16 operator * (WN_INT16 _integer16, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer16) * _float16);
}

WN_FORCE_INLINE __WNFloat16 operator * (WN_INT32 _integer32, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer32) * _float16);
}

WN_FORCE_INLINE __WNFloat16 operator * (WN_INT64 _integer64, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer64) * _float16);
}

WN_FORCE_INLINE __WNFloat16 operator * (WN_UINT8 _integer8, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer8) * _float16);
}

WN_FORCE_INLINE __WNFloat16 operator * (WN_UINT16 _integer16, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer16) * _float16);
}

WN_FORCE_INLINE __WNFloat16 operator * (WN_UINT32 _integer32, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer32) * _float16);
}

WN_FORCE_INLINE __WNFloat16 operator * (WN_UINT64 _integer64, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer64) * _float16);
}

WN_FORCE_INLINE WN_FLOAT32 operator * (WN_FLOAT32 _float32, const __WNFloat16& _float16) {
    return(_float32 * static_cast<WN_FLOAT32>(_float16));
}

WN_FORCE_INLINE WN_FLOAT64 operator * (WN_FLOAT64 _float64, const __WNFloat16& _float16) {
    return(_float64 * static_cast<WN_FLOAT64>(_float16));
}

WN_FORCE_INLINE __WNFloat16 operator / (WN_INT8 _integer8, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer8) / _float16);
}

WN_FORCE_INLINE __WNFloat16 operator / (WN_INT16 _integer16, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer16) / _float16);
}

WN_FORCE_INLINE __WNFloat16 operator / (WN_INT32 _integer32, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer32) / _float16);
}

WN_FORCE_INLINE __WNFloat16 operator / (WN_INT64 _integer64, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer64) / _float16);
}

WN_FORCE_INLINE __WNFloat16 operator / (WN_UINT8 _integer8, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer8) / _float16);
}

WN_FORCE_INLINE __WNFloat16 operator / (WN_UINT16 _integer16, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer16) / _float16);
}

WN_FORCE_INLINE __WNFloat16 operator / (WN_UINT32 _integer32, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer32) / _float16);
}

WN_FORCE_INLINE __WNFloat16 operator / (WN_UINT64 _integer64, const __WNFloat16& _float16) {
    return(__WNFloat16(_integer64) / _float16);
}

WN_FORCE_INLINE WN_FLOAT32 operator / (WN_FLOAT32 _float32, const __WNFloat16& _float16) {
    return(_float32 / static_cast<WN_FLOAT32>(_float16));
}

WN_FORCE_INLINE WN_FLOAT64 operator / (WN_FLOAT64 _float64, const __WNFloat16& _float16) {
    return(_float64 / static_cast<WN_FLOAT64>(_float16));
}

// Fixed (8 bit) ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

WN_FORCE_INLINE __WNFixed8::__WNFixed8(WN_INT8 _integer8) {
    mBits = _integer8 << WN_FIXED8_BITS;
}

WN_FORCE_INLINE __WNFixed8::__WNFixed8(WN_INT16 _integer16) {
    mBits = static_cast<WN_INT8>(_integer16) << WN_FIXED8_BITS;
}

WN_FORCE_INLINE __WNFixed8::__WNFixed8(WN_INT32 _integer32) {
    mBits = static_cast<WN_INT8>(_integer32) << WN_FIXED8_BITS;
}

WN_FORCE_INLINE __WNFixed8::__WNFixed8(WN_INT64 _integer64) {
    mBits = static_cast<WN_INT8>(_integer64) << WN_FIXED8_BITS;
}

WN_FORCE_INLINE __WNFixed8::__WNFixed8(WN_UINT8 _integer8) {
    mBits = static_cast<WN_INT8>(_integer8) << WN_FIXED8_BITS;
}

WN_FORCE_INLINE __WNFixed8::__WNFixed8(WN_UINT16 _integer16) {
    mBits = static_cast<WN_INT8>(_integer16) << WN_FIXED8_BITS;
}

WN_FORCE_INLINE __WNFixed8::__WNFixed8(WN_UINT32 _integer32) {
    mBits = static_cast<WN_INT8>(_integer32) << WN_FIXED8_BITS;
}

WN_FORCE_INLINE __WNFixed8::__WNFixed8(WN_UINT64 _integer64) {
    mBits = static_cast<WN_INT8>(_integer64) << WN_FIXED8_BITS;
}

WN_FORCE_INLINE __WNFixed8::__WNFixed8(const __WNFixed8& _fixed8) {
    mBits = _fixed8.mBits;
}

WN_FORCE_INLINE __WNFixed8::__WNFixed8(const __WNFixed16& _fixed16) {
    mBits = static_cast<WN_INT8>(_fixed16.mBits >> (WN_FIXED16_BITS - WN_FIXED8_BITS));
}

WN_FORCE_INLINE __WNFixed8::__WNFixed8(const __WNFixed32& _fixed32) {
    mBits = static_cast<WN_INT8>(_fixed32.mBits >> (WN_FIXED32_BITS - WN_FIXED8_BITS));
}

WN_FORCE_INLINE __WNFixed8::__WNFixed8(const __WNFixed64& _fixed64) {
    mBits = static_cast<WN_INT8>(_fixed64.mBits >> (WN_FIXED64_BITS - WN_FIXED8_BITS));
}

WN_FORCE_INLINE __WNFixed8::__WNFixed8(const __WNFloat8& _float8) {
    const WN_FLOAT32 float32 = static_cast<WN_FLOAT32>(_float8);

    mBits = static_cast<WN_INT8>((float32 * __WN_FIXED8_ONE) + (float32 >= 0.0f ? 0.5f : -0.5f));
}

WN_FORCE_INLINE __WNFixed8::__WNFixed8(const __WNFloat16& _float16) {
    const WN_FLOAT32 float32 = static_cast<WN_FLOAT32>(_float16);

    mBits = static_cast<WN_INT8>((float32 * __WN_FIXED8_ONE) + (float32 >= 0.0f ? 0.5f : -0.5f));
}

WN_FORCE_INLINE __WNFixed8::__WNFixed8(WN_FLOAT32 _float32) {
    mBits = static_cast<WN_INT8>((_float32 * __WN_FIXED8_ONE) + (_float32 >= 0.0f ? 0.5f : -0.5f));
}

WN_FORCE_INLINE __WNFixed8::__WNFixed8(WN_FLOAT64 _float64) {
    mBits = static_cast<WN_INT8>((_float64 * __WN_FIXED8_ONE) + (_float64 >= 0.0 ? 0.5 : -0.5));
}

WN_FORCE_INLINE __WNFixed8::operator WN_INT8 () const {
    return(mBits >> WN_FIXED8_BITS);
}

WN_FORCE_INLINE __WNFixed8::operator WN_INT16 () const {
    return(static_cast<WN_INT16>(static_cast<WN_INT8>(*this)));
}

WN_FORCE_INLINE __WNFixed8::operator WN_INT32 () const {
    return(static_cast<WN_INT32>(static_cast<WN_INT8>(*this)));
}

WN_FORCE_INLINE __WNFixed8::operator WN_INT64 () const {
    return(static_cast<WN_INT64>(static_cast<WN_INT8>(*this)));
}

WN_FORCE_INLINE __WNFixed8::operator WN_UINT8 () const {
    return(static_cast<WN_UINT8>(static_cast<WN_INT8>(*this)));
}

WN_FORCE_INLINE __WNFixed8::operator WN_UINT16 () const {
    return(static_cast<WN_UINT16>(static_cast<WN_INT8>(*this)));
}

WN_FORCE_INLINE __WNFixed8::operator WN_UINT32 () const {
    return(static_cast<WN_UINT32>(static_cast<WN_INT8>(*this)));
}

WN_FORCE_INLINE __WNFixed8::operator WN_UINT64 () const {
    return(static_cast<WN_UINT64>(static_cast<WN_INT8>(*this)));
}

WN_FORCE_INLINE __WNFixed8::operator WN_FLOAT32 () const {
    return(static_cast<WN_FLOAT32>(mBits) / __WN_FIXED8_ONE);
}

WN_FORCE_INLINE __WNFixed8::operator WN_FLOAT64 () const {
    return(static_cast<WN_FLOAT64>(static_cast<WN_FLOAT32>(*this)));
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator = (WN_INT8 _integer8) {
    *this = __WNFixed8(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator = (WN_INT16 _integer16) {
    *this = __WNFixed8(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator = (WN_INT32 _integer32) {
    *this = __WNFixed8(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator = (WN_INT64 _integer64) {
    *this = __WNFixed8(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator = (WN_UINT8 _integer8) {
    *this = __WNFixed8(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator = (WN_UINT16 _integer16) {
    *this = __WNFixed8(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator = (WN_UINT32 _integer32) {
    *this = __WNFixed8(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator = (WN_UINT64 _integer64) {
    *this = __WNFixed8(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator = (const __WNFixed8& _fixed8) {
    mBits = _fixed8.mBits;

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator = (const __WNFixed16& _fixed16) {
    *this = __WNFixed8(_fixed16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator = (const __WNFixed32& _fixed32) {
    *this = __WNFixed8(_fixed32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator = (const __WNFixed64& _fixed64) {
    *this = __WNFixed8(_fixed64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator = (const __WNFloat8& _float8) {
    *this = __WNFixed8(_float8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator = (const __WNFloat16& _float16) {
    *this = __WNFixed8(_float16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator = (WN_FLOAT32 _float32) {
    *this = __WNFixed8(_float32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator = (WN_FLOAT64 _float64) {
    *this = __WNFixed8(_float64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator + () const {
    return(*this);
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator - () const {
    __WNFixed8 result;

    result.mBits = -mBits;

    return(result);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator ++ () {
    mBits += __WN_FIXED8_ONE;

    return(*this);
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator ++ (WN_INT32) {
    const __WNFixed8 result = *this;

    ++(*this);

    return(result);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator -- () {
    mBits -= __WN_FIXED8_ONE;

    return(*this);
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator -- (WN_INT32) {
    const __WNFixed8 result = *this;

    --(*this);

    return(result);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator ! () const {
    return(!mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator == (WN_INT8 _integer8) const {
    return(*this == __WNFixed8(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator == (WN_INT16 _integer16) const {
    return(*this == __WNFixed8(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator == (WN_INT32 _integer32) const {
    return(*this == __WNFixed8(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator == (WN_INT64 _integer64) const {
    return(*this == __WNFixed8(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator == (WN_UINT8 _integer8) const {
    return(*this == __WNFixed8(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator == (WN_UINT16 _integer16) const {
    return(*this == __WNFixed8(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator == (WN_UINT32 _integer32) const {
    return(*this == __WNFixed8(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator == (WN_UINT64 _integer64) const {
    return(*this == __WNFixed8(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator == (const __WNFixed8& _fixed8) const {
    return(mBits == _fixed8.mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator == (const __WNFixed16& _fixed16) const {
    return(__WNFixed16(*this) == _fixed16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator == (const __WNFixed32& _fixed32) const {
    return(__WNFixed32(*this) == _fixed32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator == (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) == _fixed64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator == (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) == _float8);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator == (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) == _float16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator == (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) == _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator == (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) == _float64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator != (WN_INT8 _integer8) const {
    return(*this != __WNFixed8(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator != (WN_INT16 _integer16) const {
    return(*this != __WNFixed8(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator != (WN_INT32 _integer32) const {
    return(*this != __WNFixed8(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator != (WN_INT64 _integer64) const {
    return(*this != __WNFixed8(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator != (WN_UINT8 _integer8) const {
    return(*this != __WNFixed8(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator != (WN_UINT16 _integer16) const {
    return(*this != __WNFixed8(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator != (WN_UINT32 _integer32) const {
    return(*this != __WNFixed8(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator != (WN_UINT64 _integer64) const {
    return(*this != __WNFixed8(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator != (const __WNFixed8& _fixed8) const {
    return(mBits != _fixed8.mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator != (const __WNFixed16& _fixed16) const {
    return(__WNFixed16(*this) != _fixed16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator != (const __WNFixed32& _fixed32) const {
    return(__WNFixed32(*this) != _fixed32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator != (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) != _fixed64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator != (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) != _float8);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator != (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) != _float16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator != (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) != _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator != (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) != _float64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator <= (WN_INT8 _integer8) const {
    return(*this <= __WNFixed8(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator <= (WN_INT16 _integer16) const {
    return(*this <= __WNFixed8(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator <= (WN_INT32 _integer32) const {
    return(*this <= __WNFixed8(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator <= (WN_INT64 _integer64) const {
    return(*this <= __WNFixed8(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator <= (WN_UINT8 _integer8) const {
    return(*this <= __WNFixed8(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator <= (WN_UINT16 _integer16) const {
    return(*this <= __WNFixed8(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator <= (WN_UINT32 _integer32) const {
    return(*this <= __WNFixed8(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator <= (WN_UINT64 _integer64) const {
    return(*this <= __WNFixed8(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator <= (const __WNFixed8& _fixed8) const {
    return(mBits <= _fixed8.mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator <= (const __WNFixed16& _fixed16) const {
    return(__WNFixed16(*this) <= _fixed16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator <= (const __WNFixed32& _fixed32) const {
    return(__WNFixed32(*this) <= _fixed32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator <= (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) <= _fixed64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator <= (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) <= _float8);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator <= (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) <= _float16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator <= (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) <= _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator <= (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) <= _float64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator >= (WN_INT8 _integer8) const {
    return(*this >= __WNFixed8(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator >= (WN_INT16 _integer16) const {
    return(*this >= __WNFixed8(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator >= (WN_INT32 _integer32) const {
    return(*this >= __WNFixed8(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator >= (WN_INT64 _integer64) const {
    return(*this >= __WNFixed8(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator >= (WN_UINT8 _integer8) const {
    return(*this >= __WNFixed8(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator >= (WN_UINT16 _integer16) const {
    return(*this >= __WNFixed8(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator >= (WN_UINT32 _integer32) const {
    return(*this >= __WNFixed8(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator >= (WN_UINT64 _integer64) const {
    return(*this >= __WNFixed8(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator >= (const __WNFixed8& _fixed8) const {
    return(mBits >= _fixed8.mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator >= (const __WNFixed16& _fixed16) const {
    return(__WNFixed16(*this) >= _fixed16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator >= (const __WNFixed32& _fixed32) const {
    return(__WNFixed32(*this) >= _fixed32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator >= (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) >= _fixed64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator >= (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) >= _float8);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator >= (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) >= _float16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator >= (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) >= _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator >= (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) >= _float64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator < (WN_INT8 _integer8) const {
    return(*this < __WNFixed8(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator < (WN_INT16 _integer16) const {
    return(*this < __WNFixed8(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator < (WN_INT32 _integer32) const {
    return(*this < __WNFixed8(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator < (WN_INT64 _integer64) const {
    return(*this < __WNFixed8(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator < (WN_UINT8 _integer8) const {
    return(*this < __WNFixed8(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator < (WN_UINT16 _integer16) const {
    return(*this < __WNFixed8(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator < (WN_UINT32 _integer32) const {
    return(*this < __WNFixed8(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator < (WN_UINT64 _integer64) const {
    return(*this < __WNFixed8(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator < (const __WNFixed8& _fixed8) const {
    return(mBits < _fixed8.mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator < (const __WNFixed16& _fixed16) const {
    return(__WNFixed16(*this) < _fixed16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator < (const __WNFixed32& _fixed32) const {
    return(__WNFixed32(*this) < _fixed32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator < (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) < _fixed64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator < (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) < _float8);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator < (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) < _float16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator < (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) < _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator < (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) < _float64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator > (WN_INT8 _integer8) const {
    return(*this > __WNFixed8(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator > (WN_INT16 _integer16) const {
    return(*this > __WNFixed8(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator > (WN_INT32 _integer32) const {
    return(*this > __WNFixed8(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator > (WN_INT64 _integer64) const {
    return(*this > __WNFixed8(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator > (WN_UINT8 _integer8) const {
    return(*this > __WNFixed8(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator > (WN_UINT16 _integer16) const {
    return(*this > __WNFixed8(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator > (WN_UINT32 _integer32) const {
    return(*this > __WNFixed8(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator > (WN_UINT64 _integer64) const {
    return(*this > __WNFixed8(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator > (const __WNFixed8& _fixed8) const {
    return(mBits > _fixed8.mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator > (const __WNFixed16& _fixed16) const {
    return(__WNFixed16(*this) > _fixed16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator > (const __WNFixed32& _fixed32) const {
    return(__WNFixed32(*this) > _fixed32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator > (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) > _fixed64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator > (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) > _float8);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator > (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) > _float16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator > (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) > _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed8::operator > (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) > _float64);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator += (WN_INT8 _integer8) {
    *this += __WNFixed8(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator += (WN_INT16 _integer16) {
    *this += __WNFixed8(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator += (WN_INT32 _integer32) {
    *this += __WNFixed8(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator += (WN_INT64 _integer64) {
    *this += __WNFixed8(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator += (WN_UINT8 _integer8) {
    *this += __WNFixed8(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator += (WN_UINT16 _integer16) {
    *this += __WNFixed8(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator += (WN_UINT32 _integer32) {
    *this += __WNFixed8(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator += (WN_UINT64 _integer64) {
    *this += __WNFixed8(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator += (const __WNFixed8& _fixed8) {
    mBits += _fixed8.mBits;

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator += (const __WNFixed16& _fixed16) {
    *this = __WNFixed8(__WNFixed16(*this) + _fixed16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator += (const __WNFixed32& _fixed32) {
    *this = __WNFixed8(__WNFixed32(*this) + _fixed32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator += (const __WNFixed64& _fixed64) {
    *this = __WNFixed8(__WNFixed64(*this) + _fixed64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator += (const __WNFloat8& _float8) {
    *this = __WNFixed8(__WNFloat8(*this) + _float8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator += (const __WNFloat16& _float16) {
    *this = __WNFixed8(__WNFloat16(*this) + _float16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator += (WN_FLOAT32 _float32) {
    *this = __WNFixed8(static_cast<WN_FLOAT32>(*this) + _float32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator += (WN_FLOAT64 _float64) {
    *this = __WNFixed8(static_cast<WN_FLOAT64>(*this) + _float64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator -= (WN_INT8 _integer8) {
    *this -= __WNFixed8(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator -= (WN_INT16 _integer16) {
    *this -= __WNFixed8(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator -= (WN_INT32 _integer32) {
    *this -= __WNFixed8(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator -= (WN_INT64 _integer64) {
    *this -= __WNFixed8(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator -= (WN_UINT8 _integer8) {
    *this -= __WNFixed8(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator -= (WN_UINT16 _integer16) {
    *this -= __WNFixed8(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator -= (WN_UINT32 _integer32) {
    *this -= __WNFixed8(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator -= (WN_UINT64 _integer64) {
    *this -= __WNFixed8(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator -= (const __WNFixed8& _fixed8) {
    mBits -= _fixed8.mBits;

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator -= (const __WNFixed16& _fixed16) {
    *this = __WNFixed8(__WNFixed16(*this) - _fixed16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator -= (const __WNFixed32& _fixed32) {
    *this = __WNFixed8(__WNFixed32(*this) - _fixed32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator -= (const __WNFixed64& _fixed64) {
    *this = __WNFixed8(__WNFixed64(*this) - _fixed64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator -= (const __WNFloat8& _float8) {
    *this = __WNFixed8(__WNFloat8(*this) - _float8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator -= (const __WNFloat16& _float16) {
    *this = __WNFixed8(__WNFloat16(*this) - _float16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator -= (WN_FLOAT32 _float32) {
    *this = __WNFixed8(static_cast<WN_FLOAT32>(*this) - _float32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator -= (WN_FLOAT64 _float64) {
    *this = __WNFixed8(static_cast<WN_FLOAT64>(*this) - _float64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator *= (WN_INT8 _integer8) {
    *this *= __WNFixed8(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator *= (WN_INT16 _integer16) {
    *this *= __WNFixed8(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator *= (WN_INT32 _integer32) {
    *this *= __WNFixed8(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator *= (WN_INT64 _integer64) {
    *this *= __WNFixed8(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator *= (WN_UINT8 _integer8) {
    *this *= __WNFixed8(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator *= (WN_UINT16 _integer16) {
    *this *= __WNFixed8(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator *= (WN_UINT32 _integer32) {
    *this *= __WNFixed8(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator *= (WN_UINT64 _integer64) {
    *this *= __WNFixed8(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator *= (const __WNFixed8& _fixed8) {
    mBits = static_cast<WN_INT8>((static_cast<WN_INT16>(mBits) * static_cast<WN_INT16>(_fixed8.mBits)) >> WN_FIXED8_BITS);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator *= (const __WNFixed16& _fixed16) {
    *this = __WNFixed8(__WNFixed16(*this) * _fixed16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator *= (const __WNFixed32& _fixed32) {
    *this = __WNFixed8(__WNFixed32(*this) * _fixed32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator *= (const __WNFixed64& _fixed64) {
    *this = __WNFixed8(__WNFixed64(*this) * _fixed64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator *= (const __WNFloat8& _float8) {
    *this = __WNFixed8(__WNFloat8(*this) * _float8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator *= (const __WNFloat16& _float16) {
    *this = __WNFixed8(__WNFloat16(*this) * _float16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator *= (WN_FLOAT32 _float32) {
    *this = __WNFixed8(static_cast<WN_FLOAT32>(*this) * _float32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator *= (WN_FLOAT64 _float64) {
    *this = __WNFixed8(static_cast<WN_FLOAT64>(*this) * _float64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator /= (WN_INT8 _integer8) {
    *this /= __WNFixed8(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator /= (WN_INT16 _integer16) {
    *this /= __WNFixed8(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator /= (WN_INT32 _integer32) {
    *this /= __WNFixed8(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator /= (WN_INT64 _integer64) {
    *this /= __WNFixed8(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator /= (WN_UINT8 _integer8) {
    *this /= __WNFixed8(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator /= (WN_UINT16 _integer16) {
    *this /= __WNFixed8(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator /= (WN_UINT32 _integer32) {
    *this /= __WNFixed8(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator /= (WN_UINT64 _integer64) {
    *this /= __WNFixed8(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator /= (const __WNFixed8& _fixed8) {
    mBits = static_cast<WN_INT8>((static_cast<WN_INT16>(mBits) << WN_FIXED8_BITS) / _fixed8.mBits);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator /= (const __WNFixed16& _fixed16) {
    *this = __WNFixed8(__WNFixed16(*this) / _fixed16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator /= (const __WNFixed32& _fixed32) {
    *this = __WNFixed8(__WNFixed32(*this) / _fixed32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator /= (const __WNFixed64& _fixed64) {
    *this = __WNFixed8(__WNFixed64(*this) / _fixed64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator /= (const __WNFloat8& _float8) {
    *this = __WNFixed8(__WNFloat8(*this) / _float8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator /= (const __WNFloat16& _float16) {
    *this = __WNFixed8(__WNFloat16(*this) / _float16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator /= (WN_FLOAT32 _float32) {
    *this = __WNFixed8(static_cast<WN_FLOAT32>(*this) / _float32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8& __WNFixed8::operator /= (WN_FLOAT64 _float64) {
    *this = __WNFixed8(static_cast<WN_FLOAT64>(*this) / _float64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator + (WN_INT8 _integer8) const {
    return(*this + __WNFixed8(_integer8));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator + (WN_INT16 _integer16) const {
    return(*this + __WNFixed8(_integer16));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator + (WN_INT32 _integer32) const {
    return(*this + __WNFixed8(_integer32));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator + (WN_INT64 _integer64) const {
    return(*this + __WNFixed8(_integer64));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator + (WN_UINT8 _integer8) const {
    return(*this + __WNFixed8(_integer8));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator + (WN_UINT16 _integer16) const {
    return(*this + __WNFixed8(_integer16));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator + (WN_UINT32 _integer32) const {
    return(*this + __WNFixed8(_integer32));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator + (WN_UINT64 _integer64) const {
    return(*this + __WNFixed8(_integer64));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator + (const __WNFixed8& _fixed8) const {
    return(__WNFixed8(*this) += _fixed8);
}

WN_FORCE_INLINE __WNFixed16 __WNFixed8::operator + (const __WNFixed16& _fixed16) const {
    return(__WNFixed16(*this) + _fixed16);
}

WN_FORCE_INLINE __WNFixed32 __WNFixed8::operator + (const __WNFixed32& _fixed32) const {
    return(__WNFixed32(*this) + _fixed32);
}

WN_FORCE_INLINE __WNFixed64 __WNFixed8::operator + (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) + _fixed64);
}

WN_FORCE_INLINE __WNFloat8 __WNFixed8::operator + (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) + _float8);
}

WN_FORCE_INLINE __WNFloat16 __WNFixed8::operator + (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) + _float16);
}

WN_FORCE_INLINE WN_FLOAT32 __WNFixed8::operator + (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) + _float32);
}

WN_FORCE_INLINE WN_FLOAT64 __WNFixed8::operator + (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) + _float64);
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator - (WN_INT8 _integer8) const {
    return(*this - __WNFixed8(_integer8));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator - (WN_INT16 _integer16) const {
    return(*this - __WNFixed8(_integer16));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator - (WN_INT32 _integer32) const {
    return(*this - __WNFixed8(_integer32));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator - (WN_INT64 _integer64) const {
    return(*this - __WNFixed8(_integer64));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator - (WN_UINT8 _integer8) const {
    return(*this - __WNFixed8(_integer8));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator - (WN_UINT16 _integer16) const {
    return(*this - __WNFixed8(_integer16));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator - (WN_UINT32 _integer32) const {
    return(*this - __WNFixed8(_integer32));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator - (WN_UINT64 _integer64) const {
    return(*this - __WNFixed8(_integer64));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator - (const __WNFixed8& _fixed8) const {
    return(__WNFixed8(*this) -= _fixed8);
}

WN_FORCE_INLINE __WNFixed16 __WNFixed8::operator - (const __WNFixed16& _fixed16) const {
    return(__WNFixed16(*this) - _fixed16);
}

WN_FORCE_INLINE __WNFixed32 __WNFixed8::operator - (const __WNFixed32& _fixed32) const {
    return(__WNFixed32(*this) - _fixed32);
}

WN_FORCE_INLINE __WNFixed64 __WNFixed8::operator - (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) - _fixed64);
}

WN_FORCE_INLINE __WNFloat8 __WNFixed8::operator - (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) - _float8);
}

WN_FORCE_INLINE __WNFloat16 __WNFixed8::operator - (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) - _float16);
}

WN_FORCE_INLINE WN_FLOAT32 __WNFixed8::operator - (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) - _float32);
}

WN_FORCE_INLINE WN_FLOAT64 __WNFixed8::operator - (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) - _float64);
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator * (WN_INT8 _integer8) const {
    return(*this * __WNFixed8(_integer8));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator * (WN_INT16 _integer16) const {
    return(*this * __WNFixed8(_integer16));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator * (WN_INT32 _integer32) const {
    return(*this * __WNFixed8(_integer32));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator * (WN_INT64 _integer64) const {
    return(*this * __WNFixed8(_integer64));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator * (WN_UINT8 _integer8) const {
    return(*this * __WNFixed8(_integer8));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator * (WN_UINT16 _integer16) const {
    return(*this * __WNFixed8(_integer16));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator * (WN_UINT32 _integer32) const {
    return(*this * __WNFixed8(_integer32));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator * (WN_UINT64 _integer64) const {
    return(*this * __WNFixed8(_integer64));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator * (const __WNFixed8& _fixed8) const {
    return(__WNFixed8(*this) *= _fixed8);
}

WN_FORCE_INLINE __WNFixed16 __WNFixed8::operator * (const __WNFixed16& _fixed16) const {
    return(__WNFixed16(*this) * _fixed16);
}

WN_FORCE_INLINE __WNFixed32 __WNFixed8::operator * (const __WNFixed32& _fixed32) const {
    return(__WNFixed32(*this) * _fixed32);
}

WN_FORCE_INLINE __WNFixed64 __WNFixed8::operator * (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) * _fixed64);
}

WN_FORCE_INLINE __WNFloat8 __WNFixed8::operator * (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) * _float8);
}

WN_FORCE_INLINE __WNFloat16 __WNFixed8::operator * (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) * _float16);
}

WN_FORCE_INLINE WN_FLOAT32 __WNFixed8::operator * (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) * _float32);
}

WN_FORCE_INLINE WN_FLOAT64 __WNFixed8::operator * (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) * _float64);
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator / (WN_INT8 _integer8) const {
    return(*this / __WNFixed8(_integer8));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator / (WN_INT16 _integer16) const {
    return(*this / __WNFixed8(_integer16));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator / (WN_INT32 _integer32) const {
    return(*this / __WNFixed8(_integer32));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator / (WN_INT64 _integer64) const {
    return(*this / __WNFixed8(_integer64));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator / (WN_UINT8 _integer8) const {
    return(*this / __WNFixed8(_integer8));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator / (WN_UINT16 _integer16) const {
    return(*this / __WNFixed8(_integer16));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator / (WN_UINT32 _integer32) const {
    return(*this / __WNFixed8(_integer32));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator / (WN_UINT64 _integer64) const {
    return(*this / __WNFixed8(_integer64));
}

WN_FORCE_INLINE __WNFixed8 __WNFixed8::operator / (const __WNFixed8& _fixed8) const {
    return(__WNFixed8(*this) /= _fixed8);
}

WN_FORCE_INLINE __WNFixed16 __WNFixed8::operator / (const __WNFixed16& _fixed16) const {
    return(__WNFixed16(*this) / _fixed16);
}

WN_FORCE_INLINE __WNFixed32 __WNFixed8::operator / (const __WNFixed32& _fixed32) const {
    return(__WNFixed32(*this) / _fixed32);
}

WN_FORCE_INLINE __WNFixed64 __WNFixed8::operator / (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) / _fixed64);
}

WN_FORCE_INLINE __WNFloat8 __WNFixed8::operator / (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) / _float8);
}

WN_FORCE_INLINE __WNFloat16 __WNFixed8::operator / (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) / _float16);
}

WN_FORCE_INLINE WN_FLOAT32 __WNFixed8::operator / (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) / _float32);
}

WN_FORCE_INLINE WN_FLOAT64 __WNFixed8::operator / (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) / _float64);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_INT8 _integer8, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer8) == _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_INT16 _integer16, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer16) == _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_INT32 _integer32, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer32) == _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_INT64 _integer64, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer64) == _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_UINT8 _integer8, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer8) == _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_UINT16 _integer16, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer16) == _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_UINT32 _integer32, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer32) == _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_UINT64 _integer64, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer64) == _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_FLOAT32 _float32, const __WNFixed8& _fixed8) {
    return(_float32 != static_cast<WN_FLOAT32>(_fixed8));
}

WN_FORCE_INLINE WN_BOOL operator == (WN_FLOAT64 _float64, const __WNFixed8& _fixed8) {
    return(_float64 != static_cast<WN_FLOAT64>(_fixed8));
}

WN_FORCE_INLINE WN_BOOL operator != (WN_INT8 _integer8, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer8) != _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_INT16 _integer16, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer16) != _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_INT32 _integer32, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer32) != _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_INT64 _integer64, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer64) != _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_UINT8 _integer8, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer8) != _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_UINT16 _integer16, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer16) != _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_UINT32 _integer32, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer32) != _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_UINT64 _integer64, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer64) != _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_FLOAT32 _float32, const __WNFixed8& _fixed8) {
    return(_float32 != static_cast<WN_FLOAT32>(_fixed8));
}

WN_FORCE_INLINE WN_BOOL operator != (WN_FLOAT64 _float64, const __WNFixed8& _fixed8) {
    return(_float64 != static_cast<WN_FLOAT64>(_fixed8));
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_INT8 _integer8, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer8) <= _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_INT16 _integer16, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer16) <= _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_INT32 _integer32, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer32) <= _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_INT64 _integer64, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer64) <= _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_UINT8 _integer8, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer8) <= _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_UINT16 _integer16, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer16) <= _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_UINT32 _integer32, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer32) <= _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_UINT64 _integer64, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer64) <= _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_FLOAT32 _float32, const __WNFixed8& _fixed8) {
    return(_float32 <= static_cast<WN_FLOAT32>(_fixed8));
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_FLOAT64 _float64, const __WNFixed8& _fixed8) {
    return(_float64 <= static_cast<WN_FLOAT64>(_fixed8));
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_INT8 _integer8, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer8) >= _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_INT16 _integer16, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer16) >= _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_INT32 _integer32, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer32) >= _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_INT64 _integer64, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer64) >= _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_UINT8 _integer8, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer8) >= _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_UINT16 _integer16, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer16) >= _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_UINT32 _integer32, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer32) >= _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_UINT64 _integer64, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer64) >= _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_FLOAT32 _float32, const __WNFixed8& _fixed8) {
    return(_float32 >= static_cast<WN_FLOAT32>(_fixed8));
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_FLOAT64 _float64, const __WNFixed8& _fixed8) {
    return(_float64 >= static_cast<WN_FLOAT64>(_fixed8));
}

WN_FORCE_INLINE WN_BOOL operator < (WN_INT8 _integer8, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer8) < _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_INT16 _integer16, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer16) < _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_INT32 _integer32, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer32) < _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_INT64 _integer64, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer64) < _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_UINT8 _integer8, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer8) < _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_UINT16 _integer16, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer16) < _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_UINT32 _integer32, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer32) < _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_UINT64 _integer64, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer64) < _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_FLOAT32 _float32, const __WNFixed8& _fixed8) {
    return(_float32 < static_cast<WN_FLOAT32>(_fixed8));
}

WN_FORCE_INLINE WN_BOOL operator < (WN_FLOAT64 _float64, const __WNFixed8& _fixed8) {
    return(_float64 < static_cast<WN_FLOAT64>(_fixed8));
}

WN_FORCE_INLINE WN_BOOL operator > (WN_INT8 _integer8, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer8) > _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_INT16 _integer16, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer16) > _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_INT32 _integer32, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer32) > _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_INT64 _integer64, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer64) > _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_UINT8 _integer8, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer8) > _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_UINT16 _integer16, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer16) > _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_UINT32 _integer32, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer32) > _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_UINT64 _integer64, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer64) > _fixed8);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_FLOAT32 _float32, const __WNFixed8& _fixed8) {
    return(_float32 > static_cast<WN_FLOAT32>(_fixed8));
}

WN_FORCE_INLINE WN_BOOL operator > (WN_FLOAT64 _float64, const __WNFixed8& _fixed8) {
    return(_float64 > static_cast<WN_FLOAT64>(_fixed8));
}

WN_FORCE_INLINE WN_INT8& operator += (WN_INT8& _integer8, const __WNFixed8& _fixed8) {
    _integer8 += static_cast<WN_INT8>(_fixed8);

    return(_integer8);
}

WN_FORCE_INLINE WN_INT16& operator += (WN_INT16& _integer16, const __WNFixed8& _fixed8) {
    _integer16 += static_cast<WN_INT16>(_fixed8);

    return(_integer16);
}

WN_FORCE_INLINE WN_INT32& operator += (WN_INT32& _integer32, const __WNFixed8& _fixed8) {
    _integer32 += static_cast<WN_INT32>(_fixed8);

    return(_integer32);
}

WN_FORCE_INLINE WN_INT64& operator += (WN_INT64& _integer64, const __WNFixed8& _fixed8) {
    _integer64 += static_cast<WN_INT64>(_fixed8);

    return(_integer64);
}

WN_FORCE_INLINE WN_UINT8& operator += (WN_UINT8& _integer8, const __WNFixed8& _fixed8) {
    _integer8 += static_cast<WN_UINT8>(_fixed8);

    return(_integer8);
}

WN_FORCE_INLINE WN_UINT16& operator += (WN_UINT16& _integer16, const __WNFixed8& _fixed8) {
    _integer16 += static_cast<WN_UINT16>(_fixed8);

    return(_integer16);
}

WN_FORCE_INLINE WN_UINT32& operator += (WN_UINT32& _integer32, const __WNFixed8& _fixed8) {
    _integer32 += static_cast<WN_UINT32>(_fixed8);

    return(_integer32);
}

WN_FORCE_INLINE WN_UINT64& operator += (WN_UINT64& _integer64, const __WNFixed8& _fixed8) {
    _integer64 += static_cast<WN_UINT64>(_fixed8);

    return(_integer64);
}

WN_FORCE_INLINE WN_FLOAT32& operator += (WN_FLOAT32& _float32, const __WNFixed8& _fixed8) {
    _float32 += static_cast<WN_FLOAT32>(_fixed8);

    return(_float32);
}

WN_FORCE_INLINE WN_FLOAT64& operator += (WN_FLOAT64& _float64, const __WNFixed8& _fixed8) {
    _float64 += static_cast<WN_FLOAT64>(_fixed8);

    return(_float64);
}

WN_FORCE_INLINE WN_INT8& operator -= (WN_INT8& _integer8, const __WNFixed8& _fixed8) {
    _integer8 -= static_cast<WN_INT8>(_fixed8);

    return(_integer8);
}

WN_FORCE_INLINE WN_INT16& operator -= (WN_INT16& _integer16, const __WNFixed8& _fixed8) {
    _integer16 -= static_cast<WN_INT16>(_fixed8);

    return(_integer16);
}

WN_FORCE_INLINE WN_INT32& operator -= (WN_INT32& _integer32, const __WNFixed8& _fixed8) {
    _integer32 -= static_cast<WN_INT32>(_fixed8);

    return(_integer32);
}

WN_FORCE_INLINE WN_INT64& operator -= (WN_INT64& _integer64, const __WNFixed8& _fixed8) {
    _integer64 -= static_cast<WN_INT64>(_fixed8);

    return(_integer64);
}

WN_FORCE_INLINE WN_UINT8& operator -= (WN_UINT8& _integer8, const __WNFixed8& _fixed8) {
    _integer8 -= static_cast<WN_UINT8>(_fixed8);

    return(_integer8);
}

WN_FORCE_INLINE WN_UINT16& operator -= (WN_UINT16& _integer16, const __WNFixed8& _fixed8) {
    _integer16 -= static_cast<WN_UINT16>(_fixed8);

    return(_integer16);
}

WN_FORCE_INLINE WN_UINT32& operator -= (WN_UINT32& _integer32, const __WNFixed8& _fixed8) {
    _integer32 -= static_cast<WN_UINT32>(_fixed8);

    return(_integer32);
}

WN_FORCE_INLINE WN_UINT64& operator -= (WN_UINT64& _integer64, const __WNFixed8& _fixed8) {
    _integer64 -= static_cast<WN_UINT64>(_fixed8);

    return(_integer64);
}

WN_FORCE_INLINE WN_FLOAT32& operator -= (WN_FLOAT32& _float32, const __WNFixed8& _fixed8) {
    _float32 -= static_cast<WN_FLOAT32>(_fixed8);

    return(_float32);
}

WN_FORCE_INLINE WN_FLOAT64& operator -= (WN_FLOAT64& _float64, const __WNFixed8& _fixed8) {
    _float64 -= static_cast<WN_FLOAT64>(_fixed8);

    return(_float64);
}

WN_FORCE_INLINE WN_INT8& operator *= (WN_INT8& _integer8, const __WNFixed8& _fixed8) {
    _integer8 *= static_cast<WN_INT8>(_fixed8);

    return(_integer8);
}

WN_FORCE_INLINE WN_INT16& operator *= (WN_INT16& _integer16, const __WNFixed8& _fixed8) {
    _integer16 *= static_cast<WN_INT16>(_fixed8);

    return(_integer16);
}

WN_FORCE_INLINE WN_INT32& operator *= (WN_INT32& _integer32, const __WNFixed8& _fixed8) {
    _integer32 *= static_cast<WN_INT32>(_fixed8);

    return(_integer32);
}

WN_FORCE_INLINE WN_INT64& operator *= (WN_INT64& _integer64, const __WNFixed8& _fixed8) {
    _integer64 *= static_cast<WN_INT64>(_fixed8);

    return(_integer64);
}

WN_FORCE_INLINE WN_UINT8& operator *= (WN_UINT8& _integer8, const __WNFixed8& _fixed8) {
    _integer8 *= static_cast<WN_UINT8>(_fixed8);

    return(_integer8);
}

WN_FORCE_INLINE WN_UINT16& operator *= (WN_UINT16& _integer16, const __WNFixed8& _fixed8) {
    _integer16 *= static_cast<WN_UINT16>(_fixed8);

    return(_integer16);
}

WN_FORCE_INLINE WN_UINT32& operator *= (WN_UINT32& _integer32, const __WNFixed8& _fixed8) {
    _integer32 *= static_cast<WN_UINT32>(_fixed8);

    return(_integer32);
}

WN_FORCE_INLINE WN_UINT64& operator *= (WN_UINT64& _integer64, const __WNFixed8& _fixed8) {
    _integer64 *= static_cast<WN_UINT64>(_fixed8);

    return(_integer64);
}

WN_FORCE_INLINE WN_FLOAT32& operator *= (WN_FLOAT32& _float32, const __WNFixed8& _fixed8) {
    _float32 *= static_cast<WN_FLOAT32>(_fixed8);

    return(_float32);
}

WN_FORCE_INLINE WN_FLOAT64& operator *= (WN_FLOAT64& _float64, const __WNFixed8& _fixed8) {
    _float64 *= static_cast<WN_FLOAT64>(_fixed8);

    return(_float64);
}

WN_FORCE_INLINE WN_INT8& operator /= (WN_INT8& _integer8, const __WNFixed8& _fixed8) {
    _integer8 /= static_cast<WN_INT8>(_fixed8);

    return(_integer8);
}

WN_FORCE_INLINE WN_INT16& operator /= (WN_INT16& _integer16, const __WNFixed8& _fixed8) {
    _integer16 /= static_cast<WN_INT16>(_fixed8);

    return(_integer16);
}

WN_FORCE_INLINE WN_INT32& operator /= (WN_INT32& _integer32, const __WNFixed8& _fixed8) {
    _integer32 /= static_cast<WN_INT32>(_fixed8);

    return(_integer32);
}

WN_FORCE_INLINE WN_INT64& operator /= (WN_INT64& _integer64, const __WNFixed8& _fixed8) {
    _integer64 /= static_cast<WN_INT64>(_fixed8);

    return(_integer64);
}

WN_FORCE_INLINE WN_UINT8& operator /= (WN_UINT8& _integer8, const __WNFixed8& _fixed8) {
    _integer8 /= static_cast<WN_UINT8>(_fixed8);

    return(_integer8);
}

WN_FORCE_INLINE WN_UINT16& operator /= (WN_UINT16& _integer16, const __WNFixed8& _fixed8) {
    _integer16 /= static_cast<WN_UINT16>(_fixed8);

    return(_integer16);
}

WN_FORCE_INLINE WN_UINT32& operator /= (WN_UINT32& _integer32, const __WNFixed8& _fixed8) {
    _integer32 /= static_cast<WN_UINT32>(_fixed8);

    return(_integer32);
}

WN_FORCE_INLINE WN_UINT64& operator /= (WN_UINT64& _integer64, const __WNFixed8& _fixed8) {
    _integer64 /= static_cast<WN_UINT64>(_fixed8);

    return(_integer64);
}

WN_FORCE_INLINE WN_FLOAT32& operator /= (WN_FLOAT32& _float32, const __WNFixed8& _fixed8) {
    _float32 /= static_cast<WN_FLOAT32>(_fixed8);

    return(_float32);
}

WN_FORCE_INLINE WN_FLOAT64& operator /= (WN_FLOAT64& _float64, const __WNFixed8& _fixed8) {
    _float64 /= static_cast<WN_FLOAT64>(_fixed8);

    return(_float64);
}

WN_FORCE_INLINE __WNFixed8 operator + (WN_INT8 _integer8, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer8) + _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator + (WN_INT16 _integer16, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer16) + _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator + (WN_INT32 _integer32, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer32) + _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator + (WN_INT64 _integer64, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer64) + _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator + (WN_UINT8 _integer8, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer8) + _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator + (WN_UINT16 _integer16, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer16) + _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator + (WN_UINT32 _integer32, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer32) + _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator + (WN_UINT64 _integer64, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer64) + _fixed8);
}

WN_FORCE_INLINE WN_FLOAT32 operator + (WN_FLOAT32 _float32, const __WNFixed8& _fixed8) {
    return(_float32 + static_cast<WN_FLOAT32>(_fixed8));
}

WN_FORCE_INLINE WN_FLOAT64 operator + (WN_FLOAT64 _float64, const __WNFixed8& _fixed8) {
    return(_float64 + static_cast<WN_FLOAT64>(_fixed8));
}

WN_FORCE_INLINE __WNFixed8 operator - (WN_INT8 _integer8, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer8) - _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator - (WN_INT16 _integer16, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer16) - _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator - (WN_INT32 _integer32, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer32) - _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator - (WN_INT64 _integer64, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer64) - _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator - (WN_UINT8 _integer8, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer8) - _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator - (WN_UINT16 _integer16, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer16) - _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator - (WN_UINT32 _integer32, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer32) - _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator - (WN_UINT64 _integer64, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer64) - _fixed8);
}

WN_FORCE_INLINE WN_FLOAT32 operator - (WN_FLOAT32 _float32, const __WNFixed8& _fixed8) {
    return(_float32 - static_cast<WN_FLOAT32>(_fixed8));
}

WN_FORCE_INLINE WN_FLOAT64 operator - (WN_FLOAT64 _float64, const __WNFixed8& _fixed8) {
    return(_float64 - static_cast<WN_FLOAT64>(_fixed8));
}

WN_FORCE_INLINE __WNFixed8 operator * (WN_INT8 _integer8, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer8) * _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator * (WN_INT16 _integer16, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer16) * _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator * (WN_INT32 _integer32, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer32) * _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator * (WN_INT64 _integer64, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer64) * _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator * (WN_UINT8 _integer8, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer8) * _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator * (WN_UINT16 _integer16, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer16) * _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator * (WN_UINT32 _integer32, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer32) * _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator * (WN_UINT64 _integer64, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer64) * _fixed8);
}

WN_FORCE_INLINE WN_FLOAT32 operator * (WN_FLOAT32 _float32, const __WNFixed8& _fixed8) {
    return(_float32 * static_cast<WN_FLOAT32>(_fixed8));
}

WN_FORCE_INLINE WN_FLOAT64 operator * (WN_FLOAT64 _float64, const __WNFixed8& _fixed8) {
    return(_float64 * static_cast<WN_FLOAT64>(_fixed8));
}

WN_FORCE_INLINE __WNFixed8 operator / (WN_INT8 _integer8, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer8) / _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator / (WN_INT16 _integer16, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer16) / _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator / (WN_INT32 _integer32, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer32) / _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator / (WN_INT64 _integer64, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer64) / _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator / (WN_UINT8 _integer8, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer8) / _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator / (WN_UINT16 _integer16, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer16) / _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator / (WN_UINT32 _integer32, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer32) / _fixed8);
}

WN_FORCE_INLINE __WNFixed8 operator / (WN_UINT64 _integer64, const __WNFixed8& _fixed8) {
    return(__WNFixed8(_integer64) / _fixed8);
}

WN_FORCE_INLINE WN_FLOAT32 operator / (WN_FLOAT32 _float32, const __WNFixed8& _fixed8) {
    return(_float32 / static_cast<WN_FLOAT32>(_fixed8));
}

WN_FORCE_INLINE WN_FLOAT64 operator / (WN_FLOAT64 _float64, const __WNFixed8& _fixed8) {
    return(_float64 / static_cast<WN_FLOAT64>(_fixed8));
}

// Fixed (16 bit) //////////////////////////////////////////////////////////////////////////////////////////////////////////////

WN_FORCE_INLINE __WNFixed16::__WNFixed16(WN_INT8 _integer8) {
    mBits = static_cast<WN_INT16>(_integer8) << WN_FIXED16_BITS;
}

WN_FORCE_INLINE __WNFixed16::__WNFixed16(WN_INT16 _integer16) {
    mBits = static_cast<WN_INT16>(_integer16) << WN_FIXED16_BITS;
}

WN_FORCE_INLINE __WNFixed16::__WNFixed16(WN_INT32 _integer32) {
    mBits = static_cast<WN_INT16>(_integer32) << WN_FIXED16_BITS;
}

WN_FORCE_INLINE __WNFixed16::__WNFixed16(WN_INT64 _integer64) {
    mBits = static_cast<WN_INT16>(_integer64) << WN_FIXED16_BITS;
}

WN_FORCE_INLINE __WNFixed16::__WNFixed16(WN_UINT8 _integer8) {
    mBits = static_cast<WN_INT16>(_integer8) << WN_FIXED16_BITS;
}

WN_FORCE_INLINE __WNFixed16::__WNFixed16(WN_UINT16 _integer16) {
    mBits = static_cast<WN_INT16>(_integer16) << WN_FIXED16_BITS;
}

WN_FORCE_INLINE __WNFixed16::__WNFixed16(WN_UINT32 _integer32) {
    mBits = static_cast<WN_INT16>(_integer32) << WN_FIXED16_BITS;
}

WN_FORCE_INLINE __WNFixed16::__WNFixed16(WN_UINT64 _integer64) {
    mBits = static_cast<WN_INT16>(_integer64) << WN_FIXED16_BITS;
}

WN_FORCE_INLINE __WNFixed16::__WNFixed16(const __WNFixed8& _fixed8) {
    mBits = static_cast<WN_INT16>(_fixed8.mBits) << (WN_FIXED16_BITS - WN_FIXED8_BITS);
}

WN_FORCE_INLINE __WNFixed16::__WNFixed16(const __WNFixed16& _fixed16) {
    mBits = _fixed16.mBits;
}

WN_FORCE_INLINE __WNFixed16::__WNFixed16(const __WNFixed32& _fixed32) {
    mBits = static_cast<WN_INT16>(_fixed32.mBits >> (WN_FIXED32_BITS - WN_FIXED16_BITS));
}

WN_FORCE_INLINE __WNFixed16::__WNFixed16(const __WNFixed64& _fixed64) {
    mBits = static_cast<WN_INT16>(_fixed64.mBits >> (WN_FIXED64_BITS - WN_FIXED16_BITS));
}

WN_FORCE_INLINE __WNFixed16::__WNFixed16(const __WNFloat8& _float8) {
    const WN_FLOAT32 float32 = static_cast<WN_FLOAT32>(_float8);

    mBits = static_cast<WN_INT16>((float32 * __WN_FIXED16_ONE) + (float32 >= 0.0f ? 0.5f : -0.5f));
}

WN_FORCE_INLINE __WNFixed16::__WNFixed16(const __WNFloat16& _float16) {
    const WN_FLOAT32 float32 = static_cast<WN_FLOAT32>(_float16);

    mBits = static_cast<WN_INT16>((float32 * __WN_FIXED16_ONE) + (float32 >= 0.0f ? 0.5f : -0.5f));
}

WN_FORCE_INLINE __WNFixed16::__WNFixed16(WN_FLOAT32 _float32) {
    mBits = static_cast<WN_INT16>((_float32 * __WN_FIXED16_ONE) + (_float32 >= 0.0f ? 0.5f : -0.5f));
}

WN_FORCE_INLINE __WNFixed16::__WNFixed16(WN_FLOAT64 _float64) {
    mBits = static_cast<WN_INT16>((_float64 * __WN_FIXED16_ONE) + (_float64 >= 0.0 ? 0.5 : -0.5));
}

WN_FORCE_INLINE __WNFixed16::operator WN_INT8 () const {
    return(static_cast<WN_INT8>(static_cast<WN_INT16>(*this)));
}

WN_FORCE_INLINE __WNFixed16::operator WN_INT16 () const {
    return(mBits >> WN_FIXED16_BITS);
}

WN_FORCE_INLINE __WNFixed16::operator WN_INT32 () const {
    return(static_cast<WN_INT32>(static_cast<WN_INT16>(*this)));
}

WN_FORCE_INLINE __WNFixed16::operator WN_INT64 () const {
    return(static_cast<WN_INT64>(static_cast<WN_INT16>(*this)));
}

WN_FORCE_INLINE __WNFixed16::operator WN_UINT8 () const {
    return(static_cast<WN_UINT8>(static_cast<WN_INT16>(*this)));
}

WN_FORCE_INLINE __WNFixed16::operator WN_UINT16 () const {
    return(static_cast<WN_UINT16>(static_cast<WN_INT16>(*this)));
}

WN_FORCE_INLINE __WNFixed16::operator WN_UINT32 () const {
    return(static_cast<WN_UINT32>(static_cast<WN_INT16>(*this)));
}

WN_FORCE_INLINE __WNFixed16::operator WN_UINT64 () const {
    return(static_cast<WN_UINT64>(static_cast<WN_INT16>(*this)));
}

WN_FORCE_INLINE __WNFixed16::operator WN_FLOAT32 () const {
    return(static_cast<WN_FLOAT32>(mBits) / __WN_FIXED16_ONE);
}

WN_FORCE_INLINE __WNFixed16::operator WN_FLOAT64 () const {
    return(static_cast<WN_FLOAT64>(static_cast<WN_FLOAT32>(*this)));
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator = (WN_INT8 _integer8) {
    *this = __WNFixed16(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator = (WN_INT16 _integer16) {
    *this = __WNFixed16(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator = (WN_INT32 _integer32) {
    *this = __WNFixed16(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator = (WN_INT64 _integer64) {
    *this = __WNFixed16(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator = (WN_UINT8 _integer8) {
    *this = __WNFixed16(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator = (WN_UINT16 _integer16) {
    *this = __WNFixed16(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator = (WN_UINT32 _integer32) {
    *this = __WNFixed16(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator = (WN_UINT64 _integer64) {
    *this = __WNFixed16(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator = (const __WNFixed8& _fixed8) {
    *this = __WNFixed16(_fixed8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator = (const __WNFixed16& _fixed16) {
    mBits = _fixed16.mBits;

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator = (const __WNFixed32& _fixed32) {
    *this = __WNFixed16(_fixed32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator = (const __WNFixed64& _fixed64) {
    *this = __WNFixed16(_fixed64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator = (const __WNFloat8& _float8) {
    *this = __WNFixed16(_float8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator = (const __WNFloat16& _float16) {
    *this = __WNFixed16(_float16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator = (WN_FLOAT32 _float32) {
    *this = __WNFixed16(_float32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator = (WN_FLOAT64 _float64) {
    *this = __WNFixed16(_float64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator + () const {
    return(*this);
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator - () const {
    __WNFixed16 result;

    result.mBits = -mBits;

    return(result);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator ++ () {
    mBits += __WN_FIXED16_ONE;

    return(*this);
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator ++ (WN_INT32) {
    const __WNFixed16 result = *this;

    ++(*this);

    return(result);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator -- () {
    mBits -= __WN_FIXED16_ONE;

    return(*this);
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator -- (WN_INT32) {
    const __WNFixed16 result = *this;

    --(*this);

    return(result);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator ! () const {
    return(!mBits);
}


WN_FORCE_INLINE WN_BOOL __WNFixed16::operator == (WN_INT8 _integer8) const {
    return(*this == __WNFixed16(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator == (WN_INT16 _integer16) const {
    return(*this == __WNFixed16(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator == (WN_INT32 _integer32) const {
    return(*this == __WNFixed16(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator == (WN_INT64 _integer64) const {
    return(*this == __WNFixed16(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator == (WN_UINT8 _integer8) const {
    return(*this == __WNFixed16(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator == (WN_UINT16 _integer16) const {
    return(*this == __WNFixed16(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator == (WN_UINT32 _integer32) const {
    return(*this == __WNFixed16(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator == (WN_UINT64 _integer64) const {
    return(*this == __WNFixed16(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator == (const __WNFixed8& _fixed8) const {
    return(*this == __WNFixed16(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator == (const __WNFixed16& _fixed16) const {
    return(mBits == _fixed16.mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator == (const __WNFixed32& _fixed32) const {
    return(__WNFixed32(*this) == _fixed32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator == (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) == _fixed64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator == (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) == _float8);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator == (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) == _float16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator == (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) == _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator == (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) == _float64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator != (WN_INT8 _integer8) const {
    return(*this != __WNFixed16(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator != (WN_INT16 _integer16) const {
    return(*this != __WNFixed16(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator != (WN_INT32 _integer32) const {
    return(*this != __WNFixed16(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator != (WN_INT64 _integer64) const {
    return(*this != __WNFixed16(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator != (WN_UINT8 _integer8) const {
    return(*this != __WNFixed16(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator != (WN_UINT16 _integer16) const {
    return(*this != __WNFixed16(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator != (WN_UINT32 _integer32) const {
    return(*this != __WNFixed16(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator != (WN_UINT64 _integer64) const {
    return(*this != __WNFixed16(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator != (const __WNFixed8& _fixed8) const {
    return(*this != __WNFixed16(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator != (const __WNFixed16& _fixed16) const {
    return(mBits != _fixed16.mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator != (const __WNFixed32& _fixed32) const {
    return(__WNFixed32(*this) != _fixed32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator != (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) != _fixed64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator != (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) != _float8);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator != (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) != _float16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator != (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) != _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator != (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) != _float64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator <= (WN_INT8 _integer8) const {
    return(*this <= __WNFixed16(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator <= (WN_INT16 _integer16) const {
    return(*this <= __WNFixed16(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator <= (WN_INT32 _integer32) const {
    return(*this <= __WNFixed16(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator <= (WN_INT64 _integer64) const {
    return(*this <= __WNFixed16(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator <= (WN_UINT8 _integer8) const {
    return(*this <= __WNFixed16(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator <= (WN_UINT16 _integer16) const {
    return(*this <= __WNFixed16(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator <= (WN_UINT32 _integer32) const {
    return(*this <= __WNFixed16(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator <= (WN_UINT64 _integer64) const {
    return(*this <= __WNFixed16(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator <= (const __WNFixed8& _fixed8) const {
    return(*this <= __WNFixed16(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator <= (const __WNFixed16& _fixed16) const {
    return(mBits <= _fixed16.mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator <= (const __WNFixed32& _fixed32) const {
    return(__WNFixed32(*this) <= _fixed32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator <= (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) <= _fixed64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator <= (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) <= _float8);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator <= (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) <= _float16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator <= (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) <= _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator <= (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) <= _float64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator >= (WN_INT8 _integer8) const {
    return(*this >= __WNFixed16(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator >= (WN_INT16 _integer16) const {
    return(*this >= __WNFixed16(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator >= (WN_INT32 _integer32) const {
    return(*this >= __WNFixed16(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator >= (WN_INT64 _integer64) const {
    return(*this >= __WNFixed16(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator >= (WN_UINT8 _integer8) const {
    return(*this >= __WNFixed16(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator >= (WN_UINT16 _integer16) const {
    return(*this >= __WNFixed16(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator >= (WN_UINT32 _integer32) const {
    return(*this >= __WNFixed16(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator >= (WN_UINT64 _integer64) const {
    return(*this >= __WNFixed16(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator >= (const __WNFixed8& _fixed8) const {
    return(*this >= __WNFixed16(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator >= (const __WNFixed16& _fixed16) const {
    return(mBits >= _fixed16.mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator >= (const __WNFixed32& _fixed32) const {
    return(__WNFixed32(*this) >= _fixed32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator >= (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) >= _fixed64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator >= (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) >= _float8);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator >= (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) >= _float16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator >= (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) >= _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator >= (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) >= _float64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator < (WN_INT8 _integer8) const {
    return(*this < __WNFixed16(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator < (WN_INT16 _integer16) const {
    return(*this < __WNFixed16(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator < (WN_INT32 _integer32) const {
    return(*this < __WNFixed16(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator < (WN_INT64 _integer64) const {
    return(*this < __WNFixed16(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator < (WN_UINT8 _integer8) const {
    return(*this < __WNFixed16(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator < (WN_UINT16 _integer16) const {
    return(*this < __WNFixed16(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator < (WN_UINT32 _integer32) const {
    return(*this < __WNFixed16(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator < (WN_UINT64 _integer64) const {
    return(*this < __WNFixed16(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator < (const __WNFixed8& _fixed8) const {
    return(*this < __WNFixed16(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator < (const __WNFixed16& _fixed16) const {
    return(mBits < _fixed16.mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator < (const __WNFixed32& _fixed32) const {
    return(__WNFixed32(*this) < _fixed32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator < (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) < _fixed64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator < (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) < _float8);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator < (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) < _float16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator < (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) < _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator < (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) < _float64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator > (WN_INT8 _integer8) const {
    return(*this > __WNFixed16(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator > (WN_INT16 _integer16) const {
    return(*this > __WNFixed16(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator > (WN_INT32 _integer32) const {
    return(*this > __WNFixed16(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator > (WN_INT64 _integer64) const {
    return(*this > __WNFixed16(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator > (WN_UINT8 _integer8) const {
    return(*this > __WNFixed16(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator > (WN_UINT16 _integer16) const {
    return(*this > __WNFixed16(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator > (WN_UINT32 _integer32) const {
    return(*this > __WNFixed16(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator > (WN_UINT64 _integer64) const {
    return(*this > __WNFixed16(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator > (const __WNFixed8& _fixed8) const {
    return(*this > __WNFixed16(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator > (const __WNFixed16& _fixed16) const {
    return(mBits > _fixed16.mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator > (const __WNFixed32& _fixed32) const {
    return(__WNFixed32(*this) > _fixed32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator > (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) > _fixed64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator > (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) > _float8);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator > (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) > _float16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator > (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) > _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed16::operator > (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) > _float64);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator += (WN_INT8 _integer8) {
    *this += __WNFixed16(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator += (WN_INT16 _integer16) {
    *this += __WNFixed16(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator += (WN_INT32 _integer32) {
    *this += __WNFixed16(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator += (WN_INT64 _integer64) {
    *this += __WNFixed16(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator += (WN_UINT8 _integer8) {
    *this += __WNFixed16(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator += (WN_UINT16 _integer16) {
    *this += __WNFixed16(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator += (WN_UINT32 _integer32) {
    *this += __WNFixed16(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator += (WN_UINT64 _integer64) {
    *this += __WNFixed16(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator += (const __WNFixed8& _fixed8) {
    *this += __WNFixed16(_fixed8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator += (const __WNFixed16& _fixed16) {
    mBits += _fixed16.mBits;

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator += (const __WNFixed32& _fixed32) {
    *this += __WNFixed16(_fixed32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator += (const __WNFixed64& _fixed64) {
    *this += __WNFixed16(_fixed64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator += (const __WNFloat8& _float8) {
    *this += __WNFixed16(_float8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator += (const __WNFloat16& _float16) {
    *this += __WNFixed16(_float16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator += (WN_FLOAT32 _float32) {
    *this += __WNFixed16(_float32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator += (WN_FLOAT64 _float64) {
    *this += __WNFixed16(_float64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator -= (WN_INT8 _integer8) {
    *this -= __WNFixed16(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator -= (WN_INT16 _integer16) {
    *this -= __WNFixed16(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator -= (WN_INT32 _integer32) {
    *this -= __WNFixed16(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator -= (WN_INT64 _integer64) {
    *this -= __WNFixed16(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator -= (WN_UINT8 _integer8) {
    *this -= __WNFixed16(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator -= (WN_UINT16 _integer16) {
    *this -= __WNFixed16(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator -= (WN_UINT32 _integer32) {
    *this -= __WNFixed16(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator -= (WN_UINT64 _integer64) {
    *this -= __WNFixed16(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator -= (const __WNFixed8& _fixed8) {
    *this -= __WNFixed16(_fixed8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator -= (const __WNFixed16& _fixed16) {
    mBits -= _fixed16.mBits;

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator -= (const __WNFixed32& _fixed32) {
    *this -= __WNFixed16(_fixed32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator -= (const __WNFixed64& _fixed64) {
    *this -= __WNFixed16(_fixed64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator -= (const __WNFloat8& _float8) {
    *this -= __WNFixed16(_float8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator -= (const __WNFloat16& _float16) {
    *this -= __WNFixed16(_float16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator -= (WN_FLOAT32 _float32) {
    *this -= __WNFixed16(_float32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator -= (WN_FLOAT64 _float64) {
    *this -= __WNFixed16(_float64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator *= (WN_INT8 _integer8) {
    *this *= __WNFixed16(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator *= (WN_INT16 _integer16) {
    *this *= __WNFixed16(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator *= (WN_INT32 _integer32) {
    *this *= __WNFixed16(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator *= (WN_INT64 _integer64) {
    *this *= __WNFixed16(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator *= (WN_UINT8 _integer8) {
    *this *= __WNFixed16(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator *= (WN_UINT16 _integer16) {
    *this *= __WNFixed16(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator *= (WN_UINT32 _integer32) {
    *this *= __WNFixed16(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator *= (const __WNFixed8& _fixed8) {
    *this *= __WNFixed16(_fixed8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator *= (const __WNFixed16& _fixed16) {
    mBits = static_cast<WN_INT16>((static_cast<WN_INT32>(mBits) * static_cast<WN_INT32>(_fixed16.mBits)) >> WN_FIXED16_BITS);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator *= (const __WNFixed32& _fixed32) {
    *this *= __WNFixed16(_fixed32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator *= (const __WNFixed64& _fixed64) {
    *this *= __WNFixed16(_fixed64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator *= (const __WNFloat8& _float8) {
    *this *= __WNFixed16(_float8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator *= (const __WNFloat16& _float16) {
    *this *= __WNFixed16(_float16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator *= (WN_UINT64 _integer64) {
    *this *= __WNFixed16(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator *= (WN_FLOAT32 _float32) {
    *this *= __WNFixed16(_float32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator *= (WN_FLOAT64 _float64) {
    *this *= __WNFixed16(_float64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator /= (WN_INT8 _integer8) {
    *this /= __WNFixed16(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator /= (WN_INT16 _integer16) {
    *this /= __WNFixed16(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator /= (WN_INT32 _integer32) {
    *this /= __WNFixed16(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator /= (WN_INT64 _integer64) {
    *this /= __WNFixed16(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator /= (WN_UINT8 _integer8) {
    *this /= __WNFixed16(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator /= (WN_UINT16 _integer16) {
    *this /= __WNFixed16(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator /= (WN_UINT32 _integer32) {
    *this /= __WNFixed16(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator /= (WN_UINT64 _integer64) {
    *this /= __WNFixed16(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator /= (const __WNFixed8& _fixed8) {
    *this /= __WNFixed16(_fixed8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator /= (const __WNFixed16& _fixed16) {
    mBits = static_cast<WN_INT16>((static_cast<WN_INT32>(mBits) << WN_FIXED16_BITS) / _fixed16.mBits);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator /= (const __WNFixed32& _fixed32) {
    *this /= __WNFixed16(_fixed32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator /= (const __WNFixed64& _fixed64) {
    *this /= __WNFixed16(_fixed64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator /= (const __WNFloat8& _float8) {
    *this /= __WNFixed16(_float8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator /= (const __WNFloat16& _float16) {
    *this /= __WNFixed16(_float16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator /= (WN_FLOAT32 _float32) {
    *this /= __WNFixed16(_float32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16& __WNFixed16::operator /= (WN_FLOAT64 _float64) {
    *this /= __WNFixed16(_float64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator + (WN_INT8 _integer8) const {
    return(*this + __WNFixed16(_integer8));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator + (WN_INT16 _integer16) const {
    return(*this + __WNFixed16(_integer16));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator + (WN_INT32 _integer32) const {
    return(*this + __WNFixed16(_integer32));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator + (WN_INT64 _integer64) const {
    return(*this + __WNFixed16(_integer64));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator + (WN_UINT8 _integer8) const {
    return(*this + __WNFixed16(_integer8));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator + (WN_UINT16 _integer16) const {
    return(*this + __WNFixed16(_integer16));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator + (WN_UINT32 _integer32) const {
    return(*this + __WNFixed16(_integer32));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator + (WN_UINT64 _integer64) const {
    return(*this + __WNFixed16(_integer64));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator + (const __WNFixed8& _fixed8) const {
    return(*this + __WNFixed16(_fixed8));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator + (const __WNFixed16& _fixed16) const {
    return(__WNFixed16(*this) += _fixed16);
}

WN_FORCE_INLINE __WNFixed32 __WNFixed16::operator + (const __WNFixed32& _fixed32) const {
    return(__WNFixed32(*this) + _fixed32);
}

WN_FORCE_INLINE __WNFixed64 __WNFixed16::operator + (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) + _fixed64);
}

WN_FORCE_INLINE __WNFloat8 __WNFixed16::operator + (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) + _float8);
}

WN_FORCE_INLINE __WNFloat16 __WNFixed16::operator + (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) + _float16);
}

WN_FORCE_INLINE WN_FLOAT32 __WNFixed16::operator + (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) + _float32);
}

WN_FORCE_INLINE WN_FLOAT64 __WNFixed16::operator + (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) + _float64);
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator - (WN_INT8 _integer8) const {
    return(*this - __WNFixed16(_integer8));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator - (WN_INT16 _integer16) const {
    return(*this - __WNFixed16(_integer16));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator - (WN_INT32 _integer32) const {
    return(*this - __WNFixed16(_integer32));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator - (WN_INT64 _integer64) const {
    return(*this - __WNFixed16(_integer64));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator - (WN_UINT8 _integer8) const {
    return(*this - __WNFixed16(_integer8));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator - (WN_UINT16 _integer16) const {
    return(*this - __WNFixed16(_integer16));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator - (WN_UINT32 _integer32) const {
    return(*this - __WNFixed16(_integer32));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator - (WN_UINT64 _integer64) const {
    return(*this - __WNFixed16(_integer64));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator - (const __WNFixed8& _fixed8) const {
    return(*this - __WNFixed16(_fixed8));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator - (const __WNFixed16& _fixed16) const {
    return(__WNFixed16(*this) -= _fixed16);
}

WN_FORCE_INLINE __WNFixed32 __WNFixed16::operator - (const __WNFixed32& _fixed32) const {
    return(__WNFixed32(*this) - _fixed32);
}

WN_FORCE_INLINE __WNFixed64 __WNFixed16::operator - (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) - _fixed64);
}

WN_FORCE_INLINE __WNFloat8 __WNFixed16::operator - (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) - _float8);
}

WN_FORCE_INLINE __WNFloat16 __WNFixed16::operator - (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) - _float16);
}

WN_FORCE_INLINE WN_FLOAT32 __WNFixed16::operator - (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) - _float32);
}

WN_FORCE_INLINE WN_FLOAT64 __WNFixed16::operator - (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) - _float64);
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator * (WN_INT8 _integer8) const {
    return(*this * __WNFixed16(_integer8));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator * (WN_INT16 _integer16) const {
    return(*this * __WNFixed16(_integer16));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator * (WN_INT32 _integer32) const {
    return(*this * __WNFixed16(_integer32));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator * (WN_INT64 _integer64) const {
    return(*this * __WNFixed16(_integer64));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator * (WN_UINT8 _integer8) const {
    return(*this * __WNFixed16(_integer8));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator * (WN_UINT16 _integer16) const {
    return(*this * __WNFixed16(_integer16));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator * (WN_UINT32 _integer32) const {
    return(*this * __WNFixed16(_integer32));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator * (WN_UINT64 _integer64) const {
    return(*this * __WNFixed16(_integer64));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator * (const __WNFixed8& _fixed8) const {
    return(*this * __WNFixed16(_fixed8));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator * (const __WNFixed16& _fixed16) const {
    return(__WNFixed16(*this) *= _fixed16);
}

WN_FORCE_INLINE __WNFixed32 __WNFixed16::operator * (const __WNFixed32& _fixed32) const {
    return(__WNFixed32(*this) * _fixed32);
}

WN_FORCE_INLINE __WNFixed64 __WNFixed16::operator * (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) * _fixed64);
}

WN_FORCE_INLINE __WNFloat8 __WNFixed16::operator * (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) * _float8);
}

WN_FORCE_INLINE __WNFloat16 __WNFixed16::operator * (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) * _float16);
}

WN_FORCE_INLINE WN_FLOAT32 __WNFixed16::operator * (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) * _float32);
}

WN_FORCE_INLINE WN_FLOAT64 __WNFixed16::operator * (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) * _float64);
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator / (WN_INT8 _integer8) const {
    return(*this / __WNFixed16(_integer8));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator / (WN_INT16 _integer16) const {
    return(*this / __WNFixed16(_integer16));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator / (WN_INT32 _integer32) const {
    return(*this / __WNFixed16(_integer32));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator / (WN_INT64 _integer64) const {
    return(*this / __WNFixed16(_integer64));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator / (WN_UINT8 _integer8) const {
    return(*this / __WNFixed16(_integer8));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator / (WN_UINT16 _integer16) const {
    return(*this / __WNFixed16(_integer16));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator / (WN_UINT32 _integer32) const {
    return(*this / __WNFixed16(_integer32));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator / (WN_UINT64 _integer64) const {
    return(*this / __WNFixed16(_integer64));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator / (const __WNFixed8& _fixed8) const {
    return(*this / __WNFixed16(_fixed8));
}

WN_FORCE_INLINE __WNFixed16 __WNFixed16::operator / (const __WNFixed16& _fixed16) const {
    return(__WNFixed16(*this) /= _fixed16);
}

WN_FORCE_INLINE __WNFixed32 __WNFixed16::operator / (const __WNFixed32& _fixed32) const {
    return(__WNFixed32(*this) / _fixed32);
}

WN_FORCE_INLINE __WNFixed64 __WNFixed16::operator / (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) / _fixed64);
}

WN_FORCE_INLINE __WNFloat8 __WNFixed16::operator / (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) / _float8);
}

WN_FORCE_INLINE __WNFloat16 __WNFixed16::operator / (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) / _float16);
}

WN_FORCE_INLINE WN_FLOAT32 __WNFixed16::operator / (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) / _float32);
}

WN_FORCE_INLINE WN_FLOAT64 __WNFixed16::operator / (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) / _float64);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_INT8 _integer8, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer8) == _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_INT16 _integer16, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer16) == _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_INT32 _integer32, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer32) == _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_INT64 _integer64, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer64) == _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_UINT8 _integer8, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer8) == _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_UINT16 _integer16, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer16) == _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_UINT32 _integer32, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer32) == _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_UINT64 _integer64, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer64) == _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_FLOAT32 _float32, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_float32) == _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_FLOAT64 _float64, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_float64) == _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_INT8 _integer8, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer8) != _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_INT16 _integer16, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer16) != _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_INT32 _integer32, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer32) != _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_INT64 _integer64, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer64) != _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_UINT8 _integer8, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer8) != _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_UINT16 _integer16, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer16) != _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_UINT32 _integer32, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer32) != _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_UINT64 _integer64, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer64) != _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_FLOAT32 _float32, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_float32) != _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_FLOAT64 _float64, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_float64) != _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_INT8 _integer8, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer8) <= _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_INT16 _integer16, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer16) <= _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_INT32 _integer32, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer32) <= _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_INT64 _integer64, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer64) <= _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_UINT8 _integer8, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer8) <= _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_UINT16 _integer16, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer16) <= _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_UINT32 _integer32, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer32) <= _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_UINT64 _integer64, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer64) <= _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_FLOAT32 _float32, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_float32) <= _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_FLOAT64 _float64, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_float64) <= _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_INT8 _integer8, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer8) >= _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_INT16 _integer16, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer16) >= _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_INT32 _integer32, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer32) >= _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_INT64 _integer64, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer64) >= _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_UINT8 _integer8, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer8) >= _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_UINT16 _integer16, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer16) >= _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_UINT32 _integer32, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer32) >= _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_UINT64 _integer64, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer64) >= _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_FLOAT32 _float32, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_float32) >= _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_FLOAT64 _float64, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_float64) >= _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_INT8 _integer8, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer8) < _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_INT16 _integer16, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer16) < _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_INT32 _integer32, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer32) < _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_INT64 _integer64, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer64) < _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_UINT8 _integer8, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer8) < _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_UINT16 _integer16, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer16) < _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_UINT32 _integer32, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer32) < _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_UINT64 _integer64, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer64) < _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_FLOAT32 _float32, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_float32) < _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_FLOAT64 _float64, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_float64) < _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_INT8 _integer8, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer8) > _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_INT16 _integer16, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer16) > _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_INT32 _integer32, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer32) > _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_INT64 _integer64, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer64) > _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_UINT8 _integer8, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer8) > _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_UINT16 _integer16, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer16) > _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_UINT32 _integer32, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer32) > _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_UINT64 _integer64, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer64) > _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_FLOAT32 _float32, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_float32) > _fixed16);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_FLOAT64 _float64, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_float64) > _fixed16);
}

WN_FORCE_INLINE WN_INT8& operator += (WN_INT8& _integer8, const __WNFixed16& _fixed16) {
    _integer8 += static_cast<WN_INT8>(_fixed16);

    return(_integer8);
}

WN_FORCE_INLINE WN_INT16& operator += (WN_INT16& _integer16, const __WNFixed16& _fixed16) {
    _integer16 += static_cast<WN_INT16>(_fixed16);

    return(_integer16);
}

WN_FORCE_INLINE WN_INT32& operator += (WN_INT32& _integer32, const __WNFixed16& _fixed16) {
    _integer32 += static_cast<WN_INT32>(_fixed16);

    return(_integer32);
}

WN_FORCE_INLINE WN_INT64& operator += (WN_INT64& _integer64, const __WNFixed16& _fixed16) {
    _integer64 += static_cast<WN_INT64>(_fixed16);

    return(_integer64);
}

WN_FORCE_INLINE WN_UINT8& operator += (WN_UINT8& _integer8, const __WNFixed16& _fixed16) {
    _integer8 += static_cast<WN_UINT8>(_fixed16);

    return(_integer8);
}

WN_FORCE_INLINE WN_UINT16& operator += (WN_UINT16& _integer16, const __WNFixed16& _fixed16) {
    _integer16 += static_cast<WN_UINT16>(_fixed16);

    return(_integer16);
}

WN_FORCE_INLINE WN_UINT32& operator += (WN_UINT32& _integer32, const __WNFixed16& _fixed16) {
    _integer32 += static_cast<WN_UINT32>(_fixed16);

    return(_integer32);
}

WN_FORCE_INLINE WN_UINT64& operator += (WN_UINT64& _integer64, const __WNFixed16& _fixed16) {
    _integer64 += static_cast<WN_UINT64>(_fixed16);

    return(_integer64);
}

WN_FORCE_INLINE WN_FLOAT32& operator += (WN_FLOAT32& _float32, const __WNFixed16& _fixed16) {
    _float32 += static_cast<WN_FLOAT32>(_fixed16);

    return(_float32);
}

WN_FORCE_INLINE WN_FLOAT64& operator += (WN_FLOAT64& _float64, const __WNFixed16& _fixed16) {
    _float64 += static_cast<WN_FLOAT64>(_fixed16);

    return(_float64);
}

WN_FORCE_INLINE WN_INT8& operator -= (WN_INT8& _integer8, const __WNFixed16& _fixed16) {
    _integer8 -= static_cast<WN_INT8>(_fixed16);

    return(_integer8);
}

WN_FORCE_INLINE WN_INT16& operator -= (WN_INT16& _integer16, const __WNFixed16& _fixed16) {
    _integer16 -= static_cast<WN_INT16>(_fixed16);

    return(_integer16);
}

WN_FORCE_INLINE WN_INT32& operator -= (WN_INT32& _integer32, const __WNFixed16& _fixed16) {
    _integer32 -= static_cast<WN_INT32>(_fixed16);

    return(_integer32);
}

WN_FORCE_INLINE WN_INT64& operator -= (WN_INT64& _integer64, const __WNFixed16& _fixed16) {
    _integer64 -= static_cast<WN_INT64>(_fixed16);

    return(_integer64);
}

WN_FORCE_INLINE WN_UINT8& operator -= (WN_UINT8& _integer8, const __WNFixed16& _fixed16) {
    _integer8 -= static_cast<WN_UINT8>(_fixed16);

    return(_integer8);
}

WN_FORCE_INLINE WN_UINT16& operator -= (WN_UINT16& _integer16, const __WNFixed16& _fixed16) {
    _integer16 -= static_cast<WN_UINT16>(_fixed16);

    return(_integer16);
}

WN_FORCE_INLINE WN_UINT32& operator -= (WN_UINT32& _integer32, const __WNFixed16& _fixed16) {
    _integer32 -= static_cast<WN_UINT32>(_fixed16);

    return(_integer32);
}

WN_FORCE_INLINE WN_UINT64& operator -= (WN_UINT64& _integer64, const __WNFixed16& _fixed16) {
    _integer64 -= static_cast<WN_UINT64>(_fixed16);

    return(_integer64);
}

WN_FORCE_INLINE WN_FLOAT32& operator -= (WN_FLOAT32& _float32, const __WNFixed16& _fixed16) {
    _float32 -= static_cast<WN_FLOAT32>(_fixed16);

    return(_float32);
}

WN_FORCE_INLINE WN_FLOAT64& operator -= (WN_FLOAT64& _float64, const __WNFixed16& _fixed16) {
    _float64 -= static_cast<WN_FLOAT64>(_fixed16);

    return(_float64);
}

WN_FORCE_INLINE WN_INT8& operator *= (WN_INT8& _integer8, const __WNFixed16& _fixed16) {
    _integer8 *= static_cast<WN_INT8>(_fixed16);

    return(_integer8);
}

WN_FORCE_INLINE WN_INT16& operator *= (WN_INT16& _integer16, const __WNFixed16& _fixed16) {
    _integer16 *= static_cast<WN_INT16>(_fixed16);

    return(_integer16);
}

WN_FORCE_INLINE WN_INT32& operator *= (WN_INT32& _integer32, const __WNFixed16& _fixed16) {
    _integer32 *= static_cast<WN_INT32>(_fixed16);

    return(_integer32);
}

WN_FORCE_INLINE WN_INT64& operator *= (WN_INT64& _integer64, const __WNFixed16& _fixed16) {
    _integer64 *= static_cast<WN_INT64>(_fixed16);

    return(_integer64);
}

WN_FORCE_INLINE WN_UINT8& operator *= (WN_UINT8& _integer8, const __WNFixed16& _fixed16) {
    _integer8 *= static_cast<WN_UINT8>(_fixed16);

    return(_integer8);
}

WN_FORCE_INLINE WN_UINT16& operator *= (WN_UINT16& _integer16, const __WNFixed16& _fixed16) {
    _integer16 *= static_cast<WN_UINT16>(_fixed16);

    return(_integer16);
}

WN_FORCE_INLINE WN_UINT32& operator *= (WN_UINT32& _integer32, const __WNFixed16& _fixed16) {
    _integer32 *= static_cast<WN_UINT32>(_fixed16);

    return(_integer32);
}

WN_FORCE_INLINE WN_UINT64& operator *= (WN_UINT64& _integer64, const __WNFixed16& _fixed16) {
    _integer64 *= static_cast<WN_UINT64>(_fixed16);

    return(_integer64);
}

WN_FORCE_INLINE WN_FLOAT32& operator *= (WN_FLOAT32& _float32, const __WNFixed16& _fixed16) {
    _float32 *= static_cast<WN_FLOAT32>(_fixed16);

    return(_float32);
}

WN_FORCE_INLINE WN_FLOAT64& operator *= (WN_FLOAT64& _float64, const __WNFixed16& _fixed16) {
    _float64 *= static_cast<WN_FLOAT64>(_fixed16);

    return(_float64);
}

WN_FORCE_INLINE WN_INT8& operator /= (WN_INT8& _integer8, const __WNFixed16& _fixed16) {
    _integer8 /= static_cast<WN_INT8>(_fixed16);

    return(_integer8);
}

WN_FORCE_INLINE WN_INT16& operator /= (WN_INT16& _integer16, const __WNFixed16& _fixed16) {
    _integer16 /= static_cast<WN_INT16>(_fixed16);

    return(_integer16);
}

WN_FORCE_INLINE WN_INT32& operator /= (WN_INT32& _integer32, const __WNFixed16& _fixed16) {
    _integer32 /= static_cast<WN_INT32>(_fixed16);

    return(_integer32);
}

WN_FORCE_INLINE WN_INT64& operator /= (WN_INT64& _integer64, const __WNFixed16& _fixed16) {
    _integer64 /= static_cast<WN_INT64>(_fixed16);

    return(_integer64);
}

WN_FORCE_INLINE WN_UINT8& operator /= (WN_UINT8& _integer8, const __WNFixed16& _fixed16) {
    _integer8 /= static_cast<WN_UINT8>(_fixed16);

    return(_integer8);
}

WN_FORCE_INLINE WN_UINT16& operator /= (WN_UINT16& _integer16, const __WNFixed16& _fixed16) {
    _integer16 /= static_cast<WN_UINT16>(_fixed16);

    return(_integer16);
}

WN_FORCE_INLINE WN_UINT32& operator /= (WN_UINT32& _integer32, const __WNFixed16& _fixed16) {
    _integer32 /= static_cast<WN_UINT32>(_fixed16);

    return(_integer32);
}

WN_FORCE_INLINE WN_UINT64& operator /= (WN_UINT64& _integer64, const __WNFixed16& _fixed16) {
    _integer64 /= static_cast<WN_UINT64>(_fixed16);

    return(_integer64);
}

WN_FORCE_INLINE WN_FLOAT32& operator /= (WN_FLOAT32& _float32, const __WNFixed16& _fixed16) {
    _float32 /= static_cast<WN_FLOAT32>(_fixed16);

    return(_float32);
}

WN_FORCE_INLINE WN_FLOAT64& operator /= (WN_FLOAT64& _float64, const __WNFixed16& _fixed16) {
    _float64 /= static_cast<WN_FLOAT64>(_fixed16);

    return(_float64);
}

WN_FORCE_INLINE __WNFixed16 operator + (WN_INT8 _integer8, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer8) + _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator + (WN_INT16 _integer16, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer16) + _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator + (WN_INT32 _integer32, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer32) + _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator + (WN_INT64 _integer64, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer64) + _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator + (WN_UINT8 _integer8, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer8) + _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator + (WN_UINT16 _integer16, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer16) + _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator + (WN_UINT32 _integer32, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer32) + _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator + (WN_UINT64 _integer64, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer64) + _fixed16);
}

WN_FORCE_INLINE WN_FLOAT32 operator + (WN_FLOAT32 _float32, const __WNFixed16& _fixed16) {
    return(_float32 + static_cast<WN_FLOAT32>(_fixed16));
}

WN_FORCE_INLINE WN_FLOAT64 operator + (WN_FLOAT64 _float64, const __WNFixed16& _fixed16) {
    return(_float64 + static_cast<WN_FLOAT64>(_fixed16));
}

WN_FORCE_INLINE __WNFixed16 operator - (WN_INT8 _integer8, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer8) - _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator - (WN_INT16 _integer16, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer16) - _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator - (WN_INT32 _integer32, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer32) - _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator - (WN_INT64 _integer64, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer64) - _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator - (WN_UINT8 _integer8, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer8) - _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator - (WN_UINT16 _integer16, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer16) - _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator - (WN_UINT32 _integer32, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer32) - _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator - (WN_UINT64 _integer64, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer64) - _fixed16);
}

WN_FORCE_INLINE WN_FLOAT32 operator - (WN_FLOAT32 _float32, const __WNFixed16& _fixed16) {
    return(_float32 - static_cast<WN_FLOAT32>(_fixed16));
}

WN_FORCE_INLINE WN_FLOAT64 operator - (WN_FLOAT64 _float64, const __WNFixed16& _fixed16) {
    return(_float64 - static_cast<WN_FLOAT64>(_fixed16));
}

WN_FORCE_INLINE __WNFixed16 operator * (WN_INT8 _integer8, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer8) * _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator * (WN_INT16 _integer16, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer16) * _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator * (WN_INT32 _integer32, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer32) * _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator * (WN_INT64 _integer64, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer64) * _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator * (WN_UINT8 _integer8, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer8) * _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator * (WN_UINT16 _integer16, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer16) * _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator * (WN_UINT32 _integer32, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer32) * _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator * (WN_UINT64 _integer64, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer64) * _fixed16);
}

WN_FORCE_INLINE WN_FLOAT32 operator * (WN_FLOAT32 _float32, const __WNFixed16& _fixed16) {
    return(_float32 * static_cast<WN_FLOAT32>(_fixed16));
}

WN_FORCE_INLINE WN_FLOAT64 operator * (WN_FLOAT64 _float64, const __WNFixed16& _fixed16) {
    return(_float64 * static_cast<WN_FLOAT64>(_fixed16));
}

WN_FORCE_INLINE __WNFixed16 operator / (WN_INT8 _integer8, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer8) / _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator / (WN_INT16 _integer16, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer16) / _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator / (WN_INT32 _integer32, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer32) / _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator / (WN_INT64 _integer64, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer64) / _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator / (WN_UINT8 _integer8, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer8) / _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator / (WN_UINT16 _integer16, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer16) / _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator / (WN_UINT32 _integer32, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer32) / _fixed16);
}

WN_FORCE_INLINE __WNFixed16 operator / (WN_UINT64 _integer64, const __WNFixed16& _fixed16) {
    return(__WNFixed16(_integer64) / _fixed16);
}

WN_FORCE_INLINE WN_FLOAT32 operator / (WN_FLOAT32 _float32, const __WNFixed16& _fixed16) {
    return(_float32 / static_cast<WN_FLOAT32>(_fixed16));
}

WN_FORCE_INLINE WN_FLOAT64 operator / (WN_FLOAT64 _float64, const __WNFixed16& _fixed16) {
    return(_float64 / static_cast<WN_FLOAT64>(_fixed16));
}

// Fixed (32 bit) //////////////////////////////////////////////////////////////////////////////////////////////////////////////

WN_FORCE_INLINE __WNFixed32::__WNFixed32(WN_INT8 _integer8) {
    mBits = static_cast<WN_INT32>(_integer8) << WN_FIXED32_BITS;
}

WN_FORCE_INLINE __WNFixed32::__WNFixed32(WN_INT16 _integer16) {
    mBits = static_cast<WN_INT32>(_integer16) << WN_FIXED32_BITS;
}

WN_FORCE_INLINE __WNFixed32::__WNFixed32(WN_INT32 _integer32) {
    mBits = _integer32 << WN_FIXED32_BITS;
}

WN_FORCE_INLINE __WNFixed32::__WNFixed32(WN_INT64 _integer64) {
    mBits = static_cast<WN_INT32>(_integer64) << WN_FIXED32_BITS;
}

WN_FORCE_INLINE __WNFixed32::__WNFixed32(WN_UINT8 _integer8) {
    mBits = static_cast<WN_INT32>(_integer8) << WN_FIXED32_BITS;
}

WN_FORCE_INLINE __WNFixed32::__WNFixed32(WN_UINT16 _integer16) {
    mBits = static_cast<WN_INT32>(_integer16) << WN_FIXED32_BITS;
}

WN_FORCE_INLINE __WNFixed32::__WNFixed32(WN_UINT32 _integer32) {
    mBits = static_cast<WN_INT32>(_integer32) << WN_FIXED32_BITS;
}

WN_FORCE_INLINE __WNFixed32::__WNFixed32(WN_UINT64 _integer64) {
    mBits = static_cast<WN_INT32>(_integer64) << WN_FIXED32_BITS;
}

WN_FORCE_INLINE __WNFixed32::__WNFixed32(const __WNFixed8& _fixed8) {
    mBits = static_cast<WN_INT32>(_fixed8.mBits) << (WN_FIXED32_BITS - WN_FIXED8_BITS);
}

WN_FORCE_INLINE __WNFixed32::__WNFixed32(const __WNFixed16& _fixed16) {
    mBits = static_cast<WN_INT32>(_fixed16.mBits) << (WN_FIXED32_BITS - WN_FIXED16_BITS);
}

WN_FORCE_INLINE __WNFixed32::__WNFixed32(const __WNFixed32& _fixed32) {
    mBits = _fixed32.mBits;
}

WN_FORCE_INLINE __WNFixed32::__WNFixed32(const __WNFixed64& _fixed64) {
    mBits = static_cast<WN_INT32>(_fixed64.mBits >> (WN_FIXED64_BITS - WN_FIXED32_BITS));
}

WN_FORCE_INLINE __WNFixed32::__WNFixed32(const __WNFloat8& _float8) {
    const WN_FLOAT32 float32 = static_cast<WN_FLOAT32>(_float8);

    mBits = static_cast<WN_INT32>((float32 * __WN_FIXED32_ONE) + (float32 >= 0.0f ? 0.5f : -0.5f));
}

WN_FORCE_INLINE __WNFixed32::__WNFixed32(const __WNFloat16& _float16) {
    const WN_FLOAT32 float32 = static_cast<WN_FLOAT32>(_float16);

    mBits = static_cast<WN_INT32>((float32 * __WN_FIXED32_ONE) + (float32 >= 0.0f ? 0.5f : -0.5f));
}

WN_FORCE_INLINE __WNFixed32::__WNFixed32(WN_FLOAT32 _float32) {
    mBits = static_cast<WN_INT32>((_float32 * __WN_FIXED32_ONE) + (_float32 >= 0.0f ? 0.5f : -0.5f));
}

WN_FORCE_INLINE __WNFixed32::__WNFixed32(WN_FLOAT64 _float64) {
    mBits = static_cast<WN_INT32>((_float64 * __WN_FIXED32_ONE) + (_float64 >= 0.0 ? 0.5 : -0.5));
}

WN_FORCE_INLINE __WNFixed32::operator WN_INT8 () const {
    return(static_cast<WN_INT8>(static_cast<WN_INT32>(*this)));
}

WN_FORCE_INLINE __WNFixed32::operator WN_INT16 () const {
    return(static_cast<WN_INT16>(static_cast<WN_INT32>(*this)));
}

WN_FORCE_INLINE __WNFixed32::operator WN_INT32 () const {
    return(mBits >> WN_FIXED32_BITS);
}

WN_FORCE_INLINE __WNFixed32::operator WN_INT64 () const {
    return(static_cast<WN_INT64>(static_cast<WN_INT32>(*this)));
}

WN_FORCE_INLINE __WNFixed32::operator WN_UINT8 () const {
    return(static_cast<WN_UINT8>(static_cast<WN_INT32>(*this)));
}

WN_FORCE_INLINE __WNFixed32::operator WN_UINT16 () const {
    return(static_cast<WN_UINT16>(static_cast<WN_INT32>(*this)));
}

WN_FORCE_INLINE __WNFixed32::operator WN_UINT32 () const {
    return(static_cast<WN_UINT32>(static_cast<WN_INT32>(*this)));
}

WN_FORCE_INLINE __WNFixed32::operator WN_UINT64 () const {
    return(static_cast<WN_UINT64>(static_cast<WN_INT32>(*this)));
}

WN_FORCE_INLINE __WNFixed32::operator WN_FLOAT32 () const {
    return(static_cast<WN_FLOAT32>(mBits) / __WN_FIXED32_ONE);
}

WN_FORCE_INLINE __WNFixed32::operator WN_FLOAT64 () const {
    return(static_cast<WN_FLOAT64>(static_cast<WN_FLOAT32>(*this)));
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator = (WN_INT8 _integer8) {
    *this = __WNFixed32(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator = (WN_INT16 _integer16) {
    *this = __WNFixed32(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator = (WN_INT32 _integer32) {
    *this = __WNFixed32(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator = (WN_INT64 _integer64) {
    *this = __WNFixed32(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator = (WN_UINT8 _integer8) {
    *this = __WNFixed32(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator = (WN_UINT16 _integer16) {
    *this = __WNFixed32(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator = (WN_UINT32 _integer32) {
    *this = __WNFixed32(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator = (WN_UINT64 _integer64) {
    *this = __WNFixed32(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator = (const __WNFixed8& _fixed8) {
    *this = __WNFixed32(_fixed8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator = (const __WNFixed16& _fixed16) {
    *this = __WNFixed32(_fixed16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator = (const __WNFixed32& _fixed32) {
    mBits = _fixed32.mBits;

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator = (const __WNFixed64& _fixed64) {
    *this = __WNFixed32(_fixed64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator = (const __WNFloat8& _float8) {
    *this = __WNFixed32(_float8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator = (const __WNFloat16& _float16) {
    *this = __WNFixed32(_float16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator = (WN_FLOAT32 _float32) {
    *this = __WNFixed32(_float32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator = (WN_FLOAT64 _float64) {
    *this = __WNFixed32(_float64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator + () const {
    return(*this);
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator - () const {
    __WNFixed32 result;

    result.mBits = -mBits;

    return(result);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator ++ () {
    mBits += __WN_FIXED32_ONE;

    return(*this);
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator ++ (WN_INT32) {
    const __WNFixed32 result = *this;

    ++(*this);

    return(result);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator -- () {
    mBits -= __WN_FIXED32_ONE;

    return(*this);
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator -- (WN_INT32) {
    const __WNFixed32 result = *this;

    --(*this);

    return(result);
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator ! () const {
    return(!mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator == (WN_INT8 _integer8) const {
    return(*this == __WNFixed32(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator == (WN_INT16 _integer16) const {
    return(*this == __WNFixed32(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator == (WN_INT32 _integer32) const {
    return(*this == __WNFixed32(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator == (WN_INT64 _integer64) const {
    return(*this == __WNFixed32(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator == (WN_UINT8 _integer8) const {
    return(*this == __WNFixed32(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator == (WN_UINT16 _integer16) const {
    return(*this == __WNFixed32(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator == (WN_UINT32 _integer32) const {
    return(*this == __WNFixed32(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator == (WN_UINT64 _integer64) const {
    return(*this == __WNFixed32(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator == (const __WNFixed8& _fixed8) const {
    return(*this == __WNFixed32(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator == (const __WNFixed16& _fixed16) const {
    return(*this == __WNFixed32(_fixed16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator == (const __WNFixed32& _fixed32) const {
    return(mBits == _fixed32.mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator == (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) == _fixed64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator == (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) == _float8);
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator == (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) == _float16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator == (WN_FLOAT32 _float32) const {
    return(*this == __WNFixed32(_float32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator == (WN_FLOAT64 _float64) const {
    return(*this == __WNFixed32(_float64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator != (WN_INT8 _integer8) const {
    return(*this != __WNFixed32(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator != (WN_INT16 _integer16) const {
    return(*this != __WNFixed32(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator != (WN_INT32 _integer32) const {
    return(*this != __WNFixed32(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator != (WN_INT64 _integer64) const {
    return(*this != __WNFixed32(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator != (WN_UINT8 _integer8) const {
    return(*this != __WNFixed32(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator != (WN_UINT16 _integer16) const {
    return(*this != __WNFixed32(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator != (WN_UINT32 _integer32) const {
    return(*this != __WNFixed32(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator != (WN_UINT64 _integer64) const {
    return(*this != __WNFixed32(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator != (const __WNFixed8& _fixed8) const {
    return(*this != __WNFixed32(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator != (const __WNFixed16& _fixed16) const {
    return(*this != __WNFixed32(_fixed16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator != (const __WNFixed32& _fixed32) const {
    return(mBits != _fixed32.mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator != (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) != _fixed64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator != (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) != _float8);
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator != (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) != _float16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator != (WN_FLOAT32 _float32) const {
    return(*this != __WNFixed32(_float32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator != (WN_FLOAT64 _float64) const {
    return(*this != __WNFixed32(_float64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator <= (WN_INT8 _integer8) const {
    return(*this <= __WNFixed32(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator <= (WN_INT16 _integer16) const {
    return(*this <= __WNFixed32(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator <= (WN_INT32 _integer32) const {
    return(*this <= __WNFixed32(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator <= (WN_INT64 _integer64) const {
    return(*this <= __WNFixed32(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator <= (WN_UINT8 _integer8) const {
    return(*this <= __WNFixed32(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator <= (WN_UINT16 _integer16) const {
    return(*this <= __WNFixed32(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator <= (WN_UINT32 _integer32) const {
    return(*this <= __WNFixed32(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator <= (WN_UINT64 _integer64) const {
    return(*this <= __WNFixed32(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator <= (const __WNFixed8& _fixed8) const {
    return(*this <= __WNFixed32(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator <= (const __WNFixed16& _fixed16) const {
    return(*this <= __WNFixed32(_fixed16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator <= (const __WNFixed32& _fixed32) const {
    return(mBits <= _fixed32.mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator <= (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) <= _fixed64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator <= (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) <= _float8);
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator <= (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) <= _float16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator <= (WN_FLOAT32 _float32) const {
    return(*this <= __WNFixed32(_float32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator <= (WN_FLOAT64 _float64) const {
    return(*this <= __WNFixed32(_float64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator >= (WN_INT8 _integer8) const {
    return(*this >= __WNFixed32(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator >= (WN_INT16 _integer16) const {
    return(*this >= __WNFixed32(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator >= (WN_INT32 _integer32) const {
    return(*this >= __WNFixed32(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator >= (WN_INT64 _integer64) const {
    return(*this >= __WNFixed32(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator >= (WN_UINT8 _integer8) const {
    return(*this >= __WNFixed32(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator >= (WN_UINT16 _integer16) const {
    return(*this >= __WNFixed32(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator >= (WN_UINT32 _integer32) const {
    return(*this >= __WNFixed32(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator >= (WN_UINT64 _integer64) const {
    return(*this >= __WNFixed32(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator >= (const __WNFixed8& _fixed8) const {
    return(*this >= __WNFixed32(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator >= (const __WNFixed16& _fixed16) const {
    return(*this >= __WNFixed32(_fixed16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator >= (const __WNFixed32& _fixed32) const {
    return(mBits >= _fixed32.mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator >= (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) >= _fixed64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator >= (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) >= _float8);
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator >= (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) >= _float16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator >= (WN_FLOAT32 _float32) const {
    return(*this >= __WNFixed32(_float32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator >= (WN_FLOAT64 _float64) const {
    return(*this >= __WNFixed32(_float64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator < (WN_INT8 _integer8) const {
    return(*this < __WNFixed32(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator < (WN_INT16 _integer16) const {
    return(*this < __WNFixed32(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator < (WN_INT32 _integer32) const {
    return(*this < __WNFixed32(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator < (WN_INT64 _integer64) const {
    return(*this < __WNFixed32(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator < (WN_UINT8 _integer8) const {
    return(*this < __WNFixed32(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator < (WN_UINT16 _integer16) const {
    return(*this < __WNFixed32(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator < (WN_UINT32 _integer32) const {
    return(*this < __WNFixed32(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator < (WN_UINT64 _integer64) const {
    return(*this < __WNFixed32(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator < (const __WNFixed8& _fixed8) const {
    return(*this < __WNFixed32(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator < (const __WNFixed16& _fixed16) const {
    return(*this < __WNFixed32(_fixed16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator < (const __WNFixed32& _fixed32) const {
    return(mBits < _fixed32.mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator < (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) < _fixed64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator < (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) < _float8);
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator < (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) < _float16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator < (WN_FLOAT32 _float32) const {
    return(*this < __WNFixed32(_float32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator < (WN_FLOAT64 _float64) const {
    return(*this < __WNFixed32(_float64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator > (WN_INT8 _integer8) const {
    return(*this > __WNFixed32(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator > (WN_INT16 _integer16) const {
    return(*this > __WNFixed32(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator > (WN_INT32 _integer32) const {
    return(*this > __WNFixed32(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator > (WN_INT64 _integer64) const {
    return(*this > __WNFixed32(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator > (WN_UINT8 _integer8) const {
    return(*this > __WNFixed32(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator > (WN_UINT16 _integer16) const {
    return(*this > __WNFixed32(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator > (WN_UINT32 _integer32) const {
    return(*this > __WNFixed32(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator > (WN_UINT64 _integer64) const {
    return(*this > __WNFixed32(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator > (const __WNFixed8& _fixed8) const {
    return(*this > __WNFixed32(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator > (const __WNFixed16& _fixed16) const {
    return(*this > __WNFixed32(_fixed16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator > (const __WNFixed32& _fixed32) const {
    return(mBits > _fixed32.mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator > (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) > _fixed64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator > (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) > _float8);
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator > (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) > _float16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator > (WN_FLOAT32 _float32) const {
    return(*this > __WNFixed32(_float32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed32::operator > (WN_FLOAT64 _float64) const {
    return(*this > __WNFixed32(_float64));
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator += (WN_INT8 _integer8) {
    *this += __WNFixed32(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator += (WN_INT16 _integer16) {
    *this += __WNFixed32(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator += (WN_INT32 _integer32) {
    *this += __WNFixed32(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator += (WN_INT64 _integer64) {
    *this += __WNFixed32(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator += (WN_UINT8 _integer8) {
    *this += __WNFixed32(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator += (WN_UINT16 _integer16) {
    *this += __WNFixed32(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator += (WN_UINT32 _integer32) {
    *this += __WNFixed32(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator += (WN_UINT64 _integer64) {
    *this += __WNFixed32(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator += (const __WNFixed8& _fixed8) {
    *this += __WNFixed32(_fixed8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator += (const __WNFixed16& _fixed16) {
    *this += __WNFixed32(_fixed16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator += (const __WNFixed32& _fixed32) {
    mBits += _fixed32.mBits;

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator += (const __WNFixed64& _fixed64) {
    *this += __WNFixed32(_fixed64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator += (const __WNFloat8& _float8) {
    *this += __WNFixed32(_float8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator += (const __WNFloat16& _float16) {
    *this += __WNFixed32(_float16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator += (WN_FLOAT32 _float32) {
    *this += __WNFixed32(_float32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator += (WN_FLOAT64 _float64) {
    *this += __WNFixed32(_float64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator -= (WN_INT8 _integer8) {
    *this -= __WNFixed32(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator -= (WN_INT16 _integer16) {
    *this -= __WNFixed32(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator -= (WN_INT32 _integer32) {
    *this -= __WNFixed32(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator -= (WN_INT64 _integer64) {
    *this -= __WNFixed32(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator -= (WN_UINT8 _integer8) {
    *this -= __WNFixed32(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator -= (WN_UINT16 _integer16) {
    *this -= __WNFixed32(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator -= (WN_UINT32 _integer32) {
    *this -= __WNFixed32(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator -= (WN_UINT64 _integer64) {
    *this -= __WNFixed32(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator -= (const __WNFixed8& _fixed8) {
    *this -= __WNFixed32(_fixed8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator -= (const __WNFixed16& _fixed16) {
    *this -= __WNFixed32(_fixed16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator -= (const __WNFixed32& _fixed32) {
    mBits -= _fixed32.mBits;

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator -= (const __WNFixed64& _fixed64) {
    *this -= __WNFixed32(_fixed64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator -= (const __WNFloat8& _float8) {
    *this -= __WNFixed32(_float8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator -= (const __WNFloat16& _float16) {
    *this -= __WNFixed32(_float16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator -= (WN_FLOAT32 _float32) {
    *this -= __WNFixed32(_float32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator -= (WN_FLOAT64 _float64) {
    *this -= __WNFixed32(_float64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator *= (WN_INT8 _integer8) {
    *this *= __WNFixed32(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator *= (WN_INT16 _integer16) {
    *this *= __WNFixed32(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator *= (WN_INT32 _integer32) {
    *this *= __WNFixed32(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator *= (WN_INT64 _integer64) {
    *this *= __WNFixed32(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator *= (WN_UINT8 _integer8) {
    *this *= __WNFixed32(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator *= (WN_UINT16 _integer16) {
    *this *= __WNFixed32(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator *= (WN_UINT32 _integer32) {
    *this *= __WNFixed32(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator *= (WN_UINT64 _integer64) {
    *this *= __WNFixed32(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator *= (const __WNFixed8& _fixed8) {
    *this *= __WNFixed32(_fixed8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator *= (const __WNFixed16& _fixed16) {
    *this *= __WNFixed32(_fixed16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator *= (const __WNFixed32& _fixed32) {
    mBits = static_cast<WN_INT32>((static_cast<WN_INT64>(mBits) * static_cast<WN_INT64>(_fixed32.mBits)) >> WN_FIXED32_BITS);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator *= (const __WNFixed64& _fixed64) {
    *this *= __WNFixed32(_fixed64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator *= (const __WNFloat8& _float8) {
    *this *= __WNFixed32(_float8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator *= (const __WNFloat16& _float16) {
    *this *= __WNFixed32(_float16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator *= (WN_FLOAT32 _float32) {
    *this *= __WNFixed32(_float32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator *= (WN_FLOAT64 _float64) {
    *this *= __WNFixed32(_float64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator /= (WN_INT8 _integer8) {
    *this /= __WNFixed32(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator /= (WN_INT16 _integer16) {
    *this /= __WNFixed32(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator /= (WN_INT32 _integer32) {
    *this /= __WNFixed32(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator /= (WN_INT64 _integer64) {
    *this /= __WNFixed32(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator /= (WN_UINT8 _integer8) {
    *this /= __WNFixed32(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator /= (WN_UINT16 _integer16) {
    *this /= __WNFixed32(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator /= (WN_UINT32 _integer32) {
    *this /= __WNFixed32(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator /= (WN_UINT64 _integer64) {
    *this /= __WNFixed32(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator /= (const __WNFixed8& _fixed8) {
    *this /= __WNFixed32(_fixed8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator /= (const __WNFixed16& _fixed16) {
    *this /= __WNFixed32(_fixed16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator /= (const __WNFixed32& _fixed32) {
    mBits = static_cast<WN_INT32>((static_cast<WN_INT64>(mBits) << WN_FIXED32_BITS) / _fixed32.mBits);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator /= (const __WNFixed64& _fixed64) {
    *this /= __WNFixed32(_fixed64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator /= (const __WNFloat8& _float8) {
    *this /= __WNFixed32(_float8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator /= (const __WNFloat16& _float16) {
    *this /= __WNFixed32(_float16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator /= (WN_FLOAT32 _float32) {
    *this /= __WNFixed32(_float32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32& __WNFixed32::operator /= (WN_FLOAT64 _float64) {
    *this /= __WNFixed32(_float64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator + (WN_INT8 _integer8) const {
    return(*this + __WNFixed32(_integer8));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator + (WN_INT16 _integer16) const {
    return(*this + __WNFixed32(_integer16));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator + (WN_INT32 _integer32) const {
    return(*this + __WNFixed32(_integer32));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator + (WN_INT64 _integer64) const {
    return(*this + __WNFixed32(_integer64));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator + (WN_UINT8 _integer8) const {
    return(*this + __WNFixed32(_integer8));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator + (WN_UINT16 _integer16) const {
    return(*this + __WNFixed32(_integer16));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator + (WN_UINT32 _integer32) const {
    return(*this + __WNFixed32(_integer32));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator + (WN_UINT64 _integer64) const {
    return(*this + __WNFixed32(_integer64));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator + (const __WNFixed8& _fixed8) const {
    return(*this + __WNFixed32(_fixed8));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator + (const __WNFixed16& _fixed16) const {
    return(*this + __WNFixed32(_fixed16));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator + (const __WNFixed32& _fixed32) const {
    return(__WNFixed32(*this) += _fixed32);
}

WN_FORCE_INLINE __WNFixed64 __WNFixed32::operator + (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) + _fixed64);
}

WN_FORCE_INLINE __WNFloat8 __WNFixed32::operator + (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) + _float8);
}

WN_FORCE_INLINE __WNFloat16 __WNFixed32::operator + (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) + _float16);
}

WN_FORCE_INLINE WN_FLOAT32 __WNFixed32::operator + (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) + _float32);
}

WN_FORCE_INLINE WN_FLOAT64 __WNFixed32::operator + (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) + _float64);
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator - (WN_INT8 _integer8) const {
    return(*this - __WNFixed32(_integer8));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator - (WN_INT16 _integer16) const {
    return(*this - __WNFixed32(_integer16));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator - (WN_INT32 _integer32) const {
    return(*this - __WNFixed32(_integer32));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator - (WN_INT64 _integer64) const {
    return(*this - __WNFixed32(_integer64));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator - (WN_UINT8 _integer8) const {
    return(*this - __WNFixed32(_integer8));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator - (WN_UINT16 _integer16) const {
    return(*this - __WNFixed32(_integer16));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator - (WN_UINT32 _integer32) const {
    return(*this - __WNFixed32(_integer32));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator - (WN_UINT64 _integer64) const {
    return(*this - __WNFixed32(_integer64));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator - (const __WNFixed8& _fixed8) const {
    return(*this - __WNFixed32(_fixed8));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator - (const __WNFixed16& _fixed16) const {
    return(*this - __WNFixed32(_fixed16));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator - (const __WNFixed32& _fixed32) const {
    return(__WNFixed32(*this) -= _fixed32);
}

WN_FORCE_INLINE __WNFixed64 __WNFixed32::operator - (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) - _fixed64);
}

WN_FORCE_INLINE __WNFloat8 __WNFixed32::operator - (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) - _float8);
}

WN_FORCE_INLINE __WNFloat16 __WNFixed32::operator - (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) - _float16);
}

WN_FORCE_INLINE WN_FLOAT32 __WNFixed32::operator - (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) - _float32);
}

WN_FORCE_INLINE WN_FLOAT64 __WNFixed32::operator - (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) - _float64);
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator * (WN_INT8 _integer8) const {
    return(*this * __WNFixed32(_integer8));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator * (WN_INT16 _integer16) const {
    return(*this * __WNFixed32(_integer16));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator * (WN_INT32 _integer32) const {
    return(*this * __WNFixed32(_integer32));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator * (WN_INT64 _integer64) const {
    return(*this * __WNFixed32(_integer64));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator * (WN_UINT8 _integer8) const {
    return(*this * __WNFixed32(_integer8));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator * (WN_UINT16 _integer16) const {
    return(*this * __WNFixed32(_integer16));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator * (WN_UINT32 _integer32) const {
    return(*this * __WNFixed32(_integer32));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator * (WN_UINT64 _integer64) const {
    return(*this * __WNFixed32(_integer64));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator * (const __WNFixed8& _fixed8) const {
    return(*this * __WNFixed32(_fixed8));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator * (const __WNFixed16& _fixed16) const {
    return(*this * __WNFixed32(_fixed16));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator * (const __WNFixed32& _fixed32) const {
    return(__WNFixed32(*this) *= _fixed32);
}

WN_FORCE_INLINE __WNFixed64 __WNFixed32::operator * (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) * _fixed64);
}

WN_FORCE_INLINE __WNFloat8 __WNFixed32::operator * (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) * _float8);
}

WN_FORCE_INLINE __WNFloat16 __WNFixed32::operator * (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) * _float16);
}

WN_FORCE_INLINE WN_FLOAT32 __WNFixed32::operator * (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) * _float32);
}

WN_FORCE_INLINE WN_FLOAT64 __WNFixed32::operator * (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) * _float64);
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator / (WN_INT8 _integer8) const {
    return(*this / __WNFixed32(_integer8));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator / (WN_INT16 _integer16) const {
    return(*this / __WNFixed32(_integer16));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator / (WN_INT32 _integer32) const {
    return(*this / __WNFixed32(_integer32));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator / (WN_INT64 _integer64) const {
    return(*this / __WNFixed32(_integer64));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator / (WN_UINT8 _integer8) const {
    return(*this / __WNFixed32(_integer8));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator / (WN_UINT16 _integer16) const {
    return(*this / __WNFixed32(_integer16));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator / (WN_UINT32 _integer32) const {
    return(*this / __WNFixed32(_integer32));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator / (WN_UINT64 _integer64) const {
    return(*this / __WNFixed32(_integer64));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator / (const __WNFixed8& _fixed8) const {
    return(*this / __WNFixed32(_fixed8));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator / (const __WNFixed16& _fixed16) const {
    return(*this / __WNFixed32(_fixed16));
}

WN_FORCE_INLINE __WNFixed32 __WNFixed32::operator / (const __WNFixed32& _fixed32) const {
    return(__WNFixed32(*this) /= _fixed32);
}

WN_FORCE_INLINE __WNFixed64 __WNFixed32::operator / (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) / _fixed64);
}

WN_FORCE_INLINE __WNFloat8 __WNFixed32::operator / (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) / _float8);
}

WN_FORCE_INLINE __WNFloat16 __WNFixed32::operator / (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) / _float16);
}

WN_FORCE_INLINE WN_FLOAT32 __WNFixed32::operator / (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) / _float32);
}

WN_FORCE_INLINE WN_FLOAT64 __WNFixed32::operator / (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) / _float64);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_INT8 _integer8, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer8) == _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_INT16 _integer16, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer16) == _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_INT32 _integer32, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer32) == _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_INT64 _integer64, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer64) == _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_UINT8 _integer8, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer8) == _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_UINT16 _integer16, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer16) == _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_UINT32 _integer32, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer32) == _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_UINT64 _integer64, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer64) == _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_FLOAT32 _float32, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_float32) == _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator == (WN_FLOAT64 _float64, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_float64) == _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_INT8 _integer8, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer8) != _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_INT16 _integer16, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer16) != _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_INT32 _integer32, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer32) != _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_INT64 _integer64, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer64) != _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_UINT8 _integer8, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer8) != _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_UINT16 _integer16, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer16) != _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_UINT32 _integer32, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer32) != _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_UINT64 _integer64, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer64) != _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_FLOAT32 _float32, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_float32) != _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator != (WN_FLOAT64 _float64, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_float64) != _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_INT8 _integer8, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer8) <= _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_INT16 _integer16, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer16) <= _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_INT32 _integer32, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer32) <= _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_INT64 _integer64, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer64) <= _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_UINT8 _integer8, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer8) <= _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_UINT16 _integer16, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer16) <= _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_UINT32 _integer32, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer32) <= _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_UINT64 _integer64, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer64) <= _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_FLOAT32 _float32, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_float32) <= _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator <= (WN_FLOAT64 _float64, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_float64) <= _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_INT8 _integer8, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer8) >= _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_INT16 _integer16, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer16) >= _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_INT32 _integer32, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer32) >= _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_INT64 _integer64, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer64) >= _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_UINT8 _integer8, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer8) >= _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_UINT16 _integer16, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer16) >= _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_UINT32 _integer32, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer32) >= _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_UINT64 _integer64, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer64) >= _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_FLOAT32 _float32, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_float32) >= _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator >= (WN_FLOAT64 _float64, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_float64) >= _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_INT8 _integer8, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer8) < _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_INT16 _integer16, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer16) < _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_INT32 _integer32, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer32) < _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_INT64 _integer64, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer64) < _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_UINT8 _integer8, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer8) < _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_UINT16 _integer16, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer16) < _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_UINT32 _integer32, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer32) < _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_UINT64 _integer64, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer64) < _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_FLOAT32 _float32, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_float32) < _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator < (WN_FLOAT64 _float64, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_float64) < _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_INT8 _integer8, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer8) > _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_INT16 _integer16, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer16) > _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_INT32 _integer32, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer32) > _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_INT64 _integer64, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer64) > _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_UINT8 _integer8, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer8) > _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_UINT16 _integer16, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer16) > _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_UINT32 _integer32, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer32) > _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_UINT64 _integer64, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer64) > _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_FLOAT32 _float32, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_float32) > _fixed32);
}

WN_FORCE_INLINE WN_BOOL operator > (WN_FLOAT64 _float64, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_float64) > _fixed32);
}

WN_FORCE_INLINE WN_INT8& operator += (WN_INT8& _integer8, const __WNFixed32& _fixed32) {
    _integer8 += static_cast<WN_INT8>(_fixed32);

    return(_integer8);
}

WN_FORCE_INLINE WN_INT16& operator += (WN_INT16& _integer16, const __WNFixed32& _fixed32) {
    _integer16 += static_cast<WN_INT16>(_fixed32);

    return(_integer16);
}

WN_FORCE_INLINE WN_INT32& operator += (WN_INT32& _integer32, const __WNFixed32& _fixed32) {
    _integer32 += static_cast<WN_INT32>(_fixed32);

    return(_integer32);
}

WN_FORCE_INLINE WN_INT64& operator += (WN_INT64& _integer64, const __WNFixed32& _fixed32) {
    _integer64 += static_cast<WN_INT64>(_fixed32);

    return(_integer64);
}

WN_FORCE_INLINE WN_UINT8& operator += (WN_UINT8& _integer8, const __WNFixed32& _fixed32) {
    _integer8 += static_cast<WN_UINT8>(_fixed32);

    return(_integer8);
}

WN_FORCE_INLINE WN_UINT16& operator += (WN_UINT16& _integer16, const __WNFixed32& _fixed32) {
    _integer16 += static_cast<WN_UINT16>(_fixed32);

    return(_integer16);
}

WN_FORCE_INLINE WN_UINT32& operator += (WN_UINT32& _integer32, const __WNFixed32& _fixed32) {
    _integer32 += static_cast<WN_UINT32>(_fixed32);

    return(_integer32);
}

WN_FORCE_INLINE WN_UINT64& operator += (WN_UINT64& _integer64, const __WNFixed32& _fixed32) {
    _integer64 += static_cast<WN_UINT64>(_fixed32);

    return(_integer64);
}

WN_FORCE_INLINE WN_FLOAT32& operator += (WN_FLOAT32& _float32, const __WNFixed32& _fixed32) {
    _float32 += static_cast<WN_FLOAT32>(_fixed32);

    return(_float32);
}

WN_FORCE_INLINE WN_FLOAT64& operator += (WN_FLOAT64& _float64, const __WNFixed32& _fixed32) {
    _float64 += static_cast<WN_FLOAT64>(_fixed32);

    return(_float64);
}

WN_FORCE_INLINE WN_INT8& operator -= (WN_INT8& _integer8, const __WNFixed32& _fixed32) {
    _integer8 -= static_cast<WN_INT8>(_fixed32);

    return(_integer8);
}

WN_FORCE_INLINE WN_INT16& operator -= (WN_INT16& _integer16, const __WNFixed32& _fixed32) {
    _integer16 -= static_cast<WN_INT16>(_fixed32);

    return(_integer16);
}

WN_FORCE_INLINE WN_INT32& operator -= (WN_INT32& _integer32, const __WNFixed32& _fixed32) {
    _integer32 -= static_cast<WN_INT32>(_fixed32);

    return(_integer32);
}

WN_FORCE_INLINE WN_INT64& operator -= (WN_INT64& _integer64, const __WNFixed32& _fixed32) {
    _integer64 -= static_cast<WN_INT64>(_fixed32);

    return(_integer64);
}

WN_FORCE_INLINE WN_UINT8& operator -= (WN_UINT8& _integer8, const __WNFixed32& _fixed32) {
    _integer8 -= static_cast<WN_UINT8>(_fixed32);

    return(_integer8);
}

WN_FORCE_INLINE WN_UINT16& operator -= (WN_UINT16& _integer16, const __WNFixed32& _fixed32) {
    _integer16 -= static_cast<WN_UINT16>(_fixed32);

    return(_integer16);
}

WN_FORCE_INLINE WN_UINT32& operator -= (WN_UINT32& _integer32, const __WNFixed32& _fixed32) {
    _integer32 -= static_cast<WN_UINT32>(_fixed32);

    return(_integer32);
}

WN_FORCE_INLINE WN_UINT64& operator -= (WN_UINT64& _integer64, const __WNFixed32& _fixed32) {
    _integer64 -= static_cast<WN_UINT64>(_fixed32);

    return(_integer64);
}

WN_FORCE_INLINE WN_FLOAT32& operator -= (WN_FLOAT32& _float32, const __WNFixed32& _fixed32) {
    _float32 -= static_cast<WN_FLOAT32>(_fixed32);

    return(_float32);
}

WN_FORCE_INLINE WN_FLOAT64& operator -= (WN_FLOAT64& _float64, const __WNFixed32& _fixed32) {
    _float64 -= static_cast<WN_FLOAT64>(_fixed32);

    return(_float64);
}

WN_FORCE_INLINE WN_INT8& operator *= (WN_INT8& _integer8, const __WNFixed32& _fixed32) {
    _integer8 *= static_cast<WN_INT8>(_fixed32);

    return(_integer8);
}

WN_FORCE_INLINE WN_INT16& operator *= (WN_INT16& _integer16, const __WNFixed32& _fixed32) {
    _integer16 *= static_cast<WN_INT16>(_fixed32);

    return(_integer16);
}

WN_FORCE_INLINE WN_INT32& operator *= (WN_INT32& _integer32, const __WNFixed32& _fixed32) {
    _integer32 *= static_cast<WN_INT32>(_fixed32);

    return(_integer32);
}

WN_FORCE_INLINE WN_INT64& operator *= (WN_INT64& _integer64, const __WNFixed32& _fixed32) {
    _integer64 *= static_cast<WN_INT64>(_fixed32);

    return(_integer64);
}

WN_FORCE_INLINE WN_UINT8& operator *= (WN_UINT8& _integer8, const __WNFixed32& _fixed32) {
    _integer8 *= static_cast<WN_UINT8>(_fixed32);

    return(_integer8);
}

WN_FORCE_INLINE WN_UINT16& operator *= (WN_UINT16& _integer16, const __WNFixed32& _fixed32) {
    _integer16 *= static_cast<WN_UINT16>(_fixed32);

    return(_integer16);
}

WN_FORCE_INLINE WN_UINT32& operator *= (WN_UINT32& _integer32, const __WNFixed32& _fixed32) {
    _integer32 *= static_cast<WN_UINT32>(_fixed32);

    return(_integer32);
}

WN_FORCE_INLINE WN_UINT64& operator *= (WN_UINT64& _integer64, const __WNFixed32& _fixed32) {
    _integer64 *= static_cast<WN_UINT64>(_fixed32);

    return(_integer64);
}

WN_FORCE_INLINE WN_FLOAT32& operator *= (WN_FLOAT32& _float32, const __WNFixed32& _fixed32) {
    _float32 *= static_cast<WN_FLOAT32>(_fixed32);

    return(_float32);
}

WN_FORCE_INLINE WN_FLOAT64& operator *= (WN_FLOAT64& _float64, const __WNFixed32& _fixed32) {
    _float64 *= static_cast<WN_FLOAT64>(_fixed32);

    return(_float64);
}

WN_FORCE_INLINE WN_INT8& operator /= (WN_INT8& _integer8, const __WNFixed32& _fixed32) {
    _integer8 /= static_cast<WN_INT8>(_fixed32);

    return(_integer8);
}

WN_FORCE_INLINE WN_INT16& operator /= (WN_INT16& _integer16, const __WNFixed32& _fixed32) {
    _integer16 /= static_cast<WN_INT16>(_fixed32);

    return(_integer16);
}

WN_FORCE_INLINE WN_INT32& operator /= (WN_INT32& _integer32, const __WNFixed32& _fixed32) {
    _integer32 /= static_cast<WN_INT32>(_fixed32);

    return(_integer32);
}

WN_FORCE_INLINE WN_INT64& operator /= (WN_INT64& _integer64, const __WNFixed32& _fixed32) {
    _integer64 /= static_cast<WN_INT64>(_fixed32);

    return(_integer64);
}

WN_FORCE_INLINE WN_UINT8& operator /= (WN_UINT8& _integer8, const __WNFixed32& _fixed32) {
    _integer8 /= static_cast<WN_UINT8>(_fixed32);

    return(_integer8);
}

WN_FORCE_INLINE WN_UINT16& operator /= (WN_UINT16& _integer16, const __WNFixed32& _fixed32) {
    _integer16 /= static_cast<WN_UINT16>(_fixed32);

    return(_integer16);
}

WN_FORCE_INLINE WN_UINT32& operator /= (WN_UINT32& _integer32, const __WNFixed32& _fixed32) {
    _integer32 /= static_cast<WN_UINT32>(_fixed32);

    return(_integer32);
}

WN_FORCE_INLINE WN_UINT64& operator /= (WN_UINT64& _integer64, const __WNFixed32& _fixed32) {
    _integer64 /= static_cast<WN_UINT64>(_fixed32);

    return(_integer64);
}

WN_FORCE_INLINE WN_FLOAT32& operator /= (WN_FLOAT32& _float32, const __WNFixed32& _fixed32) {
    _float32 /= static_cast<WN_FLOAT32>(_fixed32);

    return(_float32);
}

WN_FORCE_INLINE WN_FLOAT64& operator /= (WN_FLOAT64& _float64, const __WNFixed32& _fixed32) {
    _float64 /= static_cast<WN_FLOAT64>(_fixed32);

    return(_float64);
}

WN_FORCE_INLINE __WNFixed32 operator + (WN_INT8 _integer8, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer8) + _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator + (WN_INT16 _integer16, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer16) + _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator + (WN_INT32 _integer32, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer32) + _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator + (WN_INT64 _integer64, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer64) + _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator + (WN_UINT8 _integer8, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer8) + _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator + (WN_UINT16 _integer16, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer16) + _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator + (WN_UINT32 _integer32, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer32) + _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator + (WN_UINT64 _integer64, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer64) + _fixed32);
}

WN_FORCE_INLINE WN_FLOAT32 operator + (WN_FLOAT32 _float32, const __WNFixed32& _fixed32) {
    return(_float32 + static_cast<WN_FLOAT32>(_fixed32));
}

WN_FORCE_INLINE WN_FLOAT64 operator + (WN_FLOAT64 _float64, const __WNFixed32& _fixed32) {
    return(_float64 + static_cast<WN_FLOAT64>(_fixed32));
}

WN_FORCE_INLINE __WNFixed32 operator - (WN_INT8 _integer8, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer8) - _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator - (WN_INT16 _integer16, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer16) - _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator - (WN_INT32 _integer32, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer32) - _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator - (WN_INT64 _integer64, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer64) - _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator - (WN_UINT8 _integer8, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer8) - _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator - (WN_UINT16 _integer16, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer16) - _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator - (WN_UINT32 _integer32, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer32) - _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator - (WN_UINT64 _integer64, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer64) - _fixed32);
}

WN_FORCE_INLINE WN_FLOAT32 operator - (WN_FLOAT32 _float32, const __WNFixed32& _fixed32) {
    return(_float32 - static_cast<WN_FLOAT32>(_fixed32));
}

WN_FORCE_INLINE WN_FLOAT64 operator - (WN_FLOAT64 _float64, const __WNFixed32& _fixed32) {
    return(_float64 - static_cast<WN_FLOAT64>(_fixed32));
}

WN_FORCE_INLINE __WNFixed32 operator * (WN_INT8 _integer8, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer8) * _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator * (WN_INT16 _integer16, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer16) * _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator * (WN_INT32 _integer32, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer32) * _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator * (WN_INT64 _integer64, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer64) * _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator * (WN_UINT8 _integer8, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer8) * _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator * (WN_UINT16 _integer16, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer16) * _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator * (WN_UINT32 _integer32, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer32) * _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator * (WN_UINT64 _integer64, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer64) * _fixed32);
}

WN_FORCE_INLINE WN_FLOAT32 operator * (WN_FLOAT32 _float32, const __WNFixed32& _fixed32) {
    return(_float32 * static_cast<WN_FLOAT32>(_fixed32));
}

WN_FORCE_INLINE WN_FLOAT64 operator * (WN_FLOAT64 _float64, const __WNFixed32& _fixed32) {
    return(_float64 * static_cast<WN_FLOAT64>(_fixed32));
}

WN_FORCE_INLINE __WNFixed32 operator / (WN_INT8 _integer8, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer8) / _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator / (WN_INT16 _integer16, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer16) / _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator / (WN_INT32 _integer32, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer32) / _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator / (WN_INT64 _integer64, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer64) / _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator / (WN_UINT8 _integer8, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer8) / _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator / (WN_UINT16 _integer16, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer16) / _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator / (WN_UINT32 _integer32, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer32) / _fixed32);
}

WN_FORCE_INLINE __WNFixed32 operator / (WN_UINT64 _integer64, const __WNFixed32& _fixed32) {
    return(__WNFixed32(_integer64) / _fixed32);
}

WN_FORCE_INLINE WN_FLOAT32 operator / (WN_FLOAT32 _float32, const __WNFixed32& _fixed32) {
    return(_float32 / static_cast<WN_FLOAT32>(_fixed32));
}

WN_FORCE_INLINE WN_FLOAT64 operator / (WN_FLOAT64 _float64, const __WNFixed32& _fixed32) {
    return(_float64 / static_cast<WN_FLOAT64>(_fixed32));
}

// Fixed (64-bit) //////////////////////////////////////////////////////////////////////////////////////////////////////////////

WN_FORCE_INLINE __WNFixed64::__WNFixed64(WN_INT8 _integer8) {
    mBits = static_cast<WN_INT64>(_integer8) << WN_FIXED64_BITS;
}

WN_FORCE_INLINE __WNFixed64::__WNFixed64(WN_INT16 _integer16) {
    mBits = static_cast<WN_INT64>(_integer16) << WN_FIXED64_BITS;
}

WN_FORCE_INLINE __WNFixed64::__WNFixed64(WN_INT32 _integer32) {
    mBits = static_cast<WN_INT64>(_integer32) << WN_FIXED64_BITS;
}

WN_FORCE_INLINE __WNFixed64::__WNFixed64(WN_INT64 _integer64) {
    mBits = _integer64 << WN_FIXED64_BITS;
}

WN_FORCE_INLINE __WNFixed64::__WNFixed64(WN_UINT8 _integer8) {
    mBits = static_cast<WN_INT64>(_integer8) << WN_FIXED64_BITS;
}

WN_FORCE_INLINE __WNFixed64::__WNFixed64(WN_UINT16 _integer16) {
    mBits = static_cast<WN_INT64>(_integer16) << WN_FIXED64_BITS;
}

WN_FORCE_INLINE __WNFixed64::__WNFixed64(WN_UINT32 _integer32) {
    mBits = static_cast<WN_INT64>(_integer32) << WN_FIXED64_BITS;
}

WN_FORCE_INLINE __WNFixed64::__WNFixed64(WN_UINT64 _integer64) {
    mBits = static_cast<WN_INT64>(_integer64) << WN_FIXED64_BITS;
}

WN_FORCE_INLINE __WNFixed64::__WNFixed64(const __WNFixed8& _fixed8) {
    mBits = _fixed8.mBits << (WN_FIXED64_BITS - WN_FIXED8_BITS);
}

WN_FORCE_INLINE __WNFixed64::__WNFixed64(const __WNFixed16& _fixed16) {
    mBits = _fixed16.mBits << (WN_FIXED64_BITS - WN_FIXED16_BITS);
}

WN_FORCE_INLINE __WNFixed64::__WNFixed64(const __WNFixed32& _fixed32) {
    mBits = _fixed32.mBits << (WN_FIXED64_BITS - WN_FIXED32_BITS);
}

WN_FORCE_INLINE __WNFixed64::__WNFixed64(const __WNFixed64& _fixed64) {
    mBits = _fixed64.mBits;
}

WN_FORCE_INLINE __WNFixed64::__WNFixed64(const __WNFloat8& _float8) {
    const WN_FLOAT32 number = static_cast<WN_FLOAT32>(_float8);

    mBits = static_cast<WN_INT64>((number * __WN_FIXED64_ONE) + (number >= 0.0f ? 0.5f : -0.5f));
}

WN_FORCE_INLINE __WNFixed64::__WNFixed64(const __WNFloat16& _float16) {
    const WN_FLOAT32 number = static_cast<WN_FLOAT32>(_float16);

    mBits = static_cast<WN_INT64>((number * __WN_FIXED64_ONE) + (number >= 0.0f ? 0.5f : -0.5f));
}

WN_FORCE_INLINE __WNFixed64::__WNFixed64(WN_FLOAT32 _float32) {
    mBits = static_cast<WN_INT64>((_float32 * __WN_FIXED64_ONE) + (_float32 >= 0.0f ? 0.5f : -0.5f));
}

WN_FORCE_INLINE __WNFixed64::__WNFixed64(WN_FLOAT64 _float64) {
    mBits = static_cast<WN_INT64>((_float64 * __WN_FIXED64_ONE) + (_float64 >= 0.0 ? 0.5 : -0.5));
}

WN_FORCE_INLINE __WNFixed64::operator WN_INT8 () const {
    return(static_cast<WN_INT8>(static_cast<WN_INT64>(*this)));
}

WN_FORCE_INLINE __WNFixed64::operator WN_INT16 () const {
    return(static_cast<WN_INT16>(static_cast<WN_INT64>(*this)));
}

WN_FORCE_INLINE __WNFixed64::operator WN_INT32 () const {
    return(static_cast<WN_INT32>(static_cast<WN_INT64>(*this)));
}

WN_FORCE_INLINE __WNFixed64::operator WN_INT64 () const {
    return(mBits >> WN_FIXED64_BITS);
}

WN_FORCE_INLINE __WNFixed64::operator WN_UINT8 () const {
    return(static_cast<WN_UINT8>(static_cast<WN_INT64>(*this)));
}

WN_FORCE_INLINE __WNFixed64::operator WN_UINT16 () const {
    return(static_cast<WN_UINT16>(static_cast<WN_INT64>(*this)));
}

WN_FORCE_INLINE __WNFixed64::operator WN_UINT32 () const {
    return(static_cast<WN_UINT32>(static_cast<WN_INT64>(*this)));
}

WN_FORCE_INLINE __WNFixed64::operator WN_UINT64 () const {
    return(static_cast<WN_UINT64>(static_cast<WN_INT64>(*this)));
}

WN_FORCE_INLINE __WNFixed64::operator WN_FLOAT32 () const {
    return(static_cast<WN_FLOAT32>(mBits) / __WN_FIXED64_ONE);
}

WN_FORCE_INLINE __WNFixed64::operator WN_FLOAT64 () const {
    return(static_cast<WN_FLOAT64>(static_cast<WN_FLOAT32>(*this)));
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator = (WN_INT8 _integer8) {
    *this = __WNFixed64(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator = (WN_INT16 _integer16) {
    *this = __WNFixed64(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator = (WN_INT32 _integer32) {
    *this = __WNFixed64(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator = (WN_INT64 _integer64) {
    *this = __WNFixed64(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator = (WN_UINT8 _integer8) {
    *this = __WNFixed64(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator = (WN_UINT16 _integer16) {
    *this = __WNFixed64(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator = (WN_UINT32 _integer32) {
    *this = __WNFixed64(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator = (WN_UINT64 _integer64) {
    *this = __WNFixed64(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator = (const __WNFixed8& _fixed8) {
    *this = __WNFixed64(_fixed8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator = (const __WNFixed16& _fixed16) {
    *this = __WNFixed64(_fixed16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator = (const __WNFixed32& _fixed32) {
    *this = __WNFixed64(_fixed32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator = (const __WNFixed64& _fixed64) {
    mBits = _fixed64.mBits;

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator = (WN_FLOAT32 _float32) {
    *this = __WNFixed64(_float32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator = (WN_FLOAT64 _float64) {
    *this = __WNFixed64(_float64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator + () const {
    return(*this);
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator - () const {
    __WNFixed64 result;

    result.mBits = -mBits;

    return(result);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator ++ () {
    mBits += __WN_FIXED64_ONE;

    return(*this);
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator ++ (WN_INT32) {
    const __WNFixed64 result = *this;

    ++(*this);

    return(result);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator -- () {
    mBits -= __WN_FIXED64_ONE;

    return(*this);
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator -- (WN_INT32) {
    const __WNFixed64 result = *this;

    --(*this);

    return(result);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator ! () const {
    return(!mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator == (WN_INT8 _integer8) const {
    return(*this == __WNFixed64(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator == (WN_INT16 _integer16) const {
    return(*this == __WNFixed64(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator == (WN_INT32 _integer32) const {
    return(*this == __WNFixed64(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator == (WN_INT64 _integer64) const {
    return(*this == __WNFixed64(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator == (WN_UINT8 _integer8) const {
    return(*this == __WNFixed64(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator == (WN_UINT16 _integer16) const {
    return(*this == __WNFixed64(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator == (WN_UINT32 _integer32) const {
    return(*this == __WNFixed64(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator == (WN_UINT64 _integer64) const {
    return(*this == __WNFixed64(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator == (const __WNFixed8& _fixed8) const {
    return(*this == __WNFixed64(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator == (const __WNFixed16& _fixed16) const {
    return(*this == __WNFixed64(_fixed16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator == (const __WNFixed32& _fixed32) const {
    return(*this == __WNFixed64(_fixed32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator == (const __WNFixed64& _fixed64) const {
    return(mBits == _fixed64.mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator == (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) == _float8);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator == (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) == _float16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator == (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) == _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator == (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) == _float64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator != (WN_INT8 _integer8) const {
    return(*this != __WNFixed64(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator != (WN_INT16 _integer16) const {
    return(*this != __WNFixed64(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator != (WN_INT32 _integer32) const {
    return(*this != __WNFixed64(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator != (WN_INT64 _integer64) const {
    return(*this != __WNFixed64(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator != (WN_UINT8 _integer8) const {
    return(*this != __WNFixed64(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator != (WN_UINT16 _integer16) const {
    return(*this != __WNFixed64(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator != (WN_UINT32 _integer32) const {
    return(*this != __WNFixed64(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator != (WN_UINT64 _integer64) const {
    return(*this != __WNFixed64(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator != (const __WNFixed8& _fixed8) const {
    return(*this != __WNFixed64(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator != (const __WNFixed16& _fixed16) const {
    return(*this != __WNFixed64(_fixed16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator != (const __WNFixed32& _fixed32) const {
    return(*this != __WNFixed64(_fixed32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator != (const __WNFixed64& _fixed64) const {
    return(mBits != _fixed64.mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator != (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) != _float8);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator != (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) != _float16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator != (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) != _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator != (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) != _float64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator <= (WN_INT8 _integer8) const {
    return(*this <= __WNFixed64(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator <= (WN_INT16 _integer16) const {
    return(*this <= __WNFixed64(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator <= (WN_INT32 _integer32) const {
    return(*this <= __WNFixed64(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator <= (WN_INT64 _integer64) const {
    return(*this <= __WNFixed64(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator <= (WN_UINT8 _integer8) const {
    return(*this <= __WNFixed64(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator <= (WN_UINT16 _integer16) const {
    return(*this <= __WNFixed64(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator <= (WN_UINT32 _integer32) const {
    return(*this <= __WNFixed64(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator <= (WN_UINT64 _integer64) const {
    return(*this <= __WNFixed64(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator <= (const __WNFixed8& _fixed8) const {
    return(*this <= __WNFixed64(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator <= (const __WNFixed16& _fixed16) const {
    return(*this <= __WNFixed64(_fixed16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator <= (const __WNFixed32& _fixed32) const {
    return(*this <= __WNFixed64(_fixed32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator <= (const __WNFixed64& _fixed64) const {
    return(mBits <= _fixed64.mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator <= (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) <= _float8);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator <= (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) <= _float16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator <= (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) <= _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator <= (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) <= _float64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator >= (WN_INT8 _integer8) const {
    return(*this >= __WNFixed64(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator >= (WN_INT16 _integer16) const {
    return(*this >= __WNFixed64(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator >= (WN_INT32 _integer32) const {
    return(*this >= __WNFixed64(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator >= (WN_INT64 _integer64) const {
    return(*this >= __WNFixed64(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator >= (WN_UINT8 _integer8) const {
    return(*this >= __WNFixed64(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator >= (WN_UINT16 _integer16) const {
    return(*this >= __WNFixed64(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator >= (WN_UINT32 _integer32) const {
    return(*this >= __WNFixed64(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator >= (WN_UINT64 _integer64) const {
    return(*this >= __WNFixed64(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator >= (const __WNFixed8& _fixed8) const {
    return(*this >= __WNFixed64(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator >= (const __WNFixed16& _fixed16) const {
    return(*this >= __WNFixed64(_fixed16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator >= (const __WNFixed32& _fixed32) const {
    return(*this >= __WNFixed64(_fixed32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator >= (const __WNFixed64& _fixed64) const {
    return(mBits >= _fixed64.mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator >= (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) >= _float8);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator >= (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) >= _float16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator >= (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) >= _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator >= (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) >= _float64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator < (WN_INT8 _integer8) const {
    return(*this < __WNFixed64(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator < (WN_INT16 _integer16) const {
    return(*this < __WNFixed64(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator < (WN_INT32 _integer32) const {
    return(*this < __WNFixed64(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator < (WN_INT64 _integer64) const {
    return(*this < __WNFixed64(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator < (WN_UINT8 _integer8) const {
    return(*this < __WNFixed64(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator < (WN_UINT16 _integer16) const {
    return(*this < __WNFixed64(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator < (WN_UINT32 _integer32) const {
    return(*this < __WNFixed64(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator < (WN_UINT64 _integer64) const {
    return(*this < __WNFixed64(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator < (const __WNFixed8& _fixed8) const {
    return(*this < __WNFixed64(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator < (const __WNFixed16& _fixed16) const {
    return(*this < __WNFixed64(_fixed16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator < (const __WNFixed32& _fixed32) const {
    return(*this < __WNFixed64(_fixed32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator < (const __WNFixed64& _fixed64) const {
    return(mBits < _fixed64.mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator < (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) < _float8);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator < (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) < _float16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator < (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) < _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator < (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) < _float64);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator > (WN_INT8 _integer8) const {
    return(*this > __WNFixed64(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator > (WN_INT16 _integer16) const {
    return(*this > __WNFixed64(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator > (WN_INT32 _integer32) const {
    return(*this > __WNFixed64(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator > (WN_INT64 _integer64) const {
    return(*this > __WNFixed64(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator > (WN_UINT8 _integer8) const {
    return(*this > __WNFixed64(_integer8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator > (WN_UINT16 _integer16) const {
    return(*this > __WNFixed64(_integer16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator > (WN_UINT32 _integer32) const {
    return(*this > __WNFixed64(_integer32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator > (WN_UINT64 _integer64) const {
    return(*this > __WNFixed64(_integer64));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator > (const __WNFixed8& _fixed8) const {
    return(*this > __WNFixed64(_fixed8));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator > (const __WNFixed16& _fixed16) const {
    return(*this > __WNFixed64(_fixed16));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator > (const __WNFixed32& _fixed32) const {
    return(*this > __WNFixed64(_fixed32));
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator > (const __WNFixed64& _fixed64) const {
    return(mBits > _fixed64.mBits);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator > (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) > _float8);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator > (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) > _float16);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator > (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) > _float32);
}

WN_FORCE_INLINE WN_BOOL __WNFixed64::operator > (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) > _float64);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator += (WN_INT8 _integer8) {
    *this += __WNFixed64(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator += (WN_INT16 _integer16) {
    *this += __WNFixed64(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator += (WN_INT32 _integer32) {
    *this += __WNFixed64(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator += (WN_INT64 _integer64) {
    *this += __WNFixed64(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator += (WN_UINT8 _integer8) {
    *this += __WNFixed64(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator += (WN_UINT16 _integer16) {
    *this += __WNFixed64(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator += (WN_UINT32 _integer32) {
    *this += __WNFixed64(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator += (WN_UINT64 _integer64) {
    *this += __WNFixed64(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator += (const __WNFixed8& _fixed8) {
    *this += __WNFixed64(_fixed8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator += (const __WNFixed16& _fixed16) {
    *this += __WNFixed64(_fixed16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator += (const __WNFixed32& _fixed32) {
    *this += __WNFixed64(_fixed32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator += (const __WNFixed64& _fixed64) {
    mBits += _fixed64.mBits;

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator += (const __WNFloat8& _float8) {
    *this += __WNFixed64(_float8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator += (const __WNFloat16& _float16) {
    *this += __WNFixed64(_float16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator += (WN_FLOAT32 _float32) {
    *this += __WNFixed64(_float32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator += (WN_FLOAT64 _float64) {
    *this += __WNFixed64(_float64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator -= (WN_INT8 _integer8) {
    *this -= __WNFixed64(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator -= (WN_INT16 _integer16) {
    *this -= __WNFixed64(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator -= (WN_INT32 _integer32) {
    *this -= __WNFixed64(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator -= (WN_INT64 _integer64) {
    *this -= __WNFixed64(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator -= (WN_UINT8 _integer8) {
    *this -= __WNFixed64(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator -= (WN_UINT16 _integer16) {
    *this -= __WNFixed64(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator -= (WN_UINT32 _integer32) {
    *this -= __WNFixed64(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator -= (WN_UINT64 _integer64) {
    *this -= __WNFixed64(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator -= (const __WNFixed8& _fixed8) {
    *this -= __WNFixed64(_fixed8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator -= (const __WNFixed16& _fixed16) {
    *this -= __WNFixed64(_fixed16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator -= (const __WNFixed32& _fixed32) {
    *this -= __WNFixed64(_fixed32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator -= (const __WNFixed64& _fixed64) {
    mBits -= _fixed64.mBits;

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator -= (const __WNFloat8& _float8) {
    *this -= __WNFixed64(_float8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator -= (const __WNFloat16& _float16) {
    *this -= __WNFixed64(_float16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator -= (WN_FLOAT32 _float32) {
    *this -= __WNFixed64(_float32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator -= (WN_FLOAT64 _float64) {
    *this -= __WNFixed64(_float64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator *= (WN_INT8 _integer8) {
    *this *= __WNFixed64(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator *= (WN_INT16 _integer16) {
    *this *= __WNFixed64(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator *= (WN_INT32 _integer32) {
    *this *= __WNFixed64(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator *= (WN_INT64 _integer64) {
    *this *= __WNFixed64(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator *= (WN_UINT8 _integer8) {
    *this *= __WNFixed64(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator *= (WN_UINT16 _integer16) {
    *this *= __WNFixed64(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator *= (WN_UINT32 _integer32) {
    *this *= __WNFixed64(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator *= (WN_UINT64 _integer64) {
    *this *= __WNFixed64(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator *= (const __WNFixed8& _fixed8) {
    *this *= __WNFixed64(_fixed8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator *= (const __WNFixed16& _fixed16) {
    *this *= __WNFixed64(_fixed16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator *= (const __WNFixed32& _fixed32) {
    *this *= __WNFixed64(_fixed32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator *= (const __WNFixed64& _fixed64) {
#if defined _WN_WINDOWS && defined _WN_64_BIT
    mBits = static_cast<WN_INT64>(MultiplyExtract128(mBits, _fixed64.mBits, WN_FIXED64_BITS));
#else
    const WN_UINT64 a1 = mBits >> 32;
    const WN_UINT64 a0 = mBits - (a1 << 32);
    const WN_UINT64 b1 = _fixed64.mBits >> 32;
    const WN_UINT64 b0 = _fixed64.mBits - (b1 << 32);
    const WN_UINT64 d = a0 * b0;
    const WN_UINT64 d1 = d >> 32;
    const WN_UINT64 d0 = d - (d1 << 32);
    const WN_UINT64 e = a0 * b1;
    const WN_UINT64 e1 = e >> 32;
    const WN_UINT64 e0 = e - (e1 << 32);
    const WN_UINT64 f = a1 * b0;
    const WN_UINT64 f1 = f >> 32;
    const WN_UINT64 f0 = f - (f1 << 32);
    const WN_UINT64 g = a1 * b1;
    const WN_UINT64 g1 = g >> 32;
    const WN_UINT64 g0 = g - (g1 << 32);
    const WN_UINT64 roll = 1ULL << 32;
    const WN_UINT64 pmax = roll - 1;
    WN_UINT64 sum = d1 + e0 + f0;
    WN_UINT64 carry = 0;

    while (pmax < sum) {
        sum -= roll;
        carry++;
    }

    const WN_INT64 highProduct = carry + e1 + f1 + g0 + (g1 << 32);
    const WN_INT64 lowProduct = d0 + (sum << 32);
    WN_UINT64 uhighProduct = (WN_UINT64)highProduct;
    WN_UINT64 ulowProduct = (WN_UINT64)lowProduct;
    WN_BOOL negate = WN_FALSE;

    if (highProduct < 0) {
        negate = WN_TRUE;
        uhighProduct = (WN_UINT64)(-highProduct);
        ulowProduct = (WN_UINT64)(-lowProduct);

        if (ulowProduct != 0) {
            uhighProduct -= 1;
        }
    }

    WN_INT64 extractedProduct = (WN_INT64)((uhighProduct << WN_FIXED64_BITS) | (ulowProduct >> WN_FIXED64_BITS));

    if (negate) {
        extractedProduct = -extractedProduct;
    }

    mBits = extractedProduct;
#endif

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator *= (const __WNFloat8& _float8) {
    *this *= __WNFixed64(_float8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator *= (const __WNFloat16& _float16) {
    *this *= __WNFixed64(_float16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator *= (WN_FLOAT32 _float32) {
    *this *= __WNFixed64(_float32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator *= (WN_FLOAT64 _float64) {
    *this *= __WNFixed64(_float64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator /= (WN_INT8 _integer8) {
    *this /= __WNFixed64(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator /= (WN_INT16 _integer16) {
    *this /= __WNFixed64(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator /= (WN_INT32 _integer32) {
    *this /= __WNFixed64(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator /= (WN_INT64 _integer64) {
    *this /= __WNFixed64(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator /= (WN_UINT8 _integer8) {
    *this /= __WNFixed64(_integer8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator /= (WN_UINT16 _integer16) {
    *this /= __WNFixed64(_integer16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator /= (WN_UINT32 _integer32) {
    *this /= __WNFixed64(_integer32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator /= (WN_UINT64 _integer64) {
    *this /= __WNFixed64(_integer64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator /= (const __WNFixed8& _fixed8) {
    *this /= __WNFixed64(_fixed8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator /= (const __WNFixed16& _fixed16) {
    *this /= __WNFixed64(_fixed16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator /= (const __WNFixed32& _fixed32) {
    *this /= __WNFixed64(_fixed32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator /= (const __WNFixed64& _fixed64) {
    WN_INT64 quotient = mBits;
    WN_INT64 divisor = _fixed64.mBits;
    WN_BOOL negate = WN_FALSE;

    if (divisor < 0) {
        divisor = -divisor;
        negate = !negate;
    }

    if (quotient < 0) {
        quotient = -quotient;
        negate = !negate;
    }

    WN_UINT64 quotientHi = quotient >> WN_FIXED64_BITS;
    WN_UINT64 quotientLow = quotient << WN_FIXED64_BITS;
    WN_INT64 remainder = 0;

    for (WN_UINT8 i = 0; i < 128; ++i) {
        const WN_UINT64 highBit = 0x8000000000000000ULL & quotientHi;
        const WN_UINT64 shiftBit = quotientLow & 0x8000000000000000ULL;

        remainder <<= 1;

        if (highBit) {
            remainder |= 1;
        }

        quotientHi <<= 1;
        quotientLow <<= 1;

        if (shiftBit) {
            quotientHi |= 1;
        }

        if (remainder >= divisor) {
            remainder -= divisor;
            quotientLow |= 1;
        }
    }

    if (negate) {
        quotientLow = static_cast<WN_UINT64>(-static_cast<WN_INT64>(quotientLow));
    }

    mBits = static_cast<WN_INT64>(quotientLow);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator /= (const __WNFloat8& _float8) {
    *this /= __WNFixed64(_float8);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator /= (const __WNFloat16& _float16) {
    *this /= __WNFixed64(_float16);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator /= (WN_FLOAT32 _float32) {
    *this /= __WNFixed64(_float32);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64& __WNFixed64::operator /= (WN_FLOAT64 _float64) {
    *this /= __WNFixed64(_float64);

    return(*this);
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator + (WN_INT8 _integer8) const {
    return(*this + __WNFixed64(_integer8));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator + (WN_INT16 _integer16) const {
    return(*this + __WNFixed64(_integer16));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator + (WN_INT32 _integer32) const {
    return(*this + __WNFixed64(_integer32));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator + (WN_INT64 _integer64) const {
    return(*this + __WNFixed64(_integer64));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator + (WN_UINT8 _integer8) const {
    return(*this + __WNFixed64(_integer8));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator + (WN_UINT16 _integer16) const {
    return(*this + __WNFixed64(_integer16));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator + (WN_UINT32 _integer32) const {
    return(*this + __WNFixed64(_integer32));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator + (WN_UINT64 _integer64) const {
    return(*this + __WNFixed64(_integer64));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator + (const __WNFixed8& _fixed8) const {
    return(*this + __WNFixed64(_fixed8));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator + (const __WNFixed16& _fixed16) const {
    return(*this + __WNFixed64(_fixed16));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator + (const __WNFixed32& _fixed32) const {
    return(*this + __WNFixed64(_fixed32));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator + (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) += _fixed64);
}

WN_FORCE_INLINE __WNFloat8 __WNFixed64::operator + (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) + _float8);
}

WN_FORCE_INLINE __WNFloat16 __WNFixed64::operator + (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) + _float16);
}

WN_FORCE_INLINE WN_FLOAT32 __WNFixed64::operator + (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) + _float32);
}

WN_FORCE_INLINE WN_FLOAT64 __WNFixed64::operator + (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) + _float64);
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator - (WN_INT8 _integer8) const {
    return(*this - __WNFixed64(_integer8));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator - (WN_INT16 _integer16) const {
    return(*this - __WNFixed64(_integer16));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator - (WN_INT32 _integer32) const {
    return(*this - __WNFixed64(_integer32));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator - (WN_INT64 _integer64) const {
    return(*this - __WNFixed64(_integer64));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator - (WN_UINT8 _integer8) const {
    return(*this - __WNFixed64(_integer8));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator - (WN_UINT16 _integer16) const {
    return(*this - __WNFixed64(_integer16));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator - (WN_UINT32 _integer32) const {
    return(*this - __WNFixed64(_integer32));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator - (WN_UINT64 _integer64) const {
    return(*this - __WNFixed64(_integer64));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator - (const __WNFixed8& _fixed8) const {
    return(*this - __WNFixed64(_fixed8));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator - (const __WNFixed16& _fixed16) const {
    return(*this - __WNFixed64(_fixed16));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator - (const __WNFixed32& _fixed32) const {
    return(*this - __WNFixed64(_fixed32));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator - (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) -= _fixed64);
}

WN_FORCE_INLINE __WNFloat8 __WNFixed64::operator - (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) - _float8);
}

WN_FORCE_INLINE __WNFloat16 __WNFixed64::operator - (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) - _float16);
}

WN_FORCE_INLINE WN_FLOAT32 __WNFixed64::operator - (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) - _float32);
}

WN_FORCE_INLINE WN_FLOAT64 __WNFixed64::operator - (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) - _float64);
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator * (WN_INT8 _integer8) const {
    return(*this * __WNFixed64(_integer8));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator * (WN_INT16 _integer16) const {
    return(*this * __WNFixed64(_integer16));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator * (WN_INT32 _integer32) const {
    return(*this * __WNFixed64(_integer32));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator * (WN_INT64 _integer64) const {
    return(*this * __WNFixed64(_integer64));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator * (WN_UINT8 _integer8) const {
    return(*this * __WNFixed64(_integer8));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator * (WN_UINT16 _integer16) const {
    return(*this * __WNFixed64(_integer16));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator * (WN_UINT32 _integer32) const {
    return(*this * __WNFixed64(_integer32));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator * (WN_UINT64 _integer64) const {
    return(*this * __WNFixed64(_integer64));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator * (const __WNFixed8& _fixed8) const {
    return(*this * __WNFixed64(_fixed8));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator * (const __WNFixed16& _fixed16) const {
    return(*this * __WNFixed64(_fixed16));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator * (const __WNFixed32& _fixed32) const {
    return(*this * __WNFixed64(_fixed32));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator * (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) *= _fixed64);
}

WN_FORCE_INLINE __WNFloat8 __WNFixed64::operator * (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) * _float8);
}

WN_FORCE_INLINE __WNFloat16 __WNFixed64::operator * (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) * _float16);
}

WN_FORCE_INLINE WN_FLOAT32 __WNFixed64::operator * (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) * _float32);
}

WN_FORCE_INLINE WN_FLOAT64 __WNFixed64::operator * (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) * _float64);
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator / (WN_INT8 _integer8) const {
    return(*this / __WNFixed64(_integer8));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator / (WN_INT16 _integer16) const {
    return(*this / __WNFixed64(_integer16));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator / (WN_INT32 _integer32) const {
    return(*this / __WNFixed64(_integer32));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator / (WN_INT64 _integer64) const {
    return(*this / __WNFixed64(_integer64));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator / (WN_UINT8 _integer8) const {
    return(*this / __WNFixed64(_integer8));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator / (WN_UINT16 _integer16) const {
    return(*this / __WNFixed64(_integer16));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator / (WN_UINT32 _integer32) const {
    return(*this / __WNFixed64(_integer32));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator / (WN_UINT64 _integer64) const {
    return(*this / __WNFixed64(_integer64));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator / (const __WNFixed8& _fixed8) const {
    return(*this / __WNFixed64(_fixed8));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator / (const __WNFixed16& _fixed16) const {
    return(*this / __WNFixed64(_fixed16));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator / (const __WNFixed32& _fixed32) const {
    return(*this / __WNFixed64(_fixed32));
}

WN_FORCE_INLINE __WNFixed64 __WNFixed64::operator / (const __WNFixed64& _fixed64) const {
    return(__WNFixed64(*this) /= _fixed64);
}

WN_FORCE_INLINE __WNFloat8 __WNFixed64::operator / (const __WNFloat8& _float8) const {
    return(__WNFloat8(*this) / _float8);
}

WN_FORCE_INLINE __WNFloat16 __WNFixed64::operator / (const __WNFloat16& _float16) const {
    return(__WNFloat16(*this) / _float16);
}

WN_FORCE_INLINE WN_FLOAT32 __WNFixed64::operator / (WN_FLOAT32 _float32) const {
    return(static_cast<WN_FLOAT32>(*this) / _float32);
}

WN_FORCE_INLINE WN_FLOAT64 __WNFixed64::operator / (WN_FLOAT64 _float64) const {
    return(static_cast<WN_FLOAT64>(*this) / _float64);
}

WN_FORCE_INLINE __WNFixed64 operator + (WN_INT8 _integer8, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer8) + _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator + (WN_INT16 _integer16, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer16) + _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator + (WN_INT32 _integer32, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer32) + _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator + (WN_INT64 _integer64, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer64) + _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator + (WN_UINT8 _integer8, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer8) + _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator + (WN_UINT16 _integer16, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer16) + _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator + (WN_UINT32 _integer32, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer32) + _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator + (WN_UINT64 _integer64, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer64) + _fixed64);
}

WN_FORCE_INLINE WN_FLOAT32 operator + (WN_FLOAT32 _float32, const __WNFixed64& _fixed64) {
    return(_float32 + static_cast<WN_FLOAT32>(_fixed64));
}

WN_FORCE_INLINE WN_FLOAT64 operator + (WN_FLOAT64 _float64, const __WNFixed64& _fixed64) {
    return(_float64 + static_cast<WN_FLOAT64>(_fixed64));
}

WN_FORCE_INLINE __WNFixed64 operator - (WN_INT8 _integer8, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer8) - _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator - (WN_INT16 _integer16, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer16) - _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator - (WN_INT32 _integer32, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer32) - _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator - (WN_INT64 _integer64, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer64) - _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator - (WN_UINT8 _integer8, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer8) - _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator - (WN_UINT16 _integer16, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer16) - _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator - (WN_UINT32 _integer32, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer32) - _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator - (WN_UINT64 _integer64, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer64) - _fixed64);
}

WN_FORCE_INLINE WN_FLOAT32 operator - (WN_FLOAT32 _float32, const __WNFixed64& _fixed64) {
    return(_float32 - static_cast<WN_FLOAT32>(_fixed64));
}

WN_FORCE_INLINE WN_FLOAT64 operator - (WN_FLOAT64 _float64, const __WNFixed64& _fixed64) {
    return(_float64 - static_cast<WN_FLOAT64>(_fixed64));
}

WN_FORCE_INLINE __WNFixed64 operator * (WN_INT8 _integer8, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer8) * _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator * (WN_INT16 _integer16, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer16) * _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator * (WN_INT32 _integer32, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer32) * _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator * (WN_INT64 _integer64, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer64) * _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator * (WN_UINT8 _integer8, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer8) * _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator * (WN_UINT16 _integer16, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer16) * _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator * (WN_UINT32 _integer32, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer32) * _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator * (WN_UINT64 _integer64, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer64) * _fixed64);
}

WN_FORCE_INLINE WN_FLOAT32 operator * (WN_FLOAT32 _float32, const __WNFixed64& _fixed64) {
    return(_float32 * static_cast<WN_FLOAT32>(_fixed64));
}

WN_FORCE_INLINE WN_FLOAT64 operator * (WN_FLOAT64 _float64, const __WNFixed64& _fixed64) {
    return(_float64 * static_cast<WN_FLOAT64>(_fixed64));
}

WN_FORCE_INLINE __WNFixed64 operator / (WN_INT8 _integer8, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer8) / _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator / (WN_INT16 _integer16, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer16) / _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator / (WN_INT32 _integer32, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer32) / _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator / (WN_INT64 _integer64, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer64) / _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator / (WN_UINT8 _integer8, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer8) / _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator / (WN_UINT16 _integer16, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer16) / _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator / (WN_UINT32 _integer32, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer32) / _fixed64);
}

WN_FORCE_INLINE __WNFixed64 operator / (WN_UINT64 _integer64, const __WNFixed64& _fixed64) {
    return(__WNFixed64(_integer64) / _fixed64);
}

WN_FORCE_INLINE WN_FLOAT32 operator / (WN_FLOAT32 _float32, const __WNFixed64& _fixed64) {
    return(_float32 / static_cast<WN_FLOAT32>(_fixed64));
}

WN_FORCE_INLINE WN_FLOAT64 operator / (WN_FLOAT64 _float64, const __WNFixed64& _fixed64) {
    return(_float64 / static_cast<WN_FLOAT64>(_fixed64));
}

template <>
WN_FORCE_INLINE WN_BOOL WNIsValid(WN_FLOAT8 _value) {
    return(_value == _value);
}

template <>
WN_FORCE_INLINE WN_BOOL WNIsValid(WN_FLOAT16 _value) {
    return(_value == _value);
}

#endif // __WN_CORE_INTERNAL_EXTENDED_TYPES_INL__