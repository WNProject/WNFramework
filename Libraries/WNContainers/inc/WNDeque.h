// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CONTAINERS_DEQUE_H__
#define __WN_CONTAINERS_DEQUE_H__

#include "WNCore/inc/WNTypeTraits.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNMemory/inc/WNBasic.h"
#include "WNContainers/inc/WNDynamicArray.h"

#include <iterator>

namespace wn {
namespace containers {

template <typename _Type, typename _Allocator, const wn_size_t _BlockSize>
class deque;

namespace internal {

template <typename _Container, typename _NonConstContainer = _Container,
    typename _Element = typename _Container::value_type>
class deque_iterator final
    : public std::iterator<std::random_access_iterator_tag, _Element,
          typename _Container::difference_type> {
private:
  using base = std::iterator<std::random_access_iterator_tag, _Element,
      typename _Container::difference_type>;

public:
  using iterator_category = typename base::iterator_category;
  using value_type = typename base::value_type;
  using size_type = typename _Container::size_type;
  using difference_type = typename base::difference_type;
  using pointer = typename base::pointer;
  using reference = typename base::reference;

  deque_iterator() : m_deque(wn_nullptr), m_element(0) {}

  deque_iterator(deque_iterator&& _other)
    : m_deque(std::move(_other.m_deque)),
      m_element(std::move(_other.m_element)) {
    _other.clear();
  }

  deque_iterator(const deque_iterator& _other)
    : m_deque(_other.m_deque), m_element(_other.m_element) {}

  template <typename _OtherContainer = _NonConstContainer>
  deque_iterator(deque_iterator<_OtherContainer, _OtherContainer,
                     typename _OtherContainer::value_type>&& _other,
      typename core::enable_if<!core::is_same<_Container,
          _OtherContainer>::value>::type* = wn_nullptr)
    : m_deque(std::move(_other.m_deque)),
      m_element(std::move(_other.m_element)) {
    _other.clear();
  }

  template <typename _OtherContainer = _NonConstContainer>
  deque_iterator(const deque_iterator<_OtherContainer, _OtherContainer,
                     typename _OtherContainer::value_type>& _other,
      typename core::enable_if<!core::is_same<_Container,
          _OtherContainer>::value>::type* = wn_nullptr)
    : m_deque(_other.m_deque), m_element(_other.m_element) {}

  deque_iterator& operator=(deque_iterator&& _other) {
    m_deque = std::move(_other.m_deque);
    m_element = std::move(_other.m_element);

    _other.clear();

    return (*this);
  }

  deque_iterator& operator=(const deque_iterator& _other) {
    m_deque = _other.m_deque;
    m_element = _other.m_element;

    return (*this);
  }

  template <typename _OtherContainer = _NonConstContainer>
  typename core::enable_if<!core::is_same<_Container, _OtherContainer>::value,
      deque_iterator>::type&
  operator=(deque_iterator<_OtherContainer, _OtherContainer,
      typename _OtherContainer::value_type>&& _other) {
    m_deque = std::move(_other.m_deque);
    m_element = std::move(_other.m_element);

    _other.clear();

    return (*this);
  }

  template <typename _OtherContainer = _NonConstContainer>
  typename core::enable_if<!core::is_same<_Container, _OtherContainer>::value,
      deque_iterator>::type&
  operator=(const deque_iterator<_OtherContainer, _OtherContainer,
      typename _OtherContainer::value_type>& _other) {
    m_deque = _other.m_deque;
    m_element = _other.m_element;

    return (*this);
  }

  difference_type operator-(const deque_iterator& _other) const {
    WN_RELEASE_ASSERT_DESC(
        m_deque == _other.m_deque, "iterators are incompatible");

    return (m_element - _other.m_element);
  }

  deque_iterator& operator+=(const wn_size_t _amount) {
    m_element += _amount;

    return (*this);
  }

  deque_iterator& operator-=(const wn_size_t _amount) {
    m_element -= _amount;

    return (*this);
  }

  deque_iterator operator+(const wn_size_t _amount) const {
    deque_iterator i(*this);

    return (i += _amount);
  }

  deque_iterator operator-(const wn_size_t _amount) const {
    deque_iterator i(*this);

    return (i -= _amount);
  }

  reference operator*() const {
    return ((*m_deque)[m_element]);
  }

  pointer operator->() const {
    return (&((*m_deque)[m_element]));
  }

  deque_iterator operator++(wn_int32) {
    deque_iterator i(*this);

    (*this) += 1;

    return (i);
  }

  deque_iterator operator--(wn_int32) {
    deque_iterator i(*this);

    (*this) -= 1;

    return (i);
  }

  deque_iterator& operator++() {
    return ((*this) += 1);
  }

  deque_iterator& operator--() {
    return ((*this) -= 1);
  }

  wn_bool operator==(const deque_iterator& _other) const {
    WN_RELEASE_ASSERT_DESC(
        m_deque == _other.m_deque, "iterators are incompatible");

    return (m_element == _other.m_element);
  }

  wn_bool operator!=(const deque_iterator& _other) const {
    WN_RELEASE_ASSERT_DESC(
        m_deque == _other.m_deque, "iterators are incompatible");

    return (m_element != _other.m_element);
  }

  wn_bool operator>(const deque_iterator& _other) const {
    WN_RELEASE_ASSERT_DESC(
        m_deque == _other.m_deque, "iterators are incompatible");

    return (m_element > _other.m_element);
  }

  wn_bool operator>=(const deque_iterator& _other) const {
    WN_RELEASE_ASSERT_DESC(
        m_deque == _other.m_deque, "iterators are incompatible");

    return (m_element >= _other.m_element);
  }

  wn_bool operator<(const deque_iterator& _other) const {
    WN_RELEASE_ASSERT_DESC(
        m_deque == _other.m_deque, "iterators are incompatible");

    return (m_element < _other.m_element);
  }

  wn_bool operator<=(const deque_iterator& _other) const {
    WN_RELEASE_ASSERT_DESC(
        m_deque == _other.m_deque, "iterators are incompatible");

    return (m_element <= _other.m_element);
  }

private:
  template <typename _Type, typename _Allocator, const wn_size_t _BlockSize>
  friend class wn::containers::deque;

  friend class deque_iterator<const _Container, _Container, const _Element>;

  explicit deque_iterator(_Container* _deque, const size_type _element)
    : m_deque(_deque), m_element(_element) {}

  wn_void clear() {
    m_deque = wn_nullptr;
    m_element = 0;
  }

  _Container* m_deque;
  size_type m_element;
};

}  // namespace internal

template <typename _Type, typename _Allocator = memory::default_allocator,
    const wn_size_t _BlockSize = 10>
class deque final {
public:
  static _Allocator s_default_allocator;
  using value_type = _Type;
  using size_type = wn_size_t;
  using difference_type = wn_signed_t;
  using allocator_type = _Allocator;
  using reference = value_type&;
  using const_reference = const value_type&;

private:
  using self_type = deque<_Type, _Allocator, _BlockSize>;

public:
 using iterator = internal::deque_iterator<self_type>;
 using const_iterator =
     internal::deque_iterator<const self_type, self_type, const value_type>;
 using reverse_iterator = std::reverse_iterator<iterator>;
 using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  deque() : deque(&s_default_allocator) {}

