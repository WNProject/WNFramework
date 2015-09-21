// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_BASIC_H__
#define __WN_MATH_BASIC_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNMath/inc/WNConstants.h"
#include "WNMath/inc/Internal/WNBasicTraits.h"

namespace wn {
    template <typename _Type>
    WN_FORCE_INLINE typename core::enable_if<core::is_signed<_Type>::value, _Type>::type abs(const _Type& _value) {
        return(internal::math::basic_traits::abs<_Type>(_value));
    }

    template <typename _Type>
    WN_FORCE_INLINE typename core::enable_if<core::is_real<_Type>::value, _Type>::type ceil(const _Type& _value) {
        return(internal::math::basic_traits::ceil(_value));
    }

    template <typename _Type>
    WN_FORCE_INLINE typename core::enable_if<core::is_real<_Type>::value, _Type>::type floor(const _Type& _value) {
        return(internal::math::basic_traits::floor(_value));
    }

    template <typename _Type>
    WN_FORCE_INLINE typename core::enable_if<core::is_real<_Type>::value, _Type>::type round(const _Type& _value) {
        return(internal::math::basic_traits::round(_value));
    }

    template <typename _Type>
    WN_FORCE_INLINE typename core::enable_if<core::is_real<_Type>::value, _Type>::type trunc(const _Type& _value) {
        return(internal::math::basic_traits::trunc(_value));
    }

    template <typename _Type>
    WN_FORCE_INLINE typename core::enable_if<(core::is_real<_Type>::value || std::is_integral<_Type>::value), _Type>::type
    mod(const _Type& _dividend, const _Type& _divisor) {
        return(internal::math::basic_traits::mod(_dividend, _divisor));
    }

    template <typename _Type>
    WN_FORCE_INLINE typename core::enable_if<(core::is_real<_Type>::value || std::is_integral<_Type>::value), _Type>::type
    sqrt(const _Type& _value) {
        return(internal::math::basic_traits::sqrt(_value));
    }

    template <typename _Type>
    WN_FORCE_INLINE typename core::enable_if<core::is_real<_Type>::value, _Type>::type invsqrt(const _Type& _value) {
        return(internal::math::basic_traits::invsqrt(_value));
    }

    template <typename _Type>
    WN_FORCE_INLINE _Type pow(const _Type& _base, const _Type& _exponent) {
        return(internal::math::basic_traits::pow(_base, _exponent));
    }

    template <typename _Type>
    WN_FORCE_INLINE _Type min(const _Type& _value1, const _Type& _value2) {
        return(internal::math::basic_traits::min(_value1, _value2));
    }

    template <typename _Type>
    WN_FORCE_INLINE _Type max(const _Type& _value1, const _Type& _value2) {
        return(internal::math::basic_traits::max(_value1, _value2));
    }

    template <typename _Type>
    WN_FORCE_INLINE _Type clamp(const _Type& _value, const _Type& _min, const _Type& _max) {
        return(internal::math::basic_traits::clamp(_value, _min, _max));
    }

    template <typename _Type>
    WN_FORCE_INLINE _Type deg_to_rad(const _Type& _value) {
        return(static_cast<_Type>(static_cast<wn_float64>(_value)* (WN_PI / 180.0)));
    }

    template <typename _Type>
    WN_FORCE_INLINE _Type rad_to_deg(const _Type& _value) {
        return(static_cast<_Type>(static_cast<wn_float64>(_value)* (180.0 / WN_PI)));
    }
}

#endif // __WN_MATH_BASIC_H__
