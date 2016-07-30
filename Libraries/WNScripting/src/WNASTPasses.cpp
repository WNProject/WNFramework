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
      m_last_temporary(0),
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
  uint32_t m_last_temporary;
  size_t m_num_warnings;
  size_t m_num_errors;
};

class dead_code_elimination_pass : public pass {
public:
  dead_code_elimination_pass(type_validator* _validator, WNLogging::WNLog* _log,
      memory::allocator* _allocator)
    : pass(_validator, _log, _allocator) {}
  void enter_scope_block(const node*) {}
  void leave_scope_block(const node*) {}
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
      _inst->set_returns(returns);
    }
    _inst->remove_dead_instructions();
  }

  void walk_function(function*) {}
  void walk_parameter(parameter*) {}

  void pre_walk_script_file(script_file*) {}
  void walk_script_file(script_file*) {}
  void walk_type(type*) {}
  memory::unique_ptr<instruction> walk_instruction(instruction*) {
    return nullptr;
  }
  void walk_struct_definition(struct_definition*) {}
  memory::unique_ptr<instruction> walk_instruction(else_if_instruction* _inst) {
    _inst->set_returns(_inst->get_body()->returns());
    return nullptr;
  }

  memory::unique_ptr<instruction> walk_instruction(if_instruction* _inst) {
    if (!_inst->get_else()) {
      // If we do not have an else clause, then we
      // have an out (since we have not done constant
      // propagation yet).
      return nullptr;
    }

    bool returns = _inst->get_body()->returns();
    for (auto& else_inst : _inst->get_else_if_instructions()) {
      returns &= else_inst->returns();
    }
    returns &= _inst->get_else()->returns();
    _inst->set_returns(returns);
    return nullptr;
  }

private:
};

memory::unique_ptr<constant_expression> make_constant(memory::allocator* _allocator,
    const node* _location_root, type_classification _type,
    containers::string_view _init) {

  memory::unique_ptr<type> decl_type =
      memory::make_unique<type>(_allocator, _allocator, _type);
  decl_type->copy_location_from(_location_root);

  memory::unique_ptr<constant_expression> constant =
      memory::make_unique<constant_expression>(
          _allocator, _allocator, _type, _init.to_string(_allocator).c_str());

  constant->set_type(core::move(decl_type));
  constant->get_type()->copy_location_from(_location_root);
  return core::move(constant);
}

memory::unique_ptr<declaration> make_constant_declaration(
    memory::allocator* _allocator, node* _location_root,
    type_classification _type, containers::string_view _name,
    containers::string_view _init) {
  memory::unique_ptr<type> decl_type =
      memory::make_unique<type>(_allocator, _allocator, _type);
  decl_type->copy_location_from(_location_root);

  memory::unique_ptr<declaration> decl =
      memory::make_unique<declaration>(_allocator, _allocator);
  decl->copy_location_from(_location_root);

  memory::unique_ptr<parameter> param = memory::make_unique<parameter>(
      _allocator, _allocator, core::move(decl_type), _name);
  param->copy_location_from(_location_root);

  decl->set_parameter(core::move(param));
  decl->set_expression(make_constant(_allocator, _location_root, _type, _init));
  return core::move(decl);
}

memory::unique_ptr<assignment_instruction> make_assignment(
    memory::allocator* _allocator, node* _location_root,
    containers::string_view _name, memory::unique_ptr<expression>&& _expr) {
  memory::unique_ptr<id_expression> lhs =
      memory::make_unique<id_expression>(_allocator, _allocator, _name);
  lhs->copy_location_from(_location_root);

  memory::unique_ptr<lvalue> lval =
      memory::make_unique<lvalue>(_allocator, _allocator, core::move(lhs));
  lval->copy_location_from(_location_root);

  memory::unique_ptr<assignment_instruction> assign =
      memory::make_unique<assignment_instruction>(
          _allocator, _allocator, core::move(lval));
  assign->copy_location_from(_location_root);
  assign->add_value(assign_type::equal, core::move(_expr));

  return core::move(assign);
}

memory::unique_ptr<assignment_instruction> make_constant_assignment(
    memory::allocator* _allocator, node* _location_root,
    type_classification _type, containers::string_view _name,
    containers::string_view _init) {
  memory::unique_ptr<type> constant_type =
      memory::make_unique<type>(_allocator, _allocator, _type);
  constant_type->copy_location_from(_location_root);

  memory::unique_ptr<constant_expression> rhs =
      memory::make_unique<constant_expression>(
          _allocator, _allocator, _type, _init.to_string(_allocator).c_str());
  rhs->set_type(core::move(constant_type));
  rhs->get_type()->copy_location_from(_location_root);

  return make_assignment(_allocator, _location_root, _name, core::move(rhs));
}

class if_reassociation_pass : public pass {
public:
  if_reassociation_pass(type_validator* _validator, WNLogging::WNLog* _log,
      memory::allocator* _allocator)
    : pass(_validator, _log, _allocator), m_break_instructions(_allocator) {}
  void enter_scope_block(const node*) {}
  void leave_scope_block(const node*) {}
  containers::deque<break_instruction*> m_break_instructions;
  memory::unique_ptr<expression> walk_expression(expression*) {
    return nullptr;
  }

  memory::unique_ptr<break_instruction> walk_instruction(break_instruction* _inst) {
    m_break_instructions.push_back(_inst);
    return nullptr;
  }

  memory::unique_ptr<do_instruction> walk_instruction(do_instruction* _inst) {
    // If we have already marked, then return.
    // This allows us to guarantee that there is at least one "break"
    // instruction from every do loop.
    if (_inst->get_body()->returns()) {
      const constant_expression* expr =
          cast_to<const constant_expression>(_inst->get_condition());
      if (expr->get_index() ==
              static_cast<uint32_t>(type_classification::bool_type) &&
          expr->get_type_text() == "true") {
        for(auto& inst: m_break_instructions) {
          inst->set_loop(_inst);
        }
        m_break_instructions.clear();
        return nullptr;
      }
    }

    memory::unique_ptr<instruction_list> body =
      _inst->take_body();
    memory::unique_ptr<do_instruction> new_do =
        memory::make_unique<do_instruction>(m_allocator, m_allocator);
    new_do->copy_location_from(_inst);
    new_do->set_condition(make_constant(m_allocator, _inst->get_condition(),
        type_classification::bool_type, "true"));

    memory::unique_ptr<binary_expression> bin_expr =
        memory::make_unique<binary_expression>(m_allocator, m_allocator,
            arithmetic_type::arithmetic_equal, _inst->take_condition(),
            make_constant(m_allocator, _inst, type_classification::bool_type,
                                                   "false"));

    memory::unique_ptr<if_instruction> if_inst =
        memory::make_unique<if_instruction>(m_allocator, m_allocator);
    if_inst->copy_location_from(new_do->get_condition());
    if_inst->set_condition(core::move(bin_expr));

    memory::unique_ptr<break_instruction> break_inst =
        memory::make_unique<break_instruction>(m_allocator, m_allocator);
    break_inst->copy_location_from(if_inst->get_condition());

    memory::unique_ptr<instruction_list> if_body =
        memory::make_unique<instruction_list>(m_allocator, m_allocator);
    if_body->copy_location_from(if_inst->get_condition());
    if_body->add_instruction(core::move(break_inst));
    if_inst->set_body(core::move(if_body));
    if_inst->set_returns(true);
    body->add_instruction(core::move(if_inst));
    // Since this pass turns every do {} while (cond),
    // into do { if (!cond) { break; } };
    body->set_returns(true);
    new_do->set_body(core::move(body));

    return core::move(new_do);
  }