  deque(const deque& _other)
    : m_allocator(&s_default_allocator),
      m_block_list(_other.m_block_list, &s_default_allocator),
      m_used_blocks(_other.m_used_blocks),
      m_start_block(_other.m_start_block),
      m_start_location(_other.m_start_location),
      m_allocated_blocks(_other.m_allocated_blocks),
      m_element_count(_other.m_element_count) {}

  deque(const deque& _other, memory::allocator* _allocator)
    : m_allocator(_allocator),
      m_block_list(_other.m_block_list, _allocator),
      m_used_blocks(_other.m_used_blocks),
      m_start_block(_other.m_start_block),
      m_start_location(_other.m_start_location),
      m_allocated_blocks(_other.m_allocated_blocks),
      m_element_count(_other.m_element_count) {}

  deque(deque&& _other)
    : m_allocator(std::move(_other.m_allocator)),
      m_block_list(std::move(_other.m_block_list)),
      m_used_blocks(std::move(_other.m_used_blocks)),
      m_start_block(std::move(_other.m_start_block)),
      m_start_location(std::move(_other.m_start_location)),
      m_allocated_blocks(std::move(_other.m_allocated_blocks)),
      m_element_count(std::move(_other.m_element_count)) {
    _other.m_used_blocks = 0;
    _other.m_start_block = 0;
    _other.m_start_location = 0;
    _other.m_allocated_blocks = 0;
    _other.m_element_count = 0;
  }

