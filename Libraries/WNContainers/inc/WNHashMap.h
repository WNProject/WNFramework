// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CONTAINERS_HASH_MAP_H__
#define __WN_CONTAINERS_HASH_MAP_H__

#include "WNMemory/inc/WNAllocator.h"
#include "WNContainers/inc/WNDynamicArray.h"
#include "WNContainers/inc/WNList.h"
#include <algorithm>
#include <utility>
#include <initializer_list>
#include <iterator>

namespace wn {
namespace containers {



template <typename _KeyType, typename _ValueType,
          typename _HashOperator = std::hash<_KeyType>,
          typename _EqualityOperator = std::equal_to<_KeyType>,
          typename _Allocator = memory::default_allocator>
class hash_map;
namespace internal {
template<typename T, typename = void>
struct map_iterator_internal_type {
  using type = T;
  using array_iterator = typename dynamic_array<list<type>>::iterator;
  using list_iterator = typename list<type>::iterator;
};

template <typename T>
struct map_iterator_internal_type<
    T, typename core::enable_if<core::is_const<T>::value>::type> {
  using type = typename std::remove_const<T>::type;
  using array_iterator = typename dynamic_array<list<type>>::const_iterator;
  using list_iterator = typename list<type>::const_iterator;
};

// hash_map_iterator is not a child_type of hash_map
// because MSVC threw an error that the symbol name
// was too large and would be truncated.
template <typename _ContainedType>
class hash_map_iterator
    : std::iterator<std::forward_iterator_tag, _ContainedType> {
 public:
  using type = _ContainedType;
  using ait = typename map_iterator_internal_type<type>::array_iterator;
  using lit = typename map_iterator_internal_type<type>::list_iterator;

  hash_map_iterator() {}
  template <typename _CT>
  hash_map_iterator(const hash_map_iterator<_CT>& _other) {
    m_bucket = _other.m_bucket;
    m_element = _other.m_element;
    m_end_bucket = _other.m_end_bucket;
  }

  hash_map_iterator& operator+=(wn_size_t _count) {
    while (_count-- > 0) {
      ++m_element;
      while (m_element == m_bucket->tend(lit())) {
        if (m_bucket + 1 == m_end_bucket) {
          break;
        }
        ++m_bucket;
        m_element = m_bucket->tbegin(lit());
      }
    }
    return *this;
  }

  hash_map_iterator& operator++() { return (*this += 1); }
  hash_map_iterator operator++(int) {
    hash_map_iterator it = *this;
    *this += 1;
    return it;
  }

  template <typename _CT>
  bool operator==(const hash_map_iterator<_CT>& _other) const {
    return (m_element == _other.m_element && m_bucket == _other.m_bucket);
  }

  template <typename _CT>
  bool operator!=(const hash_map_iterator<_CT>& _other) const {
    return !(*this == _other);
  }

  type* operator->() const { return &(*m_element); }

  type& operator*() const { return *m_element; }

 private:
  hash_map_iterator(ait bucket, ait end_bucket, lit element)
      : m_bucket(bucket), m_end_bucket(end_bucket), m_element(element) {}
  ait m_bucket;
  ait m_end_bucket;
  lit m_element;

  template <typename _KeyType, typename _ValueType, typename _HashOperator,
            typename _EqualityOperator, typename _Allocator>
  friend class wn::containers::hash_map;
  template <typename _CT>
  friend class hash_map_iterator;
};
}  // namespace internal

template <typename _KeyType, typename _ValueType, typename _HashOperator,
          typename _EqualityOperator, typename _Allocator>
class hash_map final {
  static _Allocator s_default_allocator;

 public:
  static _Allocator& get_default_allocator() { return s_default_allocator; }
  using key_type = _KeyType;
  using mapped_type = _ValueType;
  using value_type = std::pair<_KeyType, _ValueType>;
  using size_type = wn_size_t;
  using difference_type = wn_signed_t;
  using allocator_type = _Allocator;
  using hasher = _HashOperator;
  using key_equal = _EqualityOperator;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;

  using list_type = list<value_type>;
  using array_type = dynamic_array<containers::list<value_type>>;

  using iterator =
      internal::hash_map_iterator<value_type>;
  using const_iterator =
      internal::hash_map_iterator<const value_type>;
  using local_iterator = typename list_type::iterator;
  using const_local_iterator = typename list_type::const_iterator;

  explicit hash_map(memory::allocator* _allocator)
      : hash_map(0u, hasher(), key_equal(), _allocator) {}

  explicit hash_map(size_type _n = 0u, const hasher& _hasher = hasher(),
                    const key_equal& _key_equal = key_equal(),
                    memory::allocator* _allocator = &s_default_allocator)
      : m_allocator(_allocator),
        m_buckets(_n, list_type(_allocator), _allocator),
        m_total_elements(0),
        m_max_load_factor(1.0),
        m_hasher(_hasher),
        m_key_equal(_key_equal) {}

