// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CONTAINERS_DYNAMIC_ARRAY_H__
#define __WN_CONTAINERS_DYNAMIC_ARRAY_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNMemory/inc/allocator.h"

#include <iterator>
#include <limits>

namespace wn {
namespace containers {

template <typename T, const size_t _ExpandPercentage = 50>
class dynamic_array;

namespace internal {

template <typename T>
class dynamic_array_iterator {
public:
  using iterator_category = std::random_access_iterator_tag;
  using value_type = typename std::remove_const<T>::type;
  using difference_type = signed_t;
  using pointer = T*;
  using reference = T&;

  dynamic_array_iterator() : m_ptr(nullptr) {}

  template <typename _T2>
  dynamic_array_iterator(const dynamic_array_iterator<_T2>& _other) {
    m_ptr = _other.m_ptr;
  }

  dynamic_array_iterator& operator+=(size_t i) {
    m_ptr += i;

    return *this;
  }

  dynamic_array_iterator& operator-=(size_t i) {
    m_ptr -= i;

    return *this;
  }

  size_t operator-(const dynamic_array_iterator& other) {
    return (m_ptr - other.m_ptr);
  }

  pointer operator->() const {
    return (m_ptr);
  }

  reference operator*() const {
    return (*m_ptr);
  }

  dynamic_array_iterator& operator++() {
    return ((*this) += 1);
  }

  dynamic_array_iterator& operator--() {
    return (*this) -= 1;
  }

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
    return *this;
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

  template <typename T2, const size_t _ExpandPercentage>
  friend class ::wn::containers::dynamic_array;

  template <typename T2>
  friend class dynamic_array_iterator;
};

}  // namespace internal

template <typename T, const size_t _ExpandPercentage>
class dynamic_array final {
public:
  using value_type = T;
  using size_type = size_t;
  using difference_type = signed_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = internal::dynamic_array_iterator<value_type>;
  using const_iterator = internal::dynamic_array_iterator<const value_type>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  WN_FORCE_INLINE dynamic_array()
    : m_allocator(nullptr), m_data(nullptr), m_size(0), m_capacity(0) {}

  WN_FORCE_INLINE dynamic_array(const nullptr_t) : dynamic_array() {}

  WN_FORCE_INLINE explicit dynamic_array(memory::allocator* _allocator)
    : m_allocator(_allocator), m_data(nullptr), m_size(0), m_capacity(0) {}

  WN_FORCE_INLINE explicit dynamic_array(
      memory::allocator* _allocator, const size_type _count)
    : dynamic_array(_allocator) {
    for (size_t i = 0; i < _count; ++i) {
      insert(end(), T());
    }
  }

  WN_FORCE_INLINE dynamic_array(
      memory::allocator* _allocator, const size_type _count, const T& _value)
    : dynamic_array(_allocator) {
    insert(iterator(m_data), _count, _value);
  }

  template <const size_t _ExpandPercentageOther>
  dynamic_array(memory::allocator* _allocator,
      const dynamic_array<T, _ExpandPercentageOther>& _other)
    : dynamic_array(_allocator) {
    (*this) = _other;
  }

  dynamic_array(dynamic_array&& _other) : dynamic_array() {
    (*this) = core::move(_other);
  }

  dynamic_array(const dynamic_array& _other)
    : dynamic_array(_other.m_allocator) {
    (*this) = _other;
  }

  template <const size_t _ExpandPercentageOther>
  dynamic_array(dynamic_array<T, _ExpandPercentageOther>&& _other)
    : dynamic_array() {
    (*this) = core::move(_other);
  }

  dynamic_array(std::initializer_list<T> l) : dynamic_array() {
    insert(begin(), l.begin(), l.end());
  }

  dynamic_array(memory::allocator* _allocator, std::initializer_list<T> l)
    : dynamic_array(_allocator) {
    insert(begin(), l.begin(), l.end());
  }

