////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_POST_EXPRESSION_H__
#define __WN_POST_EXPRESSION_H__
#include "WNScripting/inc/WNExpression.h"

namespace WNScripting {
    class WNPostExpression : public WNExpression {
    public:
        WNPostExpression();
        virtual ~WNPostExpression();
        void AddBaseExpr(WNExpression* _expr);
    protected:
        WNExpression* mBaseExpression;
    };
}
#endif//__WN_POST_EXPRESSION_H__
