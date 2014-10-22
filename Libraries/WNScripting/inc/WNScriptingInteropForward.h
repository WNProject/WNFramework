// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_SCRIPTING_INTEROP_FORWARD_H__
#define __WN_SCRIPTING_INTEROP_FORWARD_H__
namespace WNScripting {
    template<int i = 0>
    struct DummyType {//this is so we can generate dummy functions to cast up the chain
        typedef DummyType<i+1> ParentType;
    };
    template<typename T1, typename T2>
    class WNScriptingObject;
    template<typename T>
    class WNScriptingArray;
    template<typename T, typename R, typename T1=DummyType<0>, typename T2=DummyType<0>, typename T3=DummyType<0>, typename T4=DummyType<0>, typename T5=DummyType<0>, typename T6=DummyType<0>>
    class WNMemberFunctionPointer;
    template<typename R, typename T1=DummyType<0>, typename T2=DummyType<0>, typename T3=DummyType<0>, typename T4=DummyType<0>, typename T5=DummyType<0>, typename T6=DummyType<0>>
    class WNFunctionPointer;
    struct StructInternalType {
        void* owner;
        void* structLoc;
        WN_SIZE_T refCount;
    };
    template<typename InType> struct InCaster;
    template<typename OutType> struct OutCaster;
    
   template<typename T>
    struct TypeMapping {
        static const WN_CHAR * GetTypeName() { WN_STATIC_ASSERT(WN_STATIC_ASSERT_DEPENDENT_FAIL<T>::Value); } 
    };
  
}

#endif//__WN_SCRIPTING_INTEROP_FORWARD_H__
