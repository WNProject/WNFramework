// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_STRING_VIEW_H__
#define __WN_CONTAINERS_STRING_VIEW_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNContainers/inc/WNString.h"
#include "WNMemory/inc/WNStringUtility.h"
#include "WNCore/inc/WNAlgorithm.h"

namespace wn {
namespace containers {

class string_view final {
private:
  typedef contiguous_range<const wn_char> range_type;

public:
  typedef range_type::value_type value_type;
  typedef range_type::size_type size_type;
  typedef range_type::difference_type difference_type;
  typedef range_type::pointer pointer;
  typedef range_type::const_pointer const_pointer;
  typedef range_type::reference reference;
  typedef range_type::const_reference const_reference;
  typedef range_type::iterator iterator;
  typedef range_type::const_iterator const_iterator;
  typedef range_type::reverse_iterator reverse_iterator;
  typedef range_type::const_reverse_iterator const_reverse_iterator;

  enum : size_type {
    npos = static_cast<size_type>(-1)
  };

  string_view() = default;

  WN_FORCE_INLINE string_view(const wn_nullptr_t) :
    string_view() {
  }

  WN_FORCE_INLINE explicit string_view(const wn_nullptr_t,
                                       const wn_nullptr_t) :
    string_view() {
  }

  WN_FORCE_INLINE explicit string_view(const wn_char* begin,
                                       const wn_char* end) :
    m_range(begin, end) {
  }

  WN_FORCE_INLINE explicit string_view(const wn_char* s,
                                       const size_type size) :
    m_range(s, size) {
  }

  WN_FORCE_INLINE string_view(const wn_char* s) :
    string_view(s, memory::strlen(s)) {
  }

  WN_FORCE_INLINE string_view(const string& str) :
    string_view(str.data(), str.length()) {
  }

  template <typename U,
            typename = core::enable_if_t<
              std::is_convertible<U*, const wn_char*>::value>>
  WN_FORCE_INLINE string_view(contiguous_range<U>&& range) :
    m_range(std::move(range)) {
    range = wn_nullptr;
  }

  template <typename U,
            typename = core::enable_if_t<
              std::is_convertible<U*, const wn_char*>::value>>
  WN_FORCE_INLINE string_view(const contiguous_range<U>& range) :
    m_range(range) {
  }

  WN_FORCE_INLINE string_view(string_view&& view) :
    string_view(std::move(view.m_range)) {
  }

  WN_FORCE_INLINE string_view(const string_view& view) :
    string_view(view.m_range) {
  }

  WN_FORCE_INLINE string_view& operator = (const wn_nullptr_t) {
    string_view(wn_nullptr).swap(*this);

    return(*this);
  }

  WN_FORCE_INLINE string_view& operator = (const wn_char* s) {
    string_view(s).swap(*this);

    return(*this);
  }

  WN_FORCE_INLINE string_view& operator = (const string& str) {
    string_view(str).swap(*this);

    return(*this);
  }

  template <typename U,
            typename = core::enable_if_t<
              std::is_convertible<U*, const wn_char*>::value>>
  WN_FORCE_INLINE string_view& operator = (contiguous_range<U>&& range) {
    string_view(std::move(range)).swap(*this);

    return(*this);
  }

  template <typename U,
            typename = core::enable_if_t<
              std::is_convertible<U*, const wn_char*>::value>>
  WN_FORCE_INLINE string_view& operator = (const contiguous_range<U>& range) {
    string_view(range).swap(*this);

    return(*this);
  }

  WN_FORCE_INLINE string_view& operator = (string_view&& view) {
    string_view(std::move(view)).swap(*this);

    return(*this);
  }

  WN_FORCE_INLINE string_view& operator = (const string_view& view) {
    string_view(view).swap(*this);

    return(*this);
  }

  // element access

  WN_FORCE_INLINE const_reference operator [] (const size_type pos) const {
    return(at(pos));
  }

  WN_FORCE_INLINE const_reference at(const size_type pos) const {
    return(m_range.at(pos));
  }

  WN_FORCE_INLINE const_reference front() const {
    return(at(0));
  }

  WN_FORCE_INLINE const_reference back() const {
    return(at(size() - 1));
  }

  WN_FORCE_INLINE const_pointer data() const {
    return(m_range.data());
  }

  // iterators

  WN_FORCE_INLINE const_iterator begin() const {
    return(cbegin());
  }

  WN_FORCE_INLINE const_iterator cbegin() const {
    return(m_range.cbegin());
  }

  WN_FORCE_INLINE const_iterator end() const {
    return(cend());
  }

  WN_FORCE_INLINE const_iterator cend() const {
    return(m_range.cend());
  }

  WN_FORCE_INLINE const_reverse_iterator rbegin() const {
    return(crbegin());
  }

