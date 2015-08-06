// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_SCRIPTING_JIT_ENGINE_H__
#define __WN_SCRIPTING_JIT_ENGINE_H__

#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNScriptingEngineFileManager.h"
#include "WNScripting/inc/WNScriptingEngine.h"

namespace wn {
namespace scripting {

class jit_engine : public engine {
 public:
  jit_engine(memory::allocator* _allocator, file_manager* _manager)
      : m_allocator(_allocator), m_file_manager(_manager) {}

  parse_error parse_file(const char* file) override;
 private:
  memory::allocator* m_allocator;
  file_manager* m_file_manager;
};
}
}

#endif  //__WN_SCRIPTING_JIT_ENGINE_H__
