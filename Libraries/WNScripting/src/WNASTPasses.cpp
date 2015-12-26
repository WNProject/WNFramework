#include "WNLogging/inc/WNLog.h"
#include "WNScripting/inc/WNASTPasses.h"
#include "WNScripting/inc/WNASTWalker.h"
#include "WNScripting/inc/WNNodeTypes.h"

namespace wn {
namespace scripting {

namespace {

class pass {
public:
  pass(WNLogging::WNLog* _log, memory::allocator* _allocator)
    : m_log(_log),
      m_num_warnings(0),
      m_num_errors(0),
      m_allocator(_allocator) {}

  wn_size_t warnings() const {
    return m_num_warnings;
  }
  wn_size_t errors() const {
    return m_num_errors;
  }

protected:
  WNLogging::WNLog* m_log;
  memory::allocator* m_allocator;
  wn_size_t m_num_warnings;
  wn_size_t m_num_errors;
};

class dead_code_elimination_pass : public pass {
public:
  dead_code_elimination_pass(
      WNLogging::WNLog* _log, memory::allocator* _allocator)
    : pass(_log, _allocator) {}
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
  id_association_pass(WNLogging::WNLog* _log, memory::allocator* _allocator)
    : pass(_log, _allocator), id_map(_allocator) {}
  // TODO(awoloszyn): Handle declarations here
  //                  for now this just handles parameter ids.

  void walk_expression(expression*) {}
  void walk_instruction(instruction*) {}
  void walk_instruction_list(instruction_list*) {}
  void walk_function(function*) {}

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
  type_association_pass(WNLogging::WNLog* _log, memory::allocator* _allocator)
    : pass(_log, _allocator) {}
  void walk_expression(expression*) {}
  void walk_parameter(parameter*) {}
  void enter_scope_block() {}
  void leave_scope_block() {}
  void walk_expression(id_expression* _expr) {
    const id_expression::id_source& source = _expr->get_id_source();
    if (source.param_source || source.declaration_source) {
      type* t = m_allocator->make_allocated<type>(
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
      WN_RELEASE_ASSERT_DESC(
          _expr->get_classification() != type_classification::custom_type,
          "Classification should have been set during ast generation");
      type* t = m_allocator->make_allocated<type>(
          m_allocator, _expr->get_classification());
      t->copy_location_from(_expr);
      _expr->set_type(t);
    }
    // Actually validate the text.
    switch (_expr->get_type()->get_classification()) {
      case type_classification::int_type: {
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
      case type_classification::bool_type: {
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
    if (lhs_type->get_classification() ==
        scripting::type_classification::void_type) {
      m_log->Log(
          WNLogging::eError, 0, "Cannot perform arithmetic on void type");
      _expr->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      // Continue so that more errors can be caught.
    }
    if (lhs_type->get_classification() != rhs_type->get_classification()) {
      m_log->Log(WNLogging::eError, 0, "Expected LHS and RHS to match");
      _expr->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      // Continue so that more errors can be caught.
    }
    if (lhs_type->get_classification() == type_classification::bool_type &&
        (_expr->get_arithmetic_type() != arithmetic_type::arithmetic_equal &&
            _expr->get_arithmetic_type() !=
                arithmetic_type::arithmetic_not_equal)) {
      m_log->Log(WNLogging::eError, 0,
          "The only valid operations on boolean types are == and !=");
      _expr->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      // Continue so that more errors can be caught.
    }
    type* t = wn_nullptr;
    switch (_expr->get_arithmetic_type()) {
      case arithmetic_type::arithmetic_add:
      case arithmetic_type::arithmetic_sub:
      case arithmetic_type::arithmetic_mult:
      case arithmetic_type::arithmetic_div:
      case arithmetic_type::arithmetic_mod:
        t = m_allocator->make_allocated<type>(*lhs_type);
        break;
      // type is LHS
      case arithmetic_type::arithmetic_equal:
      case arithmetic_type::arithmetic_not_equal:
      case arithmetic_type::arithmetic_less_than_or_equal:
      case arithmetic_type::arithmetic_greater_than_or_equal:
      case arithmetic_type::arithmetic_less_than:
      case arithmetic_type::arithmetic_greater_than:
        t = m_allocator->make_allocated<type>(
            m_allocator, type_classification::bool_type);
        break;
      // type is bool
      case arithmetic_type::max:
        WN_RELEASE_ASSERT_DESC(false, "Invalid arithmetic type");
    }
    if (t) {
      t->copy_location_from(_expr);
    }
    _expr->set_type(t);
  }

  void walk_instruction(instruction*) {}
  void walk_instruction_list(instruction_list*) {}
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
    if (*_decl->get_type() != *_decl->get_expression()->get_type()) {
      m_log->Log(WNLogging::eError, 0,
          "Cannot inintialize variable with expression of a different type.");
      _decl->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
    }
  }

  void walk_instruction(else_if_instruction* _else_if) {
    if (_else_if->get_condition()->get_type()->get_classification() !=
        type_classification::bool_type) {
      m_log->Log(WNLogging::eError, 0,
          "Else If conditional must be a boolean expression");
      _else_if->get_condition()->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
    }
  }

  void walk_instruction(if_instruction* _if) {
    if (_if->get_condition()->get_type()->get_classification() !=
        type_classification::bool_type) {
      m_log->Log(
          WNLogging::eError, 0, "If conditional must be a boolean expression");
      _if->get_condition()->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
    }
  }

  void walk_function(function* _func) {
    const type* function_return_type = _func->get_signature()->get_type();
    if (function_return_type->get_classification() ==
        type_classification::custom_type) {
      WN_RELEASE_ASSERT_DESC(false, "Not implented: custom return type");
    }

    for (auto& return_inst : m_returns) {
      // TODO(awoloszyn): Add type mananger to do more rigorous type matching.
      if (!return_inst->get_expression()) {
        if (function_return_type->get_classification() !=
            type_classification::void_type) {
          m_log->Log(WNLogging::eError, 0, "Expected non-void return");
          return_inst->log_line(*m_log, WNLogging::eError);
          m_num_errors += 1;
        }
      } else {
        const type* return_type = return_inst->get_expression()->get_type();
        if (*return_type != *function_return_type) {
          m_log->Log(
              WNLogging::eError, 0, "Return type does not match function");
          return_inst->log_line(*m_log, WNLogging::eError);
          m_num_errors += 1;
        }
      }
    }
  }

  void walk_script_file(script_file*) {}
  void walk_type(type*) {}

private:
  containers::deque<return_instruction*> m_returns;
  // TODO(awoloszyn): Add type_manager here once we start
  //                 caring about custom types.
};
}  // anonymous namespace

bool run_type_association_pass(script_file* _file, WNLogging::WNLog* _log,
    wn_size_t* _num_warnings, wn_size_t* _num_errors) {
  if (!_file)
    return false;
  type_association_pass pass(_log, _file->get_allocator());
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
    wn_size_t* _num_warnings, wn_size_t* _num_errors) {
  if (!_file)
    return false;
  id_association_pass pass(_log, _file->get_allocator());
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
    wn_size_t* _num_warnings, wn_size_t* _num_errors) {
  if (!_file)
    return false;
  dead_code_elimination_pass pass(_log, _file->get_allocator());
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