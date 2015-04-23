// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_SPHERE_H__
#define __WN_MATH_SPHERE_H__

#include "WNMath/inc/WNBasic.h"
#include "WNMath/inc/WNVector.h"

namespace wn {
    template <typename type>
    class WNBounds3;

    template <typename type>
    class WNBox;

    template <typename Type>
    class sphere {
    public:
        typedef Type value_type;

        WN_FORCE_INLINE sphere() :
            m_location(static_cast<value_type>(0)),
            m_radius(static_cast<value_type>(0)) {
        }

        WN_FORCE_INLINE sphere(sphere&& _sphere) :
            m_location(std::move(_sphere.m_location)),
            m_radius(std::move(_sphere.m_radius)) {
        }

        WN_FORCE_INLINE sphere(const sphere& _sphere) :
            m_location(_sphere.m_location),
            m_radius(_sphere.m_radius) {
        }

        WN_FORCE_INLINE explicit sphere(const value_type* _numbers) :
            m_location(_numbers[0], _numbers[1], _numbers[2]),
            m_radius(_numbers[3]) {
        }

        WN_FORCE_INLINE explicit sphere(vector3<value_type>&& _location, value_type&& _radius) :
            m_location(std::move(_location)),
            m_radius(std::move(_radius)) {
        }

        WN_FORCE_INLINE explicit sphere(vector3<value_type>&& _location, const value_type& _radius) :
            m_location(std::move(_location)),
            m_radius(_radius) {
        }

        WN_FORCE_INLINE explicit sphere(const vector3<value_type>& _location, value_type&& _radius) :
            m_location(_location),
            m_radius(std::move(_radius)) {
        }

        WN_FORCE_INLINE explicit sphere(const vector3<value_type>& _location, const value_type& _radius) :
            m_location(_location),
            m_radius(_radius) {
        }

        WN_FORCE_INLINE explicit sphere(const value_type& _x, const value_type& _y, const value_type& _z,
                                        const value_type& _radius) :
            m_location(_x, _y, _z),
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

        WN_FORCE_INLINE sphere& operator = (sphere&& _sphere) {
            m_location = std::move(_sphere.m_location);
            m_radius = std::move(_sphere.m_radius);

            return(*this);
        }

        WN_FORCE_INLINE sphere& operator = (const sphere& _sphere) {
            m_location = _sphere.m_location;
            m_radius = _sphere.m_radius;

            return(*this);
        }

        WN_FORCE_INLINE wn_bool operator == (const sphere& _sphere) const {
            return(m_location == _sphere.m_location && m_radius == _sphere.m_radius);
        }

        WN_FORCE_INLINE wn_bool operator != (const sphere& _sphere) const {
            return(m_location != _sphere.m_location || m_radius != _sphere.m_radius);
        }

        template <const wn_size_t _index>
        WN_FORCE_INLINE value_type& at() {
            static_assert(_index < 4, "Attempting to get element outside of bounds");

            return(at(_index));
        }

        template <const wn_size_t _index>
        WN_FORCE_INLINE const value_type& at() const {
            static_assert(_index < 4, "Attempting to get element outside of bounds");

            return(at(_index));
        }

        WN_FORCE_INLINE value_type& at(const wn_size_t _index) {
            WN_DEBUG_ASSERT_DESC(_index < 4, "Attempting to get element outside of bounds");

            return(_index > 2 ? m_radius : m_location[_index]);
        }

        WN_FORCE_INLINE const value_type& at(const wn_size_t _index) const  {
            WN_DEBUG_ASSERT_DESC(_index < 4, "Attempting to get element outside of bounds");

            return(_index > 2 ? m_radius : m_location[_index]);
        }

        WN_FORCE_INLINE const value_type& radius() const {
            return(m_radius);
        }

        WN_FORCE_INLINE const vector3<value_type>& location() const {
            return(m_location);
        }

        WN_FORCE_INLINE value_type volume() const {
            const value_type radius_cubed = m_radius * m_radius * m_radius;

            return(static_cast<value_type>(4) / static_cast<value_type>(3) * static_cast<value_type>(WN_PI) * radius_cubed);
        }

