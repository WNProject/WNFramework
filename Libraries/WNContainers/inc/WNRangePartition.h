// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_RANGE_PARTITION_H__
#define __WN_CONTAINERS_RANGE_PARTITION_H__

#include "WNMath/inc/WNBasic.h"
#include "WNMemory/inc/allocator.h"

namespace wn {
namespace containers {

template <typename T_SIZE>
struct partition_node;

template <typename T_SIZE>
class default_node_allocator {
public:
  default_node_allocator() : m_allocator(nullptr) {}
  default_node_allocator(memory::allocator* _allocator)
    : m_allocator(_allocator) {}
  partition_node<T_SIZE>* allocate_node_for(T_SIZE, T_SIZE);
  void free_node(partition_node<T_SIZE>* node);
  void move_node(T_SIZE, T_SIZE, T_SIZE, T_SIZE) {}

private:
  memory::allocator* m_allocator;
};

template <typename NodeAllocator = default_node_allocator<size_t>,
    typename T_SIZE = size_t>
class range_partition {
private:
  // TODO(awoloszyn): If our small-type optimization only misses
  // by a small bit most of the time, consider expanding it.
  static const T_SIZE k_no_allocation_size = sizeof(T_SIZE) * 8;
  inline bool small_size() const {
    return m_num_elements <= k_no_allocation_size;
  }
  partition_node<T_SIZE>* const dummy_partition_node =
      reinterpret_cast<partition_node<T_SIZE>*>(0x1);
  friend struct token;

public:
  struct token : core::non_copyable {
  public:
    T_SIZE size() const;
    T_SIZE offset() const;
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
    template <typename A, typename S>
    friend class range_partition;
    token(range_partition<NodeAllocator, T_SIZE>* _list,
        partition_node<T_SIZE>* _node)
      : list(_list), node(_node) {}
    token(range_partition<NodeAllocator, T_SIZE>* _list, T_SIZE _bits)
      : list(_list), bits(_bits) {}

    range_partition<NodeAllocator, T_SIZE>* list;

    union {
      partition_node<T_SIZE>* node;
      T_SIZE bits;
    };
  };

  range_partition(const NodeAllocator& _node_allocator, T_SIZE num_elements);

  range_partition() : m_num_elements(0) {}

  // This will free any and all nodes that have been allocated.
  // It is legal to call this while there are still some nodes that
  // are unaccounted for, but the contents of those nodes should be
  // considered no longer valid.
  ~range_partition();

  T_SIZE size() const;

  T_SIZE used() const;

  void release_interval(token token);

  token get_interval(T_SIZE size);
  token get_aligned_interval(T_SIZE size, T_SIZE alignment);
  range_partition& operator=(range_partition&& _other);

private:
  void reset();
  // Returns a node that contains _size elements.
  // Splits the given node if it is larger than _size elements,
  // returns the entire node if it is exactly _size elements.
  // It is an error to call this on a node with fewer than _size elements.
  partition_node<T_SIZE>* split_at(partition_node<T_SIZE>* node, T_SIZE _size);

  T_SIZE m_num_elements;
  T_SIZE m_used_space;
  union {
    partition_node<T_SIZE>* m_node;
    T_SIZE m_used_bucket;
  };
  partition_node<T_SIZE>* m_free_list;
  NodeAllocator m_node_allocator;
};

template <typename T_SIZE>
struct partition_node {
public:
  T_SIZE size() const {
    return m_size;
  }
  T_SIZE offset() const {
    return m_offset;
  }

private:
  template <typename A, typename S>
  friend class range_partition;
  partition_node<T_SIZE>* m_next = nullptr;
  partition_node<T_SIZE>* m_previous = nullptr;
  partition_node<T_SIZE>* m_next_free = nullptr;
  T_SIZE m_offset = 0;
  T_SIZE m_size = 0;

  bool is_first_node() const {
    return m_previous == nullptr;
  }
  bool is_last_node() const {
    return m_next == nullptr;
  }
  bool is_only_node() const {
    return (m_previous == m_next) && m_previous == nullptr;
  }

  void insert_before(partition_node<T_SIZE>* _next) {
    if (_next) {
      insert_between(_next->m_previous, _next);
    }
  }

  void insert_after(partition_node<T_SIZE>* _previous) {
    if (_previous) {
      insert_between(_previous, _previous->m_next);
    }
  }

