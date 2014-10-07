////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNScriptingMemoryManager.h"
#include "WNScripting/inc/WNParameter.h"
#include "WNScripting/inc/WNTypeManager.h"
#include "WNContainers/inc/WNAlgorithm.h"
#include <algorithm>
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

#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/ADT/Triple.h"
#include "llvm/IR/IRBuilder.h"

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

using namespace WNScripting;

WNCodeModule::WNCodeModule(WNTypeManager& _manager, WNScopedVariableList& _variableList, WNScriptingEngine* _scriptingEngine) :
    mTypeManager(_manager),
    mScopedVariableList(_variableList),
    mScriptingEngine(_scriptingEngine) {
}

const WNScriptingEngine* WNCodeModule::GetScriptingEngine() const {
    return(mScriptingEngine);
}

WNTypeManager& WNCodeModule::GetTypeManager() const {
    return(mTypeManager);
}

WNScopedVariableList& WNCodeModule::GetScopedVariableList() {
    return(mScopedVariableList);
}

eWNTypeError WNCodeModule::Initialize(WN_UINT32 flags, WNScriptingMemoryManager& _manager) {
    mModule = WN_NEW llvm::Module("MCJitModule", llvm::getGlobalContext());

#ifdef _WN_ANDROID
#ifdef _WN_ARM
    mModule->setTargetTriple(llvm::Triple::normalize("arm-linux-androideabi"));
#else
    mModule->setTargetTriple(llvm::Triple::normalize("x86-linux-androideabi"));
#endif
#elif defined(_WN_WINDOWS)
    #ifdef _WN_64_BIT
        mModule->setTargetTriple(llvm::Triple::normalize("x86_64-pc-win32-elf"));
    #else
        mModule->setTargetTriple(llvm::Triple::normalize("i686-pc-win32-elf"));
    #endif
#endif //otherwise let it figure itself out
    llvm::EngineBuilder  builder(mModule);
    builder.setEngineKind(llvm::EngineKind::JIT);
    builder.setUseMCJIT(true);
    builder.setMCJITMemoryManager(&_manager);

    if(flags & eOptimized ) {
        builder.setOptLevel(llvm::CodeGenOpt::Aggressive);
    } else {
        builder.setOptLevel(llvm::CodeGenOpt::None);
    }
    mEngine = builder.create();
    if(!mEngine) {
        delete(mModule);
        return(eWNError);
    }
    mBuilder = WN_NEW llvm::IRBuilder<>(llvm::getGlobalContext());
    if(!mBuilder) {
         WN_DELETE(mEngine);
         WN_DELETE(mModule);
         return(eWNError);
    }

    return(eWNOK);
}

llvm::ExecutionEngine* WNCodeModule::GetExecutionEngine() {
    return(mEngine);
}

llvm::Module* WNCodeModule::GetModule() {
    return(mModule);
}

llvm::IRBuilderBase* WNCodeModule::GetBuilder() {
    return(mBuilder);
}

WNFunctionDefinition* WNCodeModule::GetFunctionDefinition(const WN_CHAR* _name, const std::vector<WNParameter>& _params) {
    for(std::vector<WNFunctionDefinition*>::iterator i = mFunctions.begin(); i != mFunctions.end(); ++i) {
        if(WNStrings::WNStrCmp((*i)->mName, _name) == 0) {
            
            if(_params.size() != (*i)->mTypes.size()) {
                continue;
            }
            size_t j = 0;
            for(; j < _params.size(); ++j) {
                if(_params[j].mType != (*i)->mTypes[j]) {
                    break;
                }
            }
            if (j == _params.size()) {
                return(*i);
            }
        }
    }
    return(WN_NULL);
}

WNFunctionDefinition* WNCodeModule::GetFunctionDefinition(const WN_CHAR* _name, const std::vector<WNScriptType>& _params) {
    for(std::vector<WNFunctionDefinition*>::iterator i = mFunctions.begin(); i != mFunctions.end(); ++i) {
        if(WNStrings::WNStrCmp((*i)->mName, _name) == 0) {
            
            if(_params.size() != (*i)->mTypes.size()) {
                continue;
            }
            size_t j = 0;
            for(; j < _params.size(); ++j) {
                if(_params[j] != (*i)->mTypes[j]) {
                    break;
                }
            }
            if (j == _params.size()) {
                return(*i);
            }
        }
    }
    return(WN_NULL);
}

