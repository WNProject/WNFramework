// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CORE_EXTENDED_TYPES_H__
#define __WN_CORE_EXTENDED_TYPES_H__

#include "WNCore/inc/Internal/WNExtendedTypes.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNCore/inc/WNUtility.h"

#ifdef __WN_F16C_AVAILABLE
    #include <immintrin.h>
#endif

namespace wn {
    namespace core {
        namespace internal {
            template <typename _Traits>
            struct arithmetic_type {
                typedef _Traits traits_type;
                typedef typename traits_type::value_type value_type;

                template <typename _Type>
                struct supported : traits_type::template supported<_Type> {};

                template <typename _Type>
                struct higher_precedence : traits_type::template higher_precedence<_Type> {};

                template <typename _Type>
                struct convertable : traits_type::template convertable<_Type> {};

                arithmetic_type() = default;

                ~arithmetic_type() = default;

                arithmetic_type(arithmetic_type&& _in_value) :
                    representation(std::move(_in_value.representation)) {
                }

                arithmetic_type(const arithmetic_type& _in_value) :
                    representation(_in_value.representation) {
                }

                template <typename other_traits_type,
                          typename = enable_if_t<convertable<arithmetic_type<other_traits_type>>::value>>
                arithmetic_type(const arithmetic_type<other_traits_type>& _in_value) :
                    representation(traits_type::template construct<arithmetic_type<other_traits_type>>(_in_value.representation)) {
                }

                template <typename type,
                          typename = enable_if_t<(supported<type>::value && !convertable<type>::value)>>
                arithmetic_type(const type& _in_value) :
                    representation(traits_type::template construct<type>(_in_value)) {
                }

                template <typename type>
                operator type () const {
                    static_assert(supported<type>::value, "Arithmetic type conversion is not supported for this type");

                    return(traits_type::template convert<type>(representation));
                }

                arithmetic_type& operator = (arithmetic_type&& _in_value) {
                    representation = std::move(_in_value.representation);

                    return(*this);
                }

                arithmetic_type& operator = (const arithmetic_type& _in_value) {
                    representation = _in_value.representation;

                    return(*this);
                }

                template <typename other_traits>
                typename enable_if<convertable<arithmetic_type<other_traits>>::value, arithmetic_type>::type&
                operator = (const arithmetic_type<other_traits>& _in_value) {
                    representation = traits_type::template construct<arithmetic_type<other_traits>>(_in_value.representation);

                    return(*this);
                }

                template <typename type>
                typename enable_if<(supported<type>::value && !convertable<type>::value), arithmetic_type>::type&
                operator = (const type& _in_value) {
                    representation = traits_type::template construct<type>(_in_value);

                    return(*this);
                }

                wn_bool operator == (const arithmetic_type& _in_value) const {
                    return(traits_type::equal(representation, _in_value.representation));
                }

                wn_bool operator != (const arithmetic_type& _in_value) const {
                    return(!traits_type::equal(representation, _in_value.representation));
                }

                wn_bool operator < (const arithmetic_type& _in_value) const {
                    return(traits_type::less_than(representation, _in_value.representation));
                }

                wn_bool operator <= (const arithmetic_type& _in_value) const {
                    return(!traits_type::greater_than(representation, _in_value.representation));
                }

                wn_bool operator > (const arithmetic_type& _in_value) const {
                    return(traits_type::greater_than(representation, _in_value.representation));
                }

                wn_bool operator >= (const arithmetic_type& _in_value) const {
                    return(!traits_type::less_than(representation, _in_value.representation));
                }

                template <typename type>
                typename enable_if<supported<type>::value, wn_bool>::type operator == (const type& _in_value) const {
                    const arithmetic_type in_value(_in_value);

                    return(traits_type::template equal(representation, in_value.representation));
                }

                template <typename type>
                typename enable_if<supported<type>::value, wn_bool>::type operator != (const type& _in_value) const {
                    const arithmetic_type in_value(_in_value);

                    return(!traits_type::template equal(representation, in_value.representation));
                }

                template <typename type>
                typename enable_if<supported<type>::value, wn_bool>::type operator < (const type& _in_value) const {
                    const arithmetic_type in_value(_in_value);

                    return(traits_type::template less_than(representation, in_value.representation));
                }

                template <typename type>
                typename enable_if<supported<type>::value, wn_bool>::type operator <= (const type& _in_value) const {
                    const arithmetic_type in_value(_in_value);

                    return(!traits_type::template greater_than(representation, in_value.representation));
                }

                template <typename type>
                typename enable_if<supported<type>::value, wn_bool>::type operator > (const type& _in_value) const {
                    const arithmetic_type in_value(_in_value);

                    return(traits_type::template greater_than(representation, in_value.representation));
                }

