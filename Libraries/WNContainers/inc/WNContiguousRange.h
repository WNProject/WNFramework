// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_CONTIGUOUS_RANGE_H__
#define __WN_CONTAINERS_CONTIGUOUS_RANGE_H__

#include "WNContainers/inc/WNArray.h"
#include "WNMemory/inc/manipulation.h"
#include "core/inc/assert.h"
#include "core/inc/type_traits.h"
#include "core/inc/utilities.h"

#include <iterator>

namespace wn {
namespace containers {

template <typename T>
class contiguous_range;

namespace internal {

template <typename Container, typename NonConstContainer = Container,
    typename Element = typename Container::value_type>
class contiguous_range_iterator final
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

#ifdef _WN_DEBUG
  inline contiguous_range_iterator() : m_owner(nullptr), m_pointer(nullptr) {}

  inline contiguous_range_iterator(contiguous_range_iterator&& _other) noexcept
    : m_owner(core::move(_other.m_owner)),
      m_pointer(core::move(_other.m_pointer)) {
    _other.clear();
  }

  inline contiguous_range_iterator(const contiguous_range_iterator& _other)
    : m_owner(_other.m_owner), m_pointer(_other.m_pointer) {}

  template <typename OtherContainer = NonConstContainer,
      typename =
          core::enable_if_t<!core::is_same<Container, OtherContainer>::value>>
  inline contiguous_range_iterator(contiguous_range_iterator<OtherContainer,
      OtherContainer, typename OtherContainer::value_type>&& _other)
    : m_owner(core::move(_other.m_owner)),
      m_pointer(core::move(_other.m_pointer)) {
    _other.clear();
  }

  template <typename OtherContainer = NonConstContainer,
      typename =
          core::enable_if_t<!core::is_same<Container, OtherContainer>::value>>
  inline contiguous_range_iterator(
      const contiguous_range_iterator<OtherContainer, OtherContainer,
          typename OtherContainer::value_type>& _other)
    : m_owner(_other.m_owner), m_pointer(_other.m_pointer) {}
#else
  inline contiguous_range_iterator() : m_pointer(nullptr) {}

  inline contiguous_range_iterator(contiguous_range_iterator&& _other)
    : m_pointer(core::move(_other.m_pointer)) {
    _other.clear();
  }

  inline contiguous_range_iterator(const contiguous_range_iterator& _other)
    : m_pointer(_other.m_pointer) {}

  template <typename OtherContainer = NonConstContainer,
      typename =
          core::enable_if_t<!core::is_same<Container, OtherContainer>::value>>
  inline contiguous_range_iterator(contiguous_range_iterator<OtherContainer,
      OtherContainer, typename OtherContainer::value_type>&& _other)
    : m_pointer(core::move(_other.m_pointer)) {
    _other.clear();
  }

  template <typename OtherContainer = NonConstContainer,
      typename =
          core::enable_if_t<!core::is_same<Container, OtherContainer>::value>>
  inline contiguous_range_iterator(
      const contiguous_range_iterator<OtherContainer, OtherContainer,
          typename OtherContainer::value_type>& _other)
    : m_pointer(_other.m_pointer) {}
#endif

  inline contiguous_range_iterator& operator=(
      contiguous_range_iterator&& _other) noexcept {
    contiguous_range_iterator(core::move(_other)).swap(*this);

    return *this;
  }

  inline contiguous_range_iterator& operator=(
      const contiguous_range_iterator& _other) {
    contiguous_range_iterator(_other).swap(*this);

    return *this;
  }

  template <typename OtherContainer = NonConstContainer,
      typename =
          core::enable_if_t<!core::is_same<Container, OtherContainer>::value>>
  inline contiguous_range_iterator& operator=(
      contiguous_range_iterator<OtherContainer, OtherContainer,
          typename OtherContainer::value_type>&& _other) {
    contiguous_range_iterator(core::move(_other)).swap(*this);

    return *this;
  }

