// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_NODE_TYPES_H__
#define __WN_SCRIPTING_NODE_TYPES_H__

#include "WNContainers/inc/WNDeque.h"
#include "WNContainers/inc/WNString.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNFunctional/inc/WNFunction.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/WNUniquePtr.h"
#include "WNScripting/inc/WNEnums.h"
#include "WNScripting/inc/WNErrors.h"

namespace wn {
namespace scripting {
struct source_location {
  int64_t m_start_index;
  int64_t m_end_index;
  const uint8_t* m_line_start;
  size_t m_line_number;
  size_t m_char_number;
};

// clang-format off
enum class node_type {
  arg_list,
  array_type,
  concretized_array_type,
  parameter_list,
  parameter,
  expression,
    replaced_expression,
    array_allocation_expression,
    binary_expression,
    cast_expression,
    cond_expression,
    constant_expression,
    id_expression,
    function_pointer_expression,
    null_allocation_expression,
    post_expression,
      array_access_expression,
      function_call_expression,
      member_access_expression,
      post_unary_expression,
    sizeof_expression,
    short_circuit_expression,
    struct_allocation_expression,
    unary_expression,
  function,
  instruction,
    assignment_instruction,
    break_instruction,
    continue_instruction,
    declaration,
    set_array_length,
    do_instruction,
    else_if_instruction,
    expression_instruction,
    for_instruction,
    if_instruction,
    return_instruction,
    while_instruction,
  instruction_list,
  lvalue,
  script_file,
  struct_definition,
  type
};
// clang-format on
class declaration;
class expression;
class function;
class instruction;
class node;
class parameter;

template <typename T>
const T* cast_to(const node* _node) {
  return static_cast<const T*>(_node);
}

template <typename T>
T* cast_to(node* _node) {
  return static_cast<T*>(_node);
}

template <typename T>
memory::unique_ptr<T> clone_node(const T* val) {
  memory::unique_ptr<node> n = val->clone();
  memory::allocator* alloc = n.get_allocator();
  // FUNCTION CALLS DO NOT GUARANTEE ARGUMENT ORDERING.
  // the allocator may have already been destroyed if you call
  // n.get_allocator()         vvvvv    there
  return memory::unique_ptr<T>(alloc, static_cast<T*>(n.release()));
}

template <typename T>
memory::unique_ptr<T> clone_node(const memory::unique_ptr<T>& val) {
  if (!val) {
    return memory::unique_ptr<T>(nullptr);
  }
  memory::unique_ptr<node> n = val->clone();
  memory::allocator* alloc = n.get_allocator();
  return memory::unique_ptr<T>(alloc, static_cast<T*>(n.release()));
}

using walk_mutable_expression =
    functional::function<memory::unique_ptr<expression>(expression*)>;

template <typename T>
using walk_ftype = functional::function<void(T)>;
using walk_scope = functional::function<void(const node*)>;

WN_INLINE void handle_expression(const walk_mutable_expression& function,
    memory::unique_ptr<expression>& expr) {
  while (auto a = function(expr.get())) {
    expr = core::move(a);
  }
}

using walk_mutable_instruction =
    functional::function<memory::unique_ptr<instruction>(instruction*)>;
WN_INLINE void handle_instruction(const walk_mutable_instruction& function,
    memory::unique_ptr<instruction>& inst) {
  while (auto a = function(inst.get())) {
    inst = core::move(a);
  }
}

struct print_context {
  print_context(memory::allocator* _allocator, logging::log* _log,
      logging::log_level _level, const containers::string_view _prefix)
    : m_prefix(_prefix.to_string(_allocator)),
      m_allocator(_allocator),
      m_level(_level),
      m_log(_log) {}
  using node_type = const node*;
  containers::string m_prefix;
  memory::allocator* m_allocator;
  logging::log_level m_level;
  logging::log* m_log;
  bool ignore_next_prefix = false;

  void enter_log_scope() {
    ignore_next_prefix = false;
    m_prefix.insert(0, "|   ");
  }
  void leave_log_scope() {
    m_prefix = m_prefix.substr(0, m_prefix.size() - 4);
    ignore_next_prefix = false;
  }
  void print_prefix() {
    if (ignore_next_prefix) {
      ignore_next_prefix = false;
    } else {
      m_log->log_params(m_level,
          static_cast<size_t>(logging::log_flags::no_newline), m_prefix);
    }
  }
  template <typename... Args>
  void print_line(const Args&... args) {
    print_prefix();
    m_log->log_params(
        m_level, static_cast<size_t>(logging::log_flags::no_header), args...);
  }

  template <typename... Args>
  void print_header(const Args&... args) {
    if (ignore_next_prefix) {
      m_log->log_params(
          m_level, static_cast<size_t>(logging::log_flags::no_header), args...);
    } else {
      m_log->log_params(m_level, 0, m_prefix, args...);
    }

    ignore_next_prefix = false;
  }
  template <typename... Args, typename V>
  void print_value(const V& value, const Args&... name) {
    print_line("|-", name..., " = ", value);
  }
  template <typename... Args>
  void print_value(const containers::string& value, const Args&... name) {
    if (value.empty()) {
      print_line("|-", name..., " = \"\"");
    } else {
      print_line("|-", name..., " = ", value);
    }
  }
  template <typename... Args>
  void print_value(const node_type& value, const Args&... name);

  template <typename... Args, typename T>
  void print_value(const memory::unique_ptr<T>& value, const Args&... name) {
    return print_value(static_cast<const node*>(value.get()), name...);
  }

  template <typename... Args, typename T>
  void print_value(
      const containers::dynamic_array<T>& value, const Args&... name) {
    if (value.empty()) {
      print_line("|-", name..., " = <EMPTY>");
    } else {
      print_line("|-", name..., " = ");
      enter_log_scope();
      for (size_t i = 0; i < value.size(); ++i) {
        print_value(value[i], "|-[", i, "]");
      }
      leave_log_scope();
    }
  }

  template <typename... Args, typename T>
  void print_value(const containers::deque<T>& value, const Args&... name) {
    if (value.empty()) {
      print_line("|-", name..., " = <EMPTY>");
    } else {
      print_line("|-", name..., " = ");
      enter_log_scope();
      for (size_t i = 0; i < value.size(); ++i) {
        print_value(value[i], "|-{", i, "}");
      }
      leave_log_scope();
    }
  }
};

// Base class for all nodes in AST.
class node {
public:
  node(memory::allocator* _allocator, node_type _type)
    : m_allocator(_allocator), m_type(_type), m_is_dead(false) {
#ifdef WN_ALLOW_TESTING_LOG
    __f = &node::print_node;
#endif
  }
  virtual ~node() {}

  void copy_location_from(const node* _node) {
    m_source_location = _node->m_source_location;
  }

  // Sets the start location of first character representing this node.
  void set_start_location(const source_location& _location) {
    m_source_location = _location;
  }

  // Modifies the source_location with the given source index of the last
  // character that represents this node.
  void set_end_index(int64_t _idx) {
    m_source_location.m_end_index = _idx;
  }

  // Returns the source location representing this node.
  const source_location& get_start_location() const {
    return (m_source_location);
  }

  // Writes the first line of this expression to the given log.
  void log_line(logging::log* _log, logging::log_level _level) const {
    size_t line_length = 1023;
    const char* c = memory::strnchr(
        reinterpret_cast<const char*>(m_source_location.m_line_start), '\n',
        1023);
    if (c == nullptr) {
      // If there is no \n then we simply print everything
      c = reinterpret_cast<const char*>(m_source_location.m_line_start) +
          memory::strnlen(
              reinterpret_cast<const char*>(m_source_location.m_line_start),
              1023);
    }
    line_length =
        (c - reinterpret_cast<const char*>(m_source_location.m_line_start));
    char* data_buffer = static_cast<char*>(WN_STACK_ALLOC(line_length + 1));
    memcpy(data_buffer, m_source_location.m_line_start, line_length);
    data_buffer[line_length] = '\0';
    _log->log_params(_level, 0, "Line: ", m_source_location.m_line_number, "\n",
        data_buffer);
  }

  node_type get_node_type() const {
    return m_type;
  }
  memory::allocator* get_allocator() const {
    return m_allocator;
  }

  virtual memory::unique_ptr<node> clone() const = 0;

  void print_node(logging::log* _log, logging::log_level _level) const {
    print_context c(m_allocator, _log, _level, "");
    return print_node_internal(&c);
  }
  virtual void print_node_internal(print_context*) const {}

protected:
  void copy_node(const node* _other) {
    copy_location_from(_other);
    m_is_dead = _other->m_is_dead;
  }

  // Location of the first character of the first token contributing
  // to this node.
  source_location m_source_location;
  memory::allocator* m_allocator;
  node_type m_type;
  bool m_is_dead;
#ifdef WN_ALLOW_TESTING_LOG
  // Force the print_node functions to not go away if
  // test logging is turned on.
  void (wn::scripting::node::*__f)(logging::log*, logging::log_level) const;
#endif
};

template <typename... Args>
void print_context::print_value(
    const print_context::node_type& value, const Args&... name) {
  if (!ignore_next_prefix) {
    m_log->log_params(m_level,
        static_cast<size_t>(logging::log_flags::no_newline), m_prefix, "|-",
        name..., " = [", value, "]");
  } else {
    m_log->log_params(m_level,
        static_cast<size_t>(logging::log_flags::no_newline) |
            static_cast<size_t>(logging::log_flags::no_header),
        "|-", name..., " = [", value, "]");
  }

  enter_log_scope();
  ignore_next_prefix = true;
  if (value) {
    value->print_node_internal(this);
  } else {
    print_line("<NULL>");
  }
  leave_log_scope();
}

class type : public node {
public:
  type(memory::allocator* _allocator, const char* _custom_type)
    : node(_allocator, node_type::type),
      m_type(0),
      m_reference_type(reference_type::raw),
      m_custom_type(_allocator, _custom_type) {}

  explicit type(memory::allocator* _allocator)
    : node(_allocator, node_type::type) {}

  type(memory::allocator* _allocator, containers::string_view _custom_type)
    : node(_allocator, node_type::type),
      m_type(0),
      m_reference_type(reference_type::raw),
      m_custom_type(_custom_type.to_string(_allocator)) {}

  type(memory::allocator* _allocator, uint32_t _type)
    : node(_allocator, node_type::type),
      m_type(_type),
      m_reference_type(reference_type::raw) {}

  type(memory::allocator* _allocator, uint32_t _type, reference_type _ref)
    : node(_allocator, node_type::type),
      m_type(_type),
      m_reference_type(_ref) {}

  type(memory::allocator* _allocator, type_classification _type,
      node_type _node_type = node_type::type,
      reference_type _ref = reference_type::raw)
    : node(_allocator, _node_type),
      m_type(static_cast<uint32_t>(_type)),
      m_reference_type(_ref),
      m_custom_type(_allocator) {}

  type(const type& _other) = delete;

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<type> t =
        memory::make_unique<type>(m_allocator, m_allocator);
    t->copy_type(this);
    return core::move(t);
  }

  // Only valid after the type_association
  // pass is run.
  bool operator==(const type& _other) const {
    return m_type == _other.m_type &&
           m_reference_type == _other.m_reference_type;
  }

  void set_reference_type(reference_type _reference_type) {
    // If we adjust the reference type, then we have to
    // adjust the type id as well.
    if (m_type != static_cast<uint32_t>(type_classification::array_type)) {
      m_type += (static_cast<int32_t>(_reference_type) -
                 static_cast<int32_t>(m_reference_type));
    }
    m_reference_type = _reference_type;
  }

  void set_type_index(uint32_t _index) {
    m_type = _index + (static_cast<uint32_t>(m_reference_type));
  }

  bool operator!=(const type& _other) const {
    return !(*this == _other);
  }

  uint32_t get_type_value() const {
    return static_cast<uint32_t>(m_type);
  }

  uint32_t get_unreferenced_type() const {
    return m_type - (static_cast<uint32_t>(m_reference_type));
  }

  uint32_t get_index() const {
    return m_type;
  }
  containers::string_view custom_type_name() const {
    return m_custom_type;
  }

  reference_type get_reference_type() const {
    return m_reference_type;
  }

  virtual const type* get_subtype() const {
    return nullptr;
  }

  virtual type* get_subtype() {
    return nullptr;
  }

  virtual void walk_children(const walk_ftype<type*>&) {}

  virtual void walk_children(const walk_ftype<const type*>&) const {}

  void print_node_internal(print_context* c) const override {
    c->print_header("Type");
    c->print_value(m_custom_type, "Custom Name");
    c->print_value(m_type, "Numeric Type");
    c->print_value(m_reference_type, "Reference Type");
  }

protected:
  void copy_type(const type* _other) {
    copy_node(_other);
    m_type = _other->m_type;
    m_reference_type = _other->m_reference_type;
    m_custom_type = containers::string(m_allocator, _other->m_custom_type);
  }

