////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNScripting/inc/WNDoInstruction.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNInstructionList.h"
#include "WNScripting/inc/WNTypeManager.h"
#include "WNScripting/inc/WNExpression.h"

#ifdef _WN_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4100)
    #pragma warning(disable: 4127)
    #pragma warning(disable: 4152)
    #pragma warning(disable: 4244)
    #pragma warning(disable: 4245)
    #pragma warning(disable: 4267)
    #pragma warning(disable: 4355)
    #pragma warning(disable: 4512)
    #pragma warning(disable: 4800)
#endif

#include "llvm/IR/IRBuilder.h"


#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

using namespace WNScripting; 

WNDoInstruction::WNDoInstruction(WNExpression* _cond, WNInstructionList* _body) :
    mCondition(_cond),
    mBody(_body) {
}

WNDoInstruction::~WNDoInstruction() {
    WN_DELETE(mCondition);
    WN_DELETE(mBody);
}

eWNTypeError WNDoInstruction::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
    llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
    builder->CreateBr(bodyBlock);
    builder->SetInsertPoint(bodyBlock);
    eWNTypeError err = eWNOK;
    if(eWNOK != (err = mBody->GenerateCode(_module, _def, _compilationLog))) {
        return(err);
    }
    if(mBody->Returns()) {
        mReturns = true;
        return(eWNOK);
    }
    llvm::BasicBlock* endBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
    WNScriptType boolType = WN_NULL;
    if(eWNOK != (err = _module.GetTypeManager().GetTypeByName("Bool", boolType))) {
        _compilationLog.Log(WNLogging::eCritical, 0, "Error, cannot find Bool Type");
        LogLine(_compilationLog, WNLogging::eCritical);
        return(err);
    }
    if(eWNOK != (err = mCondition->GenerateCode(_module, _def, _compilationLog))) {
        return(err);
    }
    if(mCondition->GetType() != boolType) {
        _compilationLog.Log(WNLogging::eError, 0, "Error, conditional must be bool type");
        LogLine(_compilationLog, WNLogging::eError);
        return(eWNBadType);
    }
    llvm::Value* cv = llvm::ConstantInt::get(boolType->mLLVMType, 0);
    llvm::Value* v = mCondition->GetValue();
    v = builder->CreateICmpNE(cv, v, "");
    builder->CreateCondBr(v, bodyBlock, endBlock);
    builder->SetInsertPoint(endBlock);
    return(eWNOK);
}