struct CountingIntHelper {
    CountingIntHelper(WN_INT32 _def): def(_def), numCasts(0) {
    }
    int numCasts;
    WN_UINT32 def;
};

struct CountingStructHelper {
    CountingStructHelper(WNFunctionDefinition* _def): def(_def), numCasts(0) {
    }
    int numCasts;
    WNFunctionDefinition* def;
};

eWNTypeError WNCodeModule::GetNonCastableVirtualFunction(const WN_CHAR* _name, const WNScriptType thisType, const std::vector<WNScriptType>& _params, WN_INT32& _outIndex) {
    if(thisType->mVTable.size() == 0) {
        return(eWNDoesNotExist);
    }
    std::vector<WNFunctionDefinition*>& functions = thisType->mVTable;
    std::vector<CountingIntHelper> sharedNameDefs;
    for(WN_SIZE_T i = 0; i < functions.size(); ++i) {
        const WN_CHAR* lookupFunc = WNStrings::WNStrStr(functions[i]->mName, "$");
        lookupFunc = lookupFunc == NULL? functions[i]->mName: lookupFunc + 1;
        if(WNStrings::WNStrCmp(lookupFunc, _name) == 0) {
            if(functions[i]->mTypes.size() == _params.size() + 1) {
                sharedNameDefs.push_back(CountingIntHelper(static_cast<WN_INT32>(i)));
            }
        }
    }

    for(size_t i = 0; i < _params.size(); ++i) {
        for(std::vector<CountingIntHelper>::iterator vIt =  sharedNameDefs.begin() + 1; vIt != sharedNameDefs.end(); ) {
            if( functions[vIt->def]->mTypes[i + 1] != _params[i]) {
                vIt = sharedNameDefs.erase(vIt);
                continue;
            }
            vIt++;
        }
    }

    if(sharedNameDefs.empty()) {
        return(eWNError);
    }
    if(sharedNameDefs.size() > 1) {
            return(eWNAmbiguous);
    }
    _outIndex = sharedNameDefs.begin()->def;
    return( eWNOK);
}

