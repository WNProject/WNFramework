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

  wn_size_t warnings() const {
    return m_num_warnings;
  }
  wn_size_t errors() const {
    return m_num_errors;
  }

protected:
  WNLogging::WNLog* m_log;
  memory::allocator* m_allocator;
  type_validator* m_validator;
  wn_size_t m_num_warnings;
  wn_size_t m_num_errors;
};

class dead_code_elimination_pass : public pass {
public:
  dead_code_elimination_pass(type_validator* _validator, WNLogging::WNLog* _log,
      memory::allocator* _allocator)
    : pass(_validator, _log, _allocator) {}
  void enter_scope_block() {}
  void leave_scope_block() {}
  void walk_expression(expression*) {}

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
  // TODO(awoloszyn): Handle declarations here
  //                  for now this just handles parameter ids.

  void walk_expression(expression*) {}
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

  void walk_expression(id_expression* _expr) {
    auto param = find_param(_expr->get_name());
    if (!param) {
      m_log->Log(
          WNLogging::eError, 0, "Could not find id: ", _expr->get_name());
      _expr->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return;
    }
    _expr->set_id_source(*param);
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
    return wn_nullptr;
  }

  containers::deque<
      containers::hash_map<containers::string_view, id_expression::id_source>>
      id_map;
};

class type_association_pass : public pass {
public:
  type_association_pass(type_validator* _validator, WNLogging::WNLog* _log,
      memory::allocator* _allocator)
    : pass(_validator, _log, _allocator), m_returns(_allocator) {}
  void walk_expression(expression*) {}
  void walk_parameter(parameter*) {}
  void enter_scope_block() {}
  void leave_scope_block() {}
  void walk_expression(id_expression* _expr) {
    const id_expression::id_source& source = _expr->get_id_source();
    if (source.param_source || source.declaration_source) {
      type* t = m_allocator->construct<type>(
          source.param_source ? *source.param_source->get_type()
                              : *source.declaration_source->get_type());
      t->copy_location_from(_expr);
      _expr->set_type(t);
    } else {
      WN_DEBUG_ASSERT_DESC(wn_false, "Source for this ID.");
    }
  }

  void walk_expression(constant_expression* _expr) {
    if (!_expr->get_type()) {
      type* t = m_allocator->construct<type>(m_allocator, _expr->get_index());
      t->copy_location_from(_expr);
      _expr->set_type(t);
    }
    // Actually validate the text.
    switch (_expr->get_type()->get_index()) {
      case static_cast<wn_uint32>(type_classification::int_type): {
        const wn_char* text = _expr->get_type_text().c_str();
        bool error = false;
        if (wn::memory::strnlen(text, 13) > 12) {
          error = true;
        } else {
          wn_int64 val = atoll(text);
          if (val < std::numeric_limits<wn_int32>::min() ||
              val > std::numeric_limits<wn_int32>::max()) {
            error = true;
          }
        }
        if (error) {
          m_log->Log(WNLogging::eError, 0, "Invalid integer constant ", text);
          m_num_errors += 1;
        }
        break;
      }
      case static_cast<wn_uint32>(type_classification::bool_type): {
        break;
      }
      default:
        WN_RELEASE_ASSERT_DESC(
            wn_false, "No Implemented: non-integer contants");
    }
  }

  void walk_expression(binary_expression* _expr) {
    const type* lhs_type = _expr->get_lhs()->get_type();
    const type* rhs_type = _expr->get_rhs()->get_type();

    if (lhs_type->get_index() != rhs_type->get_index()) {
      m_log->Log(WNLogging::eError, 0, "Expected LHS and RHS to match");
      _expr->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return;
    }

    uint32_t return_type =
        m_validator->get_operations(
                       static_cast<wn_int32>(lhs_type->get_index()))
            .get_operation(_expr->get_arithmetic_type());

    if (return_type ==
        static_cast<uint32_t>(type_classification::invalid_type)) {
      m_log->Log(WNLogging::eError, 0, "Invalid operation for types");
      _expr->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return;
    } else if (return_type ==
               static_cast<uint32_t>(type_classification::void_type)) {
      m_log->Log(WNLogging::eError, 0, "Void return is invalid for arithmetic");
      _expr->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return;
    }

    WN_RELEASE_ASSERT_DESC(
        return_type < static_cast<uint32_t>(type_classification::custom_type),
        "Not Implemented: Custom types");
    type* t = m_allocator->construct<type>(m_allocator, return_type);
    t->copy_location_from(_expr);
    _expr->set_type(t);
  }

