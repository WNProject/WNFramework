// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#ifndef __WN_SCRIPTING_ENGINE_H__
#define __WN_SCRIPTING_ENGINE_H__

#include "WNCore/inc/WNTypes.h"
#include "WNScripting/inc/WNTypeElement.h"
#include "WNScripting/inc/WNScriptingErrors.h"
#include "WNContainers/inc/WNFunction.h"
#include "WNLogging/inc/WNLog.h"
#include "WNScripting/inc/WNScriptingInteropForward.h"
#include <vector>
namespace WNScripting {
    class WNTypeManager;
    struct WNParameter;

    typedef wn::containers::function<wn_void(const wn_char*, wn_size_t, const std::vector<WNLogging::WNLogColorElement>&)> WNScriptingLogCallback;

    class WNScriptingEngine {
    public:
        virtual ~WNScriptingEngine() {}
        //virtual wn_bool AddExternalFunction(wn_void* _funcPtr, wn_bool _stackRet, const wn_char* _returnType, const wn_char* _name, const wn_char* _arglist) = 0;
        virtual eWNTypeError GetFunctionPointer(const wn_char* _file, const wn_char* _functionName, WNScriptType& _retParam, const std::vector<WNScriptType>& _params, void*& _ptr) = 0;
        virtual wn_int32 GetVirtualFunctionIndex(const wn_char* _functionName, WNScriptType& _type, const std::vector<WNScriptType>& _params) = 0;
        virtual eWNTypeError Initialize() = 0;
        virtual WNScriptType GetTypeByName(const wn_char* _typeName) const = 0;
        virtual WNScriptType GetArrayOf(WNScriptType _type) = 0;
        virtual WNScriptType GetExistingArrayOf(WNScriptType _type) const = 0;
        virtual eWNTypeError RegisterFunction(const wn_char* _functionName, const WNScriptType _retParam, const std::vector<WNScriptType>& _params, void* _ptr) = 0;
        virtual eWNTypeError RegisterMemberFunction(const wn_char* _functionName, const WNScriptType _thisType, const WNScriptType _retParam, const std::vector<WNScriptType>& _params, void* _ptr) = 0;
        virtual eWNTypeError RegisterMember(const wn_char* _varName, const WNScriptType _thisType, const WNScriptType _varType, wn_size_t _offset) = 0;
        virtual void* GetRegisteredFunction(const wn_char* _functionName) const = 0;
        virtual void SetInternalLogLevel(WNLogging::WNLogLevel) = 0;
        virtual void SetLogLevel(WNLogging::WNLogLevel) = 0;
        virtual void SetCompilationLog(WNLogging::WNLog*) = 0;
        virtual eWNTypeError RegisterExternalType(const wn_char* _typeName, void(*ptr)(void*)) = 0;
        virtual eWNTypeError RegisterExternalType(const wn_char* _typeName, const wn_char* _parentType, void(*ptr)(void*)) = 0;
        virtual eWNTypeError CompileFile(const wn_char* _file) = 0;
        virtual eWNTypeError ConstructScriptingObject(WNScriptType _type, wn_void*& _retVal) const = 0;
        virtual eWNTypeError ConstructScriptingArray(WNScriptType _type, wn_size_t _size, wn_void*& _retVal) const = 0;
        virtual eWNTypeError AddExternalLibs(eWNTypeError(*)(WNTypeManager&, void*), void*) = 0;

        template<typename T1, typename T2>
        eWNTypeError ConstructScriptingObject(WNScriptingObject<T1, T2>& _outType);
        template<typename T>
        eWNTypeError ConstructScriptingArray(WNScriptingArray<T>& _array, wn_size_t _size);
        template<typename T>
        eWNTypeError RegisterCPPType();
        template<typename T, typename TParent>
        eWNTypeError RegisterCPPType();
        template<typename T, typename PT, PT T::*V>
        eWNTypeError RegisterCPPMember(const wn_char*);

