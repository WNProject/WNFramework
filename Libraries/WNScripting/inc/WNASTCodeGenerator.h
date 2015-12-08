// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_SCRIPTING_AST_CODE_GENERATOR_H__
#define __WN_SCRIPTING_AST_CODE_GENERATOR_H__

#include "WNCore/inc/WNTypes.h"
#include "WNContainers/inc/WNContiguousRange.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/WNTypeValidator.h"

namespace wn {
namespace scripting {

template<typename Traits>
class ast_code_generator {
 public:
  void set_generator(typename Traits::code_gen* _generator) {
    m_generator = _generator;
  }
  template <typename T>
  void walk_expression(T* t);
  template <typename T>
  void walk_instruction(T* t);
  void walk_parameter(parameter* _p);
  void walk_function(function* _f);
  void walk_type(type* _t);
  void walk_script_file(script_file* _f);

  typename Traits::instruction_data& get_data(const instruction* _inst);
  typename Traits::expression_data& get_data(const expression* _inst);
  typename Traits::parameter_data& get_data(const parameter* _inst);
  typename Traits::function_data& get_data(const function* _inst);
  typename Traits::type_data& get_data(const type* _type);
private:
  typename Traits::code_gen* m_generator;
  containers::hash_map<const instruction*, typename Traits::instruction_data> m_instruction_map;
  containers::hash_map<const expression*, typename Traits::expression_data> m_expression_map;
  containers::hash_map<const parameter*, typename Traits::parameter_data> m_parameter_map;
  containers::hash_map<const function*, typename Traits::function_data> m_function_map;
  containers::hash_map<const type*, typename Traits::type_data> m_type_map;
};

}  // namespace scripting
}  // namespace wn

#include "WNScripting/inc/WNASTCodeGenerator.inl"

#endif  //__WN_SCRIPTING_AST_CODE_GENERATOR_H__