// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_SCRIPTING_JIT_ENGINE_H__
#define __WN_SCRIPTING_JIT_ENGINE_H__

#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNEngineFileManager.h"
#include "WNScripting/inc/WNEngine.h"

namespace WNLogging {
class WNLogger;
}

namespace wn {
namespace scripting {

class jit_engine : public engine {
 public:
  jit_engine(memory::allocator* _allocator, file_manager* _manager,
             WNLogging::WNLog* _log)
      : m_allocator(_allocator),
        m_file_manager(_manager),
        m_compilation_log(_log) {}
  parse_error parse_file(const char* file) override;
 private:
  memory::allocator* m_allocator;
  file_manager* m_file_manager;
  WNLogging::WNLog* m_compilation_log;
};
}
}

#endif  //__WN_SCRIPTING_JIT_ENGINE_H__
