////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNScripting/inc/WNBuiltinTypeInitialization.h"
#include "WNScripting/inc/WNScriptingEngine.h"
#include <stdlib.h>

namespace WNScripting {
    WN_VOID* TestMalloc(WN_SIZE_T size) {
        void* x = malloc(size);
        return(x);
    }

    WN_VOID TestFree(WN_VOID* ptr) {
        return(free(ptr));
    }
    WN_VOID Assert() {
        assert(false);
    }
    WN_VOID DebugPrint(int x) {
        printf("%d\n", x);
    }

    eWNTypeError WNBuiltInInitializer::InitializeFunctions(WNScriptingEngine& _engine) {
        eWNTypeError err = eWNOK;

        if(eWNOK != (err = _engine.RegisterCFunction("Assert", &Assert))) {
            return(err);
        }

        if(eWNOK != (err = _engine.RegisterCFunction("DebugPrint", &DebugPrint))) {
            return(err);
        }

       return(eWNOK);
    }
}
