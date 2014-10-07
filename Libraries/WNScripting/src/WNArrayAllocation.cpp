////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNScripting/inc/WNArrayAllocation.h"
#include "WNScripting/inc/WNTypeNode.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNTypeManager.h"
#include "WNScripting/inc/WNNodeHelpers.h"

using namespace WNScripting;

WNArrayAllocation::WNArrayAllocation() :
    mLevels(0),
    mType(WN_NULL),
    mCopyInitializer(WN_NULL) {
}

WNArrayAllocation::~WNArrayAllocation() {
    if(mType) {
        WN_DELETE(mType);
    }
    for(WNScriptLinkedList<WNExpression>::WNScriptLinkedListNode* i = mArrayInitializers.first; i != WN_NULL; i = i->next) {
        WN_DELETE(i->value);
    }
}   

WN_VOID WNArrayAllocation::SetType(WNTypeNode* _typeNode) {
    mType = _typeNode;
}

WN_VOID WNArrayAllocation::AddExpression(WNExpression* _expr) {
    mArrayInitializers.PushBack(_expr);
    mLevels++;
}

WN_VOID WNArrayAllocation::AddLevel() {
    mLevels++;
}

eWNTypeError WNArrayAllocation::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    mNewlyCreated = WN_TRUE;
    mForceUse = WN_TRUE;
    eWNTypeError err = eWNOK;
    
    if(mArrayInitializers.first != WN_NULL && mCopyInitializer != WN_NULL) {
        _compilationLog.Log(WNLogging::eError, 0, "Cannot specify both size and copy construction");
        LogLine(_compilationLog, WNLogging::eError);
    }


    if(eWNOK != (err = mType->GetType(_module.GetTypeManager(), mScriptType, _compilationLog))){
        _compilationLog.Log(WNLogging::eError, 0, "Cannot find type ");
        LogLine(_compilationLog, WNLogging::eError);
        return(err);
    }
    for(WN_SIZE_T i = 0; i < mLevels; ++i){
        if(eWNOK != (err = _module.GetTypeManager().GetArrayOf(mScriptType, mScriptType))) {
            _compilationLog.Log(WNLogging::eError, 0, "Cannot create array of ", mScriptType->mName);
            LogLine(_compilationLog, WNLogging::eError);
            return(err);
        }
    }
    if(mCopyInitializer) {
        const GenerateCopyConstruction* construction = _module.GetTypeManager().GetCopyConstructionOperation(mScriptType);
        if(!construction) {
            _compilationLog.Log(WNLogging::eError, 0, "Could not find copy construction operation for ", mScriptType->mName);
            LogLine(_compilationLog, WNLogging::eError);
            return(eWNCannotCreateType);
        }   

        if(eWNOK != (err = mCopyInitializer->GenerateCode(_module, _def, _compilationLog))) {
            return(err);
        }
        llvm::Value* copyValue = mCopyInitializer->GetValue();
        if(mCopyInitializer->RequiredUse()) {
            _compilationLog.Log(WNLogging::eError, 0, "Cannot copy construct temporary value, construct instead");
            LogLine(_compilationLog, WNLogging::eError);
            return(eWNInvalidCast);
        }
        if(mCopyInitializer->GetType() != mScriptType) {
            const GenerateCastingOperation* castOp = _module.GetTypeManager().GetCastingOperation(mCopyInitializer->GetType(), mScriptType);
            if(!castOp) {
                _compilationLog.Log(WNLogging::eError, 0, "Cannot construct array of type ", mScriptType->mName, " from type ", mCopyInitializer->GetType()->mName, " different type");
                LogLine(_compilationLog, WNLogging::eError);
                return(eWNInvalidCast);
            } else {
                if(eWNOK != (err = castOp->Execute(_module.GetBuilder(), copyValue, copyValue))) {
                    _compilationLog.Log(WNLogging::eCritical, 0, "Error casting ", mCopyInitializer->GetType()->mName, " from type ", mScriptType->mName, " different type");
                    LogLine(_compilationLog, WNLogging::eError);
                    return(eWNInvalidCast);
                }
            }
        }

        if(eWNOK != (err = construction->Execute(_module, mValue, mScriptType, copyValue, _def, _compilationLog))) {
            LogLine(_compilationLog, WNLogging::eError);
            return(err);
        }
    } else {
        const GenerateConstruction* construction = _module.GetTypeManager().GetConstructionOperation(mScriptType);
        if(!construction) {
            _compilationLog.Log(WNLogging::eError, 0, "Could not find construction operation for ", mScriptType->mName);
            LogLine(_compilationLog, WNLogging::eError);
            return(eWNCannotCreateType);
        }        
        if(eWNOK != (err = construction->Execute(_module, mValue, mScriptType, mArrayInitializers.first, _def, _compilationLog))) {
            LogLine(_compilationLog, WNLogging::eError);
            return(err);
        }
    }
    return(eWNOK);            
}

WN_VOID WNArrayAllocation::SetCopyInitializer(WNExpression* _expression) {
    mCopyInitializer = _expression;
}