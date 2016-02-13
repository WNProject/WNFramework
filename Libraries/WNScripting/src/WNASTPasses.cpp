#include "WNLogging/inc/WNLog.h"
#include "WNScripting/inc/WNASTPasses.h"
#include "WNScripting/inc/WNASTWalker.h"
#include "WNScripting/inc/WNNodeTypes.h"

namespace wn {
namespace scripting {

namespace {

class pass {
public:
  pass(type_validator* _validator, WNLogging::WNLog* _log,
      memory::allocator* _allocator)
    : m_log(_log),
      m_allocator(_allocator),
      m_validator(_validator),
      m_num_warnings(0),
      m_num_errors(0) {}

  size_t warnings() const {
    return m_num_warnings;
  }
  size_t errors() const {
    return m_num_errors;
  }

protected:
  WNLogging::WNLog* m_log;
  memory::allocator* m_allocator;
  type_validator* m_validator;
  size_t m_num_warnings;
  size_t m_num_errors;
};

class dead_code_elimination_pass : public pass {
public:
  dead_code_elimination_pass(type_validator* _validator, WNLogging::WNLog* _log,
      memory::allocator* _allocator)
    : pass(_validator, _log, _allocator) {}
  void enter_scope_block() {}
  void leave_scope_block() {}
  memory::unique_ptr<expression> walk_expression(expression*) {
    return nullptr;
  }

  void walk_instruction_list(instruction_list* _inst) {
    bool returns = false;
    for (auto& inst : _inst->get_instructions()) {
      if (returns) {
        inst->set_is_dead();
        m_log->Log(WNLogging::eWarning, 0, "Code after return statement");
        inst->log_line(*m_log, WNLogging::eWarning);
        ++m_num_warnings;
      }
      returns |= inst->returns();
    }
    if (returns) {
      _inst->set_returns();
    }
    _inst->remove_dead_instructions();
  }

  void walk_function(function*) {}
  void walk_parameter(parameter*) {}
  void walk_script_file(script_file*) {}
  void walk_type(type*) {}
  void walk_instruction(instruction*) {}
  void walk_struct_definition(struct_definition*) {}
  void walk_instruction(else_if_instruction* _inst) {
    _inst->set_returns(_inst->get_body()->returns());
  }

  void walk_instruction(if_instruction* _inst) {
    if (!_inst->get_else()) {
      // If we do not have an else clause, then we
      // have an out (since we have not done constant
      // propagation yet).
      return;
    }

    bool returns = _inst->get_body()->returns();
    for (auto& else_inst : _inst->get_else_if_instructions()) {
      returns &= else_inst->returns();
    }
    returns &= _inst->get_else()->returns();
    _inst->set_returns(returns);
  }

private:
};

class id_association_pass : public pass {
public:
  id_association_pass(type_validator* _validator, WNLogging::WNLog* _log,
      memory::allocator* _allocator)
    : pass(_validator, _log, _allocator), id_map(_allocator) {}

  memory::unique_ptr<expression> walk_expression(expression*) {
    return nullptr;
  }
  void walk_instruction_list(instruction_list*) {}
  void walk_instruction(instruction*) {}
  void walk_function(function*) {}
  void walk_struct_definition(struct_definition*) {}

  void enter_scope_block() {
    id_map.emplace_back(
        containers::hash_map<containers::string_view, id_expression::id_source>(
            m_allocator));
  }

  void leave_scope_block() {
    // TODO(awoloszyn): Once we have object support, we will need to
    //                  add dereference operations on scope leave.
    id_map.pop_back();
  }

  void walk_parameter(parameter* _param) {
    if (find_param(_param->get_name())) {
      m_log->Log(WNLogging::eError, 0, "Id ", _param->get_name(),
          "hides id of the same name");
      _param->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return;
    }
    id_map.back()[_param->get_name()] = {_param, 0};
  }

