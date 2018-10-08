// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_ENGINE_BASE_CONTEXT_H__
#define __WN_ENGINE_BASE_CONTEXT_H__

#include "WNScripting/inc/WNEngine.h"

namespace wn {
namespace engine_base {

class context {
  // TODO(awoloszyn): Make this less public
public:
  scripting::engine* m_engine;
  logging::log* m_log;
};

void register_context(scripting::engine* _engine);

}  // namespace engine_base
}  // namespace wn

#endif  // __WN_ENGINE_BASE_CONTEXT_H__