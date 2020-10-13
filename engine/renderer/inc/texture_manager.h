// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_ENGINE_TEXTURE_MANAGER_H__
#define __WN_ENGINE_TEXTURE_MANAGER_H__

#include "WNScripting/inc/WNEngine.h"
#include "WNScripting/inc/resource_manager.h"

namespace wn {
namespace engine {
namespace renderer {

struct texture_manager : scripting::resource_manager {
  static void register_scripting(
      memory::allocator* _allocator, scripting::engine* _engine);
  static bool resolve_scripting(scripting::engine* _engine);

  texture_manager(scripting::engine* _engine, memory::allocator* _allocator);

  bool convert_to_function(containers::string_view _resource_name,
      containers::string_view _resource_data,
      containers::string* _dat) override;

  bool get_include_for_resource(
      containers::string_view _res, containers::string* _out) override;

  containers::string_view get_file_extension() const override {
    return ".wntex";
  }

  scripting::convert_type convert_file(logging::log* _log,
      file_system::mapping*, containers::string_view _file_name,
      containers::string* _out_data, containers::string* _out_string) override;

private:
  memory::allocator* m_allocator;
  scripting::engine* m_engine;
};

}  // namespace renderer
}  // namespace engine
}  // namespace wn

#endif  // __WN_ENGINE_TEXTURE_MANAGER_H__
