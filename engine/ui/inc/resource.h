// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_ENGINE_UI_RESOURCE_H__
#define __WN_ENGINE_UI_RESOURCE_H__

#include "WNScripting/inc/resource.h"

namespace wn {
namespace scripting {
class engine;
}
namespace file_system {
class mapping;
}
namespace engine {
namespace ui {

class resource : public scripting::resource {
public:
  static void register_scripting(memory::allocator* _allocator,
      scripting::engine* _engine, file_system::mapping* _mapping);
  static bool resolve_scripting(scripting::engine*);

  resource(memory::allocator* _allocator, file_system::mapping* _mapping);

  bool convert_to_user_data(
      containers::string_view _resource_name, void** _dat) override;

  bool get_include_for_resource(
      containers::string_view _res, containers::string* _out) override;

  containers::string_view get_file_extension() const override {
    return ".rml";
  }

  scripting::convert_type convert_file(containers::string_view _file_name,
      containers::string* _out_string) override;

private:
  memory::allocator* m_allocator;
  file_system::mapping* m_mapping;
};

}  // namespace ui
}  // namespace engine
}  // namespace wn
#endif  // __WN_ENGINE_UI_RESOURCE_H__
