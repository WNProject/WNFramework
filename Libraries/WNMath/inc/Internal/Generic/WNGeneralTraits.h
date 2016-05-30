// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_GENERIC_GENERAL_TRAITS_H__
#define __WN_MATH_INTERNAL_GENERIC_GENERAL_TRAITS_H__

#include "WNCore/inc/WNTypeTraits.h"
#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNUtility.h"
#include "WNMath/inc/Internal/WNElementArray.h"
#include "WNMath/inc/WNBasic.h"
#include "WNMath/inc/WNCommon.h"
#include "WNMath/inc/WNConfig.h"

#include <cmath>

namespace wn {
namespace math {
namespace internal {

template <typename T, const size_t Count, const bool Precise = false>
class general_traits_generic : core::non_constructable_non_copyable {
public:
  static WN_FORCE_INLINE void assign(element_array<T, Count>& _destination,
      element_array<T, Count>&& _source) {
    for (size_t i = 0; i < Count; ++i) {
      _destination[i] = core::move(_source[i]);
    }
  }

  static WN_FORCE_INLINE void assign(element_array<T, Count>& _destination,
      const element_array<T, Count>& _source) {
    for (size_t i = 0; i < Count; ++i) {
      _destination[i] = _source[i];
    }
  }

  template <const size_t OtherCount>
  static WN_FORCE_INLINE void assign(element_array<T, Count>& _destination,
      element_array<T, OtherCount>&& _source) {
    const size_t copy_size = (Count < OtherCount) ? Count : OtherCount;
    size_t i = 0;

    for (; i < copy_size; ++i) {
      _destination[i] = core::move(_source[i]);
    }

    for (; i < Count; ++i) {
      _destination[i] = T(0);
    }
  }

  template <const size_t OtherCount>
  static WN_FORCE_INLINE void assign(element_array<T, Count>& _destination,
      const element_array<T, OtherCount>& _source) {
    const size_t copy_size = (Count < OtherCount) ? Count : OtherCount;
    size_t i = 0;

    for (; i < copy_size; ++i) {
      _destination[i] = _source[i];
    }

    for (; i < Count; ++i) {
      _destination[i] = T(0);
    }
  }

  static WN_FORCE_INLINE void assign(
      element_array<T, Count>& _destination, const T& _source) {
    for (size_t i = 0; i < Count; ++i) {
      _destination[i] = _source;
    }
  }

  static WN_FORCE_INLINE void assign(element_array<T, Count>& _elements,
      const T* _numbers, const size_t _count) {
    const size_t count = (Count < _count) ? Count : _count;
    size_t i = 0;

    for (; i < count; ++i) {
      _elements[i] = _numbers[i];
    }

    for (; i < Count; ++i) {
      _elements[i] = T(0);
    }
  }

  template <typename U = T>
  static WN_FORCE_INLINE typename core::enable_if<
      core::conjunction<core::is_same<T, U>, core::is_signed<U>>::value>::type
  negate(element_array<T, Count>& _elements) {
    for (size_t i = 0; i < Count; ++i) {
      _elements[i] = (-_elements[i]);
    }
  }

  static WN_FORCE_INLINE void add(
      element_array<T, Count>& _first, const element_array<T, Count>& _second) {
    for (size_t i = 0; i < Count; ++i) {
      _first[i] += _second[i];
    }
  }

  static WN_FORCE_INLINE void add(
      element_array<T, Count>& _elements, const T& _value) {
    for (size_t i = 0; i < Count; ++i) {
      _elements[i] += _value;
    }
  }

  static WN_FORCE_INLINE bool equal(const element_array<T, Count>& _first,
      const element_array<T, Count>& _second) {
    for (size_t i = 0; i < Count; ++i) {
      if (_first[i] != _second[i]) {
        return false;
      }
    }

    return true;
  }

  static WN_FORCE_INLINE bool not_equal(const element_array<T, Count>& _first,
      const element_array<T, Count>& _second) {
    return !equal(_first, _second);
  }

  static WN_FORCE_INLINE bool zero(const element_array<T, Count>& _first) {
    for (size_t i = 0; i < Count; ++i) {
      if (_first[i] != T(0)) {
        return false;
      }
    }

    return true;
  }

  static WN_FORCE_INLINE void make_zero(element_array<T, Count>& _values) {
    for (size_t i = 0; i < Count; ++i) {
      _values[i] = T(0);
    }
  }

  static WN_FORCE_INLINE void subtract(
      element_array<T, Count>& _first, const element_array<T, Count>& _second) {
    for (size_t i = 0; i < Count; ++i) {
      _first[i] -= _second[i];
    }
  }

  static WN_FORCE_INLINE void subtract(
      element_array<T, Count>& _elements, const T& _value) {
    for (size_t i = 0; i < Count; ++i) {
      _elements[i] -= _value;
    }
  }