                template <typename type>
                typename enable_if<supported<type>::value, wn_bool>::type operator >= (const type& _in_value) const {
                    const arithmetic_type in_value(_in_value);

                    return(!traits_type::template less_than(representation, in_value.representation));
                }

                arithmetic_type operator + () const {
                    return(*this);
                }

                arithmetic_type operator - () const {
                    arithmetic_type value(*this);

                    traits_type::negate(value.representation);

                    return(value);
                }

                arithmetic_type& operator += (const arithmetic_type& _in_value) {
                    traits_type::add(representation, _in_value.representation);

                    return(*this);
                }

                arithmetic_type& operator -= (const arithmetic_type& _in_value) {
                    traits_type::subtract(representation, _in_value.representation);

                    return(*this);
                }

                arithmetic_type& operator *= (const arithmetic_type& _in_value) {
                    traits_type::multiply(representation, _in_value.representation);

                    return(*this);
                }

                arithmetic_type& operator /= (const arithmetic_type& _in_value) {
                  traits_type::divide(representation, _in_value.representation);

                    return(*this);
                }

                template <typename type>
                typename enable_if<supported<type>::value, arithmetic_type>::type& operator += (const type& _in_value) {
                    *this += arithmetic_type(_in_value);

                    return(*this);
                }

                template <typename type>
                typename enable_if<supported<type>::value, arithmetic_type>::type& operator -= (const type& _in_value) {
                    *this -= arithmetic_type(_in_value);

                    return(*this);
                }

                template <typename type>
                typename enable_if<supported<type>::value, arithmetic_type>::type& operator *= (const type& _in_value) {
                    *this *= arithmetic_type(_in_value);

                    return(*this);
                }

                template <typename type>
                typename enable_if<supported<type>::value, arithmetic_type>::type& operator /= (const type& _in_value) {
                    *this /= arithmetic_type(_in_value);

                    return(*this);
                }

                arithmetic_type operator + (const arithmetic_type& _in_value) const {
                    return(arithmetic_type(*this) += _in_value);
                }

                arithmetic_type operator - (const arithmetic_type& _in_value) const {
                    return(arithmetic_type(*this) -= _in_value);
                }

                arithmetic_type operator * (const arithmetic_type& _in_value) const {
                    return(arithmetic_type(*this) *= _in_value);
                }

                arithmetic_type operator / (const arithmetic_type& _in_value) const {
                    return(arithmetic_type(*this) /= _in_value);
                }

                template <typename type>
                typename enable_if<(supported<type>::value && higher_precedence<type>::value), type>::type
                operator + (const type& _in_value) const {
                    return(static_cast<type>(*this) + _in_value);
                }

                template <typename type>
                typename enable_if<(supported<type>::value && higher_precedence<type>::value), type>::type
                operator - (const type& _in_value) const {
                    return(static_cast<type>(*this) - _in_value);
                }

                template <typename type>
                typename enable_if<(supported<type>::value && higher_precedence<type>::value), type>::type
                operator * (const type& _in_value) const {
                    return(static_cast<type>(*this) * _in_value);
                }

                template <typename type>
                typename enable_if<(supported<type>::value && higher_precedence<type>::value), type>::type
                operator / (const type& _in_value) const {
                    return(static_cast<type>(*this) / _in_value);
                }

                template <typename type>
                typename enable_if<(supported<type>::value && !higher_precedence<type>::value), arithmetic_type>::type
                operator + (const type& _in_value) const {
                    return(arithmetic_type(_in_value) += *this);
                }

                template <typename type>
                typename enable_if<(supported<type>::value && !higher_precedence<type>::value), arithmetic_type>::type
                operator - (const type& _in_value) const {
                    return(*this - arithmetic_type(_in_value));
                }

                template <typename type>
                typename enable_if<(supported<type>::value && !higher_precedence<type>::value), arithmetic_type>::type
                operator * (const type& _in_value) const {
                    return(arithmetic_type(_in_value) *= *this);
                }

                template <typename type>
                typename enable_if<(supported<type>::value && !higher_precedence<type>::value), arithmetic_type>::type
                operator / (const type& _in_value) const {
                    return(*this / arithmetic_type(_in_value));
                }

                value_type representation;
            };

            template <typename _Type>
            struct arithmetic_traits : non_constructable_non_copyable {
                typedef _Type value_type;

                template <typename _TestType>
                struct supported : std::false_type {};

                template <typename _TestType>
                struct convertable : std::false_type {};

                template <typename _TestType>
                struct higher_precedence : std::false_type {};

