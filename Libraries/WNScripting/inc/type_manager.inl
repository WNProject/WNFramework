// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.
namespace wn {
namespace scripting {

struct exporter_base {
  void add_contained_type(
      containers::string_view _name, const ast_type* _type, uint32_t offset);
  containers::string_view add_contained_function(containers::string_view _name,
      containers::dynamic_array<const ast_type*> _types,
      bool _is_external_pseudo);

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

  template <typename U, U>
  struct fn_args {};

  template <typename U>
  static typename core::enable_if<pass_by_reference<U>::value, U>::type
  pass_by_ref_if_needed(U* val) {
    return *val;
  }

  template <typename U>
  static typename core::enable_if<!pass_by_reference<U>::value, U>::type
  pass_by_ref_if_needed(U& val) {
    return val;
  }

  template <typename U, typename enable = void>
  struct get_thunk_passed_type {
    using type = U;
  };

  template <typename U>
  struct get_thunk_passed_type<U,
      typename core::enable_if<pass_by_reference<U>::value>::type> {
    using type = U*;
  };

  template <typename R, typename... Args>
  struct member_maker {
    static const bool is_ret_by_ref = pass_by_reference<R>::value;
    template <R (T::*fn)(Args...)>
    static R
#if defined(_WN_WINDOWS) && defined(_WN_X86) && !defined(_WN_64_BIT)
        __thiscall
#endif
        member_thunk(T* t, typename get_thunk_passed_type<Args>::type... args) {
      return (t->*fn)(pass_by_ref_if_needed(args)...);
    }

    template <R (T::*fn)(Args...)>
    static void
#if defined(_WN_WINDOWS) && defined(_WN_X86) && !defined(_WN_64_BIT)
        __thiscall
#endif
        return_member_thunk(T* t, typename get_thunk_passed_type<Args>::type... args, R* _ret) {
      *_ret = (t->*fn)(pass_by_ref_if_needed(args)...);
    }
  };

  template <typename... Args>
  struct member_maker<void, Args...> {
    static const bool is_ret_by_ref = false;
    template <void (T::*fn)(Args...)>
    static void
#if defined(_WN_WINDOWS) && defined(_WN_X86) && !defined(_WN_64_BIT)
        __thiscall
#endif
        member_thunk(T* t, typename get_thunk_passed_type<Args>::type... args) {
      return (t->*fn)(pass_by_ref_if_needed(args)...);
    }

    template <void (T::*fn)(Args...)>
    static void
#if defined(_WN_WINDOWS) && defined(_WN_X86) && !defined(_WN_64_BIT)
        __thiscall
#endif
        return_member_thunk(
            T* t, typename get_thunk_passed_type<Args>::type... args) {
      (t->*fn)(pass_by_ref_if_needed(args)...);
    }
  };

  template <typename R, typename... Args>
  inline member_maker<R, Args...> get_member_maker(R (T::*)(Args...)) {
    return member_maker<R, Args...>{};
  };

  template <typename R, typename... Args>
  struct pseudo_member_maker {
    static const bool is_ret_by_ref = pass_by_reference<R>::value;
    template <R (*fn)(T*, Args...)>
    static R
        member_thunk(T* t, typename get_thunk_passed_type<Args>::type... args) {
      return (*fn)(t, pass_by_ref_if_needed(args)...);
    }

    template <R (*fn)(T*, Args...)>
    static void return_member_thunk(
        T* t, typename get_thunk_passed_type<Args>::type... args, R* _ret) {
      *_ret = (*fn)(t, pass_by_ref_if_needed(args)...);
    }
  };

  template <typename... Args>
  struct pseudo_member_maker<void, Args...> {
    static const bool is_ret_by_ref = false;
    template <void (*fn)(T*, Args...)>
    static void member_thunk(
        T* t, typename get_thunk_passed_type<Args>::type... args) {
      return (*fn)(t, pass_by_ref_if_needed(args)...);
    }

    template <void (*fn)(T*, Args...)>
    static void return_member_thunk(
        T* t, typename get_thunk_passed_type<Args>::type... args) {
      (*fn)(t, pass_by_ref_if_needed(args)...);
    }
  };

  template <typename R, typename... Args>
  inline pseudo_member_maker<R, Args...> get_pseudo_member_maker(
      R (*)(T*, Args...)) {
    return pseudo_member_maker<R, Args...>{};
  };

