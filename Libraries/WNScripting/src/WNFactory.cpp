// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNScripting/inc/WNFactory.h"
#include "WNScripting/inc/WNCTranslator.h"
#include "WNScripting/inc/WNJITEngine.h"

namespace wn {
namespace scripting {

memory::unique_ptr<engine> factory::get_engine(scripting_engine_type _type,
    type_validator* _validator, file_system::mapping* _file_mapping,
    WNLogging::WNLog* _log, memory::allocator* _allocator) {
  switch (_type) {
    case scripting_engine_type::jit_engine:
      return memory::make_unique<jit_engine>(
          _allocator, _validator, _allocator, _file_mapping, _log);
    default:
      return nullptr;
  }
}

memory::unique_ptr<translator> factory::get_translator(translator_type _type,
    type_validator* _validator, file_system::mapping* _file_mapping,
    WNLogging::WNLog* _log, memory::allocator* _allocator) {
  switch (_type) {
    case translator_type::c_translator:
      return memory::make_unique<c_translator>(
          _allocator, _validator, _allocator, _file_mapping, _log);
    default:
      return nullptr;
  }
}

}  // namespace scripting
}  // namespace wn