  void walk_instruction_list(instruction_list* _inst) {
    using instruction_queue =
        containers::deque<memory::unique_ptr<instruction>>;
    instruction_queue replaced_instructions(m_allocator);
    for (instruction_queue::iterator it = _inst->get_instructions().begin();
         it != _inst->get_instructions().end(); ++it) {
      if ((*it)->get_node_type() == node_type::if_instruction) {
        if_instruction* if_inst = static_cast<if_instruction*>(it->get());

        containers::deque<memory::unique_ptr<instruction>>& else_if_insts =
            if_inst->get_else_if_instructions();

        // This is where we actually do the work.
        char temporary_name[12] = {0};

        memory::writeuint32(
            temporary_name, m_last_temporary++, sizeof(temporary_name) - 1);
        containers::string name("__wns_if_temp", m_allocator);
        name.append(temporary_name);

        memory::unique_ptr<declaration> temp_decl =
            make_constant_declaration(m_allocator, if_inst,
                type_classification::bool_type, name, "false");

        replaced_instructions.emplace_back(core::move(temp_decl));

        // If we have an else_if instruction we have to track whether
        // or not we have entered any previous blocks.
        // This means that every if/if else must also set this
        // flag as its first instruction.
        containers::string elseif_name(m_allocator);
        elseif_name += "__wns_if_temp";

        if (!else_if_insts.empty()) {
          memory::writeuint32(
              temporary_name, m_last_temporary++, sizeof(temporary_name) - 1);
          elseif_name += temporary_name;

          memory::unique_ptr<declaration> elseif_temp_decl =
              make_constant_declaration(m_allocator, if_inst,
                  type_classification::bool_type, elseif_name, "true");
          replaced_instructions.emplace_back(core::move(elseif_temp_decl));
        }

        {
          // Add a new scope block that contains the condition.
          // We do this in a scope-block so that any temporaries
          // will be freed before the if statement.
          memory::unique_ptr<instruction_list> inst_list =
              memory::make_unique<instruction_list>(m_allocator, m_allocator);
          inst_list->copy_location_from(if_inst);

          // Actually move the condition for the if statement to an assignemnt
          // to __wns_temp0.
          memory::unique_ptr<assignment_instruction> assign = make_assignment(
              m_allocator, if_inst, name, if_inst->release_condition());
          inst_list->add_instruction(core::move(assign));
          replaced_instructions.emplace_back(core::move(inst_list));

          memory::unique_ptr<id_expression> id_expr =
              memory::make_unique<id_expression>(
                  m_allocator, m_allocator, name);
          id_expr->copy_location_from(if_inst);
          if_inst->set_condition(core::move(id_expr));
        }

        replaced_instructions.emplace_back(core::move(*it));

        if (!else_if_insts.empty()) {
          {
            if_inst->get_body()->get_instructions().emplace_front(
                make_constant_assignment(m_allocator, if_inst,
                    type_classification::bool_type, elseif_name, "false"));
          }

          for (auto& elif : else_if_insts) {
            WN_DEBUG_ASSERT_DESC(
                elif->get_node_type() == node_type::else_if_instruction,
                "This should ALWAYS be an else-if instruction");
            else_if_instruction* else_if =
                static_cast<else_if_instruction*>(elif.get());
            // Take the original body and condition.
            memory::unique_ptr<instruction_list> else_if_body =
                else_if->take_body();
            memory::unique_ptr<expression> else_if_condition =
                else_if->take_condition();

            // Inside the body we have to set the entering flag
            // to true
            else_if_body->get_instructions().emplace_front(
                make_constant_assignment(m_allocator, else_if,
                    type_classification::bool_type, elseif_name, "false"));

            // Switch this to the form
            // inst = may_enter;
            // if (inst) {
            //   inst = original_condition;
            // }

            // inst = may_enter
            memory::unique_ptr<id_expression> may_enter =
                memory::make_unique<id_expression>(
                    m_allocator, m_allocator, elseif_name);
            may_enter->copy_location_from(else_if);

            replaced_instructions.emplace_back(make_assignment(
                m_allocator, else_if, name, core::move(may_enter)));

            // if (inst) {
            memory::unique_ptr<if_instruction> conditional_if =
                memory::make_unique<if_instruction>(m_allocator, m_allocator);

            memory::unique_ptr<id_expression> entering =
                memory::make_unique<id_expression>(
                    m_allocator, m_allocator, name);
            entering->copy_location_from(else_if);

            conditional_if->set_condition(core::move(entering));

            // inst = original_condition
            memory::unique_ptr<instruction_list> inst_list =
                memory::make_unique<instruction_list>(m_allocator, m_allocator);
            inst_list->copy_location_from(else_if);

            inst_list->add_instruction(make_assignment(
                m_allocator, else_if, name, core::move(else_if_condition)));

            conditional_if->set_body(core::move(inst_list));
            replaced_instructions.emplace_back(core::move(conditional_if));

            memory::unique_ptr<id_expression> id_expr =
                memory::make_unique<id_expression>(
                    m_allocator, m_allocator, name);
            id_expr->copy_location_from(else_if);

            memory::unique_ptr<if_instruction> replaced_if_inst =
                memory::make_unique<if_instruction>(m_allocator, m_allocator);
            replaced_if_inst->copy_location_from(else_if);
            replaced_if_inst->set_condition(core::move(id_expr));
            replaced_if_inst->set_body(core::move(else_if_body));
            replaced_instructions.emplace_back(core::move(replaced_if_inst));
          }

          if (if_inst->get_else()) {
            // The else_if will be replaced by a if (may_enter) meaning we
            // have not entered yet.

            memory::unique_ptr<id_expression> id_expr =
                memory::make_unique<id_expression>(
                    m_allocator, m_allocator, elseif_name);
            id_expr->copy_location_from(if_inst->get_else());

            memory::unique_ptr<if_instruction> replaced_if_inst =
                memory::make_unique<if_instruction>(m_allocator, m_allocator);
            replaced_if_inst->set_condition(core::move(id_expr));
            replaced_if_inst->set_body(core::move(if_inst->release_else()));
            replaced_instructions.emplace_back(core::move(replaced_if_inst));
          }
          if_inst->clear_else_ifs();
          if_inst->clear_else_node();
        }
      } else {
        replaced_instructions.emplace_back(core::move(*it));
      }
    }
    _inst->set_instructions(core::move(replaced_instructions));
  }
  void walk_function(function*) {}
  void walk_parameter(parameter*) {}
  void pre_walk_script_file(script_file*) {}
  void walk_script_file(script_file*) {}
  void walk_type(type*) {}
  memory::unique_ptr<instruction> walk_instruction(instruction*) {
    return nullptr;
  }

  void walk_struct_definition(struct_definition*) {}
  memory::unique_ptr<instruction> walk_instruction(else_if_instruction*) {
    return nullptr;
  }
  memory::unique_ptr<instruction> walk_instruction(if_instruction*) {
    return nullptr;
  }
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
  memory::unique_ptr<instruction> walk_instruction(instruction*) {
    return nullptr;
  }
  void walk_function(function*) {}
  void walk_struct_definition(struct_definition*) {}

  void enter_scope_block(const node*) {
    id_map.emplace_back(
        containers::hash_map<containers::string_view, id_expression::id_source>(
            m_allocator));
  }

