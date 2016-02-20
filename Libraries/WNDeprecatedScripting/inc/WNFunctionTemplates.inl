// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_FUNCTION_TEMPLATES_INL___
#define __WN_FUNCTION_TEMPLATES_INL___
#include "WNDeprecatedScripting/inc/WNScriptingInteropForward.h"
namespace WNScripting {

template <typename T>
WNScriptType WNScriptingEngine::get_type_for_type() {
  WNScriptType t = get_type_by_name(TypeMapping<T>::get_type_name());
  return (TypeMapping<T>::GetModifiedType(t, this));
}

template <typename T>
void Destructor(void* ptr) {
  StructInternalType* internalPtr = reinterpret_cast<StructInternalType*>(ptr);
  (void)internalPtr;  // if the destructor is null, then the next line goes
                      // away, and VS things internalPtr is not referenced
  // So that we can get a T* from scripting, and we can call the destructor
  // correctly
  // this will allow us to free the memory consistently in scripting (free), but
  // get all the virtual destructors called in C++ properly
  reinterpret_cast<T*>(internalPtr->structLoc)->~T();
}

template <typename T, typename R, R (T::*P)()>
struct ObjectMethodStruct {
  static R ObjectMethod(void* t) {
    // this little bunch of awesome, allows us to call virtual member functions
    // on object of type T, given a pointer
    // it is up to scripting to give us an ACTUAL type t
    StructInternalType* internalPtr = reinterpret_cast<StructInternalType*>(t);
    return ((reinterpret_cast<T*>(internalPtr->structLoc)->*P)());
  }
};

template <typename T, typename R, typename T1, R (T::*P)(T1)>
struct ObjectMethodStruct1 {
  static R ObjectMethod(void* t, T1 _t1) {
    StructInternalType* internalPtr = reinterpret_cast<StructInternalType*>(t);
    return ((reinterpret_cast<T*>(internalPtr->structLoc)->*P)(_t1));
  }
};

template <typename T, typename R, typename T1, typename T2, R (T::*P)(T1, T2)>
struct ObjectMethodStruct2 {
  static typename InCaster<R>::OutType ObjectMethod(void* t,
      typename OutCaster<T1>::InType _t1, typename OutCaster<T2>::InType _t2) {
    StructInternalType* internalPtr = reinterpret_cast<StructInternalType*>(t);
    return (InCaster<R>::ConvertOut(
        (reinterpret_cast<T*>(internalPtr->structLoc)->*P)(
            OutCaster<T1>::ConvertOut(_t1), OutCaster<T2>::ConvertOut(_t2))));
  }
};

template <typename T, typename R, typename T1, typename T2, typename T3,
    R (T::*P)(T1, T2, T3)>
struct ObjectMethodStruct3 {
  static typename InCaster<R>::OutType ObjectMethod(void* t,
      typename OutCaster<T1>::InType _t1, typename OutCaster<T2>::InType _t2,
      typename OutCaster<T3>::InType _t3) {
    StructInternalType* internalPtr = reinterpret_cast<StructInternalType*>(t);
    return (InCaster<R>::ConvertOut(
        (reinterpret_cast<T*>(internalPtr->structLoc)->*P)(
            OutCaster<T1>::ConvertOut(_t1), OutCaster<T2>::ConvertOut(_t2),
            OutCaster<T3>::ConvertOut(_t3))));
  }
};

template <typename T, typename R, typename T1, typename T2, typename T3,
    typename T4, R (T::*P)(T1, T2, T3, T4)>
struct ObjectMethodStruct4 {
  static typename InCaster<R>::OutType ObjectMethod(void* t,
      typename OutCaster<T1>::InType _t1, typename OutCaster<T2>::InType _t2,
      typename OutCaster<T3>::InType _t3, typename OutCaster<T4>::InType _t4) {
    StructInternalType* internalPtr = reinterpret_cast<StructInternalType*>(t);
    return (InCaster<R>::ConvertOut(
        (reinterpret_cast<T*>(internalPtr->structLoc)->*P)(
            OutCaster<T1>::ConvertOut(_t1), OutCaster<T2>::ConvertOut(_t2),
            OutCaster<T3>::ConvertOut(_t3), OutCaster<T4>::ConvertOut(_t4))));
  }
};

template <typename T, typename R, typename T1, typename T2, typename T3,
    typename T4, typename T5, R (T::*P)(T1, T2, T3, T4, T5)>
struct ObjectMethodStruct5 {
  static typename InCaster<R>::OutType ObjectMethod(void* t,
      typename OutCaster<T1>::InType _t1, typename OutCaster<T2>::InType _t2,
      typename OutCaster<T3>::InType _t3, typename OutCaster<T4>::InType _t4,
      typename OutCaster<T5>::InType _t5) {
    StructInternalType* internalPtr = reinterpret_cast<StructInternalType*>(t);
    return (InCaster<R>::ConvertOut(
        (reinterpret_cast<T*>(internalPtr->structLoc)->*P)(
            OutCaster<T1>::ConvertOut(_t1), OutCaster<T2>::ConvertOut(_t2),
            OutCaster<T3>::ConvertOut(_t3), OutCaster<T4>::ConvertOut(_t4),
            OutCaster<T5>::ConvertOut(_t5))));
  }
};

template <typename T, typename R, typename T1, typename T2, typename T3,
    typename T4, typename T5, typename T6, R (T::*P)(T1, T2, T3, T4, T5, T6)>
struct ObjectMethodStruct6 {
  static typename InCaster<R>::OutType ObjectMethod(void* t,
      typename OutCaster<T1>::InType _t1, typename OutCaster<T2>::InType _t2,
      typename OutCaster<T3>::InType _t3, typename OutCaster<T4>::InType _t4,
      typename OutCaster<T5>::InType _t5, typename OutCaster<T6>::InType _t6) {
    StructInternalType* internalPtr = reinterpret_cast<StructInternalType*>(t);
    return (InCaster<R>::ConvertOut(
        (reinterpret_cast<T*>(internalPtr->structLoc)->*P)(
            OutCaster<T1>::ConvertOut(_t1), OutCaster<T2>::ConvertOut(_t2),
            OutCaster<T3>::ConvertOut(_t3), OutCaster<T4>::ConvertOut(_t4),
            OutCaster<T5>::ConvertOut(_t5), OutCaster<T6>::ConvertOut(_t6))));
  }
};

template <typename T, typename R, typename T1, typename T2, typename T3,
    typename T4, typename T5, typename T6, typename T7,
    R (T::*P)(T1, T2, T3, T4, T5, T6, T7)>
struct ObjectMethodStruct7 {
  static typename InCaster<R>::OutType ObjectMethod(void* t,
      typename OutCaster<T1>::InType _t1, typename OutCaster<T2>::InType _t2,
      typename OutCaster<T3>::InType _t3, typename OutCaster<T4>::InType _t4,
      typename OutCaster<T5>::InType _t5, typename OutCaster<T6>::InType _t6,
      typename OutCaster<T7>::InType _t7) {
    StructInternalType* internalPtr = reinterpret_cast<StructInternalType*>(t);
    return (InCaster<R>::ConvertOut(
        (reinterpret_cast<T*>(internalPtr->structLoc)->*P)(
            OutCaster<T1>::ConvertOut(_t1), OutCaster<T2>::ConvertOut(_t2),
            OutCaster<T3>::ConvertOut(_t3), OutCaster<T4>::ConvertOut(_t4),
            OutCaster<T5>::ConvertOut(_t5), OutCaster<T6>::ConvertOut(_t6),
            OutCaster<T7>::ConvertOut(_t7))));
  }
};

template <typename T1, typename T2>
eWNTypeError WNScriptingEngine::construct_scripting_object(
    WNScriptingObject<T1, T2>& _outType) {
  WNScriptType t = get_type_for_type<WNScriptingObject<T1, T2>>();
  if (!t) {
    return (error);
  }
  void* v = nullptr;
  if (ok == construct_scripting_object(t, v)) {
    _outType = WNScriptingObject<T1, T2>(v);
  }
  return (ok);
}

template <typename T1>
eWNTypeError WNScriptingEngine::construct_scripting_array(
    WNScriptingArray<T1>& _outType, size_t _size) {
  WNScriptType t = get_type_for_type<WNScriptingArray<T1>>();
  if (!t) {
    return (error);
  }
  void* v = nullptr;
  if (ok == construct_scripting_array(t, _size, v)) {
    _outType = WNScriptingArray<T1>(v);
  }
  return (ok);
}

template <typename T>
eWNTypeError WNScriptingEngine::register_cpp_Type() {
  WNScriptType t = get_type_for_type<T>();
  if (t) {
    return (eWNAlreadyExists);
  }
  register_external_type(TypeMapping<T>::get_type_name(), &Destructor<T>);
  return (ok);
}

template <typename T, typename TParent>
eWNTypeError WNScriptingEngine::register_cpp_Type() {
  WNScriptType t = get_type_for_type<T>();
  if (t) {
    return (eWNAlreadyExists);
  }
  register_external_type(TypeMapping<T>::get_type_name(),
      TypeMapping<TParent>::get_type_name(), &Destructor<T>);
  return (ok);
}

template <typename T, typename PT, PT T::*V>
eWNTypeError WNScriptingEngine::register_cpp_member(const char* _name) {
  WNScriptType t = get_type_for_type<T>();
  if (!t) {
    return (does_not_exist);
  }
  WNScriptType pt = get_type_for_type<PT>();
  if (!pt) {
    return (does_not_exist);
  }
  void* v = &((reinterpret_cast<T*>(0))->*V);

  register_member(_name, t, pt, reinterpret_cast<size_t>(v));
  return (ok);
}

template <typename T, typename R>
eWNTypeError WNScriptingEngine::get_pointer_to_member_function(
    const char* _file, const char* _functionName,
    WNMemberFunctionPointer<T, R>& _outVal) {
  eWNTypeError err = compile_file(_file);
  if (err != ok) {
    return (err);
  }
  WNScriptType r = get_type_for_type<R>();
  if (!r) {
    return (eWNBadType);
  }
  WNScriptType t = get_type_for_type<T>();
  if (!t) {
    return (eWNBadType);
  }

  void* ptr = NULL;
  std::vector<WNScriptType> params;
  params.push_back(t);
  int32_t mVirtualIndex;
  if (0 <=
      (mVirtualIndex = get_virtual_function_index(_functionName, t, params))) {
    _outVal.Assign(mVirtualIndex);
    _outVal.SetOutType(t);
    return (ok);
  }
  char fPtr[2048];
  memcpy(fPtr, t->mName, wn::memory::strlen(t->mName) + 1);
  wn::memory::strcat(fPtr, "$");
  wn::memory::strcat(fPtr, _functionName);
  if (ok != (err = get_function_pointer(_file, fPtr, r, params, ptr))) {
    return (err);
  }

  _outVal.Assign(reinterpret_cast<R (*)(T)>(ptr));
  _outVal.SetOutType(t);
  return (ok);
}

template <typename R>
eWNTypeError WNScriptingEngine::get_pointer_to_function(const char* _file,
    const char* _functionName, WNFunctionPointer<R>& _outVal) {
  eWNTypeError err = compile_file(_file);
  if (err != ok) {
    return (err);
  }

  WNScriptType t = get_type_for_type<R>();
  if (!t) {
    return (eWNBadType);
  }

  std::vector<WNScriptType> params;
  void* ptr = NULL;
  if (ok !=
      (err = get_function_pointer(_file, _functionName, t, params, ptr))) {
    return (err);
  }
  _outVal.Assign(reinterpret_cast<R (*)()>(ptr));
  _outVal.SetOutType(t);
  return (ok);
}

template <typename R, typename T1>
eWNTypeError WNScriptingEngine::get_pointer_to_function(const char* _file,
    const char* _functionName, WNFunctionPointer<R, T1>& _outVal) {
  eWNTypeError err = compile_file(_file);
  if (err != ok) {
    return (err);
  }

  WNScriptType t = get_type_for_type<R>();
  if (!t) {
    return (eWNBadType);
  }

  std::vector<WNScriptType> params;
  params.push_back(get_type_for_type<T1>());
  if (!params.back()) {
    return (eWNBadType);
  }
  void* ptr = NULL;
  if (ok !=
      (err = get_function_pointer(_file, _functionName, t, params, ptr))) {
    return (err);
  }
  _outVal.Assign(reinterpret_cast<R (*)(T1)>(ptr));
  _outVal.SetOutType(t);
  return (ok);
}

template <typename R, typename T1, typename T2>
eWNTypeError WNScriptingEngine::get_pointer_to_function(const char* _file,
    const char* _functionName, WNFunctionPointer<R, T1, T2>& _outVal) {
  eWNTypeError err = compile_file(_file);
  if (err != ok) {
    return (err);
  }

  WNScriptType t = get_type_for_type<R>();
  if (!t) {
    return (eWNBadType);
  }

  std::vector<WNScriptType> params;
  params.push_back(get_type_for_type<T1>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T2>());
  if (!params.back()) {
    return (eWNBadType);
  }

  void* ptr = NULL;
  if (ok !=
      (err = get_function_pointer(_file, _functionName, t, params, ptr))) {
    return (err);
  }
  _outVal.Assign(reinterpret_cast<R (*)(T1, T2)>(ptr));
  _outVal.SetOutType(t);
  return (ok);
}

template <typename R, typename T1, typename T2, typename T3>
eWNTypeError WNScriptingEngine::get_pointer_to_function(const char* _file,
    const char* _functionName, WNFunctionPointer<R, T1, T2, T3>& _outVal) {
  eWNTypeError err = compile_file(_file);
  if (err != ok) {
    return (err);
  }

  WNScriptType t = get_type_for_type<R>();
  if (!t) {
    return (eWNBadType);
  }

  std::vector<WNScriptType> params;
  params.push_back(get_type_for_type<T1>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T2>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T3>());
  if (!params.back()) {
    return (eWNBadType);
  }
  void* ptr = NULL;
  if (ok !=
      (err = get_function_pointer(_file, _functionName, t, params, ptr))) {
    return (err);
  }
  _outVal.Assign(reinterpret_cast<R (*)(T1, T2, T3)>(ptr));
  _outVal.SetOutType(t);
  return (ok);
}

template <typename R, typename T1, typename T2, typename T3, typename T4>
eWNTypeError WNScriptingEngine::get_pointer_to_function(const char* _file,
    const char* _functionName, WNFunctionPointer<R, T1, T2, T3, T4>& _outVal) {
  eWNTypeError err = compile_file(_file);
  if (err != ok) {
    return (err);
  }

  WNScriptType t = get_type_for_type<R>();
  if (!t) {
    return (eWNBadType);
  }

  std::vector<WNScriptType> params;
  params.push_back(get_type_for_type<T1>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T2>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T3>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T4>());
  if (!params.back()) {
    return (eWNBadType);
  }
  void* ptr = NULL;
  if (ok !=
      (err = get_function_pointer(_file, _functionName, t, params, ptr))) {
    return (err);
  }
  _outVal.Assign(reinterpret_cast<R (*)(T1, T2, T3, T4)>(ptr));
  _outVal.SetOutType(t);
  return (ok);
}

template <typename R, typename T1, typename T2, typename T3, typename T4,
    typename T5>
eWNTypeError WNScriptingEngine::get_pointer_to_function(const char* _file,
    const char* _functionName,
    WNFunctionPointer<R, T1, T2, T3, T4, T5>& _outVal) {
  eWNTypeError err = compile_file(_file);
  if (err != ok) {
    return (err);
  }

  WNScriptType t = get_type_for_type<R>();
  if (!t) {
    return (eWNBadType);
  }

  std::vector<WNScriptType> params;
  params.push_back(get_type_for_type<T1>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T2>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T3>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T4>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T5>());
  if (!params.back()) {
    return (eWNBadType);
  }
  void* ptr = NULL;
  if (ok !=
      (err = get_function_pointer(_file, _functionName, t, params, ptr))) {
    return (err);
  }
  _outVal.Assign(reinterpret_cast<R (*)(T1, T2, T3, T4, T5)>(ptr));
  _outVal.SetOutType(t);
  return (ok);
}

template <typename R, typename T1, typename T2, typename T3, typename T4,
    typename T5, typename T6>
eWNTypeError WNScriptingEngine::get_pointer_to_function(const char* _file,
    const char* _functionName,
    WNFunctionPointer<R, T1, T2, T3, T4, T5, T6>& _outVal) {
  eWNTypeError err = compile_file(_file);
  if (err != ok) {
    return (err);
  }

  WNScriptType t = get_type_for_type<R>();
  if (!t) {
    return (eWNBadType);
  }

  std::vector<WNScriptType> params;
  params.push_back(get_type_for_type<T1>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T2>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T3>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T4>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T5>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T6>());
  if (!params.back()) {
    return (eWNBadType);
  }
  void* ptr = NULL;
  if (ok !=
      (err = get_function_pointer(_file, _functionName, t, params, ptr))) {
    return (err);
  }
  _outVal.Assign(reinterpret_cast<R (*)(T1, T2, T3, T4, T5, T6)>(ptr));
  _outVal.SetOutType(t);
  return (ok);
}

template <typename T, typename R, R (T::*P)()>
eWNTypeError WNScriptingEngine::register_cpp_memberFunction(const char* _name) {
  WNScriptType ret = get_type_for_type<R>();
  WNScriptType objectType = get_type_for_type<T>();
  if (!ret) {
    return (eWNBadType);
  }

  std::vector<WNScriptType> params;
  eWNTypeError err;

  if (ok != (err = register_member_function(
                 _name, objectType, ret, params,
                 reinterpret_cast<void*>(
                     &ObjectMethodStruct<T, R, P>::ObjectMethod)))) {
    return (err);
  }
  return (ok);
}

template <typename T, typename R, typename T1, R (T::*P)(T1)>
eWNTypeError WNScriptingEngine::register_cpp_memberFunction(const char* _name) {
  WNScriptType ret = get_type_for_type<R>();
  WNScriptType objectType = get_type_for_type<T>();
  if (!ret) {
    return (eWNBadType);
  }

  std::vector<WNScriptType> params;
  eWNTypeError err;
  params.push_back(get_type_for_type<T1>());
  WN_STATIC_ASSERT(TypeMapping<T1>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<R>::RawInputType);
  if (!params.back()) {
    return (eWNBadType);
  }

  if (ok != (err = register_member_function(
                 _name, objectType, ret, params,
                 reinterpret_cast<void*>(
                     &ObjectMethodStruct1<T, R, T1, P>::ObjectMethod)))) {
    return (err);
  }
  return (ok);
}

template <typename T, typename R, typename T1, typename T2, R (T::*P)(T1, T2)>
eWNTypeError WNScriptingEngine::register_cpp_memberFunction(const char* _name) {
  WNScriptType ret = get_type_for_type<R>();
  WNScriptType objectType = get_type_for_type<T>();
  if (!ret) {
    return (eWNBadType);
  }

  WN_STATIC_ASSERT(TypeMapping<T1>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T2>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<R>::RawInputType);
  std::vector<WNScriptType> params;
  eWNTypeError err;
  params.push_back(get_type_for_type<T1>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T2>());
  if (!params.back()) {
    return (eWNBadType);
  }
  if (ok != (err = register_member_function(
                 _name, objectType, ret, params,
                 reinterpret_cast<void*>(
                     &ObjectMethodStruct2<T, R, T1, T2, P>::ObjectMethod)))) {
    return (err);
  }
  return (ok);
}
template <typename T, typename R, typename T1, typename T2, typename T3,
    R (T::*P)(T1, T2, T3)>
eWNTypeError WNScriptingEngine::register_cpp_memberFunction(const char* _name) {
  WNScriptType ret = get_type_for_type<R>();
  WNScriptType objectType = get_type_for_type<T>();
  if (!ret) {
    return (eWNBadType);
  }

  WN_STATIC_ASSERT(TypeMapping<T1>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T2>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T3>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<R>::RawInputType);

  std::vector<WNScriptType> params;
  eWNTypeError err;
  params.push_back(get_type_for_type<T1>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T2>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T3>());
  if (!params.back()) {
    return (eWNBadType);
  }
  if (ok !=
      (err = register_member_function(
           _name, objectType, ret, params,
           reinterpret_cast<void*>(
               &ObjectMethodStruct3<T, R, T1, T2, T3, P>::ObjectMethod)))) {
    return (err);
  }
  return (ok);
}
template <typename T, typename R, typename T1, typename T2, typename T3,
    typename T4, R (T::*P)(T1, T2, T3, T4)>
eWNTypeError WNScriptingEngine::register_cpp_memberFunction(const char* _name) {
  WNScriptType ret = get_type_for_type<R>();
  WNScriptType objectType = get_type_for_type<T>();
  if (!ret) {
    return (eWNBadType);
  }
  WN_STATIC_ASSERT(TypeMapping<T1>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T2>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T3>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T4>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<R>::RawInputType);

  std::vector<WNScriptType> params;
  eWNTypeError err;
  params.push_back(get_type_for_type<T1>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T2>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T3>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T4>());
  if (!params.back()) {
    return (eWNBadType);
  }
  if (ok !=
      (err = register_member_function(
           _name, objectType, ret, params,
           reinterpret_cast<void*>(
               &ObjectMethodStruct4<T, R, T1, T2, T3, T4, P>::ObjectMethod)))) {
    return (err);
  }
  return (ok);
}
template <typename T, typename R, typename T1, typename T2, typename T3,
    typename T4, typename T5, R (T::*P)(T1, T2, T3, T4, T5)>
eWNTypeError WNScriptingEngine::register_cpp_memberFunction(const char* _name) {
  WNScriptType ret = get_type_for_type<R>();
  WNScriptType objectType = get_type_for_type<T>();
  if (!ret) {
    return (eWNBadType);
  }
  WN_STATIC_ASSERT(TypeMapping<T1>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T2>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T3>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T4>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T5>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<R>::RawInputType);

  std::vector<WNScriptType> params;
  eWNTypeError err;
  params.push_back(get_type_for_type<T1>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T2>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T3>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T4>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T5>());
  if (!params.back()) {
    return (eWNBadType);
  }
  if (ok != (err = register_member_function(_name, objectType, ret, params,
                 reinterpret_cast<void*>(&ObjectMethodStruct5<T, R, T1, T2, T3,
                                                T4, T5, P>::ObjectMethod)))) {
    return (err);
  }
  return (ok);
}
template <typename T, typename R, typename T1, typename T2, typename T3,
    typename T4, typename T5, typename T6, R (T::*P)(T1, T2, T3, T4, T5, T6)>
eWNTypeError WNScriptingEngine::register_cpp_memberFunction(const char* _name) {
  WNScriptType ret = get_type_for_type<R>();
  WNScriptType objectType = get_type_for_type<T>();
  if (!ret) {
    return (eWNBadType);
  }
  WN_STATIC_ASSERT(TypeMapping<T1>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T2>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T3>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T4>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T5>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T6>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<R>::RawInputType);

  std::vector<WNScriptType> params;
  eWNTypeError err;
  params.push_back(get_type_for_type<T1>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T2>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T3>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T4>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T5>());
  if (!params.back()) {
    return (eWNBadType);
  }
  params.push_back(get_type_for_type<T6>());
  if (!params.back()) {
    return (eWNBadType);
  }
  if (ok !=
      (err = register_member_function(_name, objectType, ret, params,
           reinterpret_cast<void*>(&ObjectMethodStruct6<T, R, T1, T2, T3, T4,
                                          T5, T6, P>::ObjectMethod)))) {
    return (err);
  }
  return (ok);
}

template <typename R>
eWNTypeError WNScriptingEngine::register_c_function(
    const char* _name, R (*_ptr)()) {
  WNScriptType t = get_type_for_type<R>();
  if (!t) {
    return (eWNBadType);
  }
  std::vector<WNScriptType> params;

  eWNTypeError err;
  if (ok != (err = register_function(
                 _name, t, params, reinterpret_cast<void*>(_ptr)))) {
    return (err);
  }
  return (ok);
}

template <typename R, typename T1>
eWNTypeError WNScriptingEngine::register_c_function(
    const char* _name, R (*_ptr)(T1)) {
  WNScriptType t = get_type_for_type<R>();
  if (!t) {
    return (eWNBadType);
  }
  std::vector<WNScriptType> params;

  static_assert(TypeMapping<T1>::RawInputType,
      WN_STRINGERIZE(TypeMapping<T1>::RawInputType));
  static_assert(TypeMapping<R>::RawInputType,
      WN_STRINGERIZE(TypeMapping<R>::RawInputType));
  params.push_back(get_type_for_type<T1>());
  if (!params.back()) {
    return (eWNBadType);
  }

  eWNTypeError err;
  if (ok != (err = register_function(
                 _name, t, params, reinterpret_cast<void*>(_ptr)))) {
    return (err);
  }
  return (ok);
}

template <typename R, typename T1, typename T2>
eWNTypeError WNScriptingEngine::register_c_function(
    const char* _name, R (*_ptr)(T1, T2)) {
  WNScriptType t = get_type_for_type<R>();
  if (!t) {
    return (eWNBadType);
  }
  std::vector<WNScriptType> params;
  static_assert(TypeMapping<T1>::RawInputType,
      WN_STRINGERIZE(TypeMapping<T1>::RawInputType));
  static_assert(TypeMapping<T2>::RawInputType,
      WN_STRINGERIZE(TypeMapping<T2>::RawInputType));
  static_assert(TypeMapping<R>::RawInputType,
      WN_STRINGERIZE(TypeMapping<R>::RawInputType));

  params.push_back(get_type_for_type<T1>());
  if (!params.back()) {
    return (eWNBadType);
  }

  params.push_back(get_type_for_type<T2>());
  if (!params.back()) {
    return (eWNBadType);
  }

  eWNTypeError err;
  if (ok != (err = register_function(
                 _name, t, params, reinterpret_cast<void*>(_ptr)))) {
    return (err);
  }
  return (ok);
}

template <typename R, typename T1, typename T2, typename T3>
eWNTypeError WNScriptingEngine::register_c_function(
    const char* _name, R (*_ptr)(T1, T2, T3)) {
  WNScriptType t = get_type_for_type<R>();
  if (!t) {
    return (eWNBadType);
  }
  std::vector<WNScriptType> params;

  WN_STATIC_ASSERT(TypeMapping<T1>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T2>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T3>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<R>::RawInputType);
  params.push_back(get_type_for_type<T1>());
  if (!params.back()) {
    return (eWNBadType);
  }

  params.push_back(get_type_for_type<T2>());
  if (!params.back()) {
    return (eWNBadType);
  }

  params.push_back(get_type_for_type<T3>());
  if (!params.back()) {
    return (eWNBadType);
  }

  eWNTypeError err;
  if (ok != (err = register_function(
                 _name, t, params, reinterpret_cast<void*>(_ptr)))) {
    return (err);
  }
  return (ok);
}

template <typename R, typename T1, typename T2, typename T3, typename T4>
eWNTypeError WNScriptingEngine::register_c_function(
    const char* _name, R (*_ptr)(T1, T2, T3, T4)) {
  WNScriptType t = get_type_for_type<R>();
  if (!t) {
    return (eWNBadType);
  }
  std::vector<WNScriptType> params;

  WN_STATIC_ASSERT(TypeMapping<T1>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T2>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T3>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T4>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<R>::RawInputType);
  params.push_back(get_type_for_type<T1>());
  if (!params.back()) {
    return (eWNBadType);
  }

  params.push_back(get_type_for_type<T2>());
  if (!params.back()) {
    return (eWNBadType);
  }

  params.push_back(get_type_for_type<T3>());
  if (!params.back()) {
    return (eWNBadType);
  }

  params.push_back(get_type_for_type<T4>());
  if (!params.back()) {
    return (eWNBadType);
  }

  eWNTypeError err;
  if (ok != (err = register_function(
                 _name, t, params, reinterpret_cast<void*>(_ptr)))) {
    return (err);
  }
  return (ok);
}

template <typename R, typename T1, typename T2, typename T3, typename T4,
    typename T5>
eWNTypeError WNScriptingEngine::register_c_function(
    const char* _name, R (*_ptr)(T1, T2, T3, T4, T5)) {
  WNScriptType t = get_type_for_type<R>();
  if (!t) {
    return (eWNBadType);
  }
  std::vector<WNScriptType> params;

  WN_STATIC_ASSERT(TypeMapping<T1>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T2>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T3>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T4>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T5>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<R>::RawInputType);
  params.push_back(get_type_for_type<T1>());
  if (!params.back()) {
    return (eWNBadType);
  }

  params.push_back(get_type_for_type<T2>());
  if (!params.back()) {
    return (eWNBadType);
  }

  params.push_back(get_type_for_type<T3>());
  if (!params.back()) {
    return (eWNBadType);
  }

  params.push_back(get_type_for_type<T4>());
  if (!params.back()) {
    return (eWNBadType);
  }

  params.push_back(get_type_for_type<T5>());
  if (!params.back()) {
    return (eWNBadType);
  }

  eWNTypeError err;
  if (ok != (err = register_function(
                 _name, t, params, reinterpret_cast<void*>(_ptr)))) {
    return (err);
  }
  return (ok);
}

template <typename R, typename T1, typename T2, typename T3, typename T4,
    typename T5, typename T6>
eWNTypeError WNScriptingEngine::register_c_function(
    const char* _name, R (*_ptr)(T1, T2, T3, T4, T5, T6)) {
  WNScriptType t = get_type_for_type<R>();
  if (!t) {
    return (eWNBadType);
  }
  std::vector<WNScriptType> params;

  WN_STATIC_ASSERT(TypeMapping<T1>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T2>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T3>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T4>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T5>::RawInputType);
  WN_STATIC_ASSERT(TypeMapping<T6>::RawInputType);

  params.push_back(get_type_for_type<T1>());
  if (!params.back()) {
    return (eWNBadType);
  }

  params.push_back(get_type_for_type<T2>());
  if (!params.back()) {
    return (eWNBadType);
  }

  params.push_back(get_type_for_type<T3>());
  if (!params.back()) {
    return (eWNBadType);
  }

  params.push_back(get_type_for_type<T4>());
  if (!params.back()) {
    return (eWNBadType);
  }

  params.push_back(get_type_for_type<T5>());
  if (!params.back()) {
    return (eWNBadType);
  }

  params.push_back(get_type_for_type<T6>());
  if (!params.back()) {
    return (eWNBadType);
  }

  eWNTypeError err;
  if (ok != (err = register_function(
                 _name, t, params, reinterpret_cast<void*>(_ptr)))) {
    return (err);
  }
  return (ok);
}
}
#include "WNDeprecatedScripting/inc/WNScriptingInterop.h"
#endif
