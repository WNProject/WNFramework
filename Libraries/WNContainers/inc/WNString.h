// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CONTAINERS_STRING_H__
#define __WN_CONTAINERS_STRING_H__

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNCore/inc/WNAlgorithm.h"
#include "WNCore/inc/WNTypes.h"
#include "WNMath/inc/WNBasic.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNMemory/inc/WNStringUtility.h"

#include <limits>

namespace wn {
namespace containers {

class string;

namespace internal {

template <typename Container, typename NonConstContainer = Container,
    typename Element = typename Container::value_type>
class string_iterator WN_FINAL
  : public std::iterator<std::random_access_iterator_tag, Element,
        typename Container::difference_type> {
private:
  using base = std::iterator<std::random_access_iterator_tag, Element,
      typename Container::difference_type>;

public:
  using iterator_category = typename base::iterator_category;
  using value_type = typename base::value_type;
  using size_type = typename Container::size_type;
  using difference_type = typename base::difference_type;
  using pointer = typename base::pointer;
  using reference = typename base::reference;

  WN_FORCE_INLINE string_iterator() : m_owner(nullptr), m_position(0) {}

  WN_FORCE_INLINE string_iterator(string_iterator&& _other)
    : m_owner(core::move(_other.m_owner)),
      m_position(core::move(_other.m_position)) {
    _other.clear();
  }

  WN_FORCE_INLINE string_iterator(const string_iterator& _other)
    : m_owner(_other.m_owner), m_position(_other.m_position) {}

  template <typename OtherContainer = NonConstContainer,
      typename =
          core::enable_if_t<!core::is_same<Container, OtherContainer>::value>>
  WN_FORCE_INLINE string_iterator(string_iterator<OtherContainer,
      OtherContainer, typename OtherContainer::value_type>&& _other)
    : m_owner(core::move(_other.m_owner)),
      m_position(core::move(_other.m_position)) {
    _other.clear();
  }

  template <typename OtherContainer = NonConstContainer,
      typename =
          core::enable_if_t<!core::is_same<Container, OtherContainer>::value>>
  WN_FORCE_INLINE string_iterator(const string_iterator<OtherContainer,
      OtherContainer, typename OtherContainer::value_type>& _other)
    : m_owner(_other.m_owner), m_position(_other.m_position) {}

  WN_FORCE_INLINE string_iterator& operator=(string_iterator&& _other) {
    string_iterator(core::move(_other)).swap(*this);

    return *this;
  }

  WN_FORCE_INLINE string_iterator& operator=(const string_iterator& _other) {
    string_iterator(_other).swap(*this);

    return *this;
  }

  template <typename OtherContainer = NonConstContainer,
      typename =
          core::enable_if_t<!core::is_same<Container, OtherContainer>::value>>
  WN_FORCE_INLINE string_iterator& operator=(string_iterator<OtherContainer,
      OtherContainer, typename OtherContainer::value_type>&& _other) {
    string_iterator(core::move(_other)).swap(*this);

    return *this;
  }

  template <typename OtherContainer = NonConstContainer,
      typename =
          core::enable_if_t<!core::is_same<Container, OtherContainer>::value>>
  WN_FORCE_INLINE string_iterator& operator=(
      const string_iterator<OtherContainer, OtherContainer,
          typename OtherContainer::value_type>& _other) {
    string_iterator(_other).swap(*this);

    return *this;
  }

  WN_FORCE_INLINE difference_type operator-(
      const string_iterator& _other) const {
    WN_DEBUG_ASSERT(
        m_owner == _other.m_owner, "iterators are incompatible");

    return (m_position - _other.m_position);
  }

  WN_FORCE_INLINE string_iterator& operator+=(const size_type _count) {
    m_position += _count;

    return *this;
  }

  WN_FORCE_INLINE string_iterator& operator-=(const size_type _count) {
    m_position -= _count;

    return *this;
  }

  WN_FORCE_INLINE string_iterator operator+(const size_type _count) const {
    string_iterator i(*this);

    return (i += _count);
  }

  WN_FORCE_INLINE string_iterator operator-(const size_type _count) const {
    string_iterator i(*this);

    return (i -= _count);
  }

  WN_FORCE_INLINE reference operator*() const {
    return (*m_owner)[m_position];
  }

  WN_FORCE_INLINE pointer operator->() const {
    return &((*m_owner)[m_position]);
  }

  WN_FORCE_INLINE string_iterator operator++(int32_t) {
    string_iterator i(*this);

    (*this) += 1;

    return i;
  }

  WN_FORCE_INLINE string_iterator operator--(int32_t) {
    string_iterator i(*this);

    (*this) -= 1;

    return i;
  }

  WN_FORCE_INLINE string_iterator& operator++() {
    return ((*this) += 1);
  }

  WN_FORCE_INLINE string_iterator& operator--() {
    return ((*this) -= 1);
  }

  WN_FORCE_INLINE bool operator==(const string_iterator& _other) const {
    WN_DEBUG_ASSERT(
        m_owner == _other.m_owner, "iterators are incompatible");

    return (m_position == _other.m_position);
  }

  WN_FORCE_INLINE bool operator!=(const string_iterator& _other) const {
    WN_DEBUG_ASSERT(
        m_owner == _other.m_owner, "iterators are incompatible");

    return (m_position != _other.m_position);
  }

  WN_FORCE_INLINE bool operator>(const string_iterator& _other) const {
    WN_DEBUG_ASSERT(
        m_owner == _other.m_owner, "iterators are incompatible");

    return (m_position > _other.m_position);
  }

  WN_FORCE_INLINE bool operator>=(const string_iterator& _other) const {
    WN_DEBUG_ASSERT(
        m_owner == _other.m_owner, "iterators are incompatible");

    return (m_position >= _other.m_position);
  }

  WN_FORCE_INLINE bool operator<(const string_iterator& _other) const {
    WN_DEBUG_ASSERT(
        m_owner == _other.m_owner, "iterators are incompatible");

    return (m_position < _other.m_position);
  }

  WN_FORCE_INLINE bool operator<=(const string_iterator& _other) const {
    WN_DEBUG_ASSERT(
        m_owner == _other.m_owner, "iterators are incompatible");

    return (m_position <= _other.m_position);
  }

private:
  friend class wn::containers::string;
  friend class string_iterator<const Container, Container, const Element>;

  WN_FORCE_INLINE string_iterator(
      Container* _container, const typename Container::size_type _position)
    : m_owner(_container), m_position(_position) {}

  WN_FORCE_INLINE void swap(string_iterator& _other) {
    core::swap(m_owner, _other.m_owner);
    core::swap(m_position, _other.m_position);
  }

  WN_FORCE_INLINE void clear() {
    m_owner = nullptr;
    m_position = 0;
  }

  Container* m_owner;
  typename Container::size_type m_position;
};

}  //  namespace internal

class string_view;

class string WN_FINAL {
public:
  using value_type = char;
  using size_type = dynamic_array<value_type>::size_type;
  using difference_type = dynamic_array<value_type>::difference_type;
  using reference = dynamic_array<value_type>::reference;
  using const_reference = dynamic_array<value_type>::const_reference;
  using pointer = dynamic_array<value_type>::pointer;
  using const_pointer = dynamic_array<value_type>::const_pointer;
  using iterator = internal::string_iterator<string>;
  using const_iterator =
      internal::string_iterator<const string, string, const value_type>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  enum : size_type { npos = static_cast<size_type>(-1) };

