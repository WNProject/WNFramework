// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_CONTIGUOUS_RANGE_H__
#define __WN_CONTAINERS_CONTIGUOUS_RANGE_H__

#include "WNCore/inc/WNUtility.h"
#include "WNCore/inc/WNAssert.h"

namespace wn {
namespace containers {

template <typename T>
class contiguous_range final {
public:
  typedef T value_type;
  typedef wn_size_t size_type;
  typedef wn_signed_t difference_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;
  typedef pointer iterator;
  typedef const_pointer const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  WN_FORCE_INLINE contiguous_range() :
    m_begin(wn_nullptr),
    m_end(wn_nullptr) {
  }

  WN_FORCE_INLINE contiguous_range(const wn_nullptr_t) :
    contiguous_range() {
  }

  WN_FORCE_INLINE explicit contiguous_range(const wn_nullptr_t,
                                            const wn_nullptr_t) :
    contiguous_range() {
  }

  WN_FORCE_INLINE explicit contiguous_range(T* begin, T* end) :
    m_begin(begin),
    m_end(end) {
    WN_RELEASE_ASSERT_DESC((m_begin && m_end) || (!m_begin && !m_end),
      "invalid input parameters, both must be null or non-null");
  }

  WN_FORCE_INLINE explicit contiguous_range(T* ptr, const size_type size) :
    contiguous_range(ptr, ptr + size) {
  }

  template <typename U, const wn_size_t N,
            typename = core::enable_if_t<std::is_convertible<U*, T*>::value>>
  WN_FORCE_INLINE contiguous_range(U(&ptr)[N]) :
    contiguous_range(ptr, N) {
  }

  template <typename U,
            typename = core::enable_if_t<std::is_convertible<U*, T*>::value>>
  WN_FORCE_INLINE contiguous_range(contiguous_range<U>&& range) :
    contiguous_range(range.data(), range.size()) {
    range = wn_nullptr;
  }

  template <typename U,
            typename = core::enable_if_t<std::is_convertible<U*, T*>::value>>
  WN_FORCE_INLINE contiguous_range(const contiguous_range<U>& range) :
    contiguous_range(range.data(), range.size()) {
  }

  WN_FORCE_INLINE contiguous_range(contiguous_range&& range) :
    contiguous_range(range.m_begin, range.m_end) {
    range = wn_nullptr;
  }

  WN_FORCE_INLINE contiguous_range(const contiguous_range& range) :
    contiguous_range(range.m_begin, range.m_end) {
  }

  WN_FORCE_INLINE contiguous_range& operator = (const wn_nullptr_t) {
    contiguous_range(wn_nullptr).swap(*this);

    return(*this);
  }

  template <typename U, const wn_size_t N,
            typename = core::enable_if_t<std::is_convertible<U*, T*>::value>>
  WN_FORCE_INLINE contiguous_range& operator = (U(&ptr)[N]) {
    contiguous_range(ptr, N).swap(*this);

    return(*this);
  }

  template <typename U,
            typename = core::enable_if_t<std::is_convertible<U*, T*>::value>>
  WN_FORCE_INLINE contiguous_range &operator = (contiguous_range<U>&& range) {
    contiguous_range(std::move(range)).swap(*this);

    return(*this);
  }

  template <typename U,
            typename = core::enable_if_t<std::is_convertible<U*, T*>::value>>
  WN_FORCE_INLINE contiguous_range& operator = (const contiguous_range<U>& range) {
    contiguous_range(range).swap(*this);

    return(*this);
  }

  WN_FORCE_INLINE contiguous_range &operator = (contiguous_range&& range) {
    contiguous_range(std::move(range)).swap(*this);

    return(*this);
  }

  WN_FORCE_INLINE contiguous_range& operator = (const contiguous_range& range) {
    contiguous_range(range).swap(*this);

    return(*this);
  }

  // element access

  WN_FORCE_INLINE reference operator [] (const size_type pos) {
    return(at(pos));
  }

  WN_FORCE_INLINE const_reference operator [] (const size_type pos) const {
    return(at(pos));
  }

  WN_FORCE_INLINE reference at(const size_type pos) {
    WN_RELEASE_ASSERT_DESC(m_begin, "invalid string_view");
    WN_RELEASE_ASSERT_DESC((m_begin + pos) < m_end, "index out of bounds");

    return(*(m_begin + pos));
  }

  WN_FORCE_INLINE const_reference at(const size_type pos) const {
    WN_RELEASE_ASSERT_DESC(m_begin, "invalid string_view");
    WN_RELEASE_ASSERT_DESC((m_begin + pos) < m_end, "index out of bounds");

    return(*(m_begin + pos));
  }

  WN_FORCE_INLINE reference front() {
    return(at(0));
  }

  WN_FORCE_INLINE const_reference front() const {
    return(at(0));
  }

