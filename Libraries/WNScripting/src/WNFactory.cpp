// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNScripting/inc/WNFactory.h"
#include "WNScripting/inc/WNJITEngine.h"
#include "WNScripting/inc/WNCTranslator.h"

namespace wn {
namespace scripting {

memory::allocated_ptr<engine> factory::get_engine(
    scripting_engine_type _type, file_manager* _file_manager,
    WNLogging::WNLog* _log, wn::memory::allocator* _allocator) {
  switch (_type) {
    case scripting_engine_type::jit_engine:
      return memory::make_allocated_ptr<jit_engine>(_allocator, _allocator,
                                                    _file_manager, _log);
    default:
      return wn_nullptr;
  }
}

memory::allocated_ptr<translator> factory::get_translator(
    translator_type _type, file_manager* _file_manager, WNLogging::WNLog* _log,
    wn::memory::allocator* _allocator) {
  switch (_type) {
    case translator_type::c_translator:
      return memory::make_allocated_ptr<c_translator>(_allocator, _allocator,
                                                    _file_manager, _log);
    default:
      return wn_nullptr;
  }
}

}  // namespace scripting
}  // namespace wn