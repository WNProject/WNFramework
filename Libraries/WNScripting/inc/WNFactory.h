// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_SCRIPTING_ENGINE_FACTORY_H__
#define __WN_SCRIPTING_ENGINE_FACTORY_H__

#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/unique_ptr.h"
#include "WNScripting/inc/WNTranslator.h"
#include "WNScripting/inc/forward.h"

namespace wn {
namespace file_system {
class mapping;
}

namespace scripting {

class engine;

enum class scripting_engine_type {
  jit_engine,
};

enum class translator_type { c_translator };

class factory {
public:
  // Returns a wn::scripting::engine of the given type.
  // The engine will be constructed with the given allocator,
  // and all allocations performed by the engine will be performed
  // with the given allocator.
  virtual memory::unique_ptr<engine> get_engine(memory::allocator* _allocator,
      scripting_engine_type _type, file_system::mapping* _file_mapping,
      logging::log* _log, memory::allocator* _support_allocator = nullptr);

  // Returns a wn::scripting::translator of the given type.
  // The translator will be constructed with the given allocator,
  // and all allocations performed by the engine will be performed
  // with the given allocator.
  virtual memory::unique_ptr<translator> get_translator(
      memory::allocator* _allocator, translator_type _type,
      file_system::mapping* _file_mapping,
      file_system::mapping* _output_mapping, logging::log* _log);
};

extern thread_local const scripting_tls_data* g_scripting_tls;

}  // namespace scripting
}  // namespace wn

#endif  //__WN_SCRIPTING_ENGINE_FACTORY_H__
