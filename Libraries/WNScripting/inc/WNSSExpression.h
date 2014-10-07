////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_SS_EXPRESSION_H__
#define __WN_SS_EXPRESSION_H__
#include "WNScripting/inc/WNExpression.h"
#include "WNScripting/inc/WNScriptingEnums.h"
namespace WNScripting {
    class WNSSExpression : public WNExpression {
    public:
        WNSSExpression(WNSSType _type, WNExpression* _lhs, WNExpression* _rhs);
        virtual ~WNSSExpression();
        virtual eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);
    private:
        WNSSType mSSType;
        WNExpression* mLHS;
        WNExpression* mRHS;
    };
}
#endif//__WN_SS_EXPRESSION_H__
