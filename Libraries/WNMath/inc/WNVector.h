// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_VECTOR_H__
#define __WN_MATH_VECTOR_H__

#include "WNCore/inc/WNAssert.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNMath/inc/Internal/WNElementArray.h"
#include "WNMath/inc/Internal/WNVectorTraits.h"
#include "WNMath/inc/WNCommon.h"

#include <array>
#include <initializer_list>

namespace wn {
    template <typename _Type, const wn_size_t _dimension>
    class vector final {
    public:
        static_assert(_dimension > 1, "dimension must be at greater than 1");

        typedef _Type value_type;
        typedef internal::math::vector_traits traits_type;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef value_type* iterator;
        typedef value_type const* const_iterator;
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

        WN_FORCE_INLINE vector() {
            traits_type::initialize(m_element_array);
        }

        WN_FORCE_INLINE vector(vector&& _vector) {
            assign(std::move(_vector));
        }

        WN_FORCE_INLINE vector(const vector& _vector) {
            assign(_vector);
        }

        WN_FORCE_INLINE explicit vector(const value_type& _value) {
            assign(_value);
        }

        template <typename type,
                  typename... types,
                  typename = core::enable_if_t<core::bool_and<core::are_same<type, types..., value_type>::value,
                                                                (sizeof...(types) == (_dimension - 1)), (_dimension > 1)>::value>>
        WN_FORCE_INLINE explicit vector(const type& _value, const types&... _values) :
            vector({ _value, _values... }) {
        }

        WN_FORCE_INLINE explicit vector(const std::array<value_type, _dimension>& _array) {
            assign(_array);
        }

        WN_FORCE_INLINE explicit vector(const std::initializer_list<value_type>& _initializer_list) {
            assign(_initializer_list);
        }

        WN_FORCE_INLINE explicit vector(const value_type* _values) {
            assign(_values);
        }

        WN_FORCE_INLINE operator value_type* () {
            return(m_element_array.m_values);
        }

        WN_FORCE_INLINE operator const value_type* () const {
            return(m_element_array.m_values);
        }

        WN_FORCE_INLINE value_type& operator [] (const wn_size_t _index) {
            return(at(_index));
        }

        WN_FORCE_INLINE const value_type& operator [] (const wn_size_t _index) const {
            return(at(_index));
        }

        WN_FORCE_INLINE vector& operator = (vector&& _vector) {
            assign(std::move(_vector));

            return(*this);
        }

        WN_FORCE_INLINE vector& operator = (const vector& _vector) {
            assign(_vector);

            return(*this);
        }

        WN_FORCE_INLINE vector& operator = (const value_type& _value) {
            assign(_value);

            return(*this);
        }

        WN_FORCE_INLINE vector& operator = (const std::array<value_type, _dimension>& _array) {
            assign(_array);

            return(*this);
        }

        WN_FORCE_INLINE vector& operator = (const std::initializer_list<value_type>& _initializer_list) {
            assign(_initializer_list);

            return(*this);
        }

        WN_FORCE_INLINE vector operator + () const {
            return(*this);
        }

        template <typename type = value_type,
                  typename = core::enable_if_t<core::is_signed<type>::value>>
        WN_FORCE_INLINE vector operator - () const {
            vector vector(*this);

            traits_type::negate(vector.m_element_array);

            return(vector);
        }

        WN_FORCE_INLINE vector& operator += (const vector& _vector) {
            traits_type::add(m_element_array, _vector.m_element_array);

            return(*this);
        }

        WN_FORCE_INLINE vector& operator -= (const vector& _vector) {
            traits_type::subtract(m_element_array, _vector.m_element_array);

            return(*this);
        }

        WN_FORCE_INLINE vector& operator *= (const vector& _vector) {
            traits_type::multiply(m_element_array, _vector.m_element_array);

            return(*this);
        }

        WN_FORCE_INLINE vector& operator /= (const vector& _vector) {
            traits_type::divide(m_element_array, _vector.m_element_array);

            return(*this);
        }

        WN_FORCE_INLINE vector& operator += (const value_type& _value) {
            traits_type::add(m_element_array, _value);

            return(*this);
        }

        WN_FORCE_INLINE vector& operator -= (const value_type& _value) {
            traits_type::subtract(m_element_array, _value);

            return(*this);
        }

        WN_FORCE_INLINE vector& operator *= (const value_type& _value) {
            traits_type::multiply(m_element_array, _value);

            return(*this);
        }

        WN_FORCE_INLINE vector& operator /= (const value_type& _value) {
            traits_type::divide(m_element_array, _value);

            return(*this);
        }