eWNTypeError WNCodeModule::GetThisCallFunction(const WN_CHAR* _name, WNScriptType thisType, const std::vector<FunctionParam>& _params, WN_INT32& _outIndex, WNFunctionDefinition*& _outDefinition) {
    WN_INT32 outIndex = -1;
    
    std::vector<CountingStructHelper> sharedNameDefs;
    eWNTypeError virtualErr = GetCastableVirtualFunction(_name, thisType, _params, outIndex);
    
    WN_CHAR funcName[2048];
    while(thisType) {
        funcName[0] = '\0';
        if(_name[0] != '$') { //names starting with $ are special this means they are exposed
                              //to scripting but have arbitrary base types
            WNStrings::WNStrNCat(funcName, thisType->mName, 1024);
            WN_DEBUG_ASSERT(WNStrings::WNStrLen(funcName) == WNStrings::WNStrLen(thisType->mName));
            WNStrings::WNStrCat(funcName, "$");
        }
        WNStrings::WNStrNCat(funcName, _name, 1024);
        for(std::vector<WNFunctionDefinition*>::iterator i = mFunctions.begin(); i != mFunctions.end(); ++i) {
            if(_name[0] != '$') {
                if((*i)->mIsVirtual || !(*i)->mThisType) {
                    continue;
                }
            }
            if(WNStrings::WNStrCmp((*i)->mName, funcName) == 0) {
                if((*i)->mTypes.size() == (_params.size() + 1)) {
                    sharedNameDefs.push_back(CountingStructHelper(*i));
                }
            }
        }
        thisType = (_name[0] == '$'? 0 : thisType->mParentClass);
    }

    for(size_t i = 0; i < _params.size(); ++i) {
        for(std::vector<CountingStructHelper>::iterator vIt =  sharedNameDefs.begin(); vIt != sharedNameDefs.end(); ) {
            if( vIt->def->mTypes[i + 1] != _params[i].mType) { // we do not current care about the base type, so start with i+1
                if(!mTypeManager.GetCastingOperation(_params[i].mType, vIt->def->mTypes[i + 1])) {
                    vIt = sharedNameDefs.erase(vIt);
                    continue;
                }
                vIt->numCasts++;
                vIt++;
                continue;
            }
            vIt++;
        }
    }

    if(sharedNameDefs.empty()) {
        _outIndex = outIndex;
        return(virtualErr);
    }

    WNFunctionDefinition* perfectFunction = WN_NULL;
    std::vector<CountingStructHelper>::iterator i = std::find_if(sharedNameDefs.begin(), sharedNameDefs.end(), [](CountingStructHelper& val){ return(val.numCasts == 0); });
    if(i != sharedNameDefs.end()) {
        perfectFunction = (*i).def;
    }
    if(eWNOK == virtualErr) { //if we found a perfect match above us, then its either ambiguous, or it was good
        if(perfectFunction) {
            return(eWNAmbiguous);
        } else {
            _outIndex = outIndex;
            return(eWNPartiallyAmbiguous);
        }
    }
    if(sharedNameDefs.size() > 1) {
        
        if(!perfectFunction) {
            return(eWNAmbiguous);
        }
        _outDefinition = perfectFunction;
        return(eWNPartiallyAmbiguous);
    }

    if(!perfectFunction && eWNPartiallyAmbiguous == virtualErr) { //If we do not have a "perfect match" and neither does our virtual
                                                                  //we would have to cast, ambiguous cast
        return(eWNAmbiguous);
    }
    _outDefinition = perfectFunction ? perfectFunction : sharedNameDefs.begin()->def;
    if(perfectFunction && eWNPartiallyAmbiguous == virtualErr) {
        return(eWNPartiallyAmbiguous);
    }
    return(perfectFunction? eWNOK : eWNPartiallyAmbiguous);
}

eWNTypeError WNCodeModule::GetCastableVirtualFunction(const WN_CHAR* _name, const WNScriptType thisType, const std::vector<FunctionParam>& _params, WN_INT32& _outIndex) {
    if(thisType->mVTable.size() == 0) {
        return(eWNDoesNotExist);
    }
    std::vector<WNFunctionDefinition*>& functions = thisType->mVTable;
     std::vector<CountingIntHelper> sharedNameDefs;
    for(WN_SIZE_T i = 0; i < functions.size(); ++i) {
        const WN_CHAR* lookupFunc = WNStrings::WNStrStr(functions[i]->mName, "$");
        lookupFunc = lookupFunc == NULL? functions[i]->mName: lookupFunc + 1;
        if(WNStrings::WNStrCmp(lookupFunc, _name) == 0) {
            if(functions[i]->mTypes.size() == _params.size() + 1) {
                sharedNameDefs.push_back(CountingIntHelper(static_cast<WN_INT32>(i)));
            }
        }
    }

    for(size_t i = 0; i < _params.size(); ++i) {
        for(std::vector<CountingIntHelper>::iterator vIt =  sharedNameDefs.begin(); vIt != sharedNameDefs.end(); ) {
            if( functions[vIt->def]->mTypes[i + 1] != _params[i].mType) {
                if(!mTypeManager.GetCastingOperation(_params[i].mType, functions[vIt->def]->mTypes[i + 1])) {
                    vIt = sharedNameDefs.erase(vIt);
                    continue;
                }
                ++(vIt->numCasts);
                vIt++;
                continue;
            }
            vIt++;
        }
    }

    if(sharedNameDefs.empty()) {
        return(eWNError);
    }
    WN_INT32 perfectFunction = -1;
    std::vector<CountingIntHelper>::iterator i = std::find_if(sharedNameDefs.begin(), sharedNameDefs.end(), [](CountingIntHelper& val){ return(val.numCasts == 0); });
    if(i != sharedNameDefs.end()) {
        perfectFunction = (*i).def;
    }
    if(sharedNameDefs.size() > 1) {
        _outIndex = perfectFunction;
        if(-1 == perfectFunction) {
            return(eWNAmbiguous);
        } else {
            return(eWNPartiallyAmbiguous);
        }
    }

    _outIndex = (perfectFunction >=0)? perfectFunction : sharedNameDefs.begin()->def;

    return((perfectFunction >=0)? eWNOK : eWNPartiallyAmbiguous);
}

