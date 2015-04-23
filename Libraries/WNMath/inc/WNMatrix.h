// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_MATRIX_X_H__
#define __WN_MATH_MATRIX_X_H__

#include "WNCore/inc/WNAssert.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNMath/inc/Internal/WNElementArray.h"
#include "WNMath/inc/Internal/WNVectorTraits.h"
#include "WNMath/inc/WNCommon.h"

#include <array>
#include <initializer_list>

namespace wn {
    namespace internal {
        namespace math {
            struct matrix_traits : general_traits {

            };
        }
    }

    template <typename _type, const wn_size_t _order>
    class matrix final {
    private:
        enum {
            _dimension = (_order * _order)
        };

    public:
        static_assert(_order > 0, "_order must be at greater than 0");

        typedef _type value_type;
        typedef internal::math::matrix_traits traits_type;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef value_type* iterator;
        typedef value_type const* const_iterator;
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

        WN_FORCE_INLINE matrix() {
            traits_type::initialize(m_element_array);
        }

        WN_FORCE_INLINE matrix(matrix&& _vector) {
            assign(std::move(_vector));
        }

        WN_FORCE_INLINE matrix(const matrix& _vector) {
            assign(_vector);
        }

        WN_FORCE_INLINE explicit matrix(const value_type& _value) {
            assign(_value);
        }

        template <typename value_type,
                  typename... types,
                  typename = core::enable_if_t<core::boolean_and<core::are_same<value_type, types..., value_type>::value,
                                                                 (sizeof...(types) == (_dimension - 1)), (_order > 1)>::value>>
        WN_FORCE_INLINE explicit matrix(const value_type& _value, const types&... _values) :
            matrix({ _value, _values... }) {
        }

        WN_FORCE_INLINE explicit matrix(const std::array<value_type, _dimension>& _array) {
            assign(_array);
        }

        WN_FORCE_INLINE explicit matrix(const std::initializer_list<value_type>& _initializer_list) {
            assign(_initializer_list);
        }

