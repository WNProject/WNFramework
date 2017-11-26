// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CONTAINERS_STRING_VIEW_H__
#define __WN_CONTAINERS_STRING_VIEW_H__

#include "WNContainers/inc/WNContiguousRange.h"
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

class string_view WN_FINAL {
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

  WN_FORCE_INLINE string_view(const nullptr_t, const nullptr_t)
    : string_view() {}

  WN_FORCE_INLINE string_view(const char* _begin, const char* _end)
    : m_range(_begin, _end) {}

  WN_FORCE_INLINE string_view(
      const char* _ptr, const size_t _offset, const size_type _size)
    : m_range(_ptr, _offset, _size) {}

  WN_FORCE_INLINE string_view(const char* _ptr, const size_type _size)
    : string_view(_ptr, 0, _size) {}

  WN_FORCE_INLINE string_view(const char* _ptr)
    : string_view(_ptr, memory::strlen(_ptr)) {}

  explicit string_view(const string& _string);

  string_view(const string& _string, const size_type _size);

  string_view(
      const string& _string, const size_type _offset, const size_type _size);

  template <typename U,
      typename = core::enable_if_t<std::is_convertible<U*, const char*>::value>>
  WN_FORCE_INLINE string_view(contiguous_range<U>&& _range)
    : m_range(core::move(_range)) {}

  template <typename U,
      typename = core::enable_if_t<std::is_convertible<U*, const char*>::value>>
  WN_FORCE_INLINE string_view(const contiguous_range<U>& _range)
    : m_range(_range) {}

  WN_FORCE_INLINE string_view(string_view&& _view)
    : string_view(core::move(_view.m_range)) {}

  WN_FORCE_INLINE string_view(const string_view& _view)
    : string_view(_view.m_range) {}

  WN_FORCE_INLINE string_view& operator=(const nullptr_t) {
    assign(nullptr);

    return *this;
  }

  WN_FORCE_INLINE string_view& operator=(const char* _ptr) {
    assign(_ptr);

    return *this;
  }

  string_view& operator=(const string& _string);

  template <typename U,
      typename = core::enable_if_t<std::is_convertible<U*, const char*>::value>>
  WN_FORCE_INLINE string_view& operator=(contiguous_range<U>&& _range) {
    string_view(core::move(_range)).swap(*this);

    return *this;
  }

  template <typename U,
      typename = core::enable_if_t<std::is_convertible<U*, const char*>::value>>
  WN_FORCE_INLINE string_view& operator=(const contiguous_range<U>& _range) {
    string_view(_range).swap(*this);

    return *this;
  }

  WN_FORCE_INLINE string_view& operator=(string_view&& _view) {
    assign(core::move(_view));

    return *this;
  }

  WN_FORCE_INLINE string_view& operator=(const string_view& _view) {
    assign(_view);

    return *this;
  }

  WN_FORCE_INLINE void assign(const nullptr_t) {
    string_view(nullptr).swap(*this);
  }

  WN_FORCE_INLINE void assign(string_view&& _view) {
    string_view(core::move(_view)).swap(*this);
  }

  WN_FORCE_INLINE void assign(const string_view& _view) {
    string_view(_view).swap(*this);
  }

  void assign(const string& _string);

  WN_FORCE_INLINE void assign(const value_type* _ptr) {
    string_view(_ptr).swap(*this);
  }

  string to_string(memory::allocator* _allocator) const;

  WN_FORCE_INLINE contiguous_range<const char> to_contiguous_range() const {
    return m_range;
  }

  // element access

  WN_FORCE_INLINE const_reference operator[](const size_type _pos) const {
    return at(_pos);
  }

  WN_FORCE_INLINE const_reference at(const size_type _pos) const {
    return m_range.at(_pos);
  }

  WN_FORCE_INLINE const_reference front() const {
    return at(0);
  }

  WN_FORCE_INLINE const_reference back() const {
    return at(size() - 1);
  }

