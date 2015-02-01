// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_GENERIC_GENERAL_TRAITS_H__
#define __WN_MATH_INTERNAL_GENERIC_GENERAL_TRAITS_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNCore/inc/WNUtility.h"
#include "WNMath/inc/WNConfig.h"
#include "WNMath/inc/Internal/WNElementArray.h"

#include <cmath>

namespace wn {
    namespace internal {
        namespace math {
            struct general_traits_generic : non_constructable_non_copyable {
                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void initialize(element_array<type, dimension>& _element_array) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _element_array.m_values[i] = static_cast<type>(0);
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void assign(element_array<type, dimension>& _element_array1,
                                                   element_array<type, dimension>&& _element_array2) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _element_array1.m_values[i] = std::move(_element_array2.m_values[i]);
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void assign(element_array<type, dimension>& _element_array1,
                                                   const element_array<type, dimension>& _element_array2) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _element_array1.m_values[i] = _element_array2.m_values[i];
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void assign(element_array<type, dimension>& _element_array, const type& _value) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _element_array.m_values[i] = _value;
                    }
                }

                template <typename type, const wn_size_t dimension, typename iterator>
                static WN_FORCE_INLINE wn_void assign(element_array<type, dimension>& _element_array,
                                                      iterator _first, iterator _last) {
                    wn_size_t count = 0;

                    for (; _first != _last && count < dimension; ++_first, ++count) {
                        _element_array.m_values[count] = static_cast<type>(*_first);
                    }

                    for (; count < dimension; ++count) {
                        _element_array.m_values[count] = static_cast<type>(0);
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void assign(element_array<type, dimension>& _element_array, const type* _numbers) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _element_array.m_values[i] = _numbers[i];
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void negate(element_array<type, dimension>& _element_array) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _element_array.m_values[i] = -_element_array.m_values[i];
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void add(element_array<type, dimension>& _element_array1,
                                                   const element_array<type, dimension>& _element_array2) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _element_array1.m_values[i] += _element_array2.m_values[i];
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void add(element_array<type, dimension>& _element_array, const type& _value) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _element_array.m_values[i] += _value;
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void subtract(element_array<type, dimension>& _element_array1,
                                                        const element_array<type, dimension>& _element_array2) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _element_array1.m_values[i] -= _element_array2.m_values[i];
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void subtract(element_array<type, dimension>& _element_array, const type& _value) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _element_array.m_values[i] -= _value;
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void multiply(element_array<type, dimension>& _element_array1,
                                                        const element_array<type, dimension>& _element_array2) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _element_array1.m_values[i] *= _element_array2.m_values[i];
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void multiply(element_array<type, dimension>& _element_array, const type& _value) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _element_array.m_values[i] *= _value;
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void divide(element_array<type, dimension>& _element_array1,
                                                      const element_array<type, dimension>& _element_array2) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _element_array1.m_values[i] /= _element_array2.m_values[i];
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE typename enable_if<is_real<type>::value>::type
                divide(element_array<type, dimension>& _element_array, const type& _value) {
                    #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                        const type inverse_value = static_cast<type>(1) / _value;
                    #endif

                    for (wn_size_t i = 0; i < dimension; ++i) {
                        #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                            _element_array.m_values[i] *= inverse_value;                      
                        #else
                            _element_array.m_values[i] /= _value;
                        #endif
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE typename enable_if<!is_real<type>::value>::type
                divide(element_array<type, dimension>& _element_array, const type& _value) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _element_array.m_values[i] /= _value;
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void multiply_add(element_array<type, dimension>& _element_array,
                                                            const type& _value1, const type& _value2) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _element_array.m_values[i] *= _value1;
                        _element_array.m_values[i] += _value2;
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void multiply_add(element_array<type, dimension>& _element_array1,
                                                            const element_array<type, dimension>& _element_array2,
                                                            const element_array<type, dimension>& _element_array3) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _element_array1.m_values[i] *= _element_array2.m_values[i];
                        _element_array1.m_values[i] += _element_array3.m_values[i];
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void multiply_subtract(element_array<type, dimension>& _element_array1,
                                                                 const element_array<type, dimension>& _element_array2,
                                                                 const element_array<type, dimension>& _element_array3) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _element_array1.m_values[i] *= _element_array2.m_values[i];
                        _element_array1.m_values[i] -= _element_array3.m_values[i];
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void multiply_subtract(element_array<type, dimension>& _element_array,
                                                                 const type& _value1, const type& _value2) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _element_array.m_values[i] *= _value1;
                        _element_array.m_values[i] -= _value2;
                    }
                }
            };
        }
    }
}

#endif // __WN_MATH_INTERNAL_GENERIC_GENERAL_TRAITS_H__