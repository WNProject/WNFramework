////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_FUNCTION_PARAM_H__
#define __WN_FUNCTION_PARAM_H__
#include "WNScripting/inc/WNTypeElement.h"
namespace llvm {
    class Value;
}

namespace WNScripting {
    struct FunctionParam {
        llvm::Value* mValue;
        WNScriptType mType;
    };
}

#endif//__WN_FUNCTION_PARAM_H__
