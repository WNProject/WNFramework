// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "support/inc/subprocess.h"
#include "WNScripting/inc/WNFactory.h"
#include "platform_utils/inc/subprocess.h"

namespace wn {
namespace scripting {
template <>
struct exported_script_type<support::subprocess> {
  static containers::string_view exported_name() {
    return "Subprocess";
  }

  static void export_type(wn::scripting::exporter<support::subprocess>*) {}
};
}  // namespace scripting

namespace support {

scripting::shared_cpp_pointer<subprocess> make_subprocess(
    const char* _program, scripting::slice<const char*> _args) {
  return scripting::g_scripting_tls->_engine->make_shared_cpp<subprocess>(
      _program, scripting::g_scripting_tls->_support_allocator, _args);
}

int32_t call_subprocess(
    const char* _program, scripting::slice<const char*> _args) {
  containers::dynamic_array<containers::string_view> arr(
      scripting::g_scripting_tls->_support_allocator);
  for (auto& sv : _args) {
    arr.push_back(sv);
  }
  runtime::platform_utils::subprocess_return ret =
      runtime::platform_utils::call_subprocess(
          scripting::g_scripting_tls->_support_allocator,
          containers::string_view(_program),
          containers::contiguous_range<containers::string_view>(arr));
  return runtime::platform_utils::subprocess_error::ok == ret.err
             ? static_cast<int32_t>(ret.return_code)
             : -1;
}

void subprocess::register_scripting(
    memory::allocator*, scripting::engine* _engine) {
  _engine->register_cpp_type<subprocess>();
  _engine->register_function<decltype(&make_subprocess), &make_subprocess>(
      "make_subprocess");
  _engine->register_function<decltype(&call_subprocess), &call_subprocess>(
      "call_subprocess");
}

bool subprocess::resolve_scripting(scripting::engine*) {
  return true;
}

subprocess::subprocess(const char* _program, memory::allocator* _allocator,
    const scripting::slice<const char*>& _args) {
  (void)_program;
  (void)_allocator;
  (void)_args;
}

subprocess::~subprocess() {}

}  // namespace support
}  // namespace wn
