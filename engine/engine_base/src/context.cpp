// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "engine_base/inc/context.h"
#include "WNScripting/inc/WNEngine.h"

using namespace wn;

// First let us export the context to scripting.
namespace wn {
namespace scripting {
template <>
struct exported_script_type<engine_base::context> {
  static containers::string_view exported_name() {
    return "Context";
  }

  template <typename T>
  static void export_type(T*) {}
};
}  // namespace scripting
}  // namespace wn

namespace wn {
namespace engine_base {
void register_context(scripting::engine* _engine) {
  _engine->register_cpp_type<engine_base::context>();
}

}  // namespace engine_base
}  // namespace wn