        WN_FORCE_INLINE explicit matrix(const value_type* _values) {
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

        WN_FORCE_INLINE matrix& operator = (matrix&& _vector) {
            assign(std::move(_vector));

            return(*this);
        }

        WN_FORCE_INLINE matrix& operator = (const matrix& _vector) {
            assign(_vector);

            return(*this);
        }

        WN_FORCE_INLINE matrix& operator = (const value_type& _value) {
            assign(_value);

            return(*this);
        }

        WN_FORCE_INLINE matrix& operator = (const std::array<value_type, _dimension>& _array) {
            assign(_array);

            return(*this);
        }

        WN_FORCE_INLINE matrix& operator = (const std::initializer_list<value_type>& _initializer_list) {
            assign(_initializer_list);

            return(*this);
        }

        WN_FORCE_INLINE matrix operator + () const {
            return(*this);
        }

        WN_FORCE_INLINE matrix operator - () const {
            matrix matrix(*this);

            traits_type::negate(matrix.m_element_array);

            return(matrix);
        }

        WN_FORCE_INLINE matrix& operator += (const matrix& _matrix) {
            traits_type::add(m_element_array, _matrix.m_element_array);

            return(*this);
        }

        WN_FORCE_INLINE matrix& operator -= (const matrix& _matrix) {
            traits_type::subtract(m_element_array, _matrix.m_element_array);

            return(*this);
        }

        WN_FORCE_INLINE matrix& operator *= (const matrix& _matrix) {
            matrix matrix(*this);

            for (wn_size_t i = 0; i < _order; ++i) {
                const wn_size_t row_offset = i * _order;

                for (wn_size_t j = 0; j < _order; ++j) {
                    value_type result = static_cast<value_type>(0);

                    for (wn_size_t k = 0; k < _order; ++k) {
                        result += (matrix.m_element_array.m_values[row_offset + k] *
                                   _matrix.m_element_array.m_values[k * _order + j]);
                    }

                    m_element_array.m_values[row_offset + j] = result;
                }
            }

            return(*this);
        }

        WN_FORCE_INLINE matrix& operator += (const value_type& _number) {
            traits_type::add(m_element_array, _number);

            return(*this);
        }

        WN_FORCE_INLINE matrix& operator -= (const value_type& _number) {
            traits_type::subtract(m_element_array, _number);

            return(*this);
        }

        WN_FORCE_INLINE matrix& operator *= (const value_type& _number) {
            traits_type::multiply(m_element_array, _number);

            return(*this);
        }

        WN_FORCE_INLINE matrix& operator /= (const value_type& _number) {
            traits_type::divide(m_element_array, _number);

            return(*this);
        }

        WN_FORCE_INLINE matrix operator + (const matrix& _matrix) const {
            matrix matrix(*this);

            matrix += _matrix;

            return(matrix);
        }

        WN_FORCE_INLINE matrix operator - (const matrix& _matrix) const {
            matrix matrix(*this);

            matrix -= _matrix;

            return(matrix);
        }

        WN_FORCE_INLINE matrix operator * (const matrix& _matrix) const {
            matrix matrix(*this);

            matrix *= _matrix;

            return(matrix);
        }

        WN_FORCE_INLINE matrix operator + (const value_type& _number) const {
            matrix matrix(*this);

            matrix += _number;

            return(matrix);
        }

        WN_FORCE_INLINE matrix operator - (const value_type& _number) const {
            matrix matrix(*this);

            matrix -= _number;

            return(matrix);
        }

        WN_FORCE_INLINE matrix operator * (const value_type& _number) const {
            matrix matrix(*this);

            matrix *= _number;

            return(matrix);
        }

        WN_FORCE_INLINE matrix operator / (const value_type& _number) const {
            matrix matrix(*this);

            matrix /= _number;

            return(matrix);
        }

        WN_FORCE_INLINE wn_bool operator == (const matrix& _matrix) const {
            return(traits_type::equal(m_element_array, _matrix.m_element_array));
        }

        WN_FORCE_INLINE wn_bool operator != (const matrix& _matrix) const {
            return(traits_type::not_equal(m_element_array, _matrix.m_element_array));
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

        template <const wn_size_t _row, const wn_size_t _column>
        WN_FORCE_INLINE value_type& at() {
            static_assert(_row < _order, "Attempting to get element outside of bounds");
            static_assert(_column < _order, "Attempting to get element outside of bounds");

            return(at((_row * _order) + _column));
        }

        template <const wn_size_t _row, const wn_size_t _column>
        WN_FORCE_INLINE const value_type& at() const {
            static_assert(_row < _order, "Attempting to get element outside of bounds");
            static_assert(_column < _order, "Attempting to get element outside of bounds");

            return(at((_row * _order) + _column));
        }

        WN_FORCE_INLINE value_type& at(const wn_size_t _index) {
            WN_DEBUG_ASSERT_DESC(_index < _dimension, "Attempting to get element outside of bounds");

            return(m_element_array.m_values[_index]);
        }

        WN_FORCE_INLINE const value_type& at(const wn_size_t _index) const {
            WN_DEBUG_ASSERT_DESC(_index < _dimension, "Attempting to get element outside of bounds");

            return(m_element_array.m_values[_index]);
        }

        WN_FORCE_INLINE value_type& at(const wn_size_t _row, const wn_size_t _column) {
            WN_DEBUG_ASSERT_DESC(_row < _order, "Attempting to get element outside of bounds");
            WN_DEBUG_ASSERT_DESC(_column < _order, "Attempting to get element outside of bounds");

            return(at((_row * _order) + _column));
        }

        WN_FORCE_INLINE const value_type& at(const wn_size_t _row, const wn_size_t _column) const {
            WN_DEBUG_ASSERT_DESC(_row < _order, "Attempting to get element outside of bounds");
            WN_DEBUG_ASSERT_DESC(_column < _order, "Attempting to get element outside of bounds");

            return(at((_row * _order) + _column));
        }

        WN_FORCE_INLINE value_type* data() {
            return(m_element_array.m_values);
        }

        WN_FORCE_INLINE const value_type* data() const {
            return(m_element_array.m_values);
        }

        WN_FORCE_INLINE wn_size_t order() const {
            return(_order);
        }

        WN_FORCE_INLINE wn_size_t dimension() const {
            return(_dimension);
        }

        WN_FORCE_INLINE value_type trace() const {
            value_type trace = static_cast<value_type>(0);

            for (wn_size_t i = 0; i < _order; ++i) {
                trace += m_element_array.m_values[(i * _order) + i];
            }

            return(trace);
        }

        WN_FORCE_INLINE value_type determinant() const;

        WN_FORCE_INLINE wn_bool identity() const {
            for (wn_size_t i = 0; i < _order; ++i) {
                const wn_size_t row_offset = i * _order;

                for (wn_size_t j = 0; j < _order; ++j) {
                    const wn_size_t index = row_offset + j;

                    if (i == j) {
                        if (m_element_array.m_values[index] != static_cast<value_type>(1)) {
                            return(wn_false);
                        }
                    } else {
                        if (m_element_array.m_values[index] != static_cast<value_type>(0)) {
                            return(wn_false);
                        }
                    }
                }
            }
        }

        WN_FORCE_INLINE wn_bool symmetric() const {

        }

        WN_FORCE_INLINE wn_bool diagonal() const {
            for (wn_size_t i = 0; i < _order; ++i) {
                const wn_size_t row_offset = i * _order;

                for (wn_size_t j = 0; j < _order; ++j) {
                    if (i != j) {
                        const wn_size_t index = row_offset + j;

                        if (m_element_array.m_values[index] != static_cast<value_type>(0)) {
                            return(wn_false);
                        }
                    }
                }
            }
        }

        WN_FORCE_INLINE wn_void transpose() {
            for (wn_size_t i = 0; i < _order; ++i) {
                const wn_size_t row_offset = i * _order;

                for (wn_size_t j = 0; j < _order; ++j) {
                    const wn_size_t index = row_offset + j;

                    std::swap(m_element_array.m_values[index], m_element_array.m_values[index]);
                }
            }
        }

        WN_FORCE_INLINE wn_void assign(matrix&& _vector) {
            traits_type::assign(m_element_array, std::move(_vector.m_element_array));
        }

        WN_FORCE_INLINE wn_void assign(const matrix& _vector) {
            traits_type::assign(m_element_array, _vector.m_element_array);
        }

        WN_FORCE_INLINE wn_void assign(const value_type& _value) {
            traits_type::assign(m_element_array, _value);
        }

        template <typename value_type,
                  typename... types,
                  typename = core::enable_if_t<core::boolean_and<core::are_same<value_type, types..., value_type>::value,
                                                     sizeof...(types) == (_dimension - 1), (_order > 1)>::value>>
        WN_FORCE_INLINE wn_void assign(const value_type& _value, const types&... _values) {
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

        WN_FORCE_INLINE wn_void set_zero() {
            traits_type::set_zero(m_element_array);
        }

        WN_FORCE_INLINE wn_void set_identity() {
            for (wn_size_t i = 0; i < _order; ++i) {
                for (wn_size_t j = 0; j < _order; ++j) {
                    const wn_size_t index = (i * _order) + j;

                    if (i == j) {
                        m_element_array.m_values[index] = static_cast<value_type>(1);
                    } else {
                        m_element_array.m_values[index] = static_cast<value_type>(0);
                    }
                }
            }
        }

        WN_FORCE_INLINE matrix extract_transposed() const {
            matrix matrix(*this);

            return(matrix.transpose(), matrix);
        }

        WN_FORCE_INLINE matrix<value_type, (_order - 1)> extract_sub_matrix(const wn_size_t _row, const wn_size_t _column) {
            matrix<value_type, (_order - 1)> matrix;
            wn_size_t column_count = 0;
            wn_size_t row_count = 0;

            for (wn_size_t i = 0; i < _order; ++i) {
                if (i != _row) {
                    const wn_size_t row_offset = row_count * _order;

                    column_count = 0;

                    for (wn_size_t j = 0; j < _order; ++j) {
                        if (j != _column) {
                            const wn_size_t index = row_offset + column_count;

                            m_element_array.m_values[index] = matrix.m_element_array.m_values[index];

                            column_count++;
                        }
                    }

                    row_count++;
                }
            }

            return(matrix);
        }

    private:
        template <typename _other_type, const wn_size_t _other_order>
        friend class matrix;

        internal::math::element_array<value_type, _dimension> m_element_array;
    };

    template <typename _type>
    using matrix2 = matrix<_type, 2>;

    template <typename _type>
    using matrix3 = matrix<_type, 3>;

    template <typename _type>
    using matrix4 = matrix<_type, 4>;
}

template <typename type, const wn_size_t order>
wn::matrix<type, order> operator + (const type& _value, wn::matrix<type, order>&& _matrix) {
    return(_matrix += _value);
}

template <typename type, const wn_size_t order>
wn::matrix<type, order> operator + (const type& _value, const wn::matrix<type, order>& _matrix) {
    return(_matrix + _value);
}

template <typename type, const wn_size_t order>
wn::matrix<type, order> operator - (const type& _value, const wn::matrix<type, order>& _matrix) {
    return(wn::matrix<type, order>(_value) -= _matrix);
}

template <typename type, const wn_size_t order>
wn::matrix<type, order> operator * (const type& _value, wn::matrix<type, order>&& _matrix) {
    return(_matrix *= _value);
}

template <typename type, const wn_size_t order>
wn::matrix<type, order> operator * (const type& _value, const wn::matrix<type, order>& _matrix) {
    return(_matrix * _value);
}

template <typename type, const wn_size_t order>
wn::matrix<type, order> operator / (const type& _value, const wn::matrix<type, order>& _matrix) {
    return(wn::matrix<type, order>(_value) /= _matrix);
}

#endif // __WN_MATH_MATRIX_H__