  WN_FORCE_INLINE const_pointer data() const {
    return m_range.data();
  }

  // iterators

  WN_FORCE_INLINE const_iterator begin() const {
    return cbegin();
  }

  WN_FORCE_INLINE const_iterator cbegin() const {
    return m_range.cbegin();
  }

  WN_FORCE_INLINE const_iterator end() const {
    return cend();
  }

  WN_FORCE_INLINE const_iterator cend() const {
    return m_range.cend();
  }

  WN_FORCE_INLINE const_reverse_iterator rbegin() const {
    return crbegin();
  }

  WN_FORCE_INLINE const_reverse_iterator crbegin() const {
    return const_reverse_iterator(cend());
  }

  WN_FORCE_INLINE const_reverse_iterator rend() const {
    return crend();
  }

  WN_FORCE_INLINE const_reverse_iterator crend() const {
    return const_reverse_iterator(begin());
  }

  // capacity

  WN_FORCE_INLINE bool empty() const {
    return m_range.empty();
  }

  WN_FORCE_INLINE size_type size() const {
    return m_range.size();
  }

  WN_FORCE_INLINE size_type length() const {
    return size();
  }

  WN_FORCE_INLINE size_type capacity() const {
    return size();
  }

  WN_FORCE_INLINE size_type max_size() const {
    return std::numeric_limits<size_type>::max();
  }

  // operations

  WN_FORCE_INLINE void clear() {
    m_range.clear();
  }

  WN_FORCE_INLINE int32_t compare(const string_view& _view) const {
    const size_type size1 = size();
    const size_type size2 = _view.size();

    if (size1 != 0 && size2 != 0) {
      const size_type min_size = size1 < size2 ? size1 : size2;
      const int32_t r = memory::strncmp(data(), _view.data(), min_size);

      if (r == 0) {
        return (size1 == size2 ? 0 : (size1 < size2 ? -1 : 1));
      }

      return r;
    } else {
      return (size1 == size2 ? 0 : (size1 < size2 ? -1 : 1));
    }
  }

  WN_FORCE_INLINE int32_t compare(const size_type _pos, const size_type _count,
      const string_view& _view) const {
    return substr(_pos, _count).compare(_view);
  }

  WN_FORCE_INLINE int32_t compare(const size_type pos1, const size_type count1,
      const string_view& _view, const size_type pos2,
      const size_type count2) const {
    return substr(pos1, count1).compare(_view.substr(pos2, count2));
  }

  WN_FORCE_INLINE int32_t compare(const char* _ptr) const {
    return compare(string_view(_ptr));
  }

  WN_FORCE_INLINE int32_t compare(
      const size_type pos1, const size_type count1, const char* _ptr) const {
    return substr(pos1, count1).compare(string_view(_ptr));
  }

  WN_FORCE_INLINE int32_t compare(const size_type pos1, const size_type count1,
      const char* _ptr, const size_type count2) const {
    return substr(pos1, count1).compare(string_view(_ptr, count2));
  }

  WN_FORCE_INLINE void remove_prefix(const size_type _count) {
    m_range.remove_prefix(_count);
  }

  WN_FORCE_INLINE void remove_suffix(const size_type _count) {
    m_range.remove_suffix(_count);
  }

  WN_FORCE_INLINE string_view substr(
      const size_type _pos = 0, const size_type _count = npos) const {
    const size_type s = size() - _pos;

    return (string_view(data() + _pos, _count < s ? _count : s));
  }

  WN_FORCE_INLINE void swap(string_view& _other) {
    m_range.swap(_other.m_range);
  }

  // searching

  WN_FORCE_INLINE size_type find(
      const string_view& _view, const size_type _pos = 0) const {
    const size_type pos = math::min(_pos, size());
    auto i = std::search(cbegin() + pos, cend(), _view.cbegin(), _view.cend());

    return (i == cend() ? npos : (i - cbegin()));
  }

  WN_FORCE_INLINE size_type find(const char c, const size_type _pos = 0) const {
    return find(string_view(&c, 1), _pos);
  }