                static wn_void negate(value_type& _in_out_value) {
                    _in_out_value = (-_in_out_value);
                }

                static wn_void add(value_type& _in_out_value, const value_type& _in_value) {
                    _in_out_value += _in_value;
                }

                static wn_void subtract(value_type& _in_out_value, const value_type& _in_value) {
                    _in_out_value -= _in_value;
                }

                static wn_void multiply(value_type& _in_out_value, const value_type& _in_value) {
                    _in_out_value *= _in_value;
                }

                static wn_void divide(value_type& _in_out_value, const value_type& _in_value) {
                    _in_out_value /= _in_value;
                }

                static wn_bool equal(const value_type& _in_value1, const value_type& _in_value2) {
                    return(_in_value1 == _in_value2);
                }

                static wn_bool less_than(const value_type& _in_value1, const value_type& _in_value2) {
                    return(_in_value1 < _in_value2);
                }

                static wn_bool greater_than(const value_type& _in_value1, const value_type& _in_value2) {
                    return(_in_value1 > _in_value2);
                }
            };

            template <typename _Type>
            struct size_of : index_constant<sizeof(_Type)> {};

            template <typename _Traits>
            struct size_of<arithmetic_type<_Traits>> : index_constant<sizeof(typename arithmetic_type<_Traits>::value_type)> {};

            namespace {
                template <typename _Type>
                struct select_scale_type {
                    static_assert(dependent_false<_Type>::value, "type is not a valid integer type");
                };

                template <>
                struct select_scale_type<wn_int8> {
                    typedef wn_int16 type;
                };

                template <>
                struct select_scale_type<wn_int16> {
                    typedef wn_int32 type;
                };

                template <>
                struct select_scale_type<wn_int32> {
                    typedef wn_int64 type;
                };

                template <>
                struct select_scale_type<wn_int64> {
                    typedef wn_int64 type;
                };

                template <>
                struct select_scale_type<wn_uint8> {
                    typedef wn_uint16 type;
                };

                template <>
                struct select_scale_type<wn_uint16> {
                    typedef wn_uint32 type;
                };

                template <>
                struct select_scale_type<wn_uint32> {
                    typedef wn_uint64 type;
                };

                template <>
                struct select_scale_type<wn_uint64> {
                    typedef wn_uint64 type;
                };
            }

            template <typename _Type, const wn_uint8 _Precision>
            struct fixed_point_traits_base : arithmetic_traits<_Type> {
                typedef typename arithmetic_traits<_Type>::value_type value_type;
                static_assert(std::is_integral<value_type>::value && std::is_arithmetic<value_type>::value,
                              "Internal type must be an integer type");
                static_assert(sizeof(value_type) <= 8, "type must be no more than 64 bits");

                typedef typename select_scale_type<value_type>::type scale_type;

                enum : value_type {
                    one = (static_cast<value_type>(1) << _Precision),
                    precision = _Precision
                };

                template <typename _TestType>
                using supported = bool_constant<(std::is_integral<_TestType>::value || is_floating_point<_TestType>::value ||
                                                    is_fixed_point<_TestType>::value)>;

                template <typename _TestType>
                using convertable = is_fixed_point<_TestType>;

                template <typename _TestType>
                using higher_precedence = bool_constant<(is_floating_point<_TestType>::value ||
                                                            (is_fixed_point<_TestType>::value &&
                                                             (size_of<value_type>::value < size_of<_TestType>::value)))>;

                template <typename type>
                static typename enable_if<std::is_integral<type>::value, value_type>::type construct(const type& _in_value) {
                    return(static_cast<value_type>(_in_value) << precision);
                }

                template <typename type>
                static typename enable_if<(is_fixed_point<type>::value && precision == type::traits_type::precision),
                                               value_type>::type construct(const typename type::value_type& _in_value) {
                    return(static_cast<value_type>(_in_value));
                }

                template <typename type>
                static typename enable_if<(is_fixed_point<type>::value && precision > type::traits_type::precision),
                                               value_type>::type construct(const typename type::value_type& _in_value) {
                    return(static_cast<value_type>(_in_value) << (precision - type::traits_type::template precision));
                }

                template <typename type>
                static typename enable_if<(is_fixed_point<type>::value && precision < type::traits_type::precision),
                                               value_type>::type construct(const typename type::value_type& _in_value) {
                    return(static_cast<value_type>(_in_value >> (type::traits_type::template precision - precision)));
                }

                template <typename type>
                static typename enable_if<std::is_floating_point<type>::value, value_type>::type
                construct(const type& _in_value) {
                    const type zero = static_cast<type>(0);
                    const type half = static_cast<type>(0.5);
                    const type adjustment = (_in_value >= zero ? half : -half);

                    return(static_cast<value_type>((_in_value * static_cast<value_type>(one)) + adjustment));
                }

