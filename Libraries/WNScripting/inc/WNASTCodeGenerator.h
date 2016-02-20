// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_SCRIPTING_AST_CODE_GENERATOR_H__
#define __WN_SCRIPTING_AST_CODE_GENERATOR_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNCore/inc/WNTypes.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/WNTypeValidator.h"

namespace wn {
namespace scripting {

// Placeholder traits. If ast_code_generator,
// is initialized with this, it will fail.
// Notes on traits types:
//  ...data values must be default constructible
//  They will be default constructed and then passed
//   to the walk_*() function calls to be filled in.
//  They will be default destructed when the
//    ast_code_generator is destroyed.
struct null_traits {
  using instruction_list_data = void;
  using instruction_data = void;
  using expression_data = void;
  using parameter_data = void;
  using function_data = void;
  using type_data = void;
  using code_gen = void;
  using struct_definition_type = void;
};
template <typename Traits = null_traits>
class ast_code_generator {
public:
  ast_code_generator(memory::allocator* _allocator)
    : m_instruction_list_map(_allocator),
      m_instruction_map(_allocator),
      m_expression_map(_allocator),
      m_parameter_map(_allocator),
      m_function_map(_allocator),
      m_type_map(_allocator),
      m_struct_definition_map(_allocator) {}
  // Sets the internal generator.
  // This generator is expected to have at least the
  // following methods:
  // void walk_expression(const expression*, Traits::expression_data*);
  // void walk_instruction_list(
  //     const instruction_list*, Traits::instruction_list_data*);
  // void walk_type(const type*, Traits::type_data*);
  // void walk_instruction(const instruction*, Traits::instruction_data*);
  // void walk_parameter(const parameter*, Traits::parameter_data*);
  // void walk_function(const function*, Traits::function_data*);
  // void walk_script_file(const script_file*);
  // Any specializations of these methods, for example:
  // void walk_expression(const binary_expression*, Traits::expression_data*)
  // will be called instead of the base method if they are present,
  // otherwise the base method will be called.
  void set_generator(typename Traits::code_gen* _generator) {
    m_generator = _generator;
  }

  // All of the following create new *_data objects, and call
  // the underlying m_generator->walk_... function.
  template <typename T>
  void walk_expression(const T* t);
  template <typename T>
  void walk_instruction(const T* t);
  void walk_parameter(const parameter* _p);
  void walk_instruction_list(const instruction_list* _l);
  void walk_function(const function* _f);
  void walk_type(const type* _t);
  void walk_struct_definition(const struct_definition* _s);
  // Calls the underlying m_generator->walk_script_file function.
  void walk_script_file(const script_file* _f);
  // Called when a scope block is entered or left.
  // No-ops here, needed for the correct interface to ASTWalker.
  void enter_scope_block() {}
  void leave_scope_block() {}

  // Given a scripting node, returns the data that was
  // created in the associated call to walk_*.
  typename Traits::instruction_list_data& get_data(
      const instruction_list* _inst);
  typename Traits::instruction_data& get_data(const instruction* _inst);
  typename Traits::expression_data& get_data(const expression* _expr);
  typename Traits::parameter_data& get_data(const parameter* _param);
  typename Traits::function_data& get_data(const function* _func);
  typename Traits::type_data& get_data(const type* _type);
  typename Traits::struct_definition_data& get_data(
      const struct_definition* _type);

private:
  typename Traits::code_gen* m_generator;
  // A collection of scripting nodes to their associated data.
  containers::hash_map<const instruction_list*,
      typename Traits::instruction_list_data>
      m_instruction_list_map;
  containers::hash_map<const instruction*, typename Traits::instruction_data>
      m_instruction_map;
  containers::hash_map<const expression*, typename Traits::expression_data>
      m_expression_map;
  containers::hash_map<const parameter*, typename Traits::parameter_data>
      m_parameter_map;
  containers::hash_map<const function*, typename Traits::function_data>
      m_function_map;
  containers::hash_map<const type*, typename Traits::type_data> m_type_map;
  containers::hash_map<const struct_definition*,
      typename Traits::struct_definition_data>
      m_struct_definition_map;
};

}  // namespace scripting
}  // namespace wn

#include "WNScripting/inc/WNASTCodeGenerator.inl"

#endif  //__WN_SCRIPTING_AST_CODE_GENERATOR_H__