  template <typename OtherContainer = NonConstContainer,
      typename =
          core::enable_if_t<!core::is_same<Container, OtherContainer>::value>>
  inline contiguous_range_iterator& operator=(
      const contiguous_range_iterator<OtherContainer, OtherContainer,
          typename OtherContainer::value_type>& _other) {
    contiguous_range_iterator(_other).swap(*this);

    return *this;
  }

  inline difference_type operator-(
      const contiguous_range_iterator& _other) const {
    WN_DEBUG_ASSERT(m_owner == _other.m_owner, "iterators are incompatible");

    return (m_pointer - _other.m_pointer);
  }

  inline contiguous_range_iterator& operator+=(const size_type _count) {
    m_pointer += _count;

    return *this;
  }

  inline contiguous_range_iterator& operator-=(const size_type _count) {
    m_pointer -= _count;

    return *this;
  }

  inline contiguous_range_iterator operator+(const size_type _count) const {
    contiguous_range_iterator i(*this);

    return (i += _count);
  }

  inline contiguous_range_iterator operator-(const size_type _count) const {
    contiguous_range_iterator i(*this);

    return (i -= _count);
  }

  inline reference operator*() const {
    return *m_pointer;
  }

  inline pointer operator->() const {
    return m_pointer;
  }

  inline contiguous_range_iterator operator++(int32_t) {
    contiguous_range_iterator i(*this);

    (*this) += 1;

    return i;
  }

  inline contiguous_range_iterator operator--(int32_t) {
    contiguous_range_iterator i(*this);

    (*this) -= 1;

    return i;
  }

  inline contiguous_range_iterator& operator++() {
    return ((*this) += 1);
  }

  inline contiguous_range_iterator& operator--() {
    return ((*this) -= 1);
  }

  inline bool operator==(const contiguous_range_iterator& _other) const {
    WN_DEBUG_ASSERT(m_owner == _other.m_owner, "iterators are incompatible");

    return (m_pointer == _other.m_pointer);
  }

  inline bool operator!=(const contiguous_range_iterator& _other) const {
    WN_DEBUG_ASSERT(m_owner == _other.m_owner, "iterators are incompatible");

    return (m_pointer != _other.m_pointer);
  }

  inline bool operator>(const contiguous_range_iterator& _other) const {
    WN_DEBUG_ASSERT(m_owner == _other.m_owner, "iterators are incompatible");

    return (m_pointer > _other.m_pointer);
  }

  inline bool operator>=(const contiguous_range_iterator& _other) const {
    WN_DEBUG_ASSERT(m_owner == _other.m_owner, "iterators are incompatible");

    return (m_pointer >= _other.m_pointer);
  }

  inline bool operator<(const contiguous_range_iterator& _other) const {
    WN_DEBUG_ASSERT(m_owner == _other.m_owner, "iterators are incompatible");

    return (m_pointer < _other.m_pointer);
  }

  inline bool operator<=(const contiguous_range_iterator& _other) const {
    WN_DEBUG_ASSERT(m_owner == _other.m_owner, "iterators are incompatible");

    return (m_pointer <= _other.m_pointer);
  }

private:
  template <typename T>
  friend class wn::containers::contiguous_range;
  friend class contiguous_range_iterator<const Container, Container,
      const Element>;

#ifdef _WN_DEBUG
  inline explicit contiguous_range_iterator(Container* _owner, pointer _pointer)
    : m_owner(_owner), m_pointer(_pointer) {}
#else
  inline explicit contiguous_range_iterator(pointer _pointer)
    : m_pointer(_pointer) {}
#endif

  inline void swap(contiguous_range_iterator& _other) {
#ifdef _WN_DEBUG
    core::swap(m_owner, _other.m_owner);
#endif

    core::swap(m_pointer, _other.m_pointer);
  }

  inline void clear() {
#ifdef _WN_DEBUG
    m_owner = nullptr;
#endif

    m_pointer = nullptr;
  }

#ifdef _WN_DEBUG
  const Container* m_owner;
#endif