  template <typename F, F fptr>
  void* get_thunk() {
    void* v;
    if (decltype(get_member_maker(fptr))::is_ret_by_ref) {
      auto mt =
          &decltype(get_member_maker(fptr))::template return_member_thunk<fptr>;
      memcpy(&v, &mt, sizeof(uintptr_t));
    } else {
      auto mt = &decltype(get_member_maker(fptr))::template member_thunk<fptr>;
      memcpy(&v, &mt, sizeof(uintptr_t));
    }
    return v;
  }

  template <typename F, F fptr>
  void* get_pseudo_thunk() {
    void* v;
    if (decltype(get_pseudo_member_maker(fptr))::is_ret_by_ref) {
      auto mt = decltype(
          get_pseudo_member_maker(fptr))::template return_member_thunk<fptr>;
      memcpy(&v, &mt, sizeof(uintptr_t));
    } else {
      auto mt =
          decltype(get_pseudo_member_maker(fptr))::template member_thunk<fptr>;
      memcpy(&v, &mt, sizeof(uintptr_t));
    }
    return v;
  }

  struct _added_function {
    _added_function(containers::string_view _name,
        containers::dynamic_array<const ast_type*> _types)
      : name(_name), types(_types) {}
    containers::string_view name;
    containers::dynamic_array<const ast_type*> types;
  };

  template <typename R, typename... Args>
  _added_function _add_function(
      containers::string_view _name, R (T::*)(Args...)) {
    auto types = m_type_manager->get_types<R, T*, Args...>();
    containers::string_view v = add_contained_function(_name, types, false);
    return _added_function(v, core::move(types));
  }

  template <typename R, typename... Args>
  _added_function _add_function(containers::string_view _name, R(T*, Args...)) {
    auto types = m_type_manager->get_types<R, T*, Args...>();
    containers::string_view v = add_contained_function(_name, types, false);
    return _added_function(v, core::move(types));
  }

  template <typename F, F fptr>
  void register_nonvirtual_function(containers::string_view _name) {
    auto af = _add_function(_name, fptr);
    F f = fptr;
    void* fn;
    memcpy(&fn, &f, sizeof(uintptr_t));
    for (size_t i = 0; i < af.types.size(); ++i) {
      if (m_type_manager->is_pass_by_reference(af.types[i])) {
        auto t = get_thunk<F, fptr>();
        memcpy(&fn, &t, sizeof(uintptr_t));
        break;
      }
    }

    if (m_callback && *m_callback) {
      (*m_callback)(af.name, fn, false);
    }
  }

  template <typename F, F fptr>
  void register_pseudo_function(containers::string_view _name) {
    auto af = _add_function(_name, fptr);
    F f = fptr;
    static_assert(sizeof(f) == sizeof(uintptr_t), "Invalid function pointer");
    void* fn;
    memcpy(&fn, &f, sizeof(uintptr_t));
    for (size_t i = 0; i < af.types.size(); ++i) {
      if (m_type_manager->is_pass_by_reference(af.types[i])) {
        auto t = get_pseudo_thunk<F, fptr>();
        memcpy(&fn, &t, sizeof(uintptr_t));
        break;
      }
    }

    if (m_callback && *m_callback) {
      (*m_callback)(af.name, fn, false);
    }
  }

  template <typename R, typename... Args>
  struct virtual_registrar {
  public:
    virtual_registrar(exporter<T>* _exporter) : m_exporter(_exporter) {}

    template <R (T::*fn)(Args...)>
    void register_virtual(containers::string_view _name) {
      m_exporter->register_pseudo_function<
          decltype(&virtual_registrar<R, Args...>::dispatch<fn>),
          &virtual_registrar<R, Args...>::dispatch<fn>>(_name);
    }

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

template <typename T>
void type_manager::export_script_type() {
  if (m_externally_visible_types.find(c_type_tag<T>::get_unique_identifier()) !=
      m_externally_visible_types.end()) {
    return;
  }
  containers::string_view name = T::exported_name();
  export_script_type(name);
  ast_type* t = m_structure_types.find(name)->second.get();
  m_externally_visible_types[c_type_tag<T>::get_unique_identifier()] = t;
  m_externally_visible_types
      [c_type_tag<script_pointer<T>>::get_unique_identifier()] =
          get_reference_of(t, ast_type_classification::reference, nullptr);
  m_externally_visible_types
      [c_type_tag<shared_script_pointer<T>>::get_unique_identifier()] =
          get_reference_of(t, ast_type_classification::shared_reference, nullptr);
}

}  // namespace scripting
}  // namespace wn
