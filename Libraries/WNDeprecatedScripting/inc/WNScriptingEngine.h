// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_ENGINE_H__
#define __WN_SCRIPTING_ENGINE_H__

#include <vector>
#include "WNContainers/inc/WNFunction.h"
#include "WNCore/inc/WNTypes.h"
#include "WNDeprecatedScripting/inc/WNScriptingErrors.h"
#include "WNDeprecatedScripting/inc/WNScriptingInteropForward.h"
#include "WNDeprecatedScripting/inc/WNTypeElement.h"
#include "WNLogging/inc/WNLog.h"
namespace WNScripting {
struct WNParameter;

typedef wn::containers::function<void(
    const char*, size_t, const std::vector<WNLogging::WNLogColorElement>&)>
    WNScriptingLogCallback;

class WNScriptingEngine {
public:
  virtual ~WNScriptingEngine() {}
  virtual eWNTypeError get_function_pointer(const char* _file,
      const char* _functionName, WNScriptType& _retParam,
      const std::vector<WNScriptType>& _params, void*& _ptr) = 0;
  virtual int32_t get_virtual_function_index(const char* _functionName,
      WNScriptType& _type, const std::vector<WNScriptType>& _params) = 0;
  virtual eWNTypeError initialize() = 0;
  virtual WNScriptType get_type_by_name(const char* _typeName) const = 0;
  virtual WNScriptType get_array_of(WNScriptType _type) = 0;
  virtual WNScriptType get_existing_array_of(WNScriptType _type) const = 0;
  virtual eWNTypeError register_function(const char* _functionName,
      const WNScriptType _retParam, const std::vector<WNScriptType>& _params,
      void* _ptr) = 0;
  virtual eWNTypeError register_member_function(const char* _functionName,
      const WNScriptType _thisType, const WNScriptType _retParam,
      const std::vector<WNScriptType>& _params, void* _ptr) = 0;
  virtual eWNTypeError register_member(const char* _varName,
      const WNScriptType _thisType, const WNScriptType _varType,
      size_t _offset) = 0;
  virtual void* get_registered_function(const char* _functionName) const = 0;
  virtual void set_internal_log_level(WNLogging::WNLogLevel) = 0;
  virtual void set_log_level(WNLogging::WNLogLevel) = 0;
  virtual void set_compilation_log(WNLogging::WNLog*) = 0;
  virtual eWNTypeError register_external_type(
      const char* _typeName, void (*ptr)(void*)) = 0;
  virtual eWNTypeError register_external_type(
      const char* _typeName, const char* _parentType, void (*ptr)(void*)) = 0;
  virtual eWNTypeError compile_file(const char* _file) = 0;
  virtual eWNTypeError construct_scripting_object(
      WNScriptType _type, void*& _retVal) const = 0;
  virtual eWNTypeError construct_scripting_array(
      WNScriptType _type, size_t _size, void*& _retVal) const = 0;

  template <typename T1, typename T2>
  eWNTypeError construct_scripting_object(WNScriptingObject<T1, T2>& _outType);
  template <typename T>
  eWNTypeError construct_scripting_array(
      WNScriptingArray<T>& _array, size_t _size);
  template <typename T>
  eWNTypeError register_cpp_Type();
  template <typename T, typename TParent>
  eWNTypeError register_cpp_Type();
  template <typename T, typename PT, PT T::*V>
  eWNTypeError register_cpp_member(const char*);