  void walk_instruction(declaration* _decl) {
    if (find_param(_decl->get_name())) {
      m_log->Log(WNLogging::eError, 0, "Declaration of ", _decl->get_name(),
          "hides id of the same name");
      _decl->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return;
    }
    id_map.back()[_decl->get_name()] = {0, _decl};
  }

  memory::unique_ptr<expression> walk_expression(id_expression* _expr) {
    auto param = find_param(_expr->get_name());
    if (!param) {
      m_log->Log(
          WNLogging::eError, 0, "Could not find id: ", _expr->get_name());
      _expr->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return nullptr;
    }
    _expr->set_id_source(*param);
    return nullptr;
  }

  void walk_script_file(script_file*) {}
  void walk_type(type*) {}

private:
  const id_expression::id_source* find_param(
      const containers::string_view& v) const {
    for (auto it = id_map.cbegin(); it != id_map.cend(); ++it) {
      auto val = it->find(v);
      if (val != it->end()) {
        return &(val->second);
      }
    }
    return nullptr;
  }

  containers::deque<
      containers::hash_map<containers::string_view, id_expression::id_source>>
      id_map;
};

class type_association_pass : public pass {
public:
  type_association_pass(type_validator* _validator, WNLogging::WNLog* _log,
      memory::allocator* _allocator)
    : pass(_validator, _log, _allocator),
      m_returns(_allocator),
      m_functions(_allocator) {}
  memory::unique_ptr<expression> walk_expression(expression*) {
    return nullptr;
  }

  void walk_parameter(parameter*) {}
  void enter_scope_block() {}
  void leave_scope_block() {}
  memory::unique_ptr<expression> walk_expression(id_expression* _expr) {
    const id_expression::id_source& source = _expr->get_id_source();
    if (source.param_source || source.declaration_source) {
      type* t = m_allocator->construct<type>(
          source.param_source ? *source.param_source->get_type()
                              : *source.declaration_source->get_type());
      t->copy_location_from(_expr);
      _expr->set_type(t);
    } else {
      WN_DEBUG_ASSERT_DESC(false, "Source for this ID.");
    }
    return nullptr;
  }

  memory::unique_ptr<expression> walk_expression(constant_expression* _expr) {
    if (!_expr->get_type()) {
      type* t = m_allocator->construct<type>(m_allocator, _expr->get_index());
      t->copy_location_from(_expr);
      _expr->set_type(t);
    }
    // Actually validate the text.
    switch (_expr->get_type()->get_index()) {
      case static_cast<uint32_t>(type_classification::int_type): {
        const char* text = _expr->get_type_text().c_str();
        bool error = false;
        if (wn::memory::strnlen(text, 13) > 12) {
          error = true;
        } else {
          int64_t val = atoll(text);
          if (val < std::numeric_limits<int32_t>::min() ||
              val > std::numeric_limits<int32_t>::max()) {
            error = true;
          }
        }
        if (error) {
          m_log->Log(WNLogging::eError, 0, "Invalid integer constant ", text);
          m_num_errors += 1;
        }
        break;
      }
      case static_cast<uint32_t>(type_classification::bool_type): {
        break;
      }
      default:
        WN_RELEASE_ASSERT_DESC(false, "No Implemented: non-integer contants");
    }
    return nullptr;
  }

  memory::unique_ptr<expression> walk_expression(binary_expression* _expr) {
    const type* lhs_type = _expr->get_lhs()->get_type();
    const type* rhs_type = _expr->get_rhs()->get_type();

    if (lhs_type->get_index() != rhs_type->get_index()) {
      m_log->Log(WNLogging::eError, 0, "Expected LHS and RHS to match");
      _expr->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return nullptr;
    }

    uint32_t return_type =
        m_validator->get_operations(static_cast<int32_t>(lhs_type->get_index()))
            .get_operation(_expr->get_arithmetic_type());

    if (return_type ==
        static_cast<uint32_t>(type_classification::invalid_type)) {
      m_log->Log(WNLogging::eError, 0, "Invalid operation for types");
      _expr->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return nullptr;
    } else if (return_type ==
               static_cast<uint32_t>(type_classification::void_type)) {
      m_log->Log(WNLogging::eError, 0, "Void return is invalid for arithmetic");
      _expr->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return nullptr;
    }

    WN_RELEASE_ASSERT_DESC(
        return_type < static_cast<uint32_t>(type_classification::custom_type),
        "Not Implemented: Custom types");
    type* t = m_allocator->construct<type>(m_allocator, return_type);
    t->copy_location_from(_expr);
    _expr->set_type(t);

    return nullptr;
  }