        template<typename R>
        eWNTypeError GetPointerToFunction(const wn_char* _file, const wn_char* _functionName, WNFunctionPointer<R>&);
        template<typename R, typename T1>
        eWNTypeError GetPointerToFunction(const wn_char* _file, const wn_char* _functionName, WNFunctionPointer<R, T1>&);
        template<typename R, typename T1, typename T2>
        eWNTypeError GetPointerToFunction(const wn_char* _file, const wn_char* _functionName, WNFunctionPointer<R, T1, T2>&);
        template<typename R, typename T1, typename T2, typename T3>
        eWNTypeError GetPointerToFunction(const wn_char* _file, const wn_char* _functionName, WNFunctionPointer<R, T1, T2, T3>&);
        template<typename R, typename T1, typename T2, typename T3, typename T4>
        eWNTypeError GetPointerToFunction(const wn_char* _file, const wn_char* _functionName, WNFunctionPointer<R, T1, T2, T3, T4>&);
        template<typename R, typename T1, typename T2, typename T3, typename T4, typename T5>
        eWNTypeError GetPointerToFunction(const wn_char* _file, const wn_char* _functionName, WNFunctionPointer<R, T1, T2, T3, T4, T5>&);
        template<typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
        eWNTypeError GetPointerToFunction(const wn_char* _file, const wn_char* _functionName, WNFunctionPointer<R, T1, T2, T3, T4, T5, T6>&);

        template<typename T, typename R>
        eWNTypeError GetPointerToMemberFunction(const wn_char* _file, const wn_char* _functionName, WNMemberFunctionPointer<T, R>& _outVal);

        template<typename R>
        eWNTypeError RegisterCFunction(const wn_char* _name, R (*_ptr)());
        template<typename R, typename T1>
        eWNTypeError RegisterCFunction(const wn_char* _name, R (*_ptr)(T1));
        template<typename R, typename T1, typename T2>
        eWNTypeError RegisterCFunction(const wn_char* _name, R (*_ptr)(T1, T2));
        template<typename R, typename T1, typename T2, typename T3>
        eWNTypeError RegisterCFunction(const wn_char* _name, R (*_ptr)(T1, T2, T3));
        template<typename R, typename T1, typename T2, typename T3, typename T4>
        eWNTypeError RegisterCFunction(const wn_char* _name, R (*_ptr)(T1, T2, T3, T4));
        template<typename R, typename T1, typename T2, typename T3, typename T4, typename T5>
        eWNTypeError RegisterCFunction(const wn_char* _name, R (*_ptr)(T1, T2, T3, T4, T5));
        template<typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
        eWNTypeError RegisterCFunction(const wn_char* _name, R (*_ptr)(T1, T2, T3, T4, T5, T6));

        template<typename T, typename R, R (T::*P)()>
        eWNTypeError RegisterCPPMemberFunction(const wn_char* _name);
        template<typename T, typename R, typename T1, R (T::*P)(T1)>
        eWNTypeError RegisterCPPMemberFunction(const wn_char* _name);
        template<typename T, typename R, typename T1, typename T2, R (T::*P)(T1, T2)>
        eWNTypeError RegisterCPPMemberFunction(const wn_char* _name);
        template<typename T, typename R, typename T1, typename T2, typename T3, R (T::*P)(T1, T2, T3)>
        eWNTypeError RegisterCPPMemberFunction(const wn_char* _name);
        template<typename T, typename R, typename T1, typename T2, typename T3, typename T4, R (T::*P)(T1, T2, T3, T4)>
        eWNTypeError RegisterCPPMemberFunction(const wn_char* _name);
        template<typename T, typename R, typename T1, typename T2, typename T3, typename T4, typename T5, R (T::*P)(T1, T2, T3, T4, T5)>
        eWNTypeError RegisterCPPMemberFunction(const wn_char* _name);
        template<typename T, typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, R (T::*P)(T1, T2, T3, T4, T5, T6)>
        eWNTypeError RegisterCPPMemberFunction(const wn_char* _name);
        template<typename T, typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, R (T::*P)(T1, T2, T3, T4, T5, T6, T7)>
        eWNTypeError RegisterCPPMemberFunction(const wn_char* _name);
    private:
        template<typename T>
        WNScriptType GetTypeForType();
    };
}
#include "WNFunctionTemplates.inl"
#endif//__WN_SCRIPTING_ENGINE_H__
