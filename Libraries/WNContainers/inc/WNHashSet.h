// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_HASH_SET_H__
#define __WN_CONTAINERS_HASH_SET_H__

#include "WNMemory/inc/WNAllocator.h"
#include "WNContainers/inc/WNHashMap.h"

namespace wn {
namespace containers {
struct empty_element {};

template <typename _Key, typename _HashOperator = std::hash<_Key>,
          typename _EqualityOperator = std::equal_to<_Key>,
          typename _Allocator = memory::default_allocator>
class hash_set;

namespace internal {
// hash_set_iterator is not a child_type of hash_set
// because MSVC threw an error that the symbol name
// was too large and would be truncated.
template <typename _IteratorType, typename _ContainedType>
class hash_set_iterator
    : public std::iterator<std::forward_iterator_tag, _ContainedType> {
 public:
  hash_set_iterator() {}
  template <typename _IT, typename _CT>
  hash_set_iterator(const hash_set_iterator<_IT, _CT>& _other)
      : m_map_iterator(_other.m_map_iterator) {}

  hash_set_iterator& operator+=(wn_size_t _count) {
    m_map_iterator += _count;
    return *this;
  }

  hash_set_iterator& operator++() { return (*this += 1); }
  hash_set_iterator operator++(int) {
    hash_set_iterator it = *this;
    *this += 1;
    return it;
  }

  template <typename _IT, typename _CT>
  bool operator==(const hash_set_iterator<_IT, _CT>& _other) const {
    return (m_map_iterator == _other.m_map_iterator);
  }

  template <typename _IT, typename _CT>
  bool operator!=(const hash_set_iterator<_IT, _CT>& _other) const {
    return !(*this == _other);
  }
  _ContainedType* operator->() { return &m_map_iterator->first; }

  _ContainedType& operator*() { return m_map_iterator->first; }

 private:
  explicit hash_set_iterator(const _IteratorType& _iterator)
      : m_map_iterator(_iterator) {}
  template <typename _Key, typename _HashOperator, typename _EqualityOperator,
            typename _Allocator>
  friend class hash_set;
  template <typename _IT, typename _CT>
  friend class hash_set_iterator;
  _IteratorType m_map_iterator;
};
}  // namespace internal

template <typename _Key, typename _HashOperator, typename _EqualityOperator,
          typename _Allocator>
class hash_set final {
  static _Allocator s_default_allocator;
 public:
   static _Allocator& get_default_allocator() {
     return s_default_allocator;
   }
  typedef hash_map<_Key, empty_element, _HashOperator, _EqualityOperator,
                   _Allocator> map_type;

  
  typedef _Key key_type;
  typedef _Key value_type;

  typedef wn_size_t size_type;
  typedef wn_signed_t difference_type;
  typedef _Allocator allocator_type;
  typedef _HashOperator hasher;
  typedef _EqualityOperator key_equal;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;

  using iterator =
      internal::hash_set_iterator<typename map_type::iterator, value_type>;
  using const_iterator =
      internal::hash_set_iterator<typename map_type::const_iterator,
                                  const value_type>;

  hash_set(size_type _n = 0u, const hasher& _hasher = hasher(),
           const key_equal& _key_equal = key_equal(),
           memory::allocator* _allocator = &s_default_allocator)
      : m_map(_n, _hasher, _key_equal, _allocator) {}
  hash_set(memory::allocator* _allocator)
      : hash_set(0u, hasher(), key_equal(), _allocator) {}
  hash_set(std::initializer_list<key_type> initializer, size_type _n = 0u,
           const hasher& _hasher = hasher(),
           const key_equal& _key_equal = key_equal(),
           memory::allocator* _allocator = &s_default_allocator)
      : hash_set(0u, _hasher, _key_equal, _allocator) {
    auto begin = std::begin(initializer);
    auto end = std::end(initializer);
    wn_size_t count = end - begin;
    wn_size_t buckets = _n < count ? _n : count;
    m_map.rehash(buckets);
    for (; begin != end; ++begin) {
      insert(*begin);
    }
  }

  bool empty() const { m_map.empty(); }

  std::pair<iterator, bool> insert(const value_type& element) {
    auto internal_pair = m_map.insert(element, empty_element());
    return std::make_pair(iterator(internal_pair.first), internal_pair.second);
  }

  std::pair<iterator, bool> insert(value_type&& element) {
    auto internal_pair = m_map.insert(std::move(
        std::make_pair(std::move(element), std::move(empty_element()))));
    return std::make_pair(iterator(internal_pair.first), internal_pair.second);
  }

  iterator find(const key_type& key) { return iterator(m_map.find(key)); }
  const_iterator find(const key_type& key) const {
    return const_iterator(m_map.find(key));
  }

  iterator erase(const_iterator _it) {
    return iterator(m_map.erase(_it.m_map_iterator));
  }

  iterator begin() { return iterator(m_map.begin()); }
  const_iterator cbegin() { return const_iterator(m_map.cbegin()); }
  iterator end() { return iterator(m_map.end()); }
  const_iterator cend() { return const_iterator(m_map.cend()); }

  void rehash(wn_size_t n) { m_map.rehash(n); }

  wn_size_t size() const { return m_map.size(); }

 private:
  hash_map<_Key, empty_element, _HashOperator, _EqualityOperator, _Allocator>
      m_map;
};

template <typename _Key, typename _HashOperator, typename _EqualityOperator,
          typename _Allocator>
_Allocator hash_set<_Key, _HashOperator, _EqualityOperator,
                    _Allocator>::s_default_allocator;
}
}

#endif  //_WN_CONTAINERS_HASH_SET_H__