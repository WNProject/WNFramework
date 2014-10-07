////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_COND_EXPRESSION_H__
#define __WN_COND_EXPRESSION_H__

#include "WNScripting/inc/WNExpression.h"
namespace WNScripting {
    class WNCondExpression : public WNExpression {
    public:
        WNCondExpression(WNExpression* _cond, WNExpression* _lhs, WNExpression* _rhs);
        virtual ~WNCondExpression();
        virtual eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog); 

    private:
        WNExpression* mCondition;
        WNExpression* mLHS;
        WNExpression* mRHS;
    };
}
#endif//__WN_COND_EXPRESSION_H__