        WN_FORCE_INLINE vector operator + (const vector& _vector) const {
            vector vector(*this);

            return(vector += _vector, vector);
        }

        WN_FORCE_INLINE vector operator - (const vector& _vector) const {
            vector vector(*this);

            return(vector -= _vector, vector);
        }

        WN_FORCE_INLINE vector operator * (const vector& _vector) const {
            vector vector(*this);

            return(vector *= _vector, vector);
        }

        WN_FORCE_INLINE vector operator / (const vector& _vector) const {
            vector vector(*this);

            return(vector /= _vector, vector);
        }

        WN_FORCE_INLINE vector operator + (const value_type& _value) const {
            vector vector(*this);

            return(vector += _value, vector);
        }

        WN_FORCE_INLINE vector operator - (const value_type& _value) const {
            vector vector(*this);

            return(vector -= _value, vector);
        }

        WN_FORCE_INLINE vector operator * (const value_type& _value) const {
            vector vector(*this);

            return(vector *= _value, vector);
        }

        WN_FORCE_INLINE vector operator / (const value_type& _value) const {
            vector vector(*this);

            return(vector /= _value, vector);
        }

        WN_FORCE_INLINE wn_bool operator == (const vector& _vector) const {
            return(traits_type::template equal(m_element_array, _vector.m_element_array));
        }

        WN_FORCE_INLINE wn_bool operator != (const vector& _vector) const {
            return(traits_type::template not_equal(m_element_array, _vector.m_element_array));
        }

        WN_FORCE_INLINE iterator begin() {
            return(m_element_array.m_values);
        }

        WN_FORCE_INLINE const_iterator cbegin() const {
            return(m_element_array.m_values);
        }

        WN_FORCE_INLINE iterator end() {
            return(m_element_array.m_values + _dimension);
        }

        WN_FORCE_INLINE const_iterator cend() const {
            return(m_element_array.m_values + _dimension);
        }

        WN_FORCE_INLINE reverse_iterator rbegin() {
            return(reverse_iterator(end()));
        }

        WN_FORCE_INLINE const_reverse_iterator crbegin() const {
            return(const_reverse_iterator(cend()));
        }

        WN_FORCE_INLINE reverse_iterator rend() {
            return(reverse_iterator(begin()));
        }

        WN_FORCE_INLINE const_reverse_iterator crend() const {
            return(const_reverse_iterator(cbegin()));
        }

        template <const wn_size_t _index>
        WN_FORCE_INLINE value_type& at() {
            static_assert(_index < _dimension, "Attempting to get element outside of bounds");

            return(at(_index));
        }

        template <const wn_size_t _index>
        WN_FORCE_INLINE const value_type& at() const {
            static_assert(_index < _dimension, "Attempting to get element outside of bounds");

            return(at(_index));
        }

        WN_FORCE_INLINE value_type& at(const wn_size_t _index) {
            WN_DEBUG_ASSERT_DESC(_index < _dimension, "Attempting to get element outside of bounds");

            return(m_element_array.m_values[_index]);
        }

        WN_FORCE_INLINE const value_type& at(const wn_size_t _index) const {
            WN_DEBUG_ASSERT_DESC(_index < _dimension, "Attempting to get element outside of bounds");

            return(m_element_array.m_values[_index]);
        }

        WN_FORCE_INLINE value_type* data() {
            return(m_element_array.m_values);
        }

        WN_FORCE_INLINE const value_type* data() const {
            return(m_element_array.m_values);
        }

        template <const wn_size_t dimension = _dimension,
                  typename = core::enable_if_t<(dimension > 1 && dimension < 5)>>
        WN_FORCE_INLINE value_type& x() {
            return(at<0>());
        }

        template <const wn_size_t dimension = _dimension,
                  typename = core::enable_if_t<(dimension > 1 && dimension < 5)>>
        WN_FORCE_INLINE const value_type& x() const {
            return(at<0>());
        }

        template <const wn_size_t dimension = _dimension,
                  typename = core::enable_if_t<(dimension > 1 && dimension < 5)>>
        WN_FORCE_INLINE value_type& y() {
            return(at<1>());
        }

        template <const wn_size_t dimension = _dimension,
                  typename = core::enable_if_t<(dimension > 1 && dimension < 5)>>
        WN_FORCE_INLINE const value_type& y() const {
            return(at<1>());
        }

        template <const wn_size_t dimension = _dimension,
                  typename = core::enable_if_t<(dimension > 2 && dimension < 5)>>
        WN_FORCE_INLINE value_type& z() {
            return(at<2>());
        }

