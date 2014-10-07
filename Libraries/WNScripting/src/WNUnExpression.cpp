////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNScripting/inc/WNUnExpression.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNTypeManager.h"
#include "WNScripting/inc/WNExpression.h"

using namespace WNScripting;

WNUNExpression::WNUNExpression(WNUnaryType _type, WNExpression* _expr) :
    mType(_type), mExpression(_expr){
}

WNUNExpression::~WNUNExpression() {
    WN_DELETE(mExpression);
}

eWNTypeError WNUNExpression::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    if(mType >= UN_MAX) {
        _compilationLog.Log(WNLogging::eError, 0, "Invalid Unary Operation ", mType);
        LogLine(_compilationLog, WNLogging::eError);
        return(eWNInvalidOperation);
    }
    eWNTypeError err = eWNOK;
    if(eWNOK != (err = mExpression->GenerateCode(_module, _def, _compilationLog))) {
        return(err);
    }
    const GeneratePreUnaryOperation* preOp = _module.GetTypeManager().GetPreUnaryOperation(mType, mExpression->GetType());
    if(!preOp) {
        _compilationLog.Log(WNLogging::eError, 0, "Operation does not exist for type ", mExpression->GetType()->mName);
        LogLine(_compilationLog, WNLogging::eError);
        return(eWNInvalidOperation);
    }
    if(eWNOK != (err = preOp->Execute(_module.GetBuilder(), *mExpression, mValue))) {
        return(err);
    }
    if(mType != UN_NEG) {
        mValueLocation = mExpression->GetValueLocation();
    }
    mScriptType = mExpression->GetType();
    return(eWNOK);
}