  pointer m_pointer;
};

}  // namespace internal

template <typename T>
class write_only final {
public:
  inline write_only& operator=(const T& _value) {
    m_value = _value;

    return *this;
  }

  write_only(write_only&&) = delete;
  write_only(const write_only&) = delete;

private:
  volatile T m_value;
};

namespace internal {

template <typename T>
struct is_write_only : core::false_type {};

template <typename T>
struct is_write_only<write_only<T>> : core::true_type {};

template <typename T>
struct remove_write_only {
  using type = T;
};

template <typename T>
struct remove_write_only<write_only<T>> {
  using type = T;
};

}  // namespace internal

template <typename T>
using is_write_only = internal::is_write_only<core::decay_t<T>>;

template <typename T>
using remove_write_only = internal::remove_write_only<
    core::conditional_t<is_write_only<T>::value, core::decay_t<T>, T>>;

template <typename T>
using remove_write_only_t = typename remove_write_only<T>::type;

template <typename T>
class contiguous_range final {
private:
  static_assert(sizeof(remove_write_only_t<T>[2]) == sizeof(T[2]),
      "write only value of type not compatible with raw pointer of same type");
  static_assert(!is_write_only<T>::value ||
                    (is_write_only<T>::value && !core::is_const<T>::value &&
                        !core::is_const<remove_write_only_t<T>>::value),
      "contiguous ranges of write only values cannot be const");

  using self_type = contiguous_range<T>;

public:
  typedef T value_type;
  typedef size_t size_type;
  typedef signed_t difference_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;
  typedef internal::contiguous_range_iterator<self_type> iterator;
  typedef internal::contiguous_range_iterator<const self_type, self_type,
      const T>
      const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  inline contiguous_range() : m_begin(nullptr), m_end(nullptr) {}

  inline contiguous_range(const nullptr_t) : contiguous_range() {}

  inline explicit contiguous_range(const nullptr_t, const nullptr_t)
    : contiguous_range() {}

  template <typename U,
      typename = core::enable_if_t<core::disjunction<core::is_same<U, T>,
          core::is_same<U, remove_write_only_t<T>>,
          core::is_same<U, core::remove_const_t<T>>>::value>>
  inline explicit contiguous_range(U* _begin, U* _end)
    : m_begin(reinterpret_cast<T*>(_begin)), m_end(reinterpret_cast<T*>(_end)) {
    WN_DEBUG_ASSERT((m_begin && m_end) || (!m_begin && !m_end),
        "invalid input parameters, both must be null or non-null");
  }

  template <typename U,
      typename = core::enable_if_t<core::disjunction<core::is_same<U, T>,
          core::is_same<U, remove_write_only_t<T>>,
          core::is_same<U, core::remove_const_t<T>>>::value>>
  inline explicit contiguous_range(U* _ptr, const size_type _size)
    : contiguous_range(_ptr, _ptr + _size) {}

  template <typename U,
      typename = core::enable_if_t<core::disjunction<core::is_same<U, T>,
          core::is_same<U, remove_write_only_t<T>>,
          core::is_same<U, core::remove_const_t<T>>>::value>>
  inline explicit contiguous_range(
      U* _ptr, const size_type _offset, const size_type _size)
    : contiguous_range(_ptr + _offset, _size) {}

  template <typename U, const size_t N,
      typename = core::enable_if_t<core::disjunction<core::is_same<U, T>,
          core::is_same<U, remove_write_only_t<T>>,
          core::is_same<U, core::remove_const_t<T>>>::value>>
  inline contiguous_range(array<U, N>& _array)
    : contiguous_range(_array.data(), N) {}

  template <typename U, const size_t N,
      typename = core::enable_if_t<core::disjunction<core::is_same<U, T>,
          core::is_same<core::remove_all_extents_t<U>, remove_write_only_t<T>>,
          core::is_same<core::remove_all_extents_t<U>,
              core::remove_const_t<T>>>::value>>
  inline contiguous_range(U (&_ptr)[N]) : contiguous_range(_ptr, N) {}

