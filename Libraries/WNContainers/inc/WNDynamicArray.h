// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_DYNAMIC_ARRAY_H__
#define __WN_CONTAINERS_DYNAMIC_ARRAY_H__

#include "WNMemory/inc/WNAllocator.h"
#include <iterator>

namespace wn {
namespace containers {

template <typename _Type, typename _Allocator = memory::default_allocator>
class dynamic_array;

namespace internal {
template <typename T>
class dynamic_array_iterator
    : public std::iterator<std::random_access_iterator_tag, T> {
 public:
  typedef T* pointer;
  typedef T& reference;
  dynamic_array_iterator() : m_ptr(wn_nullptr) {}

  template <typename _T2>
  dynamic_array_iterator(const dynamic_array_iterator<_T2>& _other) {
    m_ptr = _other.m_ptr;
  }

  dynamic_array_iterator& operator+=(wn_size_t i) {
    m_ptr += i;
    return (*this);
  }

  dynamic_array_iterator& operator-=(wn_size_t i) {
    m_ptr -= i;
    return *this;
  }

  wn_size_t operator-(const dynamic_array_iterator& other) {
    return (m_ptr - other.m_ptr);
  }

  pointer operator->() { return (m_ptr); }
  reference operator*() { return (*m_ptr); }

  dynamic_array_iterator& operator++() { return ((*this) += 1); }

  dynamic_array_iterator& operator--() { return (*this) -= 1; }

  dynamic_array_iterator operator++(int) {
    dynamic_array_iterator i = *this;
    *this += 1;
    return (i);
  }

  dynamic_array_iterator operator--(int) {
    dynamic_array_iterator i = *this;
    *this -= 1;
    return (i);
  }

  template <typename _T2>
  dynamic_array_iterator& operator=(const dynamic_array_iterator<_T2>& _other) {
    m_ptr = _other.m_ptr;
    return (*this);
  }

  template <typename _T>
  bool operator==(const dynamic_array_iterator<_T>& _other) const {
    return (m_ptr == _other.m_ptr);
  }

  template <typename _T>
  bool operator!=(const dynamic_array_iterator<_T>& _other) const {
    return !(*this == _other);
  }

  template <typename _T>
  bool operator<(const dynamic_array_iterator<_T>& _other) const {
    return (m_ptr < _other.m_ptr);
  }

  template <typename _T>
  bool operator<=(const dynamic_array_iterator<_T>& _other) const {
    return (m_ptr <= _other.m_ptr);
  }

  template <typename _T>
  bool operator>(const dynamic_array_iterator<_T>& _other) const {
    return (m_ptr > _other.m_ptr);
  }

  template <typename _T>
  bool operator>=(const dynamic_array_iterator<_T>& _other) const {
    return (m_ptr >= _other.m_ptr);
  }
  dynamic_array_iterator operator+(size_t i) const {
    dynamic_array_iterator it = (*this);
    it += i;
    return it;
  }
  dynamic_array_iterator operator-(size_t i) const {
    dynamic_array_iterator it = (*this);
    it -= i;
    return it;
  }

 private:
  explicit dynamic_array_iterator(T* _ptr) : m_ptr(_ptr) {}
  T* m_ptr;

  template <typename _Type, typename _Allocator>
  friend class wn::containers::dynamic_array;

  template <typename _Type>
  friend class dynamic_array_iterator;
};
}  // namespace internal

template <typename _Type, typename _Allocator>
class dynamic_array final {
 public:
  static _Allocator s_default_allocator;
  typedef _Type value_type;
  typedef wn_size_t size_type;
  typedef wn_signed_t difference_type;
  typedef _Allocator allocator_type;
  typedef value_type& reference;
  typedef const value_type& const_reference;

  typedef internal::dynamic_array_iterator<value_type> iterator;
  typedef internal::dynamic_array_iterator<const value_type> const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  dynamic_array() : dynamic_array(&s_default_allocator) {}

  explicit dynamic_array(memory::allocator* _allocator)
      : m_allocator(_allocator), m_data(wn_nullptr), m_size(0), m_capacity(0) {}

  explicit dynamic_array(const size_type _count,
                         memory::allocator* _allocator = &s_default_allocator)
      : dynamic_array(_count, _Type(), _allocator) {}