  memory::unique_ptr<expression> walk_expression(
      member_access_expression* _expr) {
    uint32_t type_index = 0;
    const type* obj = _expr->get_base_expression()->get_type();
    if (obj) {
      type_index = obj->get_type_value();
    }
    const type_definition& def = m_validator->get_operations(type_index);
    uint32_t member_type = def.get_member_id(_expr->get_name());
    if (member_type == 0) {
      m_log->Log(WNLogging::eError, 0, "Type does not have member: ",
          _expr->get_name());
      _expr->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return nullptr;
    }

    type* t = m_allocator->construct<type>(m_allocator, member_type);
    t->copy_location_from(_expr);
    _expr->set_type(t);

    return nullptr;
  }

  memory::unique_ptr<expression> walk_expression(
      function_call_expression* _expr) {
    const expression* base_expr = _expr->get_base_expression();
    // TODO(awoloszyn): handle the other cases that give you functions (are
    // there any?)
    if (base_expr->get_node_type() != node_type::id_expression) {
      m_log->Log(WNLogging::eError, 0, "Cannot call function on non-id: ");
      _expr->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return nullptr;
    }

    const id_expression* id = static_cast<const id_expression*>(base_expr);
    auto possible_function = m_functions.find(id->get_name());
    if (possible_function == m_functions.end()) {
      m_log->Log(
          WNLogging::eError, 0, "Cannot find function: ", id->get_name());
      _expr->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return nullptr;
    }

    function* callee = nullptr;
    const auto& parameters = _expr->get_expressions();

    for (auto func : possible_function->second) {
      if (parameters.size() != 0) {
        if (!func->get_parameters()) {
          continue;
        }

        const auto& function_parameters =
            func->get_parameters()->get_parameters();
        if (function_parameters.size() != parameters.size()) {
          continue;
        }
        size_t i = 0;
        for (; i < parameters.size(); ++i) {
          if (*parameters[i]->m_expr->get_type() !=
              *function_parameters[i]->get_type()) {
            break;
          }
        }
        if (i != parameters.size()) {
          continue;
        }
      } else if (func->get_parameters() &&
                 func->get_parameters()->get_parameters().size() != 0) {
        continue;
      }
      if (callee) {
        m_log->Log(
            WNLogging::eError, 0, "Ambiguous Function call: ", id->get_name());
        _expr->log_line(*m_log, WNLogging::eError);
        ++m_num_errors;
        return nullptr;
      }
      callee = func;
    }
    // TODO(awoloszyn): Insert explicit cast operations if we need do here.
    _expr->set_callee(callee);
    type* t =
        m_allocator->construct<type>(*callee->get_signature()->get_type());
    t->copy_location_from(_expr);
    _expr->set_type(t);
    return nullptr;
  }

  void walk_instruction(instruction*) {}
  void walk_instruction_list(instruction_list*) {}
  void walk_struct_definition(struct_definition* _definition) {
    if (m_validator->get_type(_definition->get_name())) {
      m_log->Log(WNLogging::eError, 0, "Duplicate struct defined: ",
          _definition->get_name());
      _definition->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return;
    }

    const uint32_t type =
        m_validator->register_struct_type(_definition->get_name());

    type_definition& def = m_validator->get_operations(
        type + static_cast<uint32_t>(reference_type::unique));

    for (const auto& a : _definition->get_struct_members()) {
      if (!def.register_sub_type(a->get_name(), a->get_type()->get_index())) {
        m_log->Log(WNLogging::eError, 0, "Duplicate struct element defined: ",
            a->get_name());
        a->log_line(*m_log, WNLogging::eError);
        ++m_num_errors;
        return;
      }
    }
  }

