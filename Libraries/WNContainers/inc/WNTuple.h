// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CONTAINERS_TUPLE_H__
#define __WN_CONTAINERS_TUPLE_H__

#include "WNContainers/inc/WNPair.h"

namespace wn {
namespace containers {

template <const size_t Index, typename... T>
struct tuple_element;

template <const size_t Index, typename Tuple>
using tuple_element_t = typename tuple_element<Index, Tuple>::type;

template <typename... T>
class tuple;

template <>
class tuple<> {
public:
  WN_FORCE_INLINE tuple() {}

  WN_FORCE_INLINE tuple(tuple&&) {}

  WN_FORCE_INLINE tuple(const tuple&) {}

  WN_FORCE_INLINE void swap(tuple&) {}
};

template <typename T1, typename... T2>
class tuple<T1, T2...> : private tuple<T2...> {
private:
  using base = tuple<T2...>;

public:
  // constructors

  WN_FORCE_INLINE tuple() : base(), m_value() {}

  WN_FORCE_INLINE tuple(tuple&& _other)
    : base(core::move(_other)), m_value(core::move(_other.m_value)) {}

  WN_FORCE_INLINE tuple(const tuple& _other)
    : base(_other), m_value(_other.m_value) {}

  WN_FORCE_INLINE explicit tuple(T1&& _arg, T2&&... _args)
    : base(core::move(_args)...), m_value(core::move(_arg)) {}

  WN_FORCE_INLINE explicit tuple(const T1& _arg, const T2&... _args)
    : base(_args...), m_value(_arg) {}

  template <typename U,
      typename =
          core::enable_if_t<core::bool_and<core::are_same<T1, T2...>::value,
              core::is_constructible<T1, const U&>::value>::value>>
  WN_FORCE_INLINE tuple(const U& _value) : base(_value), m_value(_value) {}

  template <typename U1, typename... U2,
      typename =
          core::enable_if_t<core::bool_and<sizeof...(T2) == sizeof...(U2),
              core::is_constructible<T1, U1&&>::value,
              core::is_constructible<T2, U2&&>::value...>::value>>
  WN_FORCE_INLINE explicit tuple(U1&& _arg, U2&&... _args)
    : base(core::forward<U2>(_args)...), m_value(core::forward<U1>(_arg)) {}

  template <typename U1, typename... U2,
      typename =
          core::enable_if_t<core::bool_and<sizeof...(T2) == sizeof...(U2),
              core::is_constructible<T1, U1&&>::value,
              core::is_constructible<T2, U2&&>::value...>::value>>
  WN_FORCE_INLINE tuple(tuple<U1, U2...>&& _tuple)
    : base(core::forward<typename tuple<U1, U2...>::base>(_tuple)),
      m_value(core::forward<U1>(_tuple.m_value)) {}

  template <typename U1, typename... U2,
      typename =
          core::enable_if_t<core::bool_and<sizeof...(T2) == sizeof...(U2),
              core::is_constructible<T1, const U1&>::value,
              core::is_constructible<T2, const U2&>::value...>::value>>
  WN_FORCE_INLINE tuple(const tuple<U1, U2...>& _tuple)
    : base(static_cast<typename core::add_lvalue_reference<typename core::
                  add_const<typename tuple<U1, U2...>::base>::type>::type>(
          _tuple)),
      m_value(_tuple.m_value) {}

  template <typename U1, typename U2,
      typename = core::enable_if_t<core::bool_and<sizeof...(T2) == 1,
          core::is_constructible<T1, U1&&>::value,
          core::is_constructible<T2, U2&&>::value...>::value>>
  WN_FORCE_INLINE tuple(pair<U1, U2>&& _pair)
    : tuple(core::forward<U1>(_pair.first), core::forward<U2>(_pair.second)) {}

  template <typename U1, typename U2,
      typename = core::enable_if_t<core::bool_and<sizeof...(T2) == 1,
          core::is_constructible<T1, const U1&>::value,
          core::is_constructible<T2, const U2&>::value...>::value>>
  WN_FORCE_INLINE tuple(const pair<U1, U2>& _pair)
    : tuple(_pair.first, _pair.second) {}

  // assignments

  template <typename U1, typename U2,
      typename = core::enable_if_t<core::bool_and<sizeof...(T2) == 1,
          core::is_constructible<T1, U1&&>::value,
          core::is_constructible<T2, U2&&>::value...>::value>>
  WN_FORCE_INLINE tuple& operator=(pair<U1, U2>&& _pair) {
    tuple(core::move(_pair)).swap(*this);

    return *this;
  }

  template <typename U1, typename U2,
      typename = core::enable_if_t<core::bool_and<sizeof...(T2) == 1,
          core::is_constructible<T1, const U1&>::value,
          core::is_constructible<T2, const U2&>::value...>::value>>
  WN_FORCE_INLINE tuple& operator=(const pair<U1, U2>& _pair) {
    tuple(_pair).swap(*this);

    return *this;
  }

  WN_FORCE_INLINE tuple& operator=(tuple&& _other) {
    tuple(core::move(_other)).swap(*this);

    return *this;
  }

  WN_FORCE_INLINE tuple& operator=(const tuple& _other) {
    tuple(_other).swap(*this);

    return *this;
  }

  // modifiers

  WN_FORCE_INLINE void swap(tuple& _other) {
    core::swap(m_value, _other.m_value);

    base::swap(_other);
  }

private:
  template <typename... U>
  friend class tuple;

  template <const size_t Index, typename... T>
  friend typename core::add_lvalue_reference<
      typename tuple_element<Index, tuple<T...>>::type>::type
  get(tuple<T...>&);

