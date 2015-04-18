// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNAssert.h"
#include "WNMemory/inc/WNMemory.h"
#include "WNStrings/inc/WNStrings.h"
#include "WNScripting/inc/WNScriptingEngine.h"
#include "WNScripting/inc/WNScriptingEngineFactory.h"
#include "WNScripting/inc/WNParameter.h"
#include <iostream>

#include "WNLogging/inc/WNBufferLogger.h"
#include "WNLogging/inc/WNConsoleLogger.h"
#include "WNLogging/inc/WNFileLogger.h"


using namespace WNStrings;
using namespace WNMemory;

//extern eWNTypeError RegisterExternalString(WNScripting::WNTypeManager& _manager, WN_VOID*);

WNLogging::WNDefaultLogParameters<WNLogging::eLogMax, 1024, true> mParams;
WNLogging::WNConsoleLogger<> mConsoleLogger;
static WNLogging::WNLog mMyLog(&mConsoleLogger, mParams);

WNScripting::WNScriptingEngine* g_Engine;

WN_CHAR* getLine(WN_CHAR*_line, WN_UINT32 _sz){
   if (fgets(_line, _sz, stdin)) {
      WN_CHAR *current = WNStrChr(_line, '\n'); /* check for trailing '\n' */

      if (current != WN_NULL) {
         *current = '\0'; /* overwrite the '\n' with a terminating null */
      }
   }

   return(_line);
}
WN_INT32 get23() {
    return(23);
}

WN_INT32 add(WN_INT32 x, WN_INT32 y) {
    return(x + y);
}

WN_VOID print(WN_INT32 _val) {
    printf("%d\n", _val);
}

DEFINE_TYPED_SCRIPT_OBJECT(A);
DEFINE_TYPED_SCRIPT_CHILD(B, A);
DEFINE_TYPED_SCRIPT_CHILD(C, B);
struct X {
    virtual ~X() {
        printf("DESTROYED");
    }
    virtual WN_INT32 XRet() {
        return(35);
    }
    int x;
};

struct Y : public X{
    WN_INT32 xFunc2(WN_INT32 in) {
        return(in + 10);
    }
    int y;
};
DEFINE_CPP_TYPE(X);
DEFINE_CPP_TYPE_CHILD(Y, X);

struct Foo {
    WNScripting::ScriptPointer_X spX;
};

WN_VOID PrintString(WNScripting::WNScriptingArrayRef<WN_CHAR>* mArray) {
    WNScripting::WNScriptingArray<WN_CHAR> inArray(mArray);
    printf("%*s", static_cast<WN_INT32>(inArray.GetLength()), &(inArray[0]));
}

WNScripting::WNScriptingArrayRef<WN_CHAR>* GetHelloWorld() {
    WNScripting::WNScriptingArray<WN_CHAR> newArray;
    const char* hw = "hello world";
    WN_SIZE_T strLen = 12;
    g_Engine->ConstructScriptingArray(newArray, strLen);
    memcpy(&newArray[0], hw, strLen);
    return(newArray.ReleaseOwnership().DetachOut());
}

DEFINE_CPP_TYPE(Foo);

WN_INT32 WNMain(WN_INT32 _argc, WN_CHAR* _argv[]) {
    WN_CHAR tests[1024];
#ifdef _WN_ANDROID
    WNStrCpy(tests, "/sdcard/MCJitTest.wns" );
    const WN_CHAR* func = "test";
    const WN_CHAR* func2 = "test2";
#else

    if (_argc < 2) {
        WN_PRINTF("Enter Filename >>");

        getLine(tests, 1023);

        return(-1);
    } else {
        WNStrCpy(tests, _argv[1]);
    }

    const WN_CHAR* func = (_argc > 2) ? _argv[2] : "test";

#endif
    WNScripting::WNScriptingEngine* scriptingEngine = WNScripting::WNScriptingEngineFactory::CreateScriptingEngine();
    g_Engine = scriptingEngine;
    scriptingEngine->SetCompilationLog(&mMyLog);
    scriptingEngine->SetLogLevel(WNLogging::eNone);
    scriptingEngine->SetInternalLogLevel(WNLogging::eNone);
    WN_RELEASE_ASSERT(scriptingEngine->Initialize() == eWNOK);
    //scriptingEngine->AddExternalLibs(&RegisterExternalString, WN_NULL);
    scriptingEngine->RegisterCFunction("get23", &get23);
    scriptingEngine->RegisterCFunction("add", &add);
    scriptingEngine->RegisterCFunction("printInt", &print);
    scriptingEngine->RegisterCFunction("printString", &PrintString);
    scriptingEngine->RegisterCFunction("hello", &GetHelloWorld);

    WNScripting::WNFunctionPointer<WN_INT32, WNScripting::WNScriptingArray<WN_CHAR> > fPtr;
    WNScripting::ScriptType_A  mAType;
    WNScripting::WNScriptingArray<WN_CHAR> mArray;
    if(eWNOK != scriptingEngine->CompileFile(tests)) {
        return(-1);
    }
    WNScripting::WNMemberVariablePointer<WN_INT32, WNScripting::ScriptType_A> mVar("y", scriptingEngine);
    if(WNScripting::eResolved!= mVar.GetResolution()) {
        return(-1);
    }

    if(eWNOK != scriptingEngine->ConstructScriptingObject(mAType)) {
        return(-1);
    }

    if(eWNOK != scriptingEngine->ConstructScriptingArray(mArray, 10)) {
        return(-1);
    }
    for(char i = 0; i < 10; ++i) {
        mArray[i] = i;
    }

    printf("%d", mAType->*mVar);

    if(eWNOK == scriptingEngine->GetPointerToFunction(tests,  func, fPtr)) {
       //WN_RELEASE_ASSERT(mVPtr.GetResolution() == WNScripting::eResolved);
        mMyLog.Log(WNLogging::eInfo, 0, "Got function pointer");
        WN_INT32 ch = fPtr(mArray);
        printf("\n\n%d\n\n", ch);
    }
    WN_DELETE(scriptingEngine);
    return(0);
}

