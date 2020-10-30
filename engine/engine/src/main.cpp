// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNApplicationEntry/inc/WNApplicationEntry.h"
#include "WNFileSystem/inc/WNFactory.h"
#include "WNScripting/inc/WNFactory.h"
#include "engine/inc/script_export.h"
#include "engine_base/inc/context.h"
#include "renderer/inc/render_context.h"
#include "support/inc/regex.h"
#include "support/inc/string.h"
#include "support/inc/subprocess.h"
#include "ui/inc/ui.h"
#include "window/inc/window.h"

using namespace wn;

int32_t wn_application_main(
    const runtime::application::application_data* _application_data) {
  _application_data->default_log->set_log_level(logging::log_level::debug);
  _application_data->default_log->log_info("Engine startup.");
  bool enable_support = false;
  for (int32_t i = 0; i < _application_data->executable_data->argc; ++i) {
    if (containers::string_view("--enable_support") ==
        _application_data->executable_data->argv[i]) {
      enable_support = true;
    }
  }

  int32_t ret = 0;
  {
    memory::allocator* file_system_allocator =
        _application_data->system_allocator;
    memory::allocator* scripting_allocator =
        _application_data->system_allocator;
    memory::allocator* ui_allocator = _application_data->system_allocator;
    memory::allocator* render_allocator = _application_data->system_allocator;
    memory::allocator* support_allocator = _application_data->system_allocator;

    // TODO: If we ever need a logger in file-systems add it here.
    logging::log* scripting_logger = _application_data->default_log;

    file_system::mapping_ptr mapping =
        file_system::factory(file_system_allocator,
            _application_data->executable_data, _application_data->default_log)
            .make_mapping(file_system_allocator,
                wn::file_system::mapping_type::development_assets);
    _application_data->default_log->flush();
    memory::unique_ptr<scripting::engine> scripting_engine =
        scripting::factory().get_engine(scripting_allocator,
            scripting::scripting_engine_type::jit_engine, mapping.get(),
            scripting_logger, support_allocator);

    engine::register_scripting(scripting_engine.get());
    engine_base::register_context(scripting_engine.get());
    engine::window::window::register_scripting(scripting_engine.get());
    engine::renderer::render_context::register_scripting(
        render_allocator, scripting_engine.get());
    engine::ui::ui::register_scripting(ui_allocator, scripting_engine.get());

    if (enable_support) {
      support::regex::register_scripting(
          support_allocator, scripting_engine.get());
      support::string::register_scripting(
          support_allocator, scripting_engine.get());
      support::subprocess::register_scripting(
          support_allocator, scripting_engine.get());
    }

    scripting::parse_error err = scripting_engine->parse_file("main.wns");
    if (err != scripting::parse_error::ok) {
      _application_data->default_log->log_critical("Could not parse main.wns");
      return -1;
    }

    if (!engine::renderer::render_context::resolve_scripting(
            scripting_engine.get()) ||
        !engine::ui::ui::resolve_scripting(scripting_engine.get())) {
      _application_data->default_log->log_critical(
          "Could not resolve needed script types");
      return -1;
    }
    if (enable_support) {
      if (!support::regex::resolve_scripting(scripting_engine.get())) {
        _application_data->default_log->log_critical(
            "Could not resolve needed script types for regex");
        return -1;
      }
      if (!support::string::resolve_scripting(scripting_engine.get())) {
        _application_data->default_log->log_critical(
            "Could not resolve needed script types for regex");
        return -1;
      }
      if (!support::subprocess::resolve_scripting(scripting_engine.get())) {
        _application_data->default_log->log_critical(
            "Could not resolve needed script types for regex");
        return -1;
      }
    }

    wn::scripting::script_function<int32_t, engine_base::context*> main;
    scripting_engine->get_function("main", &main);
    if (!main) {
      _application_data->default_log->log_critical(
          "Could not find Int main(Context ctx)");
      return -1;
    }

    engine_base::context ctx;
    ctx.m_allocator = _application_data->system_allocator;
    ctx.m_application_data = _application_data;
    ctx.m_engine = scripting_engine.get();
    ctx.m_log = _application_data->default_log;
    ctx.m_file_mapping = mapping.get();

    ret = scripting_engine->invoke(main, &ctx);
  }
  _application_data->default_log->log_info("Engine shutdown.");

  _application_data->default_log->flush();
  return ret;
}