  void leave_scope_block(const node*) {
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

  memory::unique_ptr<instruction> walk_instruction(declaration* _decl) {
    if (find_param(_decl->get_name())) {
      m_log->Log(WNLogging::eError, 0, "Declaration of ", _decl->get_name(),
          "hides id of the same name");
      _decl->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return nullptr;
      ;
    }
    id_map.back()[_decl->get_name()] = {0, _decl};
    return nullptr;
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

  void pre_walk_script_file(script_file*) {}
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
      m_functions(_allocator),
      m_local_objects(_allocator) {}
  memory::unique_ptr<expression> walk_expression(expression*) {
    return nullptr;
  }

  void walk_parameter(parameter*) {}
  void enter_scope_block(const node* _node) {
    m_local_objects.push_back(local_scope(_node, m_allocator));
  }

  void leave_scope_block(const node*) {}

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
        if (memory::strnlen(text, 13) > 12) {
          error = true;
        } else {
          int64_t val = strtol(text, nullptr, 10);
          if (val < std::numeric_limits<int32_t>::min() ||
              val > std::numeric_limits<int32_t>::max() || errno == ERANGE) {
            errno = 0;
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
      case static_cast<uint32_t>(type_classification::float_type): {
        const char* text = _expr->get_type_text().c_str();
        bool error = false;
        double d = strtod(text, nullptr);
        if (errno == ERANGE || d < -std::numeric_limits<float>::max() ||
            d > std::numeric_limits<float>::max()) {
          error = true;
          errno = 0;
        }
        if (error) {
          m_log->Log(
              WNLogging::eError, 0, "Invalid floating point constant", text);
          m_num_errors += 1;
        }
        break;
      }
      default:
        WN_RELEASE_ASSERT_DESC(false, "No Implemented: constant type");
    }
    return nullptr;
  }

  template <typename T>
  memory::unique_ptr<expression> handle_binary_expression(
      expression* _root_expression, const type* lhs_type, const type* rhs_type,
      T _expression_type) {
    if (lhs_type->get_index() != rhs_type->get_index()) {
      m_log->Log(WNLogging::eError, 0, "Expected LHS and RHS to match");
      _root_expression->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return nullptr;
    }

    uint32_t return_type =
        m_validator->get_operations(static_cast<int32_t>(lhs_type->get_index()))
            .get_operation(_expression_type);

    if (return_type ==
        static_cast<uint32_t>(type_classification::invalid_type)) {
      m_log->Log(WNLogging::eError, 0, "Invalid operation for types");
      _root_expression->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return nullptr;
    } else if (return_type ==
               static_cast<uint32_t>(type_classification::void_type)) {
      m_log->Log(WNLogging::eError, 0, "Void return is invalid for arithmetic");
      _root_expression->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return nullptr;
    }

    WN_RELEASE_ASSERT_DESC(
        return_type < static_cast<uint32_t>(type_classification::custom_type),
        "Not Implemented: Custom types");
    memory::unique_ptr<type> t =
        memory::make_unique<type>(m_allocator, m_allocator, return_type);
    t->copy_location_from(_root_expression);
    _root_expression->set_type(core::move(t));
    return nullptr;
  }

  memory::unique_ptr<expression> walk_expression(binary_expression* _expr) {
    const type* lhs_type = _expr->get_lhs()->get_type();
    const type* rhs_type = _expr->get_rhs()->get_type();

    return handle_binary_expression(
        _expr, lhs_type, rhs_type, _expr->get_arithmetic_type());
  }

  memory::unique_ptr<expression> walk_expression(
      short_circuit_expression* _expr) {
    const type* lhs_type = _expr->get_lhs()->get_type();
    const type* rhs_type = _expr->get_rhs()->get_type();

    return handle_binary_expression(
        _expr, lhs_type, rhs_type, _expr->get_ss_type());
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
    const type_definition& member_def =
        m_validator->get_operations(member_type);
    if (member_type == 0) {
      m_log->Log(WNLogging::eError, 0, "Type does not have member: ",
          _expr->get_name());
      _expr->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return nullptr;
    }

    type* t = m_allocator->construct<type>(
        m_allocator, member_type, member_def.get_reference_type());

    t->copy_location_from(_expr);
    _expr->set_type(t);

    return nullptr;
  }

  memory::unique_ptr<expression> walk_expression(
      function_pointer_expression* _expr) {
    function* callee = nullptr;
    auto possible_function = m_functions.find(_expr->get_name());
    if (possible_function == m_functions.end()) {
      m_log->Log(
          WNLogging::eError, 0, "Cannot find function: ", _expr->get_name());
      _expr->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return nullptr;
    }

    for (auto func : possible_function->second) {
      if (_expr->get_types().size() != 0) {
        if (!func->get_parameters()) {
          continue;
        }

        const auto& function_parameters =
            func->get_parameters()->get_parameters();
        if (function_parameters.size() != _expr->get_types().size()) {
          continue;
        }
        size_t i = 0;
        for (; i < _expr->get_types().size(); ++i) {
          if (*_expr->get_types()[i] !=
              *function_parameters[i]->get_type()) {
            break;
          }
        }
        if (i != _expr->get_types().size()) {
          continue;
        }
      } else if (func->get_parameters() &&
                 func->get_parameters()->get_parameters().size() != 0) {
        continue;
      }

      if (callee) {
        m_log->Log(
            WNLogging::eError, 0, "Ambiguous Function call: ", _expr->get_name());
        _expr->log_line(*m_log, WNLogging::eError);
        ++m_num_errors;
        return nullptr;
      }
      callee = func;
    }

    // TODO(awoloszyn): Insert explicit cast operations if we need do here.
    _expr->set_source(callee);
    memory::unique_ptr<type> t = memory::make_unique<type>(
        m_allocator, m_allocator, type_classification::function_ptr_type);
    _expr->set_type(core::move(t));
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

  memory::unique_ptr<instruction> create_destructor(
      declaration* _decl, node* _location_copy) {
    if (_decl->get_type()->get_reference_type() == reference_type::unique ||
        _decl->get_type()->get_reference_type() == reference_type::raw) {
      containers::string m_destructor_name("_destruct_", m_allocator);
      m_destructor_name.append(_decl->get_type()->custom_type_name().data(),
          _decl->get_type()->custom_type_name().length());

      memory::unique_ptr<id_expression> object_id =
          memory::make_unique<id_expression>(
              m_allocator, m_allocator, _decl->get_name());
      object_id->set_id_source({0, _decl});
      object_id->copy_location_from(_location_copy);
      object_id->set_type(
          memory::make_unique<type>(m_allocator, *_decl->get_type()));

      memory::unique_ptr<cast_expression> cast1 =
          memory::make_unique<cast_expression>(
              m_allocator, m_allocator, core::move(object_id));
      cast1->copy_location_from(cast1->get_expression());
      cast1->set_type(
          memory::make_unique<type>(m_allocator, *_decl->get_type()));
      cast1->get_type()->set_reference_type(reference_type::self);

      memory::unique_ptr<arg_list> args =
          memory::make_unique<arg_list>(m_allocator, m_allocator);
      args->copy_location_from(_location_copy);
      args->add_expression(core::move(cast1));

      memory::unique_ptr<function_call_expression> call =
          memory::make_unique<function_call_expression>(
              m_allocator, m_allocator, core::move(args));
      call->copy_location_from(call->get_args());
      call->set_type(memory::make_unique<type>(
          m_allocator, m_allocator, type_classification::void_type));

      memory::unique_ptr<id_expression> destructor =
          memory::make_unique<id_expression>(
              m_allocator, m_allocator, m_destructor_name);
      destructor->copy_location_from(call.get());

      call->add_base_expression(core::move(destructor));

      walk_expression(call.get());  // Have the type_association pass handle
      // setting up this function call.
      return memory::make_unique<expression_instruction>(
          m_allocator, m_allocator, core::move(call));
    } else {
      containers::string m_destructor_name("_deref_shared", m_allocator);

      memory::unique_ptr<id_expression> object_id =
          memory::make_unique<id_expression>(
              m_allocator, m_allocator, _decl->get_name());
      object_id->set_id_source({0, _decl});
      object_id->copy_location_from(_location_copy);
      object_id->set_type(
          memory::make_unique<type>(m_allocator, *_decl->get_type()));

      memory::unique_ptr<cast_expression> cast1 =
          memory::make_unique<cast_expression>(
              m_allocator, m_allocator, core::move(object_id));
      cast1->copy_location_from(cast1->get_expression());
      cast1->set_type(memory::make_unique<type>(
          m_allocator, m_allocator, type_classification::void_ptr_type));
      cast1->get_type()->copy_location_from(cast1->get_expression());

      memory::unique_ptr<arg_list> args =
          memory::make_unique<arg_list>(m_allocator, m_allocator);
      args->copy_location_from(_location_copy);
      args->add_expression(core::move(cast1));

      memory::unique_ptr<function_call_expression> call =
          memory::make_unique<function_call_expression>(
              m_allocator, m_allocator, core::move(args));
      call->copy_location_from(call->get_args());
      call->set_type(memory::make_unique<type>(
          m_allocator, m_allocator, type_classification::void_type));

      memory::unique_ptr<id_expression> destructor =
          memory::make_unique<id_expression>(
              m_allocator, m_allocator, m_destructor_name);
      destructor->copy_location_from(call.get());

      call->add_base_expression(core::move(destructor));

      walk_expression(call.get());  // Have the type_association pass handle
      // setting up this function call.
      return memory::make_unique<expression_instruction>(
          m_allocator, m_allocator, core::move(call));
    }


  }

  memory::unique_ptr<instruction> walk_instruction(instruction*) {
    return nullptr;
  }
  void walk_instruction_list(instruction_list* instructions) {
    // If we are leaving a scope-block that had a return
    // at the end, then we do not need to emit desctuctors,
    // that will have already been done in the return.
    if (instructions->returns()) {
      m_local_objects.pop_back();
      return;
    }
    if (instructions->get_instructions().size() > 1 &&
        (instructions->get_instructions().back()->is_dead() ||
            instructions->get_instructions().back()->get_node_type() ==
                node_type::return_instruction)) {
      m_local_objects.pop_back();
      return;
    }
    if (m_local_objects.back().objects.empty()) {
      m_local_objects.pop_back();
      return;
    }

    // We have walked every instruction in the instruction list,
    // we are just about to leave the scope block.
    // This means we have to run the destructor for any objects
    // that are bound by this scope.
    for (auto& decl : m_local_objects.back().objects) {
      instructions->add_instruction(create_destructor(decl, instructions));
    }
    m_local_objects.pop_back();
  }
  void walk_struct_definition(struct_definition*) {}
  void pre_register_struct_definition(struct_definition* _definition) {
    if (m_validator->get_type(_definition->get_name())) {
      m_log->Log(WNLogging::eError, 0, "Duplicate struct defined: ",
          _definition->get_name());
      _definition->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return;
    }
    m_validator->register_struct_type(_definition->get_name());
  }

  // We have to split up pre_register and register because
  // Struct A may contain struct B, so we need to know
  // of ALL struct names before we register indices.
  void register_struct_definition(struct_definition* _definition) {
    const uint32_t type = m_validator->get_type(_definition->get_name());
    for (const auto& a : _definition->get_struct_members()) {
      walk_type(a->get_type());
    }

    for (auto ref_type : {reference_type::self, reference_type::unique,
             reference_type::shared}) {
      type_definition& def =
          m_validator->get_operations(type + static_cast<uint32_t>(ref_type));
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
  }

  memory::unique_ptr<instruction> walk_instruction(
      assignment_instruction* _assign) {
    if (_assign->get_assignment_type() != assign_type::equal) {
      WN_RELEASE_ASSERT_DESC(
          false, "Not Implemented: Non-equality assignment instructions.");
    }

    switch (_assign->get_lvalue()->get_expression()->get_node_type()) {
      case node_type::cast_expression:
      case node_type::id_expression:
      case node_type::unary_expression:
      case node_type::post_expression:
      case node_type::post_unary_expression:
      case node_type::array_access_expression:
      case node_type::member_access_expression:
        break;
      case node_type::function_call_expression:
      case node_type::short_circuit_expression:
      case node_type::struct_allocation_expression:
      case node_type::constant_expression:
      case node_type::null_allocation_expression:
      case node_type::cond_expression:
      case node_type::binary_expression:
      case node_type::array_allocation_expression:
        m_log->Log(WNLogging::eError, 0, "Cannot assign to expression.");
        _assign->log_line(*m_log, WNLogging::eError);
        ++m_num_errors;
        return nullptr;
    }

    if (*_assign->get_lvalue()->get_expression()->get_type() !=
        *_assign->get_expression()->get_type()) {
      m_log->Log(WNLogging::eError, 0, "Cannot assign incompatible types.");
      _assign->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
    }

    // If we are assigning a shared reference, then we have to increment the ref-count
    if (_assign->get_lvalue()
            ->get_expression()
            ->get_type()
            ->get_reference_type() == reference_type::shared) {

      memory::unique_ptr<id_expression> increment_func =
          memory::make_unique<id_expression>(
              m_allocator, m_allocator, "_assign_shared");
      increment_func->copy_location_from(_assign);


      memory::unique_ptr<type> target = memory::make_unique<type>(
          m_allocator, *_assign->get_lvalue()->get_expression()->get_type());
      target->copy_location_from(_assign);

      memory::unique_ptr<type> void_ptr_type = memory::make_unique<type>(
        m_allocator, m_allocator, type_classification::void_ptr_type);
      void_ptr_type->copy_location_from(_assign);

      memory::unique_ptr<cast_expression> cast =
          memory::make_unique<cast_expression>(
              m_allocator, m_allocator, core::move(_assign->transfer_expression()));
      cast->set_type(core::move(void_ptr_type));

      void_ptr_type = memory::make_unique<type>(
        m_allocator, m_allocator, type_classification::void_ptr_type);
      void_ptr_type->copy_location_from(_assign);

      memory::unique_ptr<expression> second_arg;
      if (!_assign->is_constructor_assigment()) {
        second_arg = memory::make_unique<cast_expression>(m_allocator, m_allocator,
            core::move(clone_node(_assign->get_lvalue()->get_expression())));
        second_arg->set_type(core::move(void_ptr_type));
      } else {
        second_arg = make_constant(
            m_allocator, _assign, type_classification::void_ptr_type, "");
      }

      memory::unique_ptr<arg_list> args =
          memory::make_unique<arg_list>(m_allocator, m_allocator);
      args->copy_location_from(cast.get());
      args->add_expression(core::move(cast));
      args->add_expression(core::move(second_arg));

      memory::unique_ptr<function_call_expression> call =
          memory::make_unique<function_call_expression>(
              m_allocator, m_allocator, core::move(args));
      call->copy_location_from(call->get_args());
      call->set_type(memory::make_unique<type>(m_allocator, *target.get()));
      call->get_type()->copy_location_from(_assign);

      call->add_base_expression(core::move(increment_func));

      walk_expression(call.get());  // Have the type_association pass handle

      cast = memory::make_unique<cast_expression>(
          m_allocator, m_allocator, core::move(call));
      cast->set_type(core::move(target));
      _assign->add_value(_assign->get_assignment_type(), core::move(cast));
      return nullptr;
    }

    return nullptr;
  }

  memory::unique_ptr<instruction> walk_instruction(break_instruction* _break) {
    // Insert ALL of the destructors here.
    if (_break->returns())
      return nullptr;  // If the break returns we have already been handled.
    memory::unique_ptr<instruction_list> new_list =
        memory::make_unique<instruction_list>(m_allocator, m_allocator);
    new_list->copy_location_from(_break);

    for (auto it = m_local_objects.rbegin(); it != m_local_objects.rend();
         ++it) {
      for (auto it2 = it->objects.rbegin(); it2 != it->objects.rend(); ++it2) {
        new_list->add_instruction(create_destructor(*it2, _break));
      }
      // If we have hit the header, don't destroy any further.
      if (it->m_header == _break->get_do_instruction()->get_body())
        break;
    }
    _break->set_returns(true);
    new_list->add_instruction(clone_node(_break));
    return core::move(new_list);
  }

  memory::unique_ptr<instruction> walk_instruction(return_instruction* _ret) {
    // Generate all of our destructors here.
    // Turn
    // return 3;
    // into
    // {
    //  Int _x = 3;
    //  call all destructors;
    //  return _x;
    // }
    if (std::find(m_returns.begin(), m_returns.end(), _ret) !=
        m_returns.end()) {
      // If we have already processed this return, we do not want to re-process
      // it to generate more destructors.
      return nullptr;
    }

    bool leave = true;
    // Insert ALL of the destructors here.
    for (auto it = m_local_objects.rbegin(); it != m_local_objects.rend();
         ++it) {
      if (!it->objects.empty()) {
        leave = false;
      }
    }
    // Small optimization, if there are NO objects to destroy,
    // then we can avoid doing anything.
    if (leave) {
      m_returns.push_back(_ret);
      return nullptr;
    }

    memory::unique_ptr<expression> expr(_ret->take_expression());

    memory::unique_ptr<instruction_list> instructions(
        memory::make_unique<instruction_list>(m_allocator, m_allocator));
    instructions->copy_location_from(_ret);

    char temporary_name[12] = {0};

    memory::writeuint32(
        temporary_name, m_last_temporary++, sizeof(temporary_name) - 1);
    containers::string name("__wns_ret_temp", m_allocator);
    name.append(temporary_name);

    memory::unique_ptr<type> return_type;
    declaration* ret_decl = nullptr;
    if (expr) {
      // Type __wns_ret_temp0 = {rhs};
      memory::unique_ptr<type> decl_type(
          memory::make_unique<type>(m_allocator, *expr->get_type()));
      decl_type->copy_location_from(_ret);

      return_type = memory::make_unique<type>(m_allocator, *expr->get_type());
      return_type->copy_location_from(_ret);

      memory::unique_ptr<parameter> param(memory::make_unique<parameter>(
          m_allocator, m_allocator, core::move(decl_type), name));

      memory::unique_ptr<declaration> decl(
          memory::make_unique<declaration>(m_allocator, m_allocator));
      decl->copy_location_from(_ret);
      decl->set_parameter(core::move(param));
      decl->set_expression(core::move(expr));
      ret_decl = decl.get();

      instructions->add_instruction(core::move(decl));
    }
    // Insert ALL of the destructors here.
    for (auto it = m_local_objects.rbegin(); it != m_local_objects.rend();
         ++it) {
      for (auto it2 = it->objects.rbegin(); it2 != it->objects.rend(); ++it2) {
        instructions->add_instruction(create_destructor(*it2, _ret));
      }
    }
    if (return_type) {
      reference_type ref_type = return_type->get_reference_type();
      memory::unique_ptr<id_expression> return_id(
          memory::make_unique<id_expression>(m_allocator, m_allocator, name));
      return_id->copy_location_from(_ret);
      return_id->set_type(core::move(return_type));
      return_id->set_id_source({nullptr, ret_decl});

      if (ref_type == reference_type::shared) {
        // return (Foo*)_return_shared((void*)__wns_ret_temp0);

        memory::unique_ptr<type> output_type =
            memory::make_unique<type>(m_allocator, *return_id->get_type());
        output_type->copy_location_from(_ret);

        memory::unique_ptr<cast_expression> cast =
            memory::make_unique<cast_expression>(
                m_allocator, m_allocator, core::move(return_id));
        cast->copy_location_from(_ret);
        cast->set_type(memory::make_unique<type>(
            m_allocator, m_allocator, type_classification::void_ptr_type));
        cast->get_type()->copy_location_from(_ret);

        memory::unique_ptr<id_expression> return_func =
            memory::make_unique<id_expression>(
                m_allocator, m_allocator, "_return_shared");
        return_func->copy_location_from(_ret);

        memory::unique_ptr<arg_list> return_args =
            memory::make_unique<arg_list>(m_allocator, m_allocator);
        return_args->copy_location_from(_ret);
        return_args->add_expression(core::move(cast));

        memory::unique_ptr<function_call_expression> return_shared=
            memory::make_unique<function_call_expression>(
                m_allocator, m_allocator, core::move(return_args));
        return_shared->copy_location_from(return_shared->get_args());
        return_shared->set_type(memory::make_unique<type>(
            m_allocator, m_allocator, type_classification::void_ptr_type));
        return_shared->get_type()->copy_location_from(_ret);

        return_shared->add_base_expression(core::move(return_func));

        // Walk the function now.
        walk_expression(return_shared.get());

        memory::unique_ptr<cast_expression> cast_back =
            memory::make_unique<cast_expression>(
                m_allocator, m_allocator, core::move(return_shared));
        cast_back->copy_location_from(_ret);
        cast_back->set_type(core::move(output_type));
        cast_back->get_type()->copy_location_from(_ret);


        memory::unique_ptr<return_instruction> new_ret_inst(
            memory::make_unique<return_instruction>(
                m_allocator, m_allocator, core::move(cast_back)));
        new_ret_inst->copy_location_from(_ret);
        new_ret_inst->set_returns(true);
        m_returns.push_back(new_ret_inst.get());
        instructions->add_instruction(core::move(new_ret_inst));
      } else {
        // return __wns_ret_temp0;

        memory::unique_ptr<return_instruction> new_ret_inst(
            memory::make_unique<return_instruction>(
                m_allocator, m_allocator, core::move(return_id)));
        new_ret_inst->copy_location_from(_ret);
        new_ret_inst->set_returns(true);
        m_returns.push_back(new_ret_inst.get());
        instructions->add_instruction(core::move(new_ret_inst));
      }
    } else {
      memory::unique_ptr<return_instruction> new_ret_inst(
          memory::make_unique<return_instruction>(m_allocator, m_allocator));
      new_ret_inst->copy_location_from(_ret);
      new_ret_inst->set_returns(true);
      m_returns.push_back(new_ret_inst.get());
      instructions->add_instruction(core::move(new_ret_inst));
    }
    instructions->set_returns(true);
    return core::move(instructions);
  }

  memory::unique_ptr<instruction> walk_instruction(declaration* _decl) {
    // TODO(awoloszyn): Handle types other than non_nullable.
    if (m_validator->get_cast(_decl->get_expression()->get_type()->get_index(),
            _decl->get_type()->get_index()) != cast_type::transparent) {
      m_log->Log(WNLogging::eError, 0,
          "Cannot initialize variable with expression of a different type.");
      _decl->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
    }

    if (m_validator->is_destructible_type(_decl->get_type()->get_index())) {
      if (_decl->get_type()->get_reference_type() == reference_type::shared) {
        // Add an increment here
        memory::unique_ptr<expression> alloc = _decl->take_expression();

        memory::unique_ptr<cast_expression> cast =
            memory::make_unique<cast_expression>(
                m_allocator, m_allocator, core::move(alloc));
        cast->copy_location_from(_decl);
        cast->set_type(memory::make_unique<type>(
            m_allocator, m_allocator, type_classification::void_ptr_type));
        cast->get_type()->copy_location_from(_decl);

        memory::unique_ptr<id_expression> assignment_func =
            memory::make_unique<id_expression>(
                m_allocator, m_allocator, "_assign_shared");
        assignment_func->copy_location_from(_decl);

        memory::unique_ptr<arg_list> assignment_args =
            memory::make_unique<arg_list>(m_allocator, m_allocator);
        assignment_args->copy_location_from(_decl);
        assignment_args->add_expression(core::move(cast));
        assignment_args->add_expression(make_constant(
          m_allocator, _decl, type_classification::void_ptr_type, ""));

        memory::unique_ptr<function_call_expression> increment =
            memory::make_unique<function_call_expression>(
                m_allocator, m_allocator, core::move(assignment_args));
        increment->copy_location_from(increment->get_args());
        increment->set_type(memory::make_unique<type>(
            m_allocator, m_allocator, type_classification::void_ptr_type));
        increment->get_type()->copy_location_from(_decl);

        increment->add_base_expression(core::move(assignment_func));

        // Walk the function now.
        walk_expression(increment.get());

        memory::unique_ptr<cast_expression> cast_back =
            memory::make_unique<cast_expression>(
                m_allocator, m_allocator, core::move(increment));
        cast_back->copy_location_from(_decl);
        cast_back->set_type(
            memory::make_unique<type>(m_allocator, *_decl->get_type()));
        cast_back->get_type()->copy_location_from(_decl);
        _decl->set_expression(core::move(cast_back));
      }
      m_local_objects.back().objects.push_back(_decl);
    }
    return nullptr;
  }

  memory::unique_ptr<instruction> walk_instruction(
      else_if_instruction* _else_if) {
    if (_else_if->get_condition()->get_type()->get_index() !=
        static_cast<uint32_t>(type_classification::bool_type)) {
      m_log->Log(WNLogging::eError, 0,
          "Else If conditional must be a boolean expression");
      _else_if->get_condition()->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
    }
    return nullptr;
  }

  memory::unique_ptr<instruction> walk_instruction(if_instruction* _if) {
    if (_if->get_condition()->get_type()->get_index() !=
        static_cast<uint32_t>(type_classification::bool_type)) {
      m_log->Log(
          WNLogging::eError, 0, "If conditional must be a boolean expression");
      _if->get_condition()->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
    }
    return nullptr;
  }

  void walk_function(function* _func) {
    const type* function_return_type = _func->get_signature()->get_type();

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

    m_returns.clear();
  }

  void register_function(function* _func) {
    walk_type(_func->get_signature()->get_type());

    containers::dynamic_array<uint32_t> params(m_allocator);
    uint32_t return_type =
        static_cast<uint32_t>(_func->get_signature()->get_type()->get_index());
    if (_func->get_parameters()) {
      for (auto& param : _func->get_parameters()->get_parameters()) {
        walk_type(param->get_type());
        params.push_back(static_cast<uint32_t>(param->get_type()->get_index()));
      }
    }

    containers::string_view name = _func->get_signature()->get_name();
    containers::string mangled_name =
        m_validator->get_mangled_name(name, return_type, params);
    _func->set_mangled_name(mangled_name);

    auto function_it = m_functions.find(name);
    if (function_it == m_functions.end()) {
      auto a = m_functions.insert(containers::make_pair(
          name, containers::deque<function*>(m_allocator)));
      function_it = a.first;
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

  void pre_walk_script_file(script_file* _file) {
    for (auto& strct : _file->get_structs()) {
      pre_register_struct_definition(strct.get());
    }
    for (auto& strct : _file->get_structs()) {
      register_struct_definition(strct.get());
    }

    for (auto& function: _file->get_external_functions()) {
      register_function(function.get());
    }
    for (auto& function : _file->get_functions()) {
      register_function(function.get());
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
  struct local_scope {
    // We have to add this here as VS2013 does not automatically
    // generate move constructors.
    local_scope(local_scope&& _other)
      : m_header(_other.m_header), objects(core::move(_other.objects)) {}
    local_scope(const node* _header, memory::allocator* _allocator)
      : m_header(_header), objects(_allocator) {}

    const node* m_header;
    containers::deque<declaration*> objects;
  };
  containers::deque<local_scope> m_local_objects;
};

class temporary_reification_pass : public pass {
public:
  temporary_reification_pass(type_validator* _validator, WNLogging::WNLog* _log,
      memory::allocator* _allocator)
    : pass(_validator, _log, _allocator),
      m_temporary_declarations(_allocator) {}

  memory::unique_ptr<instruction> walk_instruction(instruction* _inst) {
    _inst->set_temp_declarations(core::move(m_temporary_declarations));
    m_temporary_declarations =
        containers::deque<memory::unique_ptr<declaration>>(m_allocator);
    return nullptr;
  }

  void walk_instruction_list(instruction_list* _inst) {
    using instruction_queue =
        containers::deque<memory::unique_ptr<instruction>>;
    instruction_queue replaced_instructions(m_allocator);
    for (instruction_queue::iterator it = _inst->get_instructions().begin();
         it != _inst->get_instructions().end(); ++it) {
      containers::deque<memory::unique_ptr<declaration>> temp_decls =
          (*it)->release_temp_declarations();
      for (auto& decl : temp_decls) {
        replaced_instructions.push_back(core::move(decl));
      }
      replaced_instructions.push_back(core::move(*it));
    }
    _inst->set_instructions(core::move(replaced_instructions));
  }

  memory::unique_ptr<expression> walk_expression(expression* _expr) {
    if (!_expr->is_temporary()) {
      return nullptr;
    }
    memory::unique_ptr<expression> copied_expr = _expr->transfer_to_new();
    memory::unique_ptr<declaration> decl =
        memory::make_unique<declaration>(m_allocator, m_allocator);

    decl->copy_location_from(_expr);
    char temporary_name[12] = {0};

    containers::string name(m_allocator);
    name += "__wns_temp_expression";
    memory::writeuint32(
        temporary_name, m_last_temporary++, sizeof(temporary_name) - 1);
    name += temporary_name;

    memory::unique_ptr<type> t =
        memory::make_unique<type>(m_allocator, *copied_expr->get_type());
    t->copy_location_from(copied_expr->get_type());

    memory::unique_ptr<type> t2 =
          memory::make_unique<type>(m_allocator, *copied_expr->get_type());
      t2->copy_location_from(copied_expr->get_type());

    memory::unique_ptr<parameter> t_param = memory::make_unique<parameter>(
        m_allocator, m_allocator, core::move(t), name);
    t_param->copy_location_from(copied_expr.get());

    decl->set_parameter(core::move(t_param));
    if (copied_expr->get_type()->get_reference_type() == reference_type::unique) {
      copied_expr->get_type()->set_reference_type(reference_type::raw);

      decl->set_expression(core::move(copied_expr));

      decl->get_type()->set_reference_type(reference_type::raw);

      m_temporary_declarations.push_back(core::move(decl));

      memory::unique_ptr<id_expression> id =
          memory::make_unique<id_expression>(m_allocator, m_allocator, name);
      id->copy_location_from(_expr);

      memory::unique_ptr<cast_expression> cast =
          memory::make_unique<cast_expression>(
              m_allocator, m_allocator, core::move(id));
      t2->set_reference_type(reference_type::unique);
      cast->set_type(core::move(t2));

      return core::move(cast);
    } else if (copied_expr->get_type()->get_reference_type() ==
               reference_type::shared) {

      copied_expr->get_type()->set_reference_type(reference_type::shared);

      decl->set_expression(core::move(copied_expr));
      decl->get_type()->set_reference_type(reference_type::shared);
      m_temporary_declarations.push_back(core::move(decl));

      memory::unique_ptr<id_expression> id =
          memory::make_unique<id_expression>(m_allocator, m_allocator, name);
      id->copy_location_from(_expr);

      return core::move(id);
    }

    return nullptr;
  }

  void pre_walk_script_file(script_file*) {}
  void walk_script_file(script_file*) {}
  void walk_type(type*) {}
  void walk_function(function*) {}
  void walk_struct_definition(struct_definition*) {}
  void enter_scope_block(const node*) {}
  void leave_scope_block(const node*) {}
  void walk_parameter(parameter*) {}

private:
  containers::deque<memory::unique_ptr<declaration>> m_temporary_declarations;
};

class member_reassociation_pass : public pass {
public:
  member_reassociation_pass(type_validator* _validator, WNLogging::WNLog* _log,
      memory::allocator* _allocator)
    : pass(_validator, _log, _allocator), m_additional_functions(_allocator) {}

  memory::unique_ptr<instruction> walk_instruction(declaration* _decl) {
    if (_decl->get_type()->get_reference_type() == reference_type::raw) {
      return nullptr;
    }
    if (_decl->get_type()->get_reference_type() == reference_type::shared) {
      // This may have previously gotten set to temporary, BUT
      // We are defintely assigning it to a real thing, so lets make it no
      // longer temporary.
      _decl->get_expression()->set_temporary(false);
      // TODO(awoloszyn): Increment reference count here.
      return nullptr;
    }
    WN_RELEASE_ASSERT_DESC(
        _decl->get_type()->get_reference_type() == reference_type::unique,
        "Not Implemented: Other types of reference declarations");

    if (_decl->get_expression()->get_node_type() !=
        node_type::function_call_expression) {
      m_log->Log(WNLogging::eError, 0, "Expected constructor call");
      _decl->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return nullptr;
    }

    memory::unique_ptr<expression> rhs = _decl->take_expression();
    memory::unique_ptr<cast_expression> cast =
        memory::make_unique<cast_expression>(
            m_allocator, m_allocator, core::move(rhs));
    cast->copy_location_from(cast->get_expression());
    cast->set_type(memory::make_unique<type>(
        m_allocator, *cast->get_expression()->get_type()));
    cast->get_type()->set_reference_type(reference_type::unique);

    _decl->set_expression(core::move(cast));
    return nullptr;
  }

  memory::unique_ptr<instruction> walk_instruction(
      assignment_instruction* _assign) {
    if (_assign->get_expression()->get_type() &&
       _assign->get_expression()->get_type()->get_reference_type() ==
      reference_type::shared) {
      // If we have an HRS in our assignment that is shared
      // we know it is not a temporary, so do not create one.
      _assign->get_expression()->set_temporary(false);
    }
    return nullptr;
  }

  memory::unique_ptr<instruction> walk_instruction(else_if_instruction*) {
    WN_DEBUG_ASSERT_DESC(false,
        "We should not end up here"
        "all else_if blocks should have been removed in the if_reassociation "
        "pass");
    return nullptr;
  }

  memory::unique_ptr<instruction> walk_instruction(return_instruction* _ret) {
    if (_ret->get_expression() && _ret->get_expression()->get_type() &&
        _ret->get_expression()->get_type()->get_reference_type() ==
            reference_type::shared) {
      // If we are returning a shared object, then we can also know
      // that we do not need a temporary.
      _ret->get_expression()->set_temporary(false);
    }
    return nullptr;
  }

  memory::unique_ptr<expression> walk_expression(expression*) {
    return nullptr;
  }

  memory::unique_ptr<expression> walk_expression(
      struct_allocation_expression* _alloc) {
    // Replace struct_allocation with function_call to constructor with
    // struct allocation as argument.
    if (_alloc->set_initialization_mode() !=
        struct_initialization_mode::invalid) {
      return nullptr;
    }

    containers::string constructor_name("_construct_", m_allocator);
    constructor_name.append(_alloc->get_type()->custom_type_name().data(),
        _alloc->get_type()->custom_type_name().size());

    memory::unique_ptr<id_expression> constructor =
        memory::make_unique<id_expression>(
            m_allocator, m_allocator, constructor_name);

    constructor->copy_location_from(_alloc);

    if (_alloc->get_type()->get_reference_type() == reference_type::unique) {
      memory::unique_ptr<struct_allocation_expression> alloc =
          memory::make_unique<struct_allocation_expression>(
              m_allocator, m_allocator);
      alloc->set_initialization_mode(struct_initialization_mode::unspecified);
      alloc->copy_location_from(_alloc);
      alloc->set_type(_alloc->transfer_out_type());
      alloc->set_copy_initializer(_alloc->transfer_copy_initializer());

      alloc->set_temporary(true);
      const type* alloc_type = alloc->get_type();

      memory::unique_ptr<cast_expression> cast1 =
          memory::make_unique<cast_expression>(
              m_allocator, m_allocator, core::move(alloc));
      cast1->copy_location_from(cast1->get_expression());
      cast1->set_type(memory::make_unique<type>(m_allocator, *alloc_type));
      cast1->get_type()->set_reference_type(reference_type::self);

      memory::unique_ptr<arg_list> args =
          memory::make_unique<arg_list>(m_allocator, m_allocator);
      args->copy_location_from(cast1.get());
      args->add_expression(core::move(cast1));

      memory::unique_ptr<function_call_expression> call =
          memory::make_unique<function_call_expression>(
              m_allocator, m_allocator, core::move(args));
      call->copy_location_from(call->get_args());
      call->set_type(memory::make_unique<type>(m_allocator, *alloc_type));
      call->get_type()->set_reference_type(reference_type::self);

      call->add_base_expression(core::move(constructor));

      return core::move(call);
    } else if (_alloc->get_type()->get_reference_type() == reference_type::shared) {
      memory::unique_ptr<id_expression> alloc_func =
          memory::make_unique<id_expression>(
              m_allocator, m_allocator, "_allocate_shared");
      alloc_func->copy_location_from(_alloc);

      memory::unique_ptr<sizeof_expression> size_of =
        memory::make_unique<sizeof_expression>(m_allocator, m_allocator,
          memory::make_unique<type>(m_allocator, *_alloc->get_type()));
      size_of->set_type(memory::make_unique<type>(m_allocator, m_allocator,
        type_classification::size_type));
      size_of->get_type()->copy_location_from(_alloc);
      size_of->get_sized_type()->copy_location_from(_alloc);
      size_of->copy_location_from(_alloc);
      size_of->get_sized_type()->set_reference_type(reference_type::raw);

      containers::string destructor_name("_destruct_", m_allocator);
      destructor_name.append(_alloc->get_type()->custom_type_name().data(),
          _alloc->get_type()->custom_type_name().size());

      memory::unique_ptr<function_pointer_expression> fn_ptr =
          memory::make_unique<function_pointer_expression>(
              m_allocator, m_allocator, destructor_name);
      memory::unique_ptr<type> set_type = clone_node(_alloc->get_type());
      set_type->set_reference_type(reference_type::self);
      fn_ptr->copy_location_from(_alloc);
      fn_ptr->add_param(core::move(set_type));

      memory::unique_ptr<arg_list> args =
          memory::make_unique<arg_list>(m_allocator, m_allocator);
      args->copy_location_from(_alloc);
      args->add_expression(core::move(size_of));
      args->add_expression(core::move(fn_ptr));

      memory::unique_ptr<function_call_expression> allocate =
          memory::make_unique<function_call_expression>(
              m_allocator, m_allocator, core::move(args));
      allocate->add_base_expression(core::move(alloc_func));
      allocate->copy_location_from(_alloc);
      allocate->set_type(memory::make_unique<type>(m_allocator, m_allocator,
        type_classification::void_ptr_type));
      allocate->get_type()->copy_location_from(_alloc);

      memory::unique_ptr<cast_expression> cast_to_self =
          memory::make_unique<cast_expression>(
              m_allocator, m_allocator, core::move(allocate));
      cast_to_self->copy_location_from(_alloc);
      cast_to_self->set_type(
          memory::make_unique<type>(m_allocator, *_alloc->get_type()));
      cast_to_self->get_type()->set_reference_type(reference_type::self);

      args = memory::make_unique<arg_list>(m_allocator, m_allocator);
      args->copy_location_from(_alloc);
      args->add_expression(core::move(cast_to_self));

      memory::unique_ptr<function_call_expression> construct =
        memory::make_unique<function_call_expression>(
          m_allocator, m_allocator, core::move(args));

      construct->copy_location_from(_alloc);
      construct->set_type(memory::make_unique<type>(m_allocator, *_alloc->get_type()));
      construct->get_type()->set_reference_type(reference_type::self);
      construct->add_base_expression(core::move(constructor));

      memory::unique_ptr<cast_expression> cast =
          memory::make_unique<cast_expression>(
              m_allocator, m_allocator, core::move(construct));
      cast->copy_location_from(_alloc);
      cast->set_type(
          memory::make_unique<type>(m_allocator, *_alloc->get_type()));
      cast->get_type()->copy_location_from(_alloc);
      cast->set_temporary(true);
      return core::move(cast);
    }
    return nullptr;

  }

  void walk_parameter(parameter*) {}
  void walk_instruction_list(instruction_list*) {}

  void pre_walk_script_file(script_file*) {}
  void walk_script_file(script_file* _file) {
    for (auto& func : m_additional_functions) {
      // We want to prepend these functions
      // because we want these to show up before the normal functions.
      _file->prepend_function(core::move(func));
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
    containers::deque<declaration*> decls(m_allocator);
    containers::deque<declaration*> shared_ids(m_allocator);
    size_t num = _definition->get_struct_members().size();
    for (size_t i = 0; i < num; ++i) {
      auto& member = _definition->get_struct_members()[i];
      switch (member->get_type()->get_reference_type()) {
        case reference_type::raw:
          break;
        case reference_type::unique: {
          // push back a raw value with a name
          // __#name
          containers::string name("__", m_allocator);
          name += member->get_name().to_string(m_allocator);
          memory::unique_ptr<declaration> decl =
              memory::make_unique<declaration>(m_allocator, m_allocator);
          decl->copy_location_from(member.get());
          memory::unique_ptr<type> new_type =
              memory::make_unique<type>(m_allocator, *member->get_type());
          new_type->copy_location_from(member->get_type());
          memory::unique_ptr<parameter> param = memory::make_unique<parameter>(
              m_allocator, m_allocator, core::move(new_type), name.c_str());
          param->get_type()->set_reference_type(reference_type::raw);
          param->get_type()->set_type_index(
              m_validator->get_type(param->get_type()->custom_type_name()));
          decl->set_parameter(core::move(param));
          decls.push_back(decl.get());
          _definition->get_struct_members().push_back(core::move(decl));
        } break;
        case reference_type::shared:
          // do nothing;
          shared_ids.push_back(member.get());
          break;
        default:
          WN_RELEASE_ASSERT_DESC(
              false, "Not implemented: complex reference_types");
      }
    }

    {  // Destructor
      memory::unique_ptr<instruction_list> instructions =
          memory::make_unique<instruction_list>(m_allocator, m_allocator);
      instructions->copy_location_from(_definition);

      memory::unique_ptr<type> void_type = memory::make_unique<type>(
          m_allocator, m_allocator, type_classification::void_type);
      void_type->copy_location_from(_definition);

      for (auto& a : decls) {
        // Unique objects are in practice backed by raw objects.
        // It is those raw objects that we want to destroy.
        WN_RELEASE_ASSERT_DESC(
            a->get_type()->get_reference_type() == reference_type::raw,
            "Not Implemented: Destruction of non-unique objects");

        memory::unique_ptr<member_access_expression> access =
            memory::make_unique<member_access_expression>(
                m_allocator, m_allocator, a->get_name());
        access->copy_location_from(a);

        memory::unique_ptr<id_expression> id =
            memory::make_unique<id_expression>(
                m_allocator, m_allocator, "_this");
        id->copy_location_from(a);

        access->add_base_expression(core::move(id));

        memory::unique_ptr<cast_expression> cast1 =
            memory::make_unique<cast_expression>(
                m_allocator, m_allocator, core::move(access));
        cast1->copy_location_from(cast1->get_expression());
        cast1->set_type(memory::make_unique<type>(m_allocator, *a->get_type()));
        cast1->get_type()->set_reference_type(reference_type::self);

        containers::string destructor_name("_destruct_", m_allocator);
        destructor_name.append(a->get_type()->custom_type_name().data(),
            a->get_type()->custom_type_name().size());

        memory::unique_ptr<arg_list> args =
            memory::make_unique<arg_list>(m_allocator, m_allocator);
        args->copy_location_from(cast1.get());
        args->add_expression(core::move(cast1));

        memory::unique_ptr<function_call_expression> call =
            memory::make_unique<function_call_expression>(
                m_allocator, m_allocator, core::move(args));
        call->copy_location_from(call->get_args());
        call->set_type(memory::make_unique<type>(m_allocator, *a->get_type()));
        call->get_type()->set_reference_type(reference_type::self);

        memory::unique_ptr<id_expression> destructor =
            memory::make_unique<id_expression>(
                m_allocator, m_allocator, destructor_name);
        destructor->copy_location_from(call.get());

        call->add_base_expression(core::move(destructor));

        memory::unique_ptr<expression_instruction> exprinst =
            memory::make_unique<expression_instruction>(
                m_allocator, m_allocator, core::move(call));
        exprinst->copy_location_from(exprinst->get_expression());
        instructions->add_instruction(core::move(exprinst));
      }
      for (auto& shared_id: shared_ids) {
        memory::unique_ptr<member_access_expression> access =
            memory::make_unique<member_access_expression>(
                m_allocator, m_allocator, shared_id->get_name());
        access->copy_location_from(shared_id);

        memory::unique_ptr<id_expression> id =
            memory::make_unique<id_expression>(
                m_allocator, m_allocator, "_this");
        id->copy_location_from(shared_id);

        access->add_base_expression(core::move(id));

        memory::unique_ptr<cast_expression> cast1 =
            memory::make_unique<cast_expression>(
                m_allocator, m_allocator, core::move(access));
        cast1->copy_location_from(cast1->get_expression());
        cast1->set_type(memory::make_unique<type>(
            m_allocator, m_allocator, type_classification::void_ptr_type));

        containers::string deref_name("_deref_shared", m_allocator);

        memory::unique_ptr<arg_list> args =
            memory::make_unique<arg_list>(m_allocator, m_allocator);
        args->copy_location_from(cast1.get());
        args->add_expression(core::move(cast1));

        memory::unique_ptr<function_call_expression> call =
            memory::make_unique<function_call_expression>(
                m_allocator, m_allocator, core::move(args));
        call->copy_location_from(call->get_args());
        call->set_type(memory::make_unique<type>(
            m_allocator, m_allocator, type_classification::void_type));

        memory::unique_ptr<id_expression> deref =
            memory::make_unique<id_expression>(
                m_allocator, m_allocator, deref_name);
        deref->copy_location_from(call.get());

        call->add_base_expression(core::move(deref));

        memory::unique_ptr<expression_instruction> exprinst =
            memory::make_unique<expression_instruction>(
                m_allocator, m_allocator, core::move(call));
        exprinst->copy_location_from(exprinst->get_expression());

        instructions->add_instruction(core::move(exprinst));
      }

      memory::unique_ptr<return_instruction> ret =
          memory::make_unique<return_instruction>(m_allocator, m_allocator);
      ret->copy_location_from(_definition);
      instructions->add_instruction(core::move(ret));

      add_function_from_definition("_destruct_", core::move(void_type),
          _definition, core::move(instructions));
    }
    {  // Constructor
      memory::unique_ptr<instruction_list> instructions =
          memory::make_unique<instruction_list>(m_allocator, m_allocator);
      instructions->copy_location_from(_definition);

      // Make all of the instructions to put into the constructor.
      // This means: Remove all of the right-hand-sides from the
      // declarations, and create assignment instructions in an instruction
      // list.
      for (auto& a : _definition->get_struct_members()) {
        memory::unique_ptr<expression> expr(a->take_expression());
        // There is no way to write code that is missing an expression on the
        // RHS.
        // This means we have removed the RHS, and so we should not create a
        // constructor for it.
        if (!expr) {
          continue;
        }
        memory::unique_ptr<member_access_expression> access =
            memory::make_unique<member_access_expression>(
                m_allocator, m_allocator, a->get_name());
        access->copy_location_from(a.get());

        memory::unique_ptr<id_expression> id =
            memory::make_unique<id_expression>(
                m_allocator, m_allocator, "_this");
        id->copy_location_from(a.get());

        access->add_base_expression(core::move(id));

        memory::unique_ptr<lvalue> lval = memory::make_unique<lvalue>(
            m_allocator, m_allocator, core::move(access));
        lval->copy_location_from(a.get());

        memory::unique_ptr<assignment_instruction> assignment =
            memory::make_unique<assignment_instruction>(
                m_allocator, m_allocator, core::move(lval));
        assignment->copy_location_from(expr.get());

        switch (a->get_type()->get_reference_type()) {
          case reference_type::raw: {
            assignment->add_value(assign_type::equal, core::move(expr));
          } break;
          case reference_type::unique: {
            // If this is a unique, that means the RHS was a struct_creation.
            // At least for now, so we should turn this into a construct call.
            containers::string constructor_name("_construct_", m_allocator);
            constructor_name.append(a->get_type()->custom_type_name().data(),
                a->get_type()->custom_type_name().size());

            // Create _this->x = construct(&_this->__x);

            memory::unique_ptr<id_expression> this_id =
                memory::make_unique<id_expression>(
                    m_allocator, m_allocator, "_this");
            this_id->copy_location_from(_definition);

            containers::string val("__", m_allocator);
            val += a->get_name().to_string(m_allocator);

            memory::unique_ptr<member_access_expression> real_access =
                memory::make_unique<member_access_expression>(
                    m_allocator, m_allocator, val);
            real_access->copy_location_from(a.get());
            real_access->add_base_expression(core::move(this_id));
            // real_access is now _this->__x

            memory::unique_ptr<cast_expression> cast1 =
                memory::make_unique<cast_expression>(
                    m_allocator, m_allocator, core::move(real_access));
            cast1->copy_location_from(cast1->get_expression());
            cast1->set_type(
                memory::make_unique<type>(m_allocator, *a->get_type()));
            cast1->get_type()->set_reference_type(reference_type::self);

            // cast1 is now &_this->__x;
            memory::unique_ptr<arg_list> args =
                memory::make_unique<arg_list>(m_allocator, m_allocator);
            args->copy_location_from(cast1.get());
            args->add_expression(core::move(cast1));

            memory::unique_ptr<function_call_expression> call =
                memory::make_unique<function_call_expression>(
                    m_allocator, m_allocator, core::move(args));
            call->copy_location_from(call->get_args());
            call->set_type(
                memory::make_unique<type>(m_allocator, *a->get_type()));
            call->get_type()->set_reference_type(reference_type::self);

            memory::unique_ptr<id_expression> constructor =
                memory::make_unique<id_expression>(
                    m_allocator, m_allocator, constructor_name);
            constructor->copy_location_from(call.get());

            call->add_base_expression(core::move(constructor));

            memory::unique_ptr<cast_expression> cast2 =
                memory::make_unique<cast_expression>(
                    m_allocator, m_allocator, core::move(call));
            cast2->copy_location_from(cast2->get_expression());
            cast2->set_type(memory::make_unique<type>(
                m_allocator, *cast2->get_expression()->get_type()));
            cast2->get_type()->set_reference_type(reference_type::unique);

            assignment->add_value(assign_type::equal, core::move(cast2));
          } break;
          case reference_type::shared: {
            if (expr->get_node_type() ==
                node_type::struct_allocation_expression) {
              memory::unique_ptr<expression> new_expr = walk_expression(
                  cast_to<struct_allocation_expression>(expr.get()));
              if (new_expr) {
                expr = core::move(new_expr);
              }
              expr->set_temporary(false);
            }
            assignment->set_in_constructor(true);
            assignment->add_value(assign_type::equal, core::move(expr));
            break;
          }
          default:
            WN_RELEASE_ASSERT_DESC(false, "Not Implemented other types in structs");
        }
        instructions->add_instruction(core::move(assignment));
      }

      memory::unique_ptr<id_expression> id =
          memory::make_unique<id_expression>(m_allocator, m_allocator, "_this");
      id->copy_location_from(_definition);

      memory::unique_ptr<return_instruction> ret =
          memory::make_unique<return_instruction>(
              m_allocator, m_allocator, core::move(id));
      ret->copy_location_from(_definition);
      instructions->add_instruction(core::move(ret));

      memory::unique_ptr<type> return_type = memory::make_unique<type>(
          m_allocator, m_allocator, _definition->get_name());
      return_type->set_reference_type(reference_type::self);
      return_type->copy_location_from(_definition);

      add_function_from_definition("_construct_", core::move(return_type),
          _definition, core::move(instructions));
    }
  }

  void walk_type(type*) {}
  void walk_function(function*) {}
  memory::unique_ptr<instruction> walk_instruction(instruction*) {
    return nullptr;
  }
  void enter_scope_block(const node*) {}
  void leave_scope_block(const node*) {}

private:
  void add_function_from_definition(containers::string_view _name,
      memory::unique_ptr<type>&& _return_type, struct_definition* _definition,
      memory::unique_ptr<instruction_list>&& _instructions) {
    // The constructor return_type and name are here.

    memory::unique_ptr<type> parameter_type = memory::make_unique<type>(
        m_allocator, m_allocator, _definition->get_name());
    parameter_type->set_reference_type(reference_type::self);
    parameter_type->copy_location_from(_definition);

    containers::string m_constructor_name(_name.to_string(m_allocator));
    m_constructor_name.append(
        _definition->get_name().data(), _definition->get_name().length());
    memory::unique_ptr<parameter> name_ret = memory::make_unique<parameter>(
        m_allocator, m_allocator, core::move(_return_type), m_constructor_name);
    name_ret->copy_location_from(_definition);

    memory::unique_ptr<parameter> this_param = memory::make_unique<parameter>(
        m_allocator, m_allocator, core::move(parameter_type), "_this");
    this_param->copy_location_from(_definition);

    memory::unique_ptr<parameter_list> parameters =
        memory::make_unique<parameter_list>(
            m_allocator, m_allocator, core::move(this_param));
    parameters->copy_location_from(_definition);

    memory::unique_ptr<function> constructor = memory::make_unique<function>(
        m_allocator, m_allocator, core::move(name_ret), core::move(parameters),
        core::move(_instructions));
    constructor->copy_location_from(_definition);
    constructor->set_this_pointer(_definition);

    m_additional_functions.push_back(core::move(constructor));
  }

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

bool run_temporary_reification_pass(script_file* _file, WNLogging::WNLog* _log,
    type_validator* _validator, size_t* _num_warnings, size_t* _num_errors) {
  return run_pass<temporary_reification_pass>(
      _file, _log, _validator, _num_warnings, _num_errors);
}

bool run_if_reassociation_pass(script_file* _file, WNLogging::WNLog* _log,
    type_validator* _validator, size_t* _num_warnings, size_t* _num_errors) {
  return run_pass<if_reassociation_pass>(
      _file, _log, _validator, _num_warnings, _num_errors);
}

}  // namespace scripting
}  // namespace wn