  template <typename U,
      typename = core::enable_if_t<core::disjunction<core::is_same<U, T>,
          core::is_same<U, remove_write_only_t<T>>,
          core::is_same<U, core::remove_const_t<T>>>::value>>
  inline contiguous_range(contiguous_range<U>&& _other)
    : contiguous_range(_other.data(), _other.size()) {
    _other = nullptr;
  }

  template <typename U,
      typename = core::enable_if_t<core::disjunction<core::is_same<U, T>,
          core::is_same<U, remove_write_only_t<T>>,
          core::is_same<U, core::remove_const_t<T>>>::value>>
  inline contiguous_range(const contiguous_range<U>& _other)
    : contiguous_range(_other.data(), _other.size()) {}

  inline contiguous_range(contiguous_range&& _other) noexcept
    : contiguous_range(_other.m_begin, _other.m_end) {
    _other = nullptr;
  }

  inline contiguous_range(const contiguous_range& _other)
    : contiguous_range(_other.m_begin, _other.m_end) {}

  inline contiguous_range& operator=(const nullptr_t) {
    contiguous_range(nullptr).swap(*this);

    return *this;
  }

  template <typename U, const size_t N,
      typename = core::enable_if_t<core::disjunction<core::is_same<U, T>,
          core::is_same<U, remove_write_only_t<T>>,
          core::is_same<U, core::remove_const_t<T>>>::value>>
  inline contiguous_range& operator=(array<U, N>& _array) {
    contiguous_range(_array).swap(*this);

    return *this;
  }

  template <typename U, const size_t N,
      typename = core::enable_if_t<core::disjunction<core::is_same<U, T>,
          core::is_same<core::remove_all_extents_t<U>, remove_write_only_t<T>>,
          core::is_same<core::remove_all_extents_t<U>,
              core::remove_const_t<T>>>::value>>
  inline contiguous_range& operator=(U (&_ptr)[N]) {
    contiguous_range(_ptr, N).swap(*this);

    return *this;
  }

  template <typename U,
      typename = core::enable_if_t<core::disjunction<core::is_same<U, T>,
          core::is_same<U, remove_write_only_t<T>>,
          core::is_same<U, core::remove_const_t<T>>>::value>>
  inline contiguous_range& operator=(contiguous_range<U>&& _other) {
    contiguous_range(core::move(_other)).swap(*this);

    return *this;
  }

  template <typename U,
      typename = core::enable_if_t<core::disjunction<core::is_same<U, T>,
          core::is_same<U, remove_write_only_t<T>>,
          core::is_same<U, core::remove_const_t<T>>>::value>>
  inline contiguous_range& operator=(const contiguous_range<U>& _other) {
    contiguous_range(_other).swap(*this);

    return *this;
  }

  inline contiguous_range& operator=(contiguous_range&& _other) {
    contiguous_range(core::move(_other)).swap(*this);

    return *this;
  }

  inline contiguous_range& operator=(const contiguous_range& _other) {
    contiguous_range(_other).swap(*this);

    return *this;
  }

  // element access

  inline reference operator[](const size_type _pos) {
    return at(_pos);
  }

  inline const_reference operator[](const size_type _pos) const {
    return at(_pos);
  }

  inline reference at(const size_type _pos) {
    WN_DEBUG_ASSERT(m_begin, "invalid contiguous range");
    WN_DEBUG_ASSERT((m_begin + _pos) < m_end, "index out of bounds");

    return (*(m_begin + _pos));
  }

  inline const_reference at(const size_type _pos) const {
    WN_DEBUG_ASSERT(m_begin, "invalid contiguous range");
    WN_DEBUG_ASSERT((m_begin + _pos) < m_end, "index out of bounds");

    return (*(m_begin + _pos));
  }

  inline reference front() {
    return at(0);
  }

  inline const_reference front() const {
    return at(0);
  }

