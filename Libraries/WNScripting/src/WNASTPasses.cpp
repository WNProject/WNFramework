#include "WNScripting/inc/WNASTPasses.h"
#include "WNScripting/inc/WNASTWalker.h"
#include "WNScripting/inc/WNNodeTypes.h"
#include "WNLogging/inc/WNLog.h"

namespace wn {
namespace scripting {

namespace {
class id_association_pass {
 public:
  id_association_pass(WNLogging::WNLog* _log, memory::allocator* _allocator)
      : m_log(_log),
        m_num_warnings(0),
        m_num_errors(0),
        m_allocator(_allocator),
        id_map(_allocator) {}
  // TODO(awoloszyn): Add scope handling here.
  // TODO(awoloszyn): Handle declarations here
  //                  for now this just handles parameter ids.

  void walk_expression(expression*) {}
  void walk_instruction(instruction*) {}
  void walk_function(function*) { id_map.clear(); }
  void walk_parameter(parameter* _param) {
    if (id_map.find(_param->get_name()) != id_map.end()) {
      m_log->Log(WNLogging::eError, 0, "Id ", _param->get_name(),
                 "hides id of the same name");
      _param->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return;
    }
    id_map[_param->get_name()] = {_param, 0};
  }

  void walk_expression(id_expression* _expr) {
    auto it = id_map.find(_expr->get_name());
    if (it == id_map.end()) {
      m_log->Log(WNLogging::eError, 0, "Could not find id: ",
                 _expr->get_name());
      _expr->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return;
    }
    _expr->set_id_source(it->second);
  }

  void walk_script_file(script_file*) {}
  void walk_type(type*) {}

  wn_size_t warnings() const { return m_num_warnings; }
  wn_size_t errors() const { return m_num_errors; }

 private:
  WNLogging::WNLog* m_log;
  memory::allocator* m_allocator;
  wn_size_t m_num_warnings;
  wn_size_t m_num_errors;
  containers::hash_map<containers::string_view, id_expression::id_source>
      id_map;
};

class type_association_pass {
 public:
  type_association_pass(WNLogging::WNLog* _log, memory::allocator* _allocator)
      : m_log(_log),
        m_num_warnings(0),
        m_num_errors(0),
        m_allocator(_allocator) {}
  void walk_expression(expression*) {}
  void walk_parameter(parameter*) {}
  void walk_expression(constant_expression* _expr) {
    if (!_expr->get_type()) {
      WN_RELEASE_ASSERT_DESC(
          _expr->get_classification() != type_classification::custom_type,
          "Classification should have been set during ast generation");
      type* t = m_allocator->make_allocated<type>(m_allocator,
                                                  _expr->get_classification());
      t->copy_location_from(_expr);
      _expr->set_type(t);
    }
  }

  void walk_expression(binary_expression* _expr) {
    const type* lhs_type = _expr->get_lhs()->get_type();
    const type* rhs_type = _expr->get_rhs()->get_type();
    if (lhs_type->get_classification() ==
        scripting::type_classification::void_type) {
      m_log->Log(WNLogging::eError, 0,
                 "Cannot perform arithmetic on void type");
      _expr->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return;
    }
    if (lhs_type->get_classification() != rhs_type->get_classification()) {
      m_log->Log(WNLogging::eError, 0, "Expected LHS and RHS to match");
      _expr->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return;
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
        t = m_allocator->make_allocated<type>(m_allocator,
                                              type_classification::bool_type);
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
  void walk_function(function*) {}
  void walk_script_file(script_file*) {}
  void walk_type(type*) {}

  wn_size_t warnings() const { return m_num_warnings; }
  wn_size_t errors() const { return m_num_errors; }

 private:
  WNLogging::WNLog* m_log;
  wn_size_t m_num_warnings;
  wn_size_t m_num_errors;
  memory::allocator* m_allocator;

  // TODO(awoloszyn): Add type_manager here once we start
  //                 caring about custom types.
};
}  // anonymous namespace

bool run_type_association_pass(script_file* _file, WNLogging::WNLog* _log) {
  if (!_file) return false;
  type_association_pass pass(_log, _file->get_allocator());
  run_ast_pass(&pass, _file);
  return pass.errors() == 0;
}

bool run_id_association_pass(script_file* _file, WNLogging::WNLog* _log) {
  if (!_file) return false;
  id_association_pass pass(_log, _file->get_allocator());
  run_ast_pass(&pass, _file);
  return pass.errors() == 0;
}
}  // namespace scripting
}  // namespace wn