  WN_FORCE_INLINE size_type find(
      const char* _ptr, const size_type _pos, const size_type _count) const {
    return find(string_view(_ptr, _count), _pos);
  }

  WN_FORCE_INLINE size_type find(const char* _ptr, size_type _pos = 0) const {
    return find(string_view(_ptr), _pos);
  }

  WN_FORCE_INLINE size_type rfind(
      const string_view& _view, const size_type _pos = npos) const {
    const size_type pos = math::min(_pos, size());
    const auto i =
        std::search(crend() - pos, crend(), _view.crbegin(), _view.crend());

    return (i == crend() ? npos : (i.base() - cbegin() - 1));
  }

  WN_FORCE_INLINE size_type rfind(
      const char c, const size_type _pos = npos) const {
    return rfind(string_view(&c, 1), _pos);
  }

  WN_FORCE_INLINE size_type rfind(
      const char* _ptr, const size_type _pos, const size_type _count) const {
    return rfind(string_view(_ptr, _count), _pos);
  }

  WN_FORCE_INLINE size_type rfind(
      const char* _ptr, size_type _pos = npos) const {
    return rfind(string_view(_ptr), _pos);
  }

  WN_FORCE_INLINE size_type find_first_of(
      const string_view& _view, const size_type _pos = 0) const {
    const size_type pos = math::min(_pos, size());
    const auto i = std::find_first_of(
        cbegin() + pos, cend(), _view.cbegin(), _view.cend());

    return (i == cend() ? npos : (i - cbegin()));
  }

  WN_FORCE_INLINE size_type find_first_of(
      const char c, const size_type _pos = 0) const {
    return find_first_of(string_view(&c, 1), _pos);
  }

  WN_FORCE_INLINE size_type find_first_of(
      const char* _ptr, const size_type _pos, const size_type _count) const {
    return find_first_of(string_view(_ptr, _count), _pos);
  }

  WN_FORCE_INLINE size_type find_first_of(
      const char* _ptr, const size_type _pos = 0) const {
    return find_first_of(string_view(_ptr), _pos);
  }

  WN_FORCE_INLINE size_type find_last_of(
      const string_view& _view, const size_type _pos = npos) const {
    const size_type pos = math::min(_pos, size());
    const auto i = std::find_first_of(
        crend() - pos, crend(), _view.cbegin(), _view.cend());

    return (i == crend() ? npos : (i.base() - cbegin() - 1));
  }

  WN_FORCE_INLINE size_type find_last_of(
      const char c, const size_type _pos = npos) const {
    return find_last_of(string_view(&c, 1), _pos);
  }

  WN_FORCE_INLINE size_type find_last_of(
      const char* _ptr, const size_type _pos, const size_type _count) const {
    return find_last_of(string_view(_ptr, _count), _pos);
  }

  WN_FORCE_INLINE size_type find_last_of(
      const char* _ptr, const size_type _pos = npos) const {
    return find_last_of(string_view(_ptr), _pos);
  }

  WN_FORCE_INLINE size_type find_first_not_of(
      const string_view& _view, const size_type _pos = 0) const {
    const auto p = [&_view](const char c) -> bool {
      return (std::find(_view.cbegin(), _view.cend(), c) == _view.cend());
    };

    const auto i = std::find_if(cbegin() + _pos, cend(), p);

    return (i == cend() ? npos : (i - cbegin()));
  }

  WN_FORCE_INLINE size_type find_first_not_of(
      const char c, const size_type _pos = 0) const {
    return find_first_not_of(string_view(&c, 1), _pos);
  }

  WN_FORCE_INLINE size_type find_first_not_of(
      const char* _ptr, const size_type _pos, const size_type _count) const {
    return find_first_not_of(string_view(_ptr, _count), _pos);
  }

  WN_FORCE_INLINE size_type find_first_not_of(
      const char* _ptr, const size_type _pos = 0) const {
    return find_first_not_of(string_view(_ptr), _pos);
  }