  WN_FORCE_INLINE const_reverse_iterator crbegin() const {
    return(const_reverse_iterator(cend()));
  }

  WN_FORCE_INLINE const_reverse_iterator rend() const {
    return(crend());
  }

  WN_FORCE_INLINE const_reverse_iterator crend() const {
    return(const_reverse_iterator(begin()));
  }

  // capacity

  WN_FORCE_INLINE size_type size() const {
    return(m_range.size());
  }

  WN_FORCE_INLINE size_type length() const {
    return(size());
  }

  WN_FORCE_INLINE size_type max_size() const {
    return(size());
  }

  WN_FORCE_INLINE wn_bool empty() const {
    return(m_range.empty());
  }

  // modifiers

  WN_FORCE_INLINE wn_void remove_prefix(const size_type count) {
    m_range.remove_prefix(count);
  }

  WN_FORCE_INLINE wn_void remove_suffix(const size_type count) {
    m_range.remove_suffix(count);
  }

  WN_FORCE_INLINE wn_void swap(string_view& view) {
    m_range.swap(view.m_range);
  }

  // operators

  WN_FORCE_INLINE string to_string() const {
    return(string(cbegin(), cend()));
  }

  WN_FORCE_INLINE contiguous_range<const wn_char> to_contiguous_range() const {
    return(m_range);
  }

  WN_FORCE_INLINE string_view substr(const size_type pos = 0,
                                     const size_type count = npos) const {
    const size_type s = size() - pos;

    return(string_view(data() + pos, count < s ? count : s));
  }

  WN_FORCE_INLINE wn_int32 compare(const string_view& view) const {
    const size_type size1 = size();
    const size_type size2 = view.size();
    const size_type min_size = size1 < size2 ? size1 : size2;
    const wn_int32 r = memory::strncmp(data(), view.data(), min_size);

    if (r == 0) {
      return(size1 == size2 ? 0 : (size1 < size2 ? -1 : 1));
    }

    return(r);
  }

  WN_FORCE_INLINE wn_int32 compare(const size_type pos, const size_type count,
                                   const string_view& view) const {
    return(substr(pos, count).compare(view));
  }

  WN_FORCE_INLINE wn_int32 compare(const size_type pos1, const size_type count1,
                                   const string_view& view, const size_type pos2,
                                   const size_type count2) const {
    return(substr(pos1, count1).compare(view.substr(pos2, count2)));
  }

  WN_FORCE_INLINE wn_int32 compare(const wn_char* s) const {
    return(compare(string_view(s)));
  }

  WN_FORCE_INLINE wn_int32 compare(const size_type pos1, const size_type count1,
                                   const wn_char* s) const {
    return(substr(pos1, count1).compare(string_view(s)));
  }

  WN_FORCE_INLINE wn_int32 compare(const size_type pos1, const size_type count1,
                                   const wn_char* s,
                                   const size_type count2) const {
    return(substr(pos1, count1).compare(string_view(s, count2)));
  }

  WN_FORCE_INLINE size_type find(const string_view& view,
                                 const size_type pos = 0) const {
    auto i = std::search(cbegin() + pos, cend(), view.cbegin(), view.cend());

    return(i == cend() ? npos : static_cast<size_type>(i - cbegin()));
  }

  WN_FORCE_INLINE size_type find(const wn_char c,
                                 const size_type pos = 0) const {
    return(find(string_view(&c, 1), pos));
  }

  WN_FORCE_INLINE size_type find(const wn_char* s, const size_type pos,
                                 const size_type count) const {
    return(find(string_view(s, count), pos));
  }

  WN_FORCE_INLINE size_type find(const wn_char* s, size_type pos = 0) const {
    return(find(string_view(s), pos));
  }

  WN_FORCE_INLINE size_type rfind(const string_view& view,
                                  const size_type pos = 0) const {
    const auto b = cbegin();
    const auto e = const_reverse_iterator(b + pos);
    const auto i = std::search(crbegin(), e, view.crbegin(), view.crend());

    return(i == e ? npos : static_cast<size_type>(i.base() - view.size() - b));
  }

  WN_FORCE_INLINE size_type rfind(const wn_char c,
                                  const size_type pos = 0) const {
    return(rfind(string_view(&c, 1), pos));
  }

  WN_FORCE_INLINE size_type rfind(const wn_char* s, const size_type pos,
                                  const size_type count) const {
    return(rfind(string_view(s, count), pos));
  }

  WN_FORCE_INLINE size_type rfind(const wn_char* s, size_type pos = 0) const {
    return(rfind(string_view(s), pos));
  }

  WN_FORCE_INLINE size_type find_first_of(const string_view& view,
                                          const size_type pos = 0) const {
    const auto i = std::find_first_of(cbegin() + pos, cend(),
                                      view.cbegin(), view.cend());

    return(i == cend() ? npos : static_cast<size_type>(i - cbegin()));
  }


