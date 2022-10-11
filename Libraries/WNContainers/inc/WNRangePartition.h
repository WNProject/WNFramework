// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <map>

#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/stl_allocator.h"

#ifndef __WN_CONTAINERS_RANGE_PARTITION_H__
#define __WN_CONTAINERS_RANGE_PARTITION_H__

namespace wn {
namespace containers {

template <typename T_SIZE = size_t>
class range_partition {
  struct range_node;
  using node_map = std::multimap<T_SIZE, range_node*, std::less<T_SIZE>,
      memory::stl_allocator<std::pair<const T_SIZE, range_node*>,
          memory::allocator>>;
  struct range_node {
    T_SIZE size = 0;
    T_SIZE offset = 0;
    T_SIZE aligned_offset = 0;
    range_node* next = nullptr;
    range_node* prev = nullptr;
    typename node_map::iterator map_location;
  };

public:
  range_partition(memory::allocator* _allocator, T_SIZE size);
  ~range_partition();
  class token {
  public:
    token(token&& other);
    token();
    ~token();
    token& operator=(token&& _other);
    T_SIZE offset() const;
    bool is_valid() const;
    T_SIZE size() const;
    void free();

  private:
    token(range_node* node, range_partition* partition);
    range_node* node;
    range_partition* partition;
    friend class range_partition<T_SIZE>;
  };

  token get_interval(T_SIZE size, T_SIZE alignment = 1);
  void free(token* t);
  T_SIZE size() const;
  T_SIZE used() const;

private:
  memory::allocator* m_allocator;
  size_t m_used;
  size_t m_size;

  node_map free_nodes;
};

template <typename T_SIZE>
range_partition<T_SIZE>::range_partition(
    memory::allocator* _allocator, T_SIZE size)
  : m_allocator(_allocator), m_used(0), m_size(size), free_nodes(_allocator) {
  auto node = m_allocator->construct<range_node>();
  node->size = m_size;
  node->map_location = free_nodes.insert(std::make_pair(m_size, node));
}

template <typename T_SIZE>
range_partition<T_SIZE>::~range_partition() {
  WN_DEBUG_ASSERT(free_nodes.size() == 1,
      "You must free all nodes before emptying the range parition");
  m_allocator->destroy(free_nodes.begin()->second);
}

template <typename T_SIZE>
typename range_partition<T_SIZE>::token range_partition<T_SIZE>::get_interval(
    T_SIZE count, T_SIZE alignment) {
  WN_DEBUG_ASSERT(alignment > 0, "The Alignment must be > 0");
  T_SIZE align_m_1 = alignment - 1;
  T_SIZE max_alloc = count + align_m_1;

  auto it = free_nodes.lower_bound(max_alloc);
  if (it == free_nodes.end()) {
    return range_partition<T_SIZE>::token();
  }
  WN_DEBUG_ASSERT(it != free_nodes.end(), "No free nodes");
  auto node = it->second;
  free_nodes.erase(it);

  // Offset to the block + required offset to align correctly.
  auto node_offset = node->offset;
  auto offset_from_base = (node->offset + (align_m_1)) & ~(align_m_1);
  auto offset_in_block = offset_from_base - node->offset;

  T_SIZE total_alloc = max_alloc - (align_m_1 - offset_in_block);
  m_used += total_alloc;
  node->size -= total_alloc;
  node->offset += total_alloc;

  auto new_node = m_allocator->construct<range_node>();
  new_node->prev = node->prev;
  new_node->size = total_alloc;
  new_node->offset = node_offset;
  new_node->aligned_offset = offset_from_base;
  new_node->map_location = free_nodes.end();

  if (new_node->prev) {
    new_node->prev->next = new_node;
  }

  if (node->size > 0) {
    node->map_location = free_nodes.insert(std::make_pair(node->size, node));
    new_node->next = node;
    node->prev = new_node;
  } else {
    if (node->next) {
      new_node->next = node->next;
      new_node->next->prev = new_node;
    }
    m_allocator->destroy(node);
  }
  return token(new_node, this);
}

template <typename T_SIZE>
void range_partition<T_SIZE>::free(range_partition<T_SIZE>::token* t) {
  m_used -= t->node->size;
  while (t->node->prev && t->node->prev->map_location != free_nodes.end()) {
    auto prev = t->node->prev;
    prev->size += t->node->size;
    prev->next = t->node->next;
    if (t->node->next) {
      t->node->next->prev = prev;
    }

    // Erase the previous, because are not done merging,
    // we are going to re-add at the end anyway.
    free_nodes.erase(prev->map_location);
    m_allocator->destroy(t->node);
    t->node = prev;
  }

  while (t->node->next && t->node->next->map_location != free_nodes.end()) {
    auto next = t->node->next;
    t->node->size += next->size;
    t->node->next = next->next;
    if (t->node->next) {
      t->node->next->prev = t->node;
    }
    free_nodes.erase(next->map_location);
    m_allocator->destroy(next);
  }
  t->node->map_location =
      free_nodes.insert(std::make_pair(t->node->size, t->node));
  t->node = nullptr;
  t->partition = nullptr;
}

template <typename T_SIZE>
T_SIZE range_partition<T_SIZE>::used() const {
  return m_used;
}

template <typename T_SIZE>
T_SIZE range_partition<T_SIZE>::size() const {
  return m_size;
}

template <typename T_SIZE>
range_partition<T_SIZE>::token::token(range_partition<T_SIZE>::range_node* node,
    range_partition<T_SIZE>* partition)
  : node(node), partition(partition) {}

template <typename T_SIZE>
T_SIZE range_partition<T_SIZE>::token::offset() const {
  return node->aligned_offset;
}

template <typename T_SIZE>
T_SIZE range_partition<T_SIZE>::token::size() const {
  return node->size - (node->aligned_offset - node->offset);
}

template <typename T_SIZE>
void range_partition<T_SIZE>::token::free() {
  if (is_valid()) {
    partition->free(this);
  }
}

template <typename T_SIZE>
bool range_partition<T_SIZE>::token::is_valid() const {
  return partition && node;
}

template <typename T_SIZE>
range_partition<T_SIZE>::token::token(range_partition<T_SIZE>::token&& other) {
  node = other.node;
  partition = other.partition;
  other.node = nullptr;
  other.partition = nullptr;
}

template <typename T_SIZE>
range_partition<T_SIZE>::token::~token() {
  free();
}

template <typename T_SIZE>
range_partition<T_SIZE>::token::token() {
  node = nullptr;
  partition = nullptr;
}

template <typename T_SIZE>
typename range_partition<T_SIZE>::token&
range_partition<T_SIZE>::token::operator=(token&& other) {
  if (is_valid()) {
    free();
  }
  node = other.node;
  partition = other.partition;
  other.node = nullptr;
  other.partition = nullptr;
  return *this;
}

}  // namespace containers
}  // namespace wn

#endif  // __WN_CONTAINERS_RANGE_PARTITION_H__