  string() = default;

  WN_FORCE_INLINE string(const nullptr_t) : string() {}

  WN_FORCE_INLINE string(string&& _other) : m_data(core::move(_other.m_data)) {}

  WN_FORCE_INLINE string(const string& _other) : m_data(_other.m_data) {}

  WN_FORCE_INLINE explicit string(memory::allocator* _allocator)
    : m_data(_allocator) {}

  WN_FORCE_INLINE string(memory::allocator* _allocator, const size_type _count,
      const value_type _value)
    : string(_allocator) {
    m_data.reserve(_count + 1);
    m_data.insert(m_data.cbegin(), _count, _value);
    m_data.push_back('\0');
  }

  WN_FORCE_INLINE string(memory::allocator* _allocator,
      const value_type* _string, const size_type _count)
    : string(_allocator) {
    m_data.reserve(_count + 1);
    m_data.insert(m_data.cbegin(), _string, (_string + _count));
    m_data.push_back('\0');
  }

  WN_FORCE_INLINE string(memory::allocator* _allocator, const value_type* _ptr)
    : string(_allocator, _ptr, memory::strlen(_ptr)) {}

  string(memory::allocator* _allocator, const string_view& _view);

  WN_FORCE_INLINE string(memory::allocator* _allocator, const string& _other)
    : m_data(_allocator, _other.m_data.cbegin(), _other.m_data.cend()) {}