  explicit deque(memory::allocator* _allocator)
    : m_allocator(_allocator),
      m_block_list(_allocator),
      m_used_blocks(0),
      m_start_block(0),
      m_start_location(0),
      m_allocated_blocks(0),
      m_element_count(0) {}

  explicit deque(const size_type _count,
      memory::allocator* _allocator = &s_default_allocator)
    : deque(_count, _Type(), _allocator) {}

  deque(const size_type _count, const _Type& _value,
      memory::allocator* _allocator = &s_default_allocator)
    : deque(_allocator) {
    resize(_count, _value);
  }

  template <typename _InputIt,
      typename = core::enable_if_t<!std::is_integral<_InputIt>::value>>
  deque(_InputIt _first, _InputIt _last,
      memory::allocator* _allocator = &s_default_allocator)
    : deque(_allocator) {
    insert(cbegin(), _first, _last);
  }

  deque(std::initializer_list<_Type> _initializer_list,
      memory::allocator* _allocator = &s_default_allocator)
    : deque(_initializer_list.begin(), _initializer_list.end(), _allocator) {}

  deque& operator=(deque&& _other) {
    clear();
    m_allocator = std::move(_other.m_allocator);
    m_block_list = std::move(_other.m_block_list);
    m_used_blocks = std::move(_other.m_used_blocks);
    m_start_block = std::move(_other.m_start_block);
    m_start_location = std::move(_other.m_start_location);
    m_allocated_blocks = std::move(_other.m_allocated_blocks);
    m_element_count = std::move(_other.m_element_count);
    _other.m_used_blocks = 0;
    _other.m_start_block = 0;
    _other.m_start_location = 0;
    _other.m_allocated_blocks = 0;
    _other.m_element_count = 0;
    return *this;
  }

  ~deque() {
    clear();

    for (wn_size_t i = 0; i < m_allocated_blocks; ++i) {
      const wn_size_t index = (m_start_block + i) % m_block_list.size();

      m_allocator->deallocate(m_block_list[index]);
    }
  }

  // element access

  reference operator[](const size_type _pos) {
    return (at(_pos));
  }

  const_reference operator[](const size_type _pos) const {
    return (at(_pos));
  }

  reference front() {
    return (*begin());
  }

  const_reference front() const {
    return (*cbegin());
  }

  reference back() {
    iterator i = end();

    --i;

    return (*i);
  }

  const_reference back() const {
    const_reference i = cend();

    --i;

    return (*i);
  }

  reference at(const size_type _pos) {
    size_type position = m_start_location + _pos;
    size_type block = position / _BlockSize;

    block = (block + m_start_block) % m_block_list.size();
    position %= _BlockSize;

    return (m_block_list[block][position]);
  }

  const_reference at(const size_type _pos) const {
    size_type position = m_start_location + _pos;
    size_type block = position / _BlockSize;

    block = (block + m_start_block) % m_block_list.size();
    position %= _BlockSize;
    return (m_block_list[block][position]);
  }

  // iterators

  iterator begin() {
    return (iterator(this, 0));
  }

  const_iterator begin() const {
    return (cbegin());
  }

  const_iterator cbegin() const {
    return (const_iterator(this, 0));
  }

  iterator end() {
    return (iterator(this, m_element_count));
  }

  const_iterator end() const {
    return (cend());
  }

  const_iterator cend() const {
    return (const_iterator(this, m_element_count));
  }

  reverse_iterator rbegin() {
    return (reverse_iterator(end()));
  }

  const_reverse_iterator rbegin() const {
    return (crbegin());
  }

  const_reverse_iterator crbegin() const {
    return (const_reverse_iterator(cend()));
  }

  reverse_iterator rend() {
    return (reverse_iterator(begin()));
  }

  const_reverse_iterator rend() const {
    return (crend());
  }

  const_reverse_iterator crend() const {
    return (const_reverse_iterator(cbegin()));
  }

  // capacity

  wn_bool empty() const {
    return (size() == 0);
  }

  size_type size() const {
    return (m_element_count);
  }

