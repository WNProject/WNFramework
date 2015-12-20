// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNAssert.h"
#include "WNEntryPoint/inc/WNEntry.h"
#include "WNMemory/inc/WNStringUtility.h"
#include "WNDeprecatedScripting/inc/WNScriptingEngine.h"
#include "WNDeprecatedScripting/inc/WNScriptingEngineFactory.h"
#include "WNDeprecatedScripting/inc/WNParameter.h"
#include <iostream>

#include "WNLogging/inc/WNBufferLogger.h"
#include "WNLogging/inc/WNConsoleLogger.h"
#include "WNLogging/inc/WNFileLogger.h"

WNScripting::WNScriptingEngine* g_Engine;

wn_char* getLine(wn_char*_line, wn_uint32 _sz){
   if (fgets(_line, _sz, stdin)) {
      wn_char *current = wn::memory::strchr(_line, '\n'); /* check for trailing '\n' */

      if (current != wn_nullptr) {
         *current = '\0'; /* overwrite the '\n' with a terminating null */
      }
   }

   return(_line);
}

wn_int32 get23() {
    return(23);
}

wn_int32 add(wn_int32 x, wn_int32 y) {
    return(x + y);
}

wn_void print(wn_int32 _val) {
    printf("%d\n", _val);
}

DEFINE_TYPED_SCRIPT_OBJECT(A);
DEFINE_TYPED_SCRIPT_CHILD(B, A);
DEFINE_TYPED_SCRIPT_CHILD(C, B);
struct X {
    virtual ~X() {
        printf("DESTROYED");
    }
    virtual wn_int32 XRet() {
        return(35);
    }
    int x;
};

struct Y : public X{
    wn_int32 xFunc2(wn_int32 in) {
        return(in + 10);
    }
    int y;
};
DEFINE_CPP_TYPE(X);
DEFINE_CPP_TYPE_CHILD(Y, X);

struct Foo {
    WNScripting::ScriptPointer_X spX;
};

wn_void PrintString(WNScripting::WNScriptingArrayRef<wn_char>* mArray) {
    WNScripting::WNScriptingArray<wn_char> inArray(mArray);
    printf("%*s", static_cast<wn_int32>(inArray.GetLength()), &(inArray[0]));
}

WNScripting::WNScriptingArrayRef<wn_char>* GetHelloWorld() {
    WNScripting::WNScriptingArray<wn_char> newArray;
    const char* hw = "hello world";
    wn_size_t strLen = 12;
    g_Engine->construct_scripting_array(newArray, strLen);
    memcpy(&newArray[0], hw, strLen);
    return(newArray.ReleaseOwnership().DetachOut());
}

DEFINE_CPP_TYPE(Foo);

wn_int32 wn_main(wn_int32 _argc, wn_char* _argv[]) {
    wn_dummy();
    wn_char tests[1024];

    WNLogging::WNConsoleLogger<> mConsoleLogger;
    WNLogging::WNLog mMyLog(&mConsoleLogger, WNLogging::eLogMax, 1024, true);


#ifdef _WN_ANDROID
    wn::memory::strcpy(tests, "/sdcard/MCJitTest.wns" );
    const wn_char* func = "test";
    const wn_char* func2 = "test2";
#else

    if (_argc < 2) {
        ::printf("Enter Filename >>");

        getLine(tests, 1023);

        return(-1);
    } else {
        wn::memory::strcpy(tests, _argv[1]);
    }

    const wn_char* func = (_argc > 2) ? _argv[2] : "test";

#endif
    WNScripting::WNScriptingEngine* scriptingEngine = wn::scripting::scripting_engine_factory::create_scripting_engine(wn::scripting::factory_type::deprecated_engine);
    g_Engine = scriptingEngine;
    scriptingEngine->set_compilation_log(&mMyLog);
    scriptingEngine->set_log_level(WNLogging::eNone);
    scriptingEngine->set_internal_log_level(WNLogging::eNone);
    WN_RELEASE_ASSERT(scriptingEngine->initialize() == ok);
    scriptingEngine->register_c_function("get23", &get23);
    scriptingEngine->register_c_function("add", &add);
    scriptingEngine->register_c_function("printInt", &print);
    scriptingEngine->register_c_function("printString", &PrintString);
    scriptingEngine->register_c_function("hello", &GetHelloWorld);

    WNScripting::WNFunctionPointer<wn_int32, WNScripting::WNScriptingArray<wn_char> > fPtr;
    WNScripting::ScriptType_A  mAType;
    WNScripting::WNScriptingArray<wn_char> mArray;
    if(ok != scriptingEngine->compile_file(tests)) {
        return(-1);
    }
    WNScripting::WNMemberVariablePointer<wn_int32, WNScripting::ScriptType_A> mVar("y", scriptingEngine);
    if(WNScripting::eResolved!= mVar.GetResolution()) {
        return(-1);
    }

    if(ok != scriptingEngine->construct_scripting_object(mAType)) {
        return(-1);
    }

    if(ok != scriptingEngine->construct_scripting_array(mArray, 10)) {
        return(-1);
    }
    for(char i = 0; i < 10; ++i) {
        mArray[i] = i;
    }

    printf("%d", mAType->*mVar);

    if(ok == scriptingEngine->get_pointer_to_function(tests,  func, fPtr)) {
        mMyLog.Log(WNLogging::eInfo, 0, "Got function pointer");
        wn_int32 ch = fPtr(mArray);
        printf("\n\n%d\n\n", ch);
    }
    wn::memory::destroy(scriptingEngine);
    return(0);
}