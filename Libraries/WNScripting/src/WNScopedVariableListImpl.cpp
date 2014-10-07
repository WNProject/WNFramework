////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNScripting/inc/WNScopedVaraiableListImpl.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNTypeManager.h"
#include "WNCore/inc/WNAssert.h"

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
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Function.h"

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

using namespace WNScripting;

WNScopedVariableListImpl::~WNScopedVariableListImpl() {
}

eWNTypeError WNScopedVariableListImpl::PushVariable(WNScriptVariable* _variable) {
    WN_DEBUG_ASSERT(_variable != WN_NULL);
    if(!mScriptVariables.empty()) {
        for(WN_INT64 i = mScriptVariables.size() - 1; i >= 0; --i) {
            if(!mScriptVariables[static_cast<WN_SIZE_T>(i)]) {
                break;
            }
            if(0 == WNStrings::WNStrNCmp(_variable->GetName(), mScriptVariables[static_cast<WN_SIZE_T>(i)]->GetName(), 256)) {
                return(eWNAlreadyExists);
            }
        }
    }
    

    mScriptVariables.push_back(_variable);
    return(eWNOK);
}

WN_VOID WNScopedVariableListImpl::PushScopeBlock(WNCodeModule& _module) {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>* >(_module.GetBuilder());
    llvm::Function* fnc = llvm::Intrinsic::getDeclaration(_module.GetModule(), llvm::Intrinsic::stacksave);
    mScopeBlocks.push_back(builder->CreateCall(fnc, ""));
    mScriptVariables.push_back(WN_NULL);
}

eWNTypeError WNScopedVariableListImpl::GenerateReturn(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    eWNTypeError err = eWNOK;
    if(mScriptVariables.empty()) {
        return(eWNOK);
    }
    for(WN_INT64 i = mScriptVariables.size() - 1; i >= 0; --i) {
        if(mScriptVariables[static_cast<WN_SIZE_T>(i)] == WN_NULL || !mScriptVariables[static_cast<WN_SIZE_T>(i)]->Clean()) {
            continue;
        }

        const GenerateDestruction* dest = _module.GetTypeManager().GetDestructionOperation(mScriptVariables[static_cast<WN_SIZE_T>(i)]->GetType());
        if(dest) {
            if(eWNOK != (err = dest->Execute(_module, mScriptVariables[static_cast<WN_SIZE_T>(i)]->GetType(), mScriptVariables[static_cast<WN_SIZE_T>(i)]->GetLocation(), _def, _compilationLog)))
            {
                return(err);
            }
        }
    }
    return(eWNOK);
}

WN_VOID WNScopedVariableListImpl::ClearScope() {
    if(mScriptVariables.empty()) {
        return;
    }
    for(WN_INT32 i = static_cast<WN_INT32>(mScriptVariables.size()) - 1; i >= 0; --i) {
        if(mScriptVariables[WN_SIZE_T(i)] == WN_NULL) {
            mScriptVariables.pop_back();
            continue;
        }
        WN_DELETE(mScriptVariables[i]);
        mScriptVariables.pop_back();
    }
}

eWNTypeError WNScopedVariableListImpl::PopScopeBlock(WNCodeModule& _module, WN_BOOL _cleanStack, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    eWNTypeError err = eWNOK;

    WN_DEBUG_ASSERT(!mScriptVariables.empty() && !mScopeBlocks.empty());
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>* >(_module.GetBuilder());
    for(WN_INT32 i = static_cast<WN_INT32>(mScriptVariables.size()) - 1; i >= 0; --i) {
        if(mScriptVariables[static_cast<WN_SIZE_T>(i)] == WN_NULL) {
            mScriptVariables.pop_back();
            break;
        }
        if(_cleanStack) {
            const GenerateDestruction* dest = _module.GetTypeManager().GetDestructionOperation(mScriptVariables[static_cast<WN_SIZE_T>(i)]->GetType());
            if(dest) {
                if(eWNOK != (err = dest->Execute(_module, mScriptVariables[i]->GetType(), mScriptVariables[static_cast<WN_SIZE_T>(i)]->GetLocation(), _def, _compilationLog))) {
                    return(err);
                }
            }
        }
        WN_DELETE(mScriptVariables[static_cast<WN_SIZE_T>(i)]);
        mScriptVariables.pop_back();
    }
    if(_cleanStack) {
        llvm::Function* fnc = llvm::Intrinsic::getDeclaration(_module.GetModule(), llvm::Intrinsic::stackrestore);
        builder->CreateCall(fnc,  mScopeBlocks.back(), "");
    }
    mScopeBlocks.pop_back();
    return(eWNOK);
}

const WNScriptVariable* WNScopedVariableListImpl::GetVariable(const WN_CHAR* _variableName) {
    if(mScriptVariables.empty()) {
        return(WN_NULL);
    }
    for(WN_INT64 i = mScriptVariables.size() - 1; i >= 0; --i) {
        if(mScriptVariables[static_cast<WN_SIZE_T>(i)] && (0 == WNStrings::WNStrNCmp(_variableName, mScriptVariables[static_cast<WN_SIZE_T>(i)]->GetName(), 256))) {
            return(mScriptVariables[static_cast<WN_SIZE_T>(i)]);
        }
    }
    return(WN_NULL);
}
