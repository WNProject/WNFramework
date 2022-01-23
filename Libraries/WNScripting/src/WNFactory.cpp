// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNFactory.h"

#include "WNScripting/inc/WNCTranslator.h"
#include "WNScripting/inc/WNJITEngine.h"

namespace wn {
namespace scripting {

memory::unique_ptr<engine> factory::get_engine(memory::allocator* _allocator,
    scripting_engine_type _type, file_system::mapping* _file_mapping,
    logging::log* _log, memory::allocator* _support_allocator) {
  switch (_type) {
    case scripting_engine_type::jit_engine:
      return memory::make_unique<jit_engine>(
          _allocator, _allocator, _file_mapping, _log, _support_allocator);
    default:
      return nullptr;
  }
}

memory::unique_ptr<translator> factory::get_translator(
    memory::allocator* _allocator, translator_type _type,
    file_system::mapping* _file_mapping, file_system::mapping* _output_mapping,
    logging::log* _log) {
  switch (_type) {
    case translator_type::c_translator:
      return memory::make_unique<c_translator>(
          _allocator, _allocator, _file_mapping, _output_mapping, _log);
    default:
      return nullptr;
  }
}

void script_object_type::free(void* val) {
  // TODO(awoloszyn): Use the allocator here.
  // For now we allocated with malloc, so free with malloc
  ::free(val);
}

void do_engine_free(const engine* _engine, void* v) {
  _engine->free_shared(v);
}

thread_local const scripting_tls_data* g_scripting_tls = nullptr;

const scripting_tls_data*& get_scripting_tls() {
  return g_scripting_tls;
}

}  // namespace scripting
}  // namespace wn