  template <typename InputIt>
  dynamic_array(memory::allocator* _allocator, InputIt _first, InputIt _last)
    : dynamic_array(_allocator) {
    reserve(_last - _first);
    while (_first != _last) {
      push_back(*_first);
      ++_first;
    }
  }

  template <typename InputIt>
  dynamic_array(memory::allocator* _allocator, InputIt _first, size_t _size)
    : dynamic_array(_allocator) {
    reserve(_size);
    while (_size) {
      push_back(*_first);
      ++_first;
      --_size;
    }
  }

  WN_FORCE_INLINE ~dynamic_array() {
    clear();
    deallocate(m_data);
  }

  WN_FORCE_INLINE operator contiguous_range<value_type>() {
    return to_contiguous_range();
  }

  WN_FORCE_INLINE operator contiguous_range<value_type>() const {
    return to_contiguous_range();
  }

  WN_FORCE_INLINE contiguous_range<value_type> to_contiguous_range() {
    return contiguous_range<value_type>(data(), size());
  }

  WN_FORCE_INLINE contiguous_range<const value_type> to_contiguous_range()
      const {
    return contiguous_range<const value_type>(data(), size());
  }

  template <const size_t _ExpandPercentageOther, typename U,
      typename = core::enable_if_t<core::conjunction<
          core::is_same<core::remove_cv<U>, core::remove_cv<T>>,
          core::is_copy_constructible<T>>::value>>
  dynamic_array& operator=(
      const dynamic_array<T, _ExpandPercentageOther>& _other) {
    if (&_other == this) {
      return *this;
    }
    if (m_data) {
      for (size_t i = 0; i < m_size; ++i) {
        m_data[i].~T();
      }
    }
    m_size = 0;
    reserve(_other.m_size);
    for (size_t i = 0; i < _other.m_size; ++i) {
      new (reinterpret_cast<void*>(&m_data[i])) T(_other.m_data[i]);
    }
    m_size = _other.m_size;
    return *this;
  }

  dynamic_array& operator=(const dynamic_array& _other) {
    if (&_other == this) {
      return *this;
    }
    if (m_data) {
      for (size_t i = 0; i < m_size; ++i) {
        m_data[i].~T();
      }
    }
    m_size = 0;
    reserve(_other.m_size);
    for (size_t i = 0; i < _other.m_size; ++i) {
      new (reinterpret_cast<void*>(&m_data[i])) T(_other.m_data[i]);
    }
    m_size = _other.m_size;
    return *this;
  }

  dynamic_array& operator=(dynamic_array&& _other) {
    if (&_other == this) {
      return *this;
    }
    if (m_data) {
      for (size_t i = 0; i < m_size; ++i) {
        m_data[i].~T();
      }
      deallocate(m_data);
    }
    m_allocator = _other.m_allocator;
    _other.m_allocator = nullptr;
    m_capacity = _other.m_capacity;
    _other.m_capacity = 0;
    m_data = _other.m_data;
    _other.m_data = 0;
    m_size = _other.m_size;
    _other.m_size = 0;
    return *this;
  }

  template <const size_t _ExpandPercentageOther>
  dynamic_array& operator=(dynamic_array<T, _ExpandPercentageOther>&& _other) {
    if (&_other == this) {
      return *this;
    }
    if (m_data) {
      for (size_t i = 0; i < m_size; ++i) {
        m_data[i].~T();
      }
      deallocate(m_data);
    }
    m_allocator = _other.m_allocator;
    _other.m_allocator = nullptr;
    m_capacity = _other.m_capacity;
    _other.m_capacity = 0;
    m_data = _other.m_data;
    _other.m_data = 0;
    m_size = _other.m_size;
    _other.m_size = 0;

    return *this;
  }

  WN_FORCE_INLINE memory::allocator* get_allocator() const {
    return m_allocator;
  }

  // element access

  WN_FORCE_INLINE reference operator[](const size_type _pos) {
    return at(_pos);
  }

  WN_FORCE_INLINE const_reference operator[](const size_type _pos) const {
    return at(_pos);
  }

