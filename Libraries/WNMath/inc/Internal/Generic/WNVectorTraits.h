// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_GENERIC_VECTOR_TRAITS_H__
#define __WN_MATH_INTERNAL_GENERIC_VECTOR_TRAITS_H__

#include "WNMath/inc/WNBasic.h"
#include "WNMath/inc/WNCommon.h"
#include "WNMath/inc/Internal/WNGeneralTraits.h"

namespace wn {
    namespace internal {
        namespace math {
            struct vector_traits_generic : general_traits {
                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE type length_squared(const element_array<type, dimension>& _element_array) {
                    type result = static_cast<type>(0);

                    for (wn_size_t i = 0; i < dimension; ++i) {
                        result += _element_array.m_values[i] * _element_array.m_values[i];
                    }

                    return(result);
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_bool equal(const element_array<type, dimension>& first,
                                                     const element_array<type, dimension>& second) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        if(first.m_values[i] != second.m_values[i]) {
                          return wn_false;
                        }
                    }
                    return wn_true;
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_bool not_equal(const element_array<type, dimension>& first,
                                                     const element_array<type, dimension>& second) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        if(first.m_values[i] == second.m_values[i]) {
                          return wn_false;
                        }
                    }
                    return wn_true;
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE type dot(const element_array<type, dimension>& first,
                                                     const element_array<type, dimension>& second) {
                    type accumulator = static_cast<type>(0);
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        accumulator += first.m_values[i] * second.m_values[i];
                    }
                    return accumulator;
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_bool zero(const element_array<type, dimension>& first) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        if(first.m_values[i] != static_cast<type>(0)) {
                          return wn_false;
                        }
                    }
                    return wn_true;
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE void set_zero(const element_array<type, dimension>& first) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        first.m_values[i] = static_cast<type>(0);
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE void snap(element_array<type, dimension>& first,
                                                 wn::snap_direction direction) {
                    for(wn_size_t i = 0; i < dimension; ++i) {
                        switch(direction) {
                            case snap_direction::nearest:
                              first.m_values[i] = wn::round(first.m_values[i]);
                              break;
                            case snap_direction::down:
                              first.m_values[i] = wn::floor(first.m_values[i]);
                              break;
                            case snap_direction::up:
                              first.m_values[i] = wn::ceil(first.m_values[i]);
                              break;
                            case snap_direction::truncate:
                              first.m_values[i] = wn::trunc(first.m_values[i]);
                              break;
                        }
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE typename enable_if<(is_real<type>::value ||
                                                           std::is_integral<type>::value), type>::type
                length(const element_array<type, dimension>& _element_array) {
                    return(sqrt(length_squared(_element_array)));
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE typename enable_if<is_real<type>::value>::type
                normalize(element_array<type, dimension>& _element_array) {
                    const type result = length(_element_array);

                    #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                        const type inverse_result = static_cast<type>(1) / result;
                    #endif

                    for (wn_size_t i = 0; i < dimension; ++i) {
                        #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                            _element_array.m_values[i] *= inverse_result;
                        #else
                            _element_array.m_values[i] /= result;
                        #endif
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE typename enable_if<is_real<type>::value>::type
                truncate(element_array<type, dimension>& _element_array, const type _length) {
                    const type zero = static_cast<type>(0);

                    if (_length > zero) {
                        const type result = length_squared<type, dimension>();

                        if (result > (_length * _length)) {
                            const type adjusted_result = _length / invsqrt(result);

                            divide(_element_array, adjusted_result);
                        }
                    } else {
                        assign(_element_array, zero);
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE typename enable_if<is_real<type>::value>::type
                recipricol(element_array<type, dimension>& _element_array) {
                    const type one = static_cast<type>(1);

                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _element_array.m_values[i] = one / _element_array.m_values[i];
                    }
                }
            };
        }
    }
}

#endif // __WN_MATH_INTERNAL_GENERIC_VECTOR_TRAITS_H__
