// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_GENERIC_BASIC_TRAITS_H__
#define __WN_MATH_INTERNAL_GENERIC_BASIC_TRAITS_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNCore/inc/WNUtility.h"
#include "WNMath/inc/WNConfig.h"

#include <cmath>

namespace wn {
    namespace internal {
        namespace math {
            struct basic_traits_generic : core::non_constructable_non_copyable {
                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<(std::is_integral<type>::value &&
                                                                 core::is_signed<type>::value), type>::type abs(const type& _value) {
                    const type negate = static_cast<type>(_value < 0);

                    return((_value ^ -negate) + negate);
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<(core::is_fixed_point<type>::value &&
                                                                 core::is_signed<type>::value), type>::type abs(const type& _value) {
                    type value;

                    value.representation = abs(_value.representation);

                    return(value);
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<(core::is_floating_point<type>::value &&
                                                                 std::is_fundamental<type>::value), type>::type
                abs(const type& _value) {
                    return(::fabs(_value));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<(core::is_floating_point<type>::value &&
                                                                 !std::is_fundamental<type>::value), type>::type
                abs(const type& _value) {
                    return(static_cast<type>(abs(static_cast<wn_float32>(_value))));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<(!std::is_integral<type>::value &&
                                                                 !core::is_floating_point<type>::value &&
                                                                 !core::is_fixed_point<type>::value &&
                                                                 core::is_signed<type>::value), type>::type abs(const type& _value) {
                    return(_value < static_cast<type>(0) ? -_value : _value);
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<std::is_integral<type>::value, type>::type
                mod(const type& _dividend, const type& _divisor) {
                    return(_dividend % _divisor);
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<core::is_fixed_point<type>::value, type>::type
                mod(const type& _dividend, const type& _divisor) {
                    type value;

                    value.representation = mod(_dividend.representation, _divisor.representation);

                    return(value);
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<std::is_floating_point<type>::value, type>::type
                mod(const type& _dividend, const type& _divisor) {
                    return(::fmod(_dividend, _divisor));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<(core::is_floating_point<type>::value &&
                                                                 !std::is_floating_point<type>::value), type>::type
                mod(const type& _dividend, const type& _divisor) {
                    return(static_cast<type>(mod(static_cast<wn_float32>(_dividend), static_cast<wn_float32>(_divisor))));
                }

                static WN_FORCE_INLINE wn_float32 sqrt(const wn_float32& _value) {
                    #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                        const wn_uint32 equal_zero = -(_value == 0.0f);
                        const wn_float32 result = invsqrt(_value);
                        wn_int32 int_value = *reinterpret_cast<const wn_int32*>(&result);

                        int_value &= ~(equal_zero);

                        return(_value * (*reinterpret_cast<const wn_float32*>(&int_value)));
                    #else
                        return(::sqrt(_value));
                    #endif
                }

                static WN_FORCE_INLINE wn_float64 sqrt(const wn_float64& _value) {
                    return(::sqrt(_value));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<std::is_integral<type>::value, type>::type
                sqrt(const type& _value) {
                    return(static_cast<type>(sqrt(static_cast<wn_float64>(_value))));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<(core::is_floating_point<type>::value &&
                                                                 !std::is_floating_point<type>::value), type>::type
                sqrt(const type& _value) {
                    return(static_cast<type>(sqrt(static_cast<wn_float32>(_value))));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<(std::is_floating_point<type>::value &&
                                                                 !core::is_same<type, wn_float32>::value &&
                                                                 !core::is_same<type, wn_float64>::value), type>::type
                sqrt(const type& _value) {
                    return(::sqrt(_value));
                }

                static WN_FORCE_INLINE wn_float32 invsqrt(const wn_float32& _value) {
                    #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                        const wn_uint32 less_than_zero = -(_value < 0.0f);
                        const wn_uint32 equal_zero = -(_value == 0.0f);
                        const wn_float32 half_number = _value * 0.5f;
                        wn_int32 int_value = *reinterpret_cast<const wn_int32*>(&_value);

                        int_value = 0x5F375A86 - (int_value >> 1);

                        wn_float32 real_value = *reinterpret_cast<const wn_float32*>(&int_value);

                        real_value = real_value * (1.5f - real_value * real_value * half_number);
                        int_value = *reinterpret_cast<const wn_int32*>(&real_value);
                        int_value = (int_value & ~less_than_zero) ^ (less_than_zero & 0xFFC00000);
                        int_value = (int_value & 0x80000000) | ((int_value & ~equal_zero) ^ (equal_zero & 0x7F800000));

                        return(*reinterpret_cast<const wn_float32*>(&int_value));
                    #else
                        return(1.0f / sqrt(_value));
                    #endif
                }

                static WN_FORCE_INLINE wn_float64 invsqrt(const wn_float64& _value) {
                    return(1.0 / sqrt(_value));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<(core::is_floating_point<type>::value &&
                                                                 !std::is_floating_point<type>::value), type>::type
                invsqrt(const type& _value) {
                    return(static_cast<type>(invsqrt(static_cast<wn_float32>(_value))));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<(std::is_floating_point<type>::value &&
                                                                 !core::is_same<type, wn_float32>::value &&
                                                                 !core::is_same<type, wn_float64>::value), type>::type
                invsqrt(const type& _value) {
                    return(static_cast<type>(1.0) / sqrt(_value));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<std::is_integral<type>::value, type>::type
                min(const type& _value1, const type& _value2) {
                    return(_value2 ^ ((_value1 ^ _value2) & -(_value1 < _value2)));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<std::is_floating_point<type>::value, type>::type
                min(const type& _value1, const type& _value2) {
                    return(::fmin(_value1, _value2));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<(core::is_floating_point<type>::value &&
                                                                 !std::is_floating_point<type>::value), type>::type
                min(const type& _value1, const type& _value2) {
                    return(static_cast<type>(min(static_cast<wn_float32>(_value1), static_cast<wn_float32>(_value2))));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<(!std::is_integral<type>::value &&
                                                                 !core::is_floating_point<type>::value), type>::type
                min(const type& _value1, const type& _value2) {
                    return(_value1 < _value2 ? _value1 : _value2);
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<std::is_integral<type>::value, type>::type
                max(const type& _value1, const type& _value2) {
                    return(_value1 ^ ((_value1 ^ _value2) & -(_value1 < _value2)));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<std::is_floating_point<type>::value, type>::type
                max(const type& _value1, const type& _value2) {
                    return(::fmax(_value1, _value2));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<(core::is_floating_point<type>::value &&
                                                                 !std::is_floating_point<type>::value), type>::type
                max(const type& _value1, const type& _value2) {
                    return(static_cast<type>(max(static_cast<wn_float32>(_value1), static_cast<wn_float32>(_value2))));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<(!std::is_integral<type>::value &&
                                                                 !core::is_floating_point<type>::value), type>::type
                max(const type& _value1, const type& _value2) {
                    return(_value1 > _value2 ? _value1 : _value2);
                }

                template <typename type>
                static WN_FORCE_INLINE type clamp(const type& _value, const type& _min, const type& _max) {
                    return(min(max(_value, _min), _max));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<std::is_integral<type>::value, type>::type
                pow(const type& _base, const type& _exponent) {
                    type base = _base;
                    type exponent = _exponent;
                    type result = 1;

                    while (exponent) {
                        if (exponent & 1) {
                            result *= base;
                        }

                        exponent >>= 1;
                        base *= base;
                    }

                    return(result);
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<core::is_fixed_point<type>::value, type>::type
                pow(const type& _base, const type& _exponent) {
                    type value;

                    value.representation = pow(_base.representation, _exponent.representation);

                    return(value);
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<(core::is_floating_point<type>::value &&
                                                                 !std::is_floating_point<type>::value), type>::type
                pow(const type& _base, const type& _exponent) {
                    return(static_cast<type>(pow(static_cast<wn_float32>(_base), static_cast<wn_float32>(_exponent))));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<std::is_floating_point<type>::value, type>::type
                pow(const type& _base, const type& _exponent) {
                    return(::pow(_base, _exponent));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<core::is_fixed_point<type>::value, type>::type ceil(const type& _value) {
                    typedef typename type::value_type value_type;

                    const value_type mask = pow2<value_type, type::traits_type::precision>::value - 1;
                    type value;

                    value.representation = _value.representation & ~mask;
                    value += static_cast<type>((_value.representation & mask) ? 1 : 0);

                    return(value);
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<(core::is_floating_point<type>::value &&
                                                                 !std::is_floating_point<type>::value), type>::type
                ceil(const type& _value) {
                    return(static_cast<type>(ceil(static_cast<wn_float32>(_value))));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<std::is_floating_point<type>::value, type>::type
                ceil(const type& _value) {
                    return(::ceil(_value));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<core::is_fixed_point<type>::value, type>::type floor(const type& _value) {
                    typedef typename type::value_type value_type;

                    const value_type mask = pow2<value_type, type::traits_type::precision>::value - 1;
                    type value;

                    value.representation = _value.representation & ~mask;

                    return(value);
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<(core::is_floating_point<type>::value &&
                                                                 !std::is_floating_point<type>::value), type>::type
                floor(const type& _value) {
                    return(static_cast<type>(floor(static_cast<wn_float32>(_value))));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<std::is_floating_point<type>::value, type>::type
                floor(const type& _value) {
                    return(::floor(_value));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<core::is_fixed_point<type>::value, type>::type round(const type& _value) {
                    return(floor(_value + static_cast<type>(0.5)));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<(core::is_floating_point<type>::value &&
                                                                 !std::is_floating_point<type>::value), type>::type
                round(const type& _value) {
                    return(static_cast<type>(round(static_cast<wn_float32>(_value))));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<std::is_floating_point<type>::value, type>::type
                round(const type& _value) {
                    #ifdef __WN_HAS_CPP11_STD_ROUND
                        return(::round(_value));
                    #else
                        return(floor(_value + static_cast<type>(0.5)));
                    #endif
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<core::is_fixed_point<type>::value, type>::type trunc(const type& _value) {
                    if (_value > 0) {
                        return(floor(_value + static_cast<type>(0.5)));
                    } else if (_value < 0) {
                        return(ceil(_value - static_cast<type>(0.5)));
                    }

                    return(_value);
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<(core::is_floating_point<type>::value &&
                                                                 !std::is_floating_point<type>::value), type>::type
                trunc(const type& _value) {
                    return(static_cast<type>(trunc(static_cast<wn_float32>(_value))));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<std::is_floating_point<type>::value, type>::type
                trunc(const type& _value) {
                    return(::trunc(_value));
                }

            private:
                template <typename type, const size_t exponent>
                struct pow2 : core::integral_constant<type, (2 * pow2<type, (exponent - 1)>::value)> {};

                template <typename type>
                struct pow2<type, 0> : core::integral_constant<type, 1> {};
            };
        }
    }
}

#endif // __WN_MATH_INTERNAL_GENERIC_BASIC_TRAITS_H__