  WN_FORCE_INLINE size_type find_last_not_of(
      const string_view& _view, const size_type _pos = 0) const {
    const auto p = [&_view](const char c) -> bool {
      return (std::find(_view.cbegin(), _view.cend(), c) == _view.cend());
    };

    const auto b = cbegin();
    const auto e = const_reverse_iterator(b + _pos);
    const auto i = std::find_if(crbegin(), e, p);

    return (i == e ? npos : static_cast<size_type>(i.base() - b - 1));
  }

  WN_FORCE_INLINE size_type find_last_not_of(
      const char c, const size_type _pos = 0) const {
    return find_last_not_of(string_view(&c, 1), _pos);
  }

  WN_FORCE_INLINE size_type find_last_not_of(
      const char* _ptr, const size_type _pos, const size_type _count) const {
    return find_last_not_of(string_view(_ptr, _count), _pos);
  }

  WN_FORCE_INLINE size_type find_last_not_of(
      const char* _ptr, const size_type _pos = 0) const {
    return find_last_not_of(string_view(_ptr), _pos);
  }

  // Trims all of the characters in the given string_view from the left
  // hand side.
  WN_FORCE_INLINE containers::string_view trim_left(
      const containers::string_view& _vals) const {
    containers::string_view other = *this;
    size_type x = other.find_first_not_of(_vals);
    if (x != npos) {
      other.remove_prefix(x);
    } else {
      other = "";
    }
    return other;
  }

  // Trims all of the characters in the given string_view from the right
  // hand side.
  WN_FORCE_INLINE containers::string_view trim_right(
      const containers::string_view& _vals) const {
    containers::string_view other = *this;
    size_type x = other.find_last_not_of(_vals);
    if (x != npos) {
      other.remove_suffix(size() - x - 1);
    } else {
      return "";
    }
    return other;
  }

  WN_FORCE_INLINE containers::dynamic_array<containers::string_view> split(
      memory::allocator* _allocator, const char _splitter,
      const bool _leave_empty = false) const {
    return split(
        _allocator, containers::string_view(&_splitter, 1), _leave_empty);
  }

  WN_FORCE_INLINE containers::dynamic_array<containers::string_view> split(
      memory::allocator* _allocator, const string_view& _splitters,
      const bool _leave_empty = false) const {
    containers::dynamic_array<containers::string_view> return_split(_allocator);
    if (empty()) {
      return core::move(return_split);
    }
    size_t b = 0;
    while (b != size()) {
      size_t n = find_first_of(_splitters, b);
      if (n == npos) {
        size_t s = size() - b;
        if (s == 1 && _splitters.find(at(b)) != npos) {
          if (_leave_empty) {
            return_split.push_back("");
          }
          return core::move(return_split);
        }
        return_split.push_back(substr(b, s));
        return core::move(return_split);
      }
      if (n - b == 0 && _splitters.find(at(b)) != npos) {
        if (_leave_empty) {
          return_split.push_back("");
        }
        b = n + 1;
        continue;
      }
      return_split.push_back(substr(b, n - b));
      b = n + 1;
    }
    return core::move(return_split);
  }

private:
  range_type m_range;
};

WN_FORCE_INLINE bool operator==(
    const string_view& _lhs, const string_view& _rhs) {
  return (_lhs.compare(_rhs) == 0);
}

WN_FORCE_INLINE bool operator==(const string_view& _lhs, const char* _rhs) {
  return (_lhs.compare(_rhs) == 0);
}

WN_FORCE_INLINE bool operator==(const char* _lhs, const string_view& _rhs) {
  return (_rhs.compare(_lhs) == 0);
}

WN_FORCE_INLINE bool operator!=(
    const string_view& _lhs, const string_view& _rhs) {
  return (_lhs.compare(_rhs) != 0);
}

WN_FORCE_INLINE bool operator!=(const string_view& _lhs, const char* _rhs) {
  return (_lhs.compare(_rhs) != 0);
}