        template <const wn_size_t dimension = _dimension,
                  typename = core::enable_if_t<(dimension > 2 && dimension < 5)>>
        WN_FORCE_INLINE const value_type& z() const {
            return(at<2>());
        }

        template <const wn_size_t dimension = _dimension,
                  typename = core::enable_if_t<(dimension > 3 && dimension < 5)>>
        WN_FORCE_INLINE value_type& w() {
            return(at<3>());
        }

        template <const wn_size_t dimension = _dimension,
                  typename = core::enable_if_t<(dimension > 3 && dimension < 5)>>
        WN_FORCE_INLINE const value_type& w() const {
            return(at<3>());
        }

        WN_FORCE_INLINE wn_size_t dimension() const {
            return(_dimension);
        }

        WN_FORCE_INLINE value_type length() const {
            return(traits_type::length(m_element_array));
        }

        WN_FORCE_INLINE value_type length_squared() const {
            return(traits_type::length_squared(m_element_array));
        }

        WN_FORCE_INLINE value_type dot(const vector& _vector) const {
            return(traits_type::dot(m_element_array, _vector.m_element_array));
        }

        WN_FORCE_INLINE wn_bool zero() const {
            return(traits_type::zero(m_element_array));
        }

        WN_FORCE_INLINE wn_void multiply_add(const vector& _vector1, const vector& _vector2) {
            traits_type::multiply_add(m_element_array, _vector1.m_element_array, _vector2.m_element_array);
        }

        WN_FORCE_INLINE wn_void multiply_add(const value_type& _number1, const value_type& _number2) {
            traits_type::multiply_add(m_element_array, _number1, _number2);
        }

        WN_FORCE_INLINE wn_void multiply_subtract(const vector& _vector1, const vector& _vector2) {
            traits_type::multiply_subtract(m_element_array, _vector1.m_element_array, _vector2.m_element_array);
        }

        WN_FORCE_INLINE wn_void multiply_subtract(const value_type& _number1, const value_type& _number2) {
            traits_type::multiply_subtract(m_element_array, _number1, _number2);
        }

        WN_FORCE_INLINE wn_void clamp(const vector& _minimum, const vector& _maximum) {
            for (wn_size_t i = 0; i < _dimension; ++i) {
                m_element_array.m_values[i] = clamp(m_element_array.m_values[i],
                                                    _minimum.m_element_array.m_values[i],
                                                    _maximum.m_element_array.m_values[i]);
            }
        }

        WN_FORCE_INLINE wn_void clamp(const value_type& _minimum, const value_type& _maximum) {
            for (wn_size_t i = 0; i < _dimension; ++i) {
                m_element_array.m_values[i] = clamp(m_element_array.m_values[i], _minimum, _maximum);
            }
        }

        WN_FORCE_INLINE wn_void minimum(const vector& _vector) {
            for (wn_size_t i = 0; i < _dimension; ++i) {
                m_element_array.m_values[i] = min(m_element_array.m_values[i], _vector.m_element_array.m_values[i]);
            }
        }

        WN_FORCE_INLINE wn_void maximum(const vector& _vector) {
            for (wn_size_t i = 0; i < _dimension; ++i) {
                m_element_array.m_values[i] = max(m_element_array.m_values[i], _vector.m_element_array.m_values[i]);
            }
        }

        WN_FORCE_INLINE wn_void average(const vector& _vector) {
            for (wn_size_t i = 0; i < _dimension; ++i) {
                m_element_array.m_values[i] += _vector.m_element_array.m_values[i];
                m_element_array.m_values[i] /= static_cast<value_type>(2);
            }
        }

        WN_FORCE_INLINE wn_void saturate() {
            for (wn_size_t i = 0; i < _dimension; ++i) {
                m_element_array.m_values[i] = clamp(m_element_array.m_values[i],
                                                    static_cast<value_type>(0),
                                                    static_cast<value_type>(1));
            }
        }

        WN_FORCE_INLINE wn_void reverse() {
            for (wn_size_t i = 0; i < (_dimension / 2); ++i) {
                std::swap(m_element_array.m_values[i], m_element_array.m_values[(_dimension - 1) - i]);
            }
        }

        //reflect
        //refract

        template <typename type = value_type,
                  typename = core::enable_if_t<core::bool_and<core::is_same<type, value_type>::value, core::is_real<type>::value>::value>>
        WN_FORCE_INLINE wn_void normalize() {
            traits_type::normalize(m_element_array);
        }