                template <typename type>
                static typename enable_if<(is_floating_point<type>::value && !std::is_floating_point<type>::value),
                                               value_type>::type construct(const type& _in_value) {
                    const wn_float32 in_value = static_cast<wn_float32>(_in_value);
                    const wn_float32 adjustment = (_in_value >= 0.0f ? 0.5f : -0.5f);

                    return(static_cast<value_type>((in_value * static_cast<value_type>(one)) + adjustment));
                }

                template <typename type>
                static typename enable_if<is_same_decayed<type, wn_bool>::value, wn_bool>::type
                convert(const value_type& _in_value) {
                    return(_in_value == 0 ? wn_false : wn_true);
                }

                template <typename type>
                static typename enable_if<(std::is_integral<type>::value && !is_same_decayed<type, wn_bool>::value),
                                               type>::type convert(const value_type& _in_value) {
                    return(static_cast<type>(_in_value >> precision));
                }

                template <typename type>
                static typename enable_if<is_floating_point<type>::value, type>::type convert(const value_type& _in_value) {
                    return(static_cast<type>(_in_value) / static_cast<value_type>(one));
                }
            };

            template <typename _Type, const wn_uint8 _Precision>
            struct fixed_point_traits : fixed_point_traits_base<_Type, _Precision> {
                typedef typename fixed_point_traits_base<_Type, _Precision>::value_type value_type;
                typedef typename fixed_point_traits_base<_Type, _Precision>::scale_type scale_type;
                enum : value_type {
                    one = (static_cast<value_type>(1) << _Precision),
                    precision = _Precision
                };
                static wn_void multiply(value_type& _in_out_value, const value_type& _in_value) {
                    const scale_type in_out_value = static_cast<scale_type>(_in_out_value);
                    const scale_type in_value = static_cast<scale_type>(_in_value);

                    _in_out_value = static_cast<value_type>((in_out_value * in_value) >> precision);
                }

                static wn_void divide(value_type& _in_out_value, const value_type& _in_value) {
                    const scale_type in_out_value = static_cast<scale_type>(_in_out_value);

                    _in_out_value = static_cast<value_type>((in_out_value << precision) / _in_value);
                }
            };

            template <const wn_uint8 precision>
            struct fixed_point_traits<wn_int64, precision> : fixed_point_traits_base<wn_int64, precision> {
                typedef typename fixed_point_traits_base<wn_int64, precision>::value_type value_type;
                static wn_void multiply(wn_int64& _in_out_value, const wn_int64& _in_value) {
                    #if defined _WN_WINDOWS && defined _WN_64_BIT
                        _in_out_value = static_cast<value_type>(::MultiplyExtract128(_in_out_value, _in_value, precision));
                    #else
                        const wn_uint64 a1 = _in_out_value >> precision;
                        const wn_uint64 a0 = _in_out_value - (a1 << precision);
                        const wn_uint64 b1 = _in_value >> precision;
                        const wn_uint64 b0 = _in_value - (b1 << precision);
                        const wn_uint64 d = a0 * b0;
                        const wn_uint64 d1 = d >> precision;
                        const wn_uint64 d0 = d - (d1 << precision);
                        const wn_uint64 e = a0 * b1;
                        const wn_uint64 e1 = e >> precision;
                        const wn_uint64 e0 = e - (e1 << precision);
                        const wn_uint64 f = a1 * b0;
                        const wn_uint64 f1 = f >> precision;
                        const wn_uint64 f0 = f - (f1 << precision);
                        const wn_uint64 g = a1 * b1;
                        const wn_uint64 g1 = g >> precision;
                        const wn_uint64 g0 = g - (g1 << precision);
                        const wn_uint64 roll = 1ULL << precision;
                        const wn_uint64 pmax = roll - 1;
                        wn_uint64 sum = d1 + e0 + f0;
                        wn_uint64 carry = 0;

                        while (pmax < sum) {
                            sum -= roll;
                            carry++;
                        }

                        const wn_int64 high_product = carry + e1 + f1 + g0 + (g1 << precision);
                        const wn_int64 low_product = d0 + (sum << precision);
                        wn_uint64 unsigned_high_product = static_cast<wn_uint64>(high_product);
                        wn_uint64 unsigned_low_product = static_cast<wn_uint64>(low_product);
                        wn_bool negate = wn_false;

                        if (high_product < 0) {
                            negate = wn_true;
                            unsigned_high_product = static_cast<wn_uint64>(-high_product);
                            unsigned_low_product = static_cast<wn_uint64>(-low_product);

                            if (unsigned_low_product != 0) {
                                unsigned_high_product -= 1;
                            }
                        }

                        wn_int64 extracted_product = static_cast<wn_int64>((unsigned_high_product << precision) |
                                                                           (unsigned_low_product >> precision));

                        if (negate) {
                            extracted_product = -extracted_product;
                        }

                        _in_out_value = extracted_product;
                    #endif
                }