  uint32_t m_type;
  reference_type m_reference_type;
  containers::string m_custom_type;
};

class array_type : public type {
public:
  array_type(memory::allocator* _allocator, type* _sub_type)
    : type(_allocator, type_classification::array_type, node_type::array_type),
      m_subtype(memory::unique_ptr<type>(_allocator, _sub_type)) {}

  array_type(memory::allocator* _allocator, memory::unique_ptr<type> _sub_type)
    : type(_allocator, type_classification::array_type, node_type::array_type),
      m_subtype(core::move(_sub_type)) {}

  explicit array_type(memory::allocator* _allocator)
    : type(_allocator, type_classification::array_type, node_type::array_type) {
  }

  array_type(memory::allocator* _allocator, node_type _node_type)
    : type(_allocator, type_classification::array_type, _node_type) {}

  array_type(memory::allocator* _allocator, node_type _node_type,
      memory::unique_ptr<type> _sub_type)
    : type(_allocator, type_classification::array_type, _node_type),
      m_subtype(core::move(_sub_type)) {}

  const type* get_subtype() const override {
    return m_subtype.get();
  }

  type* get_subtype() override {
    return m_subtype.get();
  }

  void set_subtype(memory::unique_ptr<type>&& _subtype) {
    m_subtype = core::move(_subtype);
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<array_type> t =
        memory::make_unique<array_type>(m_allocator, m_allocator);
    t->copy_type(this);
    t->m_subtype = clone_node(m_subtype);
    return core::move(t);
  }

  void copy_array_type(const array_type* _other) {
    copy_type(_other);
    m_subtype = clone_node(_other->m_subtype);
  }

  void walk_children(const walk_ftype<type*>& _type) override {
    _type(m_subtype.get());
  }

  void walk_children(const walk_ftype<const type*>& _type) const override {
    _type(m_subtype.get());
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Array");
    c->print_value(m_subtype, "SubType");
  }

private:
  memory::unique_ptr<type> m_subtype;
};

class concretized_array_type : public type {
public:
  concretized_array_type(memory::allocator* _allocator)
    : type(_allocator, type_classification::array_type,
          node_type::concretized_array_type),
      m_array_sizes(_allocator) {}

  void set_array_sizes(const containers::dynamic_array<uint32_t>& _size) {
    m_array_sizes.clear();
    m_array_sizes.insert(m_array_sizes.begin(), _size.begin(), _size.end());
  }

  void add_array_size(uint32_t size) {
    m_array_sizes.push_back(size);
  }

  const containers::dynamic_array<uint32_t>& get_sizes() const {
    return m_array_sizes;
  }

  void walk_children(const walk_ftype<type*>& _type) override {
    _type(m_subtype.get());
  }

  void walk_children(const walk_ftype<const type*>& _type) const override {
    _type(m_subtype.get());
  }

  const type* get_subtype() const override {
    return m_subtype.get();
  }

  type* get_subtype() override {
    return m_subtype.get();
  }

  void set_subtype(memory::unique_ptr<type>&& _subtype) {
    m_subtype = core::move(_subtype);
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<concretized_array_type> t =
        memory::make_unique<concretized_array_type>(m_allocator, m_allocator);
    t->copy_type(this);
    t->m_subtype = clone_node(m_subtype);
    t->m_array_sizes.insert(
        t->m_array_sizes.begin(), m_array_sizes.begin(), m_array_sizes.end());
    return core::move(t);
  }

  void copy_array_type(const concretized_array_type* _other) {
    copy_type(_other);
    m_subtype = clone_node(_other->m_subtype);
    m_array_sizes.clear();
    m_array_sizes.insert(m_array_sizes.begin(), _other->m_array_sizes.begin(),
        _other->m_array_sizes.end());
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("SizedArray");
    c->print_value(m_array_sizes, "NumElements");
    c->print_value(m_subtype, "Subtype");
  }

private:
  containers::dynamic_array<uint32_t> m_array_sizes;
  memory::unique_ptr<type> m_subtype;
};

class struct_allocation_expression;

// Base class for all expression nodes in the AST.
class expression : public node {
public:
  expression(memory::allocator* _allocator, node_type _type)
    : node(_allocator, _type), m_type(nullptr), m_is_temporary(false) {}

  expression(memory::allocator* _allocator, node_type _node_type, type* _type)
    : node(_allocator, _node_type),
      m_type(memory::unique_ptr<type>(_allocator, _type)),
      m_is_temporary(false) {}

  const type* get_type() const {
    return m_type.get();
  }
  type* get_type() {
    return m_type.get();
  }
  void set_type(type* _type) {
    m_type = memory::unique_ptr<type>(m_allocator, _type);
  }

  void set_type(memory::unique_ptr<type>&& _type) {
    m_type = core::move(_type);
  }

  virtual void walk_children(
      const walk_mutable_expression&, const walk_ftype<type*>&) {
    WN_RELEASE_ASSERT_DESC(false, "Not implemented yet");
  }
  virtual void walk_children(const walk_ftype<const expression*>&,
      const walk_ftype<const type*>&) const {
    WN_RELEASE_ASSERT_DESC(false, "Not implemented yet");
  }

  memory::unique_ptr<type> transfer_out_type() {
    return core::move(m_type);
  }

  // Constructs a new expression from this expression but transfers out
  // all of the internals.
  virtual memory::unique_ptr<expression> transfer_to_new() = 0;

  bool is_temporary() const {
    return m_is_temporary;
  }

  void set_temporary(bool _temporary) {
    m_is_temporary = _temporary;
  }

protected:
  void copy_expression(const expression* _other) {
    copy_node(_other);
    m_type = clone_node(_other->m_type);
    m_is_temporary = _other->m_is_temporary;
  }

  memory::unique_ptr<type> m_type;
  bool m_is_temporary;
};

enum class struct_initialization_mode {
  invalid,
  unspecified,
  unique,
  strong,
  max
};

class array_allocation_expression : public expression {
public:
  explicit array_allocation_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::array_allocation_expression),
      m_array_initializers(_allocator),
      m_levels(0),
      m_init_mode(struct_initialization_mode::invalid) {}

  containers::deque<memory::unique_ptr<expression>>& get_array_initializers() {
    return m_array_initializers;
  }

  void set_static_array_initializers(
      containers::dynamic_array<uint32_t>&& _dynamic_array) {
    m_static_array_initializers = core::move(_dynamic_array);
    m_array_initializers.clear();
  }

  struct_initialization_mode set_initialization_mode() const {
    return m_init_mode;
  }

  void set_initialization_mode(struct_initialization_mode _mode) {
    m_init_mode = _mode;
  }

  const containers::dynamic_array<uint32_t>& get_static_array_initializers()
      const {
    return m_static_array_initializers;
  }

  bool is_static_sized() const {
    return m_static_array_initializers.size() > 0;
  }

  void add_expression(expression* _expr) {
    m_array_initializers.emplace_back(
        memory::unique_ptr<expression>(m_allocator, _expr));
    m_levels++;
  }
  void add_level() {
    m_levels++;
  }
  void set_copy_initializer(expression* _expression) {
    m_copy_initializer =
        memory::unique_ptr<expression>(m_allocator, _expression);
  }

  const expression* get_copy_initializer() const {
    return m_copy_initializer.get();
  }

  memory::unique_ptr<expression> take_initializer() {
    return core::move(m_copy_initializer);
  }

  containers::string constructor_name() {
    return m_constructor_name;
  }

  void set_constructor_name(containers::string_view name) {
    m_constructor_name = name.to_string(m_allocator);
  }

  // Constructs a new expression from this expression but transfers out
  // all of the internals.
  memory::unique_ptr<expression> transfer_to_new() override {
    memory::unique_ptr<array_allocation_expression> alloc =
        memory::make_unique<array_allocation_expression>(
            m_allocator, m_allocator);
    alloc->copy_location_from(this);
    alloc->set_type(core::move(m_type));
    alloc->m_is_dead = m_is_dead;
    alloc->m_is_temporary = m_is_temporary;
    alloc->m_array_initializers = core::move(m_array_initializers);
    alloc->m_copy_initializer = core::move(m_copy_initializer);
    alloc->m_levels = core::move(m_copy_initializer);
    return core::move(alloc);
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<array_allocation_expression> t =
        memory::make_unique<array_allocation_expression>(
            m_allocator, m_allocator);
    t->copy_expression(this);

    for (auto& a : m_array_initializers) {
      t->m_array_initializers.emplace_back(clone_node(a));
    }
    t->m_copy_initializer = clone_node(m_copy_initializer);
    return core::move(t);
  }

  virtual void walk_children(
      const walk_mutable_expression& _expr, const walk_ftype<type*>& _type) {
    for (auto& expr : m_array_initializers) {
      handle_expression(_expr, expr);
    }

    _type(m_type.get());
  }

  virtual void walk_children(const walk_ftype<const expression*>& _expr,
      const walk_ftype<const type*>& _type) const {
    for (auto& expr : m_array_initializers) {
      _expr(expr.get());
    }

    _type(m_type.get());
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Array Initializer");
    c->print_value(m_type, "Type");
    c->print_value(m_array_initializers, "Initializers");
    c->print_value(m_static_array_initializers, "Static Initializers");
    c->print_value(m_copy_initializer, "Copy Initializer");
  }

private:
  containers::deque<memory::unique_ptr<expression>> m_array_initializers;
  containers::dynamic_array<uint32_t> m_static_array_initializers;
  memory::unique_ptr<expression> m_copy_initializer;
  containers::string m_constructor_name;
  struct_initialization_mode m_init_mode;
  size_t m_levels;
};

class binary_expression : public expression {
public:
  binary_expression(memory::allocator* _allocator, arithmetic_type _type,
      expression* _lhs, expression* _rhs)
    : expression(_allocator, node_type::binary_expression),
      m_arith_type(_type),
      m_lhs(memory::unique_ptr<expression>(m_allocator, _lhs)),
      m_rhs(memory::unique_ptr<expression>(m_allocator, _rhs)) {}

  binary_expression(memory::allocator* _allocator, arithmetic_type _type,
      memory::unique_ptr<expression> _lhs, memory::unique_ptr<expression> _rhs)
    : expression(_allocator, node_type::binary_expression),
      m_arith_type(_type),
      m_lhs(core::move(_lhs)),
      m_rhs(core::move(_rhs)) {}

  explicit binary_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::binary_expression),
      m_arith_type(arithmetic_type::max) {}

  // Constructs a new expression from this expression but transfers out
  // all of the internals.
  memory::unique_ptr<expression> transfer_to_new() override {
    memory::unique_ptr<binary_expression> expr =
        memory::make_unique<binary_expression>(m_allocator, m_allocator);
    expr->copy_location_from(this);
    expr->set_type(core::move(m_type));
    expr->m_is_dead = m_is_dead;
    expr->m_is_temporary = m_is_temporary;
    expr->m_arith_type = m_arith_type;
    expr->m_lhs = core::move(m_lhs);
    expr->m_rhs = core::move(m_rhs);
    return core::move(expr);
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<binary_expression> t =
        memory::make_unique<binary_expression>(m_allocator, m_allocator);
    t->copy_expression(this);

    t->m_arith_type = m_arith_type;
    t->m_lhs = clone_node(m_lhs);
    t->m_rhs = clone_node(m_rhs);

    return core::move(t);
  }

  const expression* get_lhs() const {
    return m_lhs.get();
  }
  const expression* get_rhs() const {
    return m_rhs.get();
  }
  const arithmetic_type get_arithmetic_type() const {
    return m_arith_type;
  }

  virtual void walk_children(
      const walk_mutable_expression& _func, const walk_ftype<type*>&) override {
    handle_expression(_func, m_lhs);
    handle_expression(_func, m_rhs);
  }
  virtual void walk_children(const walk_ftype<const expression*>& _func,
      const walk_ftype<const type*>&) const override {
    _func(m_lhs.get());
    _func(m_rhs.get());
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Binary Expression");
    c->print_value(m_type, "Type");
    c->print_value(m_arith_type, "ArithType");
    c->print_value(m_lhs, "LHS");
    c->print_value(m_rhs, "RHS");
  }

private:
  arithmetic_type m_arith_type;
  memory::unique_ptr<expression> m_lhs;
  memory::unique_ptr<expression> m_rhs;
};

class cond_expression : public expression {
public:
  cond_expression(memory::allocator* _allocator, expression* _cond,
      expression* _lhs, expression* _rhs)
    : expression(_allocator, node_type::cond_expression),
      m_condition(memory::unique_ptr<expression>(m_allocator, _cond)),
      m_lhs(memory::unique_ptr<expression>(m_allocator, _lhs)),
      m_rhs(memory::unique_ptr<expression>(m_allocator, _rhs)) {}

