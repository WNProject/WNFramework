// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_SCRIPTING_AST_WALKER_H__
#define __WN_SCRIPTING_AST_WALKER_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/WNTypeValidator.h"

namespace wn {
namespace scripting {

// ast_walker description.
// The ast_walker expects a class containing callbacks
// with at least the following signatures.
// Note for all types the pointer is first cast to
// the exact type of the node. This means that if you
// declare traits::expression_type walk_expression(binary_expression*,
//    contiguous_range<contiguous_range<traits::expression_type>>,
//    traits::type_type)
// it will be called instead of the generic one.

// void pre_walk_expression(expression*);
// traits::expression_type walk_expression(expression*,
//         contiguous_range<contiguous_range<traits::expression_type>>,
//         wn_uint32,
//         );

// void pre_walk_function(function*);
// traits::function_type walk_function(function*,
//    traits::declaration*,
//    contiguous_range<traits::declaration>,
//    contiguous_range<traits::instruction>);

// void pre_walk_assignment_instruction(assignment_instruction*);
// traits::assignment_instruction_type walk_assignment_instruction
//         (assignment_instruction*, traits::lvalue, traits::expression);

// void pre_walk_declaration(declaration*);
// traits::instruction_type walk_declaration(declaration*);

// void loop_instruction(instruction*)
// traits::instruction_type loop_instruction(instruction*,
//     traits::expression, // start of loop condition
//     traits::expression, // end of loop condition
//     contiguous_range<traits::instruction>, // instructions
//     traits::instruction, // pre-entry instruction
//     traits::instruction); // loop continuation instruction

// void pre_walk_if_instruction(if_instruction*)
// For each condition there is a matching block, there may be an extra
// block at the end, if there is a default case.
// traits::instruction_type walk_if_instruction(instruction*,
//     contiguous_range<traits::expression>, // conditions
//     contiguous_range<contiguous_range<traits::instruction>>) blocks

// void pre_walk_return_instruction(return_instruction*)
// traits::return_instruction_type  walk_return_instruction(
//          return_instruction*)
// traits::return_instruction_type  walk_return_instruction(
//          return_instruction*, traits::expression)

// void pre_walk_lvalue(lvalue*);
// traits::lvalue_type walk_lvalue(lvalue*, traits::expression*);

// void pre_walk_script_file(script_file*);
// traits::script_file_type walk_script_file(script_file*,
//                       contiguous_range<traits::function_type>,
//                       contiguous_range<traits::struct_definition_type>);

// void pre_walk_struct_definition(struct_definition*);
// traits::struct_definition_type walk_struct_definition(struct_definition*,
//    contiguous_range<traits::parameter_type>,
//    contiguous_range<traits::function>);

// void pre_walk_type(type*);
// traits::type_type walk_type(type*);

class default_ast_walker_traits {
  typedef expression* expression_type;
  typedef function* function_type;
  typedef void* function_header_type;
  typedef parameter* parameter_type;
  typedef instruction* instruction_type;
  typedef lvalue* lvalue_type;
  typedef script_file* script_file_type;
  typedef struct_definition* struct_definition_type;
  typedef type* type_type;
  typedef containers::string include_type;
};

template <typename T>
T* cast_to(const node* _node) {
  return static_cast<T*>(_node);
}

template <typename T>
T* cast_to(node* _node) {
  return static_cast<T*>(_node);
}

template <typename T, typename Traits>
class ast_walker {
 public:
  ast_walker(T* _consumer, WNLogging::WNLog* _log,
             scripting::type_validator* _validator,
             memory::allocator* _allocator)
      : m_consumer(_consumer),
        m_allocator(_allocator),
        m_log(_log),
        m_validator(_validator) {}

  typename Traits::expression_type handle_walk_expression(
      const expression* _node) {
    switch (_node->get_node_type()) {
      case node_type::array_allocation_expression:
        return walk_array_allocation_expression(
            cast_to<const array_allocation_expression>(_node));
      case node_type::binary_expression:
        return walk_binary_expression(cast_to<const binary_expression>(_node));
      case node_type::cast_expression:
        return walk_cast_expression(cast_to<const cast_expression>(_node));
      case node_type::cond_expression:
        return walk_cond_expression(cast_to<const cond_expression>(_node));
      case node_type::constant_expression:
        return walk_constant_expression(
            cast_to<const constant_expression>(_node));
      case node_type::id_expression:
        return walk_id_expression(cast_to<const id_expression>(_node));
      case node_type::null_allocation_expression:
        return walk_null_allocation_expression(
            cast_to<const null_allocation_expression>(_node));
      case node_type::array_access_expression:
        return walk_array_access_expression(
            cast_to<const array_access_expression>(_node));
      case node_type::function_call_expression:
        return walk_function_call_expression(
            cast_to<const function_call_expression>(_node));
      case node_type::member_access_expression:
        return walk_member_access_expression(
            cast_to<const member_access_expression>(_node));
      case node_type::post_unary_expression:
        return walk_post_unary_expression(
            cast_to<const post_unary_expression>(_node));
      case node_type::short_circuit_expression:
        return walk_short_circuit_expression(
            cast_to<const short_circuit_expression>(_node));
      case node_type::struct_allocation_expression:
        return walk_struct_allocation_expression(
            cast_to<const struct_allocation_expression>(_node));
      case node_type::unary_expression:
        return walk_unary_expression(cast_to<const unary_expression>(_node));
      default:
        WN_DEBUG_ASSERT_DESC(wn_false, "Invalid expression type");
        return typename Traits::expression_type();
        break;
    }
  }