  void walk_instruction(assignment_instruction* _assign) {
    if (_assign->get_assignment_type() != assign_type::equal) {
      WN_RELEASE_ASSERT_DESC(
          false, "Not Implemented: Non-equality assignment instructions.");
    }

    if (*_assign->get_lvalue()->get_expression()->get_type() !=
        *_assign->get_expression()->get_type()) {
      m_log->Log(WNLogging::eError, 0, "Cannot assign incompatible types.");
      _assign->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
    }
  }

  void walk_instruction(return_instruction* _ret) {
    m_returns.push_back(_ret);
  }

  void walk_instruction(declaration* _decl) {
    // TODO(awoloszyn): Handle types other than non_nullable.
    if (_decl->get_type()->get_reference_type() == reference_type::unique) {
      _decl->propagate_reference_type();
    }
    if (_decl->get_type()->get_index() !=
        _decl->get_expression()->get_type()->get_index()) {
      m_log->Log(WNLogging::eError, 0,
          "Cannot inintialize variable with expression of a different type.");
      _decl->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
    }
  }

  void walk_instruction(else_if_instruction* _else_if) {
    if (_else_if->get_condition()->get_type()->get_index() !=
        static_cast<uint32_t>(type_classification::bool_type)) {
      m_log->Log(WNLogging::eError, 0,
          "Else If conditional must be a boolean expression");
      _else_if->get_condition()->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
    }
  }

  void walk_instruction(if_instruction* _if) {
    if (_if->get_condition()->get_type()->get_index() !=
        static_cast<uint32_t>(type_classification::bool_type)) {
      m_log->Log(
          WNLogging::eError, 0, "If conditional must be a boolean expression");
      _if->get_condition()->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
    }
  }

  void walk_function(function* _func) {
    const type* function_return_type = _func->get_signature()->get_type();
    if (function_return_type->get_index() >=
        static_cast<uint32_t>(type_classification::custom_type)) {
      WN_RELEASE_ASSERT_DESC(false, "Not implented: custom return type");
    }

    for (const auto& return_inst : m_returns) {
      // TODO(awoloszyn): Add type mananger to do more rigorous type matching.
      if (!return_inst->get_expression()) {
        if (function_return_type->get_index() !=
            static_cast<uint32_t>(type_classification::void_type)) {
          m_log->Log(WNLogging::eError, 0, "Expected non-void return");
          return_inst->log_line(*m_log, WNLogging::eError);
          m_num_errors += 1;
        }
      } else {
        const type* return_type = return_inst->get_expression()->get_type();
        // If another error has occurred then return_type may not have been
        // set up.
        if (return_type) {
          if (*return_type != *function_return_type) {
            m_log->Log(
                WNLogging::eError, 0, "Return type does not match function");
            return_inst->log_line(*m_log, WNLogging::eError);
            m_num_errors += 1;
          }
        }
      }
    }

    containers::dynamic_array<uint32_t> params(m_allocator);
    uint32_t return_type =
        static_cast<uint32_t>(_func->get_signature()->get_type()->get_index());
    if (_func->get_parameters()) {
      for (const auto& param : _func->get_parameters()->get_parameters()) {
        params.push_back(static_cast<uint32_t>(param->get_type()->get_index()));
      }
    }

    containers::string_view name = _func->get_signature()->get_name();
    containers::string mangled_name =
        m_validator->get_mangled_name(name, return_type, params);
    _func->set_mangled_name(mangled_name);

    auto function_it = m_functions.find(name);
    if (function_it == m_functions.end()) {
      std::tie(function_it, std::ignore) = m_functions.insert(
          std::make_pair(name, containers::deque<function*>(m_allocator)));
    }

    if (std::find_if(function_it->second.begin(), function_it->second.end(),
            [&mangled_name](function* _f) {
              return (_f->get_mangled_name() == mangled_name);
            }) != function_it->second.end()) {
      m_log->Log(WNLogging::eError, 0, "Conflicting function found: ",
          containers::string_view(_func->get_mangled_name()));
      _func->log_line(*m_log, WNLogging::eError);
      m_num_errors += 1;
    } else {
      function_it->second.push_back(_func);
    }
  }

