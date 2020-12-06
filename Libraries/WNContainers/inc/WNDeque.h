// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CONTAINERS_DEQUE_H__
#define __WN_CONTAINERS_DEQUE_H__

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/manipulation.h"
#include "core/inc/type_traits.h"

#include <iterator>

namespace wn {
namespace containers {

template <typename _Type, const size_t _BlockSize>
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

  deque_iterator() : m_deque(nullptr), m_element(0) {}

  deque_iterator(deque_iterator&& _other) noexcept
    : m_deque(core::move(_other.m_deque)),
      m_element(core::move(_other.m_element)) {
    _other.clear();
  }

  deque_iterator(const deque_iterator& _other)
    : m_deque(_other.m_deque), m_element(_other.m_element) {}

  template <typename _OtherContainer = _NonConstContainer>
  deque_iterator(deque_iterator<_OtherContainer, _OtherContainer,
                     typename _OtherContainer::value_type>&& _other,
      typename core::enable_if<
          !core::is_same<_Container, _OtherContainer>::value>::type* = nullptr)
    : m_deque(core::move(_other.m_deque)),
      m_element(core::move(_other.m_element)) {
    _other.clear();
  }

  template <typename _OtherContainer = _NonConstContainer>
  deque_iterator(const deque_iterator<_OtherContainer, _OtherContainer,
                     typename _OtherContainer::value_type>& _other,
      typename core::enable_if<
          !core::is_same<_Container, _OtherContainer>::value>::type* = nullptr)
    : m_deque(_other.m_deque), m_element(_other.m_element) {}

  deque_iterator& operator=(deque_iterator&& _other) {
    m_deque = core::move(_other.m_deque);
    m_element = core::move(_other.m_element);

    _other.clear();

    return *this;
  }

  deque_iterator& operator=(const deque_iterator& _other) {
    m_deque = _other.m_deque;
    m_element = _other.m_element;

    return *this;
  }

  template <typename _OtherContainer = _NonConstContainer>
  typename core::enable_if<!core::is_same<_Container, _OtherContainer>::value,
      deque_iterator>::type&
  operator=(deque_iterator<_OtherContainer, _OtherContainer,
      typename _OtherContainer::value_type>&& _other) {
    m_deque = core::move(_other.m_deque);
    m_element = core::move(_other.m_element);

    _other.clear();

    return *this;
  }

  template <typename _OtherContainer = _NonConstContainer>
  typename core::enable_if<!core::is_same<_Container, _OtherContainer>::value,
      deque_iterator>::type&
  operator=(const deque_iterator<_OtherContainer, _OtherContainer,
      typename _OtherContainer::value_type>& _other) {
    m_deque = _other.m_deque;
    m_element = _other.m_element;

    return *this;
  }

  difference_type operator-(const deque_iterator& _other) const {
    WN_RELEASE_ASSERT(m_deque == _other.m_deque, "iterators are incompatible");

    return (m_element - _other.m_element);
  }

  deque_iterator& operator+=(const size_t _amount) {
    m_element += _amount;

    return *this;
  }

  deque_iterator& operator-=(const size_t _amount) {
    m_element -= _amount;

    return *this;
  }

  deque_iterator operator+(const size_t _amount) const {
    deque_iterator i(*this);

    return (i += _amount);
  }

  deque_iterator operator-(const size_t _amount) const {
    deque_iterator i(*this);

    return (i -= _amount);
  }

  reference operator*() const {
    return ((*m_deque)[m_element]);
  }

  pointer operator->() const {
    return (&((*m_deque)[m_element]));
  }

  deque_iterator operator++(int32_t) {
    deque_iterator i(*this);

    (*this) += 1;

    return (i);
  }