  WN_FORCE_INLINE string& operator=(nullptr_t) {
    assign(nullptr);

    return *this;
  }

  WN_FORCE_INLINE string& operator=(string&& _other) {
    assign(core::move(_other));

    return *this;
  }

  WN_FORCE_INLINE string& operator=(const string& _other) {
    assign(_other);

    return *this;
  }

  WN_FORCE_INLINE string& operator=(const value_type* _ptr) {
    assign(_ptr);

    return *this;
  }

  string& operator=(const string_view& _view);

  WN_FORCE_INLINE void assign(const nullptr_t) {
    string(nullptr).swap(*this);
  }

  WN_FORCE_INLINE void assign(string&& _other) {
    string(core::move(_other)).swap(*this);
  }

  WN_FORCE_INLINE void assign(const string& _other) {
    string(_other).swap(*this);
  }

  WN_FORCE_INLINE void assign(const value_type* _ptr) {
    string(m_data.get_allocator(), _ptr).swap(*this);
  }

  WN_FORCE_INLINE void assign(const value_type* _ptr, const size_type _count) {
    string(m_data.get_allocator(), _ptr, _count).swap(*this);
  }

  void assign(const string_view& _view);

  WN_FORCE_INLINE memory::allocator* get_allocator() const {
    return m_data.get_allocator();
  }

  // element access

  WN_FORCE_INLINE reference operator[](const size_type _pos) {
    return at(_pos);
  }

  WN_FORCE_INLINE const_reference operator[](const size_type _pos) const {
    return at(_pos);
  }

  WN_FORCE_INLINE reference at(const size_type _pos) {
    return m_data.at(_pos);
  }

  WN_FORCE_INLINE const_reference at(const size_type _pos) const {
    return m_data.at(_pos);
  }

  WN_FORCE_INLINE reference front() {
    return at(0);
  }

  WN_FORCE_INLINE const_reference front() const {
    return at(0);
  }

  WN_FORCE_INLINE reference back() {
    return at(size() - 1);
  }

  WN_FORCE_INLINE const_reference back() const {
    return at(size() - 1);
  }

  WN_FORCE_INLINE pointer data() {
    return m_data.data();
  }

  WN_FORCE_INLINE const_pointer data() const {
    return m_data.data();
  }

  WN_FORCE_INLINE const_pointer c_str() const {
    return data();
  }

  operator string_view() const;

  string_view to_string_view() const;

  WN_FORCE_INLINE contiguous_range<char> to_contiguous_range() {
    return contiguous_range<char>(data(), size());
  }

  WN_FORCE_INLINE contiguous_range<const char> to_contiguous_range() const {
    return contiguous_range<const char>(data(), size());
  }

  // iterators

  WN_FORCE_INLINE iterator begin() {
    return iterator(this, 0);
  }

  WN_FORCE_INLINE const_iterator begin() const {
    return cbegin();
  }

  WN_FORCE_INLINE const_iterator cbegin() const {
    return const_iterator(this, 0);
  }