  explicit cond_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::cond_expression) {}

  memory::unique_ptr<expression> transfer_to_new() override {
    memory::unique_ptr<cond_expression> expr =
        memory::make_unique<cond_expression>(m_allocator, m_allocator);
    expr->copy_location_from(this);
    expr->set_type(core::move(m_type));
    expr->m_is_dead = m_is_dead;
    expr->m_is_temporary = m_is_temporary;
    expr->m_condition = core::move(m_condition);
    expr->m_lhs = core::move(m_lhs);
    expr->m_rhs = core::move(m_rhs);
    return core::move(expr);
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<cond_expression> t =
        memory::make_unique<cond_expression>(m_allocator, m_allocator);
    t->copy_expression(this);

    t->m_condition = clone_node(m_condition);
    t->m_lhs = clone_node(m_lhs);
    t->m_rhs = clone_node(m_rhs);

    return core::move(t);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Conditional Expression");
    c->print_value(m_type, "Type");
    c->print_value(m_condition, "Condition");
    c->print_value(m_lhs, "LHS");
    c->print_value(m_rhs, "RHS");
  }

private:
  memory::unique_ptr<expression> m_condition;
  memory::unique_ptr<expression> m_lhs;
  memory::unique_ptr<expression> m_rhs;
};
class constant_expression : public expression {
public:
  constant_expression(memory::allocator* _allocator, type_classification _type,
      const char* _text)
    : expression(_allocator, node_type::constant_expression),
      m_type_classification(static_cast<uint32_t>(_type)),
      m_text(_allocator, _text) {}

  constant_expression(
      memory::allocator* _allocator, type* _type, const char* _text)
    : expression(_allocator, node_type::constant_expression, _type),
      m_type_classification(_type->get_index()),
      m_text(_allocator, _text) {}

  explicit constant_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::constant_expression) {}

  uint32_t get_index() const {
    return m_type_classification;
  }
  const containers::string& get_type_text() const {
    return m_text;
  }

  virtual void walk_children(
      const walk_mutable_expression&, const walk_ftype<type*>& _type) override {
    _type(m_type.get());
  }
  virtual void walk_children(const walk_ftype<const expression*>&,
      const walk_ftype<const type*>& _type) const override {
    _type(m_type.get());
  }

  memory::unique_ptr<expression> transfer_to_new() override {
    memory::unique_ptr<constant_expression> expr =
        memory::make_unique<constant_expression>(m_allocator, m_allocator);
    expr->copy_location_from(this);
    expr->set_type(core::move(m_type));
    expr->m_is_dead = m_is_dead;
    expr->m_is_temporary = m_is_temporary;
    expr->m_type_classification = m_type_classification;
    expr->m_text = core::move(m_text);
    return core::move(expr);
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<constant_expression> t =
        memory::make_unique<constant_expression>(m_allocator, m_allocator);
    t->copy_expression(this);

    t->m_type_classification = m_type_classification;
    t->m_text = containers::string(m_allocator, m_text);

    return core::move(t);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Constant");
    c->print_value(m_type, "Type");
    c->print_value(m_type_classification, "Classification");
    c->print_value(m_text, "Value");
  }

private:
  uint32_t m_type_classification;
  containers::string m_text;
};

class function_pointer_expression : public expression {
public:
  function_pointer_expression(memory::allocator* _allocator, const char* _name)
    : expression(_allocator, node_type::function_pointer_expression),
      m_source(nullptr),
      m_name(_allocator, _name),
      m_types(_allocator) {}

  function_pointer_expression(
      memory::allocator* _allocator, containers::string_view _name)
    : expression(_allocator, node_type::function_pointer_expression),
      m_source(nullptr),
      m_name(_name.to_string(_allocator)),
      m_types(_allocator) {}

  explicit function_pointer_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::function_pointer_expression),
      m_source(nullptr),
      m_types(_allocator) {}

  containers::string_view get_name() const {
    return m_name;
  }

  virtual void walk_children(
      const walk_mutable_expression&, const walk_ftype<type*>& _type) override {
    if (m_type) {
      _type(m_type.get());
    }
    for (auto& type : m_types) {
      _type(type.get());
    }
  }

  virtual void walk_children(const walk_ftype<const expression*>&,
      const walk_ftype<const type*>& _type) const override {
    if (m_type) {
      _type(m_type.get());
    }
    for (auto& type : m_types) {
      _type(type.get());
    }
  }

  void set_source(function* _source) {
    m_source = _source;
  };

  const function* get_source() const {
    return m_source;
  }

  memory::unique_ptr<expression> transfer_to_new() override {
    memory::unique_ptr<function_pointer_expression> expr =
        memory::make_unique<function_pointer_expression>(
            m_allocator, m_allocator);
    expr->copy_location_from(this);
    expr->set_type(core::move(m_type));
    expr->m_is_dead = m_is_dead;
    expr->m_is_temporary = m_is_temporary;
    expr->m_source = m_source;
    expr->m_name = core::move(m_name);
    return core::move(expr);
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<function_pointer_expression> t =
        memory::make_unique<function_pointer_expression>(
            m_allocator, m_allocator);
    t->copy_expression(this);
    t->set_source(m_source);
    t->m_name = containers::string(m_allocator, m_name);
    for (const auto& ty : m_types) {
      t->m_types.push_back(clone_node(ty));
    }
    return core::move(t);
  }

  void add_param(memory::unique_ptr<type>&& _type) {
    m_types.push_back(core::move(_type));
  }

  const containers::deque<memory::unique_ptr<type>>& get_types() {
    return m_types;
  }

  virtual void print_node_internal(print_context* c) const {
    c->print_header("Function Pointer");
    c->print_value(m_name, "Function");
    c->print_value(m_type, "Type");
    c->print_value(m_types, "Types");
  }

private:
  function* m_source;
  containers::deque<memory::unique_ptr<type>> m_types;
  containers::string m_name;
  containers::string m_mangled_name;
};

class id_expression : public expression {
public:
  id_expression(memory::allocator* _allocator, const char* _name)
    : expression(_allocator, node_type::id_expression),
      m_source({nullptr, nullptr, containers::deque<function*>(_allocator)}),
      m_name(_allocator, _name) {}

  id_expression(memory::allocator* _allocator, containers::string_view _name)
    : expression(_allocator, node_type::id_expression),
      m_source({nullptr, nullptr, containers::deque<function*>(_allocator)}),
      m_name(_name.to_string(_allocator)) {}

  explicit id_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::id_expression),
      m_source({nullptr, nullptr, containers::deque<function*>(_allocator)}) {}

  containers::string_view get_name() const {
    return m_name;
  }

  struct id_source {
    parameter* param_source;
    declaration* declaration_source;
    containers::deque<function*> function_sources;
    void copy_from(const id_source& _other) {
      param_source = _other.param_source;
      declaration_source = _other.declaration_source, function_sources.clear();
      function_sources.insert(function_sources.begin(),
          _other.function_sources.begin(), _other.function_sources.end());
    }
  };

  void set_id_source(const id_source& _source) {
    WN_RELEASE_ASSERT_DESC(
        ((_source.param_source == 0) + (_source.declaration_source == 0) +
            (!_source.function_sources.empty())) == 1,
        "The source must have come from somewhere.");
    m_source.copy_from(_source);
  };

  const id_source& get_id_source() const {
    return m_source;
  }

  memory::unique_ptr<expression> transfer_to_new() override {
    memory::unique_ptr<id_expression> expr =
        memory::make_unique<id_expression>(m_allocator, m_allocator);
    expr->copy_location_from(this);
    expr->set_type(core::move(m_type));
    expr->m_is_dead = m_is_dead;
    expr->m_is_temporary = m_is_temporary;
    expr->m_source.copy_from(m_source);
    expr->m_name = core::move(m_name);
    return core::move(expr);
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<id_expression> t =
        memory::make_unique<id_expression>(m_allocator, m_allocator);
    t->copy_expression(this);

    if (((m_source.param_source == 0) + (m_source.declaration_source == 0) +
            (!m_source.function_sources.empty())) == 1) {
      t->set_id_source(m_source);
    }

    t->m_name = containers::string(m_allocator, m_name);

    return core::move(t);
  }

  virtual void walk_children(
      const walk_mutable_expression&, const walk_ftype<type*>& _type) {
    if (m_type) {
      _type(m_type.get());
    }
  }
  virtual void walk_children(const walk_ftype<const expression*>&,
      const walk_ftype<const type*>& _type) const {
    if (m_type) {
      _type(m_type.get());
    }
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Id Expression");
    c->print_value(m_type, "Type");
    c->print_value(m_name, "Value");
  }

private:
  id_source m_source;
  containers::string m_name;
};

class null_allocation_expression : public expression {
public:
  null_allocation_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::null_allocation_expression) {}
  memory::unique_ptr<expression> transfer_to_new() override {
    memory::unique_ptr<null_allocation_expression> expr =
        memory::make_unique<null_allocation_expression>(
            m_allocator, m_allocator);
    expr->copy_location_from(this);
    expr->set_type(core::move(m_type));
    expr->m_is_dead = m_is_dead;
    expr->m_is_temporary = m_is_temporary;
    return core::move(expr);
  }
  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<null_allocation_expression> t =
        memory::make_unique<null_allocation_expression>(
            m_allocator, m_allocator);
    t->copy_expression(this);
    return core::move(t);
  }
  void print_node_internal(print_context* c) const override {
    c->print_header("Null Allocation");
    c->print_value(m_type, "Type");
  }

private:
};

class post_expression : public expression {
public:
  post_expression(memory::allocator* _allocator, node_type _type)
    : expression(_allocator, _type) {}
  void add_base_expression(expression* _expr) {
    m_base_expression = memory::unique_ptr<expression>(m_allocator, _expr);
  }
  void add_base_expression(memory::unique_ptr<expression>&& _expr) {
    m_base_expression = core::move(_expr);
  }

  const expression* get_base_expression() const {
    return m_base_expression.get();
  }

  expression* get_base_expression() {
    return m_base_expression.get();
  }

  void walk_children(
      const walk_mutable_expression& expr, const walk_ftype<type*>&) override {
    handle_expression(expr, m_base_expression);
  }
  void walk_children(const walk_ftype<const expression*>& expr,
      const walk_ftype<const type*>&) const override {
    expr(m_base_expression.get());
  }

protected:
  void copy_post_expression(const post_expression* _other) {
    copy_expression(_other);
    m_base_expression = clone_node(_other->m_base_expression);
  }
  memory::unique_ptr<expression> m_base_expression;
};

class array_access_expression : public post_expression {
public:
  array_access_expression(memory::allocator* _allocator, expression* _expr)
    : post_expression(_allocator, node_type::array_access_expression),
      m_array_access(memory::unique_ptr<expression>(m_allocator, _expr)),
      m_is_construction(false) {}

  array_access_expression(
      memory::allocator* _allocator, memory::unique_ptr<expression> _expr)
    : post_expression(_allocator, node_type::array_access_expression),
      m_array_access(core::move(_expr)),
      m_is_construction(false) {}

  explicit array_access_expression(memory::allocator* _allocator)
    : post_expression(_allocator, node_type::array_access_expression) {}

  memory::unique_ptr<expression> transfer_to_new() override {
    memory::unique_ptr<array_access_expression> expr =
        memory::make_unique<array_access_expression>(m_allocator, m_allocator);
    expr->copy_location_from(this);
    expr->set_type(core::move(m_type));
    expr->m_is_dead = m_is_dead;
    expr->m_is_temporary = m_is_temporary;
    expr->m_base_expression = core::move(m_base_expression);
    expr->m_array_access = core::move(m_array_access);
    expr->m_is_construction = m_is_construction;
    return core::move(expr);
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<array_access_expression> t =
        memory::make_unique<array_access_expression>(m_allocator, m_allocator);
    t->copy_post_expression(this);
    t->m_array_access = clone_node(m_array_access);
    t->m_is_construction = m_is_construction;
    return core::move(t);
  }

  virtual void walk_children(
      const walk_mutable_expression& expr, const walk_ftype<type*>&) override {
    handle_expression(expr, m_base_expression);
    handle_expression(expr, m_array_access);
  }

  virtual void walk_children(const walk_ftype<const expression*>& expr,
      const walk_ftype<const type*>&) const override {
    expr(m_base_expression.get());
    expr(m_array_access.get());
  }

  expression* get_access() const {
    return m_array_access.get();
  }

  bool is_construction() const {
    return m_is_construction;
  }
  void set_construction() {
    m_is_construction = true;
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Array Access");
    c->print_value(m_type, "Type");
    c->print_value(m_base_expression, "Base Expression");
    c->print_value(m_array_access, "Expression");
  }

private:
  memory::unique_ptr<expression> m_array_access;
  bool m_is_construction;
};