  WN_FORCE_INLINE reference front() {
    return *begin();
  }

  WN_FORCE_INLINE const_reference front() const {
    return *cbegin();
  }

  WN_FORCE_INLINE reference back() {
    return *(--end());
  }

  WN_FORCE_INLINE const_reference back() const {
    return *(--end());
  }

  WN_FORCE_INLINE reference at(const size_type _pos) {
    return m_data[_pos];
  }

  WN_FORCE_INLINE const_reference at(const size_type _pos) const {
    return m_data[_pos];
  }

  WN_FORCE_INLINE T* data() {
    return m_data;
  }

  WN_FORCE_INLINE const T* data() const {
    return m_data;
  }

  // iterators

  WN_FORCE_INLINE iterator begin() {
    return iterator(m_data);
  }

  WN_FORCE_INLINE const_iterator begin() const {
    return cbegin();
  }

  WN_FORCE_INLINE const_iterator cbegin() const {
    return const_iterator(m_data);
  }

  WN_FORCE_INLINE iterator end() {
    return iterator(m_data + m_size);
  }

  WN_FORCE_INLINE const_iterator end() const {
    return cend();
  }

  WN_FORCE_INLINE const_iterator cend() const {
    return const_iterator(m_data + m_size);
  }

  WN_FORCE_INLINE reverse_iterator rbegin() {
    return reverse_iterator(end());
  }

  WN_FORCE_INLINE const_reverse_iterator rbegin() const {
    return crbegin();
  }

  WN_FORCE_INLINE const_reverse_iterator crbegin() const {
    return const_reverse_iterator(cend());
  }

  WN_FORCE_INLINE reverse_iterator rend() {
    return reverse_iterator(begin());
  }

  WN_FORCE_INLINE const_reverse_iterator rend() const {
    return crend();
  }

  WN_FORCE_INLINE const_reverse_iterator crend() const {
    return const_reverse_iterator(cbegin());
  }

  // capacity

  WN_FORCE_INLINE bool empty() const {
    return (size() == 0);
  }

  WN_FORCE_INLINE size_type size() const {
    return m_size;
  }

  WN_FORCE_INLINE size_type max_size() const {
    return std::numeric_limits<size_type>::max();
  }

  inline void reserve(const size_type _new_cap = 0) {
    if (_new_cap > m_capacity) {
      if (!m_data) {
        void* p = allocate(sizeof(T), _new_cap);

        m_capacity = _new_cap;
        m_data = reinterpret_cast<T*>(p);
      } else {
        // TODO optimize this based on T traits
        void* p = allocate(sizeof(T), _new_cap);

        m_capacity = _new_cap;

        T* new_data = reinterpret_cast<T*>(p);

        for (size_type i = 0; i < m_size; ++i) {
          new (reinterpret_cast<void*>(&new_data[i])) T(core::move(m_data[i]));

          m_data[i].~T();
        }

        deallocate(m_data);

        m_data = new_data;
      }
    }
  }

  WN_FORCE_INLINE size_type capacity() const {
    return m_capacity;
  }

  // operations

  WN_FORCE_INLINE void clear() {
    if (m_data) {
      for (size_type i = 0; i < size(); ++i) {
        m_data[i].~T();
      }

      m_size = 0;
    }
  }

  iterator insert(const_iterator _pos, T&& _value) {
    iterator position = shift(_pos, 1);
    iterator new_position = position;

    memory::construct_at<T>(position.m_ptr, core::move(_value));

    return (new_position);
  }

  iterator insert(const_iterator _pos, const T& _value) {
    T value(_value);

    return (insert(_pos, core::move(value)));
  }

  iterator insert(
      const_iterator _pos, const size_type _count, const T& _value) {
    size_type count = _count;
    iterator position = shift(_pos, count);
    iterator new_position = position;

    while (count--) {
      memory::construct_at<T>((position++).m_ptr, _value);
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
      memory::construct_at<T>((position++).m_ptr, *_first);
    }

    return (new_position);
  }

