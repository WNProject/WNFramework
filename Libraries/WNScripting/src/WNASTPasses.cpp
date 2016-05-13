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
      _inst->set_returns(returns);
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

  memory::unique_ptr<constant_expression> initializer =
      memory::make_unique<constant_expression>(
          _allocator, _allocator, _type, _init.to_string(_allocator).c_str());

  initializer->set_type(memory::make_unique<type>(_allocator, *decl_type));
  initializer->get_type()->copy_location_from(_location_root);

  memory::unique_ptr<parameter> param = memory::make_unique<parameter>(
      _allocator, _allocator, core::move(decl_type), _name);
  param->copy_location_from(_location_root);

  decl->set_parameter(core::move(param));
  decl->set_expression(core::move(initializer));
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
    : pass(_validator, _log, _allocator), m_last_temporary(0) {}
  void enter_scope_block() {}
  void leave_scope_block() {}
  memory::unique_ptr<expression> walk_expression(expression*) {
    return nullptr;
  }
  void walk_instruction_list(instruction_list* _inst) {
    using instruction_queue =
        containers::deque<memory::unique_ptr<instruction>>;
    instruction_queue replaced_instructions(m_allocator);
    for (instruction_queue::iterator it = _inst->get_instructions().begin();
         it != _inst->get_instructions().end(); ++it) {
      if ((*it)->get_node_type() == node_type::if_instruction) {
        if_instruction* if_inst = static_cast<if_instruction*>(it->get());

        containers::deque<memory::unique_ptr<else_if_instruction>>&
            else_if_insts = if_inst->get_else_if_instructions();

        // This is where we actually do the work.
        char temporary_name[12] = {0};

        wn::memory::writeuint32(
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
          wn::memory::writeuint32(
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

          for (auto& else_if : else_if_insts) {
            // Take the original body and condition.
            memory::unique_ptr<instruction_list> else_if_body =
                else_if->take_body();
            memory::unique_ptr<expression> else_if_condition =
                else_if->take_condition();

            // Inside the body we have to set the entering flag
            // to true
            else_if_body->get_instructions().emplace_front(
                make_constant_assignment(m_allocator, else_if.get(),
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
            may_enter->copy_location_from(else_if.get());

            replaced_instructions.emplace_back(make_assignment(
                m_allocator, else_if.get(), name, core::move(may_enter)));

            // if (inst) {
            memory::unique_ptr<if_instruction> conditional_if =
                memory::make_unique<if_instruction>(m_allocator, m_allocator);

            memory::unique_ptr<id_expression> entering =
                memory::make_unique<id_expression>(
                    m_allocator, m_allocator, name);
            entering->copy_location_from(else_if.get());

            conditional_if->set_condition(core::move(entering));

            // inst = original_condition
            memory::unique_ptr<instruction_list> inst_list =
                memory::make_unique<instruction_list>(m_allocator, m_allocator);
            inst_list->copy_location_from(else_if.get());

            inst_list->add_instruction(make_assignment(m_allocator,
                else_if.get(), name, core::move(else_if_condition)));

            conditional_if->set_body(core::move(inst_list));
            replaced_instructions.emplace_back(core::move(conditional_if));

            memory::unique_ptr<id_expression> id_expr =
                memory::make_unique<id_expression>(
                    m_allocator, m_allocator, name);
            id_expr->copy_location_from(else_if.get());

            memory::unique_ptr<if_instruction> replaced_if_inst =
                memory::make_unique<if_instruction>(m_allocator, m_allocator);
            replaced_if_inst->copy_location_from(else_if.get());
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
  void walk_script_file(script_file*) {}
  void walk_type(type*) {}
  void walk_instruction(instruction*) {}
  void walk_struct_definition(struct_definition*) {}
  void walk_instruction(else_if_instruction*) {}
  void walk_instruction(if_instruction*) {}

private:
  uint32_t m_last_temporary;
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
      m_functions(_allocator),
      m_local_objects(_allocator) {}
  memory::unique_ptr<expression> walk_expression(expression*) {
    return nullptr;
  }

  void walk_parameter(parameter*) {}
  void enter_scope_block() {
    m_local_objects.push_back(containers::deque<declaration*>(m_allocator));
  }

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
  void walk_instruction_list(instruction_list* instructions) {
    // If we are leaving a scope-block that had a return
    // at the end, then we do not need to emit desctuctors,
    // that will have already been done in the return.
    if (instructions->get_instructions().size() > 1 &&
        (instructions->get_instructions().back()->is_dead() ||
            instructions->get_instructions().back()->get_node_type() ==
                node_type::return_instruction)) {
      m_local_objects.pop_back();
      return;
    }
    if (m_local_objects.back().empty()) {
      m_local_objects.pop_back();
      return;
    }

    // We have walked every instruction in the instruction list,
     // we are just about to leave the scope block.
     // This means we have to run the destructor for any objects
     // that are bound by this scope.
    for(auto& decl: m_local_objects.back()) {

      containers::string m_destructor_name("_destruct_", m_allocator);
      m_destructor_name.append(decl->get_type()->custom_type_name().data(),
          decl->get_type()->custom_type_name().length());

      memory::unique_ptr<id_expression> object_id =
          memory::make_unique<id_expression>(
              m_allocator, m_allocator, decl->get_name());
      object_id->set_id_source({0, decl});
      object_id->copy_location_from(instructions);
      object_id->set_type(memory::make_unique<type>(m_allocator, *decl->get_type()));

      memory::unique_ptr<cast_expression> cast1 =
        memory::make_unique<cast_expression>(
            m_allocator, m_allocator, core::move(object_id));
      cast1->copy_location_from(cast1->get_expression());
      cast1->set_type(memory::make_unique<type>(m_allocator, *decl->get_type()));
      cast1->get_type()->set_reference_type(reference_type::self);

      memory::unique_ptr<arg_list> args =
          memory::make_unique<arg_list>(m_allocator, m_allocator);
      args->copy_location_from(instructions);
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
      instructions->add_instruction(memory::make_unique<expression_instruction>(
          m_allocator, m_allocator, core::move(call)));
    }
    m_local_objects.pop_back();
  }

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
    for (auto ref_type : {reference_type::self, reference_type::unique}) {
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

  void walk_instruction(assignment_instruction* _assign) {
    if (_assign->get_assignment_type() != assign_type::equal) {
      WN_RELEASE_ASSERT_DESC(
          false, "Not Implemented: Non-equality assignment instructions.");
    }

    switch (_assign->get_lvalue()->get_expression()->get_node_type()) {
      case node_type::replaced_expression:
        WN_RELEASE_ASSERT_DESC(false, "TODO: handle replaced instructions here");
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
        return;
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
    if (m_validator->get_cast(_decl->get_expression()->get_type()->get_index(),
            _decl->get_type()->get_index()) != cast_type::transparent) {
      m_log->Log(WNLogging::eError, 0,
          "Cannot inintialize variable with expression of a different type.");
      _decl->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
    }

    if (_decl->get_type()->get_reference_type() == reference_type::unique) {
      m_local_objects.back().push_back(_decl);
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

    m_returns.clear();
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
  containers::deque<containers::deque<declaration*>> m_local_objects;
};

containers::deque<const struct_allocation_expression*> accumulate_temporaries(
    memory::allocator* _allocator, const expression* _root,
    bool _include_top_expression) {
  containers::deque<const struct_allocation_expression*> expressions(
      _allocator);
  if (!_root) {
    return expressions;
  }
  if (_include_top_expression) {
    if (_root->get_node_type() == node_type::struct_allocation_expression) {
      expressions.push_back(
          static_cast<const struct_allocation_expression*>(_root));
    }
  } else {
    _root->accumulate_expression_subtree([&expressions](const expression* e) {
      if (e->get_node_type() == node_type::struct_allocation_expression) {
        expressions.push_back(
            static_cast<const struct_allocation_expression*>(e));
      }
    });
  }
  return core::move(expressions);
}

class member_reassociation_pass : public pass {
public:
  member_reassociation_pass(type_validator* _validator, WNLogging::WNLog* _log,
      memory::allocator* _allocator)
    : pass(_validator, _log, _allocator), m_additional_functions(_allocator) {}

  void walk_instruction(declaration* _decl) {
    if (_decl->get_type()->get_reference_type() == reference_type::raw) {
      return;
    }
    WN_RELEASE_ASSERT_DESC(
        _decl->get_type()->get_reference_type() == reference_type::unique,
        "Not Implemented: Other types of reference declarations");

    if (_decl->get_expression()->get_node_type() !=
        node_type::function_call_expression) {
      m_log->Log(WNLogging::eError, 0, "Expected constructor call");
      _decl->log_line(*m_log, WNLogging::eError);
      ++m_num_errors;
      return;
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
    for (auto& t :
        accumulate_temporaries(m_allocator, _decl->get_expression(), false)) {
      _decl->add_temporary(t);
    }
  }

  void walk_instruction(assignment_instruction* _assign) {
    for (auto& t :
        accumulate_temporaries(m_allocator, _assign->get_expression(), true)) {
      _assign->add_temporary(t);
    }
  }

  void walk_instruction(if_instruction* _if) {
    for (auto& t :
        accumulate_temporaries(m_allocator, _if->get_condition(), true)) {
      _if->add_temporary(t);
    }
  }

  void walk_instruction(expression_instruction* _expr) {
    for (auto& t :
        accumulate_temporaries(m_allocator, _expr->get_expression(), true)) {
      _expr->add_temporary(t);
    }
  }

  void walk_instruction(else_if_instruction*) {
    WN_DEBUG_ASSERT_DESC(false,
        "We should not end up here"
        "all else_if blocks should have been removed in the if_reassociation "
        "pass");
  }

  void walk_instruction(return_instruction* _ret) {
    for (auto& t :
        accumulate_temporaries(m_allocator, _ret->get_expression(), true)) {
      _ret->add_temporary(t);
    }
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

    memory::unique_ptr<struct_allocation_expression> alloc =
        memory::make_unique<struct_allocation_expression>(
            m_allocator, m_allocator);
    alloc->set_initialization_mode(struct_initialization_mode::unspecified);
    alloc->copy_location_from(_alloc);
    alloc->set_type(_alloc->transfer_out_type());
    alloc->set_copy_initializer(_alloc->transfer_copy_initializer());

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

    memory::unique_ptr<id_expression> constructor =
        memory::make_unique<id_expression>(
            m_allocator, m_allocator, constructor_name);
    constructor->copy_location_from(call.get());

    call->add_base_expression(core::move(constructor));

    return core::move(call);
  }

  void walk_parameter(parameter*) {}
  void walk_instruction_list(instruction_list*) {}

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
    {  // Destructor
      memory::unique_ptr<instruction_list> instructions =
          memory::make_unique<instruction_list>(m_allocator, m_allocator);
      instructions->copy_location_from(_definition);

      memory::unique_ptr<type> void_type = memory::make_unique<type>(
          m_allocator, m_allocator, type_classification::void_type);
      void_type->copy_location_from(_definition);

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
        memory::unique_ptr<expression> expression(a->take_expression());

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
        assignment->copy_location_from(expression.get());
        assignment->add_value(assign_type::equal, core::move(expression));
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
  void walk_instruction(instruction*) {}
  void enter_scope_block() {}
  void leave_scope_block() {}

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

bool run_if_reassociation_pass(script_file* _file, WNLogging::WNLog* _log,
    type_validator* _validator, size_t* _num_warnings, size_t* _num_errors) {
  return run_pass<if_reassociation_pass>(
      _file, _log, _validator, _num_warnings, _num_errors);
}

}  // namespace scripting
}  // namespace wn
