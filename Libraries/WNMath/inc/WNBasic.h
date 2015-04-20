// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_BASIC_H__
#define __WN_MATH_BASIC_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNMath/inc/WNConstants.h"
#include "WNMath/inc/Internal/WNBasicTraits.h"

namespace wn {
    template <typename type_>
    WN_FORCE_INLINE typename enable_if<is_signed<type_>::value, type_>::type abs(const type_& _value) {
        return(internal::math::basic_traits::abs<type_>(_value));
    }

    template <typename type_>
    WN_FORCE_INLINE typename enable_if<is_real<type_>::value, type_>::type ceil(const type_& _value) {
        return(internal::math::basic_traits::ceil(_value));
    }

    template <typename type_>
    WN_FORCE_INLINE typename enable_if<is_real<type_>::value, type_>::type floor(const type_& _value) {
        return(internal::math::basic_traits::floor(_value));
    }

    template <typename type_>
    WN_FORCE_INLINE typename enable_if<is_real<type_>::value, type_>::type round(const type_& _value) {
        return(internal::math::basic_traits::round(_value));
    }

    template <typename type_>
    WN_FORCE_INLINE typename enable_if<is_real<type_>::value, type_>::type trunc(const type_& _value) {
        return(internal::math::basic_traits::trunc(_value));
    }

    template <typename type_>
    WN_FORCE_INLINE typename enable_if<(is_real<type_>::value || std::is_integral<type_>::value), type_>::type
    mod(const type_& _dividend, const type_& _divisor) {
        return(internal::math::basic_traits::mod(_dividend, _divisor));
    }

    template <typename type_>
    WN_FORCE_INLINE typename enable_if<(is_real<type_>::value || std::is_integral<type_>::value), type_>::type
    sqrt(const type_& _value) {
        return(internal::math::basic_traits::sqrt(_value));
    }

    template <typename type_>
    WN_FORCE_INLINE typename enable_if<is_real<type_>::value, type_>::type invsqrt(const type_& _value) {
        return(internal::math::basic_traits::invsqrt(_value));
    }

    template <typename type_>
    WN_FORCE_INLINE type_ pow(const type_& _base, const type_& _exponent) {
        return(internal::math::basic_traits::pow(_base, _exponent));
    }

    template <typename type_>
    WN_FORCE_INLINE type_ min(const type_& _value1, const type_& _value2) {
        return(internal::math::basic_traits::min(_value1, _value2));
    }

    template <typename type_>
    WN_FORCE_INLINE type_ max(const type_& _value1, const type_& _value2) {
        return(internal::math::basic_traits::max(_value1, _value2));
    }

    template <typename type_>
    WN_FORCE_INLINE type_ clamp(const type_& _value, const type_& _min, const type_& _max) {
        return(internal::math::basic_traits::clamp(_value, _min, _max));
    }

    template <typename type_>
    WN_FORCE_INLINE type_ deg_to_rad(const type_& _value) {
        return(static_cast<type_>(static_cast<wn_float64>(_value)* (WN_PI / 180.0)));
    }

    template <typename type_>
    WN_FORCE_INLINE type_ rad_to_deg(const type_& _value) {
        return(static_cast<type_>(static_cast<wn_float64>(_value)* (180.0 / WN_PI)));
    }
}

#endif // __WN_MATH_BASIC_H__