  iterator insert(
      const_iterator _pos, std::initializer_list<T> _initializer_list) {
    return (insert(_pos, _initializer_list.begin(), _initializer_list.end()));
  }

  template <typename _Function,
      typename =
          core::enable_if_t<core::is_callable<_Function, T(size_type)>::value>>
  iterator insert(
      const_iterator _pos, const size_type _count, _Function&& _generator) {
    iterator position = shift(_pos, _count);
    iterator new_position = position;

    for (size_type i = 0; i < _count; ++i) {
      memory::construct_at<T>((position++).m_ptr, core::move(_generator(i++)));
    }

    return (new_position);
  }

  template <typename... _Arguments>
  iterator emplace(const_iterator _pos, _Arguments&&... _arguments) {
    T value(std::forward<_Arguments>(_arguments)...);

    return (insert(_pos, core::move(value)));
  }

  iterator erase(const_iterator _pos) {
    return (erase(_pos, 1));
  }

  iterator erase(const_iterator _pos, const size_type _count) {
    WN_DEBUG_ASSERT(_pos >= const_iterator(m_data) &&
                        _pos <= const_iterator(m_data + m_size),
        "iterator out of bounds");
    WN_DEBUG_ASSERT((_pos + _count) <= const_iterator(m_data + m_size),
        "range out of bounds");

    return unshift(_pos, _count);
  }

  iterator erase(const_iterator _first, const_iterator _last) {
    WN_DEBUG_ASSERT(_first >= const_iterator(m_data) &&
                        _first <= const_iterator(m_data + m_size),
        "iterator out of bounds");
    WN_DEBUG_ASSERT(_last >= const_iterator(m_data) &&
                        _last <= const_iterator(m_data + m_size),
        "iterator out of bounds");

    const difference_type count = _last - _first;

    if (count > 0) {
      return erase(_first, static_cast<size_type>(count));
    }

    return iterator(m_data + (_first.m_ptr - m_data));
  }

  WN_FORCE_INLINE void push_back(T&& _value) {
    insert(cend(), core::move(_value));
  }

  WN_FORCE_INLINE void push_back(const T& _value) {
    T value(_value);

    push_back(core::move(value));
  }

  template <typename... _Arguments>
  WN_FORCE_INLINE void emplace_back(_Arguments&&... _arguments) {
    push_back(T(std::forward<_Arguments>(_arguments)...));
  }

  WN_FORCE_INLINE void pop_back() {
    erase(cend() - 1);
  }

  WN_FORCE_INLINE void resize(const size_type _count) {
    resize(_count, value_type());
  }

  WN_FORCE_INLINE void resize(
      const size_type _count, const value_type& _value) {
    if (_count > m_size) {
      insert(const_iterator(m_data + m_size), (_count - m_size), _value);
    } else {
      erase(const_iterator(m_data + _count), const_iterator(m_data + m_size));
    }
  }

  WN_FORCE_INLINE void swap(dynamic_array& _other) {
    if (&_other != this) {
      core::swap(m_allocator, _other.m_allocator);
      core::swap(m_capacity, _other.m_capacity);
      core::swap(m_data, _other.m_data);
      core::swap(m_size, _other.m_size);
    }
  }

  WN_FORCE_INLINE void shrink_to_fit() {
    const size_type current_size = size();

    if (capacity() != current_size) {
      T* allocated = reinterpret_cast<T*>(allocate(sizeof(T), current_size));
      T* current = allocated;

      for (T* i = m_data; i < (m_data + current_size); ++i) {
        new (reinterpret_cast<void*>(current++)) T(core::move(*i));

        i->~T();
      }

      deallocate(m_data);

      m_data = allocated;
      m_capacity = current_size;
    }
  }

private:
  WN_FORCE_INLINE void* allocate(const size_t _size, const size_t _count) {
    return m_allocator->allocate(_size * _count);
  }