  deque_iterator operator--(int32_t) {
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

  bool operator==(const deque_iterator& _other) const {
    WN_RELEASE_ASSERT(m_deque == _other.m_deque, "iterators are incompatible");

    return (m_element == _other.m_element);
  }

  bool operator!=(const deque_iterator& _other) const {
    WN_RELEASE_ASSERT(m_deque == _other.m_deque, "iterators are incompatible");

    return (m_element != _other.m_element);
  }

  bool operator>(const deque_iterator& _other) const {
    WN_RELEASE_ASSERT(m_deque == _other.m_deque, "iterators are incompatible");

    return (m_element > _other.m_element);
  }

  bool operator>=(const deque_iterator& _other) const {
    WN_RELEASE_ASSERT(m_deque == _other.m_deque, "iterators are incompatible");

    return (m_element >= _other.m_element);
  }

  bool operator<(const deque_iterator& _other) const {
    WN_RELEASE_ASSERT(m_deque == _other.m_deque, "iterators are incompatible");

    return (m_element < _other.m_element);
  }

  bool operator<=(const deque_iterator& _other) const {
    WN_RELEASE_ASSERT(m_deque == _other.m_deque, "iterators are incompatible");

    return (m_element <= _other.m_element);
  }

private:
  template <typename _Type, const size_t _BlockSize>
  friend class wn::containers::deque;

  friend class deque_iterator<const _Container, _Container, const _Element>;

  explicit deque_iterator(_Container* _deque, const size_type _element)
    : m_deque(_deque), m_element(_element) {}

  void clear() {
    m_deque = nullptr;
    m_element = 0;
  }

  _Container* m_deque;
  size_type m_element;
};

}  // namespace internal

template <typename _Type, const size_t _BlockSize = 10>
class deque final {
public:
  using value_type = _Type;
  using size_type = size_t;
  using difference_type = signed_t;
  using reference = value_type&;
  using const_reference = const value_type&;

private:
  using self_type = deque<_Type, _BlockSize>;

public:
  using iterator = internal::deque_iterator<self_type>;
  using const_iterator =
      internal::deque_iterator<const self_type, self_type, const value_type>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  deque() : deque(nullptr) {}

  deque(memory::allocator* _allocator, const deque& _other)
    : deque(_allocator) {
    for (const auto& it : _other) {
      push_back(it);
    }
  }