  WN_FORCE_INLINE size_type find_first_of(const wn_char c,
                                          const size_type pos = 0) const {
    return(find_first_of(string_view(&c, 1), pos));
  }

  WN_FORCE_INLINE size_type find_first_of(const wn_char *s, const size_type pos,
                                          const size_type count) const {
    return(find_first_of(string_view(s, count), pos));
  }

  WN_FORCE_INLINE size_type find_first_of(const wn_char* s,
                                          const size_type pos = 0) const {
    return(find_first_of(string_view(s), pos));
  }

  WN_FORCE_INLINE size_type find_last_of(const string_view& view,
                                         const size_type pos = 0) const {
    const auto b = cbegin();
    const auto e = const_reverse_iterator(b + pos);
    const auto i = std::find_first_of(crbegin(), e,
                                      view.cbegin(), view.cend());

    return(i == e ? npos : static_cast<size_type>(i.base() - b - 1));
  }


  WN_FORCE_INLINE size_type find_last_of(const wn_char c,
                                         const size_type pos = 0) const {
    return(find_last_of(string_view(&c, 1), pos));
  }

  WN_FORCE_INLINE size_type find_last_of(const wn_char* s,
                                         const size_type pos,
                                         const size_type count) const {
    return(find_last_of(string_view(s, count), pos));
  }

  WN_FORCE_INLINE size_type find_last_of(const wn_char* s,
                                         const size_type pos = 0) const {
    return(find_last_of(string_view(s), pos));
  }

  WN_FORCE_INLINE size_type find_first_not_of(const string_view& view,
                                              const size_type pos = 0) const {
    const auto p = [&view](const wn_char c) -> wn_bool {
      return(std::find(view.cbegin(), view.cend(), c) == view.cend());
    };
    const auto i = std::find_if(cbegin() + pos, cend(), p);

    return(i == cend() ? npos : static_cast<size_type>(i - cbegin()));
  }

  WN_FORCE_INLINE size_type find_first_not_of(const wn_char c,
                                              const size_type pos = 0) const {
    return(find_first_not_of(string_view(&c, 1), pos));
  }

  WN_FORCE_INLINE size_type find_first_not_of(const wn_char* s,
                                              const size_type pos,
                                              const size_type count) const {
    return(find_first_not_of(string_view(s, count), pos));
  }

  WN_FORCE_INLINE size_type find_first_not_of(const wn_char* s,
                                              const size_type pos = 0) const {
    return(find_first_not_of(string_view(s), pos));
  }

  WN_FORCE_INLINE size_type find_last_not_of(const string_view& view,
                                             const size_type pos = 0) const {
    const auto p = [&view](const wn_char c) -> wn_bool {
      return(std::find(view.cbegin(), view.cend(), c) == view.cend());
    };
    const auto b = cbegin();
    const auto e = const_reverse_iterator(b + pos);
    const auto i = std::find_if(crbegin(), e, p);

    return(i == e ? npos : static_cast<size_type>(i.base() - b - 1));
  }

  WN_FORCE_INLINE size_type find_last_not_of(const wn_char c,
                                             const size_type pos = 0) const {
    return(find_last_not_of(string_view(&c, 1), pos));
  }

  WN_FORCE_INLINE size_type find_last_not_of(const wn_char* s,
                                             const size_type pos,
                                             const size_type count) const {
    return(find_last_not_of(string_view(s, count), pos));
  }

  WN_FORCE_INLINE size_type find_last_not_of(const wn_char* s,
                                             const size_type pos = 0) const {
    return(find_last_not_of(string_view(s), pos));
  }

private:
  range_type m_range;
};

WN_FORCE_INLINE wn_bool operator == (const string_view& lhs,
                                     const string_view& rhs) {
  return(lhs.compare(rhs) == 0);
}

WN_FORCE_INLINE wn_bool operator != (const string_view& lhs,
                                     const string_view& rhs) {
  return(lhs.compare(rhs) != 0);
}

WN_FORCE_INLINE wn_bool operator < (const string_view& lhs,
                                    const string_view& rhs) {
  return(lhs.compare(rhs) < 0);
}

WN_FORCE_INLINE wn_bool operator <= (const string_view& lhs,
                                     const string_view& rhs) {
  return(lhs.compare(rhs) <= 0);
}

WN_FORCE_INLINE wn_bool operator > (const string_view& lhs,
                                    const string_view& rhs) {
  return(lhs.compare(rhs) > 0);
}

WN_FORCE_INLINE wn_bool operator >= (const string_view& lhs,
                                     const string_view& rhs) {
  return(lhs.compare(rhs) >= 0);
}

} // namespace containers
} // namespace wn

#endif // __WN_CONTAINERS_STRING_VIEW_H__