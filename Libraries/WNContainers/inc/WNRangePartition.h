// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CONTAINERS_RANGE_PARTITION_H__
#define __WN_CONTAINERS_RANGE_PARTITION_H__

#include "WNMath/inc/WNBasic.h"
#include "WNMemory/inc/allocator.h"

namespace wn {
namespace containers {

struct partition_node;

class default_node_allocator {
public:
  default_node_allocator() : m_allocator(nullptr) {}
  default_node_allocator(memory::allocator* _allocator)
    : m_allocator(_allocator) {}
  partition_node* allocate_node_for(size_t, size_t);
  void free_node(partition_node* node);
  void move_node(size_t, size_t, size_t, size_t) {}

private:
  memory::allocator* m_allocator;
};

template <typename NodeAllocator = default_node_allocator>
class range_partition {
private:
  // TODO(awoloszyn): If our small-type optimization only misses
  // by a small bit most of the time, consider expanding it.
  static const size_t k_no_allocation_size = sizeof(size_t) * 8;
  WN_FORCE_INLINE bool small_size() const {
    return m_num_elements <= k_no_allocation_size;
  }
  partition_node* const dummy_partition_node =
      reinterpret_cast<partition_node*>(0x1);
  friend struct token;

public:
  struct token : core::non_copyable {
  public:
    size_t size() const;
    size_t offset() const;
    bool is_valid() const;

    token(token&& _other) {
      list = _other.list;
      _other.list = nullptr;
      node = _other.node;
    }

    token& operator=(token&& _other) {
      free();
      list = _other.list;
      _other.list = nullptr;
      node = _other.node;
      return *this;
    }

    token(const token&) = delete;
    ~token() {
      free();
    }
    void unsafe_release() {
      list = nullptr;
    }

    void free() {
      if (list) {
        auto* l = list;
        l->release_interval(core::move(*this));
      }
      list = nullptr;
    }
    token() : list(nullptr) {}

  private:
    template <typename T>
    friend class range_partition;
    token(range_partition<NodeAllocator>* _list, partition_node* _node)
      : list(_list), node(_node) {}
    token(range_partition<NodeAllocator>* _list, size_t _bits)
      : list(_list), bits(_bits) {}

    range_partition<NodeAllocator>* list;

    union {
      partition_node* node;
      size_t bits;
    };
  };

  range_partition(const NodeAllocator& _node_allocator, size_t num_elements);

  range_partition() : m_num_elements(0) {}

  // This will free any and all nodes that have been allocated.
  // It is legal to call this while there are still some nodes that
  // are unaccounted for, but the contents of those nodes should be
  // considered no longer valid.
  ~range_partition();

  size_t size() const;

  size_t used() const;

  void release_interval(token token);

  token get_interval(size_t size);
  range_partition& operator=(range_partition&& _other);

private:
  void reset();
  // Returns a node that contains _size elements.
  // Splits the given node if it is larger than _size elements,
  // returns the entire node if it is exactly _size elements.
  // It is an error to call this on a node with fewer than _size elements.
  partition_node* split_at(partition_node* node, size_t _size);

  size_t m_num_elements;
  size_t m_used_space;
  union {
    partition_node* m_node;
    size_t m_used_bucket;
  };
  partition_node* m_free_list;
  NodeAllocator m_node_allocator;
};

struct partition_node {
public:
  size_t size() const {
    return m_size;
  }
  size_t offset() const {
    return m_offset;
  }

private:
  template <typename T>
  friend class range_partition;
  partition_node* m_next = nullptr;
  partition_node* m_previous = nullptr;
  partition_node* m_next_free = nullptr;
  size_t m_offset = 0;
  size_t m_size = 0;

  bool is_first_node() const {
    return m_previous == nullptr;
  }
  bool is_last_node() const {
    return m_next == nullptr;
  }
  bool is_only_node() const {
    return (m_previous == m_next) && m_previous == nullptr;
  }

  void insert_before(partition_node* _next) {
    if (_next) {
      insert_between(_next->m_previous, _next);
    }
  }