  inline reference back() {
    return at(size() - 1);
  }

  inline const_reference back() const {
    return at(size() - 1);
  }

  inline pointer data() {
    return m_begin;
  }

  inline const_pointer data() const {
    return m_begin;
  }

  // iterators

  inline iterator begin() {
#ifdef _WN_DEBUG
    return iterator(this, m_begin);
#else
    return iterator(m_begin);
#endif
  }

  inline const_iterator begin() const {
    return cbegin();
  }

  inline const_iterator cbegin() const {
#ifdef _WN_DEBUG
    return const_iterator(this, m_begin);
#else
    return const_iterator(m_begin);
#endif
  }

  inline iterator end() {
#ifdef _WN_DEBUG
    return iterator(this, m_end);
#else
    return iterator(m_end);
#endif
  }

  inline const_iterator end() const {
    return cend();
  }

  inline const_iterator cend() const {
#ifdef _WN_DEBUG
    return const_iterator(this, m_end);
#else
    return const_iterator(m_end);
#endif
  }

  inline reverse_iterator rbegin() {
    return reverse_iterator(end());
  }

  inline const_reverse_iterator rbegin() const {
    return crbegin();
  }

  inline const_reverse_iterator crbegin() const {
    return const_reverse_iterator(cend());
  }

  inline reverse_iterator rend() {
    return reverse_iterator(begin());
  }

  inline const_reverse_iterator rend() const {
    return crend();
  }

  inline const_reverse_iterator crend() const {
    return const_reverse_iterator(cbegin());
  }

  // capacity

  inline size_type size() const {
    return (m_end - m_begin);
  }

  inline size_type max_size() const {
    return size();
  }

  inline bool empty() const {
    return (size() == 0);
  }

  // modifiers

  inline void clear() {
    contiguous_range().swap(*this);
  }

  inline void remove_prefix(const size_type _count) {
    WN_DEBUG_ASSERT(m_begin, "invalid contiguous range");
    WN_DEBUG_ASSERT((m_begin + _count) <= m_end, "count too large");

    m_begin += _count;
  }

  inline void remove_suffix(const size_type _count) {
    WN_DEBUG_ASSERT(m_begin, "invalid contiguous range");
    WN_DEBUG_ASSERT((m_end - _count) >= m_begin, "count too large");

    m_end -= _count;
  }