  size_type capacity() const {
    return (m_allocated_blocks * _BlockSize);
  }

  // modifiers

  wn_void clear() {
    erase(begin(), end());
  }

  iterator insert(const_iterator _pos, _Type&& _value) {
    iterator iter = allocate(_pos, 1);
    iterator newIter = iter;

    memory::construct_at<_Type>(&(*(newIter++)), std::move(_value));

    return (iter);
  }

  iterator insert(const_iterator _pos, const _Type& _value) {
    _Type value(_value);

    return (insert(_pos, std::move(value)));
  }

  iterator insert(
      const_iterator _pos, const size_type _count, const _Type& _value) {
    iterator iter = allocate(_pos, _count);
    iterator newIter = iter;

    for (wn_size_t i = 0; i < _count; ++i) {
      memory::construct_at<_Type>(&(*(newIter++)), _value);
    }

    return (iter);
  }

  template <typename _InputIt,
      typename = core::enable_if_t<!std::is_integral<_InputIt>::value>>
  iterator insert(const_iterator _pos, _InputIt _first, _InputIt _last) {
    const difference_type count = _last - _first;
    iterator position = allocate(_pos, count);
    iterator new_position = position;

    for (; _first != _last; ++_first) {
      memory::construct_at<_Type>(&(*(position++)), *_first);
    }

    return (new_position);
  }

  iterator insert(
      const_iterator _pos, std::initializer_list<_Type> _initializer_list) {
    return (insert(_pos, _initializer_list.begin(), _initializer_list.end()));
  }

  template <typename _Function, typename = core::enable_if_t<core::is_callable<
                                    _Function, _Type(size_type)>::value>>
  iterator insert(
      const_iterator _pos, const size_type _count, _Function&& _generator) {
    iterator position = allocate(_pos, _count);
    iterator new_position = position;

    for (size_type i = 0; i < _count; ++i) {
      memory::construct_at<_Type>(&(*(position++)), std::move(_generator(i++)));
    }

    return (new_position);
  }

  iterator erase(const_iterator _pos) {
    return (erase(_pos, 1));
  }

  iterator erase(const_iterator _pos, const size_type _count) {
    iterator pos = make_iterator(_pos);
    iterator erase_start = pos;

    for (size_type i = 0; i < _count; ++i) {
      (*(erase_start++)).~_Type();
    }

    if (closer_to_front(pos)) {
      iterator copy_to = pos + _count - 1;
      iterator copy_from = pos - 1;
      const size_type copy_count = pos - begin();

      for (size_type i = 0; i < copy_count; ++i) {
        memory::construct_at<_Type>(&(*copy_to), std::move(*copy_from));

        (*copy_from).~_Type();

        --copy_to;
        --copy_from;
      }

      remove_front(_count);

      return (copy_to);
    } else {
      iterator copy_to = pos;
      iterator copy_from = copy_to + _count;
      const size_type location = (pos - begin());
      const size_type copy_count = m_element_count - location;

      for (size_type i = 0; i < copy_count; ++i) {
        memory::construct_at<_Type>(&(*copy_to), std::move(*copy_from));

        (*copy_from).~_Type();

        ++copy_to;
        ++copy_from;
      }

      remove_back(_count);

      return (copy_from);
    }
  }
  iterator erase(const_iterator _first, const_iterator _last) {
    const size_type count = (_last - _first);
    return (erase(_first, count));
  }

  template <typename... Args>
  wn_void emplace_front(Args&&... _args) {
    iterator iter = allocate(cbegin(), 1);
    iterator newIter = iter;

    memory::construct_at<_Type>(&(*(newIter++)), std::forward<Args>(_args)...);
  }
  wn_void push_front(_Type&& _value) {
    insert(cbegin(), std::move(_value));
  }

  wn_void push_front(const _Type& _value) {
    _Type value(_value);

    push_front(std::move(value));
  }

  template <typename... Args>
  wn_void emplace_back(Args&&... _args) {
    iterator iter = allocate(cbegin() + m_element_count, 1);
    iterator newIter = iter;

    memory::construct_at<_Type>(&(*(newIter++)), std::forward<Args>(_args)...);
  }

  wn_void push_back(_Type&& _value) {
    insert(cbegin() + m_element_count, std::move(_value));
  }

  wn_void push_back(const _Type& _value) {
    _Type value(_value);
    push_back(std::move(value));
  }

