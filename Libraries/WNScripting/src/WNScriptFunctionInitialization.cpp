// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNBuiltinTypeInitialization.h"
#include "WNScripting/inc/WNScriptingEngine.h"
#include <stdlib.h>

namespace WNScripting {
    wn_void* TestMalloc(wn_size_t size) {
        void* x = malloc(size);
        return(x);
    }

    wn_void TestFree(wn_void* ptr) {
        return(free(ptr));
    }
    wn_void Assert() {
        assert(false);
    }
    wn_void DebugPrint(int x) {
        printf("%d\n", x);
    }

    eWNTypeError WNBuiltInInitializer::InitializeFunctions(WNScriptingEngine& _engine) {
        eWNTypeError err = ok;

        if(ok != (err = _engine.RegisterCFunction("Assert", &Assert))) {
            return(err);
        }

        if(ok != (err = _engine.RegisterCFunction("DebugPrint", &DebugPrint))) {
            return(err);
        }

       return(ok);
    }
}