class short_circuit_expression : public expression {
public:
  short_circuit_expression(memory::allocator* _allocator,
      short_circuit_type _type, expression* _lhs, expression* _rhs)
    : expression(_allocator, node_type::short_circuit_expression),
      m_ss_type(_type),
      m_lhs(memory::unique_ptr<expression>(m_allocator, _lhs)),
      m_rhs(memory::unique_ptr<expression>(m_allocator, _rhs)) {}

  short_circuit_expression(memory::allocator* _allocator,
      short_circuit_type _type, memory::unique_ptr<expression>&& _lhs,
      memory::unique_ptr<expression>&& _rhs)
    : expression(_allocator, node_type::short_circuit_expression),
      m_ss_type(_type),
      m_lhs(core::move(_lhs)),
      m_rhs(core::move(_rhs)) {}

  explicit short_circuit_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::short_circuit_expression) {}

  const expression* get_lhs() const {
    return m_lhs.get();
  }

  const expression* get_rhs() const {
    return m_rhs.get();
  }

  virtual void walk_children(
      const walk_mutable_expression& _func, const walk_ftype<type*>&) override {
    handle_expression(_func, m_lhs);
    handle_expression(_func, m_rhs);
  }

  virtual void walk_children(const walk_ftype<const expression*>& _func,
      const walk_ftype<const type*>&) const override {
    _func(m_lhs.get());
    _func(m_rhs.get());
  }

  short_circuit_type get_ss_type() const {
    return m_ss_type;
  }

  memory::unique_ptr<expression> transfer_to_new() override {
    memory::unique_ptr<short_circuit_expression> expr =
        memory::make_unique<short_circuit_expression>(m_allocator, m_allocator);
    expr->copy_location_from(this);
    expr->set_type(core::move(m_type));
    expr->m_is_dead = m_is_dead;
    expr->m_is_temporary = m_is_temporary;
    expr->m_ss_type = m_ss_type;
    expr->m_lhs = core::move(m_lhs);
    expr->m_rhs = core::move(m_rhs);
    return core::move(expr);
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<short_circuit_expression> t =
        memory::make_unique<short_circuit_expression>(m_allocator, m_allocator);
    t->copy_expression(this);
    t->m_ss_type = m_ss_type;
    t->m_lhs = clone_node(m_lhs);
    t->m_rhs = clone_node(m_rhs);
    return core::move(t);
  }
  void print_node_internal(print_context* c) const override {
    c->print_header("Short Circuit");
    c->print_value(m_type, "Type");
    c->print_value(m_ss_type, "Short-Circuit Type");
    c->print_value(m_lhs, "LHS");
    c->print_value(m_rhs, "RHS");
  }

private:
  short_circuit_type m_ss_type;
  memory::unique_ptr<expression> m_lhs;
  memory::unique_ptr<expression> m_rhs;
};

class member_access_expression : public post_expression {
public:
  member_access_expression(memory::allocator* _allocator, const char* _member)
    : post_expression(_allocator, node_type::member_access_expression),
      m_member(_allocator, _member) {}

  member_access_expression(
      memory::allocator* _allocator, containers::string_view _member)
    : post_expression(_allocator, node_type::member_access_expression),
      m_member(_member.to_string(_allocator)) {}

  member_access_expression(memory::allocator* _allocator)
    : post_expression(_allocator, node_type::member_access_expression) {}

  const containers::string_view get_name() const {
    return m_member;
  }
  memory::unique_ptr<expression> transfer_to_new() override {
    memory::unique_ptr<member_access_expression> expr =
        memory::make_unique<member_access_expression>(m_allocator, m_allocator);
    expr->copy_location_from(this);
    expr->set_type(core::move(m_type));
    expr->m_is_dead = m_is_dead;
    expr->m_is_temporary = m_is_temporary;
    expr->m_base_expression = core::move(m_base_expression);
    expr->m_member = core::move(m_member);
    return core::move(expr);
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<member_access_expression> t =
        memory::make_unique<member_access_expression>(m_allocator, m_allocator);
    t->copy_post_expression(this);
    t->m_member = containers::string(m_allocator, m_member);
    return core::move(t);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Member Access");
    c->print_value(m_type, "Type");
    c->print_value(m_member, "Value");
    c->print_value(m_base_expression, "Base Expression");
  }

private:
  containers::string m_member;
};

class post_unary_expression : public post_expression {
public:
  post_unary_expression(memory::allocator* _allocator, post_unary_type _type)
    : post_expression(_allocator, node_type::post_unary_expression),
      m_unary_type(_type) {}
  explicit post_unary_expression(memory::allocator* _allocator)
    : post_expression(_allocator, node_type::post_unary_expression) {}

  memory::unique_ptr<expression> transfer_to_new() override {
    memory::unique_ptr<post_unary_expression> expr =
        memory::make_unique<post_unary_expression>(m_allocator, m_allocator);
    expr->copy_location_from(this);
    expr->set_type(core::move(m_type));
    expr->m_is_dead = m_is_dead;
    expr->m_is_temporary = m_is_temporary;
    expr->m_base_expression = core::move(m_base_expression);
    expr->m_unary_type = core::move(m_unary_type);
    return core::move(expr);
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<post_unary_expression> t =
        memory::make_unique<post_unary_expression>(m_allocator, m_allocator);
    t->copy_post_expression(this);
    t->m_unary_type = m_unary_type;
    return core::move(t);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Post Unary Expression");
    c->print_value(m_type, "Type");
    c->print_value(m_unary_type, "Unary Type");
    c->print_value(m_base_expression, "Base Expression");
  }

private:
  post_unary_type m_unary_type;
};

class cast_expression : public expression {
public:
  cast_expression(memory::allocator* _allocator, expression* _expression)
    : expression(_allocator, node_type::cast_expression),
      m_expression(memory::unique_ptr<expression>(m_allocator, _expression)) {}

  cast_expression(memory::allocator* _allocator,
      memory::unique_ptr<expression>&& _expression)
    : expression(_allocator, node_type::cast_expression),
      m_expression(core::move(_expression)) {}

  explicit cast_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::cast_expression) {}

  const expression* get_expression() const {
    return m_expression.get();
  }
  expression* get_expression() {
    return m_expression.get();
  }
  void set_expression(memory::unique_ptr<expression> expr) {
    m_expression = core::move(expr);
  }
  virtual void walk_children(const walk_mutable_expression& _expr,
      const walk_ftype<type*>& _type) override {
    handle_expression(_expr, m_expression);
    _type(m_type.get());
  }

  virtual void walk_children(const walk_ftype<const expression*>& _expr,
      const walk_ftype<const type*>& _type) const override {
    _expr(m_expression.get());
    _type(m_type.get());
  }

  memory::unique_ptr<expression> transfer_to_new() override {
    memory::unique_ptr<cast_expression> expr =
        memory::make_unique<cast_expression>(m_allocator, m_allocator);
    expr->copy_location_from(this);
    expr->set_type(core::move(m_type));
    expr->m_is_dead = m_is_dead;
    expr->m_is_temporary = m_is_temporary;
    expr->m_expression = core::move(m_expression);
    return core::move(expr);
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<cast_expression> t =
        memory::make_unique<cast_expression>(m_allocator, m_allocator);
    t->copy_expression(this);
    t->m_expression = clone_node(m_expression);
    return core::move(t);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Cast");
    c->print_value(m_type, "Type");
    c->print_value(m_expression, "Expression");
  }

private:
  memory::unique_ptr<expression> m_expression;
};

class sizeof_expression : public expression {
public:
  sizeof_expression(
      memory::allocator* _allocator, memory::unique_ptr<type>&& _type)
    : expression(_allocator, node_type::sizeof_expression),
      m_sized_type(core::move(_type)) {}
  explicit sizeof_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::sizeof_expression) {}

  const type* get_sized_type() const {
    return m_sized_type.get();
  }

  type* get_sized_type() {
    return m_sized_type.get();
  }

  virtual void walk_children(
      const walk_mutable_expression&, const walk_ftype<type*>& _type) override {
    _type(m_type.get());
    _type(m_sized_type.get());
  }

  virtual void walk_children(const walk_ftype<const expression*>&,
      const walk_ftype<const type*>& _type) const override {
    _type(m_type.get());
    _type(m_sized_type.get());
  }

  memory::unique_ptr<expression> transfer_to_new() override {
    memory::unique_ptr<sizeof_expression> expr =
        memory::make_unique<sizeof_expression>(m_allocator, m_allocator);
    expr->copy_location_from(this);
    expr->set_type(core::move(m_type));
    expr->m_is_dead = m_is_dead;
    expr->m_is_temporary = m_is_temporary;
    expr->m_sized_type = core::move(m_sized_type);
    return core::move(expr);
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<sizeof_expression> t =
        memory::make_unique<sizeof_expression>(m_allocator, m_allocator);
    t->copy_expression(this);
    t->m_sized_type = clone_node(m_sized_type);
    return core::move(t);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Size Of");
    c->print_value(m_type, "Type");
    c->print_value(m_sized_type, "Sized Type");
  }

private:
  memory::unique_ptr<type> m_sized_type;
};

class struct_allocation_expression : public expression {
public:
  explicit struct_allocation_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::struct_allocation_expression),
      m_copy_initializer(nullptr),
      m_init_mode(struct_initialization_mode::invalid) {}

  void set_copy_initializer(expression* _expression) {
    m_copy_initializer =
        memory::unique_ptr<expression>(m_allocator, _expression);
  }

  void set_initialization_mode(struct_initialization_mode _init) {
    m_init_mode = _init;
  }
  struct_initialization_mode set_initialization_mode() const {
    return m_init_mode;
  }
  void set_copy_initializer(memory::unique_ptr<expression>&& _expression) {
    m_copy_initializer = core::move(_expression);
  }

  memory::unique_ptr<expression> transfer_copy_initializer() {
    return core::move(m_copy_initializer);
  }

  expression* data_source() {
    return m_data_source.get();
  }

  memory::unique_ptr<expression> transfer_out_data_source() {
    return core::move(m_data_source);
  }

  void set_data_source(memory::unique_ptr<expression> source) {
    m_data_source = core::move(source);
  }

  virtual void walk_children(
      const walk_mutable_expression&, const walk_ftype<type*>& _type) override {
    _type(m_type.get());
  }

  virtual void walk_children(const walk_ftype<const expression*>&,
      const walk_ftype<const type*>& _type) const override {
    _type(m_type.get());
  }

  memory::unique_ptr<expression> transfer_to_new() override {
    memory::unique_ptr<struct_allocation_expression> expr =
        memory::make_unique<struct_allocation_expression>(
            m_allocator, m_allocator);
    expr->copy_location_from(this);
    expr->set_type(core::move(m_type));
    expr->m_is_dead = m_is_dead;
    expr->m_is_temporary = m_is_temporary;
    expr->m_copy_initializer = core::move(m_copy_initializer);
    expr->m_data_source = core::move(m_data_source);
    expr->m_init_mode = m_init_mode;
    return core::move(expr);
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<struct_allocation_expression> t =
        memory::make_unique<struct_allocation_expression>(
            m_allocator, m_allocator);
    t->copy_expression(this);
    t->m_copy_initializer = clone_node(m_copy_initializer);
    t->m_data_source = clone_node(m_data_source);
    t->m_init_mode = m_init_mode;
    return core::move(t);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Struct Allocation");
    c->print_value(m_type, "Type");
    c->print_value(m_init_mode, "Init Mode");
    c->print_value(m_copy_initializer, "Copy Initializer");
    c->print_value(m_data_source, "Data Source");
  }

private:
  memory::unique_ptr<expression> m_copy_initializer;
  memory::unique_ptr<expression> m_data_source;
  struct_initialization_mode m_init_mode;
};

class unary_expression : public expression {
public:
  unary_expression(
      memory::allocator* _allocator, unary_type _type, expression* _expr)
    : expression(_allocator, node_type::unary_expression),
      m_unary_type(_type),
      m_root_expression(memory::unique_ptr<expression>(m_allocator, _expr)) {}

  explicit unary_expression(memory::allocator* _allocator)
    : expression(_allocator, node_type::unary_expression) {}

  memory::unique_ptr<expression> transfer_to_new() override {
    memory::unique_ptr<unary_expression> expr =
        memory::make_unique<unary_expression>(m_allocator, m_allocator);
    expr->copy_location_from(this);
    expr->set_type(core::move(m_type));
    expr->m_is_dead = m_is_dead;
    expr->m_is_temporary = m_is_temporary;
    expr->m_root_expression = core::move(m_root_expression);
    expr->m_unary_type = m_unary_type;
    return core::move(expr);
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<unary_expression> t =
        memory::make_unique<unary_expression>(m_allocator, m_allocator);
    t->copy_expression(this);
    t->m_root_expression = clone_node(m_root_expression);
    t->m_unary_type = m_unary_type;
    return core::move(t);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Unary Expression");
    c->print_value(m_type, "Type");
    c->print_value(m_unary_type, "Unary Type");
    c->print_value(m_root_expression, "Root Expression");
  }

private:
  unary_type m_unary_type;
  memory::unique_ptr<expression> m_root_expression;
};

