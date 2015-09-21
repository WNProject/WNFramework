// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.


#ifndef __WN_SCRIPTING_ENGINE_H__
#define __WN_SCRIPTING_ENGINE_H__

#include "WNCore/inc/WNTypes.h"
#include "WNDeprecatedScripting/inc/WNTypeElement.h"
#include "WNDeprecatedScripting/inc/WNScriptingErrors.h"
#include "WNContainers/inc/WNFunction.h"
#include "WNLogging/inc/WNLog.h"
#include "WNDeprecatedScripting/inc/WNScriptingInteropForward.h"
#include <vector>
namespace WNScripting {
    struct WNParameter;

    typedef wn::containers::function<wn_void(const wn_char*, wn_size_t, const std::vector<WNLogging::WNLogColorElement>&)> WNScriptingLogCallback;

    class WNScriptingEngine {
    public:
        virtual ~WNScriptingEngine() {}
        virtual eWNTypeError get_function_pointer(const wn_char* _file, const wn_char* _functionName, WNScriptType& _retParam, const std::vector<WNScriptType>& _params, void*& _ptr) = 0;
        virtual wn_int32 get_virtual_function_index(const wn_char* _functionName, WNScriptType& _type, const std::vector<WNScriptType>& _params) = 0;
        virtual eWNTypeError initialize() = 0;
        virtual WNScriptType get_type_by_name(const wn_char* _typeName) const = 0;
        virtual WNScriptType get_array_of(WNScriptType _type) = 0;
        virtual WNScriptType get_existing_array_of(WNScriptType _type) const = 0;
        virtual eWNTypeError register_function(const wn_char* _functionName, const WNScriptType _retParam, const std::vector<WNScriptType>& _params, void* _ptr) = 0;
        virtual eWNTypeError register_member_function(const wn_char* _functionName, const WNScriptType _thisType, const WNScriptType _retParam, const std::vector<WNScriptType>& _params, void* _ptr) = 0;
        virtual eWNTypeError register_member(const wn_char* _varName, const WNScriptType _thisType, const WNScriptType _varType, wn_size_t _offset) = 0;
        virtual void* get_registered_function(const wn_char* _functionName) const = 0;
        virtual void set_internal_log_level(WNLogging::WNLogLevel) = 0;
        virtual void set_log_level(WNLogging::WNLogLevel) = 0;
        virtual void set_compilation_log(WNLogging::WNLog*) = 0;
        virtual eWNTypeError register_external_type(const wn_char* _typeName, void(*ptr)(void*)) = 0;
        virtual eWNTypeError register_external_type(const wn_char* _typeName, const wn_char* _parentType, void(*ptr)(void*)) = 0;
        virtual eWNTypeError compile_file(const wn_char* _file) = 0;
        virtual eWNTypeError construct_scripting_object(WNScriptType _type, wn_void*& _retVal) const = 0;
        virtual eWNTypeError construct_scripting_array(WNScriptType _type, wn_size_t _size, wn_void*& _retVal) const = 0;

        template<typename T1, typename T2>
        eWNTypeError construct_scripting_object(WNScriptingObject<T1, T2>& _outType);
        template<typename T>
        eWNTypeError construct_scripting_array(WNScriptingArray<T>& _array, wn_size_t _size);
        template<typename T>
        eWNTypeError register_cpp_Type();
        template<typename T, typename TParent>
        eWNTypeError register_cpp_Type();
        template<typename T, typename PT, PT T::*V>
        eWNTypeError register_cpp_member(const wn_char*);