                static wn_void divide(wn_int64& _in_out_value, const wn_int64& _in_value) {
                    wn_int64 quotient = _in_out_value;
                    wn_int64 divisor = _in_value;
                    wn_bool negate = wn_false;

                    if (divisor < 0) {
                        divisor = -divisor;
                        negate = !negate;
                    }

                    if (quotient < 0) {
                        quotient = -quotient;
                        negate = !negate;
                    }

                    wn_uint64 quotient_hi = quotient >> precision;
                    wn_uint64 quotient_low = quotient << precision;
                    wn_int64 remainder = 0;

                    for (auto i = 0; i < 128; ++i) {
                        static const wn_uint64 highest_bit = 0x8000000000000000ull;
                        const wn_uint64 high_bit = quotient_hi & highest_bit;
                        const wn_uint64 shift_bit = quotient_low & highest_bit;

                        remainder <<= 1;

                        if (high_bit) {
                            remainder |= 1;
                        }

                        quotient_hi <<= 1;
                        quotient_low <<= 1;

                        if (shift_bit) {
                            quotient_hi |= 1;
                        }

                        if (remainder >= divisor) {
                            remainder -= divisor;
                            quotient_low |= 1;
                        }
                    }

                    if (negate) {
                        quotient_low = static_cast<wn_uint64>(-static_cast<wn_int64>(quotient_low));
                    }

                    _in_out_value = static_cast<wn_int64>(quotient_low);
                }
            };

            template <typename _Type, typename _conversion_type>
            struct floating_point_traits : arithmetic_traits<_Type> {
                typedef typename arithmetic_traits<_Type>::value_type value_type;
                typedef _conversion_type conversion_type;
                template <typename type>
                struct supported : bool_constant<(std::is_integral<type>::value || is_floating_point<type>::value ||
                                                       is_fixed_point<type>::value)> {};

                template <typename type>
                struct higher_precedence : bool_constant<(is_floating_point<type>::value &&
                                                               (size_of<value_type>::value < size_of<type>::value))> {};

                template <typename type>
                static typename enable_if<supported<type>::value, value_type>::type construct(const type& _in_value) {
                    return(conversion_type::compress(static_cast<wn_float32>(_in_value)));
                }

                template <typename type>
                static typename enable_if<is_same_decayed<type, wn_bool>::value, wn_bool>::type
                convert(const value_type& _in_value) {
                    return(conversion_type::decompress(_in_value) == 0.0f ? wn_false : wn_true);
                }

                template <typename type>
                static typename enable_if<(supported<type>::value && !is_same_decayed<type, wn_bool>::value), type>::type
                convert(const value_type& _in_value) {
                    return(static_cast<type>(conversion_type::decompress(_in_value)));
                }

                static wn_void negate(value_type& _in_out_value) {
                    wn_float32 out_value = conversion_type::decompress(_in_out_value);

                    out_value = -out_value;

                    _in_out_value = conversion_type::compress(out_value);
                }

                static wn_void add(value_type& _in_out_value, const value_type& _in_value) {
                    wn_float32 out_value = conversion_type::decompress(_in_out_value);

                    out_value += conversion_type::decompress(_in_value);

                    _in_out_value = conversion_type::compress(out_value);
                }

                static wn_void subtract(value_type& _in_out_value, const value_type& _in_value) {
                    wn_float32 out_value = conversion_type::decompress(_in_out_value);

                    out_value -= conversion_type::decompress(_in_value);

                    _in_out_value = conversion_type::compress(out_value);
                }

                static wn_void multiply(value_type& _in_out_value, const value_type& _in_value) {
                    wn_float32 out_value = conversion_type::decompress(_in_out_value);

                    out_value *= conversion_type::decompress(_in_value);

                    _in_out_value = conversion_type::compress(out_value);
                }

                static wn_void divide(value_type& _in_out_value, const value_type& _in_value) {
                    wn_float32 out_value = conversion_type::decompress(_in_out_value);

                    out_value /= conversion_type::decompress(_in_value);

                    _in_out_value = conversion_type::compress(out_value);
                }

                static wn_bool equal(const value_type& _in_value1, const value_type& _in_value2) {
                    return(conversion_type::decompress(_in_value1) == conversion_type::decompress(_in_value2));
                }