  WN_FORCE_INLINE iterator end() {
    return iterator(this, size());
  }

  WN_FORCE_INLINE const_iterator end() const {
    return cend();
  }

  WN_FORCE_INLINE const_iterator cend() const {
    return const_iterator(this, size());
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
    return m_data.empty();
  }

  WN_FORCE_INLINE size_type size() const {
    return ((m_data.size() == 0) ? 0 : (m_data.size() - 1));
  }

  WN_FORCE_INLINE size_type length() const {
    return size();
  }

  WN_FORCE_INLINE size_type max_size() const {
    return std::numeric_limits<size_type>::max() - 1;
  }

  WN_FORCE_INLINE void reserve(const size_type _new_cap = 0) {
    size_type new_cap = _new_cap;

    if (new_cap > 0) {
      new_cap++;
    }

    m_data.reserve(new_cap);
  }

  WN_FORCE_INLINE size_type capacity() const {
    return ((m_data.capacity() == 0) ? 0 : (m_data.capacity() - 1));
  }

  WN_FORCE_INLINE void shrink_to_fit() {
    m_data.shrink_to_fit();
  }

  // operations

  WN_FORCE_INLINE string& operator+=(const string& _string) {
    append(_string);

    return *this;
  }

  WN_FORCE_INLINE string& operator+=(const value_type _value) {
    append(_value);

    return *this;
  }

  WN_FORCE_INLINE string& operator+=(const value_type* _ptr) {
    append(_ptr);

    return *this;
  }

  string& operator+=(const string_view& _view);

  WN_FORCE_INLINE void clear() {
    m_data.clear();
  }

  WN_FORCE_INLINE string& erase(
      const size_type _index = 0, const size_type _count = npos) {
    WN_DEBUG_ASSERT((_index < size()), "index is out of bounds");

    m_data.erase(
        (m_data.cbegin() + _index), math::min(_count, size() - _index));

    if (m_data.size() == 1) {
      clear();
    }

    return *this;
  }

  WN_FORCE_INLINE iterator erase(const_iterator _position) {
    return erase(_position, _position + 1);
  }

  WN_FORCE_INLINE iterator erase(const_iterator _first, const_iterator _last) {
    const size_type index = _first.m_position;

    erase(index, _last - _first);

    return iterator(this, index);
  }

  WN_FORCE_INLINE string& insert(
      const size_type _index, const size_type _count, const value_type _value) {
    WN_DEBUG_ASSERT((_index <= size()), "index is out of bounds");

    m_data.reserve(size() + _count + 1);

    if (empty()) {
      m_data.push_back('\0');
    }

    m_data.insert(m_data.cbegin() + _index, _count, _value);

    return *this;
  }

  WN_FORCE_INLINE string& insert(
      const size_type _index, const value_type* _ptr) {
    return insert(_index, _ptr, memory::strlen(_ptr));
  }

  WN_FORCE_INLINE string& insert(
      const size_type _index, const value_type* _ptr, const size_type _count) {
    WN_DEBUG_ASSERT((_index <= size()), "index is out of bounds");

    m_data.reserve(size() + _count + 1);

    if (empty()) {
      m_data.push_back('\0');
    }

    m_data.insert(m_data.cbegin() + _index, _ptr, _ptr + _count);

    return *this;
  }

  WN_FORCE_INLINE string& insert(const size_type _index, const string& _other) {
    return insert(_index, _other.data(), _other.size());
  }

  string& insert(const size_type _index, const string_view& _view);

  WN_FORCE_INLINE iterator insert(
      const_iterator _pos, const value_type _value) {
    return insert(_pos, 1, _value);
  }

  WN_FORCE_INLINE iterator insert(
      const_iterator _pos, const size_type _count, const value_type _value) {
    const size_type offset = _pos - cbegin();

    insert(offset, _count, _value);

    return (begin() + offset);
  }

  WN_FORCE_INLINE void push_back(const value_type _value) {
    insert(size(), 1, _value);
  }

