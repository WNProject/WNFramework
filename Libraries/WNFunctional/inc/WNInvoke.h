// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_FUNCTIONAL_INVOKE_H__
#define __WN_FUNCTIONAL_INVOKE_H__

#include "WNCore/inc/WNTypeTraits.h"
#include "WNCore/inc/WNUtility.h"

namespace wn {
namespace functional {
namespace internal {

template <typename F, typename... Args>
WN_FORCE_INLINE auto invoke(F&& _function, Args&&... _args)
    -> core::enable_if_t<!core::is_member_pointer<core::decay_t<F>>::value,
        decltype(core::forward<F>(_function)(core::forward<Args>(_args)...))> {
  return (core::forward<F>(_function)(core::forward<Args>(_args)...));
}

template <typename Base, typename T, typename Derived, typename... Args>
WN_FORCE_INLINE auto invoke(
    T Base::*_member_function, Derived&& _reference, Args&&... _args)
    -> core::enable_if_t<
        core::is_function<T>::value &&
            core::is_base_of<Base, core::decay_t<Derived>>::value,
        decltype((core::forward<Derived>(_reference).*_member_function)(
            core::forward<Args>(_args)...))> {
  return ((core::forward<Derived>(_reference).*_member_function)(
      core::forward<Args>(_args)...));
}

template <typename Base, typename T, typename Derived>
WN_FORCE_INLINE auto invoke(T Base::*_member_data, Derived&& _reference)
    -> core::enable_if_t<
        !core::is_function<T>::value &&
            core::is_base_of<Base, core::decay_t<Derived>>::value,
        decltype(core::forward<Derived>(_reference).*_member_data)> {
  return (core::forward<Derived>(_reference).*_member_data);
}

template <typename T>
struct is_reference_wrapper : core::false_type {};

template <typename U>
struct is_reference_wrapper<std::reference_wrapper<U>> : core::true_type {};

template <typename Base, typename T, typename ReferenceWrapper,
    typename... Args>
WN_FORCE_INLINE auto invoke(
    T Base::*_member_function, ReferenceWrapper&& _reference, Args&&... _args)
    -> core::enable_if_t<
        core::is_function<T>::value &&
            is_reference_wrapper<core::decay_t<ReferenceWrapper>>::value,
        decltype((_reference.get().*_member_function)(
            core::forward<Args>(_args)...))> {
  return ((_reference.get().*_member_function)(core::forward<Args>(_args)...));
}

template <typename Base, typename T, typename Pointer, typename... Args>
WN_FORCE_INLINE auto invoke(
    T Base::*_member_function, Pointer&& ptr, Args&&... _args)
    -> core::enable_if_t<
        core::is_function<T>::value &&
            !is_reference_wrapper<core::decay_t<Pointer>>::value &&
            !core::is_base_of<Base, core::decay_t<Pointer>>::value,
        decltype(((*core::forward<Pointer>(ptr)).*_member_function)(
            core::forward<Args>(_args)...))> {
  return (((*core::forward<Pointer>(ptr)).*_member_function)(
      core::forward<Args>(_args)...));
}

template <typename Base, typename T, typename ReferenceWrapper>
WN_FORCE_INLINE auto invoke(
    T Base::*_member_data, ReferenceWrapper&& _reference)
    -> core::enable_if_t<
        !core::is_function<T>::value &&
            is_reference_wrapper<core::decay_t<ReferenceWrapper>>::value,
        decltype(_reference.get().*_member_data)> {
  return (_reference.get().*_member_data);
}

template <typename Base, typename T, typename Pointer>
WN_FORCE_INLINE auto invoke(T Base::*_member_data, Pointer&& ptr)
    -> core::enable_if_t<
        !core::is_function<T>::value &&
            !is_reference_wrapper<core::decay_t<Pointer>>::value &&
            !core::is_base_of<Base, core::decay_t<Pointer>>::value,
        decltype((*core::forward<Pointer>(ptr)).*_member_data)> {
  return ((*core::forward<Pointer>(ptr)).*_member_data);
}

}  // namespace internal

template <typename F, typename... Args>
WN_FORCE_INLINE auto invoke(F&& _f, Args&&... _args) -> decltype(
    internal::invoke(core::forward<F>(_f), core::forward<Args>(_args)...)) {
  return internal::invoke(core::forward<F>(_f), core::forward<Args>(_args)...);
}

}  // namespace functional
}  // namespace wn

#endif  //  __WN_FUNCTIONAL_INVOKE_H__