        template <typename type = value_type,
                  typename = core::enable_if_t<core::bool_and<core::is_same<type, value_type>::value, core::is_real<type>::value>::value>>
        WN_FORCE_INLINE wn_void truncate(const value_type& _length) {
            traits_type::truncate(m_element_array, _length);
        }

        template <typename type = value_type,
                  typename = core::enable_if_t<core::bool_and<core::is_same<type, value_type>::value, core::is_real<type>::value>::value>>
        WN_FORCE_INLINE wn_void snap(const snap_direction_type _direction = snap_direction::nearest) {
            traits_type::snap(m_element_array, _direction);
        }

        template <typename type = value_type,
                  typename = core::enable_if_t<core::bool_and<core::is_same<type, value_type>::value, core::is_real<type>::value>::value>>
        WN_FORCE_INLINE wn_void recipricol() {
            traits_type::recipricol(m_element_array);
        }

        WN_FORCE_INLINE wn_void translate(const vector& _vector) {
            (*this) += _vector;
        }

        WN_FORCE_INLINE wn_void translate(const value_type& _value) {
            (*this) += _value;
        }

        template <typename... types,
                  typename = core::enable_if_t<core::bool_and<core::are_same<types..., value_type>::value,
                                                     sizeof...(types) == (_dimension - 1)>::value>>
        WN_FORCE_INLINE wn_void translate(const value_type& _value, const types&... _values) {
            (*this) += vector(_value, _values...);
        }

        WN_FORCE_INLINE wn_void scale(const vector& _vector) {
            (*this) *= _vector;
        }

        WN_FORCE_INLINE wn_void scale(const value_type& _value) {
            (*this) *= _value;
        }

        template <typename... types,
                  typename = core::enable_if_t<core::bool_and<core::are_same<types..., value_type>::value,
                                                     sizeof...(types) == (_dimension - 1)>::value>>
        WN_FORCE_INLINE wn_void scale(const value_type& _value, const types&... _values) {
            (*this) *= vector(_value, _values...);
        }

        WN_FORCE_INLINE wn_void homogenize() {
            m_element_array.m_values[_dimension - 1] = static_cast<value_type>(1);
        }

        WN_FORCE_INLINE wn_void assign(vector&& _vector) {
            traits_type::assign(m_element_array, std::move(_vector.m_element_array));
        }

        WN_FORCE_INLINE wn_void assign(const vector& _vector) {
            traits_type::assign(m_element_array, _vector.m_element_array);
        }

        WN_FORCE_INLINE wn_void assign(const value_type& _value) {
            traits_type::assign(m_element_array, _value);
        }

        template <typename type,
                  typename... types,
                  typename = core::enable_if_t<core::bool_and<core::are_same<type, types..., value_type>::value,
                                                     sizeof...(types) == (_dimension - 1), (_dimension > 1)>::value>>
        WN_FORCE_INLINE wn_void assign(const type& _value, const types&... _values) {
            assign({ _value, _values... });
        }

        WN_FORCE_INLINE wn_void assign(const std::array<value_type, _dimension>& _array) {
            traits_type::assign(m_element_array, _array.cbegin(), _array.cend());
        }

        WN_FORCE_INLINE wn_void assign(const std::initializer_list<value_type>& _initializer_list) {
            WN_DEBUG_ASSERT_DESC(_initializer_list.size() <= _dimension, "too many initializers");

            traits_type::assign(m_element_array, _initializer_list.begin(), _initializer_list.end());
        }

        WN_FORCE_INLINE wn_void assign(const value_type* _values) {
            traits_type::assign(m_element_array, _values);
        }

        template <const wn_size_t dimension = _dimension,
                  typename = core::enable_if_t<(dimension > 1 && dimension < 5)>>
        WN_FORCE_INLINE wn_void assign_x(const value_type& _value) {

        }

        template <const wn_size_t dimension = _dimension,
                  typename = core::enable_if_t<(dimension > 1 && dimension < 5)>>
        WN_FORCE_INLINE wn_void assign_y(const value_type& _value) {

        }

        template <const wn_size_t dimension = _dimension,
                  typename = core::enable_if_t<(dimension > 2 && dimension < 5)>>
        WN_FORCE_INLINE wn_void assign_z(const value_type& _value) {

        }

        template <const wn_size_t dimension = _dimension,
                  typename = core::enable_if_t<(dimension > 3 && dimension < 5)>>
        WN_FORCE_INLINE wn_void assign_w(const value_type& _value) {

        }

        WN_FORCE_INLINE wn_void set_zero() {
            traits_type::set_zero(m_element_array);
        }