  WN_FORCE_INLINE void pop_back() {
    erase(size() - 1, 1);
  }

  WN_FORCE_INLINE string& append(const string& _string) {
    return append(_string.data(), _string.size());
  }

  WN_FORCE_INLINE string& append(const value_type _value) {
    push_back(_value);

    return *this;
  }

  WN_FORCE_INLINE string& append(
      const value_type* _ptr, const size_type _count) {
    return insert(size(), _ptr, _count);
  }

  WN_FORCE_INLINE string& append(const value_type* _ptr) {
    return append(_ptr, memory::strlen(_ptr));
  }

  string& append(const string_view& _view);

  int32_t compare(const string& _other) const;

  int32_t compare(const value_type* _ptr) const;

  int32_t compare(const string_view& _view) const;

  WN_FORCE_INLINE string substr(
      const size_type _pos = 0, const size_type _count = npos) const {
    WN_DEBUG_ASSERT(_pos < size(), "string position is out of bounds");

    const size_type count =
        (_count == npos || (_pos + _count) > size()) ? size() - _pos : _count;

    return string(get_allocator(), (data() + _pos), count);
  }

  WN_FORCE_INLINE void resize(const size_type _count) {
    resize(_count, value_type());
  }

  WN_FORCE_INLINE void resize(const size_type _count, const value_type _value) {
    if (!m_data.empty()) {
      m_data.back() = _value;
    }

    m_data.resize((_count + 1), _value);
    m_data.back() = '\0';
  }

  WN_FORCE_INLINE void swap(string& _other) {
    m_data.swap(_other.m_data);
  }

  // search

  size_type rfind(const string& str, const size_type _pos = npos) const;
  size_type rfind(const value_type* _ptr, const size_type _pos,
      const size_type _count) const;
  size_type rfind(const value_type* _ptr, const size_type _pos = npos) const;
  size_type rfind(const value_type _value, const size_type _pos = npos) const;
  size_type rfind(const string_view _view, const size_type _pos = npos) const;

  size_type find_first_of(
      const string& _string, const size_type _pos = 0) const;
  size_type find_first_of(const value_type* _ptr, const size_type _pos,
      const size_type _count) const;
  size_type find_first_of(
      const value_type* _ptr, const size_type _pos = 0) const;
  size_type find_first_of(
      const value_type _value, const size_type _pos = 0) const;
  size_type find_first_of(
      const string_view _view, const size_type _pos = 0) const;

