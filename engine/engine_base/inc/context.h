// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_ENGINE_BASE_CONTEXT_H__
#define __WN_ENGINE_BASE_CONTEXT_H__

#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNScripting/inc/WNEngine.h"

namespace wn {
namespace engine_base {

class context {
  // TODO(awoloszyn): Make this less public
public:
  const runtime::application::application_data* m_application_data;
  scripting::engine* m_engine;
  memory::allocator* m_allocator;
  logging::log* m_log;
};

void register_context(scripting::engine* _engine);

}  // namespace engine_base
}  // namespace wn

#endif  // __WN_ENGINE_BASE_CONTEXT_H__