class instruction_list;
class instruction : public node {
public:
  instruction(memory::allocator* _allocator, node_type _type)
    : node(_allocator, _type),
      m_returns(false),
      m_is_dead(false),
      m_breaks(false),
      m_non_linear(false),
      m_temporaries(_allocator),
      m_temporary_declarations(_allocator) {}
  instruction(memory::allocator* _allocator, node_type _type, bool _returns,
      bool _is_non_linear = false, bool _breaks = false)
    : instruction(_allocator, _type) {
    m_returns = _returns;
    m_non_linear = _is_non_linear;
    m_breaks = _breaks;
  }

  // Returns true if this instruction causes the function to return.
  bool returns() const {
    return (m_returns);
  }

  // Returns true if this instruction involves non-linear control flow.
  bool is_non_linear() const {
    return m_returns || m_non_linear;
  }

  bool breaks() const {
    return m_breaks;
  }

  virtual void walk_children(const walk_mutable_instruction&,
      const walk_mutable_expression&, const walk_ftype<type*>&,
      const walk_ftype<instruction_list*>&, const walk_scope&,
      const walk_scope&) {
    WN_RELEASE_ASSERT_DESC(false, "Not Implemented");
  }

  virtual void walk_children(const walk_ftype<const instruction*>&,
      const walk_ftype<const expression*>&, const walk_ftype<const type*>&,
      const walk_ftype<const instruction_list*>&, const walk_scope&,
      const walk_scope&) const {
    WN_RELEASE_ASSERT_DESC(false, "Not Implemented");
  }

  void set_is_dead() {
    m_is_dead = true;
  }
  void set_returns(bool returns) {
    m_returns = returns;
  }
  void set_breaks(bool breaks) {
    m_breaks = breaks;
  }
  void set_is_non_linear(bool _is_non_linear) {
    m_non_linear = _is_non_linear;
  }
  bool is_dead() const {
    return m_is_dead;
  }

  void add_temporary(const struct_allocation_expression* _temp) {
    m_temporaries.push_back(_temp);
  }

  void set_temp_declarations(
      containers::deque<memory::unique_ptr<declaration>>&& declarations) {
    m_temporary_declarations = core::move(declarations);
  }

  containers::deque<memory::unique_ptr<declaration>>
  release_temp_declarations() {
    return core::move(m_temporary_declarations);
  }

protected:
  void copy_instruction(const instruction* _other) {
    copy_node(_other);
    m_temporaries.insert(m_temporaries.end(), _other->m_temporaries.begin(),
        _other->m_temporaries.end());
    for (const auto& temp : _other->m_temporary_declarations) {
      m_temporary_declarations.emplace_back(clone_node(temp));
    }
    m_is_dead = _other->m_is_dead;
    m_breaks = _other->m_breaks;
    m_returns = _other->m_returns;
    m_non_linear = _other->m_non_linear;
  }
  containers::deque<const struct_allocation_expression*> m_temporaries;
  containers::deque<memory::unique_ptr<declaration>> m_temporary_declarations;
  bool m_breaks;
  bool m_is_dead;
  bool m_returns;
  bool m_non_linear;
};

struct expression_instruction : public instruction {
  expression_instruction(memory::allocator* _allocator, expression* _expr)
    : instruction(_allocator, node_type::expression_instruction),
      m_expression(memory::unique_ptr<expression>(m_allocator, _expr)) {}

  expression_instruction(
      memory::allocator* _allocator, memory::unique_ptr<expression>&& _expr)
    : instruction(_allocator, node_type::expression_instruction),
      m_expression(core::move(_expr)) {}

  expression_instruction(memory::allocator* _allocator)
    : instruction(_allocator, node_type::expression_instruction) {}

  virtual void walk_children(const walk_mutable_instruction&,
      const walk_mutable_expression& _cond, const walk_ftype<type*>&,
      const walk_ftype<instruction_list*>&, const walk_scope&,
      const walk_scope&) override {
    handle_expression(_cond, m_expression);
  }

  virtual void walk_children(const walk_ftype<const instruction*>&,
      const walk_ftype<const expression*>& _cond,
      const walk_ftype<const type*>&,
      const walk_ftype<const instruction_list*>&, const walk_scope&,
      const walk_scope&) const override {
    _cond(m_expression.get());
  }

  const expression* get_expression() const {
    return m_expression.get();
  }

  expression* get_expression() {
    return m_expression.get();
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<expression_instruction> t =
        memory::make_unique<expression_instruction>(m_allocator, m_allocator);
    t->copy_instruction(this);
    t->m_expression = clone_node(m_expression);
    return core::move(t);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Expression Instruction");
    c->print_value(m_expression, "Expression");
  }

private:
  memory::unique_ptr<expression> m_expression;
};

class instruction_list : public instruction {
public:
  instruction_list(memory::allocator* _allocator)
    : instruction(_allocator, node_type::instruction_list),
      m_instructions(_allocator) {}
  instruction_list(memory::allocator* _allocator, instruction* inst)
    : instruction(_allocator, node_type::instruction_list),
      m_instructions(_allocator) {
    m_instructions.push_back(
        memory::unique_ptr<instruction>(m_allocator, inst));
  }
  ~instruction_list() {}
  void add_instruction(instruction* inst) {
    m_instructions.push_back(
        memory::unique_ptr<instruction>(m_allocator, inst));
  }

  void add_instruction(memory::unique_ptr<instruction>&& inst) {
    m_instructions.push_back(core::move(inst));
  }

  void prepend_instruction(memory::unique_ptr<instruction>&& inst) {
    m_instructions.push_front(core::move(inst));
  }

  void pop_back() {
    m_instructions.pop_back();
  }

  const containers::deque<memory::unique_ptr<instruction>>& get_instructions()
      const {
    return m_instructions;
  }

  containers::deque<memory::unique_ptr<instruction>>& get_instructions() {
    return m_instructions;
  }

  void set_instructions(
      containers::deque<memory::unique_ptr<instruction>>&& instructions) {
    m_instructions.take_over(core::move(instructions));
  }

  void walk_children(const walk_mutable_instruction& _instruction,
      const walk_mutable_expression&, const walk_ftype<type*>&,
      const walk_ftype<instruction_list*>&, const walk_scope& _enter_scope,
      const walk_scope& _leave_scope) override {
    _enter_scope(this);
    for (auto& inst : m_instructions) {
      handle_instruction(_instruction, inst);
    }
    _leave_scope(this);
  }

  void walk_children(const walk_ftype<const instruction*>& _instruction,
      const walk_ftype<const expression*>&, const walk_ftype<const type*>&,
      const walk_ftype<const instruction_list*>&,
      const walk_scope& _enter_scope,
      const walk_scope& _leave_scope) const override {
    _enter_scope(this);
    for (auto& inst : m_instructions) {
      _instruction(inst.get());
    }
    _leave_scope(this);
  }

  void remove_dead_instructions() {
    containers::deque<memory::unique_ptr<instruction>> instructions(
        m_allocator);
    while (!m_instructions.empty()) {
      if (!m_instructions.front()->is_dead()) {
        instructions.push_back(core::move(*m_instructions.begin()));
      }
      m_instructions.pop_front();
    }
    m_instructions = core::move(instructions);
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<instruction_list> t =
        memory::make_unique<instruction_list>(m_allocator, m_allocator);
    t->copy_instruction(this);
    for (const auto& inst : m_instructions) {
      t->m_instructions.push_back(clone_node(inst));
    }
    return core::move(t);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("InstructionList<", m_instructions.size(), ">");
    c->print_value(m_instructions, "Instructions");
  }

private:
  containers::deque<memory::unique_ptr<instruction>> m_instructions;
};

struct function_expression {
  function_expression(
      memory::allocator* _allocator, expression* _expr, bool _hand_ownership)
    : m_expr(_allocator, _expr), m_hand_ownership(_hand_ownership) {}
  function_expression(
      memory::unique_ptr<expression>&& _expr, bool _hand_ownership)
    : m_expr(core::move(_expr)), m_hand_ownership(_hand_ownership) {}

  memory::unique_ptr<expression> m_expr;
  bool m_hand_ownership;
};

class arg_list : public node {
public:
  arg_list(memory::allocator* _allocator)
    : node(_allocator, node_type::arg_list), m_expression_list(_allocator) {}
  void add_expression(expression* _expr, bool _hand_ownership = false) {
    m_expression_list.emplace_back(memory::make_unique<function_expression>(
        m_allocator, m_allocator, _expr, _hand_ownership));
  }

  void add_expression(
      memory::unique_ptr<expression> _expr, bool _hand_ownership = false) {
    m_expression_list.emplace_back(memory::make_unique<function_expression>(
        m_allocator, core::move(_expr), _hand_ownership));
  }

  containers::deque<memory::unique_ptr<function_expression>>&
  get_expressions() {
    return (m_expression_list);
  }

  const containers::deque<memory::unique_ptr<function_expression>>&
  get_expressions() const {
    return (m_expression_list);
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<arg_list> t =
        memory::make_unique<arg_list>(m_allocator, m_allocator);
    t->copy_node(this);
    for (const auto& expr : m_expression_list) {
      t->m_expression_list.push_back(memory::make_unique<function_expression>(
          m_allocator, clone_node(expr->m_expr), expr->m_hand_ownership));
    }
    return core::move(t);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Args");
    c->enter_log_scope();
    for (auto& e : m_expression_list) {
      e->m_expr->print_node_internal(c);
    }
    c->leave_log_scope();
  }

private:
  containers::deque<memory::unique_ptr<function_expression>> m_expression_list;
};

class function_call_expression : public post_expression {
public:
  function_call_expression(memory::allocator* _allocator)
    : post_expression(_allocator, node_type::function_call_expression) {}
  function_call_expression(memory::allocator* _allocator, arg_list* _list)
    : post_expression(_allocator, node_type::function_call_expression),
      m_args(_allocator, _list) {}

  function_call_expression(
      memory::allocator* _allocator, memory::unique_ptr<arg_list>&& _list)
    : post_expression(_allocator, node_type::function_call_expression),
      m_args(core::move(_list)) {}

  virtual void walk_children(
      const walk_mutable_expression& _func, const walk_ftype<type*>&) override {
    if (!m_args) {
      return;
    }
    for (auto& a : m_args->get_expressions()) {
      handle_expression(_func, a->m_expr);
    }
  }

  virtual void walk_children(const walk_ftype<const expression*>& _func,
      const walk_ftype<const type*>&) const override {
    if (!m_args) {
      return;
    }
    for (auto& a : m_args->get_expressions()) {
      _func(a->m_expr.get());
    }
  }

  void set_callee(function* _callee) {
    m_callee = _callee;
  }

  function* callee() {
    return m_callee;
  }

  const function* callee() const {
    return m_callee;
  }

  const containers::deque<memory::unique_ptr<function_expression>>&
  get_expressions() const {
    if (m_args) {
      return m_args->get_expressions();
    }
    static const containers::deque<memory::unique_ptr<function_expression>>
        empty_expressions;
    return empty_expressions;
  }

  containers::deque<memory::unique_ptr<function_expression>>&
  get_expressions() {
    if (m_args) {
      return m_args->get_expressions();
    }
    static containers::deque<memory::unique_ptr<function_expression>>
        empty_expressions;
    return empty_expressions;
  }

  memory::unique_ptr<arg_list> copy_out_args() {
    return core::move(m_args);
  }

  const arg_list* get_args() const {
    return m_args.get();
  }

  memory::unique_ptr<expression> transfer_to_new() override {
    memory::unique_ptr<function_call_expression> alloc =
        memory::make_unique<function_call_expression>(m_allocator, m_allocator);
    alloc->copy_location_from(this);
    alloc->set_type(core::move(m_type));
    alloc->m_is_dead = m_is_dead;
    alloc->m_is_temporary = m_is_temporary;
    alloc->m_callee = m_callee;
    alloc->m_args = core::move(m_args);
    return core::move(alloc);
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<function_call_expression> t =
        memory::make_unique<function_call_expression>(m_allocator, m_allocator);
    t->copy_post_expression(this);
    t->m_callee = m_callee;
    t->m_args = clone_node(m_args);
    return core::move(t);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Function Call");
    c->print_value(m_type, "Type");
    c->print_value(m_base_expression, "BaseExpression");
    c->print_value(m_args, "Args");
  }

private:
  function* m_callee;
  memory::unique_ptr<arg_list> m_args;
};