WN_FORCE_INLINE bool operator!=(const char* _lhs, const string_view& _rhs) {
  return (_rhs.compare(_lhs) != 0);
}

WN_FORCE_INLINE bool operator<(
    const string_view& _lhs, const string_view& _rhs) {
  return (_lhs.compare(_rhs) < 0);
}

WN_FORCE_INLINE bool operator<(const string_view& _lhs, const char* _rhs) {
  return (_lhs.compare(_rhs) < 0);
}

WN_FORCE_INLINE bool operator<(const char* _lhs, const string_view& _rhs) {
  return (_rhs.compare(_lhs) > 0);
}

WN_FORCE_INLINE bool operator<=(
    const string_view& _lhs, const string_view& _rhs) {
  return (_lhs.compare(_rhs) <= 0);
}

WN_FORCE_INLINE bool operator<=(const string_view& _lhs, const char* _rhs) {
  return (_lhs.compare(_rhs) <= 0);
}

WN_FORCE_INLINE bool operator<=(const char* _lhs, const string_view& _rhs) {
  return (_rhs.compare(_lhs) >= 0);
}

WN_FORCE_INLINE bool operator>(
    const string_view& _lhs, const string_view& _rhs) {
  return (_lhs.compare(_rhs) > 0);
}

WN_FORCE_INLINE bool operator>(const string_view& _lhs, const char* _rhs) {
  return (_lhs.compare(_rhs) > 0);
}

WN_FORCE_INLINE bool operator>(const char* _lhs, const string_view& _rhs) {
  return (_rhs.compare(_lhs) < 0);
}

WN_FORCE_INLINE bool operator>=(
    const string_view& _lhs, const string_view& _rhs) {
  return (_lhs.compare(_rhs) >= 0);
}

WN_FORCE_INLINE bool operator>=(const string_view& _lhs, const char* _rhs) {
  return (_lhs.compare(_rhs) >= 0);
}

WN_FORCE_INLINE bool operator>=(const char* _lhs, const string_view& _rhs) {
  return (_rhs.compare(_lhs) <= 0);
}

}  // namespace containers
}  // namespace wn

// string related defintions

#include "WNContainers/inc/WNString.h"

namespace wn {
namespace containers {

WN_FORCE_INLINE string_view::string_view(const string& _string)
  : string_view(_string.data(), _string.size()) {}

WN_FORCE_INLINE string_view& string_view::operator=(const string& _string) {
  assign(_string);

  return *this;
}

WN_FORCE_INLINE void string_view::assign(const string& _string) {
  string_view(_string).swap(*this);
}

WN_FORCE_INLINE string string_view::to_string(
    memory::allocator* _allocator) const {
  return string(_allocator, data(), size());
}

WN_FORCE_INLINE string_view::string_view(
    const string& _string, const size_type _size)
  : string_view(_string.data(), _size) {
  WN_DEBUG_ASSERT_DESC(
      _size <= _string.size(), "attempting to access outside of string bounds");
}

WN_FORCE_INLINE string_view::string_view(
    const string& _string, const size_type _offset, const size_type _size)
  : string_view(_string.data(), _offset, _size) {
  WN_DEBUG_ASSERT_DESC((_offset + _size) <= _string.size(),
      "attempting to access outside of string bounds");
}

}  // namespace containers
}  // namespace wn

namespace std {

template <>
struct hash<wn::containers::string_view> {
  WN_FORCE_INLINE size_t operator()(
      const wn::containers::string_view& _view) const {
    return wn::memory::strnhash(_view.data(), _view.size());
  }
};

template <>
struct equal_to<wn::containers::string_view> {
  WN_FORCE_INLINE size_t operator()(const wn::containers::string_view& _lhs,
      const wn::containers::string_view& _rhs) const {
    return (_lhs == _rhs);
  }
};

}  // namespace std

#endif  // __WN_CONTAINERS_STRING_VIEW_H__