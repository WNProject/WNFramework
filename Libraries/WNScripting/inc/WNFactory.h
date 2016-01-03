// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_SCRIPTING_ENGINE_FACTORY_H__
#define __WN_SCRIPTING_ENGINE_FACTORY_H__

#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNEngineFileManager.h"
#include "WNScripting/inc/WNTranslator.h"

namespace WNLogging {
class WNLog;
}

namespace wn {
namespace scripting {

class engine;
class type_validator;

enum class scripting_engine_type {
  jit_engine,
};

enum class translator_type {
  c_translator
};

class factory {
 public:
   virtual memory::allocated_ptr<engine> get_engine(scripting_engine_type _type,
       type_validator* _validator, file_manager* _file_manager,
       WNLogging::WNLog* _log, wn::memory::allocator* _allocator);
   virtual memory::allocated_ptr<translator> get_translator(
       translator_type _type, type_validator* _validator,
       file_manager* _file_manager, WNLogging::WNLog* _log,
       wn::memory::allocator* _allocator);
};
}  // namespace scripting
}  // namesapce wn

#endif  //__WN_SCRIPTING_ENGINE_FACTORY_H__