  template <const size_t Index, typename... T>
  friend typename core::add_lvalue_reference<typename core::add_const<
      typename tuple_element<Index, tuple<T...>>::type>::type>::type
  get(const tuple<T...>&);

  template <const size_t Index, typename... T>
  friend typename core::add_rvalue_reference<
      typename tuple_element<Index, tuple<T...>>::type>::type
  get(tuple<T...>&& _tuple);

  template <const size_t Index, typename... T>
  friend typename core::add_rvalue_reference<typename core::add_const<
      typename tuple_element<Index, tuple<T...>>::type>::type>::type
  get(const tuple<T...>&& _tuple);

  T1 m_value;
};

template <typename T1, typename... T2>
class tuple_element<0, tuple<T1, T2...>> {
public:
  using type = T1;

private:
  template <const size_t Index, typename... T>
  friend typename core::add_lvalue_reference<
      typename tuple_element<Index, tuple<T...>>::type>::type
  get(tuple<T...>&);

  template <const size_t Index, typename... T>
  friend typename core::add_lvalue_reference<typename core::add_const<
      typename tuple_element<Index, tuple<T...>>::type>::type>::type
  get(const tuple<T...>&);

  template <const size_t Index, typename... T>
  friend typename core::add_rvalue_reference<
      typename tuple_element<Index, tuple<T...>>::type>::type
  get(tuple<T...>&& _tuple);

  template <const size_t Index, typename... T>
  friend typename core::add_rvalue_reference<typename core::add_const<
      typename tuple_element<Index, tuple<T...>>::type>::type>::type
  get(const tuple<T...>&& _tuple);

  using tuple_type = tuple<T1, T2...>;
};

template <const size_t Index, typename T1, typename... T2>
struct tuple_element<Index, tuple<T1, T2...>>
    : public tuple_element<Index - 1, tuple<T2...>> {};

template <const size_t Index, typename Tuple>
struct tuple_element<Index, const Tuple> : public tuple_element<Index, Tuple> {
  using type = typename core::add_const<
      typename tuple_element<Index, Tuple>::type>::type;
};

template <const size_t Index, typename Tuple>
struct tuple_element<Index, volatile Tuple>
    : public tuple_element<Index, Tuple> {
  using type = typename core::add_volatile<
      typename tuple_element<Index, Tuple>::type>::type;
};

template <const size_t Index, typename Tuple>
struct tuple_element<Index, const volatile Tuple>
    : public tuple_element<Index, Tuple> {
  using type =
      typename core::add_cv<typename tuple_element<Index, Tuple>::type>::type;
};

template <const size_t Index, typename... T>
WN_FORCE_INLINE typename core::add_lvalue_reference<
    typename tuple_element<Index, tuple<T...>>::type>::type
get(tuple<T...>& _tuple) {
  using tuple_type = typename tuple_element<Index, tuple<T...>>::tuple_type;

  return static_cast<tuple_type&>(_tuple).m_value;
}

template <const size_t Index, typename... T>
WN_FORCE_INLINE typename core::add_lvalue_reference<typename core::add_const<
    typename tuple_element<Index, tuple<T...>>::type>::type>::type
get(const tuple<T...>& _tuple) {
  using tuple_type = typename core::add_const<
      typename tuple_element<Index, tuple<T...>>::tuple_type>::type;

  return static_cast<tuple_type&>(_tuple).m_value;
}

template <const size_t Index, typename... T>
WN_FORCE_INLINE typename core::add_rvalue_reference<
    typename tuple_element<Index, tuple<T...>>::type>::type
get(tuple<T...>&& _tuple) {
  using tuple_type = typename tuple_element<Index, tuple<T...>>::tuple_type;
  using rvalue_type = typename core::add_rvalue_reference<
      typename tuple_element<Index, tuple<T...>>::type>::type;

  return core::forward<rvalue_type>(static_cast<tuple_type&>(_tuple).m_value);
}

template <const size_t Index, typename... T>
WN_FORCE_INLINE typename core::add_rvalue_reference<typename core::add_const<
    typename tuple_element<Index, tuple<T...>>::type>::type>::type
get(const tuple<T...>&& _tuple) {
  using tuple_type = typename core::add_const<
      typename tuple_element<Index, tuple<T...>>::tuple_type>::type;
  using rvalue_type =
      typename core::add_const<typename core::add_rvalue_reference<
          typename tuple_element<Index, tuple<T...>>::type>::type>::type;

  return core::forward<rvalue_type>(static_cast<tuple_type&>(_tuple).m_value);
}

template <typename T>
struct tuple_size;

template <typename... T>
struct tuple_size<tuple<T...>> : core::index_constant<sizeof...(T)> {};

template <typename T>
struct tuple_size<const T> : core::index_constant<tuple_size<T>::value> {};

template <typename T>
struct tuple_size<volatile T> : core::index_constant<tuple_size<T>::value> {};

template <typename T>
struct tuple_size<const volatile T>
    : core::index_constant<tuple_size<T>::value> {};

template <typename... T>
WN_FORCE_INLINE tuple<internal::arg_decay_t<T>...> make_tuple(T&&... _args) {
  return tuple<internal::arg_decay_t<T>...>(core::forward<T>(_args)...);
}

template <typename... T>
WN_FORCE_INLINE tuple<T&&...> forward_as_tuple(T&&... _args) {
  return tuple<T&&...>(core::forward<T>(_args)...);
}

}  // namespace containers
}  // namespace wn

#endif  // __WN_CONTAINERS_TUPLE_H__