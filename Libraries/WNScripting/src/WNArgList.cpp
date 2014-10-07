////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNScripting/inc/WNArgList.h"
#include "WNMemory/inc/WNAllocation.h"
#include "WNScripting/inc/WNExpression.h"
using namespace WNScripting; 
WNArgList::WNArgList() {
}

WNArgList::~WNArgList() {
    for(WNScriptLinkedList<WNFunctionExpression>::WNScriptLinkedListNode * i = mExpressionList.first; i != WN_NULL; i = i->next) {
        WN_DELETE(i->value->expr);
        WN_DELETE(i->value);
    }
}

void WNArgList::AddExpression(WNExpression* _expr, WN_BOOL _handOwnership) {
    mExpressionList.PushBack(new WNFunctionExpression(_expr, _handOwnership));
}

WNScriptLinkedList<WNFunctionExpression>& WNArgList::GetExpressions() {
    return(mExpressionList);
}