  hash_map(std::initializer_list<value_type> initializer, size_type _n = 0u,
           const hasher& _hasher = hasher(),
           const key_equal& _key_equal = key_equal(),
           memory::allocator* _allocator = &s_default_allocator)
      : hash_map(0u /* we will resize */, _hasher, _key_equal, _allocator) {
    auto begin = std::begin(initializer);
    auto end = std::end(initializer);
    wn_size_t count = end - begin;
    m_buckets.insert(m_buckets.begin(), (count > _n ? count : _n),
                     list_type(_allocator));
    for (; begin != end; ++begin) {
      insert(*begin);
    }
  }

  hash_map(std::initializer_list<value_type> initializer,
           memory::allocator* _allocator)
      : hash_map(initializer, 0u, hasher(), key_equal(), _allocator) {}

  template <typename _A>
  hash_map(hash_map<_KeyType, _ValueType, _HashOperator, _EqualityOperator,
                    _A>&& _other)
      : m_allocator(_other.m_allocator),
        m_buckets(std::move(_other.m_buckets)),
        m_total_elements(_other.m_total_elements),
        m_max_load_factor(_other.m_max_load_factor),
        m_hasher(std::move(_other.m_hasher)),
        m_key_equal(std::move(_other.m_key_equal)) {
    _other.m_total_elements = 0;
    _other.m_max_load_factor = 1.0f;
  }

  hash_map(const hash_map& _other)
      : m_allocator(_other.m_allocator),
        m_buckets(_other.m_buckets),
        m_total_elements(_other.m_total_elements),
        m_max_load_factor(_other.m_max_load_factor),
        m_hasher(_other.m_hasher),
        m_key_equal(_other.m_key_equal) {}

  bool empty() const { return m_total_elements == 0; }

  std::pair<iterator, bool> insert(const value_type& element) {
    wn_size_t hash = get_hash(element.first);
    iterator old_position =
        iterator_from_const_iterator(find_hashed(element.first, hash));
    if (old_position != end()) {
      return std::make_pair(old_position, false);
    }
    if (rehash_if_needed(size() + 1)) {
      hash = get_hash(element.first);
    }
    m_buckets[hash].push_back(element);
    m_total_elements += 1;
    return std::make_pair<iterator, bool>(
        iterator(m_buckets.begin() + hash, m_buckets.end(),
                 m_buckets[hash].end() - 1),
        true);
  }

  std::pair<iterator, bool> insert(value_type&& element) {
    wn_size_t hash;
    hash = get_hash(element.first);
    iterator old_position =
        iterator_from_const_iterator(find_hashed(element.first, hash));
    if (old_position != end()) {
      return std::make_pair(old_position, false);
    }
    if (rehash_if_needed(size() + 1)) {
      hash = get_hash(element.first);
    }
    m_buckets[hash].push_back(std::move(element));
    m_total_elements += 1;
    return std::make_pair<iterator, bool>(
        iterator(m_buckets.begin() + hash, m_buckets.end(),
                 m_buckets[hash].end() - 1),
        true);
  }

  std::pair<iterator, bool> insert(const key_type& kt, const mapped_type& mt) {
    return insert(std::make_pair(kt, mt));
  }

  mapped_type& operator[](key_type&& key) {
    iterator it = find(key);
    if (it != end()) {
      return it->second;
    }
    return insert(std::make_pair(std::move(key), mapped_type())).first->second;
  }

  mapped_type& operator[](const key_type& key) {
    iterator it = find(key);
    if (it != end()) {
      return it->second;
    }
    return insert(std::make_pair(key, mapped_type())).first->second;
  }

  const mapped_type& operator[](const key_type& key) const {
    iterator it = find(key);
    if (it != end()) {
      return it->second;
    }
    return insert(key, mapped_type())->first->second;
  }

  iterator find(const key_type& key) {
    if (empty()) {
      return (end());
    }
    wn_size_t hash = get_hash(key);
    return iterator_from_const_iterator(find_hashed(key, hash));
  }

  const_iterator find(const key_type& key) const {
    if (empty()) {
      return (cend());
    }
    wn_size_t hash = get_hash(key);
    return find_hashed(key, hash);
  }

  void clear() { m_buckets.clear(); }

  iterator erase(const_iterator _it) {
    size_t bucket_num = _it.m_bucket - m_buckets.begin();
    iterator it = iterator(m_buckets.begin() + bucket_num, m_buckets.end(),
                           m_buckets[bucket_num].erase(_it.m_element));
    while (it != end()) {
      if (it.m_bucket->empty()) {
        it.m_bucket += 1;
        it.m_element = it.m_bucket->begin();
      } else {
        break;
      }
    }
    m_total_elements -= 1;
    return it;
  }

