////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNScripting/inc/WNPostExpression.h"

using namespace WNScripting;

WNPostExpression::WNPostExpression():
    mBaseExpression(WN_NULL) {
}

WNPostExpression::~WNPostExpression() {
    if(mBaseExpression) {
        WN_DELETE(mBaseExpression);
    }
}

void WNPostExpression::AddBaseExpr(WNExpression* _expr) {
    mBaseExpression = _expr;
}