  wn_void pop_front() {
    erase(begin(), 1);
  }

  wn_void pop_back() {
    erase(end() - 1, 1);
  }

  wn_void resize(const size_type _count) {
    resize(_count, _Type());
  }

  wn_void resize(const size_type _count, const value_type& _value) {
    const size_type current_size = size();
    if (_count != current_size) {
      if (_count > current_size) {
        const size_type difference = _count - current_size;

        insert(cend(), difference, _value);
      } else {
        erase(cbegin() + _count, cend());
      }
    }
  }

  wn_void swap(deque& _other) {
    if (&_other != this) {
      m_block_list.swap(_other.m_block_list);

      std::swap(m_allocator, _other.m_allocator);
      std::swap(m_used_blocks, _other.m_used_blocks);
      std::swap(m_allocated_blocks, _other.m_allocated_blocks);
      std::swap(m_start_location, _other.m_start_location);
      std::swap(m_element_count, _other.m_element_count);
      std::swap(m_start_block, _other.m_start_block);
    }
  }

private:
  iterator make_iterator(const_iterator _pos) {
    return (begin() + (_pos - cbegin()));
  }

  iterator allocate(const_iterator _pos, const size_type _count) {
    if (closer_to_front(_pos)) {
      const size_type location = (_pos - cbegin());

      allocate_front(_count);

      iterator copy_to = begin();
      iterator copy_from = copy_to + _count;

      for (size_type i = 0; i < location; ++i) {
        memory::construct_at<_Type>(&(*copy_to), std::move(*copy_from));

        (*copy_from).~value_type();

        ++copy_to;
        ++copy_from;
      }

      return (copy_to);
    } else {
      const size_type location = (_pos - cbegin());
      const size_type elements_to_end = m_element_count - location;

      allocate_back(_count);

      iterator copy_to = end() - 1;
      iterator copy_from = begin() + location + elements_to_end - 1;

      for (size_type i = 0; i < elements_to_end; ++i) {
        memory::construct_at<_Type>(&(*copy_to), std::move(*copy_from));

        (*copy_from).~value_type();

        --copy_to;
        --copy_from;
      }

      return (begin() + location);
    }
  }

  wn_void allocate_front(const size_type _count) {
    const size_type leftovers_in_block = m_start_location;

    if (leftovers_in_block < _count) {
      size_type accountedElements = leftovers_in_block;
      size_type additional_blocks = (_count - accountedElements);

      additional_blocks = (additional_blocks + _BlockSize) / _BlockSize;

      size_type haveBlocks = total_unused_blocks();

      if (haveBlocks < additional_blocks) {
        const size_type neededBlocks = additional_blocks - haveBlocks;

        if (m_block_list.size() - m_allocated_blocks < neededBlocks) {
          add_block_space(
              m_allocated_blocks + neededBlocks - m_block_list.size());
        }

        for (size_type i = 0; i < neededBlocks; ++i) {
          wn::memory::allocation_pair p =
              m_allocator->allocate(sizeof(value_type), _BlockSize);
          m_block_list[(m_start_block + m_allocated_blocks) %
                       m_block_list.size()] =
              static_cast<value_type*>(p.m_location);
          m_allocated_blocks += 1;
        }
      }

      for (size_type i = 0; i < additional_blocks; ++i) {
        if (m_start_block == 0) {
          m_start_block = m_block_list.size() - 1;
        } else {
          m_start_block -= 1;
        }

        m_used_blocks += 1;

        if (m_block_list.size() != m_allocated_blocks) {
          m_block_list[m_start_block] =
              m_block_list[(m_start_block + m_allocated_blocks) %
                           m_block_list.size()];
          m_block_list[(m_start_block + m_allocated_blocks) %
                       m_block_list.size()] = wn_nullptr;
        }

        accountedElements += _BlockSize;
      }
      m_start_location = accountedElements - _count;
    } else {
      m_start_location -=
          leftovers_in_block < _count ? leftovers_in_block : _count;
    }

    m_element_count += _count;
  }