  typename Traits::instruction_type walk_instruction(
      const instruction* _instruction) {
    switch (_instruction->get_node_type()) {
      case node_type::assignment_instruction:
        return walk_assignment_instruction(
            cast_to<const assignment_instruction>(_instruction));
        break;
      case node_type::declaration:
        return walk_declaration(cast_to<const declaration>(_instruction));
        break; 
      case node_type::do_instruction:
        return walk_do_instruction(cast_to<const do_instruction>(_instruction));
        break;
      case node_type::else_if_instruction:
        WN_DEBUG_ASSERT_DESC(wn_false,
                             "You should not have an else_if by itself.");
        break;
      case node_type::for_instruction:
        return walk_for_instruction(
            cast_to<const for_instruction>(_instruction));
        break;
      case node_type::if_instruction:
        return walk_if_instruction(cast_to<const if_instruction>(_instruction));
        break;
      case node_type::return_instruction:
        return walk_return_instruction(
            cast_to<const return_instruction>(_instruction));
        break;
      case node_type::while_instruction:
        return walk_while_instruction(
            cast_to<const while_instruction>(_instruction));
        break;
      default:
        WN_DEBUG_ASSERT_DESC(wn_false, "Invalid instruction type");
    }
    return typename Traits::instruction_type();
  }

  typename Traits::instruction_type walk_assignment_instruction(
      const assignment_instruction*) {
    WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
    return typename Traits::instruction_type();
  }
  typename Traits::instruction_type walk_do_instruction(const do_instruction*) {
    WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
    return typename Traits::instruction_type();
  }
  typename Traits::instruction_type walk_for_instruction(
      const for_instruction*) {
    WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
    return typename Traits::instruction_type();
  }
  typename Traits::instruction_type walk_if_instruction(const if_instruction*) {
    WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
    return typename Traits::instruction_type();
  }

  typename Traits::instruction_type walk_return_instruction(
      const return_instruction* _instruction) {
    m_consumer->pre_walk_return_instruction(_instruction);

    if (_instruction->get_expression()) {
      typename Traits::expression_type expr =
          handle_walk_expression(_instruction->get_expression());
      return m_consumer->walk_return_instruction(_instruction, expr);
    }

    return m_consumer->walk_return_instruction(_instruction);
  }

  typename Traits::instruction_type walk_while_instruction(
      const while_instruction*) {
    WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
    return typename Traits::instruction_type();
  }

  typename Traits::type_type walk_type(const type* _type) {
    m_consumer->pre_walk_type(_type);
    WN_RELEASE_ASSERT_DESC(
        _type->get_classification() != type_classification::custom_type,
        "Implement custom types");
    if (!m_validator->is_valid_type(_type->get_type_value())) {
      m_log->Log(WNLogging::eError, 0, "Unknown type: ",
                 _type->custom_type_name().to_string(m_allocator).c_str());
      _type->log_line(*m_log, WNLogging::eError);
    }
    return m_consumer->walk_type(_type);
  }

  typename Traits::instruction_type walk_declaration(
      const declaration* _declaration) {
    m_consumer->pre_walk_declaration(_declaration);
    typename Traits::type_type type = walk_type(_declaration->get_type());
    WN_RELEASE_ASSERT_DESC(_declaration->get_expression() == wn_nullptr,
                           "Not implemented declaration expressions.");
    WN_RELEASE_ASSERT_DESC(_declaration->get_array_sizes().empty(),
                           "Not implemented sized arrays.");
    WN_RELEASE_ASSERT_DESC(
        _declaration->get_num_unsized_array_initializers() == 0,
        "Not implemented unsized array initializers.");
    return m_consumer->walk_declaration(_declaration, type);
  }

