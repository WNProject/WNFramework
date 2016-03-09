// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CONTAINERS_STRING_VIEW_H__
#define __WN_CONTAINERS_STRING_VIEW_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNContainers/inc/WNString.h"
#include "WNCore/inc/WNAlgorithm.h"
#include "WNMemory/inc/WNStringUtility.h"

namespace wn {
namespace containers {

class string_view final {
private:
  using range_type = contiguous_range<const char>;

public:
  using value_type = range_type::value_type;
  using size_type = range_type::size_type;
  using difference_type = range_type::difference_type;
  using pointer = range_type::pointer;
  using const_pointer = range_type::const_pointer;
  using reference = range_type::reference;
  using const_reference = range_type::const_reference;
  using iterator = range_type::iterator;
  using const_iterator = range_type::const_iterator;
  using reverse_iterator = range_type::reverse_iterator;
  using const_reverse_iterator = range_type::const_reverse_iterator;

  enum : size_type { npos = static_cast<size_type>(-1) };

  string_view() = default;

  WN_FORCE_INLINE string_view(const nullptr_t) : string_view() {}

  WN_FORCE_INLINE explicit string_view(const nullptr_t, const nullptr_t)
    : string_view() {}

  WN_FORCE_INLINE explicit string_view(const char* begin, const char* end)
    : m_range(begin, end) {}

  WN_FORCE_INLINE explicit string_view(
      const char* s, const size_t offset, const size_type size)
    : m_range(s, offset, size) {}

  WN_FORCE_INLINE explicit string_view(const char* s, const size_type size)
    : string_view(s, 0, size) {}

  WN_FORCE_INLINE string_view(const char* s)
    : string_view(s, memory::strlen(s)) {}

  WN_FORCE_INLINE string_view(const string& str)
    : string_view(str.data(), str.length()) {}

  WN_FORCE_INLINE string_view(const string& str, const size_type size)
    : string_view(str.data(), size) {
    WN_DEBUG_ASSERT_DESC(
        size <= str.length(), "attempting to access outside of string bounds");
  }

  WN_FORCE_INLINE string_view(
      const string& str, const size_type offset, const size_type size)
    : string_view(str.data(), offset, size) {
    WN_DEBUG_ASSERT_DESC((offset + size) <= str.length(),
        "attempting to access outside of string bounds");
  }

  template <typename U,
      typename = core::enable_if_t<std::is_convertible<U*, const char*>::value>>
  WN_FORCE_INLINE string_view(contiguous_range<U>&& range)
    : m_range(std::move(range)) {}

  template <typename U,
      typename = core::enable_if_t<std::is_convertible<U*, const char*>::value>>
  WN_FORCE_INLINE string_view(const contiguous_range<U>& range)
    : m_range(range) {}

  WN_FORCE_INLINE string_view(string_view&& view)
    : string_view(std::move(view.m_range)) {}

  WN_FORCE_INLINE string_view(const string_view& view)
    : string_view(view.m_range) {}

  WN_FORCE_INLINE string_view& operator=(const nullptr_t) {
    string_view(nullptr).swap(*this);

    return (*this);
  }

  WN_FORCE_INLINE string_view& operator=(const char* s) {
    string_view(s).swap(*this);

    return (*this);
  }

  WN_FORCE_INLINE string_view& operator=(const string& str) {
    string_view(str).swap(*this);

    return (*this);
  }

  template <typename U,
      typename = core::enable_if_t<std::is_convertible<U*, const char*>::value>>
  WN_FORCE_INLINE string_view& operator=(contiguous_range<U>&& range) {
    string_view(std::move(range)).swap(*this);

    return (*this);
  }

  template <typename U,
      typename = core::enable_if_t<std::is_convertible<U*, const char*>::value>>
  WN_FORCE_INLINE string_view& operator=(const contiguous_range<U>& range) {
    string_view(range).swap(*this);

    return (*this);
  }

  WN_FORCE_INLINE string_view& operator=(string_view&& view) {
    string_view(std::move(view)).swap(*this);

    return (*this);
  }

  WN_FORCE_INLINE string_view& operator=(const string_view& view) {
    string_view(view).swap(*this);

    return (*this);
  }

  // element access

  WN_FORCE_INLINE const_reference operator[](const size_type pos) const {
    return (at(pos));
  }

  WN_FORCE_INLINE const_reference at(const size_type pos) const {
    return (m_range.at(pos));
  }

  WN_FORCE_INLINE const_reference front() const {
    return (at(0));
  }

  WN_FORCE_INLINE const_reference back() const {
    return (at(size() - 1));
  }

  WN_FORCE_INLINE const_pointer data() const {
    return (m_range.data());
  }

  // iterators

  WN_FORCE_INLINE const_iterator begin() const {
    return (cbegin());
  }

  WN_FORCE_INLINE const_iterator cbegin() const {
    return (m_range.cbegin());
  }

  WN_FORCE_INLINE const_iterator end() const {
    return (cend());
  }

  WN_FORCE_INLINE const_iterator cend() const {
    return (m_range.cend());
  }

