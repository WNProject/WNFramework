////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_BUILTIN_TYPE_INITIALIZATION_H__
#define __WN_BUILTIN_TYPE_INITIALIZATION_H__

#include "WNScripting/inc/WNScriptingErrors.h"

namespace WNScripting {
    class WNTypeManager;
    class WNScriptingEngine;
    class WNBuiltInInitializer {
    public:
        static eWNTypeError InitializeIntTypes(WNScriptingEngine* _engine, WNTypeManager& _manager);
        static eWNTypeError InitializeFloatTypes(WNScriptingEngine* _engine, WNTypeManager& _manager);
        //static eWNTypeError InitializeStringTypes();
        static eWNTypeError InitializeScriptingCasts(WNScriptingEngine* _engine, WNTypeManager& _manager);
        static eWNTypeError InitializeFunctions(WNScriptingEngine& _engine);
    };
}

#endif//__WN_BUILTIN_TYPE_INITIALIZATION_H__