  void walk_script_file(script_file*) {}
  void walk_type(type* _type) {
    if (_type->get_index() == 0) {
      uint32_t type_index = m_validator->get_type(_type->custom_type_name());
      if (type_index) {
        _type->set_type_index(type_index);
      } else {
        m_log->Log(
            WNLogging::eError, 0, "Unknown Type: ", _type->custom_type_name());
        _type->log_line(*m_log, WNLogging::eError);
        m_num_errors += 1;
      }
    }
  }

private:
  containers::deque<return_instruction*> m_returns;
  containers::hash_map<containers::string_view, containers::deque<function*>>
      m_functions;
};

class member_reassociation_pass : public pass {
public:
  member_reassociation_pass(type_validator* _validator, WNLogging::WNLog* _log,
      memory::allocator* _allocator)
    : pass(_validator, _log, _allocator) {}

  memory::unique_ptr<expression> walk_expression(expression*) {
    return nullptr;
  }
  void walk_parameter(parameter*) {}
  void walk_instruction_list(instruction_list*) {}

  void walk_script_file(script_file* _file) {
    for (auto& func : m_additional_functions) {
      _file->add_function(std::move(func));
    }
    m_additional_functions.clear();
  }

  // Turn
  // struct X {
  //   Int x = 4;
  //   Int y = 3;
  // }
  //
  // Into:
  // struct X {
  //   Int x;
  //   Int y;
  // }
  //
  // X _construct_X(X _this) {
  //   _this.x = 4;
  //   _this.y = 3;
  //   return _this;
  // }
  // Note this is in roughly WNScript syntax, so _this is passed by
  // reference.
  // TODO(awoloszyn): Add a "self" identifier in the language.
  // TODO(awoloszyn): In the id-association pass, start promoting
  // member access. Ie. allow struct X { int x = 4; int y = x + 3; }