        WN_FORCE_INLINE value_type surface_area() const {
            const value_type radius_squared = m_radius * m_radius;

            return(static_cast<value_type>(4) * static_cast<value_type>(WN_PI) * radius_squared);
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

        WN_FORCE_INLINE wn_bool expand(const vector3<value_type>& _vector, const wn_bool _anchor = wn_false) {
            if (m_radius < static_cast<value_type>(0)) {
                if (!_anchor) {
                    m_location = _vector;
                    m_radius = static_cast<value_type>(0);
                } else {
                    const vector3<value_type> difference = m_location - _vector;
                    const value_type distance = difference.length();

                    m_radius = distance;
                }

                return(wn_true);
            } else {
                const vector3<value_type> difference = m_location - _vector;
                const value_type distance_squared = difference.length_squared();

                if (distance_squared > m_radius * m_radius) {
                    const value_type distance = sqrt(distance_squared);

                    if (!_anchor) {
                        m_location += ((_vector - m_location) / static_cast<value_type>(2)) *
                                      (static_cast<value_type>(1) - m_radius / distance);
                        m_radius += (distance - m_radius) / static_cast<value_type>(2);
                    } else {
                        m_radius = distance;
                    }

                    return(wn_true);
                }
            }

            return(wn_false);
        }

        WN_FORCE_INLINE wn_bool expand(const WNBox<value_type>& _box, const wn_bool _anchor = wn_false) {
            vector3<value_type> points[8];

            _box.GetPoints(points);

            return(expand(points[0], _anchor) || expand(points[1], _anchor) ||
                   expand(points[2], _anchor) || expand(points[3], _anchor) ||
                   expand(points[4], _anchor) || expand(points[5], _anchor) ||
                   expand(points[6], _anchor) || expand(points[7], _anchor));
        }

        WN_FORCE_INLINE wn_bool expand(const WNBounds3<value_type>& _bounds, const wn_bool _anchor = wn_false) {
            vector3<value_type> points[8];

            _bounds.GetPoints(points);

            return(expand(points[0], _anchor) || expand(points[1], _anchor) ||
                   expand(points[2], _anchor) || expand(points[3], _anchor) ||
                   expand(points[4], _anchor) || expand(points[5], _anchor) ||
                   expand(points[6], _anchor) || expand(points[7], _anchor));
        }

        WN_FORCE_INLINE wn_bool expand(const sphere& _sphere, const wn_bool _anchor = wn_false) {
            if (m_radius < static_cast<value_type>(0)) {
                if (!_anchor) {
                    m_location = _sphere.m_location;
                    m_radius = _sphere.m_radius;
                } else {
                    const vector3<value_type> difference = m_location - _sphere.m_location;
                    const value_type distance = difference.length();

                    m_radius = distance + _sphere.m_radius;
                }

                return(wn_true);
            } else {
                const vector3<value_type> difference = m_location - _sphere.m_location;
                const value_type distance_squared = difference.length_squared();

                if (distance_squared > (m_radius + _sphere.m_radius) * (m_radius + _sphere.m_radius)) {
                    const value_type distance = sqrt(distance_squared);

                    if (!_anchor) {
                        m_location += (_sphere.m_location - m_location) / static_cast<value_type>(2) *
                                      (static_cast<value_type>(1) - m_radius / (distance + _sphere.m_radius));
                        m_radius += ((distance + _sphere.m_radius) - m_radius) / static_cast<value_type>(2);
                    } else {
                        m_radius = distance;
                    }

                    return(wn_true);
                }
            }

            return(wn_false);
        }

        WN_FORCE_INLINE wn_void translate(const vector3<value_type>& _vector) {
            m_location.translate(_vector);
        }

        WN_FORCE_INLINE wn_void translate(const value_type& _x, const value_type& _y, const value_type& _z) {
            m_location.translate(_x, _y, _z);
        }

        WN_FORCE_INLINE wn_void translate_x(const value_type& _value) {
            m_location[0] += _value;
        }

        WN_FORCE_INLINE wn_void translate_y(const value_type& _value) {
            m_location[1] += _value;
        }

        WN_FORCE_INLINE wn_void translate_z(const value_type& _value) {
            m_location[2] += _value;
        }

        WN_FORCE_INLINE wn_void scale(const value_type& _value) {
            m_radius *= _value;
        }

        WN_FORCE_INLINE wn_void assign(vector3<value_type>&& _location, const value_type& _radius) {
            set_location(std::move(_location));
            set_radius(_radius);
        }

        WN_FORCE_INLINE wn_void assign(const vector3<value_type>& _location, const value_type& _radius) {
            set_location(_location);
            set_radius(_radius);
        }

        WN_FORCE_INLINE wn_void assign(const value_type& _x, const value_type& _y, const value_type& _z,
                                    const value_type& _radius) {
            set_location(vector3<value_type>(_x, _y, _z));
            set_radius(_radius);
        }

        WN_FORCE_INLINE wn_void set_radius(const value_type& _value) {
            m_radius = _value;
        }

        WN_FORCE_INLINE wn_void set_location(vector3<value_type>&& _vector) {
            m_location = std::move(_vector);
        }

        WN_FORCE_INLINE wn_void set_location(const vector3<value_type>& _vector) {
            m_location = _vector;
        }

        WN_FORCE_INLINE wn_void set_location(const value_type& _x, const value_type& _y, const value_type& _z) {
            m_location.assign(_x, _y, _z);
        }

        WN_FORCE_INLINE wn_void set_location_x(const value_type& _value) {
            m_location[0] = _value;
        }

        WN_FORCE_INLINE wn_void set_location_y(const value_type& _value) {
            m_location[1] = _value;
        }

        WN_FORCE_INLINE wn_void set_location_z(const value_type& _value) {
            m_location[2] = _value;
        }

        WN_FORCE_INLINE wn_void set_zero() {
            m_location.set_zero();

            m_radius = static_cast<value_type>(0);
        }

        WN_FORCE_INLINE sphere extract_expanded(const value_type& _amount) const {
            sphere sphere(*this);

            return(sphere.expand(_amount), sphere);
        }

        WN_FORCE_INLINE sphere extract_expanded(const vector3<value_type>& _vector, const wn_bool _anchor = wn_false) const {
            sphere sphere(*this);

            return(sphere.expand(_vector, _anchor), sphere);
        }

        WN_FORCE_INLINE sphere extract_expanded(const WNBox<value_type>& _box, const wn_bool _anchor = wn_false) const {
            sphere sphere(*this);

            return(sphere.expand(_box, _anchor), sphere);
        }

        WN_FORCE_INLINE sphere extract_expanded(const WNBounds3<value_type>& _bounds, const wn_bool _anchor = wn_false) const {
            sphere sphere(*this);

            return(sphere.expand(_bounds, _anchor), sphere);
        }

        WN_FORCE_INLINE sphere extract_expanded(const sphere& _sphere, const wn_bool _anchor = wn_false) const {
            sphere sphere(*this);

            return(sphere.expand(_sphere, _anchor), sphere);
        }

        WN_FORCE_INLINE sphere extract_translated(const vector3<value_type>& _vector) const {
            sphere sphere(*this);

            return(sphere.translate(_vector), sphere);
        }

        WN_FORCE_INLINE sphere extract_translated(const value_type& _x, const value_type& _y, const value_type& _z) const {
            sphere sphere(*this);

            return(sphere.translate(_x, _y, _z), sphere);
        }

        WN_FORCE_INLINE sphere extract_translated_x(const value_type& _value) const {
            sphere sphere(*this);

            return(sphere.translate_x(_value), sphere);
        }

        WN_FORCE_INLINE sphere extract_translated_y(const value_type& _value) const {
            sphere sphere(*this);

            return(sphere.translate_y(_value), sphere);
        }

        WN_FORCE_INLINE sphere extract_translated_z(const value_type& _value) const {
            sphere sphere(*this);

            return(sphere.translate_z(_value), sphere);
        }

        WN_FORCE_INLINE sphere extract_scaled(const value_type& _value) const {
            sphere sphere(*this);

            return(sphere.scale(_value), sphere);
        }

    private:
        vector3<value_type> m_location;
        value_type m_radius;
    };
}

#endif // __WN_MATH_SPHERE_H__