  size_type find_last_of(
      const string& _string, const size_type _pos = npos) const;
  size_type find_last_of(const value_type* _ptr, const size_type _pos,
      const size_type _count) const;
  size_type find_last_of(
      const value_type* _ptr, const size_type _pos = npos) const;
  size_type find_last_of(
      const value_type _value, const size_type _pos = npos) const;
  size_type find_last_of(
      const string_view _view, const size_type _pos = npos) const;

private:
  dynamic_array<char> m_data;
};

// addition operators

WN_FORCE_INLINE string operator+(const string& _lhs, const string& _rhs) {
  string s(_lhs.get_allocator());

  s.reserve(_lhs.size() + _rhs.size() + 1);
  s.append(_lhs);
  s.append(_rhs);

  return s;
}

WN_FORCE_INLINE string operator+(const char* _lhs, const string& _rhs) {
  const size_t count = memory::strlen(_lhs);
  string s(_rhs.get_allocator());

  s.reserve(count + _rhs.size() + 1);
  s.append(_lhs, count);
  s.append(_rhs);

  return s;
}

WN_FORCE_INLINE string operator+(const char _lhs, const string& _rhs) {
  string s(_rhs.get_allocator());

  s.reserve(_rhs.size() + 2);
  s.append(_lhs);
  s.append(_rhs);

  return s;
}

WN_FORCE_INLINE string operator+(const string& _lhs, const char* _rhs) {
  const size_t count = memory::strlen(_rhs);
  string s(_lhs.get_allocator());

  s.reserve(_lhs.size() + count + 1);
  s.append(_lhs);
  s.append(_rhs, count);

  return s;
}

WN_FORCE_INLINE string operator+(const string& _lhs, const char _rhs) {
  string s(_lhs.get_allocator());

  s.reserve(_lhs.size() + 2);
  s.append(_lhs);
  s.append(_rhs);

  return s;
}

WN_FORCE_INLINE string operator+(string&& _lhs, const string& _rhs) {
  _lhs += _rhs;

  return core::move(_lhs);
}

WN_FORCE_INLINE string operator+(const string& _lhs, string&& _rhs) {
  _rhs.insert(0, _lhs);

  return core::move(_rhs);
}

WN_FORCE_INLINE string operator+(string&& _lhs, string&& _rhs) {
  _lhs += core::move(_rhs);

  return core::move(_lhs);
}

WN_FORCE_INLINE string operator+(const char* _lhs, string&& _rhs) {
  _rhs.insert(0, _lhs);

  return core::move(_rhs);
}

WN_FORCE_INLINE string operator+(const char _lhs, string&& _rhs) {
  _rhs.insert(0, 1, _lhs);

  return core::move(_rhs);
}

WN_FORCE_INLINE string operator+(string&& _lhs, const char* _rhs) {
  _lhs += _rhs;

  return core::move(_lhs);
}

WN_FORCE_INLINE string operator+(string&& _lhs, const char _rhs) {
  _lhs += _rhs;

  return core::move(_lhs);
}

// comparison operators

WN_FORCE_INLINE bool operator==(const string& _lhs, const string& _rhs) {
  return (_lhs.compare(_rhs) == 0);
}

WN_FORCE_INLINE bool operator==(const char* _lhs, const string& _rhs) {
  return (_rhs.compare(_lhs) == 0);
}

WN_FORCE_INLINE bool operator==(const string& _lhs, const char* _rhs) {
  return (_lhs.compare(_rhs) == 0);
}

WN_FORCE_INLINE bool operator!=(const string& _lhs, const string& _rhs) {
  return (_lhs.compare(_rhs) != 0);
}

WN_FORCE_INLINE bool operator!=(const char* _lhs, const string& _rhs) {
  return (_rhs.compare(_lhs) != 0);
}

WN_FORCE_INLINE bool operator!=(const string& _lhs, const char* _rhs) {
  return (_lhs.compare(_rhs) != 0);
}

WN_FORCE_INLINE bool operator>(const string& _lhs, const string& _rhs) {
  return (_lhs.compare(_rhs) > 0);
}

WN_FORCE_INLINE bool operator>=(const string& _lhs, const string& _rhs) {
  return (_lhs.compare(_rhs) >= 0);
}

WN_FORCE_INLINE bool operator<(const string& _lhs, const string& _rhs) {
  return (_lhs.compare(_rhs) < 0);
}

WN_FORCE_INLINE bool operator<=(const string& _lhs, const string& _rhs) {
  return (_lhs.compare(_rhs) <= 0);
}

}  // namespace containers
}  // namespace wn

// string_view related defintions

#include "WNContainers/inc/WNStringView.h"

