// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef ROCKETCORECONTEXT_H__
#define ROCKETCORECONTEXT_H__

#include <unordered_map>

namespace Rocket {
namespace Core {

class Context {
public:
  Context();

  template <typename T, typename... Args>
  void RegisterCachedType(T*& _key, Args&&... a) {
    void* v = static_cast<void*>(new T(a...));
    m_elements[static_cast<void*>(&_key)] = v;
  }

  template <typename T>
  T& operator()(T*& _key) {
    void* v = m_elements[static_cast<void*>(&_key)];
    return *reinterpret_cast<T*>(v);
  }

private:
  std::unordered_map<void*, void*> m_elements;
};

}  // namespace Core
}  // namespace Rocket

#endif  // ROCKETCORECONTEXT_H__
