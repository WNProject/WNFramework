// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "support/inc/subprocess.h"
#include "WNMultiTasking/inc/job_pool.h"
#include "WNScripting/inc/WNFactory.h"
#include "engine_base/inc/context.h"
#include "platform_utils/inc/subprocess.h"

namespace wn {
namespace scripting {
template <>
struct exported_script_type<support::subprocess> {
  static containers::string_view exported_name() {
    return "Subprocess";
  }

  static void export_type(
      wn::scripting::exporter<support::subprocess>* _exporter) {
    _exporter->register_nonvirtual_function<
        decltype(&support::subprocess::get_stderr),
        &support::subprocess::get_stderr>("get_stderr");
    _exporter->register_nonvirtual_function<
        decltype(&support::subprocess::get_stdout),
        &support::subprocess::get_stdout>("get_stdout");
    _exporter->register_nonvirtual_function<
        decltype(&support::subprocess::join), &support::subprocess::join>(
        "join");
  }
};
template <>
struct exported_script_type<support::subprocess_args> {
  static containers::string_view exported_name() {
    return "SubprocessArgs";
  }

  static void export_type(
      wn::scripting::exporter<support::subprocess_args>* _exporter) {
    _exporter->register_nonvirtual_function<
        decltype(&support::subprocess_args::append),
        &support::subprocess_args::append>("append");
  }
};
}  // namespace scripting

namespace support {

scripting::shared_cpp_pointer<subprocess> make_subprocess(
    engine_base::context* _context, const char* _program,
    scripting::slice<const char*> _args) {
  return scripting::get_scripting_tls()->_engine->make_shared_cpp<subprocess>(
      scripting::get_scripting_tls()->_support_allocator, _context, _program,
      _args);
}

scripting::shared_cpp_pointer<subprocess> make_subprocess2(
    engine_base::context* _context, const char* _program) {
  return scripting::get_scripting_tls()->_engine->make_shared_cpp<subprocess>(
      scripting::get_scripting_tls()->_support_allocator, _context, _program,
      scripting::slice<const char*>());
}

scripting::shared_cpp_pointer<subprocess> make_subprocess3(
    engine_base::context* _context, const char* _program,
    scripting::shared_cpp_pointer<subprocess_args> _args) {
  return scripting::get_scripting_tls()->_engine->make_shared_cpp<subprocess>(
      scripting::get_scripting_tls()->_support_allocator, _context, _program,
      _args.get());
}

scripting::shared_cpp_pointer<subprocess_args> make_subprocess_args(
    engine_base::context* _context) {
  return scripting::get_scripting_tls()
      ->_engine->make_shared_cpp<subprocess_args>(_context);
}

int32_t call_subprocess(
    const char* _program, scripting::slice<const char*> _args) {
  containers::dynamic_array<containers::string_view> arr(
      scripting::get_scripting_tls()->_support_allocator);
  for (auto& sv : _args) {
    arr.push_back(sv);
  }
  runtime::platform_utils::subprocess_return ret =
      runtime::platform_utils::call_subprocess(
          scripting::get_scripting_tls()->_support_allocator,
          containers::string_view(_program),
          containers::contiguous_range<containers::string_view>(arr));
  return runtime::platform_utils::subprocess_error::ok == ret.err
             ? static_cast<int32_t>(ret.return_code)
             : -1;
}

void subprocess::register_scripting(
    memory::allocator*, scripting::engine* _engine) {
  _engine->register_cpp_type<subprocess_args>();
  _engine->register_cpp_type<subprocess>();
  _engine->register_function<decltype(&make_subprocess), &make_subprocess>(
      "make_subprocess");
  _engine->register_function<decltype(&make_subprocess2), &make_subprocess2>(
      "make_subprocess");
  _engine->register_function<decltype(&make_subprocess3), &make_subprocess3>(
      "make_subprocess");
  _engine->register_function<decltype(&call_subprocess), &call_subprocess>(
      "call_subprocess");
  _engine->register_function<decltype(&make_subprocess_args),
      &make_subprocess_args>("make_subprocess_args");
}

bool subprocess::resolve_scripting(scripting::engine*) {
  return true;
}

subprocess::subprocess(memory::allocator* _allocator,
    engine_base::context* _context, const char* _program,
    const subprocess_args* _args)
  : m_signal(_context->m_application_data->default_job_pool->get_signal()) {
  containers::dynamic_array<containers::string> args(
      scripting::get_scripting_tls()->_support_allocator);
  for (const auto& sv : _args->args) {
    args.push_back(containers::string(_allocator, sv));
  }

  containers::string prog(_allocator, _program);
  auto tls = scripting::get_scripting_tls();
  _context->m_application_data->default_job_pool->call_blocking_function(
      JOB_NAME,
      functional::function<void()>(
          scripting::get_scripting_tls()->_support_allocator,
          [this, tls, prog{core::move(prog)}, args{core::move(args)}]() {
            containers::dynamic_array<containers::string_view> arr(
                tls->_support_allocator);
            for (auto& sv : args) {
              arr.push_back(sv);
            }
            m_return = runtime::platform_utils::call_subprocess(
                tls->_support_allocator, prog.to_string_view(),
                containers::contiguous_range<containers::string_view>(arr));
          }),
      m_signal);
}

subprocess::subprocess(memory::allocator* _allocator,
    engine_base::context* _context, const char* _program,
    const scripting::slice<const char*> _args)
  : m_signal(_context->m_application_data->default_job_pool->get_signal()) {
  containers::dynamic_array<containers::string> args(
      scripting::get_scripting_tls()->_support_allocator);
  for (const auto& sv : _args) {
    args.push_back(containers::string(_allocator, sv));
  }
  containers::string prog(_allocator, _program);
  auto tls = scripting::get_scripting_tls();
  _context->m_application_data->default_job_pool->call_blocking_function(
      JOB_NAME,
      functional::function<void()>(
          scripting::get_scripting_tls()->_support_allocator,
          [this, tls, prog{core::move(prog)}, args{core::move(args)}]() {
            containers::dynamic_array<containers::string_view> arr(
                tls->_support_allocator);
            for (auto& sv : args) {
              arr.push_back(sv);
            }
            m_return = runtime::platform_utils::call_subprocess(
                tls->_support_allocator, prog.to_string_view(),
                containers::contiguous_range<containers::string_view>(arr));
          }),
      m_signal);
}

subprocess::~subprocess() {
  m_signal.wait_until(1);
}

int32_t subprocess::join() {
  m_signal.wait_until(1);
  return runtime::platform_utils::subprocess_error::ok == m_return.err
             ? static_cast<int32_t>(m_return.return_code)
             : -1;
}

const char* subprocess::get_stdout() {
  m_signal.wait_until(1);
  return m_return.std_out.c_str();
}

const char* subprocess::get_stderr() {
  m_signal.wait_until(1);
  return m_return.std_err.c_str();
}

subprocess_args::subprocess_args(engine_base::context* _context)
  : args(_context->m_allocator), allocator(_context->m_allocator) {}

}  // namespace support
}  // namespace wn
