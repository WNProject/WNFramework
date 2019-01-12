// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNScripting/inc/WNScriptHelpers.h"
#include "WNScripting/inc/WNScriptTLS.h"

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

  template <typename R, typename... Args>
  struct member_maker {
    static const bool is_ret_by_ref = pass_by_reference<R>::value;
    template <R (T::*fn)(Args...)>
    static typename get_thunk_passed_type<R>::ret_type
#if defined(_WN_WINDOWS) && defined(_WN_X86) && !defined(_WN_64_BIT)
        __thiscall
#endif
        member_thunk(T* t, typename get_thunk_passed_type<Args>::type... args) {
      tls_resetter reset;
      return get_thunk_passed_type<R>::unwrap(
          (t->*fn)(get_thunk_passed_type<Args>::unwrap(args)...));
    }

    template <R (T::*fn)(Args...)>
    static void
#if defined(_WN_WINDOWS) && defined(_WN_X86) && !defined(_WN_64_BIT)
        __thiscall
#endif
        return_member_thunk(T* t,
            typename get_thunk_passed_type<Args>::type... args,
            typename get_thunk_passed_type<R>::ret_type* _ret) {
      tls_resetter reset;
      *_ret = get_thunk_passed_type<R>::unwrap(
          (t->*fn)(get_thunk_passed_type<Args>::unwrap(args)...));
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
      tls_resetter reset;
      return (t->*fn)(get_thunk_passed_type<Args>::unwrap(args)...);
    }

    template <void (T::*fn)(Args...)>
    static void
#if defined(_WN_WINDOWS) && defined(_WN_X86) && !defined(_WN_64_BIT)
        __thiscall
#endif
        return_member_thunk(
            T* t, typename get_thunk_passed_type<Args>::type... args) {
      tls_resetter reset;
      (t->*fn)(get_thunk_passed_type<Args>::unwrap(args)...);
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
    static typename get_thunk_passed_type<R>::ret_type member_thunk(
        T* t, typename get_thunk_passed_type<Args>::type... args) {
      tls_resetter reset;
      return get_thunk_passed_type<R>::unwrap(
          (*fn)(t, get_thunk_passed_type<Args>::unwrap(args)...));
    }

    template <R (*fn)(T*, Args...)>
    static void return_member_thunk(T* t,
        typename get_thunk_passed_type<Args>::type... args,
        typename get_thunk_passed_type<R>::ret_type* _ret) {
      tls_resetter reset;
      *_ret = get_thunk_passed_type<R>::unwrap(
          (*fn)(t, get_thunk_passed_type<Args>::unwrap(args)...));
    }
  };

  template <typename... Args>
  struct pseudo_member_maker<void, Args...> {
    // What you have to do now is add something like (fixup_return_type)
    // to these functions. It may actually BE fixup_return_type,
    // but we have to figure out how to plumb the right things through
    static const bool is_ret_by_ref = false;
    template <void (*fn)(T*, Args...)>
    static void member_thunk(
        T* t, typename get_thunk_passed_type<Args>::type... args) {
      tls_resetter reset;
      return (*fn)(t, get_thunk_passed_type<Args>::unwrap(args)...);
    }

    template <void (*fn)(T*, Args...)>
    static void return_member_thunk(
        T* t, typename get_thunk_passed_type<Args>::type... args) {
      tls_resetter reset;
      (*fn)(t, get_thunk_passed_type<Args>::unwrap(args)...);
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
    containers::string_view v = add_contained_function(_name, types, true);
    return _added_function(v, core::move(types));
  }

  template <typename F, F fptr>
  void register_nonvirtual_function(containers::string_view _name) {
    auto af = _add_function(_name, fptr);
    void* fn = get_thunk<F, fptr>();

    if (m_callback && *m_callback) {
      (*m_callback)(af.name, fn, false);
    }
  }

  template <typename F, F fptr>
  void register_pseudo_function(containers::string_view _name) {
    auto af = _add_function(_name, fptr);
    static_assert(sizeof(F) == sizeof(uintptr_t), "Invalid function pointer");
    void* fn = get_pseudo_thunk<F, fptr>();

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
      using t = virtual_registrar<R, Args...>;
      m_exporter->register_pseudo_function<
          decltype(&t::dispatch<fn>),
          &t::dispatch<fn>>(_name);
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
  WN_DEBUG_ASSERT(m_externally_visible_types.find(core::type_id<T>::value()) ==
                      m_externally_visible_types.end(),
      "You registered the same type c++ type twice in this scripting engine");
  containers::string_view name = exported_script_type<T>::exported_name();
  ast_type* parent_type = m_externally_visible_types
      [core::type_id<typename exported_script_type<T>::parent_type>::value()];

  ast_type* t = register_external_type(name, sizeof(T), parent_type);
  m_externally_visible_types[core::type_id<T>::value()] = t;
  m_externally_visible_types[core::type_id<T*>::value()] =
      get_reference_of(t, ast_type_classification::reference, nullptr);
  m_externally_visible_types[core::type_id<shared_cpp_pointer<T>>::value()] =
      get_reference_of(t, ast_type_classification::shared_reference, nullptr);
  exporter<T> exporter(t, m_allocator, this, &_fn);
  exported_script_type<T>::export_type(&exporter);
  finalize_external_type(t);
}

template <typename T>
void type_manager::register_cpp_type(functional::function<void(
        containers::string_view _mangled_name, void* _ptr, bool _is_virtual)>
        _fn) {
  WN_DEBUG_ASSERT(m_externally_visible_types.find(core::type_id<T>::value()) ==
                      m_externally_visible_types.end(),
      "You registered the same type c++ type twice in this scripting engine");
  containers::string_view name = exported_script_type<T>::exported_name();

  ast_type* t = register_external_type(name, sizeof(T), nullptr);
  m_externally_visible_types[core::type_id<T>::value()] = t;
  m_externally_visible_types[core::type_id<T*>::value()] =
      get_reference_of(t, ast_type_classification::reference, nullptr);
  m_externally_visible_types[core::type_id<shared_cpp_pointer<T>>::value()] =
      get_reference_of(t, ast_type_classification::shared_reference, nullptr);
  m_externally_visible_types[core::type_id<shared_cpp_pointer<T>>::value()] =
      get_reference_of(t, ast_type_classification::shared_reference, nullptr);
  exporter<T> exporter(t, m_allocator, this, &_fn);
  exported_script_type<T>::export_type(&exporter);
  finalize_external_type(t);
}

template <typename T>
void type_manager::export_script_type() {
  if (m_externally_visible_types.find(core::type_id<T>::value()) !=
      m_externally_visible_types.end()) {
    return;
  }
  containers::string_view name = T::exported_name();
  export_script_type(name);
  ast_type* t = m_structure_types.find(name)->second.get();
  m_externally_visible_types[core::type_id<T>::value()] = t;
  m_externally_visible_types[core::type_id<script_pointer<T>>::value()] =
      get_reference_of(t, ast_type_classification::reference, nullptr);
  m_externally_visible_types[core::type_id<shared_script_pointer<T>>::value()] =
      get_reference_of(t, ast_type_classification::shared_reference, nullptr);
  m_externally_visible_types[core::type_id<wn_array<T>>::value()] =
      get_array_of(t, 0, nullptr, false);
  m_externally_visible_types
      [core::type_id<wn_array<script_pointer<T>>>::value()] = get_array_of(
          get_reference_of(t, ast_type_classification::reference, nullptr), 0,
          nullptr, false);
}

}  // namespace scripting
}  // namespace wn