  template <typename R>
  eWNTypeError get_pointer_to_function(
      const char* _file, const char* _functionName, WNFunctionPointer<R>&);
  template <typename R, typename T1>
  eWNTypeError get_pointer_to_function(
      const char* _file, const char* _functionName, WNFunctionPointer<R, T1>&);
  template <typename R, typename T1, typename T2>
  eWNTypeError get_pointer_to_function(const char* _file,
      const char* _functionName, WNFunctionPointer<R, T1, T2>&);
  template <typename R, typename T1, typename T2, typename T3>
  eWNTypeError get_pointer_to_function(const char* _file,
      const char* _functionName, WNFunctionPointer<R, T1, T2, T3>&);
  template <typename R, typename T1, typename T2, typename T3, typename T4>
  eWNTypeError get_pointer_to_function(const char* _file,
      const char* _functionName, WNFunctionPointer<R, T1, T2, T3, T4>&);
  template <typename R, typename T1, typename T2, typename T3, typename T4,
      typename T5>
  eWNTypeError get_pointer_to_function(const char* _file,
      const char* _functionName, WNFunctionPointer<R, T1, T2, T3, T4, T5>&);
  template <typename R, typename T1, typename T2, typename T3, typename T4,
      typename T5, typename T6>
  eWNTypeError get_pointer_to_function(const char* _file,
      const char* _functionName, WNFunctionPointer<R, T1, T2, T3, T4, T5, T6>&);

  template <typename T, typename R>
  eWNTypeError get_pointer_to_member_function(const char* _file,
      const char* _functionName, WNMemberFunctionPointer<T, R>& _outVal);

  template <typename R>
  eWNTypeError register_c_function(const char* _name, R (*_ptr)());
  template <typename R, typename T1>
  eWNTypeError register_c_function(const char* _name, R (*_ptr)(T1));
  template <typename R, typename T1, typename T2>
  eWNTypeError register_c_function(const char* _name, R (*_ptr)(T1, T2));
  template <typename R, typename T1, typename T2, typename T3>
  eWNTypeError register_c_function(const char* _name, R (*_ptr)(T1, T2, T3));
  template <typename R, typename T1, typename T2, typename T3, typename T4>
  eWNTypeError register_c_function(
      const char* _name, R (*_ptr)(T1, T2, T3, T4));
  template <typename R, typename T1, typename T2, typename T3, typename T4,
      typename T5>
  eWNTypeError register_c_function(
      const char* _name, R (*_ptr)(T1, T2, T3, T4, T5));
  template <typename R, typename T1, typename T2, typename T3, typename T4,
      typename T5, typename T6>
  eWNTypeError register_c_function(
      const char* _name, R (*_ptr)(T1, T2, T3, T4, T5, T6));

  template <typename T, typename R, R (T::*P)()>
  eWNTypeError register_cpp_memberFunction(const char* _name);
  template <typename T, typename R, typename T1, R (T::*P)(T1)>
  eWNTypeError register_cpp_memberFunction(const char* _name);
  template <typename T, typename R, typename T1, typename T2, R (T::*P)(T1, T2)>
  eWNTypeError register_cpp_memberFunction(const char* _name);
  template <typename T, typename R, typename T1, typename T2, typename T3,
      R (T::*P)(T1, T2, T3)>
  eWNTypeError register_cpp_memberFunction(const char* _name);
  template <typename T, typename R, typename T1, typename T2, typename T3,
      typename T4, R (T::*P)(T1, T2, T3, T4)>
  eWNTypeError register_cpp_memberFunction(const char* _name);
  template <typename T, typename R, typename T1, typename T2, typename T3,
      typename T4, typename T5, R (T::*P)(T1, T2, T3, T4, T5)>
  eWNTypeError register_cpp_memberFunction(const char* _name);
  template <typename T, typename R, typename T1, typename T2, typename T3,
      typename T4, typename T5, typename T6, R (T::*P)(T1, T2, T3, T4, T5, T6)>
  eWNTypeError register_cpp_memberFunction(const char* _name);
  template <typename T, typename R, typename T1, typename T2, typename T3,
      typename T4, typename T5, typename T6, typename T7,
      R (T::*P)(T1, T2, T3, T4, T5, T6, T7)>
  eWNTypeError register_cpp_memberFunction(const char* _name);

private:
  template <typename T>
  WNScriptType get_type_for_type();
};
}
#include "WNFunctionTemplates.inl"
#endif  //__WN_SCRIPTING_ENGINE_H__
