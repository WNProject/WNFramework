// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_SCRIPTING_AST_WALKER_H__
#define __WN_SCRIPTING_AST_WALKER_H__

#include "WNCore/inc/WNTypes.h"
#include "WNContainers/inc/WNContiguousRange.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/WNTypeValidator.h"

#include <type_traits>
namespace wn {
namespace scripting {

template <typename T, bool Const = wn_false>
class ast_walker {
 public:
  ast_walker(T* t) : m_walker(t) {}
  using expression_type =
      typename std::conditional<Const, const expression*, expression*>::type;
  using instruction_type =
      typename std::conditional<Const, const instruction*, instruction*>::type;
  using function_type =
      typename std::conditional<Const, const function*, function*>::type;
  using parameter_type =
      typename std::conditional<Const, const parameter*, parameter*>::type;
  using type_type = typename std::conditional<Const, const type*, type*>::type;
  using script_file_type =
      typename std::conditional<Const, const script_file*, script_file*>::type;
  using instruction_list_type =
    typename std::conditional<Const, const instruction_list*, instruction_list*>::type;

  void walk_script_file(script_file_type file);

 private:
  void walk_expression(expression_type _expr);
  void walk_instruction(instruction_type _inst);
  void walk_instruction_list(instruction_list_type _inst);
  void walk_function(function_type _func);
  void walk_parameter(parameter_type _param);
  void walk_type(type_type _type);
  void enter_scope_block();
  void leave_scope_block();
  T* m_walker;
};

template <typename T>
void run_ast_pass(T* _pass, script_file* _file) {
  ast_walker<T, false>(_pass).walk_script_file(_file);
}

template <typename T>
void run_ast_pass(T* _pass, const script_file* _file) {
  ast_walker<T, true>(_pass).walk_script_file(_file);
}


}  // namespace scripting
}  // namespace wn

#include "WNScripting/inc/WNASTWalker.inl"

#endif  //__WN_SCRIPTING_AST_WALKER_H__