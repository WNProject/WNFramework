// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ROCKETCORECONTEXT_H__
#define ROCKETCORECONTEXT_H__

#include <unordered_map>

namespace Rocket {
namespace Core {

class Context {
public:
  Context();
  bool LoadScriptContents() {
    return m_load_script_contents;
  }
  void SetLoadScriptContents(bool _load) {
    m_load_script_contents = _load;
  }
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
  bool m_load_script_contents;
};

}  // namespace Core
}  // namespace Rocket

#endif  // ROCKETCORECONTEXT_H__
