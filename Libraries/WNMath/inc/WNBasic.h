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
    template <typename type>
    WN_FORCE_INLINE typename enable_if<is_signed<type>::value, type>::type abs(const type& _value) {
        return(internal::math::basic_traits::abs(_value));
    }

    template <typename type>
    WN_FORCE_INLINE typename enable_if<is_real<type>::value, type>::type ceil(const type& _value) {
        return(internal::math::basic_traits::ceil(_value));
    }

    template <typename type>
    WN_FORCE_INLINE typename enable_if<is_real<type>::value, type>::type floor(const type& _value) {
        return(internal::math::basic_traits::floor(_value));
    }

    template <typename type>
    WN_FORCE_INLINE typename enable_if<is_real<type>::value, type>::type round(const type& _value) {
        return(internal::math::basic_traits::round(_value));
    }

    template <typename type>
    WN_FORCE_INLINE typename enable_if<is_real<type>::value, type>::type trunc(const type& _value) {
        return(internal::math::basic_traits::trunc(_value));
    }

    template <typename type>
    WN_FORCE_INLINE typename enable_if<(is_real<type>::value || std::is_integral<type>::value), type>::type
    mod(const type& _dividend, const type& _divisor) {
        return(internal::math::basic_traits::mod(_dividend, _divisor));
    }

    template <typename type>
    WN_FORCE_INLINE typename enable_if<(is_real<type>::value || std::is_integral<type>::value), type>::type
    sqrt(const type& _value) {
        return(internal::math::basic_traits::sqrt(_value));
    }

    template <typename type>
    WN_FORCE_INLINE typename enable_if<is_real<type>::value, type>::type invsqrt(const type& _value) {
        return(internal::math::basic_traits::invsqrt(_value));
    }

    template <typename type>
    WN_FORCE_INLINE type pow(const type& _base, const type& _exponent) {
        return(internal::math::basic_traits::pow(_base, _exponent));
    }

    template <typename type>
    WN_FORCE_INLINE type min(const type& _value1, const type& _value2) {
        return(internal::math::basic_traits::min(_value1, _value2));
    }

    template <typename type>
    WN_FORCE_INLINE type max(const type& _value1, const type& _value2) {
        return(internal::math::basic_traits::max(_value1, _value2));
    }

    template <typename type>
    WN_FORCE_INLINE type clamp(const type& _value, const type& _min, const type& _max) {
        return(internal::math::basic_traits::clamp(_value, _min, _max));
    }

    template <typename type>
    WN_FORCE_INLINE type deg_to_rad(const type& _value) {
        return(static_cast<type>(static_cast<wn_float64>(_value)* (WN_PI / 180.0)));
    }

    template <typename type>
    WN_FORCE_INLINE type rad_to_deg(const type& _value) {
        return(static_cast<type>(static_cast<wn_float64>(_value)* (180.0 / WN_PI)));
    }
}

#endif // __WN_MATH_BASIC_H__