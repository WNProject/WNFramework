////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNScripting/inc/WNLValue.h"
#include "WNScripting/inc/WNExpression.h"

using namespace WNScripting;

WNLValue::WNLValue(WNExpression* _expr) :
    mExpression(_expr) {

}

WNLValue::~WNLValue() {
    WN_DELETE(mExpression);
}

eWNTypeError WNLValue::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    return(mExpression->GenerateCode(_module, _def, _compilationLog));
}

llvm::Value* WNLValue::GetValueLocation() {
    return(mExpression->GetValueLocation());
}

WNScriptType WNLValue::GetValueType() {
    return(mExpression->GetType());
}

WN_BOOL WNLValue::RequiredUse() {
    return(mExpression->RequiredUse());
}