  void walk_struct_definition(struct_definition* _definition) {
    memory::unique_ptr<instruction_list> instructions =
        memory::make_unique<instruction_list>(m_allocator, m_allocator);
    instructions->copy_location_from(_definition);

    // Make all of the instructions to put into the constructor.
    // This means: Remove all of the right-hand-sides from the
    // declarations, and create assignment instructions in an instruction list.
    for (auto& a : _definition->get_struct_members()) {
      memory::unique_ptr<expression> expression(a->take_expression());

      memory::unique_ptr<member_access_expression> access =
          memory::make_unique<member_access_expression>(
              m_allocator, m_allocator, a->get_name());
      access->copy_location_from(a.get());

      memory::unique_ptr<id_expression> id =
          memory::make_unique<id_expression>(m_allocator, m_allocator, "_this");
      id->copy_location_from(a.get());

      access->add_base_expression(std::move(id));

      memory::unique_ptr<lvalue> lval = memory::make_unique<lvalue>(
          m_allocator, m_allocator, std::move(access));
      lval->copy_location_from(a.get());

      memory::unique_ptr<assignment_instruction> assignment =
          memory::make_unique<assignment_instruction>(
              m_allocator, m_allocator, std::move(lval));
      assignment->copy_location_from(expression.get());
      assignment->add_value(assign_type::equal, std::move(expression));
      instructions->add_instruction(std::move(assignment));
    }

    memory::unique_ptr<id_expression> id =
        memory::make_unique<id_expression>(m_allocator, m_allocator, "_this");
    id->copy_location_from(_definition);

    memory::unique_ptr<return_instruction> ret =
        memory::make_unique<return_instruction>(
            m_allocator, m_allocator, std::move(id));
    ret->copy_location_from(_definition);
    instructions->add_instruction(std::move(ret));

    containers::string_view struct_name = _definition->get_name();

    memory::unique_ptr<type> parameter_type = memory::make_unique<type>(
        m_allocator, m_allocator, _definition->get_name());
    parameter_type->copy_location_from(_definition);

    // The constructor return_type and name are here.
    memory::unique_ptr<type> return_type =
        memory::make_unique<type>(m_allocator, *parameter_type);
    return_type->copy_location_from(parameter_type.get());

    containers::string m_constructor_name(m_allocator);
    m_constructor_name.append("_construct_");
    m_constructor_name.append(struct_name.data(), struct_name.length());
    memory::unique_ptr<parameter> name_ret = memory::make_unique<parameter>(
        m_allocator, m_allocator, std::move(return_type), m_constructor_name);
    name_ret->copy_location_from(parameter_type.get());

    memory::unique_ptr<parameter> this_param = memory::make_unique<parameter>(
        m_allocator, m_allocator, std::move(parameter_type), "_this");
    this_param->copy_location_from(_definition);

    memory::unique_ptr<parameter_list> parameters =
        memory::make_unique<parameter_list>(
            m_allocator, m_allocator, std::move(this_param));
    parameters->copy_location_from(_definition);

    memory::unique_ptr<function> constructor = memory::make_unique<function>(
        m_allocator, m_allocator, std::move(name_ret), std::move(parameters),
        std::move(instructions));
    constructor->copy_location_from(_definition);
    constructor->set_this_pointer(_definition);

    m_additional_functions.push_back(std::move(constructor));
  }

  void walk_type(type*) {}
  void walk_function(function*) {}
  void walk_instruction(instruction*) {}
  void enter_scope_block() {}
  void leave_scope_block() {}

private:
  containers::deque<memory::unique_ptr<function>> m_additional_functions;
  // TODO(awoloszyn): Once inline constructors are supported,
  // there will likely be inline types here, handle that.
};

}  // anonymous namespace

template <typename T>
bool run_pass(script_file* _file, WNLogging::WNLog* _log,
    type_validator* _validator, size_t* _num_warnings, size_t* _num_errors) {
  if (!_file)
    return false;
  T pass(_validator, _log, _file->get_allocator());
  run_ast_pass(&pass, _file);
  if (_num_errors) {
    *_num_errors += pass.errors();
  }
  if (_num_warnings) {
    *_num_warnings += pass.warnings();
  }
  return pass.errors() == 0;
}

bool run_type_association_pass(script_file* _file, WNLogging::WNLog* _log,
    type_validator* _validator, size_t* _num_warnings, size_t* _num_errors) {
  return run_pass<type_association_pass>(
      _file, _log, _validator, _num_warnings, _num_errors);
}

bool run_id_association_pass(script_file* _file, WNLogging::WNLog* _log,
    type_validator* _validator, size_t* _num_warnings, size_t* _num_errors) {
  return run_pass<id_association_pass>(
      _file, _log, _validator, _num_warnings, _num_errors);
}

bool run_dce_pass(script_file* _file, WNLogging::WNLog* _log,
    type_validator* _validator, size_t* _num_warnings, size_t* _num_errors) {
  return run_pass<dead_code_elimination_pass>(
      _file, _log, _validator, _num_warnings, _num_errors);
}

bool run_member_reassociation_pass(script_file* _file, WNLogging::WNLog* _log,
    type_validator* _validator, size_t* _num_warnings, size_t* _num_errors) {
  return run_pass<member_reassociation_pass>(
      _file, _log, _validator, _num_warnings, _num_errors);
}

}  // namespace scripting
}  // namespace wn