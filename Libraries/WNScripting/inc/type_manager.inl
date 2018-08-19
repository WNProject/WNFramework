// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.
namespace wn {
namespace scripting {

struct exporter_base {
  void add_contained_type(
      containers::string_view _name, const ast_type* _type, uint32_t offset);
  containers::string_view add_contained_function(containers::string_view _name,
      containers::dynamic_array<const ast_type*> _types, bool _is_external_pseudo);

protected:
  exporter_base(ast_type* _type, memory::allocator* _allocator,
      type_manager* _manager,
      functional::function<void(containers::string_view _mangled_name,
          void* _ptr, bool _is_virtual)>* _cb)
    : m_type(_type),
      m_allocator(_allocator),
      m_type_manager(_manager),
      m_callback(_cb) {}
  ast_type* m_type;
  memory::allocator* m_allocator;
  type_manager* m_type_manager;
  functional::function<void(containers::string_view _mangled_name, void* _ptr,
      bool _is_virtual)>* m_callback;
};

template <typename T>
struct exporter : public exporter_base {
  exporter(ast_type* _type, memory::allocator* _allocator,
      type_manager* _manager,
      functional::function<void(containers::string_view _mangled_name,
          void* _ptr, bool _is_virtual)>* _cb)
    : exporter_base(_type, _allocator, _manager, _cb) {}
  template <typename V>
  void register_member(containers::string_view _name, V T::*_member) {
    size_t offset =
        reinterpret_cast<size_t>(
            &(reinterpret_cast<T*>(static_cast<size_t>(100))->*_member)) -
        100;
    add_contained_type(
        _name, m_type_manager->get_type<V>(), static_cast<uint32_t>(offset));
  }

  template <typename R, typename... Args>
  void register_nonvirtual_function(
      containers::string_view _name, R (T::*_fn)(Args...)) {
    auto types = m_type_manager->get_types<R, T*, Args...>();
    containers::string_view v = add_contained_function(_name, types, false);
    void* fn;
    memcpy(&fn, &_fn, sizeof(uintptr_t));
    if (m_callback && *m_callback) {
      (*m_callback)(v, fn, false);
    }
  }

  template <typename R, typename... Args>
  void register_pseudo_function(
      containers::string_view _name, R (*_fn)(T*, Args...)) {
    static_assert(sizeof(_fn) == sizeof(uintptr_t), "Invalid function pointer");
    auto types = m_type_manager->get_types<R, T*, Args...>();
    containers::string_view v = add_contained_function(_name, types, true);
    void* fn;
    memcpy(&fn, &_fn, sizeof(uintptr_t));
    if (m_callback && *m_callback) {
      (*m_callback)(v, fn, true);
    }
  }

  template <typename R, typename... Args>
  struct virtual_registrar {
  public:
    virtual_registrar(exporter<T>* _exporter) : m_exporter(_exporter) {}

    template <R (T::*fn)(Args...)>
    void register_virtual(containers::string_view _name) {
      m_exporter->register_pseudo_function(
          _name, &virtual_registrar<R, Args...>::dispatch<fn>);
    }

  private:
    template <R (T::*fn)(Args...)>
    static R dispatch(T* _base, Args... _args) {
      return (_base->*fn)(_args...);
    }
    exporter<T>* m_exporter;
  };

  template <typename R, typename... Args>
  virtual_registrar<R, Args...> register_virtual() {
    return virtual_registrar<R, Args...>(this);
  }
};

template <typename T>
void type_manager::register_child_cpp_type(functional::function<void(
        containers::string_view _mangled_name, void* _ptr, bool _is_virtual)>
        _fn) {
  WN_DEBUG_ASSERT(
      m_externally_visible_types.find(c_type_tag<T>::get_unique_identifier()) ==
          m_externally_visible_types.end(),
      "You registered the same type c++ type twice in this scripting engine");
  containers::string_view name = exported_script_type<T>::exported_name();
  ast_type* parent_type = m_externally_visible_types[c_type_tag<
      typename exported_script_type<T>::parent_type>::get_unique_identifier()];

  ast_type* t = register_external_type(name, sizeof(T), parent_type);
  m_externally_visible_types[c_type_tag<T>::get_unique_identifier()] = t;
  m_externally_visible_types[c_type_tag<T*>::get_unique_identifier()] =
      get_reference_of(t, ast_type_classification::reference, nullptr);
  exporter<T> exporter(t, m_allocator, this, &_fn);
  exported_script_type<T>::export_type(&exporter);
  finalize_external_type(t);
}

template <typename T>
void type_manager::register_cpp_type(functional::function<void(
        containers::string_view _mangled_name, void* _ptr, bool _is_virtual)>
        _fn) {
  WN_DEBUG_ASSERT(
      m_externally_visible_types.find(c_type_tag<T>::get_unique_identifier()) ==
          m_externally_visible_types.end(),
      "You registered the same type c++ type twice in this scripting engine");
  containers::string_view name = exported_script_type<T>::exported_name();

  ast_type* t = register_external_type(name, sizeof(T), nullptr);
  m_externally_visible_types[c_type_tag<T>::get_unique_identifier()] = t;
  m_externally_visible_types[c_type_tag<T*>::get_unique_identifier()] =
      get_reference_of(t, ast_type_classification::reference, nullptr);
  exporter<T> exporter(t, m_allocator, this, &_fn);
  exported_script_type<T>::export_type(&exporter);
  finalize_external_type(t);
}

}  // namespace scripting
}  // namespace wn