  wn_void allocate_back(const size_type _count) {
    const size_type unused_blocks = total_unused_blocks();
    const size_type extra_last_elements = total_extra_elements_in_last_block();
    const size_type totalElements =
        (extra_last_elements + unused_blocks * _BlockSize);

    if (totalElements > _count) {
      size_type extraElements = _count;

      extraElements -= extra_last_elements < extraElements ? extra_last_elements
                                                           : extraElements;

      size_type extra_used_blocks =
          ((extraElements + _BlockSize - 1) / _BlockSize);

      m_used_blocks += extra_used_blocks;
    } else {
      size_type neededExtraBlocks =
          (_count - totalElements + _BlockSize - 1) / _BlockSize;

      if (m_allocated_blocks + neededExtraBlocks > m_block_list.size()) {
        add_block_space(
            m_allocated_blocks + neededExtraBlocks - m_block_list.size());
      }

      for (size_type i = 0; i < neededExtraBlocks; ++i) {
        wn::memory::allocation_pair p =
            m_allocator->allocate(sizeof(value_type), _BlockSize);

        m_block_list[(m_start_block + m_allocated_blocks) %
                     m_block_list.size()] =
            static_cast<value_type*>(p.m_location);
        m_allocated_blocks += 1;
      }
      m_used_blocks = m_allocated_blocks;
    }
    m_element_count += _count;
  }

  wn_void remove_front(const size_type _count) {
    size_type count = _count;
    size_type elements_in_first_block = m_start_location;

    if (elements_in_first_block < count) {
      while (count > elements_in_first_block && elements_in_first_block > 0) {
        count -= elements_in_first_block;

        if (m_block_list.size() != m_allocated_blocks) {
          m_block_list[(m_start_block + m_allocated_blocks + 1) %
                       m_block_list.size()] = m_block_list[m_start_block];
          m_block_list[m_start_block] = wn_nullptr;
        }

        m_start_location = 0;
        m_start_block += 1;
        m_used_blocks -= 1;
        m_element_count -= elements_in_first_block;
        elements_in_first_block =
            _BlockSize < m_element_count ? _BlockSize : m_element_count;
      }

      m_element_count -= count;
      m_start_location += count;
    } else {
      m_element_count -= count;
      m_start_location += count;
    }
  }

  wn_void remove_back(const size_type _count) {
    size_type count = _count;
    size_type elements_in_last_block =
        (m_start_location + m_element_count) % _BlockSize;

    if (elements_in_last_block < count) {
      while (count > elements_in_last_block && elements_in_last_block > 0) {
        count -= elements_in_last_block;
        m_used_blocks -= 1;
        m_element_count -= elements_in_last_block;
        elements_in_last_block =
            _BlockSize < m_element_count ? _BlockSize : m_element_count;
      }

      m_element_count -= count;
    } else {
      m_element_count -= count;
    }
  }

  wn_bool closer_to_front(const_iterator _pos) const {
    return (_pos - cbegin() < cend() - _pos);
  }

  wn_void add_block_space(const size_type _count) {
    const size_type old_count = m_block_list.size();
    m_block_list.insert(m_block_list.end(), _count, wn_nullptr);
    m_block_list.insert(m_block_list.end(),
        m_block_list.capacity() - m_block_list.size(), wn_nullptr);

    if (m_start_block != 0) {
      const size_type added_count = m_block_list.size() - old_count;
      const size_type copy_size = (old_count - m_start_block);

      wn::memory::memory_move(&m_block_list[m_start_block + added_count],
          &m_block_list[m_start_block], copy_size);

#ifdef _WN_DEBUG
      wn::memory::memory_zero(&m_block_list[m_start_block], added_count);
#endif

      m_start_block += added_count;
    }
  }

  size_type total_unused_blocks() const {
    return (m_allocated_blocks - m_used_blocks);
  }

  size_type total_extra_elements_in_last_block() const {
    const size_type count =
        _BlockSize - (m_start_location + m_element_count) % _BlockSize;

    return ((count == _BlockSize) ? 0 : count);
  }

  memory::allocator* m_allocator;
  dynamic_array<_Type*, _Allocator> m_block_list;
  size_type m_used_blocks;
  size_type m_allocated_blocks;
  size_type m_start_block;
  size_type m_start_location;
  size_type m_element_count;
};

template <typename _Type, typename _Allocator, const wn_size_t _BlockSize>
_Allocator deque<_Type, _Allocator, _BlockSize>::s_default_allocator;

}  // namespace containers
}  // namespace wn

#endif