  dynamic_array(const size_type _count, const _Type& _value,
                memory::allocator* _allocator = &s_default_allocator)
      : dynamic_array(_allocator) {
    insert(iterator(m_data), _count, _value);
  }

  template <typename T_Alloc>
  dynamic_array(const dynamic_array<_Type, T_Alloc>& _other)
      : dynamic_array() {
    (*this) = _other;
  }

  dynamic_array(const dynamic_array& _other)
      : dynamic_array(_other.m_allocator) {
    (*this) = _other;
  }

  dynamic_array(dynamic_array&& _other) : dynamic_array() {
    (*this) = std::move(_other);
  }

  template <typename T_Alloc>
  dynamic_array(dynamic_array<_Type, T_Alloc>&& _other)
      : dynamic_array() {
    (*this) = std::move(_other);
  }

  template <typename TOther>
  dynamic_array(TOther begin, TOther end,
                memory::allocator* _alloc = &s_default_allocator) :
    dynamic_array(_alloc) {
    reserve(end - begin);
    while(begin != end) {
      push_back(*begin);
      ++begin;
    }
  }

  ~dynamic_array() {
    clear();

    m_allocator->deallocate(m_data);
  }

  template <typename T_Alloc>
  dynamic_array& operator=(const dynamic_array<_Type, T_Alloc>& _other) {
    if (&_other == this) {
      return (*this);
    }
    if (m_data) {
      for (wn_size_t i = 0; i < m_size; ++i) {
        m_data[i].~_Type();
      }
    }
    m_size = 0;
    reserve(_other.m_size);
    for (wn_size_t i = 0; i < _other.m_size; ++i) {
      new (reinterpret_cast<wn_void*>(&m_data[i])) _Type(_other.m_data[i]);
    }
    m_size = _other.m_size;
    return(*this);
  }

  dynamic_array& operator=(const dynamic_array& _other) {
    if (&_other == this) {
      return (*this);
    }
    if (m_data) {
      for (wn_size_t i = 0; i < m_size; ++i) {
        m_data[i].~_Type();
      }
    }
    m_size = 0;
    reserve(_other.m_size);
    for (wn_size_t i = 0; i < _other.m_size; ++i) {
      new (reinterpret_cast<wn_void*>(&m_data[i])) _Type(_other.m_data[i]);
    }
    m_size = _other.m_size;
    return(*this);
  }

  dynamic_array& operator=(dynamic_array&& _other) {
    if (&_other == this) {
      return (*this);
    }
    if (m_data) {
      for (wn_size_t i = 0; i < m_size; ++i) {
        m_data[i].~_Type();
      }
      m_allocator->deallocate(m_data);
    }
    m_allocator = _other.m_allocator;
    _other.m_allocator = &s_default_allocator;
    m_capacity = _other.m_capacity;
    _other.m_capacity = 0;
    m_data = _other.m_data;
    _other.m_data = 0;
    m_size = _other.m_size;
    _other.m_size = 0;
    return (*this);
  }

  template <typename T_Alloc>
  dynamic_array& operator=(dynamic_array<_Type, T_Alloc>&& _other) {
    if (&_other == this) {
      return (*this);
    }
    if (m_data) {
      for (wn_size_t i = 0; i < m_size; ++i) {
        m_data[i].~_Type();
      }
      m_allocator->deallocate(m_data);
    }
    m_allocator = _other.m_allocator;
    _other.m_allocator = &s_default_allocator;
    m_capacity = _other.m_capacity;
    _other.m_capacity = 0;
    m_data = _other.m_data;
    _other.m_data = 0;
    m_size = _other.m_size;
    _other.m_size = 0;
    return (*this);
  }

  // element access

  reference operator[](const size_type _pos) { return (at(_pos)); }

  const_reference operator[](const size_type _pos) const { return (at(_pos)); }

  reference front() { return (*begin()); }

  const_reference front() const { return (*cbegin()); }

  reference back() {
    iterator i = end();

    --i;

    return (*i);
  }

  const_reference back() const {
    const_reference i = cend();

    --i;

    return (*i);
  }

  reference at(const size_type _pos) { return (m_data[_pos]); }

  const_reference at(const size_type _pos) const { return (m_data[_pos]); }

  _Type* data() { return (m_data); }

  const _Type* data() const { return (m_data); }

  // iterators

  iterator begin() { return (iterator(m_data)); }

  const_iterator begin() const { return (cbegin()); }

