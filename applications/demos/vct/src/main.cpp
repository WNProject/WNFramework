// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNFileSystem/inc/WNFactory.h"
#include "WNLogging/inc/WNConsoleLogger.h"
#include "WNScripting/inc/WNFactory.h"
#include "executable_entry/inc/executable_entry.h"
#include "scene_demo.h"
#include "scene_file.h"
#include "texture_file.h"
#include "texture_manager.h"

#include <chrono>

using namespace wn;

int32_t wn_application_main(
    const runtime::application::application_data* _data) {
  memory::allocator* alloc = _data->system_allocator;

  containers::string_view data_dir;
  bool parse_data_dir = false;
  bool has_data_dir = false;
  bool verbose = false;
  const size_t argument_count =
      static_cast<size_t>(_data->executable_data->argc);

  for (size_t i = 0; i < argument_count; ++i) {
    if (containers::string_view("--data_dir") ==
        containers::string_view(_data->executable_data->argv[i])) {
      parse_data_dir = true;
      continue;
    }
    if (containers::string_view("--verbose") ==
        containers::string_view(_data->executable_data->argv[i])) {
      verbose = true;
    }

    if (parse_data_dir) {
      data_dir = _data->executable_data->argv[i];
      parse_data_dir = false;
      has_data_dir = true;
    }
  }

  static logging::console_logger<> logger;
  static logging::static_log<logging::log_level::max, 1024> log(
      &logger, verbose ? logging::log_level::max : logging::log_level::info);

  file_system::factory fs_factory(alloc, _data->executable_data, log.log());

  file_system::mapping_ptr files =
      has_data_dir ? fs_factory.make_mapping(alloc, data_dir)
                   : fs_factory.make_mapping(
                         alloc, file_system::mapping_type::development_assets);

  file_system::result res;
  file_system::file_ptr ptr = files->open_file("scene.scn", res);
  WN_RELEASE_ASSERT(res == file_system::result::ok, "Cannot open scene.scn");

  texture_manager textures(alloc, files.get());
  scene_file f(alloc, ptr, &textures, log.log());

  scene_demo demo(alloc, log.log(), _data, &textures, &f, files.get());

  auto last_time = std::chrono::high_resolution_clock::now();
  while (true) {
    auto now_time = std::chrono::high_resolution_clock::now();
    float time_since_last_clock =
        std::chrono::duration<float>(now_time - last_time).count();
    last_time = now_time;
    // If this fails then we should shut down.
    // In order to do that, we need to wait for previous stuff to be done.
    if (!demo.render_scene(false, time_since_last_clock)) {
      demo.render_scene(true, 0.0f);
      break;
    }
  }
  return 0;
}
