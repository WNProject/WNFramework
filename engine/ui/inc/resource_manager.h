// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_ENGINE_UI_RESOURCE_H__
#define __WN_ENGINE_UI_RESOURCE_H__

#include "WNContainers/inc/WNHashMap.h"
#include "WNScripting/inc/resource_manager.h"

namespace wn {
namespace scripting {
class engine;
}
namespace file_system {
class mapping;
}
namespace engine {
namespace ui {

class resource_manager : public scripting::resource_manager {
public:
  static void register_scripting(
      memory::allocator* _allocator, scripting::engine* _engine);
  static bool resolve_scripting(scripting::engine*);

  resource_manager(scripting::engine* _engine, memory::allocator* _allocator);

  bool convert_to_function(containers::string_view _resource_name,
      containers::string_view _resource_data, logging::log*,
      containers::string*, core::optional<uintptr_t>* _user_data) override;

  bool setup_resource(
      containers::string_view _res, containers::string* _out) override;

  containers::string_view get_file_extension() const override {
    return ".rml";
  }

  scripting::convert_type convert_file(logging::log* _log,
      file_system::mapping*, containers::string_view _file_name,
      containers::string* _out_data, containers::string* _out_string) override;

private:
  memory::allocator* m_allocator;
  scripting::engine* m_engine;
  containers::hash_map<containers::string, containers::string> m_ui_names;
};

}  // namespace ui
}  // namespace engine
}  // namespace wn
#endif  // __WN_ENGINE_UI_RESOURCE_H__