  const_iterator cbegin() const { return (const_iterator(m_data)); }

  iterator end() { return (iterator(m_data + m_size)); }

  const_iterator end() const { return (cend()); }

  const_iterator cend() const { return (const_iterator(m_data + m_size)); }

  reverse_iterator rbegin() { return (reverse_iterator(end())); }

  const_reverse_iterator rbegin() const { return (crbegin()); }

  const_reverse_iterator crbegin() const {
    return (const_reverse_iterator(cend()));
  }

  reverse_iterator rend() { return (reverse_iterator(begin())); }

  const_reverse_iterator rend() const { return (crend()); }

  const_reverse_iterator crend() const {
    return (const_reverse_iterator(cbegin()));
  }

  // capacity

  size_type empty() const { return (size() == 0); }

  size_type size() const { return (m_size); }

  size_type capacity() const { return (m_capacity); }

  wn_void reserve(const size_type _new_cap) {
    if (_new_cap > m_capacity) {
      if (!m_data) {
        wn::memory::allocation_pair pair =
            m_allocator->allocate(sizeof(_Type), _new_cap);
        m_capacity = pair.m_count;
        m_data = reinterpret_cast<_Type*>(pair.m_location);
      } else {
        // TODO optimize this based on _Type traits
        wn::memory::allocation_pair pair = m_allocator->allocate_for_resize(
            sizeof(_Type), _new_cap, m_capacity);
        m_capacity = pair.m_count;
        _Type* newData = reinterpret_cast<_Type*>(pair.m_location);
        for (wn_size_t i = 0; i < m_size; ++i) {
          new (reinterpret_cast<wn_void*>(&newData[i]))
              _Type(std::move(m_data[i]));
          m_data[i].~_Type();
        }
        m_allocator->deallocate(m_data);
        m_data = newData;
      }
    }
  }

  // modifiers

  wn_void clear() {
    if (m_data != wn_nullptr) {
      for (size_type i = 0; i < m_size; ++i) {
        m_data[i].~_Type();
      }

      m_size = 0;
    }
  }

  iterator insert(const_iterator _pos, _Type&& _value) {
    iterator position = shift(_pos, 1);
    iterator new_position = position;

    memory::construct_at<_Type>(position.m_ptr, std::move(_value));

    return (new_position);
  }

  iterator insert(const_iterator _pos, const _Type& _value) {
    _Type value(_value);

    return (insert(_pos, std::move(_value)));
  }

  iterator insert(const_iterator _pos, const size_type _count,
                  const _Type& _value) {
    size_type count = _count;
    iterator position = shift(_pos, count);
    iterator new_position = position;

    while (count--) {
      memory::construct_at<_Type>((position++).m_ptr, _value);
    }

    return (new_position);
  }

  template <typename _InputIt,
            typename = core::enable_if_t<!std::is_integral<_InputIt>::value>>
  iterator insert(const_iterator _pos, _InputIt _first, _InputIt _last) {
    const difference_type count = _last - _first;
    iterator position = shift(_pos, count);
    iterator new_position = position;

    for (; _first != _last; ++_first) {
      memory::construct_at<_Type>((position++).m_ptr, *_first);
    }

    return (new_position);
  }

  iterator insert(const_iterator _pos,
                  std::initializer_list<_Type> _initializer_list) {
    return (insert(_pos, _initializer_list.begin(), _initializer_list.end()));
  }

  template <typename _Function, typename = core::enable_if_t<core::is_callable<
                                    _Function, _Type(size_type)>::value>>
  iterator insert(const_iterator _pos, const size_type _count,
                  _Function&& _generator) {
    iterator position = shift(_pos, _count);
    iterator new_position = position;

    for (size_type i = 0; i < _count; ++i) {
      memory::construct_at<_Type>((position++).m_ptr,
                                  std::move(_generator(i++)));
    }

    return (new_position);
  }

  template <typename... _Arguments>
  iterator emplace(const_iterator _pos, _Arguments&&... _arguments) {
    _Type value(std::forward<_Arguments>(_arguments)...);

    return (insert(_pos, std::move(value)));
  }

  iterator erase(const_iterator _pos) { return (erase(_pos, 1)); }

  iterator erase(const_iterator _pos, const size_type _count) {
    WN_DEBUG_ASSERT(_pos >= const_iterator(m_data) &&
                    _pos <= const_iterator(m_data + m_size));
    WN_DEBUG_ASSERT(_pos + _count < const_iterator(m_data + m_size));

    return (unshift(_pos, _count));
  }

