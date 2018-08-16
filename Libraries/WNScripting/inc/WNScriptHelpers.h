// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_SCRIPT_HELPERS_H__
#define __WN_SCRIPTING_SCRIPT_HELPERS_H__

#include "WNContainers/inc/WNArray.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/unique_ptr.h"
#include "WNScripting/inc/WNEnums.h"

namespace wn {
namespace scripting {
class script_file;
struct ast_script_file;
class type_manager;
struct ast_type;

struct external_function {
  containers::string_view name;
  containers::dynamic_array<ast_type*> params;
};

// Use this type instead of normal size_t in order for
// size_t to automatically work between scripting and C++.
// This is because size_t is not a "proper" type in most
// compilers.
struct wn_size_t {
  size_t val;
};

template <typename T, size_t S = 0>
struct wn_array_ptr;

template <typename T, size_t S = 0>
struct wn_array {
private:
  uint32_t _size = S;
  T _values[S == 0 ? 1 : S];

public:
  wn_array_ptr<T, 0> operator&();
  T& operator[](size_t i) {
    return _values[i];
  }
  const T& operator[](size_t i) const {
    return _values[i];
  }

  size_t size() const {
    return _size;
  }
};

template <typename T, size_t S>
struct wn_array_ptr {
  wn_array<T, S>* ptr;
  wn_array<T, S>& operator*() {
    return *ptr;
  }

  wn_array<T, S>* operator->() {
    return ptr;
  }
  operator wn_array_ptr<T, 0>() {
    return wn_array_ptr<T, 0>{reinterpret_cast<wn_array<T, 0>*>(ptr)};
  }
};

template <typename T, size_t S>
wn_array_ptr<T, 0> wn_array<T, S>::operator&() {
  auto* t = this;
  static_assert(sizeof(this) == sizeof(wn_array_ptr<T, 0>),
      "Invalid conversion to array_ptr");
  return *reinterpret_cast<wn_array_ptr<T, 0>*>(&t);
}

// Simple helper that parses a script and runs any
// passes that are required to make the AST valid.

memory::unique_ptr<ast_script_file> parse_script(memory::allocator* _allocator,
    const char* file_name, containers::string_view view,
    type_manager* _type_manager, bool _print_ast_on_failure, logging::log* _log,
    size_t* _num_warnings, size_t* _num_errors);
}  // namespace scripting
}  // namespace wn

#endif  // __WN_SCRIPTING_SCRIPT_HELPERS_H__