  static WN_FORCE_INLINE void multiply(
      element_array<T, Count>& _first, const element_array<T, Count>& _second) {
    for (size_t i = 0; i < Count; ++i) {
      _first[i] *= _second[i];
    }
  }

  static WN_FORCE_INLINE void multiply(
      element_array<T, Count>& _values, const T& _value) {
    for (size_t i = 0; i < Count; ++i) {
      _values[i] *= _value;
    }
  }

  static WN_FORCE_INLINE void divide(
      element_array<T, Count>& _first, const element_array<T, Count>& _second) {
    for (size_t i = 0; i < Count; ++i) {
      _first[i] /= _second[i];
    }
  }

  template <typename U = T>
  static WN_FORCE_INLINE typename core::enable_if<
      core::conjunction<core::is_same<T, U>, core::is_real<U>,
          core::negation<core::bool_constant<Precise>>>::value>::type
  divide(element_array<T, Count>& _values, const T& _value) {
    const U inverse_value(U(1) / _value);

    multiply(_values, inverse_value);
  }

  template <typename U = T>
  static WN_FORCE_INLINE
      typename core::enable_if<core::conjunction<core::is_same<T, U>,
          core::disjunction<core::negation<core::is_real<U>>,
              core::conjunction<core::is_real<U>,
                  core::bool_constant<Precise>>>>::value>::type
      divide(element_array<T, Count>& _values, const T& _value) {
    for (size_t i = 0; i < Count; ++i) {
      _values[i] /= _value;
    }
  }

  static WN_FORCE_INLINE void multiply_add(
      element_array<T, Count>& _values, const T& _value1, const T& _value2) {
    for (size_t i = 0; i < Count; ++i) {
      _values[i] *= _value1;
      _values[i] += _value2;
    }
  }

  static WN_FORCE_INLINE void multiply_add(element_array<T, Count>& _first,
      const element_array<T, Count>& _second,
      const element_array<T, Count>& _third) {
    for (size_t i = 0; i < Count; ++i) {
      _first[i] *= _second[i];
      _first[i] += _third[i];
    }
  }

  static WN_FORCE_INLINE void multiply_subtract(element_array<T, Count>& _first,
      const element_array<T, Count>& _second,
      const element_array<T, Count>& _third) {
    for (size_t i = 0; i < Count; ++i) {
      _first[i] *= _second[i];
      _first[i] -= _third[i];
    }
  }

  static WN_FORCE_INLINE void multiply_subtract(
      element_array<T, Count>& _values, const T& _value1, const T& _value2) {
    for (size_t i = 0; i < Count; ++i) {
      _values[i] *= _value1;
      _values[i] -= _value2;
    }
  }

  static WN_FORCE_INLINE void clamp(element_array<T, Count>& _values,
      const element_array<T, Count>& _minimum,
      const element_array<T, Count>& _maximum) {
    for (size_t i = 0; i < Count; ++i) {
      _values[i] = math::clamp(_values[i], _minimum[i], _maximum[i]);
    }
  }

  static WN_FORCE_INLINE void clamp(
      element_array<T, Count>& _values, const T& _minimum, const T& _maximum) {
    for (size_t i = 0; i < Count; ++i) {
      _values[i] = math::clamp(_values[i], _minimum, _maximum);
    }
  }

  static WN_FORCE_INLINE void saturate(element_array<T, Count>& _first) {
    clamp(_first, T(0), T(1));
  }

  static WN_FORCE_INLINE void reverse(element_array<T, Count>& _first) {
    for (size_t i = 0; i < (Count / 2); ++i) {
      core::swap(_first[i], _first[(Count - 1) - i]);
    }
  }

  static WN_FORCE_INLINE void snap(
      element_array<T, Count>& _values, snap_direction _direction) {
    switch (_direction) {
      case snap_direction::nearest:
        for (size_t i = 0; i < Count; ++i) {
          _values[i] = math::round(_values[i]);
        }

        break;
      case snap_direction::down:
        for (size_t i = 0; i < Count; ++i) {
          _values[i] = math::floor(_values[i]);
        }

        break;
      case snap_direction::up:
        for (size_t i = 0; i < Count; ++i) {
          _values[i] = math::ceil(_values[i]);
        }

        break;
      case snap_direction::truncate:
        for (size_t i = 0; i < Count; ++i) {
          _values[i] = math::trunc(_values[i]);
        }

        break;
    }
  }

  static WN_FORCE_INLINE void swap(
      element_array<T, Count>& _first, element_array<T, Count>& _second) {
    for (size_t i = 0; i < Count; ++i) {
      core::swap(_first[i], _second[i]);
    }
  }
};

}  // namespace internal
}  // namespace math
}  // namespace wn

#endif  // __WN_MATH_INTERNAL_GENERIC_GENERAL_TRAITS_H__