class parameter : public node {
public:
  parameter(memory::allocator* _allocator, type* _type, const char* _name)
    : node(_allocator, scripting::node_type::parameter),
      m_type(memory::unique_ptr<type>(_allocator, _type)),
      m_name(_allocator, _name) {}

  parameter(memory::allocator* _allocator, memory::unique_ptr<type>&& _type,
      containers::string_view _name)
    : node(_allocator, scripting::node_type::parameter),
      m_type(core::move(_type)),
      m_name(_name.to_string(_allocator)) {}

  explicit parameter(memory::allocator* _allocator)
    : node(_allocator, scripting::node_type::parameter) {}

  const containers::string_view get_name() const {
    return m_name.to_string_view();
  }

  void set_name(const containers::string_view& view) {
    m_name = view.to_string(m_allocator);
  }
  void set_type(memory::unique_ptr<type>&& _new_type) {
    m_type = core::move(_new_type);
  }

  const type* get_type() const {
    return m_type.get();
  }
  type* get_type() {
    return m_type.get();
  }

  virtual void walk_children(const walk_ftype<type*>& _type_func) {
    _type_func(m_type.get());
  }
  virtual void walk_children(const walk_ftype<const type*>& _type_func) const {
    _type_func(m_type.get());
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<parameter> t =
        memory::make_unique<parameter>(m_allocator, m_allocator);
    t->copy_node(this);
    t->m_name = containers::string(m_allocator, m_name);
    t->m_type = clone_node(m_type);
    return core::move(t);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header(m_name);
    c->print_value(m_type, "Type");
  }

private:
  friend class declaration;
  containers::string m_name;
  memory::unique_ptr<type> m_type;
};

class declaration : public instruction {
public:
  declaration(memory::allocator* _allocator)
    : instruction(_allocator, node_type::declaration) {}
  void set_parameter(parameter* _parameter) {
    m_parameter = memory::unique_ptr<parameter>(m_allocator, _parameter);
  }

  void set_parameter(memory::unique_ptr<parameter>&& _parameter) {
    m_parameter = core::move(_parameter);
  }

  void add_expression_initializer(expression* _expr) {
    m_expression = memory::unique_ptr<expression>(m_allocator, _expr);
  }

  void set_type(memory::unique_ptr<type>&& _new_type) {
    m_parameter->set_type(core::move(_new_type));
  }

  expression* get_expression() {
    return m_expression.get();
  }

  const expression* get_expression() const {
    return m_expression.get();
  }

  memory::unique_ptr<expression> take_expression() {
    return core::move(m_expression);
  }

  void set_expression(memory::unique_ptr<expression>&& _expr) {
    m_expression = core::move(_expr);
  }

  void set_destructor(containers::string_view view) {
    m_destructor_name = view.to_string(m_allocator);
  }

  containers::string_view get_destructor_name() const {
    return m_destructor_name;
  }

  const type* get_type() const {
    return m_parameter->get_type();
  }
  type* get_type() {
    return m_parameter->get_type();
  }
  const containers::string_view get_name() const {
    return m_parameter->get_name();
  }

  virtual void walk_children(const walk_mutable_instruction&,
      const walk_mutable_expression& expr, const walk_ftype<type*>& type,
      const walk_ftype<instruction_list*>&, const walk_scope&,
      const walk_scope&) override {
    if (type) {
      type(m_parameter->m_type.get());
    }
    if (m_expression) {
      handle_expression(expr, m_expression);
    }
  }

  virtual void walk_children(const walk_ftype<const instruction*>&,
      const walk_ftype<const expression*>& expr,
      const walk_ftype<const type*>& type,
      const walk_ftype<const instruction_list*>&, const walk_scope&,
      const walk_scope&) const override {
    if (type) {
      type(m_parameter->m_type.get());
    }
    if (m_expression) {
      expr(m_expression.get());
    }
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<declaration> t =
        memory::make_unique<declaration>(m_allocator, m_allocator);
    t->copy_instruction(this);
    t->m_parameter = clone_node(m_parameter);
    t->m_expression = clone_node(m_expression);
    return core::move(t);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Declaration:");
    c->print_value(m_parameter, "Parameter");
    c->print_value(m_expression, "Expression");
  }

private:
  memory::unique_ptr<parameter> m_parameter;
  memory::unique_ptr<expression> m_expression;
  containers::string m_destructor_name;
};

class function;
class struct_definition : public node {
public:
  struct_definition(memory::allocator* _allocator, const char* _name,
      bool _is_class = false, const char* _parent_type = nullptr)
    : node(_allocator, node_type::struct_definition),
      m_name(_allocator, _name),
      m_parent_name(_allocator),
      m_is_class(_is_class),
      m_type_index(0),
      m_struct_members(_allocator),
      m_struct_functions(_allocator) {
    if (_parent_type) {
      m_parent_name = _parent_type;
    }
  }

  struct_definition(memory::allocator* _allocator)
    : node(_allocator, node_type::struct_definition),
      m_struct_members(_allocator),
      m_struct_functions(_allocator) {}

  void add_struct_elem(declaration* _decl) {
    m_struct_members.emplace_back(
        memory::unique_ptr<declaration>(m_allocator, _decl));
  }

  void add_function(function* _func) {
    m_struct_functions.emplace_back(
        memory::unique_ptr<function>(m_allocator, _func));
  }

  void set_type_index(uint32_t _index) {
    m_type_index = _index;
  }

  uint32_t get_type_index() const {
    return m_type_index;
  }

  containers::string_view get_name() const {
    return m_name;
  }

  containers::deque<memory::unique_ptr<declaration>>& get_struct_members() {
    return m_struct_members;
  }

  const containers::deque<memory::unique_ptr<declaration>>& get_struct_members()
      const {
    return m_struct_members;
  }

  virtual void walk_children(const walk_ftype<type*>& type) {
    // We explicitly do not run through the definition,
    // and assignment have to happen in slightly different places,
    // so this is not a "NORMAL" assignment.

    for (auto& member : m_struct_members) {
      type(member->get_type());
    }
  }

  virtual void walk_children(const walk_ftype<const type*>& type) const {
    for (auto& member : m_struct_members) {
      type(member->get_type());
    }
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<struct_definition> t =
        memory::make_unique<struct_definition>(m_allocator, m_allocator);
    t->copy_node(this);
    t->m_name = containers::string(m_allocator, m_name);
    t->m_parent_name = containers::string(m_allocator, m_parent_name);
    t->m_is_class = m_is_class;
    for (auto& member : m_struct_members) {
      t->m_struct_members.push_back(clone_node(member));
    }
    for (auto& function : m_struct_functions) {
      t->m_struct_functions.push_back(clone_node(function));
    }
    t->m_type_index = m_type_index;
    return core::move(t);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header(
        "Struct [", m_name, "]", "(", m_type_index, ")", ":", m_parent_name);
    c->print_value(m_struct_members, "Struct Members");
    c->print_value(m_struct_functions, "Struct functions");
  }

private:
  containers::string m_name;
  containers::string m_parent_name;
  bool m_is_class;
  uint32_t m_type_index;
  containers::deque<memory::unique_ptr<declaration>> m_struct_members;
  containers::deque<memory::unique_ptr<function>> m_struct_functions;
};

class parameter_list : public node {
public:
  explicit parameter_list(memory::allocator* _allocator)
    : node(_allocator, node_type::parameter_list), m_parameters(_allocator) {}
  parameter_list(memory::allocator* _allocator, parameter* _param)
    : parameter_list(_allocator) {
    m_parameters.emplace_back(
        memory::unique_ptr<parameter>(m_allocator, _param));
  }

  parameter_list(
      memory::allocator* _allocator, memory::unique_ptr<parameter>&& _param)
    : node(_allocator, node_type::parameter_list), m_parameters(_allocator) {
    m_parameters.push_back(core::move(_param));
  }

  void add_parameter(parameter* _param) {
    m_parameters.emplace_back(
        memory::unique_ptr<parameter>(m_allocator, _param));
  }

  void add_parameter(memory::unique_ptr<parameter>&& _param) {
    m_parameters.emplace_back(core::move(_param));
  }

  const containers::deque<memory::unique_ptr<parameter>>& get_parameters()
      const {
    return m_parameters;
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<parameter_list> t =
        memory::make_unique<parameter_list>(m_allocator, m_allocator);
    t->copy_node(this);
    for (auto& param : m_parameters) {
      t->m_parameters.push_back(clone_node(param));
    }
    return core::move(t);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Parameter List");
    c->print_value(m_parameters, "Parameters");
  }

private:
  containers::deque<memory::unique_ptr<parameter>> m_parameters;
};

class function : public node {
public:
  function(memory::allocator* _allocator, parameter* _signature,
      parameter_list* _params, instruction_list* _body)
    : node(_allocator, node_type::function),
      m_signature(memory::unique_ptr<parameter>(m_allocator, _signature)),
      m_parameters(memory::unique_ptr<parameter_list>(m_allocator, _params)),
      m_body(memory::unique_ptr<instruction_list>(m_allocator, _body)),
      m_this_pointer(nullptr),
      m_is_override(false),
      m_is_virtual(false) {}

  explicit function(memory::allocator* _allocator)
    : node(_allocator, node_type::function) {}

  function(memory::allocator* _allocator,
      memory::unique_ptr<parameter>&& _signature,
      memory::unique_ptr<parameter_list>&& _params,
      memory::unique_ptr<instruction_list>&& _body)
    : node(_allocator, node_type::function),
      m_signature(core::move(_signature)),
      m_parameters(core::move(_params)),
      m_body(core::move(_body)),
      m_this_pointer(nullptr),
      m_is_override(false),
      m_is_virtual(false) {}

  void set_is_virtual(bool is_virtual) {
    m_is_virtual = is_virtual;
  }
  void set_is_override(bool is_override) {
    m_is_virtual = is_override;
  }
  const parameter* get_signature() const {
    return m_signature.get();
  }
  parameter* get_signature() {
    return m_signature.get();
  }
  const parameter_list* get_parameters() const {
    return m_parameters.get();
  }
  parameter_list* get_parameters() {
    return m_parameters.get();
  }
  const instruction_list* get_body() const {
    return m_body.get();
  }

  virtual void walk_children(const walk_scope& _enter_scope,
      const walk_scope& _leave_scope, const walk_ftype<parameter*>& _parameter,
      const walk_ftype<instruction_list*>& _instructions,
      const walk_ftype<type*>& _type) {
    m_signature->walk_children(_type);
    _enter_scope(this);
    if (m_body) {
      if (m_parameters) {
        for (auto& param : m_parameters->get_parameters()) {
          _parameter(param.get());
        }
      }

      _instructions(m_body.get());
    }

    _leave_scope(this);
  }

  virtual void walk_children(const walk_scope& _enter_scope,
      const walk_scope& _leave_scope,
      const walk_ftype<const parameter*>& _parameter,
      const walk_ftype<const instruction_list*>& _instructions,
      const walk_ftype<const type*>& _type) const {
    // m_signature->walk_children confuses visual studio.
    const parameter* tmp_param = m_signature.get();
    tmp_param->walk_children(_type);

    _enter_scope(this);
    if (m_body) {
      if (m_parameters) {
        for (auto& param : m_parameters->get_parameters()) {
          _parameter(param.get());
        }
      }

      _instructions(m_body.get());
    }
    _leave_scope(this);
  }

  void set_mangled_name(const containers::string& _name) {
    m_mangled_name = containers::string(m_allocator, _name);
  }

  // This will not be valid until after the type_association pass.
  const containers::string& get_mangled_name() const {
    return m_mangled_name;
  }

  void set_this_pointer(struct_definition* _this_pointer) {
    m_this_pointer = _this_pointer;
  }

  const struct_definition* get_this_pointer() const {
    return m_this_pointer;
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<function> t =
        memory::make_unique<function>(m_allocator, m_allocator);
    t->copy_node(this);
    t->m_signature = clone_node(m_signature);
    t->m_parameters = clone_node(m_parameters);
    t->m_body = clone_node(m_body);
    t->m_this_pointer = m_this_pointer;
    t->m_mangled_name = containers::string(m_allocator, m_mangled_name);
    t->m_is_override = m_is_override;
    t->m_is_virtual = m_is_virtual;
    return core::move(t);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Function");
    c->print_value(m_mangled_name, "Mangled Name");
    if (m_this_pointer) {
      // WE don't use the normal print_value, because this
      // would result in a circular reference.
      c->print_value(static_cast<const void*>(m_this_pointer), "This");
    } else {
      c->print_value("<NULL>", "This");
    }
    c->print_value(m_signature, "Signature");
    c->print_value(m_parameters, "Parameters");
    c->print_value(m_body, "Body");
  }

private:
  memory::unique_ptr<parameter> m_signature;
  memory::unique_ptr<parameter_list> m_parameters;
  memory::unique_ptr<instruction_list> m_body;
  struct_definition* m_this_pointer;
  containers::string m_mangled_name;
  bool m_is_override;
  bool m_is_virtual;
};

