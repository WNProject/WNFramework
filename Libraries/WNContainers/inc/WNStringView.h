// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_STRING_VIEW_H__
#define __WN_CONTAINERS_STRING_VIEW_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNContainers/inc/WNDynamicArray.h"
#include "WNMath/inc/WNBasic.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/string_utility.h"
#include "core/inc/types.h"

#include <algorithm>
#include <limits>

namespace wn {
namespace containers {

class string;

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

  string_view(nullptr_t) : string_view() {}

  string_view(nullptr_t, nullptr_t) : string_view() {}

  string_view(const char* _begin, const char* _end) : m_range(_begin, _end) {}

  string_view(const char* _ptr, const size_t _offset, const size_type _size)
    : m_range(_ptr, _offset, _size) {}

  string_view(const char* _ptr, const size_type _size)
    : string_view(_ptr, 0, _size) {}

  string_view(const char* _ptr) : string_view(_ptr, memory::strlen(_ptr)) {}

  template <size_t N>
  explicit string_view(const char (&_val)[N]) : string_view(&_val[0], 0, N) {}

  explicit string_view(const string& _string);

  string_view(const string& _string, const size_type _size);

  string_view(
      const string& _string, const size_type _offset, const size_type _size);

  template <typename U,
      typename = core::enable_if_t<std::is_convertible<U*, const char*>::value>>
  string_view(contiguous_range<U>&& _range) : m_range(core::move(_range)) {}

  template <typename U,
      typename = core::enable_if_t<std::is_convertible<U*, const char*>::value>>
  string_view(const contiguous_range<U>& _range) : m_range(_range) {}

  string_view(string_view&& _view) : string_view(core::move(_view.m_range)) {}

  string_view(const string_view& _view) : string_view(_view.m_range) {}

  string_view& operator=(nullptr_t) {
    assign(nullptr);

    return *this;
  }

  string_view& operator=(const char* _ptr) {
    assign(_ptr);

    return *this;
  }

  string_view& operator=(const string& _string);

  template <typename U,
      typename = core::enable_if_t<std::is_convertible<U*, const char*>::value>>
  string_view& operator=(contiguous_range<U>&& _range) {
    string_view(core::move(_range)).swap(*this);

    return *this;
  }

  template <typename U,
      typename = core::enable_if_t<std::is_convertible<U*, const char*>::value>>
  string_view& operator=(const contiguous_range<U>& _range) {
    string_view(_range).swap(*this);

    return *this;
  }

  string_view& operator=(string_view&& _view) {
    assign(core::move(_view));

    return *this;
  }

  string_view& operator=(const string_view& _view) {
    assign(_view);

    return *this;
  }

  void assign(nullptr_t) {
    string_view(nullptr).swap(*this);
  }

  void assign(string_view&& _view) {
    string_view(core::move(_view)).swap(*this);
  }

  void assign(const string_view& _view) {
    string_view(_view).swap(*this);
  }

  void assign(const string& _string);

  void assign(const value_type* _ptr) {
    string_view(_ptr).swap(*this);
  }

  string to_string(memory::allocator* _allocator) const;

  contiguous_range<const char> to_contiguous_range() const {
    return m_range;
  }

  // element access

  const_reference operator[](const size_type _pos) const {
    return at(_pos);
  }

  const_reference at(const size_type _pos) const {
    return m_range.at(_pos);
  }

  const_reference front() const {
    return at(0);
  }

  const_reference back() const {
    return at(size() - 1);
  }

  const_pointer data() const {
    return m_range.data();
  }

  // iterators

  const_iterator begin() const {
    return cbegin();
  }

  const_iterator cbegin() const {
    return m_range.cbegin();
  }

  const_iterator end() const {
    return cend();
  }

  const_iterator cend() const {
    return m_range.cend();
  }

  const_reverse_iterator rbegin() const {
    return crbegin();
  }

  const_reverse_iterator crbegin() const {
    return const_reverse_iterator(cend());
  }

  const_reverse_iterator rend() const {
    return crend();
  }

  const_reverse_iterator crend() const {
    return const_reverse_iterator(begin());
  }

  // capacity

  bool empty() const {
    return m_range.empty();
  }