  void walk_expression(member_access_expression* _expr) {
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
      return;
    }

    type* t = m_allocator->construct<type>(m_allocator, member_type);
    t->copy_location_from(_expr);
    _expr->set_type(t);
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
        m_validator->get_or_register_type(_definition->get_name());

    type_definition& def = m_validator->get_operations(type);
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
    if (_decl->get_type()->get_qualifier() == type_qualifier::non_nullable) {
      _decl->propagate_qualifier();
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
        static_cast<wn_uint32>(type_classification::bool_type)) {
      m_log->Log(WNLogging::eError, 0,
          "Else If conditional must be a boolean expression");
      _else_if->get_condition()->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
    }
  }

  void walk_instruction(if_instruction* _if) {
    if (_if->get_condition()->get_type()->get_index() !=
        static_cast<wn_uint32>(type_classification::bool_type)) {
      m_log->Log(
          WNLogging::eError, 0, "If conditional must be a boolean expression");
      _if->get_condition()->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
    }
  }

  void walk_function(function* _func) {
    const type* function_return_type = _func->get_signature()->get_type();
    if (function_return_type->get_index() >=
        static_cast<wn_uint32>(type_classification::custom_type)) {
      WN_RELEASE_ASSERT_DESC(false, "Not implented: custom return type");
    }

    for (const auto& return_inst : m_returns) {
      // TODO(awoloszyn): Add type mananger to do more rigorous type matching.
      if (!return_inst->get_expression()) {
        if (function_return_type->get_index() !=
            static_cast<wn_uint32>(type_classification::void_type)) {
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
    _func->set_mangled_name(m_validator->get_mangled_name(
        _func->get_signature()->get_name(), return_type, params));
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
};
}  // anonymous namespace

bool run_type_association_pass(script_file* _file, WNLogging::WNLog* _log,
    type_validator* _validator, wn_size_t* _num_warnings,
    wn_size_t* _num_errors) {
  if (!_file)
    return false;
  type_association_pass pass(_validator, _log, _file->get_allocator());
  run_ast_pass(&pass, _file);
  if (_num_errors) {
    *_num_errors += pass.errors();
  }
  if (_num_warnings) {
    *_num_warnings += pass.warnings();
  }
  return pass.errors() == 0;
}

bool run_id_association_pass(script_file* _file, WNLogging::WNLog* _log,
    type_validator* _validator, wn_size_t* _num_warnings,
    wn_size_t* _num_errors) {
  if (!_file)
    return false;
  id_association_pass pass(_validator, _log, _file->get_allocator());
  run_ast_pass(&pass, _file);
  if (_num_errors) {
    *_num_errors += pass.errors();
  }
  if (_num_warnings) {
    *_num_warnings += pass.warnings();
  }
  return pass.errors() == 0;
}

bool run_dce_pass(script_file* _file, WNLogging::WNLog* _log,
    type_validator* _validator, wn_size_t* _num_warnings,
    wn_size_t* _num_errors) {
  if (!_file)
    return false;
  dead_code_elimination_pass pass(_validator, _log, _file->get_allocator());
  run_ast_pass(&pass, _file);
  if (_num_errors) {
    *_num_errors += pass.errors();
  }
  if (_num_warnings) {
    *_num_warnings += pass.warnings();
  }
  return pass.errors() == 0;
}

}  // namespace scripting
}  // namespace wn