// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_CIRCLE_H__
#define __WN_MATH_CIRCLE_H__

#include "WNMath/inc/WNVector.h"

namespace wn {
    template <typename type> class WNBounds2;
    template <typename type> class WNRectangle;

    template <typename Type>
    class circle {
    public:
        typedef Type value_type;

        WN_FORCE_INLINE circle() :
            m_location(static_cast<value_type>(0)),
            m_radius(static_cast<value_type>(0)) {
        }

        WN_FORCE_INLINE circle(circle&& _circle) :
            m_location(std::move(_circle.m_location)),
            m_radius(std::move(_circle.m_radius)) {
        }

        WN_FORCE_INLINE circle(const circle& _circle) :
            m_location(_circle.m_location),
            m_radius(_circle.m_radius) {
        }

        WN_FORCE_INLINE explicit circle(const value_type* _numbers) :
            m_location(_numbers[0], _numbers[1]),
            m_radius(_numbers[2]) {
        }

        WN_FORCE_INLINE explicit circle(vector2<value_type>&& _location, value_type&& _radius) :
            m_location(std::move(_location)),
            m_radius(std::move(_radius)) {
        }

        WN_FORCE_INLINE explicit circle(vector2<value_type>&& _location, const value_type& _radius) :
            m_location(std::move(_location)),
            m_radius(_radius) {
        }

        WN_FORCE_INLINE explicit circle(const vector2<value_type>& _location, value_type&& _radius) :
            m_location(_location),
            m_radius(std::move(_radius)) {
        }

        WN_FORCE_INLINE explicit circle(const vector2<value_type>& _location, const value_type& _radius) :
            m_location(_location),
            m_radius(_radius) {
        }

        WN_FORCE_INLINE explicit circle(const value_type& _x, const value_type& _y, const value_type& _radius) :
            m_location(_x, _y),
            m_radius(_radius) {
        }

        WN_FORCE_INLINE value_type& operator [] (const wn_size_t _index) {
            return(at(_index));
        }

        WN_FORCE_INLINE const value_type& operator [] (const wn_size_t _index) const {
            return(at(_index));
        }

        WN_FORCE_INLINE value_type& operator () (const wn_size_t _index) {
            return(at(_index));
        }

        WN_FORCE_INLINE const value_type& operator () (const wn_size_t _index) const {
            return(at(_index));
        }

        WN_FORCE_INLINE circle& operator = (circle&& _circle) {
            m_location = std::move(_circle.m_location);
            m_radius = std::move(_circle.m_radius);

            return(*this);
        }

        WN_FORCE_INLINE circle& operator = (const circle& _circle) {
            m_location = _circle.m_location;
            m_radius = _circle.m_radius;

            return(*this);
        }

        WN_FORCE_INLINE wn_bool operator == (const circle& _circle) const {
            return(m_location == _circle.m_location && m_radius == _circle.m_radius);
        }

        WN_FORCE_INLINE wn_bool operator != (const circle& _circle) const {
            return(m_location != _circle.m_location || m_radius != _circle.m_radius);
        }

        template <const wn_size_t _index>
        WN_FORCE_INLINE value_type& at() {
            static_assert(_index < 3, "Attempting to get element outside of bounds");

            return(at(_index));
        }

        template <const wn_size_t _index>
        WN_FORCE_INLINE const value_type& at() const {
            static_assert(_index < 3, "Attempting to get element outside of bounds");

            return(at(_index));
        }

        WN_FORCE_INLINE value_type& at(const wn_size_t _index) {
            WN_DEBUG_ASSERT_DESC(_index < 3, "Attempting to get element outside of bounds");

            return(_index > 1 ? m_radius : m_location[_index]);
        }

        WN_FORCE_INLINE const value_type& at(const wn_size_t _index) const {
            WN_DEBUG_ASSERT_DESC(_index < 3, "Attempting to get element outside of bounds");

            return(_index > 1 ? m_radius : m_location[_index]);
        }

        WN_FORCE_INLINE const value_type& radius() const {
            return(m_radius);
        }

        WN_FORCE_INLINE const vector2<value_type>& location() const {
            return(m_location);
        }

        WN_FORCE_INLINE value_type circumference() const {
            return(static_cast<value_type>(WN_PI) * static_cast<value_type>(2) * m_radius);
        }

        WN_FORCE_INLINE value_type area() const {
            return(static_cast<value_type>(WN_PI) * m_radius * m_radius);
        }

        WN_FORCE_INLINE wn_bool zero() const {
            return(m_location.zero() && m_radius == static_cast<value_type>(0));
        }

        template <typename _type = value_type,
                  typename = core::enable_if_t<core::is_signed<_type>::value>>
        WN_FORCE_INLINE wn_bool inside_out() const {
            return(m_radius < static_cast<value_type>(0));
        }