  void insert_after(partition_node* _previous) {
    if (_previous) {
      insert_between(_previous, _previous->m_next);
    }
  }

  void insert_between(partition_node* _previous, partition_node* _next) {
    if (_previous) {
      _previous->m_next = this;
      m_previous = _previous;
    }
    if (_next) {
      _next->m_previous = this;
      m_next = _next;
    }
  }
  bool remove() {
    if (m_previous) {
      m_previous->m_next = m_next;
    }
    if (m_next) {
      m_next->m_previous = m_previous;
    }
    m_previous = nullptr;
    m_next = nullptr;
    return true;
  }

  WN_FORCE_INLINE partition_node* merge_left() {
    WN_DEBUG_ASSERT(m_next_free, "Cannot merge wiht previous non-free block");
    if (m_previous && m_previous->m_next_free) {
      partition_node* p = m_previous;
      m_next_free = m_previous->m_next_free;
      m_size += m_previous->m_size;
      m_previous = m_previous->m_previous;
      if (m_previous) {
        m_previous->m_next = this;
      }
      return p;
    }
    return nullptr;
  }

  partition_node* merge_right() {
    WN_DEBUG_ASSERT(m_next_free, "Cannot merge wiht previous non-free block");
    if (m_next && m_next->m_next_free) {
      partition_node* n = m_next;
      m_next_free = m_next->m_next_free;
      m_size += m_next->m_size;

      m_next = m_next->m_next;
      if (m_next) {
        m_next->m_previous = this;
      }

      return n;
    }
    return nullptr;
  }
};

inline partition_node* default_node_allocator::allocate_node_for(
    size_t, size_t) {
  return m_allocator->construct<partition_node>();
}
inline void default_node_allocator::free_node(partition_node* node) {
  m_allocator->destroy(node);
}

template <typename NodeAllocator>
inline size_t range_partition<NodeAllocator>::token::size() const {
  return list->small_size() ? math::popcount_sparse(bits) : node->size();
}
template <typename NodeAllocator>
inline size_t range_partition<NodeAllocator>::token::offset() const {
  return list->small_size() ? math::trailing_zeros(bits) : node->offset();
}
template <typename NodeAllocator>
inline bool range_partition<NodeAllocator>::token::is_valid() const {
  return list && (node || bits);
}

template <typename NodeAllocator>
range_partition<NodeAllocator>::range_partition(
    const NodeAllocator& _node_allocator, size_t num_elements)
  : m_num_elements(num_elements),
    m_used_space(0),
    m_free_list(dummy_partition_node),
    m_node_allocator(_node_allocator) {
  if (small_size()) {
    if (num_elements == k_no_allocation_size) {
      m_used_bucket = 0;
    } else {
      // Mask off all of the buckets that we cannot use.
      m_used_bucket = ~((static_cast<size_t>(1) << num_elements) - 1);
    }
  } else {
    m_node = m_node_allocator.allocate_node_for(0, num_elements);
    m_node->m_size = m_num_elements;
    m_node->m_next_free = dummy_partition_node;
    m_free_list = m_node;
  }
}

template <typename NodeAllocator>
void range_partition<NodeAllocator>::reset() {
  if (!small_size() && m_num_elements > 0) {
    while (!m_node->is_last_node()) {
      partition_node* next = m_node->m_next;
      m_node_allocator.free_node(m_node);
      m_node = next;
    }
    m_node_allocator.free_node(m_node);
  }
}

// This will free any and all nodes that have been allocated.
// It is legal to call this while there are still some nodes that
// are unaccounted for, but the contents of those nodes should be
// considered no longer valid.
template <typename NodeAllocator>
range_partition<NodeAllocator>::~range_partition() {
  reset();
}

template <typename NodeAllocator>
range_partition<NodeAllocator>& range_partition<NodeAllocator>::operator=(
    range_partition<NodeAllocator>&& _other) {
  reset();
  m_num_elements = _other.m_num_elements;
  _other.m_num_elements = 0;
  m_used_space = _other.m_used_space;
  m_node_allocator = _other.m_node_allocator;
  m_free_list = _other.m_free_list;
  if (!small_size()) {
    m_node = _other.m_node;
  } else {
    m_used_bucket = _other.m_used_bucket;
  }
  return *this;
}

template <typename NodeAllocator>
inline size_t range_partition<NodeAllocator>::size() const {
  return m_num_elements;
}

template <typename NodeAllocator>
inline size_t range_partition<NodeAllocator>::used() const {
  return m_used_space;
}

template <typename NodeAllocator>
inline void range_partition<NodeAllocator>::release_interval(token token) {
  if (small_size()) {
    m_used_bucket = m_used_bucket & ~token.bits;
    m_used_space -= token.size();
  } else {
    m_used_space -= token.size();
    token.node->m_next_free = m_free_list;
    m_free_list = token.node;
    partition_node* node;
    if (0 != (node = m_free_list->merge_left())) {
      if (!node->m_previous) {
        m_node = m_free_list;
      }
      m_node_allocator.free_node(node);
    }
    if (0 != (node = m_free_list->merge_right())) {
      m_node_allocator.free_node(node);
    }
  }
  token.list = nullptr;
}

template <typename NodeAllocator>
inline typename range_partition<NodeAllocator>::token
range_partition<NodeAllocator>::get_interval(size_t _size) {
  if (small_size()) {
    size_t bit_mask = _size == k_no_allocation_size
                          ? static_cast<size_t>(-1)
                          : (static_cast<size_t>(1) << _size) - 1;
    size_t inv_bucket = ~m_used_bucket;
    size_t offset = math::trailing_zeros(inv_bucket);
    inv_bucket >>= offset;
    while (inv_bucket & ((inv_bucket & bit_mask) != bit_mask)) {
      // We could not fit into the free bits on the end. Remove them
      size_t new_offset = math::trailing_zeros(~inv_bucket);
      inv_bucket >>= new_offset;
      offset += new_offset;
      // We definitely won't be able to fit in the used bits at the end
      // so remove all of those as well.
      new_offset = math::trailing_zeros(inv_bucket);
      inv_bucket >>= new_offset;
      offset += new_offset;
    }
    if (!inv_bucket) {
      return token();
    }
    m_used_bucket |= bit_mask << offset;
    m_used_space += _size;
    return token(this, bit_mask << offset);
  } else {
    partition_node* free_node = m_free_list;
    partition_node* last_free_node = nullptr;
    while (free_node != dummy_partition_node) {
      if (free_node->m_size >= _size) {
        partition_node* new_node = split_at(free_node, _size);
        m_used_space += _size;
        if (new_node != free_node) {
          return token(this, new_node);
        }
        // Otherwise: The node returned WAS the free-node, update free-list.
        if (last_free_node == nullptr) {
          // If we were the first free_node then we need to set the root
          // free-list.
          m_free_list = new_node->m_next_free;
        } else {
          last_free_node->m_next_free = new_node->m_next_free;
        }
        new_node->m_next_free = nullptr;
        return token(this, new_node);
      } else {
        free_node = free_node->m_next_free;
      }
    }
    return token();
  }
}
template <typename NodeAllocator>
inline partition_node* range_partition<NodeAllocator>::split_at(
    partition_node* node, size_t _size) {
  WN_DEBUG_ASSERT(node->m_next_free != nullptr, "Can only split free nodes");
  WN_DEBUG_ASSERT(
      node->m_size >= _size, "Can only split nodes that are large enough");

  if (node->m_size == _size) {
    return node;
  }
  size_t old_offset = node->m_offset;
  node->m_offset += _size;
  node->m_size -= _size;

  m_node_allocator.move_node(
      node->m_size + _size, old_offset, node->m_size, node->m_offset);

  partition_node* new_node =
      m_node_allocator.allocate_node_for(old_offset, _size);
  new_node->m_size = _size;
  new_node->m_offset = old_offset;
  new_node->insert_before(node);
  if (m_node == node) {
    m_node = new_node;
  }
  return new_node;
}

using default_range_partition = range_partition<>;

}  // namespace containers
}  // namespace wn

#endif  // __WN_CONTAINERS__RANGE_PARTITION_H__
