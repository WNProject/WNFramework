// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_LIST_H__
#define __WN_CONTAINERS_LIST_H__

#include "WNCore/inc/WNTypes.h"
#include "WNMemory/inc/WNAllocator.h"
#include <utility>
#include <iterator>

namespace wn {
namespace containers {

template <typename _Type, typename _Allocator = memory::default_allocator>
class list;

template <typename _NodeType, typename _ValueType>
struct list_iterator final
  : public std::iterator<std::bidirectional_iterator_tag, _ValueType> {
  typedef _ValueType& reference;
  typedef _ValueType* pointer;
  list_iterator& operator+=(const wn_size_t _amount) {
    for (size_t i = 0; i < _amount; ++i) {
      m_ptr = m_ptr->m_next;
    }
    return (*this);
  }

  list_iterator& operator-=(const wn_size_t _amount) {
    for (size_t i = 0; i < _amount; ++i) {
      m_ptr = m_ptr->m_prev;
    }
    return (*this);
  }

  list_iterator operator+(const wn_size_t amount) {
    list_iterator i(*this);
    i += amount;
    return i;
  }

  list_iterator operator-(const wn_size_t amount) {
    list_iterator i(*this);
    i -= amount;
    return i;
  }

  list_iterator operator++(wn_int32) {
    list_iterator i(*this);
    (*this) += 1;

    return (i);
  }

  list_iterator operator--(wn_int32) {
    list_iterator i(*this);
    (*this) -= 1;

    return (i);
  }

  template <typename _T1, typename _T2>
  wn_bool operator==(const list_iterator<_T1, _T2>& _other) const {
    return (m_ptr == _other.m_ptr);
  }

  template <typename _T1, typename _T2>
  wn_bool operator!=(const list_iterator<_T1, _T2>& _other) const {
    return (m_ptr != _other.m_ptr);
  }

  list_iterator& operator++() { return ((*this) += 1); }
  list_iterator& operator--() { return ((*this) -= 1); }

  reference operator*() const { return m_ptr->m_element; }
  pointer operator->() const { return (&(m_ptr->m_element)); }

  template <typename _T1, typename _T2>
  list_iterator(const list_iterator<_T1, _T2>& _other) {
    m_ptr = _other.m_ptr;
  }

  list_iterator() : m_ptr(wn_nullptr) {}

private:
  _NodeType* m_ptr;
  list_iterator(_NodeType* node) : m_ptr(node) {}

  template <typename _T, typename _A>
  friend class list;
  template <typename _NT, typename _VT>
  friend struct list_iterator;
};


template <typename _Type, typename _Allocator>
class list final {
 public:
  static _Allocator s_default_allocator;

 private:
  struct list_node {
   public:
    template <typename... _Args>
    list_node(_Args&&... _args)
        : m_element(std::forward<_Args>(_args)...) {}
    list_node* m_prev;
    list_node* m_next;
    _Type m_element;
  };

  struct dummy_end_node {
    list_node* m_prev;
    list_node* m_next;
  };

  static_assert(&(reinterpret_cast<dummy_end_node*>(4096)->m_prev) ==
                    &(reinterpret_cast<list_node*>(4096)->m_prev),
                "dummy_end_node cannot masquerade");
  static_assert(&(reinterpret_cast<dummy_end_node*>(4096)->m_next) ==
                    &(reinterpret_cast<list_node*>(4096)->m_next),
                "dummy_end_node cannot masquerade");

 public:
  typedef _Type value_type;
  typedef wn_size_t size_type;
  typedef wn_signed_t difference_type;
  typedef _Allocator allocator_type;
  typedef _Type& reference;
  typedef const _Type& const_reference;
  typedef _Type* pointer;
  typedef const _Type* const_pointer;

 public:
  typedef list_iterator<list_node, _Type> iterator;
  typedef list_iterator<list_node, const _Type> const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  list(memory::allocator* _allocator = &s_default_allocator)
      : m_allocator(_allocator),
        m_size(0),
        m_begin(reinterpret_cast<list_node*>(&m_dummy_end_node)) {
    m_begin->m_prev = wn_nullptr;
    m_begin->m_next = m_begin;
  }
  ~list() { clear(); }

  list(const list& _other) : list(_other.m_allocator) {
    for (auto it = _other.cbegin(); it != _other.cend(); ++it) {
      push_back(*it);
    }
  }

