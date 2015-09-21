// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

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
            struct general_traits_generic : core::non_constructable_non_copyable {
                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void initialize(element_array<type, dimension>& _values) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _values.m_values[i] = static_cast<type>(0);
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void assign(element_array<type, dimension>& _first,
                                                   element_array<type, dimension>&& _second) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _first.m_values[i] = std::move(_second.m_values[i]);
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void assign(element_array<type, dimension>& _first,
                                                   const element_array<type, dimension>& _second) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _first.m_values[i] = _second.m_values[i];
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void assign(element_array<type, dimension>& _values, const type& _value) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _values.m_values[i] = _value;
                    }
                }

                template <typename type, const wn_size_t dimension, typename iterator>
                static WN_FORCE_INLINE wn_void assign(element_array<type, dimension>& _values,
                                                      iterator _first, iterator _last) {
                    wn_size_t count = 0;

                    for (; _first != _last && count < dimension; ++_first, ++count) {
                        _values.m_values[count] = static_cast<type>(*_first);
                    }

                    for (; count < dimension; ++count) {
                        _values.m_values[count] = static_cast<type>(0);
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void assign(element_array<type, dimension>& _values, const type* _numbers) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _values.m_values[i] = _numbers[i];
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void negate(element_array<type, dimension>& _values) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _values.m_values[i] = -_values.m_values[i];
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void add(element_array<type, dimension>& _first,
                                                   const element_array<type, dimension>& _second) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _first.m_values[i] += _second.m_values[i];
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void add(element_array<type, dimension>& _values, const type& _value) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _values.m_values[i] += _value;
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_bool equal(const element_array<type, dimension>& _first,
                                                     const element_array<type, dimension>& _second) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        if (_first.m_values[i] != _second.m_values[i]) {
                          return(wn_false);
                        }
                    }

                    return(wn_true);
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_bool not_equal(const element_array<type, dimension>& _first,
                                                     const element_array<type, dimension>& _second) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        if (_first.m_values[i] == _second.m_values[i]) {
                            return(wn_false);
                        }
                    }

                    return(wn_true);
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE type dot(const element_array<type, dimension>& _first,
                                                     const element_array<type, dimension>& _second) {
                    type accumulated_dot = static_cast<type>(0);

                    for (wn_size_t i = 0; i < dimension; ++i) {
                        accumulated_dot += _first.m_values[i] * _second.m_values[i];
                    }

                    return(accumulated_dot);
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_bool zero(const element_array<type, dimension>& _first) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        if (_first.m_values[i] != static_cast<type>(0)) {
                            return(wn_false);
                        }
                    }

                    return(wn_true);
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void set_zero(element_array<type, dimension>& _values) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _values.m_values[i] = static_cast<type>(0);
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE void snap(element_array<type, dimension>& _values,
                                                 wn::snap_direction _direction) {
                    for(wn_size_t i = 0; i < dimension; ++i) {
                        switch (_direction) {
                            case snap_direction::nearest:
                              _values.m_values[i] = wn::round(_values.m_values[i]);

                              break;
                            case snap_direction::down:
                              _values.m_values[i] = wn::floor(_values.m_values[i]);

                              break;
                            case snap_direction::up:
                              _values.m_values[i] = wn::ceil(_values.m_values[i]);

                              break;
                            case snap_direction::truncate:
                              _values.m_values[i] = wn::trunc(_values.m_values[i]);

                              break;
                        }
                    }
                }


                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void subtract(element_array<type, dimension>& _first,
                                                        const element_array<type, dimension>& _second) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _first.m_values[i] -= _second.m_values[i];
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void subtract(element_array<type, dimension>& _values, const type& _value) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _values.m_values[i] -= _value;
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void multiply(element_array<type, dimension>& _first,
                                                        const element_array<type, dimension>& _second) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _first.m_values[i] *= _second.m_values[i];
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void multiply(element_array<type, dimension>& _values, const type& _value) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _values.m_values[i] *= _value;
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void divide(element_array<type, dimension>& _first,
                                                      const element_array<type, dimension>& _second) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _first.m_values[i] /= _second.m_values[i];
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE typename core::enable_if<core::is_real<type>::value>::type
                divide(element_array<type, dimension>& _values, const type& _value) {
                    #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                        const type inverse_value = static_cast<type>(1) / _value;
                    #endif

                    for (wn_size_t i = 0; i < dimension; ++i) {
                        #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                            _values.m_values[i] *= inverse_value;
                        #else
                            _values.m_values[i] /= _value;
                        #endif
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE typename core::enable_if<!core::is_real<type>::value>::type
                divide(element_array<type, dimension>& _values, const type& _value) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _values.m_values[i] /= _value;
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void multiply_add(element_array<type, dimension>& _values,
                                                            const type& _value1, const type& _value2) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _values.m_values[i] *= _value1;
                        _values.m_values[i] += _value2;
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void multiply_add(element_array<type, dimension>& _first,
                                                            const element_array<type, dimension>& _second,
                                                            const element_array<type, dimension>& _element_array3) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _first.m_values[i] *= _second.m_values[i];
                        _first.m_values[i] += _element_array3.m_values[i];
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void multiply_subtract(element_array<type, dimension>& _first,
                                                                 const element_array<type, dimension>& _second,
                                                                 const element_array<type, dimension>& _element_array3) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _first.m_values[i] *= _second.m_values[i];
                        _first.m_values[i] -= _element_array3.m_values[i];
                    }
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void multiply_subtract(element_array<type, dimension>& _values,
                                                                 const type& _value1, const type& _value2) {
                    for (wn_size_t i = 0; i < dimension; ++i) {
                        _values.m_values[i] *= _value1;
                        _values.m_values[i] -= _value2;
                    }
                }
            };
        }
    }
}

#endif // __WN_MATH_INTERNAL_GENERIC_GENERAL_TRAITS_H__
