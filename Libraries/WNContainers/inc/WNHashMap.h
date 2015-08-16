// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_HASH_MAP_H__
#define __WN_CONTAINERS_HASH_MAP_H__

#include "WNMemory/inc/WNAllocator.h"
#include "WNContainers/inc/WNDynamicArray.h"
#include "WNContainers/inc/WNList.h"
#include <algorithm>
#include <utility>
#include <iterator>

namespace wn {
namespace containers {
template <typename _KeyType, typename _ValueType,
          typename _HashOperator = std::hash<_KeyType>,
          typename _EqualityOperator = std::equal_to<_KeyType>,
          typename _Allocator = memory::default_allocator>
class hash_map final {
  static _Allocator s_default_allocator;
public:
  static _Allocator& get_default_allocator() {
    return s_default_allocator;
  }
  typedef _KeyType key_type;
  typedef _ValueType mapped_type;
  typedef std::pair<_KeyType, _ValueType> value_type;
  typedef wn_size_t size_type;
  typedef wn_signed_t difference_type;
  typedef _Allocator allocator_type;
  typedef _HashOperator hasher;
  typedef _EqualityOperator key_equal;
  typedef std::pair<_KeyType, _ValueType>& reference;
  typedef const std::pair<_KeyType, _ValueType>& const_reference;
  typedef std::pair<_KeyType, _ValueType>* pointer;
  typedef const std::pair<_KeyType, _ValueType>* const_pointer;

  typedef list<value_type> list_type;
  typedef dynamic_array<list<value_type>> array_type;
  template <typename _ContainedType, typename _ArrayIteratorType,
            typename _ListIteratorType>
  class hash_map_iterator
      : std::iterator<std::forward_iterator_tag, _ContainedType> {
   public:
    hash_map_iterator() {}
    template <typename _CT, typename _AIT, typename _LIT>
    hash_map_iterator(const hash_map_iterator<_CT, _AIT, _LIT>& _other) {
      m_bucket = _other.m_bucket;
      m_element = _other.m_element;
      m_end_bucket = _other.m_end_bucket;
    }

    hash_map_iterator& operator+=(wn_size_t _count) {
      while (_count-- > 0) {
        ++m_element;
        while (m_element == m_bucket->tend(_ListIteratorType())) {
          if (m_bucket + 1 == m_end_bucket) {
            break;
          }
          ++m_bucket;
          m_element = m_bucket->tbegin(_ListIteratorType());
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

    template <typename _CT, typename _AIT, typename _LIT>
    bool operator==(const hash_map_iterator<_CT, _AIT, _LIT>& _other) const {
      return (m_element == _other.m_element && m_bucket == _other.m_bucket);
    }

    template <typename _CT, typename _AIT, typename _LIT>
    bool operator!=(const hash_map_iterator<_CT, _AIT, _LIT>& _other) const {
      return !(*this == _other);
    }
    _ContainedType* operator->() { return &(*m_element); }

    _ContainedType& operator*() { return *m_element; }

   private:
    hash_map_iterator(_ArrayIteratorType bucket, _ArrayIteratorType end_bucket,
                      _ListIteratorType element)
        : m_bucket(bucket), m_end_bucket(end_bucket), m_element(element) {}
    _ArrayIteratorType m_bucket;
    _ArrayIteratorType m_end_bucket;
    _ListIteratorType m_element;
    friend class hash_map;
  };

  using iterator = hash_map_iterator<value_type, typename array_type::iterator,
                                     typename list_type::iterator>;
  using const_iterator =
      hash_map_iterator<const value_type, typename array_type::const_iterator,
                        typename list_type::const_iterator>;
  typedef typename list_type::iterator local_iterator;
  typedef typename list_type::const_iterator const_local_iterator;

  hash_map() : hash_map(&s_default_allocator) {}
  hash_map(memory::allocator* _allocator)
      : m_allocator(_allocator),
        m_buckets(_allocator),
        m_total_elements(0),
        m_max_load_factor(1.0) {}

  bool empty() const { return m_total_elements == 0; }

  std::pair<iterator, bool> insert(const value_type& element) {
    wn_size_t hash = get_hash(element->first);
    iterator old_position = find_hashed(element->first, hash);
    if (old_position != end()) {
      return std::make_pair(old_position, false);
    }
    if (rehash_if_needed(size() + 1)) {
      hash = get_hash(element->first);
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
    iterator old_position = find_hashed(element.first, hash);
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

  iterator find(const key_type& key) {
    if (empty()) {
      return (end());
    }
    wn_size_t hash = get_hash(key);
    return find_hashed(key, hash);
  }

  const_iterator find(const key_type& key) const {
    if (empty()) {
      return (cend());
    }
    wn_size_t hash = get_hash(key);
    return find_hashed(key, hash);
  }

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

  iterator begin() {
    if (empty()) {
      return (end());
    }
    iterator it = iterator(m_buckets.begin(), m_buckets.end(),
                           m_buckets.begin()->begin());
    while (it.m_bucket->empty()) {
      it.m_bucket += 1;
    }
    it.m_element = it.m_bucket->begin();
    return it;
  }
  const_iterator cbegin() { return begin(); }
  iterator end() {
    if (m_buckets.size() == 0) {
      return iterator(typename array_type::iterator(),
                      typename array_type::iterator(),
                      typename list_type::iterator());
    }
    return iterator(m_buckets.end() - 1, m_buckets.end(),
                    (m_buckets.end() - 1)->end());
  }
  const_iterator cend() { return end(); }

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
  iterator find_hashed(const key_type& key, wn_size_t hash) {
    if (empty()) {
      return (end());
    }
    for (auto it = m_buckets[hash].begin(); it != m_buckets[hash].end(); ++it) {
      if (key_equal()(it->first, key)) {
        auto new_it = m_buckets.begin() + hash;
        return iterator(new_it, m_buckets.end(), it);
      }
    }
    return end();
  }

  wn_size_t get_hash(const key_type& key) const {
    return get_hash(key, m_buckets.size());
  }

  wn_size_t get_hash(const key_type& key, wn_size_t buckets) const {
    wn_size_t hash = hasher()(key);
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
};

template <typename _KeyType, typename _ValueType, typename _HashOperator,
          typename _EqualityOperator, typename _Allocator>
_Allocator hash_map<_KeyType, _ValueType, _HashOperator, _EqualityOperator,
                    _Allocator>::s_default_allocator;
}
}

#endif  //_WN_CONTAINERS_HASH_MAP_H__