  iterator begin() { return iterator(m_begin); }
  const_iterator cbegin() const { return const_iterator(m_begin); }

  iterator end() {
    return iterator(reinterpret_cast<list_node*>(&m_dummy_end_node));
  }
  const_iterator cend() const {
    return const_iterator(const_cast<list_node*>(
        reinterpret_cast<const list_node*>(&m_dummy_end_node)));
  }

  reverse_iterator rbegin() { return reverse_iterator(end()); }
  reverse_iterator rend() { return reverse_iterator(begin()); }

  const_reverse_iterator crbegin() { return const_reverse_iterator(cend()); }
  const_reverse_iterator crend() { return const_reverse_iterator(cbegin()); }

  const_iterator tbegin(const const_iterator&) const { return cbegin(); }
  iterator tbegin(const iterator&) { return begin(); }

  // tbegin are used to return const/non-const iterators
  // as appropriate. This is so that templated objects,
  // that just have an iterator type can do the right thing.
  // see hash_map_iterator::+= for an example.
  const_iterator tend(const const_iterator&) const { return cend(); }
  iterator tend(const iterator&) { return end(); }

  template <typename _Alloc>
  iterator transfer_to(const_iterator source_it,
                       typename list<_Type, _Alloc>::iterator dest_it,
                       list<_Type, _Alloc>& _other) {
    if (m_allocator == _other.m_allocator) {
      list_node* node = source_it.m_ptr;
      iterator it = unlink(source_it);
      _other.link(dest_it, node);
      return it;
    } else {
      list_node* node = source_it.m_ptr;
      _other.insert(dest_it, std::move(node->m_element));
      return erase(source_it);
    }
  }

  template <typename... _Args>
  iterator emplace(const_iterator _it, _Args&&... _args) {
    list_node* new_node =
        m_allocator->make_allocated<list_node>(std::forward<_Args>(_args)...);
    return (link(_it, new_node));
  }

  iterator insert(const_iterator _it, const _Type& _element) {
    list_node* new_node = m_allocator->make_allocated<list_node>(_element);
    return (link(_it, new_node));
  }

  iterator insert(const_iterator _it, _Type&& _element) {
    list_node* new_node =
        m_allocator->make_allocated<list_node>(std::move(_element));
    return (link(_it, new_node));
  }

  void push_back(const _Type& _element) { insert(end(), _element); }

  void push_back(_Type&& _element) { insert(end(), _element); }

  iterator erase(const_iterator _it) {
    list_node* ptr = _it.m_ptr;
    iterator ret = unlink(_it);
    ptr->m_element.~_Type();
    m_allocator->deallocate(ptr);
    return ret;
  }

  void clear() {
    while (m_begin->m_next != m_begin) {
      erase(iterator(m_begin));
    }
  }

  bool empty() const { return m_size == 0; }

  wn_size_t size() const { return m_size; }

 private:
  iterator unlink(iterator _it) {
    WN_DEBUG_ASSERT_DESC(
        static_cast<void*>(_it.m_ptr) != static_cast<void*>(&m_dummy_end_node),
        "You are trying to delete end()");
    list_node* ptr = _it.m_ptr;
    if (ptr->m_prev) {
      ptr->m_prev->m_next = ptr->m_next;
    } else {
      m_begin = ptr->m_next;
    }
    ptr->m_next->m_prev = ptr->m_prev;
    list_node* next = ptr->m_next;
    m_size -= 1;
    return iterator(next);
  }
  iterator link(iterator _it, list_node* _node) {
    // m_end->prev == nullptr if there is nothing allocated yet.
    if (_it.m_ptr->m_prev) {
      _node->m_prev = _it.m_ptr->m_prev;
      _it.m_ptr->m_prev->m_next = _node;
    } else {
      m_begin = _node;
      _node->m_prev = 0;
    }
    // Guaranteed to have a ->next. end()->next == end(), so this will
    // insert at the end.
    _it.m_ptr->m_prev = _node;
    _node->m_next = _it.m_ptr;
    m_size += 1;
    return iterator(_node);
  }

  memory::allocator* m_allocator;
  dummy_end_node m_dummy_end_node;
  wn_size_t m_size;
  list_node* m_begin;
};

template <typename _Type, typename _Allocator>
_Allocator list<_Type, _Allocator>::s_default_allocator;
}
}

#endif  //_WN_CONTAINERS_LIST_H_