  WN_FORCE_INLINE void deallocate(void* _ptr) {
    if (m_allocator) {
      m_allocator->deallocate(_ptr);
    } else {
      WN_DEBUG_ASSERT(
          _ptr == nullptr, "m_allocator is nullptr, where did ptr come from");
    }
  }

  inline iterator shift(const_iterator _pos, const size_type _count) {
    WN_DEBUG_ASSERT(_pos <= const_iterator(m_data + m_size) &&
                        (_pos >= const_iterator(m_data)),
        "iterator out of bounds");

    const size_type originalPosition = _pos.m_ptr - m_data;

    if (m_capacity < (m_size + _count)) {
      iterator startPt = iterator(m_data + (_pos - begin()));
      const size_type new_capacity = static_cast<size_type>(
          (m_size + _count) * (1 + (_ExpandPercentage / 100.0f)));
      void* memory = allocate(sizeof(T), new_capacity);

      m_capacity = new_capacity;

      T* new_data = reinterpret_cast<T*>(memory);
      T* allocated = new_data;

      for (T* i = m_data; i < startPt.m_ptr; ++i) {
        new (reinterpret_cast<void*>(new_data++)) T(core::move(*i));

        i->~T();
      }

      new_data += _count;

      for (T* i = startPt.m_ptr; i < m_data + m_size; ++i) {
        new (reinterpret_cast<void*>(new_data++)) T(core::move(*i));

        i->~T();
      }

      deallocate(m_data);

      m_data = allocated;
    } else {
      for (T* i = m_data + m_size + _count - 1; i > (_pos.m_ptr + _count - 1);
           --i) {
        new (reinterpret_cast<void*>(i)) T(core::move(*(i - _count)));

        (i - _count)->~T();
      }
    }

    m_size += _count;

    return iterator(m_data + originalPosition);
  }

  inline iterator unshift(const_iterator _pos, const size_type _count) {
    WN_DEBUG_ASSERT(_pos <= const_iterator(m_data + m_size) &&
                        (_pos >= const_iterator(m_data)),
        "iterator out of bounds");
    WN_DEBUG_ASSERT(_pos + _count <= const_iterator(m_data + m_size),
        "range out of bounds");

    iterator itr(m_data + (_pos.m_ptr - m_data));

    for (T* i = itr.m_ptr; i < itr.m_ptr + _count; ++i) {
      i->~T();
    }

    for (T* i = itr.m_ptr + _count; i < m_data + m_size; ++i) {
      new (reinterpret_cast<void*>(i - _count)) T(core::move(*i));

      i->~T();
    }

    m_size -= _count;

    return itr;
  }

  memory::allocator* m_allocator;
  value_type* m_data;
  size_type m_size;
  size_type m_capacity;
};

}  // namespace containers
}  // namespace wn

namespace std {
template <typename T, const size_t _ExpandPercentage>
struct hash<wn::containers::dynamic_array<T, _ExpandPercentage>> {
  std::hash<T> _underlying_hash;
  size_t operator()(
      const wn::containers::dynamic_array<T, _ExpandPercentage>& _arr) const {
    size_t bits = sizeof(size_t) * 8;
    size_t hash = 0;
    for (size_t i = 0; i < _arr.size(); ++i) {
      hash ^= _underlying_hash(_arr[i]);
      hash = (hash >> 11) | (hash << (bits - 11));
    }
    return hash;
  }
};

template <typename T, const size_t _ExpandPercentage>
struct equal_to<wn::containers::dynamic_array<T, _ExpandPercentage>> {
  std::equal_to<T> _underlying_equality;
  bool operator()(const wn::containers::dynamic_array<T, _ExpandPercentage>& _0,
      const wn::containers::dynamic_array<T, _ExpandPercentage>& _1) const {
    if (_0.size() != _1.size()) {
      return false;
    }
    for (size_t i = 0; i < _0.size(); ++i) {
      if (!_underlying_equality(_0[i], _1[i])) {
        return false;
      }
    }
    return true;
  }
};

}  // namespace std

#endif