  void insert_between(
      partition_node<T_SIZE>* _previous, partition_node<T_SIZE>* _next) {
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

  inline partition_node<T_SIZE>* merge_left_and_free() {
    WN_RELEASE_ASSERT(!m_next_free, "Cannot merge an already free node");
    if (m_previous && m_previous->m_next_free) {
      WN_DEBUG_ASSERT(m_previous->m_next == this, "This should be impossible");
      m_previous->m_size += m_size;
      m_previous->m_next = m_next;
      if (m_next) {
        m_next->m_previous = m_previous;
      }
      return m_previous;
    }
    return nullptr;
  }

  partition_node<T_SIZE>* merge_right_and_free() {
    WN_RELEASE_ASSERT(!m_next_free, "Cannot merge an already free node");
    if (m_next && m_next->m_next_free) {
      WN_DEBUG_ASSERT(m_next->m_previous == this, "This should be impossible");
      m_next->m_size += m_size;
      m_next->m_offset = m_offset;
      m_next->m_previous = m_previous;
      if (m_previous) {
        m_previous->m_next = m_next;
      }
      return m_next;
    }
    return nullptr;
  }
};

template <typename T_SIZE>
inline partition_node<T_SIZE>*
default_node_allocator<T_SIZE>::allocate_node_for(T_SIZE, T_SIZE) {
  return m_allocator->construct<partition_node<T_SIZE>>();
}

template <typename T_SIZE>
inline void default_node_allocator<T_SIZE>::free_node(
    partition_node<T_SIZE>* node) {
  m_allocator->destroy(node);
}

template <typename NodeAllocator, typename T_SIZE>
inline T_SIZE range_partition<NodeAllocator, T_SIZE>::token::size() const {
  return list->small_size() ? math::popcount_sparse(bits) : node->size();
}
template <typename NodeAllocator, typename T_SIZE>
inline T_SIZE range_partition<NodeAllocator, T_SIZE>::token::offset() const {
  return list->small_size() ? math::trailing_zeros(static_cast<uint64_t>(bits))
                            : node->offset();
}
template <typename NodeAllocator, typename T_SIZE>
inline bool range_partition<NodeAllocator, T_SIZE>::token::is_valid() const {
  return list && (node || bits);
}

template <typename NodeAllocator, typename T_SIZE>
range_partition<NodeAllocator, T_SIZE>::range_partition(
    const NodeAllocator& _node_allocator, T_SIZE num_elements)
  : m_num_elements(num_elements),
    m_used_space(0),
    m_free_list(dummy_partition_node),
    m_node_allocator(_node_allocator) {
  if (small_size()) {
    if (num_elements == k_no_allocation_size) {
      m_used_bucket = 0;
    } else {
      // Mask off all of the buckets that we cannot use.
      m_used_bucket = ~((static_cast<T_SIZE>(1) << num_elements) - 1);
    }
  } else {
    m_node = m_node_allocator.allocate_node_for(0, num_elements);
    m_node->m_size = m_num_elements;
    m_node->m_next_free = dummy_partition_node;
    m_free_list = m_node;
  }
}

template <typename NodeAllocator, typename T_SIZE>
void range_partition<NodeAllocator, T_SIZE>::reset() {
  if (!small_size() && m_num_elements > 0) {
    while (!m_node->is_last_node()) {
      partition_node<T_SIZE>* next = m_node->m_next;
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
template <typename NodeAllocator, typename T_SIZE>
range_partition<NodeAllocator, T_SIZE>::~range_partition() {
  reset();
}

template <typename NodeAllocator, typename T_SIZE>
range_partition<NodeAllocator, T_SIZE>&
range_partition<NodeAllocator, T_SIZE>::operator=(
    range_partition<NodeAllocator, T_SIZE>&& _other) {
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

template <typename NodeAllocator, typename T_SIZE>
inline T_SIZE range_partition<NodeAllocator, T_SIZE>::size() const {
  return m_num_elements;
}

template <typename NodeAllocator, typename T_SIZE>
inline T_SIZE range_partition<NodeAllocator, T_SIZE>::used() const {
  return m_used_space;
}

template <typename NodeAllocator, typename T_SIZE>
inline void range_partition<NodeAllocator, T_SIZE>::release_interval(
    token token) {
  if (small_size()) {
    m_used_bucket = m_used_bucket & ~token.bits;
    m_used_space -= token.size();
  } else {
    m_used_space -= token.size();
    partition_node<T_SIZE>* n = token.node->merge_left_and_free();
    if (n) {
      m_node_allocator.free_node(token.node);
      if (n->m_next && n->m_next->m_next_free) {
        // We want to merge right, but this is the slow path.
        // We have to remove this node from the free-list.
        partition_node<T_SIZE>* free = m_free_list;
        if (free == n) {
          // free is the first, removal is trivial.
          m_free_list = n->m_next_free;
          n->m_next_free = nullptr;
        } else {
          // Boo linear scan over the list.
          // Maybe switch this to be a doubly-linked list.
          while (free->m_next_free != dummy_partition_node) {
            if (free->m_next_free == n) {
              free->m_next_free = n->m_next_free;
              // This makes sure we will hit an assert
              // if this was not hit, in merge_right_and_free;
              n->m_next_free = nullptr;
              break;
            }
            free = free->m_next_free;
          }
        }

        partition_node<T_SIZE>* new_node = n->merge_right_and_free();
        WN_RELEASE_ASSERT(
            new_node, "This has to be mergable, we already checked");
        m_node_allocator.free_node(n);
        if (m_node == n) {
          m_node = new_node;
        }
      }
    } else if (nullptr != (n = token.node->merge_right_and_free())) {
      m_node_allocator.free_node(token.node);
      if (m_node == token.node) {
        m_node = n;
      }
    } else {
      token.node->m_next_free = m_free_list;
      m_free_list = token.node;
    }
  }
  token.list = nullptr;
}

template <typename NodeAllocator, typename T_SIZE>
inline typename range_partition<NodeAllocator, T_SIZE>::token
range_partition<NodeAllocator, T_SIZE>::get_aligned_interval(
    T_SIZE _size, T_SIZE _alignment) {
  WN_DEBUG_ASSERT(_size != 0, "Size must be > 0");
  WN_DEBUG_ASSERT(_alignment > 0 && (_alignment & (_alignment - 1)) == 0,
      "Alignment must be a power of 2");
  if (small_size()) {
    T_SIZE bit_mask = _size == k_no_allocation_size
                          ? static_cast<T_SIZE>(-1)
                          : (static_cast<T_SIZE>(1) << _size) - 1;
    T_SIZE inv_bucket = ~m_used_bucket;
    T_SIZE offset = math::trailing_zeros(static_cast<uint64_t>(inv_bucket));
    inv_bucket >>= offset;

    while (inv_bucket) {
      while (inv_bucket & ((inv_bucket & bit_mask) != bit_mask)) {
        // We could not fit into the free bits on the end. Remove them
        T_SIZE new_offset =
            math::trailing_zeros(static_cast<uint64_t>(~inv_bucket));
        inv_bucket >>= new_offset;
        offset += new_offset;
        // We definitely won't be able to fit in the used bits at the end
        // so remove all of those as well.
        new_offset = math::trailing_zeros(static_cast<uint64_t>(inv_bucket));
        inv_bucket >>= new_offset;
        offset += new_offset;
      }
      if (!inv_bucket) {
        return token();
      }
      if ((offset & (_alignment - 1)) == 0) {
        break;
      }
      offset += 1;
      inv_bucket >>= 1;
    }
    if (!inv_bucket) {
      return token();
    }

    m_used_bucket |= bit_mask << offset;
    m_used_space += _size;
    return token(this, static_cast<T_SIZE>(bit_mask << offset));
  } else {
    partition_node<T_SIZE>* free_node = m_free_list;
    partition_node<T_SIZE>* last_free_node = nullptr;
    while (free_node != dummy_partition_node) {
      if (free_node->m_size >= _size) {
        // We found a node that can fit us, but first lets also check the
        // alignment
        T_SIZE node_offset = free_node->m_offset & (_alignment - 1);
        if (node_offset != 0) {
          node_offset = _alignment - node_offset;
        }
        if (free_node->m_size < node_offset + _size) {
          last_free_node = free_node;
          free_node = free_node->m_next_free;
          continue;
        }
        // If we had to chop some of this off for alignment reasons:
        if (node_offset != 0) {
          partition_node<T_SIZE>* temp = split_at(free_node, node_offset);
          temp->m_next_free = m_free_list;
          m_free_list = temp;
        }
        partition_node<T_SIZE>* new_node = split_at(free_node, _size);
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
      }
      last_free_node = free_node;
      free_node = free_node->m_next_free;
    }
    return token();
  }
}

template <typename NodeAllocator, typename T_SIZE>
inline typename range_partition<NodeAllocator, T_SIZE>::token
range_partition<NodeAllocator, T_SIZE>::get_interval(T_SIZE _size) {
  if (small_size()) {
    T_SIZE bit_mask = _size == k_no_allocation_size
                          ? static_cast<T_SIZE>(-1)
                          : (static_cast<T_SIZE>(1) << _size) - 1;
    T_SIZE inv_bucket = ~m_used_bucket;
    T_SIZE offset = math::trailing_zeros(static_cast<uint64_t>(inv_bucket));
    inv_bucket >>= offset;
    while (inv_bucket & ((inv_bucket & bit_mask) != bit_mask)) {
      // We could not fit into the free bits on the end. Remove them
      T_SIZE new_offset =
          math::trailing_zeros(static_cast<uint64_t>(~inv_bucket));
      inv_bucket >>= new_offset;
      offset += new_offset;
      // We definitely won't be able to fit in the used bits at the end
      // so remove all of those as well.
      new_offset = math::trailing_zeros(static_cast<uint64_t>(inv_bucket));
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
    partition_node<T_SIZE>* free_node = m_free_list;
    partition_node<T_SIZE>* last_free_node = nullptr;
    while (free_node != dummy_partition_node) {
      if (free_node->m_size >= _size) {
        partition_node<T_SIZE>* new_node = split_at(free_node, _size);
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

template <typename NodeAllocator, typename T_SIZE>
inline partition_node<T_SIZE>* range_partition<NodeAllocator, T_SIZE>::split_at(
    partition_node<T_SIZE>* node, T_SIZE _size) {
  WN_DEBUG_ASSERT(node->m_next_free != nullptr, "Can only split free nodes");
  WN_DEBUG_ASSERT(
      node->m_size >= _size, "Can only split nodes that are large enough");

  if (node->m_size == _size) {
    return node;
  }
  T_SIZE old_offset = node->m_offset;
  node->m_offset += _size;
  node->m_size -= _size;

  m_node_allocator.move_node(
      node->m_size + _size, old_offset, node->m_size, node->m_offset);

  partition_node<T_SIZE>* new_node =
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
