////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_LVALUE_H__
#define __WN_LVALUE_H__

#include "WNScripting/inc/WNNode.h"
#include "WNScripting/inc/WNTypeElement.h"
#include "WNScripting/inc/WNScriptingErrors.h"
#include "WNLogging/inc/WNLog.h"

namespace llvm {
    class Value;
}

namespace WNScripting {
    class WNExpression;
    class WNCodeModule;
    struct WNFunctionDefinition;
    class WNLValue : public WNNode {
    public:

        WNLValue(WNExpression* _expr);
        virtual ~WNLValue();
        virtual eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);

        llvm::Value* GetValueLocation();

        WNScriptType GetValueType();

        WN_BOOL RequiredUse();

    private:
        WNExpression* mExpression;
    };
}
#endif//__WN_LVALUE_H__
