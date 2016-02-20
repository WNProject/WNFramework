// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_SSE_GENERAL_TRAITS_H__
#define __WN_MATH_INTERNAL_X86_SSE_GENERAL_TRAITS_H__

#include "WNMath/inc/Internal/Generic/WNGeneralTraits.h"

#include <xmmintrin.h>

namespace wn {
namespace internal {
namespace math {
struct general_traits_sse : general_traits_generic {
  template <typename type, const size_t dimension>
  static WN_FORCE_INLINE void initialize(
      element_array<type, dimension>& _element_array) {
    base::initialize(_element_array);
  }

  template <const size_t dimension>
  static WN_FORCE_INLINE typename core::enable_if<dimension <= 4>::type
  initialize(element_array<float, dimension>& _element_array) {
    _element_array.m_xmm_values[0] = _mm_setzero_ps();
  }

  template <const size_t dimension>
  static WN_FORCE_INLINE typename core::enable_if<(dimension > 4)>::type
  initialize(element_array<float, dimension>& _element_array) {
    for (size_t i = 0; i < xmm_count_ps<dimension>::value; ++i) {
      _element_array.m_xmm_values[i] = _mm_setzero_ps();
    }
  }

  template <typename type, const size_t dimension>
  static WN_FORCE_INLINE void assign(
      element_array<type, dimension>& _element_array1,
      element_array<type, dimension>&& _element_array2) {
    base::assign(_element_array1, std::move(_element_array2));
  }

  template <typename type, const size_t dimension>
  static WN_FORCE_INLINE void assign(
      element_array<type, dimension>& _element_array1,
      const element_array<type, dimension>& _element_array2) {
    base::assign(_element_array1, _element_array2);
  }

  template <typename type, const size_t dimension>
  static WN_FORCE_INLINE void assign(
      element_array<type, dimension>& _element_array, const type& _value) {
    base::assign(_element_array, _value);
  }

  template <typename type>
  static WN_FORCE_INLINE
      typename core::enable_if<core::is_same<type, void>::value>::type
      assign(element_array<float, 2>& _element_array, const float& _value) {
    _element_array.m_xmm_values[0] = _mm_setr_ps(_value, _value, 0.0f, 0.0f);
  }

  template <typename type>
  static WN_FORCE_INLINE
      typename core::enable_if<core::is_same<type, void>::value>::type
      assign(element_array<float, 3>& _element_array, const float& _value) {
    _element_array.m_xmm_values[0] = _mm_setr_ps(_value, _value, _value, 0.0f);
  }

  template <typename type>
  static WN_FORCE_INLINE
      typename core::enable_if<core::is_same<type, void>::value>::type
      assign(element_array<float, 4>& _element_array, const float& _value) {
    _element_array.m_xmm_values[0] = _mm_set1_ps(_value);
  }

  template <const size_t dimension>
  static WN_FORCE_INLINE
      typename core::enable_if<(dimension > 4 && dimension % 4 == 0)>::type
      assign(element_array<float, dimension>& _element_array,
          const float& _value) {
    const __m128 value = _mm_set1_ps(_value);

    for (size_t i = 0; i < xmm_count_ps<dimension>::value; ++i) {
      _element_array.m_xmm_values[i] = value;
    }
  }

  template <const size_t dimension>
  static WN_FORCE_INLINE
      typename core::enable_if<(dimension > 4 && dimension % 4 == 1)>::type
      assign(element_array<float, dimension>& _element_array,
          const float& _value) {
    const __m128 value = _mm_set1_ps(_value);
    const size_t xmm_count = xmm_count_ps<dimension>::value - 1;

    for (size_t i = 0; i < xmm_count; ++i) {
      _element_array.m_xmm_values[i] = value;
    }

    _element_array.m_xmm_values[xmm_count] = _mm_set_ss(_value);
  }

  template <const size_t dimension>
  static WN_FORCE_INLINE
      typename core::enable_if<(dimension > 4 && dimension % 4 == 2)>::type
      assign(element_array<float, dimension>& _element_array,
          const float& _value) {
    const __m128 value = _mm_set1_ps(_value);
    const size_t xmm_count = xmm_count_ps<dimension>::value - 1;

    for (size_t i = 0; i < xmm_count; ++i) {
      _element_array.m_xmm_values[i] = value;
    }

    _element_array.m_xmm_values[xmm_count] =
        _mm_setr_ps(_value, _value, 0.0f, 0.0f);
  }

  template <const size_t dimension>
  static WN_FORCE_INLINE
      typename core::enable_if<(dimension > 4 && dimension % 4 == 3)>::type
      assign(element_array<float, dimension>& _element_array,
          const float& _value) {
    const __m128 value = _mm_set1_ps(_value);
    const size_t xmm_count = xmm_count_ps<dimension>::value - 1;

    for (size_t i = 0; i < xmm_count; ++i) {
      _element_array.m_xmm_values[i] = value;
    }

    _element_array.m_xmm_values[xmm_count] =
        _mm_setr_ps(_value, _value, _value, 0.0f);
  }