  typename Traits::parameter_type walk_parameter(
      const parameter* _parameter) {
    m_consumer->pre_walk_parameter(_parameter);
    typename Traits::type_type type = walk_type(_parameter->get_type());
    return m_consumer->walk_parameter(_parameter, type);
  }


  typename Traits::expression_type walk_array_allocation_expression(
      const array_allocation_expression*) {
    WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
    return typename Traits::expression_type();
  }

  typename Traits::expression_type walk_binary_expression(
      const binary_expression*) {
    WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
    return typename Traits::expression_type();
  }

  typename Traits::expression_type walk_cast_expression(
      const cast_expression*) {
    WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
    return typename Traits::expression_type();
  }

  typename Traits::expression_type walk_cond_expression(
      const cond_expression*) {
    WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
    return typename Traits::expression_type();
  }

  typename Traits::expression_type walk_constant_expression(
      const constant_expression*) {
    WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
    return typename Traits::expression_type();
  }

  typename Traits::expression_type walk_id_expression(const id_expression*) {
    WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
    return typename Traits::expression_type();
  }

  typename Traits::expression_type walk_null_allocation_expression(
      const null_allocation_expression* _expression) {
    m_consumer->pre_walk_expression(_expression);
    m_consumer->walk_expression(
        _expression,
        containers::contiguous_range<
            containers::contiguous_range<typename Traits::expression_type>>(),
        0);
    return typename Traits::expression_type();
  }

  typename Traits::expression_type walk_array_access_expression(
      const array_access_expression*) {
    WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
    return typename Traits::expression_type();
  }

  typename Traits::expression_type walk_function_call_expression(
      const function_call_expression*) {
    WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
    return typename Traits::expression_type();
  }

  typename Traits::expression_type walk_member_access_expression(
      const member_access_expression*) {
    WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
    return typename Traits::expression_type();
  }

  typename Traits::expression_type walk_post_unary_expression(
      const post_unary_expression*) {
    WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
    return typename Traits::expression_type();
  }

  typename Traits::expression_type walk_short_circuit_expression(
      const short_circuit_expression*) {
    WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
    return typename Traits::expression_type();
  }

  typename Traits::expression_type walk_struct_allocation_expression(
      const struct_allocation_expression*) {
    WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
    return typename Traits::expression_type();
  }

  typename Traits::expression_type walk_unary_expression(
      const unary_expression*) {
    WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
    return typename Traits::expression_type();
  }

  typename Traits::function_type walk_function(const function* _function) {
    m_consumer->pre_walk_function(_function);
    m_consumer->pre_walk_function_header(_function);

    typename Traits::parameter_type function_sig =
        walk_parameter(_function->get_signature());
    containers::dynamic_array<typename Traits::parameter_type> parameters(
        m_allocator);
    if (_function->get_parameters()) {
      parameters.reserve(
          _function->get_parameters()->get_parameters().size());
      for (const auto& parameter :
           _function->get_parameters()->get_parameters()) {
        parameters.emplace_back(walk_parameter(parameter.get()));
      }
    }

    typename Traits::function_header_type header =
        m_consumer->walk_function_header(_function, function_sig, parameters);

    containers::dynamic_array<typename Traits::instruction_type> body(
        m_allocator);
    body.reserve(_function->get_body()->get_instructions().size());
    for (const auto& instruction : _function->get_body()->get_instructions()) {
      body.emplace_back(walk_instruction(instruction.get()));
    }
    return m_consumer->walk_function(_function, header, body);
  }

  typename Traits::script_file_type walk_script_file(const script_file* _file) {
    WN_RELEASE_ASSERT_DESC(_file->get_includes().size() == 0,
                           "Not Implemented: Includes");
    WN_RELEASE_ASSERT_DESC(_file->get_structs().size() == 0,
                           "Not Implemented: Structs");
    m_consumer->pre_walk_script_file(_file);
    containers::dynamic_array<typename Traits::function_type> functions;
    containers::dynamic_array<typename Traits::include_type> includes;
    containers::dynamic_array<typename Traits::struct_definition_type> structs;

    functions.reserve(_file->get_functions().size());
    for (auto& function : _file->get_functions()) {
      functions.push_back(walk_function(function.get()));
    }

    return m_consumer->walk_script_file(
        _file, containers::contiguous_range<typename Traits::function_type>(
                   functions.data(), functions.size()),
        containers::contiguous_range<typename Traits::include_type>(
            includes.data(), includes.size()),
        containers::contiguous_range<typename Traits::struct_definition_type>(
            structs.data(), structs.size()));
  }

 private:
  T* m_consumer;
  scripting::type_validator* m_validator;
  memory::allocator* m_allocator;
  WNLogging::WNLog* m_log;
};
}
}

#endif  //__WN_SCRIPTING_AST_WALKER_H__