  WN_FORCE_INLINE const_reverse_iterator rbegin() const {
    return (crbegin());
  }

  WN_FORCE_INLINE const_reverse_iterator crbegin() const {
    return (const_reverse_iterator(cend()));
  }

  WN_FORCE_INLINE const_reverse_iterator rend() const {
    return (crend());
  }

  WN_FORCE_INLINE const_reverse_iterator crend() const {
    return (const_reverse_iterator(begin()));
  }

  // capacity

  WN_FORCE_INLINE size_type size() const {
    return (m_range.size());
  }

  WN_FORCE_INLINE size_type length() const {
    return (size());
  }

  WN_FORCE_INLINE size_type max_size() const {
    return (size());
  }

  WN_FORCE_INLINE bool empty() const {
    return (m_range.empty());
  }

  // modifiers

  WN_FORCE_INLINE void clear() {
    m_range.clear();
  }

  WN_FORCE_INLINE void remove_prefix(const size_type count) {
    m_range.remove_prefix(count);
  }

  WN_FORCE_INLINE void remove_suffix(const size_type count) {
    m_range.remove_suffix(count);
  }

  WN_FORCE_INLINE void swap(string_view& view) {
    m_range.swap(view.m_range);
  }

  // operators

  WN_FORCE_INLINE string to_string() const {
    return (string(data(), size()));
  }

  WN_FORCE_INLINE string to_string(memory::allocator* _allocator) const {
    return (string(data(), size(), _allocator));
  }

  WN_FORCE_INLINE contiguous_range<const char> to_contiguous_range() const {
    return (m_range);
  }

  WN_FORCE_INLINE string_view substr(
      const size_type pos = 0, const size_type count = npos) const {
    const size_type s = size() - pos;

    return (string_view(data() + pos, count < s ? count : s));
  }

  WN_FORCE_INLINE int32_t compare(const string_view& view) const {
    const size_type size1 = size();
    const size_type size2 = view.size();

    if (size1 != 0 && size2 != 0) {
      const size_type min_size = size1 < size2 ? size1 : size2;
      const int32_t r = memory::strncmp(data(), view.data(), min_size);

      if (r == 0) {
        return (size1 == size2 ? 0 : (size1 < size2 ? -1 : 1));
      }

      return r;
    } else {
      return (size1 == size2 ? 0 : (size1 < size2 ? -1 : 1));
    }
  }

  WN_FORCE_INLINE int32_t compare(const size_type pos, const size_type count,
      const string_view& view) const {
    return (substr(pos, count).compare(view));
  }

  WN_FORCE_INLINE int32_t compare(const size_type pos1, const size_type count1,
      const string_view& view, const size_type pos2,
      const size_type count2) const {
    return (substr(pos1, count1).compare(view.substr(pos2, count2)));
  }

  WN_FORCE_INLINE int32_t compare(const char* s) const {
    return (compare(string_view(s)));
  }

  WN_FORCE_INLINE int32_t compare(
      const size_type pos1, const size_type count1, const char* s) const {
    return (substr(pos1, count1).compare(string_view(s)));
  }

  WN_FORCE_INLINE int32_t compare(const size_type pos1, const size_type count1,
      const char* s, const size_type count2) const {
    return (substr(pos1, count1).compare(string_view(s, count2)));
  }

  WN_FORCE_INLINE size_type find(
      const string_view& view, const size_type pos = 0) const {
    auto i = std::search(cbegin() + pos, cend(), view.cbegin(), view.cend());

    return (i == cend() ? npos : static_cast<size_type>(i - cbegin()));
  }

  WN_FORCE_INLINE size_type find(const char c, const size_type pos = 0) const {
    return (find(string_view(&c, 1), pos));
  }

  WN_FORCE_INLINE size_type find(
      const char* s, const size_type pos, const size_type count) const {
    return (find(string_view(s, count), pos));
  }

  WN_FORCE_INLINE size_type find(const char* s, size_type pos = 0) const {
    return (find(string_view(s), pos));
  }

  WN_FORCE_INLINE size_type rfind(
      const string_view& view, const size_type pos = 0) const {
    const auto b = cbegin();
    const auto e = const_reverse_iterator(b + pos);
    const auto i = std::search(crbegin(), e, view.crbegin(), view.crend());

    return (i == e ? npos : static_cast<size_type>(i.base() - view.size() - b));
  }

  WN_FORCE_INLINE size_type rfind(const char c, const size_type pos = 0) const {
    return (rfind(string_view(&c, 1), pos));
  }

  WN_FORCE_INLINE size_type rfind(
      const char* s, const size_type pos, const size_type count) const {
    return (rfind(string_view(s, count), pos));
  }

  WN_FORCE_INLINE size_type rfind(const char* s, size_type pos = 0) const {
    return (rfind(string_view(s), pos));
  }

  WN_FORCE_INLINE size_type find_first_of(
      const string_view& view, const size_type pos = 0) const {
    const auto i =
        std::find_first_of(cbegin() + pos, cend(), view.cbegin(), view.cend());

    return (i == cend() ? npos : static_cast<size_type>(i - cbegin()));
  }