eWNTypeError WNCodeModule::GetCastableFunctionDefinition(const WN_CHAR* _name, const std::vector<WNScriptType>& _params, WNFunctionDefinition*& _outDefinition) {
    std::vector<CountingStructHelper> sharedNameDefs;
    
    for(std::vector<WNFunctionDefinition*>::iterator i = mFunctions.begin(); i != mFunctions.end(); ++i) {
        if(WNStrings::WNStrCmp((*i)->mName, _name) == 0) {
            if((*i)->mTypes.size() == _params.size()) {
                sharedNameDefs.push_back(CountingStructHelper(*i));
            }
        }
    }

    for(size_t i = 0; i < _params.size(); ++i) {
        for(std::vector<CountingStructHelper>::iterator vIt =  sharedNameDefs.begin(); vIt != sharedNameDefs.end(); ) {
            if( vIt->def->mTypes[i] != _params[i]) {
                if(!mTypeManager.GetCastingOperation(_params[i], vIt->def->mTypes[i])) {
                    vIt = sharedNameDefs.erase(vIt);
                    continue;
                }
                vIt->numCasts++;
                vIt++;
                continue;
            }
            vIt++;
        }
    }

    if(sharedNameDefs.empty()) {
        return(eWNError);
    }
     
    WNFunctionDefinition* perfectFunction = WN_NULL;
    std::vector<CountingStructHelper>::iterator i = std::find_if(sharedNameDefs.begin(), sharedNameDefs.end(), [](CountingStructHelper& val){ return(val.numCasts == 0); });
    if(i != sharedNameDefs.end()) {
        perfectFunction = (*i).def;
    }
    if(sharedNameDefs.size() > 1) {
        _outDefinition = perfectFunction;
        if(!perfectFunction) {
            return(eWNAmbiguous);
        } else {
            return(eWNPartiallyAmbiguous);
        }
    }

    _outDefinition = perfectFunction ? perfectFunction : sharedNameDefs.begin()->def;

    return(perfectFunction? eWNOK : eWNPartiallyAmbiguous);
}

eWNTypeError WNCodeModule::GenerateFunctionDefinition(const WN_CHAR* _name, const std::vector<WNScriptType>& _params, const WNScriptType& _return, WNFunctionDefinition*& _outFunctionDefinition, WNScriptType _thisType, bool _virtual) {
    _outFunctionDefinition = GetFunctionDefinition(_name, _params);
    if(_outFunctionDefinition){
        return(eWNAlreadyExists);
    }
    WNFunctionDefinition* def = WN_NEW WNFunctionDefinition();
    def->mName = WNStrings::WNStrNDup(_name, 256);
    def->mReturn = _return;
    def->mTypes.assign(_params.begin(), _params.end());
    def->mFunction = WN_NULL;
    def->mFunctionType = WN_NULL;
    def->mFunctionPointer = WN_NULL;
    def->mThisType = _thisType;
    def->mIsVirtual = _virtual;
    def->mScriptingEngine = mScriptingEngine;
    _outFunctionDefinition = def;
    return(eWNOK);
}

eWNTypeError WNCodeModule::AddFunctionDefinition(const WN_CHAR* _name, const std::vector<WNScriptType>& _params, const WNScriptType& _return, WNFunctionDefinition*& _outFunctionDefinition, WNFunctionDefinition*& _equivalentFunction, WNScriptType _thisType, bool _virtual) {
    eWNTypeError err = GenerateFunctionDefinition(_name, _params, _return, _outFunctionDefinition, _thisType, _virtual);
    if(eWNOK != err) {
        return(err);
    }
     if(WN_NULL != (_equivalentFunction = GetCallableEquivalent(_outFunctionDefinition))) {
        return(eWNAlreadyExists);
    }
    mFunctions.push_back(_outFunctionDefinition);
    return(eWNOK);
}

