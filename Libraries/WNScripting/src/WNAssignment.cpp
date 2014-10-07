////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNScripting/inc/WNAssignment.h"
#include "WNScripting/inc/WNLValue.h"
#include "WNScripting/inc/WNExpression.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNTypeManager.h"

using namespace WNScripting;

WNAssignment::WNAssignment(WNLValue* _lValue):
    mLValue(_lValue),
    mAssignType(AT_MAX),
    mAssignExpression(WN_NULL) {
}

WNAssignment::~WNAssignment() {
    WN_DELETE(mLValue);
    if(mAssignExpression) {
        WN_DELETE(mAssignExpression);
    }
}

void WNAssignment::AddValue(WNAssignType _type, WNExpression* value) {
    mAssignType = _type;
    mAssignExpression = value;
}

eWNTypeError WNAssignment::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    eWNTypeError err = eWNOK;
    if(eWNOK != (err = mLValue->GenerateCode(_module, _def, _compilationLog))) {
        return(err);
    }
    
    if(!mAssignExpression) {
        if(mLValue->RequiredUse()) {
            _compilationLog.Log(WNLogging::eError, 0, mLValue->GetValueType()->mName, "must be assigned to");
            LogLine(_compilationLog, WNLogging::eError);
            return(eWNInvalidOperation);
        } else {
            return(eWNOK);
        }
    }
    if(!mLValue->GetValueLocation()) {
        _compilationLog.Log(WNLogging::eError, 0, mLValue->GetValueType()->mName, "is not a valid assignee");
        LogLine(_compilationLog, WNLogging::eError);
        return(eWNBadType);
    }
    const GenerateAssignment* assign = _module.GetTypeManager().GetAssignmentOperation(mLValue->GetValueType(), mAssignType);
    if(!assign) {
        _compilationLog.Log(WNLogging::eError, 0, "Cannot generate assignment for ", mLValue->GetValueType()->mName);
        LogLine(_compilationLog, WNLogging::eError);
        return(eWNBadType);
    }
    
    WNScriptType type = mLValue->GetValueType();
    if(eWNOK != (err = mAssignExpression->GenerateCode(_module, _def, _compilationLog))) {
        return(err);
    }

    llvm::Value* val = mAssignExpression->GetValue();
    if(mAssignExpression->GetType() != type) { 
        const GenerateCastingOperation* op = _module.GetTypeManager().GetCastingOperation(mAssignExpression->GetType(), mLValue->GetValueType());
        if(op == WN_NULL) {
            _compilationLog.Log(WNLogging::eError, 0, "Cannot cast from ", mAssignExpression->GetType()->mName, " to ", mLValue->GetValueType()->mName);
            LogLine(_compilationLog, WNLogging::eError);
            return(eWNInvalidCast);
        }
        
        if(eWNOK != (err = op->Execute(_module.GetBuilder(), mAssignExpression->GetValue(), val))) {
            _compilationLog.Log(WNLogging::eCritical, 0, "Error generating cast from ", mAssignExpression->GetType()->mName, " to ", mLValue->GetValueType()->mName);
            LogLine(_compilationLog, WNLogging::eCritical);
            return(eWNOK);
        }
    }
    if(eWNOK != (err = assign->Execute(_module, false, type, val, mLValue->GetValueLocation(), _def, _compilationLog))) {
        _compilationLog.Log(WNLogging::eCritical, 0, "Error generating assignment");
        LogLine(_compilationLog, WNLogging::eCritical);
        return(err);
    }

    return(eWNOK);
}