  iterator erase(const_iterator _first, const_iterator _last) {
    WN_DEBUG_ASSERT(_first >= const_iterator(m_data) &&
                    _first <= const_iterator(m_data + m_size));
    WN_DEBUG_ASSERT(_last >= const_iterator(m_data) &&
                    _last <= const_iterator(m_data + m_size));

    const difference_type count = _last - _first;

    if (count > 0) {
      return (erase(_first, static_cast<size_type>(count)));
    }

    return (iterator(m_data + (_first.m_ptr - m_data)));
  }

  wn_void push_back(_Type&& _value) { insert(cend(), std::move(_value)); }

  wn_void push_back(const _Type& _value) {
    _Type value(_value);

    push_back(std::move(value));
  }

  template <typename... _Arguments>
  wn_void emplace_back(_Arguments&&... _arguments) {
    _Type value(std::forward<_Arguments>(_arguments)...);

    push_back(std::move(value));
  }

  wn_void pop_back() { erase(cend() - 1); }

  wn_void resize(const size_type _count) {
    if (_count > m_size) {
      shift(const_iterator(m_data), m_size - _count);
    } else {
      erase(const_iterator(m_data + m_size),
            const_iterator(m_data + m_size + _count));
    }
  }

  wn_void swap(dynamic_array& _other) {
    if (&_other != this) {
      std::swap(m_allocator, _other.m_allocator);
      std::swap(m_capacity, _other.m_capacity);
      std::swap(m_data, _other.m_data);
      std::swap(m_size, _other.m_size);
    }
  }

 private:
  iterator shift(const_iterator _pos, const size_type _count) {
    WN_DEBUG_ASSERT(_pos <= const_iterator(m_data + m_size) &&
                    (_pos >= const_iterator(m_data)));

    wn_size_t originalPosition = _pos.m_ptr - m_data;

    if (m_capacity < (m_size + _count)) {
      iterator startPt = iterator(m_data + (_pos - begin()));
      wn::memory::allocation_pair alloc = m_allocator->allocate_for_resize(
          sizeof(_Type), m_size + _count, m_capacity);
      m_capacity = alloc.m_count;
      _Type* newData = reinterpret_cast<_Type*>(alloc.m_location);
      _Type* allocated = newData;
      for (_Type* i = m_data; i < startPt.m_ptr; ++i) {
        new (reinterpret_cast<wn_void*>(newData++)) _Type(std::move(*i));
        i->~_Type();
      }
      newData += _count;
      for (_Type* i = startPt.m_ptr; i < m_data + m_size; ++i) {
        new (reinterpret_cast<wn_void*>(newData++)) _Type(std::move(*i));
        i->~_Type();
      }
      m_allocator->deallocate(m_data);
      m_data = allocated;
    } else {
      for (_Type* i = m_data + m_size + _count - 1;
           i > (_pos.m_ptr + _count - 1); --i) {
        new (reinterpret_cast<wn_void*>(i)) _Type(std::move(*(i - _count - 1)));
        (i - _count - 1)->~_Type();
      }
    }
    m_size = m_size + _count;
    return (iterator(m_data + originalPosition));
  }

  iterator unshift(const_iterator _pos, const size_type _count) {
    WN_DEBUG_ASSERT(_pos <= const_iterator(m_data + m_size) &&
                    (_pos >= const_iterator(m_data)));
    WN_DEBUG_ASSERT(_pos + _count <= const_iterator(m_data + m_size));

    iterator startPt = iterator(m_data + (_pos.m_ptr - m_data));

    for (_Type* i = startPt.m_ptr; i < startPt.m_ptr + _count; ++i) {
      i->~_Type();
    }

    for (_Type* i = startPt.m_ptr + _count; i < m_data + m_size; ++i) {
      new (reinterpret_cast<wn_void*>(i - _count)) _Type(std::move(*i));
      i->~_Type();
    }
    m_size = m_size - _count;
    return (startPt);
  }

  memory::allocator* m_allocator;
  value_type* m_data;
  size_type m_capacity;
  size_type m_size;
};
template <typename _Type, typename _Allocator>
_Allocator dynamic_array<_Type, _Allocator>::s_default_allocator;
}
};

#endif
