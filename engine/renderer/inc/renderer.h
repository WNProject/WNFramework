// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_ENGINE_RENDERER_RENDERER_H__
#define __WN_ENGINE_RENDERER_RENDERER_H__

#include "WNLogging/inc/WNLog.h"
#include "WNScripting/inc/WNEngine.h"
#include "engine_base/inc/context.h"

namespace wn {
namespace engine {
namespace renderer {

class renderer {
public:
  renderer(logging::log* _log) : m_log(_log) {}
  static void register_scripting(scripting::engine* _engine);

private:
  logging::log* m_log;
};

}  // namespace renderer
}  // namespace engine
}  // namespace wn

#endif  // __WN_ENGINE_RENDERER_RENDERER_H__