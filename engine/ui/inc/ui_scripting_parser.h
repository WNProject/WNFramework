// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_ENGINE_UI_SCRIPTING_PARSER_H__
#define __WN_ENGINE_UI_SCRIPTING_PARSER_H__
#include "Rocket/Core.h"
#include "WNContainers/inc/WNList.h"
#include "WNContainers/inc/WNString.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/allocator.h"

namespace wn {
namespace file_system {
class mapping;
}
namespace engine {
namespace ui {
class ui_scripting_parser {
public:
  ui_scripting_parser(logging::log* _log, memory::allocator* _allocator)
    : m_allocator(_allocator), m_log(_log) {}

  bool parse_ui(file_system::mapping* _mapping, containers::string_view _view,
      containers::string* _out_string);

private:
  memory::allocator* m_allocator;
  logging::log* m_log;
};

}  // namespace ui
}  // namespace engine
}  // namespace wn
#endif