  WN_FORCE_INLINE reference back() {
    return(at(size() - 1));
  }

  WN_FORCE_INLINE const_reference back() const {
    return(at(size() - 1));
  }

  WN_FORCE_INLINE pointer data() {
    return(m_begin);
  }

  WN_FORCE_INLINE const_pointer data() const {
    return(m_begin);
  }

  // iterators

  WN_FORCE_INLINE iterator begin() {
    return(m_begin);
  }

  WN_FORCE_INLINE const_iterator begin() const {
    return(cbegin());
  }

  WN_FORCE_INLINE const_iterator cbegin() const {
    return(m_begin);
  }

  WN_FORCE_INLINE iterator end() {
    return(m_end);
  }

  WN_FORCE_INLINE const_iterator end() const {
    return(cend());
  }

  WN_FORCE_INLINE const_iterator cend() const {
    return(m_end);
  }

  WN_FORCE_INLINE reverse_iterator rbegin() {
    return(reverse_iterator(end()));
  }

  WN_FORCE_INLINE const_reverse_iterator rbegin() const {
    return(crbegin());
  }

  WN_FORCE_INLINE const_reverse_iterator crbegin() const {
    return(const_reverse_iterator(cend()));
  }

  WN_FORCE_INLINE reverse_iterator rend() {
    return(reverse_iterator(begin()));
  }

  WN_FORCE_INLINE const_reverse_iterator rend() const {
    return(crend());
  }

  WN_FORCE_INLINE const_reverse_iterator crend() const {
    return(const_reverse_iterator(begin()));
  }

  // capacity

  WN_FORCE_INLINE size_type size() const {
    return(m_end - m_begin);
  }

  WN_FORCE_INLINE size_type max_size() const {
    return(size());
  }

  WN_FORCE_INLINE wn_bool empty() const {
    return(size() == 0);
  }

  // modifiers

  WN_FORCE_INLINE wn_void remove_prefix(const size_type count) {
    WN_RELEASE_ASSERT_DESC(m_begin, "invalid string_view");
    WN_RELEASE_ASSERT_DESC((m_begin + count) < m_end, "count too large");

    m_begin += count;
  }

  WN_FORCE_INLINE wn_void remove_suffix(const size_type count) {
    WN_RELEASE_ASSERT_DESC(m_begin, "invalid string_view");
    WN_RELEASE_ASSERT_DESC((m_end - count) >= m_begin, "count too large");

    m_end -= count;
  }

  WN_FORCE_INLINE wn_void swap(contiguous_range& range) {
    std::swap(m_begin, range.m_begin);
    std::swap(m_end, range.m_end);
  }

private:
  pointer m_begin;
  pointer m_end;
};

template <typename T, typename U>
WN_FORCE_INLINE wn_bool operator == (const contiguous_range<T>& lhs,
                                     const contiguous_range<U>& rhs) {
  return(lhs.data() == rhs.data() &&
         (lhs.data() + lhs.size()) == (rhs.data() + rhs.size()));
}

template <typename T, typename U>
WN_FORCE_INLINE wn_bool operator != (const contiguous_range<T>& lhs,
                                     const contiguous_range<U>& rhs) {
  return(lhs.data() != rhs.data() ||
         (lhs.data() + lhs.size()) != (rhs.data() + rhs.size()));}

template <typename T, typename U>
WN_FORCE_INLINE wn_bool operator < (const contiguous_range<T>& lhs,
                                    const contiguous_range<U>& rhs) {
  return(lhs.data() < rhs.data() ||
         (lhs.data() == rhs.data() &&
          (lhs.data() + lhs.size()) < (rhs.data() + rhs.size())));
}

template <typename T, typename U>
WN_FORCE_INLINE wn_bool operator <= (const contiguous_range<T>& lhs,
                                     const contiguous_range<U>& rhs) {
  return(lhs.data() < rhs.data() ||
         (lhs.data() == rhs.data() &&
          (lhs.data() + lhs.size()) <= (rhs.data() + rhs.size())));
}

template <typename T, typename U>
WN_FORCE_INLINE wn_bool operator > (const contiguous_range<T>& lhs,
                                    const contiguous_range<U>& rhs) {
  return(lhs.data() > rhs.data() ||
         (lhs.data() == rhs.data() &&
          (lhs.data() + lhs.size()) > (rhs.data() + rhs.size())));
}

template <typename T, typename U>
WN_FORCE_INLINE wn_bool operator >= (const contiguous_range<T>& lhs,
                                     const contiguous_range<U>& rhs) {
  return(lhs.data() > rhs.data() ||
         (lhs.data() == rhs.data() &&
          (lhs.data() + lhs.size()) >= (rhs.data() + rhs.size())));
}

} // namespace containers
} // namespace wn

#endif // __WN_CONTAINERS_CONTIGUOUS_RANGE_H__