  WN_FORCE_INLINE size_type find_first_of(
      const char c, const size_type pos = 0) const {
    return (find_first_of(string_view(&c, 1), pos));
  }

  WN_FORCE_INLINE size_type find_first_of(
      const char* s, const size_type pos, const size_type count) const {
    return (find_first_of(string_view(s, count), pos));
  }

  WN_FORCE_INLINE size_type find_first_of(
      const char* s, const size_type pos = 0) const {
    return (find_first_of(string_view(s), pos));
  }

  WN_FORCE_INLINE size_type find_last_of(
      const string_view& view, const size_type pos = 0) const {
    const auto b = cbegin();
    const auto e = const_reverse_iterator(b + pos);
    const auto i = std::find_first_of(crbegin(), e, view.cbegin(), view.cend());

    return (i == e ? npos : static_cast<size_type>(i.base() - b - 1));
  }

  WN_FORCE_INLINE size_type find_last_of(
      const char c, const size_type pos = 0) const {
    return (find_last_of(string_view(&c, 1), pos));
  }

  WN_FORCE_INLINE size_type find_last_of(
      const char* s, const size_type pos, const size_type count) const {
    return (find_last_of(string_view(s, count), pos));
  }

  WN_FORCE_INLINE size_type find_last_of(
      const char* s, const size_type pos = 0) const {
    return (find_last_of(string_view(s), pos));
  }

  WN_FORCE_INLINE size_type find_first_not_of(
      const string_view& view, const size_type pos = 0) const {
    const auto p = [&view](const char c) -> bool {
      return (std::find(view.cbegin(), view.cend(), c) == view.cend());
    };
    const auto i = std::find_if(cbegin() + pos, cend(), p);

    return (i == cend() ? npos : static_cast<size_type>(i - cbegin()));
  }

  WN_FORCE_INLINE size_type find_first_not_of(
      const char c, const size_type pos = 0) const {
    return (find_first_not_of(string_view(&c, 1), pos));
  }

  WN_FORCE_INLINE size_type find_first_not_of(
      const char* s, const size_type pos, const size_type count) const {
    return (find_first_not_of(string_view(s, count), pos));
  }

  WN_FORCE_INLINE size_type find_first_not_of(
      const char* s, const size_type pos = 0) const {
    return (find_first_not_of(string_view(s), pos));
  }

  WN_FORCE_INLINE size_type find_last_not_of(
      const string_view& view, const size_type pos = 0) const {
    const auto p = [&view](const char c) -> bool {
      return (std::find(view.cbegin(), view.cend(), c) == view.cend());
    };
    const auto b = cbegin();
    const auto e = const_reverse_iterator(b + pos);
    const auto i = std::find_if(crbegin(), e, p);

    return (i == e ? npos : static_cast<size_type>(i.base() - b - 1));
  }

  WN_FORCE_INLINE size_type find_last_not_of(
      const char c, const size_type pos = 0) const {
    return (find_last_not_of(string_view(&c, 1), pos));
  }

  WN_FORCE_INLINE size_type find_last_not_of(
      const char* s, const size_type pos, const size_type count) const {
    return (find_last_not_of(string_view(s, count), pos));
  }

  WN_FORCE_INLINE size_type find_last_not_of(
      const char* s, const size_type pos = 0) const {
    return (find_last_not_of(string_view(s), pos));
  }

private:
  range_type m_range;
};

WN_FORCE_INLINE bool operator==(
    const string_view& lhs, const string_view& rhs) {
  return (lhs.compare(rhs) == 0);
}

WN_FORCE_INLINE bool operator!=(
    const string_view& lhs, const string_view& rhs) {
  return (lhs.compare(rhs) != 0);
}

WN_FORCE_INLINE bool operator<(const string_view& lhs, const string_view& rhs) {
  return (lhs.compare(rhs) < 0);
}

WN_FORCE_INLINE bool operator<=(
    const string_view& lhs, const string_view& rhs) {
  return (lhs.compare(rhs) <= 0);
}

WN_FORCE_INLINE bool operator>(const string_view& lhs, const string_view& rhs) {
  return (lhs.compare(rhs) > 0);
}

WN_FORCE_INLINE bool operator>=(
    const string_view& lhs, const string_view& rhs) {
  return (lhs.compare(rhs) >= 0);
}

}  // namespace containers
}  // namespace wn

namespace std {

template <>
struct hash<wn::containers::string_view> {
  size_t operator()(const wn::containers::string_view& _view) const {
    return wn::memory::strnhash(_view.data(), _view.size());
  }
};

template <>
struct equal_to<wn::containers::string_view> {
  bool operator()(const wn::containers::string_view& _view1,
      const wn::containers::string_view& _view2) const {
    if (_view1.size() != _view2.size()) {
      return false;
    }

    if (_view1.empty()) {
      return true;
    }

    return (
        wn::memory::strncmp(_view1.data(), _view2.data(), _view1.size()) == 0);
  }
};

}  // namespace std

#endif  // __WN_CONTAINERS_STRING_VIEW_H__