  inline void swap(contiguous_range& _other) {
    if (&_other != this) {
      core::swap(m_begin, _other.m_begin);
      core::swap(m_end, _other.m_end);
    }
  }

private:
  pointer m_begin;
  pointer m_end;
};

namespace internal {

template <typename T>
struct is_contiguous_range : core::false_type {};

template <typename T>
struct is_contiguous_range<contiguous_range<T>> : core::true_type {};

}  // namespace internal

template <typename T>
using is_contiguous_range = internal::is_contiguous_range<core::decay_t<T>>;

template <typename T, typename U>
inline bool operator==(
    const contiguous_range<T>& _lhs, const contiguous_range<U>& _rhs) {
  return (_lhs.data() == _rhs.data() &&
          (_lhs.data() + _lhs.size()) == (_rhs.data() + _rhs.size()));
}

template <typename T, typename U>
inline bool operator!=(
    const contiguous_range<T>& _lhs, const contiguous_range<U>& _rhs) {
  return (_lhs.data() != _rhs.data() ||
          (_lhs.data() + _lhs.size()) != (_rhs.data() + _rhs.size()));
}

template <typename T, typename U>
inline bool operator<(
    const contiguous_range<T>& _lhs, const contiguous_range<U>& _rhs) {
  return (_lhs.data() < _rhs.data() ||
          (_lhs.data() == _rhs.data() &&
              (_lhs.data() + _lhs.size()) < (_rhs.data() + _rhs.size())));
}

template <typename T, typename U>
inline bool operator<=(
    const contiguous_range<T>& _lhs, const contiguous_range<U>& _rhs) {
  return (_lhs.data() < _rhs.data() ||
          (_lhs.data() == _rhs.data() &&
              (_lhs.data() + _lhs.size()) <= (_rhs.data() + _rhs.size())));
}

template <typename T, typename U>
inline bool operator>(
    const contiguous_range<T>& _lhs, const contiguous_range<U>& _rhs) {
  return (_lhs.data() > _rhs.data() ||
          (_lhs.data() == _rhs.data() &&
              (_lhs.data() + _lhs.size()) > (_rhs.data() + _rhs.size())));
}

template <typename T, typename U>
inline bool operator>=(
    const contiguous_range<T>& _lhs, const contiguous_range<U>& _rhs) {
  return (_lhs.data() > _rhs.data() ||
          (_lhs.data() == _rhs.data() &&
              (_lhs.data() + _lhs.size()) >= (_rhs.data() + _rhs.size())));
}

template <typename T, typename U>
inline typename core::enable_if<
    core::conjunction<core::is_same_decayed<U, remove_write_only_t<T>>,
        core::is_trivial<U>>::value>::type
copy_to(contiguous_range<T>* _dest, const U* _src, const size_t _offset,
    const size_t _count) {
  static_assert(!core::is_const<remove_write_only_t<T>>::value,
      "cannot copy to a contiguous range containing const values");
  WN_DEBUG_ASSERT(
      (_dest->data() + _offset + _count) <= (_dest->data() + _dest->size()),
      "attempting to copy outside of bounds of contiguous range");

  memory::memory_copy(
      reinterpret_cast<U*>(_dest->data() + _offset), _src, _count);
}

template <typename T, typename U>
inline typename core::enable_if<
    core::conjunction<core::is_convertible<U*, remove_write_only_t<T>*>,
        core::negation<core::is_trivial<U>>>::value>::type
copy_to(contiguous_range<T>* _dest, const U* _src, const size_t _offset,
    const size_t _count) {
  static_assert(!core::is_const<remove_write_only_t<T>>::value,
      "cannot copy to a contiguous range containing const values");
  WN_DEBUG_ASSERT(
      (_dest->data() + _offset + _count) <= (_dest->data() + _dest->size()),
      "attempting to copy outside the bounds of given contiguous range");

  T* ptr = _dest->data() + _offset;

  for (size_t i = 0; i < _count; ++i) {
    *(ptr + i) = _src[i];
  }
}

template <typename T, typename U>
inline typename core::enable_if<
    core::is_convertible<U*, remove_write_only_t<T>*>::value>::type
copy_to(contiguous_range<T>* _dest, const U* _src, const size_t _count) {
  copy_to(_dest, _src, 0, _count);
}

template <typename T, typename U>
inline typename core::enable_if<
    core::is_convertible<typename core::remove_const<U>::type*,
        remove_write_only_t<T>*>::value>::type
copy_to(contiguous_range<T>* _dest, const contiguous_range<U>& _src) {
  WN_DEBUG_ASSERT(_dest->size() <= _src.size(),
      "attempting to copy outside the bounds of given contiguous range");

  copy_to(_dest, _src.data(), _src.size());
}

template <typename T>
using read_only_contiguous_range = contiguous_range<core::add_const_t<T>>;

template <typename T>
using write_only_contiguous_range = contiguous_range<write_only<T>>;

template <typename T>
using is_read_only_contiguous_range =
    core::conditional_t<is_contiguous_range<T>::value,
        core::conditional_t<core::is_const<typename T::value_type>::value,
            core::true_type, core::false_type>,
        core::false_type>;

template <typename T>
using is_write_only_contiguous_range =
    core::conditional_t<is_contiguous_range<T>::value,
        core::conditional_t<is_write_only<typename T::value_type>::value,
            core::true_type, core::false_type>,
        core::false_type>;

}  // namespace containers
}  // namespace wn

#endif  // __WN_CONTAINERS_CONTIGUOUS_RANGE_H__