        template<typename R>
        eWNTypeError get_pointer_to_function(const wn_char* _file, const wn_char* _functionName, WNFunctionPointer<R>&);
        template<typename R, typename T1>
        eWNTypeError get_pointer_to_function(const wn_char* _file, const wn_char* _functionName, WNFunctionPointer<R, T1>&);
        template<typename R, typename T1, typename T2>
        eWNTypeError get_pointer_to_function(const wn_char* _file, const wn_char* _functionName, WNFunctionPointer<R, T1, T2>&);
        template<typename R, typename T1, typename T2, typename T3>
        eWNTypeError get_pointer_to_function(const wn_char* _file, const wn_char* _functionName, WNFunctionPointer<R, T1, T2, T3>&);
        template<typename R, typename T1, typename T2, typename T3, typename T4>
        eWNTypeError get_pointer_to_function(const wn_char* _file, const wn_char* _functionName, WNFunctionPointer<R, T1, T2, T3, T4>&);
        template<typename R, typename T1, typename T2, typename T3, typename T4, typename T5>
        eWNTypeError get_pointer_to_function(const wn_char* _file, const wn_char* _functionName, WNFunctionPointer<R, T1, T2, T3, T4, T5>&);
        template<typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
        eWNTypeError get_pointer_to_function(const wn_char* _file, const wn_char* _functionName, WNFunctionPointer<R, T1, T2, T3, T4, T5, T6>&);

        template<typename T, typename R>
        eWNTypeError get_pointer_to_member_function(const wn_char* _file, const wn_char* _functionName, WNMemberFunctionPointer<T, R>& _outVal);

        template<typename R>
        eWNTypeError register_c_function(const wn_char* _name, R (*_ptr)());
        template<typename R, typename T1>
        eWNTypeError register_c_function(const wn_char* _name, R (*_ptr)(T1));
        template<typename R, typename T1, typename T2>
        eWNTypeError register_c_function(const wn_char* _name, R (*_ptr)(T1, T2));
        template<typename R, typename T1, typename T2, typename T3>
        eWNTypeError register_c_function(const wn_char* _name, R (*_ptr)(T1, T2, T3));
        template<typename R, typename T1, typename T2, typename T3, typename T4>
        eWNTypeError register_c_function(const wn_char* _name, R (*_ptr)(T1, T2, T3, T4));
        template<typename R, typename T1, typename T2, typename T3, typename T4, typename T5>
        eWNTypeError register_c_function(const wn_char* _name, R (*_ptr)(T1, T2, T3, T4, T5));
        template<typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
        eWNTypeError register_c_function(const wn_char* _name, R (*_ptr)(T1, T2, T3, T4, T5, T6));

        template<typename T, typename R, R (T::*P)()>
        eWNTypeError register_cpp_memberFunction(const wn_char* _name);
        template<typename T, typename R, typename T1, R (T::*P)(T1)>
        eWNTypeError register_cpp_memberFunction(const wn_char* _name);
        template<typename T, typename R, typename T1, typename T2, R (T::*P)(T1, T2)>
        eWNTypeError register_cpp_memberFunction(const wn_char* _name);
        template<typename T, typename R, typename T1, typename T2, typename T3, R (T::*P)(T1, T2, T3)>
        eWNTypeError register_cpp_memberFunction(const wn_char* _name);
        template<typename T, typename R, typename T1, typename T2, typename T3, typename T4, R (T::*P)(T1, T2, T3, T4)>
        eWNTypeError register_cpp_memberFunction(const wn_char* _name);
        template<typename T, typename R, typename T1, typename T2, typename T3, typename T4, typename T5, R (T::*P)(T1, T2, T3, T4, T5)>
        eWNTypeError register_cpp_memberFunction(const wn_char* _name);
        template<typename T, typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, R (T::*P)(T1, T2, T3, T4, T5, T6)>
        eWNTypeError register_cpp_memberFunction(const wn_char* _name);
        template<typename T, typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, R (T::*P)(T1, T2, T3, T4, T5, T6, T7)>
        eWNTypeError register_cpp_memberFunction(const wn_char* _name);
    private:
        template<typename T>
        WNScriptType get_type_for_type();
    };
}
#include "WNFunctionTemplates.inl"
#endif//__WN_SCRIPTING_ENGINE_H__