  size_type size() const {
    return m_range.size();
  }

  size_type length() const {
    return size();
  }

  size_type capacity() const {
    return size();
  }

  size_type max_size() const {
    return std::numeric_limits<size_type>::max();
  }

  // operations

  void clear() {
    m_range.clear();
  }

  int32_t compare(const string_view& _view) const {
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

  int32_t compare(const size_type _pos, const size_type _count,
      const string_view& _view) const {
    return substr(_pos, _count).compare(_view);
  }

  int32_t compare(const size_type pos1, const size_type count1,
      const string_view& _view, const size_type pos2,
      const size_type count2) const {
    return substr(pos1, count1).compare(_view.substr(pos2, count2));
  }

  int32_t compare(const char* _ptr) const {
    return compare(string_view(_ptr));
  }

  int32_t compare(
      const size_type pos1, const size_type count1, const char* _ptr) const {
    return substr(pos1, count1).compare(string_view(_ptr));
  }

  int32_t compare(const size_type pos1, const size_type count1,
      const char* _ptr, const size_type count2) const {
    return substr(pos1, count1).compare(string_view(_ptr, count2));
  }

  bool starts_with(const string_view& _view) const {
    const size_type vs = _view.size();
    const size_type s = size();

    return ((s >= vs) && (_view.compare(substr(0, vs)) == 0));
  }

  bool starts_with(const char* _ptr) const {
    return starts_with(string_view(_ptr));
  }

  bool starts_with(char _c) const {
    return starts_with(string_view(&_c, 1));
  }

  bool ends_with(const string_view& _view) const {
    const size_type vs = _view.size();
    const size_type s = size();

    return ((s >= vs) && (_view.compare(substr((s - vs), vs)) == 0));
  }

  bool ends_with(const char* _ptr) const {
    return ends_with(string_view(_ptr));
  }

  bool ends_with(char _c) const {
    return ends_with(string_view(&_c, 1));
  }

  void remove_prefix(const size_type _count) {
    m_range.remove_prefix(_count);
  }

  void remove_suffix(const size_type _count) {
    m_range.remove_suffix(_count);
  }

  string_view trim_left(const string_view& _vals) const {
    string_view copy = *this;
    size_type x = copy.find_first_not_of(_vals);

    if (x != npos) {
      copy.remove_prefix(x);
    } else {
      copy = "";
    }

    return copy;
  }

  string_view trim_right(const string_view& _vals) const {
    string_view copy = *this;
    size_type x = copy.find_last_not_of(_vals);

    if (x != npos) {
      copy.remove_suffix(size() - x - 1);
    } else {
      return "";
    }

    return copy;
  }

  dynamic_array<string_view> split(memory::allocator* _allocator,
      char _delimiter, bool _leave_empty = false) const {
    return split(_allocator, string_view(&_delimiter, 1), _leave_empty);
  }

  dynamic_array<string_view> split(memory::allocator* _allocator,
      const string_view& _delimiters, bool _leave_empty = false) const {
    dynamic_array<string_view> return_split(_allocator);

    if (empty()) {
      return return_split;
    }

    size_type b = 0;

    while (b != size()) {
      const size_type n = find_first_of(_delimiters, b);

      if (n == npos) {
        const size_type s = size() - b;

        if (s == 1 && _delimiters.find(at(b)) != npos) {
          if (_leave_empty) {
            return_split.push_back("");
          }

          return return_split;
        }

        return_split.push_back(substr(b, s));

        return return_split;
      }

      if (n - b == 0 && _delimiters.find(at(b)) != npos) {
        if (_leave_empty) {
          return_split.push_back("");
        }

        b = n + 1;

        continue;
      }

      return_split.push_back(substr(b, n - b));
      b = n + 1;
    }

    return return_split;
  }

  string_view substr(
      const size_type _pos = 0, const size_type _count = npos) const {
    const size_type s = size() - _pos;

    return string_view(data() + _pos, _count < s ? _count : s);
  }

  void swap(string_view& _other) {
    m_range.swap(_other.m_range);
  }

  // searching

  size_type find(const string_view& _view, const size_type _pos = 0) const {
    const size_type pos = math::min(_pos, size());
    auto i = std::search(cbegin() + pos, cend(), _view.cbegin(), _view.cend());

    return (i == cend() ? npos : (i - cbegin()));
  }

  size_type find(const char _c, const size_type _pos = 0) const {
    return find(string_view(&_c, 1), _pos);
  }

  size_type find(
      const char* _ptr, const size_type _pos, const size_type _count) const {
    return find(string_view(_ptr, _count), _pos);
  }

  size_type find(const char* _ptr, size_type _pos = 0) const {
    return find(string_view(_ptr), _pos);
  }

  size_type rfind(const string_view& _view, const size_type _pos = npos) const {
    const size_type pos = math::min(_pos, size());
    const auto i =
        std::search(crend() - pos, crend(), _view.crbegin(), _view.crend());

    return (i == crend() ? npos : (i.base() - cbegin() - 1));
  }

  size_type rfind(const char _c, const size_type _pos = npos) const {
    return rfind(string_view(&_c, 1), _pos);
  }

  size_type rfind(
      const char* _ptr, const size_type _pos, const size_type _count) const {
    return rfind(string_view(_ptr, _count), _pos);
  }

  size_type rfind(const char* _ptr, size_type _pos = npos) const {
    return rfind(string_view(_ptr), _pos);
  }

  size_type find_first_of(
      const string_view& _view, const size_type _pos = 0) const {
    const size_type pos = math::min(_pos, size());
    const auto i = std::find_first_of(
        cbegin() + pos, cend(), _view.cbegin(), _view.cend());

    return (i == cend() ? npos : (i - cbegin()));
  }

  size_type find_first_of(const char _c, const size_type _pos = 0) const {
    return find_first_of(string_view(&_c, 1), _pos);
  }

  size_type find_first_of(
      const char* _ptr, const size_type _pos, const size_type _count) const {
    return find_first_of(string_view(_ptr, _count), _pos);
  }

  size_type find_first_of(const char* _ptr, const size_type _pos = 0) const {
    return find_first_of(string_view(_ptr), _pos);
  }

  size_type find_last_of(
      const string_view& _view, const size_type _pos = npos) const {
    const size_type pos = math::min(_pos, size());
    const auto i = std::find_first_of(
        crend() - pos, crend(), _view.cbegin(), _view.cend());

    return (i == crend() ? npos : (i.base() - cbegin() - 1));
  }

  size_type find_last_of(const char _c, const size_type _pos = npos) const {
    return find_last_of(string_view(&_c, 1), _pos);
  }

  size_type find_last_of(
      const char* _ptr, const size_type _pos, const size_type _count) const {
    return find_last_of(string_view(_ptr, _count), _pos);
  }

  size_type find_last_of(const char* _ptr, const size_type _pos = npos) const {
    return find_last_of(string_view(_ptr), _pos);
  }

  size_type find_first_not_of(
      const string_view& _view, const size_type _pos = 0) const {
    const auto p = [&_view](const char _c) -> bool {
      return (std::find(_view.cbegin(), _view.cend(), _c) == _view.cend());
    };

    const auto i = std::find_if(cbegin() + _pos, cend(), p);

    return (i == cend() ? npos : (i - cbegin()));
  }

  size_type find_first_not_of(const char _c, const size_type _pos = 0) const {
    return find_first_not_of(string_view(&_c, 1), _pos);
  }

  size_type find_first_not_of(
      const char* _ptr, const size_type _pos, const size_type _count) const {
    return find_first_not_of(string_view(_ptr, _count), _pos);
  }

  size_type find_first_not_of(
      const char* _ptr, const size_type _pos = 0) const {
    return find_first_not_of(string_view(_ptr), _pos);
  }

  size_type find_last_not_of(
      const string_view& _view, const size_type _pos = 0) const {
    const auto p = [&_view](const char _c) -> bool {
      return (std::find(_view.cbegin(), _view.cend(), _c) == _view.cend());
    };

    const auto b = cbegin();
    const auto e = const_reverse_iterator(b + _pos);
    const auto i = std::find_if(crbegin(), e, p);

    return (i == e ? npos : static_cast<size_type>(i.base() - b - 1));
  }

  size_type find_last_not_of(const char _c, const size_type _pos = 0) const {
    return find_last_not_of(string_view(&_c, 1), _pos);
  }

  size_type find_last_not_of(
      const char* _ptr, const size_type _pos, const size_type _count) const {
    return find_last_not_of(string_view(_ptr, _count), _pos);
  }

  size_type find_last_not_of(const char* _ptr, const size_type _pos = 0) const {
    return find_last_not_of(string_view(_ptr), _pos);
  }

private:
  range_type m_range;
};

inline bool operator==(const string_view& _lhs, const string_view& _rhs) {
  return (_lhs.compare(_rhs) == 0);
}

inline bool operator==(const string_view& _lhs, const char* _rhs) {
  return (_lhs.compare(_rhs) == 0);
}

inline bool operator==(const char* _lhs, const string_view& _rhs) {
  return (_rhs.compare(_lhs) == 0);
}

inline bool operator!=(const string_view& _lhs, const string_view& _rhs) {
  return (_lhs.compare(_rhs) != 0);
}

inline bool operator!=(const string_view& _lhs, const char* _rhs) {
  return (_lhs.compare(_rhs) != 0);
}

inline bool operator!=(const char* _lhs, const string_view& _rhs) {
  return (_rhs.compare(_lhs) != 0);
}

inline bool operator<(const string_view& _lhs, const string_view& _rhs) {
  return (_lhs.compare(_rhs) < 0);
}

inline bool operator<(const string_view& _lhs, const char* _rhs) {
  return (_lhs.compare(_rhs) < 0);
}

inline bool operator<(const char* _lhs, const string_view& _rhs) {
  return (_rhs.compare(_lhs) > 0);
}

inline bool operator<=(const string_view& _lhs, const string_view& _rhs) {
  return (_lhs.compare(_rhs) <= 0);
}

inline bool operator<=(const string_view& _lhs, const char* _rhs) {
  return (_lhs.compare(_rhs) <= 0);
}

inline bool operator<=(const char* _lhs, const string_view& _rhs) {
  return (_rhs.compare(_lhs) >= 0);
}

inline bool operator>(const string_view& _lhs, const string_view& _rhs) {
  return (_lhs.compare(_rhs) > 0);
}

inline bool operator>(const string_view& _lhs, const char* _rhs) {
  return (_lhs.compare(_rhs) > 0);
}

inline bool operator>(const char* _lhs, const string_view& _rhs) {
  return (_rhs.compare(_lhs) < 0);
}

inline bool operator>=(const string_view& _lhs, const string_view& _rhs) {
  return (_lhs.compare(_rhs) >= 0);
}

inline bool operator>=(const string_view& _lhs, const char* _rhs) {
  return (_lhs.compare(_rhs) >= 0);
}

inline bool operator>=(const char* _lhs, const string_view& _rhs) {
  return (_rhs.compare(_lhs) <= 0);
}

}  // namespace containers
}  // namespace wn

// string related defintions

#include "WNContainers/inc/WNString.h"

namespace wn {
namespace containers {

inline string_view::string_view(const string& _string)
  : string_view(_string.data(), _string.size()) {}

inline string_view& string_view::operator=(const string& _string) {
  assign(_string);

  return *this;
}

inline void string_view::assign(const string& _string) {
  string_view(_string).swap(*this);
}

inline string string_view::to_string(memory::allocator* _allocator) const {
  return string(_allocator, data(), size());
}

inline string_view::string_view(const string& _string, const size_type _size)
  : string_view(_string.data(), _size) {
  WN_DEBUG_ASSERT(
      _size <= _string.size(), "attempting to access outside of string bounds");
}

inline string_view::string_view(
    const string& _string, const size_type _offset, const size_type _size)
  : string_view(_string.data(), _offset, _size) {
  WN_DEBUG_ASSERT((_offset + _size) <= _string.size(),
      "attempting to access outside of string bounds");
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
  size_t operator()(const wn::containers::string_view& _lhs,
      const wn::containers::string_view& _rhs) const {
    return (_lhs == _rhs);
  }
};

}  // namespace std

#endif  // __WN_CONTAINERS_STRING_VIEW_H__
