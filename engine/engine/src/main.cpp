// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNApplicationEntry/inc/WNApplicationEntry.h"
#include "WNFileSystem/inc/WNFactory.h"
#include "WNScripting/inc/WNFactory.h"
#include "engine/inc/engine_scripts.h"
#include "engine/inc/script_export.h"
#include "engine_base/inc/context.h"
#include "profiling/inc/allocator.h"
#include "renderer/inc/render_context.h"
#include "support/inc/command_line.h"
#include "support/inc/json.h"
#include "support/inc/log.h"
#include "support/inc/regex.h"
#include "support/inc/string.h"
#include "support/inc/subprocess.h"
#include "support/inc/sync.h"
#include "ui/inc/ui.h"
#include "window/inc/window.h"

using namespace wn;

int32_t wn_application_main(
    const runtime::application::application_data* _application_data) {
  _application_data->default_log->set_log_level(logging::log_level::debug);
  _application_data->default_log->log_info("Engine startup.");
  containers::string_view script_dir;
  containers::string_view script_file = "main.wns";
  if (_application_data->executable_data->argc > 1 &&
      _application_data->executable_data->argv[1][0] != '-') {
    containers::string_view v(_application_data->executable_data->argv[1]);
    size_t p = v.find_last_of("\\/");
    if (p != containers::string_view::npos) {
      script_dir = v.substr(0, p);
    } else {
      p = 0;
      script_dir = ".";
    }
    script_file = v.substr(p + 1);
  }

  int32_t ret = 0;
  {
    engine_base::context ctx(_application_data->system_allocator);

    profiling::allocator file_system_allocator(
        _application_data->system_allocator, "FileSystem");
    profiling::allocator scripting_allocator(
        _application_data->system_allocator, "Scripting");
    profiling::allocator ui_allocator(
        _application_data->system_allocator, "UI");
    profiling::allocator render_allocator(
        _application_data->system_allocator, "Renderer");
    profiling::allocator support_allocator(
        _application_data->system_allocator, "Support");
    profiling::allocator actor_allocator(
        _application_data->system_allocator, "Actors");
    // TODO: If we ever need a logger in file-systems add it here.
    logging::log* scripting_logger = _application_data->default_log;

    support::command_line_manager command_line_mgr(
        &scripting_allocator, scripting_logger);
    file_system::factory fs_factory(&file_system_allocator,
        _application_data->executable_data, _application_data->default_log);

    file_system::mapping_ptr ptrs[2];
    if (!script_dir.empty()) {
      ptrs[0] = fs_factory.make_mapping(&file_system_allocator, script_dir);
      if (!ptrs[0]) {
        _application_data->default_log->log_error(
            "Could not find folder for ", script_dir);
        return -1;
      }
    } else {
      ptrs[0] = fs_factory.make_mapping(&file_system_allocator,
          wn::file_system::mapping_type::development_assets);
    }

    ptrs[1] = fs_factory.make_mapping(
        &file_system_allocator, wn::file_system::mapping_type::memory_backed);
    ptrs[1]->initialize_files(engine_scripts::get_files());

    file_system::mapping_ptr script_mapping =
        fs_factory.overlay_readonly_mappings(&file_system_allocator, ptrs);

    _application_data->default_log->flush();
    memory::unique_ptr<scripting::engine> scripting_engine =
        scripting::factory().get_engine(&scripting_allocator,
            scripting::scripting_engine_type::jit_engine, script_mapping.get(),
            scripting_logger, &support_allocator, &actor_allocator, &ctx);

    engine::register_scripting(scripting_engine.get());
    engine_base::register_context(scripting_engine.get());
    engine::window::window::register_scripting(scripting_engine.get());
    engine::renderer::render_context::register_scripting(
        &render_allocator, scripting_engine.get());
    {
      support::regex_manager::register_scripting(
          &support_allocator, scripting_engine.get());
      support::string::register_scripting(
          &support_allocator, scripting_engine.get());
      support::subprocess::register_scripting(
          &support_allocator, scripting_engine.get());
      support::log::register_scripting(
          &support_allocator, scripting_engine.get());
      support::json_document::register_scripting(
          &support_allocator, scripting_engine.get());
      support::sync::register_scripting(
          &support_allocator, scripting_engine.get());
      command_line_mgr.register_scripting(
          &support_allocator, scripting_engine.get());
    }
    engine::ui::ui::register_scripting(&ui_allocator, scripting_engine.get());

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
    {
      if (!support::regex_manager::resolve_scripting(scripting_engine.get())) {
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
            "Could not resolve needed script types for subprocess");
        return -1;
      }
      if (!support::log::resolve_scripting(scripting_engine.get())) {
        _application_data->default_log->log_critical(
            "Could not resolve needed script types for logging");
        return -1;
      }
      if (!support::json_document::resolve_scripting(scripting_engine.get())) {
        _application_data->default_log->log_critical(
            "Could not resolve needed script types for json");
        return -1;
      }
      if (!support::sync::resolve_scripting(scripting_engine.get())) {
        _application_data->default_log->log_critical(
            "Could not resolve needed script types for json");
        return -1;
      }
      if (!command_line_mgr.resolve_scripting(scripting_engine.get())) {
        _application_data->default_log->log_critical(
            "Could not resolve needed script types for command_line");
        return -1;
      }
    }
    command_line_mgr.set_global_program_name(
        _application_data->executable_data->argv[0]);
    ctx.m_ui_allocator = &ui_allocator;
    ctx.m_allocator = _application_data->system_allocator;
    ctx.m_application_data = _application_data;
    ctx.m_engine = scripting_engine.get();
    ctx.m_log = _application_data->default_log;
    ctx.m_file_mapping = script_mapping.get();
    bool cmd_err = false;
    for (int32_t i = script_dir.empty() ? 1 : 2;
         i < _application_data->executable_data->argc; ++i) {
      if (!command_line_mgr.insert_global_param(
              _application_data->executable_data->argv[i])) {
        cmd_err = true;
        break;
      }
    }
    if (!cmd_err) {
      ret = command_line_mgr.run_application(scripting_engine.get(), &ctx);
    }
  }
  _application_data->default_log->log_info("Engine shutdown.");

  _application_data->default_log->flush();
  return ret;
}
