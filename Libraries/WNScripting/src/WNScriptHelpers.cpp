// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNScripting/inc/WNScriptHelpers.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNMemory/inc/WNUniquePtr.h"
#include "WNScripting/inc/WNASTPasses.h"
#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/WNTypeValidator.h"
#include "WNScripting/src/WNScriptASTLexer.hpp"
#include "WNScripting/src/WNScriptASTParser.hpp"

namespace wn {
namespace scripting {

namespace {
memory::unique_ptr<function> create_external_function(
    memory::allocator* _allocator, const type_validator* _validator,
    const containers::string_view& _file, const external_function& _function) {
  const type_definition& ops = _validator->get_operations(_function.ret_type);
  memory::unique_ptr<type> ret_type = memory::make_unique<type>(
      _allocator, _allocator, _function.ret_type, ops.get_reference_type());
  ret_type->set_start_location(
      {0, 0, reinterpret_cast<const uint8_t*>(_file.data()), 0, 0});

  memory::unique_ptr<parameter_list> params =
      memory::make_unique<parameter_list>(_allocator, _allocator);
  params->copy_location_from(ret_type.get());

  char name[5] = {'_', '\0', '\0', '\0', '\0'};
  WN_RELEASE_ASSERT_DESC(_function.params.size() < 32,
      "Invalid to have a function with more than 31 parameters");
  for (uint32_t i = 0; i < _function.params.size(); ++i) {
    memory::writeuint32(name + 1, i, sizeof(name) - 2);
    const type_definition& param_ops =
        _validator->get_operations(_function.params[i]);

    memory::unique_ptr<type> param_type = memory::make_unique<type>(_allocator,
        _allocator, _function.params[i], param_ops.get_reference_type());
    param_type->copy_location_from(ret_type.get());
    memory::unique_ptr<parameter> param = memory::make_unique<parameter>(
        _allocator, _allocator, core::move(param_type), name);
    param->copy_location_from(ret_type.get());

    params->add_parameter(core::move(param));
  }

  memory::unique_ptr<parameter> signature = memory::make_unique<parameter>(
      _allocator, _allocator, core::move(ret_type), _function.name);
  signature->copy_location_from(signature->get_type());
  memory::unique_ptr<function> fn = memory::make_unique<function>(_allocator,
      _allocator, core::move(signature), core::move(params), nullptr);
  fn->set_mangled_name(_validator->get_mangled_name(
      _function.name, _function.ret_type, _function.params));
  return core::move(fn);
}
}  // anonymous namespace

memory::unique_ptr<script_file> parse_script(memory::allocator* _allocator,
    scripting::type_validator* _validator, const char* file_name,
    const containers::contiguous_range<external_function>& _external_functions,
    containers::string_view view, bool _dump_ast_on_failure, logging::log* _log,
    size_t* _num_warnings, size_t* _num_errors) {
  memory::unique_ptr<script_file> ptr;
  {
    WNScriptASTLexer::InputStreamType input(
        const_cast<ANTLR_UINT8*>(
            reinterpret_cast<const ANTLR_UINT8*>(view.data())),
        ANTLR_ENC_8BIT, static_cast<ANTLR_UINT32>(view.size()),
        const_cast<ANTLR_UINT8*>(
            reinterpret_cast<const ANTLR_UINT8*>(file_name)));

    WNScriptASTLexer lexer(&input);
    WNScriptASTParser::TokenStreamType tStream(
        ANTLR_SIZE_HINT, lexer.get_tokSource());
    WNScriptASTParser parser(&tStream);
    parser.set_allocator(_allocator);
    ptr = core::move(
        memory::unique_ptr<script_file>(_allocator, parser.program()));
    if (parser.getNumberOfSyntaxErrors() != 0 ||
        lexer.getNumberOfSyntaxErrors() != 0) {
      if (_num_errors) {
        _num_errors += parser.getNumberOfSyntaxErrors();
      }
      if (_dump_ast_on_failure) {
        ptr->print_node(_log, wn::logging::log_level::error);
      }
      return nullptr;
    }

    for (const auto& func : _external_functions) {
      ptr->add_external_function(
          create_external_function(_allocator, _validator, view, func));
    }

    if (!run_struct_normalization_pass(
            ptr.get(), _log, _validator, _num_warnings, _num_errors)) {
      if (_dump_ast_on_failure) {
        ptr->print_node(_log, wn::logging::log_level::error);
      }
      return nullptr;
    }

    if (!run_symbol_resolution_pass(
            ptr.get(), _log, _validator, _num_warnings, _num_errors)) {
      if (_dump_ast_on_failure) {
        ptr->print_node(_log, wn::logging::log_level::error);
      }
      return nullptr;
    }

    if (!run_dce_pass(
            ptr.get(), _log, _validator, _num_warnings, _num_errors)) {
      if (_dump_ast_on_failure) {
        ptr->print_node(_log, wn::logging::log_level::error);
      }
      return nullptr;
    }

    if (!run_array_determination_pass(
            ptr.get(), _log, _validator, _num_warnings, _num_errors)) {
      if (_dump_ast_on_failure) {
        ptr->print_node(_log, wn::logging::log_level::error);
      }
      return nullptr;
    }

    if (!run_if_reassociation_pass(
            ptr.get(), _log, _validator, _num_warnings, _num_errors)) {
      if (_dump_ast_on_failure) {
        ptr->print_node(_log, wn::logging::log_level::error);
      }
      return nullptr;
    }

    if (!run_member_reassociation_pass(
            ptr.get(), _log, _validator, _num_warnings, _num_errors)) {
      if (_dump_ast_on_failure) {
        ptr->print_node(_log, wn::logging::log_level::error);
      }
      return nullptr;
    }

    if (!run_temporary_reification_pass(
            ptr.get(), _log, _validator, _num_warnings, _num_errors)) {
      if (_dump_ast_on_failure) {
        ptr->print_node(_log, wn::logging::log_level::error);
      }
      return nullptr;
    }
    if (!run_id_association_pass(
            ptr.get(), _log, _validator, _num_warnings, _num_errors)) {
      if (_dump_ast_on_failure) {
        ptr->print_node(_log, wn::logging::log_level::error);
      }
      return nullptr;
    }

    if (!run_type_association_pass(
            ptr.get(), _log, _validator, _num_warnings, _num_errors)) {
      if (_dump_ast_on_failure) {
        ptr->print_node(_log, wn::logging::log_level::error);
      }
      return nullptr;
    }
  }
  return core::move(ptr);
}

}  // namespace scripting
}  // namespace wn