                static wn_bool less_than(const value_type& _in_value1, const value_type& _in_value2) {
                    return(conversion_type::decompress(_in_value1) < conversion_type::decompress(_in_value2));
                }

                static wn_bool greater_than(const value_type& _in_value1, const value_type& _in_value2) {
                    return(conversion_type::decompress(_in_value1) > conversion_type::decompress(_in_value2));
                }
            };

            struct float_conversions : non_constructable_non_copyable {
            protected:
                union bits {
                    wn_float32 f32;
                    wn_int32 si32;
                    wn_uint32 ui32;
                };
            };

            struct float8_conversions : float_conversions {
                static wn_uint8 compress(const wn_float32& _in_value) {
                    WN_UNUSED_ARGUMENT(_in_value);

                    return(1);
                }

                static wn_float32 decompress(const wn_uint8& _in_value) {
                    WN_UNUSED_ARGUMENT(_in_value);

                    return(1.0f);
                }
            };

            struct float16_conversions : float_conversions {
                static wn_uint16 compress(const wn_float32& _in_value) {
                    #ifdef __WN_F16C_AVAILABLE
                        return(static_cast<wn_uint16>(_mm_cvtsi128_si32(_mm_cvtps_ph(_mm_set_ss(_in_value), 0))));
                    #else
                        bits in_value_raw;

                        in_value_raw.f32 = _in_value;

                        wn_uint32 sign = in_value_raw.si32 & f32_sign_bit;

                        in_value_raw.si32 ^= sign;
                        sign >>= f16_sign_shift;

                        bits temp_raw;

                        temp_raw.si32 = mulN;

                        const wn_float32 fix_subnormals = temp_raw.f32 * in_value_raw.f32;

                        temp_raw.si32 = *reinterpret_cast<const wn_int32*>(&fix_subnormals);
                        in_value_raw.si32 ^= (temp_raw.si32 ^ in_value_raw.si32) & -(f16_min_normal_as_f32 > in_value_raw.si32);
                        in_value_raw.si32 ^= (f32_infinity ^ in_value_raw.si32) &
                                             -((f32_infinity > in_value_raw.si32) & (in_value_raw.si32 > f16_max_normal_as_f32));
                        in_value_raw.si32 ^= (f16_min_nan_as_f32 ^ in_value_raw.si32) &
                                             -((f16_min_nan_as_f32 > in_value_raw.si32) & (in_value_raw.si32 > f32_infinity));
                        in_value_raw.ui32 >>= f16_shift;
                        in_value_raw.si32 ^= ((in_value_raw.si32 - maxD) ^ in_value_raw.si32) &
                                             -(in_value_raw.si32 > f16_max_normal);
                        in_value_raw.si32 ^= ((in_value_raw.si32 - minD) ^ in_value_raw.si32) &
                                             -(in_value_raw.si32 > f32_max_subnormal_downshifted);

                        return(static_cast<wn_uint16>(in_value_raw.ui32 | sign));
                    #endif
                }

                static wn_float32 decompress(const wn_uint16& _in_value) {
                    #ifdef __WN_F16C_AVAILABLE
                        return(_mm_cvtss_f32(_mm_cvtph_ps(_mm_cvtsi32_si128(static_cast<wn_uint32>(_in_value)))));
                    #else
                        bits in_value_raw;

                        in_value_raw.ui32 = _in_value;

                        wn_int32 sign = in_value_raw.si32 & f16_sign_bit;

                        in_value_raw.si32 ^= sign;
                        sign <<= f16_sign_shift;
                        in_value_raw.si32 ^= ((in_value_raw.si32 + minD) ^ in_value_raw.si32) &
                                             -(in_value_raw.si32 > f32_max_subnormal_downshifted);
                        in_value_raw.si32 ^= ((in_value_raw.si32 + maxD) ^ in_value_raw.si32) &
                                             -(in_value_raw.si32 > f16_max_normal);

                        bits temp_raw;

                        temp_raw.si32 = mulC;
                        temp_raw.f32 *= in_value_raw.si32;

                        const wn_int32 mask = -(f32_min_normal_downshifted > in_value_raw.si32);

                        in_value_raw.si32 <<= f16_shift;
                        in_value_raw.si32 ^= (temp_raw.si32 ^ in_value_raw.si32) & mask;
                        in_value_raw.si32 |= sign;

                        return(in_value_raw.f32);
                    #endif
                }