class assignment_instruction;
class set_array_length;
class lvalue : public node {
public:
  lvalue(memory::allocator* _allocator, expression* _expr)
    : node(_allocator, node_type::lvalue),
      m_expression(memory::unique_ptr<expression>(_allocator, _expr)) {}

  lvalue(memory::allocator* _allocator, memory::unique_ptr<expression>&& _expr)
    : node(_allocator, node_type::lvalue), m_expression(core::move(_expr)) {}

  explicit lvalue(memory::allocator* _allocator)
    : node(_allocator, node_type::lvalue) {}

  const expression* get_expression() const {
    return m_expression.get();
  }

  void set_expression(memory::unique_ptr<expression> expr) {
    m_expression = core::move(expr);
  }

  memory::unique_ptr<expression> transfer_expression() {
    return core::move(m_expression);
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<lvalue> t =
        memory::make_unique<lvalue>(m_allocator, m_allocator);
    t->copy_node(this);
    t->m_expression = clone_node(m_expression);
    return core::move(t);
  }
  void print_node_internal(print_context* c) const override {
    c->print_header("L-Value");
    c->print_value(m_expression, "Expression");
  }

private:
  friend class assignment_instruction;
  friend class set_array_length;
  memory::unique_ptr<expression> m_expression;
};

class assignment_instruction : public instruction {
public:
  assignment_instruction(memory::allocator* _allocator, lvalue* _lvalue)
    : instruction(_allocator, node_type::assignment_instruction),
      m_lvalue(memory::unique_ptr<lvalue>(m_allocator, _lvalue)),
      m_assign_type(assign_type::max),
      m_in_constructor(false) {}

  explicit assignment_instruction(memory::allocator* _allocator)
    : instruction(_allocator, node_type::assignment_instruction) {}

  assignment_instruction(
      memory::allocator* _allocator, memory::unique_ptr<lvalue>&& _lvalue)
    : instruction(_allocator, node_type::assignment_instruction),
      m_lvalue(core::move(_lvalue)),
      m_assign_type(assign_type::max),
      m_in_constructor(false) {}

  void add_value(assign_type _type, memory::unique_ptr<expression>&& _value) {
    m_assign_type = _type;
    m_assign_expression = core::move(_value);
  }

  void add_value(assign_type _type, expression* _value) {
    m_assign_type = _type;
    m_assign_expression = memory::unique_ptr<expression>(m_allocator, _value);
  }

  bool is_constructor_assigment() const {
    return m_in_constructor;
  }

  void set_in_constructor(bool _in) {
    m_in_constructor = _in;
  }

  assign_type get_assignment_type() const {
    return m_assign_type;
  }

  const expression* get_expression() const {
    return m_assign_expression.get();
  }
  const lvalue* get_lvalue() const {
    return m_lvalue.get();
  }
  lvalue* get_lvalue() {
    return m_lvalue.get();
  }
  expression* get_expression() {
    return m_assign_expression.get();
  }

  memory::unique_ptr<lvalue> transfer_lvalue() {
    return core::move(m_lvalue);
  }

  void set_lvalue(memory::unique_ptr<lvalue> lvalue) {
    m_lvalue = core::move(lvalue);
  }

  memory::unique_ptr<expression> transfer_expression() {
    return core::move(m_assign_expression);
  }

  virtual void walk_children(const walk_mutable_instruction&,
      const walk_mutable_expression& expr, const walk_ftype<type*>&,
      const walk_ftype<instruction_list*>&, const walk_scope&,
      const walk_scope&) override {
    if (expr) {
      handle_expression(expr, m_lvalue->m_expression);
      handle_expression(expr, m_assign_expression);
    }
  }

  virtual void walk_children(const walk_ftype<const instruction*>&,
      const walk_ftype<const expression*>& expr, const walk_ftype<const type*>&,
      const walk_ftype<const instruction_list*>&, const walk_scope&,
      const walk_scope&) const override {
    if (expr) {
      expr(m_lvalue->m_expression.get());
      expr(m_assign_expression.get());
    }
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<assignment_instruction> t =
        memory::make_unique<assignment_instruction>(m_allocator, m_allocator);
    t->copy_instruction(this);
    t->m_lvalue = clone_node(m_lvalue);
    t->m_assign_expression = clone_node(m_assign_expression);
    t->m_assign_type = m_assign_type;
    t->m_in_constructor = m_in_constructor;
    return core::move(t);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Assignment");
    c->print_value(m_assign_type, "Assign Type");
    c->print_value(m_lvalue, "L-Value");
    c->print_value(m_assign_expression, "Expression");
  }

private:
  assign_type m_assign_type;
  memory::unique_ptr<lvalue> m_lvalue;
  memory::unique_ptr<expression> m_assign_expression;
  bool m_in_constructor;
};

class do_instruction;
class break_instruction : public instruction {
public:
  break_instruction(memory::allocator* _allocator)
    : instruction(_allocator, node_type::break_instruction, false, true) {}

  virtual void walk_children(const walk_mutable_instruction&,
      const walk_mutable_expression&, const walk_ftype<type*>&,
      const walk_ftype<instruction_list*>&, const walk_scope&,
      const walk_scope&) override {}

  virtual void walk_children(const walk_ftype<const instruction*>&,
      const walk_ftype<const expression*>&, const walk_ftype<const type*>&,
      const walk_ftype<const instruction_list*>&, const walk_scope&,
      const walk_scope&) const override {}

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<break_instruction> t =
        memory::make_unique<break_instruction>(m_allocator, m_allocator);
    t->copy_instruction(this);
    return core::move(t);
  }

  void set_loop(do_instruction* _inst) {
    m_loop_break = _inst;
  }

  const do_instruction* get_do_instruction() {
    return m_loop_break;
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Break Instruction");
    c->print_value(static_cast<const void*>(m_loop_break), "Break Loop");
  }

private:
  const do_instruction* m_loop_break;
};

class set_array_length : public instruction {
public:
  set_array_length(memory::allocator* _allocator,
      memory::unique_ptr<lvalue> arr, memory::unique_ptr<expression> val)
    : instruction(_allocator, node_type::set_array_length, false, false),
      m_lvalue(core::move(arr)),
      m_expression(core::move(val)) {}
  const expression* get_expression() const {
    return m_expression.get();
  }
  const lvalue* get_lvalue() const {
    return m_lvalue.get();
  }

  virtual memory::unique_ptr<node> clone() const {
    memory::unique_ptr<set_array_length> t =
        memory::make_unique<set_array_length>(m_allocator, m_allocator,
            clone_node(m_lvalue.get()), clone_node(m_expression.get()));
    t->copy_instruction(this);
    return core::move(t);
  }

  virtual void walk_children(const walk_mutable_instruction&,
      const walk_mutable_expression& e, const walk_ftype<type*>&,
      const walk_ftype<instruction_list*>&, const walk_scope&,
      const walk_scope&) override {
    handle_expression(e, m_expression);
    handle_expression(e, m_lvalue->m_expression);
  }

  virtual void walk_children(const walk_ftype<const instruction*>&,
      const walk_ftype<const expression*>& e, const walk_ftype<const type*>&,
      const walk_ftype<const instruction_list*>&, const walk_scope&,
      const walk_scope&) const override {
    e(m_expression.get());
    e(m_lvalue->get_expression());
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Set Array Length");
    c->print_value(m_expression, "Expression");
    c->print_value(m_lvalue, "L-Value");
  }

private:
  memory::unique_ptr<expression> m_expression;
  memory::unique_ptr<lvalue> m_lvalue;
};

class continue_instruction : public instruction {
public:
  continue_instruction(memory::allocator* _allocator)
    : instruction(_allocator, node_type::continue_instruction, false, true) {}

  virtual void walk_children(const walk_mutable_instruction&,
      const walk_mutable_expression&, const walk_ftype<type*>&,
      const walk_ftype<instruction_list*>&, const walk_scope&,
      const walk_scope&) override {}

  virtual void walk_children(const walk_ftype<const instruction*>&,
      const walk_ftype<const expression*>&, const walk_ftype<const type*>&,
      const walk_ftype<const instruction_list*>&, const walk_scope&,
      const walk_scope&) const override {}

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<continue_instruction> t =
        memory::make_unique<continue_instruction>(m_allocator, m_allocator);
    t->copy_instruction(this);
    return core::move(t);
  }

  void set_loop(do_instruction* _inst) {
    m_loop_break = _inst;
  }

  const do_instruction* get_do_instruction() {
    return m_loop_break;
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Continue");
    c->print_value(static_cast<const void*>(m_loop_break), "Continue Loop");
  }

private:
  const do_instruction* m_loop_break;
};

class do_instruction : public instruction {
public:
  do_instruction(
      memory::allocator* _allocator, expression* _cond, instruction_list* _body)
    : instruction(_allocator, node_type::do_instruction),
      m_condition(memory::unique_ptr<expression>(m_allocator, _cond)),
      m_body(memory::unique_ptr<instruction_list>(m_allocator, _body)) {}

  do_instruction(memory::allocator* _allocator)
    : instruction(_allocator, node_type::do_instruction) {}

  void set_condition(memory::unique_ptr<expression>&& _expr) {
    m_condition = core::move(_expr);
  }

  void set_body(memory::unique_ptr<instruction_list>&& _body) {
    m_body = core::move(_body);
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<do_instruction> t =
        memory::make_unique<do_instruction>(m_allocator, m_allocator);
    t->copy_instruction(this);
    t->m_condition = clone_node(m_condition);
    t->m_body = clone_node(m_body);
    return core::move(t);
  }

  virtual void walk_children(const walk_mutable_instruction&,
      const walk_mutable_expression& _expr, const walk_ftype<type*>&,
      const walk_ftype<instruction_list*>& _inst, const walk_scope&,
      const walk_scope&) override {
    if (_expr) {
      handle_expression(_expr, m_condition);
    }
    if (_inst) {
      _inst(m_body.get());
    }
  }

  virtual void walk_children(const walk_ftype<const instruction*>&,
      const walk_ftype<const expression*>& _expr,
      const walk_ftype<const type*>&,
      const walk_ftype<const instruction_list*>& _inst, const walk_scope&,
      const walk_scope&) const override {
    if (_expr) {
      _expr(m_condition.get());
    }
    if (_inst) {
      _inst(m_body.get());
    }
  }

  const expression* get_condition() const {
    return m_condition.get();
  }

  memory::unique_ptr<expression> take_condition() {
    return core::move(m_condition);
  }

  const instruction_list* get_body() const {
    return m_body.get();
  }

  memory::unique_ptr<instruction_list> take_body() {
    return core::move(m_body);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Do Loop");
    c->print_value(m_condition, "Condition");
    c->print_value(m_body, "Body");
  }

private:
  memory::unique_ptr<expression> m_condition;
  memory::unique_ptr<instruction_list> m_body;
};

class else_if_instruction : public instruction {
public:
  else_if_instruction(
      memory::allocator* _allocator, expression* _cond, instruction_list* _body)
    : instruction(_allocator, node_type::else_if_instruction),
      m_condition(memory::unique_ptr<expression>(m_allocator, _cond)),
      m_body(memory::unique_ptr<instruction_list>(m_allocator, _body)) {}

  explicit else_if_instruction(memory::allocator* _allocator)
    : instruction(_allocator, node_type::else_if_instruction) {}

  const expression* get_condition() const {
    return m_condition.get();
  }

  memory::unique_ptr<expression> take_condition() {
    return core::move(m_condition);
  }

  const instruction_list* get_body() const {
    return m_body.get();
  }

  memory::unique_ptr<instruction_list> take_body() {
    return core::move(m_body);
  }

  virtual void walk_children(const walk_mutable_instruction&,
      const walk_mutable_expression& _cond, const walk_ftype<type*>&,
      const walk_ftype<instruction_list*>& _body, const walk_scope&,
      const walk_scope&) override {
    handle_expression(_cond, m_condition);
    _body(m_body.get());
  }
  virtual void walk_children(const walk_ftype<const instruction*>&,
      const walk_ftype<const expression*>& _cond,
      const walk_ftype<const type*>&,
      const walk_ftype<const instruction_list*>& _body, const walk_scope&,
      const walk_scope&) const override {
    _cond(m_condition.get());
    _body(m_body.get());
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<else_if_instruction> t =
        memory::make_unique<else_if_instruction>(m_allocator, m_allocator);
    t->copy_instruction(this);
    t->m_condition = clone_node(m_condition);
    t->m_body = clone_node(m_body);
    return core::move(t);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Else If");
    c->print_value(m_condition, "Condition");
    c->print_value(m_body, "Body");
  }

private:
  memory::unique_ptr<expression> m_condition;
  memory::unique_ptr<instruction_list> m_body;
};