        WN_FORCE_INLINE vector extract_clamped(const vector& _minimum, const vector& _maximum) const {
            vector vector(*this);

            return(vector.clamp(_minimum, _maximum), vector);
        }

        WN_FORCE_INLINE vector extract_clamped(const value_type& _minimum, const value_type& _maximum) const {
            vector vector(*this);

            return(vector.clamp(_minimum, _maximum), vector);
        }

        WN_FORCE_INLINE vector extract_minimum(const vector& _vector) const {
            vector vector(*this);

            return(vector.minimum(_vector), vector);
        }

        WN_FORCE_INLINE vector extract_maximum(const vector& _vector) const {
            vector vector(*this);

            return(vector.maximum(_vector), vector);
        }

        WN_FORCE_INLINE vector extract_scaled(const value_type& _value) const {
            vector vector(*this);

            return(vector.scale(_value), vector);
        }

        WN_FORCE_INLINE vector extract_scaled(const vector& _vector) const {
            vector vector(*this);

            return(vector.scale(_vector), vector);
        }

        WN_FORCE_INLINE vector extract_average(const vector& _vector) const {
            vector vector(*this);

            return(vector.average(_vector), vector);
        }

        WN_FORCE_INLINE vector extract_saturated() const {
            vector vector(*this);

            return(vector.saturate(), vector);
        }

        WN_FORCE_INLINE vector extract_reversed() const {
            vector vector(*this);

            return(vector.reverse(), vector);
        }

        template <typename type = value_type,
                  typename = core::enable_if_t<core::bool_and<core::is_same<type, value_type>::value, core::is_real<type>::value>::value>>
        WN_FORCE_INLINE vector extract_normalized() const {
            vector vector(*this);

            return(vector.normalize(), vector);
        }

        template <typename type = value_type,
                  typename = core::enable_if_t<core::bool_and<core::is_same<type, value_type>::value, core::is_real<type>::value>::value>>
        WN_FORCE_INLINE vector extract_truncated(const value_type& _length) const {
            vector vector(*this);

            return(vector.truncate(_length), vector);
        }

        template <typename type = value_type,
                  typename = core::enable_if_t<core::bool_and<core::is_same<type, value_type>::value, core::is_real<type>::value>::value>>
        WN_FORCE_INLINE vector extract_snaped(const snap_direction _direction = snap_direction::nearest) const {
            vector vector(*this);

            return(vector.snap(_direction), vector);
        }

        template <typename type = value_type,
                  typename = core::enable_if_t<core::bool_and<core::is_same<type, value_type>::value, core::is_real<type>::value>::value>>
        WN_FORCE_INLINE vector extract_recipricol() const {
            vector vector(*this);

            return(vector.recipricol(), vector);
        }

        WN_FORCE_INLINE vector extract_homogenious() {
            vector vector(*this);

            return(vector.homogenize(), vector);
        }

    private:
        template <typename _other_type, const wn_size_t _other_dimension>
        friend class vector;

        internal::math::element_array<value_type, _dimension> m_element_array;
    };

    template <typename _Type>
    using vector2 = vector<_Type, 2>;

    template <typename _Type>
    using vector3 = vector<_Type, 3>;

    template <typename _Type>
    using vector4 = vector<_Type, 4>;
}

template <typename type, const wn_size_t dimension>
wn::vector<type, dimension> operator + (const type& _value, wn::vector<type, dimension>&& _vector) {
    return(_vector += _value);
}

template <typename type, const wn_size_t dimension>
wn::vector<type, dimension> operator + (const type& _value, const wn::vector<type, dimension>& _vector) {
    return(_vector + _value);
}

template <typename type, const wn_size_t dimension>
wn::vector<type, dimension> operator - (const type& _value, const wn::vector<type, dimension>& _vector) {
    return(wn::vector<type, dimension>(_value) -= _vector);
}

template <typename type, const wn_size_t dimension>
wn::vector<type, dimension> operator * (const type& _value, wn::vector<type, dimension>&& _vector) {
    return(_vector *= _value);
}

template <typename type, const wn_size_t dimension>
wn::vector<type, dimension> operator * (const type& _value, const wn::vector<type, dimension>& _vector) {
    return(_vector * _value);
}

template <typename type, const wn_size_t dimension>
wn::vector<type, dimension> operator / (const type& _value, const wn::vector<type, dimension>& _vector) {
    return(wn::vector<type, dimension>(_value) /= _vector);
}

#endif // __WN_MATH_VECTOR_H__