            private:
                enum : wn_uint32 {
                    f16_shift = 13,
                    f16_sign_shift = 16,
                    f32_infinity = 0x7F800000, // float32 infinity
                    f16_max_normal_as_f32 = 0x477FE000, // max float16 normal as a float32
                    f16_min_normal_as_f32 = 0x38800000, // min float16 normal as a float32
                    f32_sign_bit = 0x80000000, // float32 sign bit
                    f16_infinity = f32_infinity >> f16_shift,
                    f16_min_nan_as_f32 = (f16_infinity + 1) << f16_shift, // minimum float16 nan as a float32
                    f16_max_normal = f16_max_normal_as_f32 >> f16_shift,
                    f16_min_normal = f16_min_normal_as_f32 >> f16_shift,
                    f16_sign_bit = f32_sign_bit >> f16_sign_shift, // float16 sign bit
                    mulN = 0x52000000, // (1 << 23) / f16_min_normal_as_f32
                    mulC = 0x33800000, // f16_min_normal_as_f32 / (1 << (23 - f16_shift))
                    f32_max_subnormal_downshifted = 0x003FF, // max float32 subnormal down shifted
                    f32_min_normal_downshifted = 0x00400, // min float32 normal down shifted
                    maxD = f16_infinity - f16_max_normal - 1,
                    minD = f16_min_normal - f32_max_subnormal_downshifted - 1
                };
            };
        }
    }
}

template <typename _Type1, typename _Type2>
typename wn::core::enable_if<(!wn::core::is_extended_type<_Type1>::value && wn::core::is_extended_type<_Type2>::value &&
                        (_Type2::template supported<_Type1>::value) &&
                        (_Type2::template higher_precedence<_Type1>::value)), _Type1>::type
operator + (const _Type1& _in_value1, const _Type2& _in_value2) {
    return(_in_value1 + _Type1(_in_value2));
}

template <typename _Type1, typename _Type2>
typename wn::core::enable_if<(!wn::core::is_extended_type<_Type1>::value && wn::core::is_extended_type<_Type2>::value &&
                        (_Type2::template supported<_Type1>::value) &&
                        !(_Type2::template higher_precedence<_Type1>::value)), _Type2>::type
operator + (const _Type1& _in_value1, const _Type2& _in_value2) {
    return(_Type2(_in_value1) + _in_value2);
}

template <typename _Type1, typename _Type2>
typename wn::core::enable_if<(!wn::core::is_extended_type<_Type1>::value && wn::core::is_extended_type<_Type2>::value &&
                        (_Type2::template supported<_Type1>::value) &&
                        (_Type2::template higher_precedence<_Type1>::value)), _Type1>::type
operator - (const _Type1& _in_value1, const _Type2& _in_value2) {
    return(_in_value1 - _Type1(_in_value2));
}

template <typename _Type1, typename _Type2>
typename wn::core::enable_if<(!wn::core::is_extended_type<_Type1>::value && wn::core::is_extended_type<_Type2>::value &&
                        (_Type2::template supported<_Type1>::value) &&
                        !(_Type2::template higher_precedence<_Type1>::value)), _Type2>::type
operator - (const _Type1& _in_value1, const _Type2& _in_value2) {
    return(_Type2(_in_value1) - _in_value2);
}

template <typename _Type1, typename _Type2>
typename wn::core::enable_if<(!wn::core::is_extended_type<_Type1>::value && wn::core::is_extended_type<_Type2>::value &&
                        (_Type2::template supported<_Type1>::value) &&
                        (_Type2::template higher_precedence<_Type1>::value)), _Type1>::type
operator * (const _Type1& _in_value1, const _Type2& _in_value2) {
    return(_in_value1 * _Type1(_in_value2));
}

template <typename _Type1, typename _Type2>
typename wn::core::enable_if<(!wn::core::is_extended_type<_Type1>::value && wn::core::is_extended_type<_Type2>::value &&
                        (_Type2::template supported<_Type1>::value) &&
                        !(_Type2::template higher_precedence<_Type1>::value)), _Type2>::type
operator * (const _Type1& _in_value1, const _Type2& _in_value2) {
    return(_Type2(_in_value1) * _in_value2);
}

template <typename _Type1, typename _Type2>
typename wn::core::enable_if<(!wn::core::is_extended_type<_Type1>::value && wn::core::is_extended_type<_Type2>::value && (_Type2::template supported<_Type1>::value) && (_Type2::template higher_precedence<_Type1>::value)), _Type1>::type operator / (const _Type1& _in_value1, const _Type2& _in_value2) {
    return(_in_value1 / _Type1(_in_value2));
}

template <typename _Type1, typename _Type2>
typename wn::core::enable_if<(!wn::core::is_extended_type<_Type1>::value && wn::core::is_extended_type<_Type2>::value && (_Type2::template supported<_Type1>::value) && !(_Type2::template higher_precedence<_Type1>::value)), _Type2>::type operator / (const _Type1& _in_value1, const _Type2& _in_value2) {
    return(_Type2(_in_value1) / _in_value2);
}