class if_instruction : public instruction {
public:
  if_instruction(
      memory::allocator* _allocator, expression* _cond, instruction_list* _body)
    : instruction(_allocator, node_type::if_instruction),
      m_condition(memory::unique_ptr<expression>(m_allocator, _cond)),
      m_body(memory::unique_ptr<instruction_list>(m_allocator, _body)),
      m_else_if_nodes(_allocator) {}

  if_instruction(memory::allocator* _allocator)
    : instruction(_allocator, node_type::if_instruction),
      m_else_if_nodes(_allocator) {}

  void add_else_if(else_if_instruction* _elseif) {
    m_else_if_nodes.emplace_back(
        memory::unique_ptr<else_if_instruction>(m_allocator, _elseif));
  }
  void add_else(instruction_list* _else) {
    m_else = memory::unique_ptr<instruction_list>(m_allocator, _else);
  }

  void set_body(memory::unique_ptr<instruction_list>&& instructions) {
    m_body = core::move(instructions);
  }

  void clear_else_ifs() {
    m_else_if_nodes.clear();
  }

  void clear_else_node() {
    m_else.reset();
  }

  virtual void walk_children(const walk_mutable_instruction& _inst,
      const walk_mutable_expression& _cond, const walk_ftype<type*>&,
      const walk_ftype<instruction_list*>& _body, const walk_scope&,
      const walk_scope&) override {
    handle_expression(_cond, m_condition);
    _body(m_body.get());
    for (auto& else_inst : m_else_if_nodes) {
      handle_instruction(_inst, else_inst);
    }
    if (m_else) {
      _body(m_else.get());
    }
  }

  virtual void walk_children(const walk_ftype<const instruction*>& _inst,
      const walk_ftype<const expression*>& _cond,
      const walk_ftype<const type*>&,
      const walk_ftype<const instruction_list*>& _body, const walk_scope&,
      const walk_scope&) const override {
    _cond(m_condition.get());
    _body(m_body.get());
    for (auto& else_inst : m_else_if_nodes) {
      _inst(else_inst.get());
    }
    if (m_else) {
      _body(m_else.get());
    }
  }

  const expression* get_condition() const {
    return m_condition.get();
  }

  expression* get_condition() {
    return m_condition.get();
  }

  memory::unique_ptr<expression> release_condition() {
    return core::move(m_condition);
  }

  void set_condition(memory::unique_ptr<expression>&& _expr) {
    m_condition = core::move(_expr);
  }

  const instruction_list* get_body() const {
    return m_body.get();
  }

  instruction_list* get_body() {
    return m_body.get();
  }

  const instruction_list* get_else() const {
    return m_else.get();
  }

  memory::unique_ptr<instruction_list> release_else() {
    return core::move(m_else);
  }

  containers::deque<memory::unique_ptr<instruction>>&
  get_else_if_instructions() {
    return m_else_if_nodes;
  }

  const containers::deque<memory::unique_ptr<instruction>>&
  get_else_if_instructions() const {
    return m_else_if_nodes;
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<if_instruction> t =
        memory::make_unique<if_instruction>(m_allocator, m_allocator);
    t->copy_instruction(this);
    t->m_condition = clone_node(m_condition);
    t->m_else = clone_node(m_else);
    t->m_body = clone_node(m_body);
    for (const auto& e : m_else_if_nodes) {
      t->m_else_if_nodes.push_back(clone_node(e));
    }
    return core::move(t);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("If");
    c->print_value(m_condition, "Condition");
    c->print_value(m_body, "Body");
    c->print_value(m_else_if_nodes, "ElseIfs");
    c->print_value(m_else, "Else");
  }

private:
  memory::unique_ptr<expression> m_condition;
  memory::unique_ptr<instruction_list> m_else;
  memory::unique_ptr<instruction_list> m_body;
  containers::deque<memory::unique_ptr<instruction>> m_else_if_nodes;
};

class for_instruction : public instruction {
public:
  for_instruction(memory::allocator* _allocator)
    : instruction(_allocator, node_type::for_instruction) {}
  void add_initializer(instruction* _init) {
    m_initializer = memory::unique_ptr<instruction>(m_allocator, _init);
  }
  void add_condition(expression* _cond) {
    m_condition = memory::unique_ptr<expression>(m_allocator, _cond);
  }
  void add_post_op(instruction* _inst) {
    m_post_op = memory::unique_ptr<instruction>(m_allocator, _inst);
  }
  void add_body(instruction_list* _body) {
    m_body = memory::unique_ptr<instruction_list>(m_allocator, _body);
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<for_instruction> t =
        memory::make_unique<for_instruction>(m_allocator, m_allocator);
    t->copy_instruction(this);
    t->m_condition = clone_node(m_condition);
    t->m_initializer = clone_node(m_initializer);
    t->m_body = clone_node(m_body);
    t->m_post_op = clone_node(m_post_op);
    return core::move(t);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("For");
    c->print_value(m_initializer, "Initializer");
    c->print_value(m_condition, "Condition");
    c->print_value(m_post_op, "PostOp");
    c->print_value(m_body, "Body");
  }

private:
  memory::unique_ptr<instruction> m_initializer;
  memory::unique_ptr<expression> m_condition;
  memory::unique_ptr<instruction> m_post_op;
  memory::unique_ptr<instruction_list> m_body;
};

class return_instruction : public instruction {
public:
  return_instruction(memory::allocator* _allocator, expression* _expr,
      bool _change_ownership = false)
    : instruction(_allocator, node_type::return_instruction, true, true),
      m_expression(memory::unique_ptr<expression>(m_allocator, _expr)),
      m_change_ownership(_change_ownership) {}

  return_instruction(memory::allocator* _allocator,
      memory::unique_ptr<expression>&& _expr, bool _change_ownership = false)
    : instruction(_allocator, node_type::return_instruction, true, true),
      m_expression(core::move(_expr)),
      m_change_ownership(_change_ownership) {}

  return_instruction(memory::allocator* _allocator)
    : instruction(_allocator, node_type::return_instruction, true, true) {
    m_change_ownership = false;
  }

  const expression* get_expression() const {
    return m_expression.get();
  }

  expression* get_expression() {
    return m_expression.get();
  }

  virtual void walk_children(const walk_mutable_instruction&,
      const walk_mutable_expression& expr, const walk_ftype<type*>&,
      const walk_ftype<instruction_list*>&, const walk_scope&,
      const walk_scope&) override {
    if (m_expression) {
      handle_expression(expr, m_expression);
    }
  }

  virtual void walk_children(const walk_ftype<const instruction*>&,
      const walk_ftype<const expression*>& expr, const walk_ftype<const type*>&,
      const walk_ftype<const instruction_list*>&, const walk_scope&,
      const walk_scope&) const override {
    if (m_expression) {
      expr(m_expression.get());
    }
  }

  memory::unique_ptr<expression> take_expression() {
    return core::move(m_expression);
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<return_instruction> t =
        memory::make_unique<return_instruction>(m_allocator, m_allocator);
    t->copy_instruction(this);
    t->m_expression = clone_node(m_expression);
    t->m_change_ownership = m_change_ownership;
    return core::move(t);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("Return");
    c->print_value(m_expression, "Value");
  }

private:
  memory::unique_ptr<expression> m_expression;
  bool m_change_ownership;
};

class while_instruction : public instruction {
public:
  while_instruction(
      memory::allocator* _allocator, expression* _cond, instruction_list* _body)
    : instruction(_allocator, node_type::while_instruction),
      m_condition(memory::unique_ptr<expression>(m_allocator, _cond)),
      m_body(memory::unique_ptr<instruction_list>(m_allocator, _body)) {}

  while_instruction(memory::allocator* _allocator)
    : instruction(_allocator, node_type::while_instruction) {}

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<while_instruction> t =
        memory::make_unique<while_instruction>(m_allocator, m_allocator);
    t->copy_instruction(this);
    t->m_condition = clone_node(m_condition);
    t->m_body = clone_node(m_body);
    return core::move(t);
  }

  void print_node_internal(print_context* c) const override {
    c->print_header("While");
    c->print_value(m_condition, "Condition");
    c->print_value(m_body, "Body");
  }

private:
  memory::unique_ptr<expression> m_condition;
  memory::unique_ptr<instruction_list> m_body;
};

class script_file : public node {
public:
  script_file(memory::allocator* _allocator)
    : node(_allocator, node_type::script_file),
      m_functions(_allocator),
      m_external_functions(_allocator),
      m_structs(_allocator),
      m_includes(_allocator) {}
  void add_function(function* _node) {
    m_functions.emplace_back(memory::unique_ptr<function>(m_allocator, _node));
  }

  void prepend_function(memory::unique_ptr<function>&& _function) {
    m_functions.push_front(core::move(_function));
  }

  void add_function(memory::unique_ptr<function>&& _function) {
    m_functions.push_back(core::move(_function));
  }

  void add_external_function(memory::unique_ptr<function>&& _function) {
    m_external_functions.push_back(core::move(_function));
  }

  void add_struct(struct_definition* _node) {
    m_structs.emplace_back(
        memory::unique_ptr<struct_definition>(m_allocator, _node));
  }

  void add_include(const char* _node) {
    m_includes.emplace_back(m_allocator, _node);
  }

  const containers::deque<memory::unique_ptr<function>>&
  get_external_functions() const {
    return m_external_functions;
  }

  const containers::deque<memory::unique_ptr<function>>& get_functions() const {
    return m_functions;
  }

  const containers::deque<memory::unique_ptr<struct_definition>>& get_structs()
      const {
    return m_structs;
  }

  containers::deque<memory::unique_ptr<struct_definition>> take_structs() {
    return core::move(m_structs);
  }

  void put_structs(
      containers::deque<memory::unique_ptr<struct_definition>> structs) {
    m_structs = core::move(structs);
  }

  const containers::deque<containers::string>& get_includes() const {
    return m_includes;
  }

  virtual void walk_structs(const walk_scope&, const walk_scope&,
      const walk_mutable_instruction&, const walk_mutable_expression&,
      const walk_ftype<function*>&, const walk_ftype<struct_definition*>& s) {
    for (auto& def : m_structs) {
      s(def.get());
    }
  }

  virtual void walk_structs(const walk_scope&, const walk_scope&,
      const walk_ftype<const instruction*>&,
      const walk_ftype<const expression*>&, const walk_ftype<const function*>&,
      const walk_ftype<struct_definition*>& s) const {
    for (auto& def : m_structs) {
      s(def.get());
    }
  }

  virtual void walk_functions(const walk_scope&, const walk_scope&,
      const walk_mutable_instruction&, const walk_mutable_expression&,
      const walk_ftype<function*>& f, const walk_ftype<struct_definition*>&) {
    for (auto& function : m_external_functions) {
      f(function.get());
    }
    for (auto& function : m_functions) {
      f(function.get());
    }
  }

  virtual void walk_functions(const walk_scope&, const walk_scope&,
      const walk_ftype<const instruction*>&,
      const walk_ftype<const expression*>&,
      const walk_ftype<const function*>& f,
      const walk_ftype<struct_definition*>&) const {
    for (auto& function : m_external_functions) {
      f(function.get());
    }
    for (auto& function : m_functions) {
      f(function.get());
    }
  }

  memory::unique_ptr<node> clone() const override {
    memory::unique_ptr<script_file> t =
        memory::make_unique<script_file>(m_allocator, m_allocator);
    t->copy_node(this);
    for (const auto& function : m_functions) {
      t->m_functions.push_back(clone_node(function));
    }
    for (const auto& external_function : m_external_functions) {
      t->m_external_functions.push_back(clone_node(external_function));
    }
    for (const auto& strt : m_structs) {
      t->m_structs.push_back(clone_node(strt));
    }
    for (const auto& include : m_includes) {
      t->m_includes.push_back(containers::string(m_allocator, include));
    }
    return core::move(t);
  }

  virtual void print_node_internal(print_context* c) const override {
    c->print_header("Script File");
    c->print_value(m_includes, "Includes");
    c->print_value(m_external_functions, "External Functions");
    c->print_value(m_functions, "Functions");
    c->print_value(m_structs, "Structs");
  }

private:
  containers::deque<memory::unique_ptr<function>> m_functions;
  containers::deque<memory::unique_ptr<function>> m_external_functions;
  containers::deque<memory::unique_ptr<struct_definition>> m_structs;
  containers::deque<containers::string> m_includes;
};

}  // namespace scripting
}  // namespace wn

#endif  // __WN_SCRIPTING_NODE_TYPES_H__