namespace wn {
namespace containers {

WN_FORCE_INLINE string::string(
    memory::allocator* _allocator, const string_view& _view)
  : m_data(_allocator, _view.cbegin(), _view.cend()) {
  m_data.push_back('\0');
}

WN_FORCE_INLINE string& string::operator=(const string_view& _view) {
  assign(_view);

  return *this;
}

WN_FORCE_INLINE void string::assign(const string_view& _view) {
  string(m_data.get_allocator(), _view).swap(*this);
}

WN_FORCE_INLINE string::operator string_view() const {
  return to_string_view();
}

WN_FORCE_INLINE string_view string::to_string_view() const {
  return string_view(data(), size());
}

WN_FORCE_INLINE string& string::operator+=(const string_view& _view) {
  append(_view);

  return *this;
}

WN_FORCE_INLINE string& string::insert(
    const size_type _index, const string_view& _view) {
  return insert(_index, _view.data(), _view.size());
}

WN_FORCE_INLINE string& string::append(const string_view& _view) {
  return append(_view.data(), _view.size());
}

WN_FORCE_INLINE int32_t string::compare(const string_view& _view) const {
  return string_view(data(), size()).compare(_view);
}

WN_FORCE_INLINE int32_t string::compare(const string& _other) const {
  return compare(_other.to_string_view());
}

WN_FORCE_INLINE int32_t string::compare(const value_type* _ptr) const {
  return compare(string_view(_ptr, memory::strlen(_ptr)));
}

WN_FORCE_INLINE string::size_type string::rfind(
    const string& _string, const size_type _pos) const {
  return rfind(_string.to_string_view(), _pos);
}

WN_FORCE_INLINE string::size_type string::rfind(const value_type* _ptr,
    const size_type _pos, const size_type _count) const {
  return rfind(string_view(_ptr, _count), _pos);
}

WN_FORCE_INLINE string::size_type string::rfind(
    const value_type* _ptr, const size_type _pos) const {
  return rfind(string_view(_ptr), _pos);
}

WN_FORCE_INLINE string::size_type string::rfind(
    const value_type _value, const size_type _pos) const {
  return rfind(string_view(&_value, 1), _pos);
}

WN_FORCE_INLINE string::size_type string::rfind(
    const string_view _view, const size_type _pos) const {
  return to_string_view().rfind(_view, _pos);
}

WN_FORCE_INLINE string::size_type string::find_first_of(
    const string& _string, const size_type _pos) const {
  return find_first_of(_string.to_string_view(), _pos);
}

WN_FORCE_INLINE string::size_type string::find_first_of(const value_type* _ptr,
    const size_type _pos, const size_type _count) const {
  return find_first_of(string_view(_ptr, _count), _pos);
}

WN_FORCE_INLINE string::size_type string::find_first_of(
    const value_type* _ptr, const size_type _pos) const {
  return find_first_of(string_view(_ptr), _pos);
}

WN_FORCE_INLINE string::size_type string::find_first_of(
    const value_type _value, const size_type _pos) const {
  return find_first_of(string_view(&_value, 1), _pos);
}

WN_FORCE_INLINE string::size_type string::find_first_of(
    const string_view _view, const size_type _pos) const {
  return to_string_view().find_first_of(_view, _pos);
}

WN_FORCE_INLINE string::size_type string::find_last_of(
    const string& _string, const size_type _pos) const {
  return find_last_of(_string.to_string_view(), _pos);
}

WN_FORCE_INLINE string::size_type string::find_last_of(const value_type* _ptr,
    const size_type _pos, const size_type _count) const {
  return find_last_of(string_view(_ptr, _count), _pos);
}

WN_FORCE_INLINE string::size_type string::find_last_of(
    const value_type* _ptr, const size_type _pos) const {
  return find_last_of(string_view(_ptr), _pos);
}

WN_FORCE_INLINE string::size_type string::find_last_of(
    const value_type _value, const size_type _pos) const {
  return find_last_of(string_view(&_value, 1), _pos);
}

WN_FORCE_INLINE string::size_type string::find_last_of(
    const string_view _view, const size_type _pos) const {
  return to_string_view().find_last_of(_view, _pos);
}

}  // namespace containers
}  // namespace wn

namespace std {

template <>
struct hash<wn::containers::string> {
  WN_FORCE_INLINE size_t operator()(
      const wn::containers::string& _string) const {
    return wn::memory::strnhash(_string.data(), _string.size());
  }
};

template <>
struct equal_to<wn::containers::string> {
  WN_FORCE_INLINE size_t operator()(const wn::containers::string& _lhs,
      const wn::containers::string& _rhs) const {
    return (_lhs == _rhs);
  }
};

}  // namespace std

#endif  // __WN_CONTAINERS_STRING_H__