template <typename _Type1, typename _Type2>
typename wn::core::enable_if<(!wn::core::is_extended_type<_Type1>::value && wn::core::is_extended_type<_Type2>::value && (_Type2::template supported<_Type1>::value) && (_Type2::template higher_precedence<_Type1>::value)), _Type1>::type operator == (const _Type1& _in_value1, const _Type2& _in_value2) {
    return(_in_value1 == _Type1(_in_value2));
}

template <typename _Type1, typename _Type2>
typename wn::core::enable_if<(!wn::core::is_extended_type<_Type1>::value && wn::core::is_extended_type<_Type2>::value && (_Type2::template supported<_Type1>::value) && !(_Type2::template higher_precedence<_Type1>::value)), _Type2>::type operator == (const _Type1& _in_value1, const _Type2& _in_value2) {
    return(_Type2(_in_value1) == _in_value2);
}

template <typename _Type1, typename _Type2>
typename wn::core::enable_if<(!wn::core::is_extended_type<_Type1>::value && wn::core::is_extended_type<_Type2>::value && (_Type2::template supported<_Type1>::value) && (_Type2::template higher_precedence<_Type1>::value)), _Type1>::type operator < (const _Type1& _in_value1, const _Type2& _in_value2) {
    return(_in_value1 < _Type1(_in_value2));
}

template <typename _Type1, typename _Type2>
typename wn::core::enable_if<(!wn::core::is_extended_type<_Type1>::value && wn::core::is_extended_type<_Type2>::value && (_Type2::template supported<_Type1>::value) && !(_Type2::template higher_precedence<_Type1>::value)), _Type2>::type operator < (const _Type1& _in_value1, const _Type2& _in_value2) {
    return(_Type2(_in_value1) < _in_value2);
}

template <typename _Type1, typename _Type2>
typename wn::core::enable_if<(!wn::core::is_extended_type<_Type1>::value && wn::core::is_extended_type<_Type2>::value && (_Type2::template supported<_Type1>::value) && (_Type2::template higher_precedence<_Type1>::value)), _Type1>::type operator <= (const _Type1& _in_value1, const _Type2& _in_value2) {
    return(_in_value1 <= _Type1(_in_value2));
}

template <typename _Type1, typename _Type2>
typename wn::core::enable_if<(!wn::core::is_extended_type<_Type1>::value && wn::core::is_extended_type<_Type2>::value && (_Type2::template supported<_Type1>::value) && !(_Type2::template higher_precedence<_Type1>::value)), _Type2>::type operator <= (const _Type1& _in_value1, const _Type2& _in_value2) {
    return(_Type2(_in_value1) <= _in_value2);
}

template <typename _Type1, typename _Type2>
typename wn::core::enable_if<(!wn::core::is_extended_type<_Type1>::value && wn::core::is_extended_type<_Type2>::value && (_Type2::template supported<_Type1>::value) && (_Type2::template higher_precedence<_Type1>::value)), _Type1>::type operator > (const _Type1& _in_value1, const _Type2& _in_value2) {
    return(_in_value1 > _Type1(_in_value2));
}

template <typename _Type1, typename _Type2>
typename wn::core::enable_if<(!wn::core::is_extended_type<_Type1>::value && wn::core::is_extended_type<_Type2>::value && (_Type2::template supported<_Type1>::value) && !(_Type2::template higher_precedence<_Type1>::value)), _Type2>::type operator > (const _Type1& _in_value1, const _Type2& _in_value2) {
    return(_Type2(_in_value1) > _in_value2);
}

template <typename _Type1, typename _Type2>
typename wn::core::enable_if<(!wn::core::is_extended_type<_Type1>::value && wn::core::is_extended_type<_Type2>::value && (_Type2::template supported<_Type1>::value) && (_Type2::template higher_precedence<_Type1>::value)), _Type1>::type operator >= (const _Type1& _in_value1, const _Type2& _in_value2) {
    return(_in_value1 >= _Type1(_in_value2));
}

template <typename _Type1, typename _Type2>
typename wn::core::enable_if<(!wn::core::is_extended_type<_Type1>::value && wn::core::is_extended_type<_Type2>::value && (_Type2::template supported<_Type1>::value) && !(_Type2::template higher_precedence<_Type1>::value)), _Type2>::type operator >= (const _Type1& _in_value1, const _Type2& _in_value2) {
    return(_Type2(_in_value1) >= _in_value2);
}

#endif // __WN_CORE_EXTENDED_TYPES_H__