  template <typename type, const size_t dimension, typename iterator>
  static WN_FORCE_INLINE void assign(
      element_array<type, dimension>& _element_array, iterator _first,
      iterator _last) {
    base::assign(_element_array, _first, _last);
  }

  template <typename type, const size_t dimension>
  static WN_FORCE_INLINE void assign(
      element_array<type, dimension>& _element_array, const type* _numbers) {
    base::assign(_element_array, _numbers);
  }

  template <typename type, const size_t dimension>
  static WN_FORCE_INLINE void add(
      element_array<type, dimension>& _element_array1,
      const element_array<type, dimension>& _element_array2) {
    base::add(_element_array1, _element_array2);
  }

  template <const size_t dimension>
  static WN_FORCE_INLINE
      typename core::enable_if<(dimension > 1 && dimension <= 4)>::type
      add(element_array<float, dimension>& _element_array1,
          const element_array<float, dimension>& _element_array2) {
    _element_array1.m_xmm_values[0] = _mm_add_ps(
        _element_array1.m_xmm_values[0], _element_array2.m_xmm_values[0]);
  }

  template <const size_t dimension>
  static WN_FORCE_INLINE
      typename core::enable_if<(dimension > 4 && dimension % 4 == 1)>::type
      add(element_array<float, dimension>& _element_array1,
          const element_array<float, dimension>& _element_array2) {
    const size_t xmm_count = xmm_count_ps<dimension>::value - 1;

    for (size_t i = 0; i < xmm_count; ++i) {
      _element_array1.m_xmm_values[i] = _mm_add_ps(
          _element_array1.m_xmm_values[i], _element_array2.m_xmm_values[i]);
    }

    _element_array1.m_xmm_values[xmm_count] =
        _mm_add_ss(_element_array1.m_xmm_values[xmm_count],
            _element_array2.m_xmm_values[xmm_count]);
  }

  template <const size_t dimension>
  static WN_FORCE_INLINE
      typename core::enable_if<(dimension > 4 && dimension % 4 != 1)>::type
      add(element_array<float, dimension>& _element_array1,
          const element_array<float, dimension>& _element_array2) {
    for (size_t i = 0; i < xmm_count_ps<dimension>::value; ++i) {
      _element_array1.m_xmm_values[i] = _mm_add_ps(
          _element_array1.m_xmm_values[i], _element_array2.m_xmm_values[i]);
    }
  }

  template <typename type, const size_t dimension>
  static WN_FORCE_INLINE void add(
      element_array<type, dimension>& _element_array, const type& _value) {
    base::add(_element_array, _value);
  }

  template <typename type>
  static WN_FORCE_INLINE
      typename std::enable_if<core::is_same<type, void>::value>::type
      add(element_array<float, 2>& _element_array, const float& _value) {
    _element_array.m_xmm_values[0] = _mm_add_ps(_element_array.m_xmm_values[0],
        _mm_setr_ps(_value, _value, 0.0f, 0.0f));
  }

  template <typename type>
  static WN_FORCE_INLINE
      typename std::enable_if<core::is_same<type, void>::value>::type
      add(element_array<float, 3>& _element_array, const float& _value) {
    _element_array.m_xmm_values[0] = _mm_add_ps(_element_array.m_xmm_values[0],
        _mm_setr_ps(_value, _value, _value, 0.0f));
  }

  template <typename type>
  static WN_FORCE_INLINE
      typename std::enable_if<core::is_same<type, void>::value>::type
      add(element_array<float, 4>& _element_array, const float& _value) {
    _element_array.m_xmm_values[0] =
        _mm_add_ps(_element_array.m_xmm_values[0], _mm_set1_ps(_value));
  }

  template <const size_t dimension>
  static WN_FORCE_INLINE
      typename core::enable_if<(dimension > 4 && dimension % 4 == 1)>::type
      add(element_array<float, dimension>& _element_array,
          const float& _value) {
    const __m128 value = _mm_set1_ps(_value);
    const size_t xmm_count = xmm_count_ps<dimension>::value - 1;

    for (size_t i = 0; i < xmm_count; ++i) {
      _element_array.m_xmm_values[i] =
          _mm_add_ps(_element_array.m_xmm_values[i], value);
    }

    _element_array.m_xmm_values[xmm_count] =
        _mm_add_ss(_element_array.m_xmm_values[xmm_count], value);
  }

  template <const size_t dimension>
  static WN_FORCE_INLINE
      typename core::enable_if<(dimension > 4 && dimension % 4 != 1)>::type
      add(element_array<float, dimension>& _element_array,
          const float& _value) {
    const __m128 value = _mm_set1_ps(_value);

    for (size_t i = 0; i < xmm_count_ps<dimension>::value; ++i) {
      _element_array.m_xmm_values[i] =
          _mm_add_ps(_element_array.m_xmm_values[i], value);
    }
  }

private:
  typedef general_traits_generic base;
};
}
}
}

#endif  // __WN_MATH_INTERNAL_X86_SSE_GENERAL_TRAITS_H__