eWNTypeError WNCodeModule::AddExternalScriptFunction(WNFunctionDefinition* _functionDefinition,WNFunctionDefinition*& _equivalentFunction) {
    if(WN_NULL != (_equivalentFunction = GetCallableEquivalent(_functionDefinition))) {
        return(eWNAlreadyExists);
    }
    mFunctions.push_back(_functionDefinition);

    return(eWNOK);
}

bool WNCodeModule::NamedFunctionExists(const WN_CHAR* _name) const {
    for(std::vector<WNFunctionDefinition*>::const_iterator i = mFunctions.begin(); i != mFunctions.end(); ++i) {
        if(WNStrings::WNStrCmp((*i)->mName, _name) == 0) {
            return(true);
        }
    }
    return(false);
}

eWNTypeError WNCodeModule::AddExternalDefinition(const WN_CHAR* _name, const WN_CHAR* _tag, WNScriptType _thisType, const std::vector<WNScriptType>& _params, const WNScriptType& _return) {
    if(GetFunctionDefinition(_name, _params)) {
        return(eWNAlreadyExists);
    }

    WNFunctionDefinition* def = WN_NEW WNFunctionDefinition();
    def->mName = WNStrings::WNStrNDup(_name, 256);
    def->mTag = WNStrings::WNStrNDup(_tag, 1024);
    def->mReturn = _return;
    def->mTypes.assign(_params.begin(), _params.end());
    def->mThisType = _thisType;
    std::vector<llvm::Type*> scriptTypes;
    for(std::vector<WNScriptType>::const_iterator i = _params.begin(); i != _params.end(); ++i) {
        scriptTypes.push_back((*i)->mLLVMType);
    }

    llvm::FunctionType* fTy = llvm::FunctionType::get(_return->mLLVMType, scriptTypes, false);
    llvm::Function* func = llvm::Function::Create(fTy, llvm::GlobalValue::ExternalLinkage, def->mTag, mModule);
    def->mFunctionType = fTy;
    def->mFunction = func;
    mFunctions.push_back(def);
    return(eWNOK);
}

WNFunctionDefinition* WNCodeModule::GetCallableEquivalent(WNFunctionDefinition* def) const {
    const WN_CHAR* _name = WNStrings::WNStrStr(def->mName, "$");
    _name = _name == NULL? def->mName: _name + 1;   
    
    for(WN_SIZE_T i = 0; i < mFunctions.size(); ++i) {
        const WN_CHAR* lookupFunc = WNStrings::WNStrStr(mFunctions[i]->mName, "$");
        lookupFunc = lookupFunc == NULL? mFunctions[i]->mName: lookupFunc + 1;
        if(WNStrings::WNStrCmp(lookupFunc, _name) == 0) {
            if(mFunctions[i]->mTypes.size() == def->mTypes.size()) {
                if((mFunctions[i]->mThisType != 0) != (def->mThisType != 0)){
                    continue; //if one is a this-func and the other is not
                }
                int adder = 0;
                if(mFunctions[i]->mThisType) {
                    adder = 1;
                }
                if(!WNContainers::ContainerEqual(mFunctions[i]->mTypes.begin() + adder, mFunctions[i]->mTypes.end(),
                                                def->mTypes.begin() + adder, def->mTypes.end())) {
                    continue;
                }
                if(mFunctions[i]->mIsVirtual && def->mIsVirtual) { //If they are both virtual then by definition they cannot hide each other
                    continue;
                }
                if(IsAncestor(mFunctions[i]->mThisType, def->mThisType) || 
                    IsAncestor(def->mThisType, mFunctions[i]->mThisType)) {
                    return(mFunctions[i]);
                }
            }
        }
    }
    return(WN_NULL);
}