  deque(deque&& _other) noexcept
    : m_allocator(core::move(_other.m_allocator)),
      m_block_list(core::move(_other.m_block_list)),
      m_used_blocks(core::move(_other.m_used_blocks)),
      m_allocated_blocks(core::move(_other.m_allocated_blocks)),
      m_start_block(core::move(_other.m_start_block)),
      m_start_location(core::move(_other.m_start_location)),
      m_element_count(core::move(_other.m_element_count)) {
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
      m_allocated_blocks(0),
      m_start_block(0),
      m_start_location(0),
      m_element_count(0) {}

  explicit deque(memory::allocator* _allocator, const size_type _count)
    : deque(_allocator, _count, _Type()) {}

  deque(memory::allocator* _allocator, const size_type _count,
      const _Type& _value)
    : deque(_allocator) {
    resize(_count, _value);
  }

  template <typename _InputIt,
      typename = core::enable_if_t<!std::is_integral<_InputIt>::value>>
  deque(memory::allocator* _allocator, _InputIt _first, _InputIt _last)
    : deque(_allocator) {
    insert(cbegin(), _first, _last);
  }

  deque(memory::allocator* _allocator,
      std::initializer_list<_Type> _initializer_list)
    : deque(_allocator, _initializer_list.begin(), _initializer_list.end()) {}

  // If the allocator is the same, then we just core::move other.
  // If the allocator is different, then we core::move the elements,
  // and clear the other.
  void take_over(deque&& _other) {
    if (m_allocator == _other.m_allocator) {
      *this = core::move(_other);
    } else {
      clear();
      clean_blocks();
      for (deque::iterator it = _other.begin(); it != _other.end(); ++it) {
        emplace_back(core::move(*it));
      }
      _other.clear();
      _other.clean_blocks();
    }
  }

  deque& operator=(deque&& _other) noexcept {
    clear();
    clean_blocks();
    m_allocator = core::move(_other.m_allocator);
    m_block_list = core::move(_other.m_block_list);
    m_used_blocks = core::move(_other.m_used_blocks);
    m_start_block = core::move(_other.m_start_block);
    m_start_location = core::move(_other.m_start_location);
    m_allocated_blocks = core::move(_other.m_allocated_blocks);
    m_element_count = core::move(_other.m_element_count);
    _other.m_used_blocks = 0;
    _other.m_start_block = 0;
    _other.m_start_location = 0;
    _other.m_allocated_blocks = 0;
    _other.m_element_count = 0;
    return *this;
  }

  ~deque() {
    clear();
    clean_blocks();
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
    const_iterator i = cend();

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

  bool empty() const {
    return (size() == 0);
  }

  size_type size() const {
    return (m_element_count);
  }

  size_type capacity() const {
    return (m_allocated_blocks * _BlockSize);
  }

  // modifiers

  void clear() {
    erase(begin(), end());
  }

  iterator insert(const_iterator _pos, _Type&& _value) {
    iterator iter = allocate(_pos, 1);
    iterator newIter = iter;

    memory::construct_at<_Type>(&(*(newIter++)), core::move(_value));

    return (iter);
  }

  iterator insert(const_iterator _pos, const _Type& _value) {
    _Type value(_value);

    return (insert(_pos, core::move(value)));
  }

  iterator insert(
      const_iterator _pos, const size_type _count, const _Type& _value) {
    iterator iter = allocate(_pos, _count);
    iterator newIter = iter;

    for (size_t i = 0; i < _count; ++i) {
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

  template <typename _Function,
      typename = core::enable_if_t<
          core::is_invocable_r<_Type, _Function, size_type>::value>>
  iterator insert(
      const_iterator _pos, const size_type _count, _Function&& _generator) {
    iterator position = allocate(_pos, _count);
    iterator new_position = position;

    for (size_type i = 0; i < _count; ++i) {
      memory::construct_at<_Type>(
          &(*(position++)), core::move(_generator(i++)));
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
      _Type& t = *(erase_start++);
      (void)t;  // If _Type does not have a destructor
                // some compilers will complain without this.
      t.~_Type();
    }

    if (closer_to_front(pos)) {
      iterator copy_to = pos + _count - 1;
      iterator copy_from = pos - 1;
      const size_type copy_count = pos - begin();

      for (size_type i = 0; i < copy_count; ++i) {
        memory::construct_at<_Type>(&(*copy_to), core::move(*copy_from));

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
      // m_element_count - location gives us the number of elements between
      // pos and the end, (it is the same as end() - pos), we have to subtract
      // _count to determine how many elements need copying. This correctly
      // handles
      // for example, when end() - 1 is the element being removed.
      const size_type copy_count = m_element_count - location - _count;

      for (size_type i = 0; i < copy_count; ++i) {
        memory::construct_at<_Type>(&(*copy_to), core::move(*copy_from));

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
  void emplace_front(Args&&... _args) {
    iterator iter = allocate(cbegin(), 1);
    iterator newIter = iter;

    memory::construct_at<_Type>(&(*(newIter++)), std::forward<Args>(_args)...);
  }
  void push_front(_Type&& _value) {
    insert(cbegin(), core::move(_value));
  }

  void push_front(const _Type& _value) {
    _Type value(_value);

    push_front(core::move(value));
  }

  template <typename... Args>
  void emplace_back(Args&&... _args) {
    iterator iter = allocate(cbegin() + m_element_count, 1);
    iterator newIter = iter;

    memory::construct_at<_Type>(&(*(newIter++)), std::forward<Args>(_args)...);
  }

  void push_back(_Type&& _value) {
    insert(cbegin() + m_element_count, core::move(_value));
  }

  void push_back(const _Type& _value) {
    _Type value(_value);
    push_back(core::move(value));
  }

  void pop_front() {
    erase(begin(), 1);
  }

  void pop_back() {
    erase(end() - 1, 1);
  }

  void resize(const size_type _count) {
    resize(_count, _Type());
  }

  void resize(const size_type _count, const value_type& _value) {
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

  void swap(deque& _other) {
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
  void* allocate(const size_t _size, const size_t _count) {
    return m_allocator->allocate(_size * _count);
  }

  void clean_blocks() {
    for (size_t i = 0; i < m_allocated_blocks; ++i) {
      const size_t index = (m_start_block + i) % m_block_list.size();

      deallocate(m_block_list[index]);
    }
    m_block_list.clear();
  }

  void deallocate(void* ptr) {
    if (m_allocator) {
      m_allocator->deallocate(ptr);
    } else {
      WN_DEBUG_ASSERT(
          ptr == nullptr, "m_allocator is nullptr, where did ptr come from");
    }
  }

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
        memory::construct_at<_Type>(&(*copy_to), core::move(*copy_from));

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
        memory::construct_at<_Type>(&(*copy_to), core::move(*copy_from));

        (*copy_from).~value_type();

        --copy_to;
        --copy_from;
      }

      return (begin() + location);
    }
  }

  void allocate_front(const size_type _count) {
    const size_type leftovers_in_block = m_start_location;

    if (leftovers_in_block < _count) {
      size_type accountedElements = leftovers_in_block;
      size_type additional_blocks = (_count - accountedElements);

      additional_blocks = (additional_blocks + _BlockSize) / _BlockSize;

      size_type haveBlocks = total_unused_blocks();

      if (haveBlocks < additional_blocks) {
        const size_type neededBlocks = additional_blocks - haveBlocks;
        size_type allocated_blocks = neededBlocks;
        if (m_block_list.size() - m_allocated_blocks < neededBlocks) {
          add_block_space(
              m_allocated_blocks + neededBlocks - m_block_list.size());
        }

        for (size_type i = 0; i < allocated_blocks; ++i) {
          void* p = allocate(sizeof(value_type), _BlockSize);
          m_block_list[(m_start_block + m_allocated_blocks) %
                       m_block_list.size()] = static_cast<value_type*>(p);
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
                       m_block_list.size()] = nullptr;
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

  void allocate_back(const size_type _count) {
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
        void* p = allocate(sizeof(value_type), _BlockSize);

        m_block_list[(m_start_block + m_allocated_blocks) %
                     m_block_list.size()] = static_cast<value_type*>(p);
        m_allocated_blocks += 1;
      }
      m_used_blocks = m_allocated_blocks;
    }
    m_element_count += _count;
  }

  void remove_front(const size_type _count) {
    size_type count = _count;
    size_type elements_in_first_block = (_BlockSize - m_start_location);
    elements_in_first_block = elements_in_first_block > m_element_count
                                  ? m_element_count
                                  : elements_in_first_block;

    if (elements_in_first_block < count) {
      while (count > elements_in_first_block && elements_in_first_block > 0) {
        count -= elements_in_first_block;

        if (m_block_list.size() != m_allocated_blocks) {
          m_block_list[(m_start_block + m_allocated_blocks + 1) %
                       m_block_list.size()] = m_block_list[m_start_block];
          m_block_list[m_start_block] = nullptr;
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

  void remove_back(const size_type _count) {
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

  bool closer_to_front(const_iterator _pos) const {
    return (_pos - cbegin() < cend() - _pos);
  }

  void add_block_space(const size_type _count) {
    const size_type old_count = m_block_list.size();
    m_block_list.insert(m_block_list.end(), _count, nullptr);

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
  dynamic_array<_Type*> m_block_list;
  size_type m_used_blocks;
  size_type m_allocated_blocks;
  size_type m_start_block;
  size_type m_start_location;
  size_type m_element_count;
};

}  // namespace containers
}  // namespace wn

#endif