  const_iterator begin() const {
    if (empty()) {
      return (end());
    }
    const_iterator it = const_iterator(m_buckets.cbegin(), m_buckets.cend(),
                                       m_buckets.cbegin()->cbegin());
    while (it.m_bucket->empty()) {
      it.m_bucket += 1;
    }
    it.m_element = it.m_bucket->cbegin();
    return it;
  }

  iterator begin() {
    return iterator_from_const_iterator(cbegin());
  }

  const_iterator cbegin() const { return begin(); }

  iterator end() {
    return iterator_from_const_iterator(cend());
  }

  const_iterator end() const {
    if (m_buckets.size() == 0) {
      return iterator(typename array_type::iterator(),
                      typename array_type::iterator(),
                      typename list_type::iterator());
    }
    return const_iterator(m_buckets.cend() - 1, m_buckets.cend(),
                    (m_buckets.cend() - 1)->cend());
  }
  const_iterator cend() const { return end(); }

  // The only allocation that rehash performs is an
  // increase on the number of buckets, all of the elements
  // in the buckets are left unchanged.
  void rehash(wn_size_t n) {
    if (n == m_buckets.size()) {
      return;
    }
    if (((float)m_total_elements / n) > m_max_load_factor) {
      return;
    }

    array_type new_buckets(n, list_type(m_allocator), m_allocator);
    for (auto& bucket : m_buckets) {
      for (auto it = bucket.begin(); it != bucket.end();) {
        wn_size_t hash = get_hash(it->first, n);
        it = bucket.transfer_to(it, new_buckets[hash].end(), new_buckets[hash]);
      }
    }
    m_buckets.swap(new_buckets);
    // new_buckets should have only empty elements;
    WN_DEBUG_ASSERT(std::find_if(new_buckets.begin(), new_buckets.end(),
                                 [](list_type& bucket) {
                                   if (!bucket.empty()) {
                                     return true;
                                   }
                                   return false;
                                 }) == new_buckets.end());
  }

  wn_size_t size() const { return m_total_elements; }

 private:
  iterator iterator_from_const_iterator(const_iterator it) {
    iterator new_it;
    new_it.m_bucket = m_buckets.begin() + (it.m_bucket - m_buckets.cbegin());
    new_it.m_end_bucket = m_buckets.end();
    new_it.m_element = new_it.m_bucket->convert_iterator(it.m_element);
    return new_it;
   }

  const_iterator find_hashed(const key_type& key, wn_size_t hash) const {
    if (empty()) {
      return (cend());
    }
    for (auto it = m_buckets[hash].cbegin(); it != m_buckets[hash].cend();
         ++it) {
      if (m_key_equal(it->first, key)) {
        auto new_it = m_buckets.cbegin() + hash;
        return const_iterator(new_it, m_buckets.cend(), it);
      }
    }
    return cend();
  }

  wn_size_t get_hash(const key_type& key) const {
    return get_hash(key, m_buckets.size());
  }

  wn_size_t get_hash(const key_type& key, wn_size_t buckets) const {
    wn_size_t hash = m_hasher(key);
    if (buckets == 0) {
      return 0;
    }
    return hash % buckets;
  }

  bool rehash_if_needed(wn_size_t num_expected_elements) {
    wn_size_t i = required_buckets(num_expected_elements);
    if (i > m_buckets.size()) {
      // If we are allocating buckets, let's give ourselves
      // at least 10 buckets.
      i = i > 10 ? i : 10;

      // If we are expanding past 10 buckets then we should probably
      // pick some exponential factor. (1.5 for now).
      wn_size_t exponential_buckets =
          static_cast<size_t>(m_buckets.size() * 1.5f);
      wn_size_t num_new_buckets =
          i > exponential_buckets ? i : exponential_buckets;
      rehash(num_new_buckets);
      return true;
    }
    return false;
  }

  wn_size_t required_buckets(wn_size_t num_expected_elements) const {
    return static_cast<wn_size_t>(num_expected_elements / m_max_load_factor);
  }

 private:
  memory::allocator* m_allocator;
  array_type m_buckets;
  wn_size_t m_total_elements;
  wn_float32 m_max_load_factor;

  hasher m_hasher;
  key_equal m_key_equal;
};

template <typename _KeyType, typename _ValueType, typename _HashOperator,
          typename _EqualityOperator, typename _Allocator>
_Allocator hash_map<_KeyType, _ValueType, _HashOperator, _EqualityOperator,
                    _Allocator>::s_default_allocator;
}
}

#endif  //_WN_CONTAINERS_HASH_MAP_H__
