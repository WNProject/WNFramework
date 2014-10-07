////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_FUNCTION_CALL_EXPR_H__
#define __WN_FUNCTION_CALL_EXPR_H__

#include "WNScripting/inc/WNPostExpression.h"
namespace WNScripting {
    class WNArgList;
    class WNFunctionCallExpr : public WNPostExpression {
    public:
        WNFunctionCallExpr();
        WNFunctionCallExpr(WNArgList* _list);
        virtual ~WNFunctionCallExpr();
        virtual eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog); 
    private:
        WNArgList* mArgs;
    };
}
#endif//__WN_FUNCTION_CALL_EXPR_H__