        WN_FORCE_INLINE wn_void expand(const value_type& _amount) {
            m_radius += _amount;
        }

        wn_bool Expand(const vector2<value_type>& _vector, wn_bool _anchor = wn_false);

        WN_FORCE_INLINE wn_bool expand(const WNRectangle<value_type>& _box, const wn_bool _anchor = wn_false) {
            vector2<value_type> points[4];

            _box.GetPoints(points);

            return(expand(points[0], _anchor) || expand(points[1], _anchor) ||
                   expand(points[2], _anchor) || expand(points[3], _anchor));
        }

        WN_FORCE_INLINE wn_bool expand(const WNBounds2<value_type>& _bounds, const wn_bool _anchor = wn_false) {
            vector2<value_type> points[4];

            _bounds.GetPoints(points);

            return(expand(points[0], _anchor) || expand(points[1], _anchor) ||
                   expand(points[2], _anchor) || expand(points[3], _anchor));
        }

        wn_bool Expand(const circle& _circle, wn_bool _anchor = wn_false);

        WN_FORCE_INLINE wn_void translate(const vector2<value_type>& _vector) {
            m_location.translate(_vector);
        }

        WN_FORCE_INLINE wn_void translate(const value_type& _x, const value_type& _y) {
            m_location.translate(_x, _y);
        }

        WN_FORCE_INLINE wn_void translate_x(const value_type& _value) {
            m_location[0] += _value;
        }

        WN_FORCE_INLINE wn_void translate_y(const value_type& _value) {
            m_location[1] += _value;
        }

        WN_FORCE_INLINE wn_void scale(const value_type& _factor) {
            m_radius *= _factor;
        }

        WN_FORCE_INLINE wn_void assign(vector2<value_type>&& _location, const value_type& _radius) {
            set_location(std::move(_location));
            set_radius(_radius);
        }

        WN_FORCE_INLINE wn_void assign(const vector2<value_type>& _location, const value_type& _radius) {
            set_location(_location);
            set_radius(_radius);
        }

        WN_FORCE_INLINE wn_void assign(const value_type& _x, const value_type& _y, const value_type& _radius) {
            set_location(vector2<value_type>(_x, _y));
            set_radius(_radius);
        }

        WN_FORCE_INLINE wn_void set_radius(const value_type& _value) {
            m_radius = _value;
        }

        WN_FORCE_INLINE wn_void set_location(vector2<value_type>&& _vector) {
            m_location = std::move(_vector);
        }

        WN_FORCE_INLINE wn_void set_location(const vector2<value_type>& _vector) {
            m_location = _vector;
        }

        WN_FORCE_INLINE wn_void set_location(const value_type& _x, const value_type& _y) {
            m_location.assign(_x, _y);
        }

        WN_FORCE_INLINE wn_void set_location_x(const value_type& _value) {
            m_location[0] = _value;
        }

        WN_FORCE_INLINE wn_void set_location_y(const value_type& _value) {
            m_location[1] = _value;
        }

        WN_FORCE_INLINE wn_void set_zero() {
            m_location.set_zero();

            m_radius = static_cast<value_type>(0);
        }

        WN_FORCE_INLINE circle extract_expanded(const value_type& _amount) const {
            circle circle(*this);

            return(circle.expand(_amount), circle);
        }

        circle GetExpanded(const vector2<value_type>& _vector, wn_bool _anchor = wn_false) const;
        circle GetExpanded(const WNRectangle<value_type>& _rectangle, wn_bool _anchor = wn_false) const;
        circle GetExpanded(const WNBounds2<value_type>& _bounds, wn_bool _anchor = wn_false) const;
        circle GetExpanded(const circle& _circle, wn_bool _anchor = wn_false) const;

        WN_FORCE_INLINE circle extract_translated(const vector2<value_type>& _vector) const {
            circle circle(*this);

            return(circle.translate(_vector), circle);
        }

        WN_FORCE_INLINE circle extract_translated(const value_type& _x, const value_type& _y) const {
            circle circle(*this);

            return(circle.translate(_x, _y), circle);
        }

        WN_FORCE_INLINE circle extract_translated_x(const value_type& _value) const {
            circle circle(*this);

            return(circle.translate_x(_value), circle);
        }

        WN_FORCE_INLINE circle extract_translated_y(const value_type& _value) const {
            circle circle(*this);

            return(circle.translate_y(_value), circle);
        }

        WN_FORCE_INLINE circle extract_scaled(const value_type& _value) const {
            circle circle(*this);

            return(circle.scale(_value), circle);
        }

    public:
        vector2<value_type> m_location;
        value_type m_radius;
    };
}

#endif // __WN_MATH_CIRCLE_H__
