// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_HASH_SET_H__
#define __WN_CONTAINERS_HASH_SET_H__

#include "WNContainers/inc/WNHashMap.h"
#include "WNMemory/inc/allocator.h"

namespace wn {
namespace containers {

struct empty_element {};

template <typename _Key, typename _HashOperator = std::hash<_Key>,
    typename _EqualityOperator = std::equal_to<_Key>>
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

  hash_set_iterator& operator+=(size_t _count) {
    m_map_iterator += _count;
    return *this;
  }

  hash_set_iterator& operator++() {
    return (*this += 1);
  }
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
  _ContainedType* operator->() {
    return &m_map_iterator->first;
  }

  _ContainedType& operator*() {
    return m_map_iterator->first;
  }

private:
  explicit hash_set_iterator(const _IteratorType& _iterator)
    : m_map_iterator(_iterator) {}

  template <typename _Key, typename _HashOperator, typename _EqualityOperator>
  friend class wn::containers::hash_set;

  template <typename _IT, typename _CT>
  friend class hash_set_iterator;

  _IteratorType m_map_iterator;
};
}  // namespace internal

template <typename _Key, typename _HashOperator, typename _EqualityOperator>
class hash_set final {
public:
  using map_type =
      hash_map<_Key, empty_element, _HashOperator, _EqualityOperator>;

  using key_type = _Key;
  using value_type = _Key;
  using size_type = size_t;
  using difference_type = signed_t;
  using hasher = _HashOperator;
  using key_equal = _EqualityOperator;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;

  using iterator =
      internal::hash_set_iterator<typename map_type::iterator, value_type>;
  using const_iterator =
      internal::hash_set_iterator<typename map_type::const_iterator,
          const value_type>;

  hash_set(memory::allocator* _allocator, const size_type _n,
      const hasher& _hasher = hasher(),
      const key_equal& _key_equal = key_equal())
    : m_map(_allocator, _n, _hasher, _key_equal) {}
  hash_set(memory::allocator* _allocator)
    : hash_set(_allocator, 0u, hasher(), key_equal()) {}
  hash_set(memory::allocator* _allocator,
      std::initializer_list<key_type> initializer, size_type _n = 0u,
      const hasher& _hasher = hasher(),
      const key_equal& _key_equal = key_equal())
    : hash_set(_allocator, 0u, _hasher, _key_equal) {
    auto begin = std::begin(initializer);
    auto end = std::end(initializer);
    size_t count = end - begin;
    size_t buckets = _n < count ? _n : count;
    m_map.rehash(buckets);
    for (; begin != end; ++begin) {
      insert(*begin);
    }
  }

  bool empty() const {
    return m_map.empty();
  }

  core::pair<iterator, bool> insert(const value_type& element) {
    auto internal_pair = m_map.insert(element, empty_element());
    return core::make_pair(iterator(internal_pair.first), internal_pair.second);
  }

  core::pair<iterator, bool> insert(value_type&& element) {
    auto internal_pair = m_map.insert(core::move(
        core::make_pair(core::move(element), core::move(empty_element()))));
    return core::make_pair(iterator(internal_pair.first), internal_pair.second);
  }

  iterator find(const key_type& key) {
    return iterator(m_map.find(key));
  }
  const_iterator find(const key_type& key) const {
    return const_iterator(m_map.find(key));
  }

  iterator erase(const_iterator _it) {
    return iterator(m_map.erase(_it.m_map_iterator));
  }

  iterator begin() {
    return iterator(m_map.begin());
  }
  const_iterator cbegin() {
    return const_iterator(m_map.cbegin());
  }
  iterator end() {
    return iterator(m_map.end());
  }
  const_iterator cend() {
    return const_iterator(m_map.cend());
  }

  const_iterator begin() const {
    return const_iterator(m_map.begin());
  }
  const_iterator end() const {
    return const_iterator(m_map.end());
  }

  void rehash(size_t n) {
    m_map.rehash(n);
  }

  size_t size() const {
    return m_map.size();
  }

private:
  hash_map<_Key, empty_element, _HashOperator, _EqualityOperator> m_map;
};

}  // namespace containers
}  // namespace wn

#endif  //_WN_CONTAINERS_HASH_SET_H__
