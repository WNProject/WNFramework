  // Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_SCRIPTING_ENGINE_FACTORY_H__
#define __WN_SCRIPTING_ENGINE_FACTORY_H__

#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNEngineFileManager.h"

namespace wn {
namespace scripting {

class engine;

enum class scripting_engine_type {
  jit_engine,
};

class engine_factory {
public:
  virtual memory::allocated_ptr<engine> get_engine(
      scripting_engine_type _type, file_manager* _file_manager,
      wn::memory::allocator* _allocator);
};
}
}

#endif  //__WN_SCRIPTING_ENGINE_FACTORY_H__