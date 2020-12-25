// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#pragma once

#ifndef __WN_ENGINE_TEXTURE_H__
#define __WN_ENGINE_TEXTURE_H__

#include "WNGraphics/inc/WNCommandList.h"
#include "WNGraphics/inc/WNImage.h"
#include "WNScripting/inc/WNEngine.h"
#include "WNScripting/inc/forward.h"
#include "renderer/inc/gpu_heap.h"

namespace wn {
namespace file_system {
class mapping;
}
namespace runtime {
namespace graphics {
class command_list;
}  // namespace graphics
}  // namespace runtime
namespace engine {
namespace renderer {
class render_context;
struct texture_desc : scripting::script_object_type {
  using parent_type = void;

  void export_type(
      scripting::engine::script_type_importer<texture_desc>* _importer) {
    _importer->register_function("get_name", &get_name);
    _importer->register_function("get_width", &get_width);
    _importer->register_function("get_height", &get_height);
    _importer->register_function("get_format", &get_format);
  }

  static wn::containers::string_view exported_name() {
    return "TextureDescription";
  }

  static wn::containers::string_view required_script() {
    return "renderer/texture_description.wns";
  }

  scripting::scripting_object_function<texture_desc, const char*> get_name;
  scripting::scripting_object_function<texture_desc, int32_t> get_width;
  scripting::scripting_object_function<texture_desc, int32_t> get_height;
  scripting::scripting_object_function<texture_desc, int32_t> get_format;
};

class texture {
public:
  texture(render_context* _render_context, file_system::mapping* _mapping,
      runtime::graphics::command_list* _setup_list,
      scripting::script_pointer<texture_desc> texture, logging::log* _log);

  runtime::graphics::image& image() {
    return m_image;
  }

  runtime::graphics::image_view& view() {
    return m_image_view;
  }
  bool valid() const {
    return m_valid;
  }

private:
  runtime::graphics::image m_image;
  runtime::graphics::image_view m_image_view;
  gpu_allocation m_allocation;

  bool m_valid = false;
};

}  // namespace renderer
}  // namespace engine
}  // namespace wn

#endif  // __WN_ENGINE_TEXTURE_H__
