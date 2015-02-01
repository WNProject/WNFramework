// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNScriptingEngineImpl.h"
#include "WNScripting/inc/WNBuiltinTypeInitialization.h"
#include "WNScripting/src/WNScriptASTLexer.hpp"
#include "WNScripting/src/WNScriptASTParser.hpp"
#include "WNScripting/inc/WNScopedVariableList.h"
#include "WNStrings/inc/WNStrings.h"
#include "WNScripting/inc/WNScriptingFactoryInternal.h"
#include "WNScripting/inc/WNScriptingMemoryManager.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNParameter.h"
#include "WNScripting/inc/WNScriptingInterop.h"
#include "WNFileSystem/inc/WNFile.h"
#include "WNMemory/inc/WNAllocation.h"

#include <algorithm>
#include <vector>

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
#include "llvm/Support/TargetSelect.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ADT/APInt.h"
#include "llvm/IR/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Vectorize.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/LinkAllPasses.h"


#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

using namespace WNScripting;

static WNLogging::WNDefaultLogParameters<WNLogging::eError, 1024, true> mLogParams;

WNScriptingEngineImpl::WNScriptingEngineImpl(): mLogLevel(WNLogging::eWarning), mInternalLogger(&mConsoleLogger, mLogParams), mCompilationLog(&mInternalLogger) {
    mInternalLogger.SetLogLevel(mLogLevel);
}

WNScriptingEngineImpl::~WNScriptingEngineImpl() {
}

wn_void* WNScriptingEngineImpl::CompileLazyFile(wn_void* _scriptingEngine, wn_void* _requestedFunction) {
    WNScriptingEngine* scriptingEngine = reinterpret_cast<WNScriptingEngine*>(_scriptingEngine);
    WNFunctionDefinition* def = reinterpret_cast<WNFunctionDefinition*>(_requestedFunction);
    wn_void* funcPtr = wn_nullptr;
    if( ok != scriptingEngine->GetFunctionPointer(def->mContainedFile->mFileName, def->mName, def->mReturn, def->mTypes, funcPtr)) {
        return(wn_nullptr);
    }
    return(funcPtr);
}

eWNTypeError WNScriptingEngineImpl::Initialize() {
    mInternalLogger.Log(WNLogging::eInfo, 0, "WNScriptingEngine Initialization Started");
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    mTypeManager = WNScriptingFactoryInternal::CreateTypeManager();
    eWNTypeError err = ok;
    WNScriptType throwawayType;
    mTypeManager->RegisterScalarType("-Function", this, 0, throwawayType, 0, sizeof(wn_size_t));
    mTypeManager->RegisterScalarType("-Ptr", this, 0, throwawayType, llvm::Type::getInt8PtrTy(llvm::getGlobalContext()), sizeof(wn_size_t));
    mTypeManager->RegisterScalarType("-Object", this, 0, throwawayType, llvm::Type::getInt8PtrTy(llvm::getGlobalContext()), sizeof(wn_size_t));
    mTypeManager->RegisterStructType("-Null", this, throwawayType);
    mTypeManager->RegisterScalarType("-Array", this, 0, throwawayType, llvm::Type::getInt8PtrTy(llvm::getGlobalContext()), sizeof(wn_size_t));
    mMemoryManager = WN_NEW WNScriptingMemoryManager(*this);

    if(( err = WNBuiltInInitializer::InitializeIntTypes(this, *mTypeManager)) != ok) return err;
    if(( err = WNBuiltInInitializer::InitializeFloatTypes(this, *mTypeManager)) != ok) return err;
    if(( err = WNBuiltInInitializer::InitializeScriptingCasts(this, *mTypeManager)) != ok) return err;
    if(( err = WNBuiltInInitializer::InitializeFunctions(*this)) != ok) return err;

    WNScriptType nullType;
    WNScriptType voidType;
    WNScriptType intType;
    if(ok != mTypeManager->GetTypeByName("-Array", nullType) || ok != mTypeManager->GetTypeByName("Void", voidType) || ok != mTypeManager->GetTypeByName("Int", intType)) {
        return(eWNPlatformError);
    }
    std::vector<WNScriptType> params;
    params.push_back(nullType);
    params.push_back(intType);
    RegisterFunction("$resizeArray", voidType, params, reinterpret_cast<void*>(&Resize));
    if(( err = RegisterCFunction("CompileLazyFunction", CompileLazyFile)) != ok) return err;
    mInternalLogger.Log(WNLogging::eInfo, 0, "WNScriptingEngine Initialization Finished");
    return(ok);
}

WNCodeModule* WNScriptingEngineImpl::GetCompiledModule(const wn_char* _file) const {
    wn_char* c = WNStrings::WNStrNDup(_file, 1024);
    WNFileSystem::WNFile::CollapseFolderStructure(c);

    for(std::vector<std::pair<wn_char*, WNCodeModule*> >::const_iterator i = mFileList.begin(); i != mFileList.end(); ++i) {
        if(WNStrings::WNStrCmp(c, i->first) == 0){
            wn::free(c);
            return(i->second);
        }
    }
    wn::free(c);
    return(wn_nullptr);
}

eWNTypeError WNScriptingEngineImpl::DumpIntoModule(WNCodeModule*& _module, bool mCurrentFile, WNPreprocessedFile* _preprocessedFile, std::deque<WNPreprocessedFile*>& _nextToProcess) {
    if(_preprocessedFile->mMarked) {
        return(ok);
    }

    for(std::vector<WNPreprocessedFile*>::iterator i = _preprocessedFile->mIncludedFiles.begin(); i != _preprocessedFile->mIncludedFiles.end(); ++i) {
        _nextToProcess.push_back(*i);
    }

    for(std::vector<WNFunctionDefinition*>::iterator i = _preprocessedFile->mFunctionDefinitions.begin(); i != _preprocessedFile->mFunctionDefinitions.end(); ++i) {
        if(mCurrentFile) {
            (*i)->mCurrentFile = true;
        } else {
            (*i)->mCurrentFile = false;
        }
        (*i)->mFunction = wn_nullptr;
        eWNTypeError err = ok;
        WNFunctionDefinition* def;
        if(ok != (err = _module->AddExternalScriptFunction(*i, def))) {
            if(eWNAlreadyExists == err) {
                mCompilationLog->Log(WNLogging::eError, 0, "Error could not add function ", (*i)->mName, " because it would hide/be hidden by ", def->mName);
            } else {
                mCompilationLog->Log(WNLogging::eError, 0, "Error could not add function ", (*i)->mName);
            }
            return(err);

        }
    }

    _preprocessedFile->mMarked = true;
    return(ok);
}

eWNTypeError WNScriptingEngineImpl::DumpPreprocessedfile(WNCodeModule*& _module, WNPreprocessedFile* _preprocessedFile) {

    for(std::vector<WNPreprocessedFile*>::iterator i = mPreprocessedFiles.begin(); i != mPreprocessedFiles.end(); ++i) {
        (*i)->mMarked = false;
    }

    std::deque<WNPreprocessedFile*> toProcess;

    eWNTypeError err = ok;

    if(ok != (err = DumpIntoModule(_module, true, _preprocessedFile, toProcess))) {
        return(err);
    }
    for(std::vector<WNPreprocessedFile*>::iterator i = _preprocessedFile->mIncludedFiles.begin(); i != _preprocessedFile->mIncludedFiles.end(); ++i) {
        toProcess.push_back(*i);
    }
    while(!toProcess.empty()) {
        if(ok != (err = DumpIntoModule(_module, false, *toProcess.begin(), toProcess))) {
            return(err);
        }
        toProcess.pop_front();
    }
    return(ok);
}

eWNTypeError WNScriptingEngineImpl::LinkStructs(WNCodeModule*& _module, std::list<WNStruct*>& _toBeLinked) {
    wn_size_t listSize = _toBeLinked.size();
    WNTypeManager& manager = _module->GetTypeManager();
    eWNTypeError err = ok;
    mCompilationLog->Log(WNLogging::eInfo, 0, "Starting type link ", listSize);
    while(listSize > 0) {
        for(std::list<WNStruct*>::iterator i = _toBeLinked.begin(); i != _toBeLinked.end(); ) { //intentionally not incrementing here (may be removing)
            if(ok == (err = (*i)->FinalizeStruct(manager, *mCompilationLog))) {
                i = _toBeLinked.erase(i);
            }
            else if (err != eWNUnknownResolution) {
                std::vector<WNPreprocessedFile*> toRemoveFiles;
                std::vector<WNScriptType> toRemoveTypes;
                for(std::list<WNStruct*>::iterator i = _toBeLinked.begin(); i != _toBeLinked.end(); ++i) {
                    if((*i)->GetType()->mContainedFile != wn_nullptr) {
                        for(std::vector<WNScriptType>::iterator j = (*i)->GetType()->mContainedFile->mExposedTypes.begin();
                            j != (*i)->GetType()->mContainedFile->mExposedTypes.end(); ++j) {
                            toRemoveTypes.push_back(*j);
                        }
                        toRemoveFiles.push_back((*i)->GetType()->mContainedFile);
                    }
                }
                std::sort(toRemoveFiles.begin(), toRemoveFiles.end());
                std::sort(toRemoveTypes.begin(), toRemoveTypes.end());
                std::unique(toRemoveFiles.begin(), toRemoveFiles.end());
                std::unique(toRemoveTypes.begin(), toRemoveTypes.end());
                for(std::vector<WNScriptType>::iterator i = toRemoveTypes.begin(); i != toRemoveTypes.end(); ++i) {
                    mTypeManager->RemoveType(*i);
                }
                for(std::vector<WNPreprocessedFile*>::iterator i = toRemoveFiles.begin(); i != toRemoveFiles.end(); ++i) {
                    std::vector<WNPreprocessedFile*>::iterator toRemove = std::find(mPreprocessedFiles.begin(), mPreprocessedFiles.end(), *i);
                    if(toRemove != mPreprocessedFiles.end())
                    {
                        mPreprocessedFiles.erase(toRemove);
                    }

                    WN_DELETE(*i);
                }
                return(err);
            } else {
                ++i;
            }
        }

        if(listSize == _toBeLinked.size()) { //nothing has been removed, this is bad
            mCompilationLog->Log(WNLogging::eError, 0, "Error could not find valid resolution order for ", listSize, " class");
            for(std::list<WNStruct*>::iterator i = _toBeLinked.begin(); i != _toBeLinked.end(); ++i) {
                mCompilationLog->Log(WNLogging::eError, 0, "Class ", (*i)->GetName());
            }
            return(eWNBadType);
        }
        listSize = _toBeLinked.size();
        mCompilationLog->Log(WNLogging::eDebug, 0, "After one iteration only ", listSize, " structs to be linked");
    }
    mCompilationLog->Log(WNLogging::eInfo, 0, "Completed type link ", listSize);
    return(ok);
}

eWNTypeError WNScriptingEngineImpl::PreprocessFile(const wn_char* _file, WNCodeModule*& _module, WNPreprocessedFile*& _outFile, std::list<WNStruct*>& _toBeLinked) {
    char cBuff[2048] ;
    WNStrings::WNStrNCpy(cBuff, _file, 2047);
    WNFileSystem::WNFile::CollapseFolderStructure(cBuff);

    eWNTypeError err = ok;

    for(std::vector<WNPreprocessedFile*>::iterator i = mPreprocessedFiles.begin(); i != mPreprocessedFiles.end(); ++i) {
        if(WNStrings::WNStrNCmp((*i)->mFileName, cBuff, 1024) == 0) {
            _outFile = *i;
            return(ok);
        }
    }
    mInternalLogger.Log(WNLogging::eInfo, 0, "WNScriptingEngine Preprocessing File: ", _file);
    WNFileSystem::WNFile f;
    if(WNFileSystem::WNFile::ok != f.OpenFile(cBuff, WNFileSystem::WNFile::eWNFMRead)) {
        return(eWNInvalidFile);
    }
    wn_char* c = WNStrings::WNStrNDup(cBuff, 1024);

    const wn_char* buff = f.GetDataBuffer();
    WNScriptASTLexer::InputStreamType input(reinterpret_cast<const ANTLR_UINT8*>(buff), ANTLR_ENC_8BIT, static_cast<ANTLR_UINT32>(f.GetFileSize()), reinterpret_cast<ANTLR_UINT8*>(c));

    WNScriptASTLexer lexer(&input);
    WNScriptASTParser::TokenStreamType tStream(ANTLR_SIZE_HINT, lexer.get_tokSource() );
    WNScriptASTParser parser(&tStream);

    WNScriptFile* scriptFile = parser.program();
    if(parser.getNumberOfSyntaxErrors() > 0 ||
        lexer.getNumberOfSyntaxErrors() > 0 ){
        wn::free(c);
        return(error);
    }

    std::vector<WNScriptType> containedTypes;
    std::vector<WNFunctionDefinition*> containedFunctions;

    WNPreprocessedFile* preFile = WN_NEW WNPreprocessedFile();
    preFile->mFileName = c;

    if(ok != (err = scriptFile->PreProcess(*_module, preFile->mExposedTypes, preFile->mFunctionDefinitions, *mCompilationLog))) {
        return(err);
    }
    mPreprocessedFiles.push_back(preFile);
    _outFile = preFile;
    wn_char* folderName = WNFileSystem::WNFile::GetFolderName(cBuff);
    for(const WNScriptLinkedList<wn_char>::WNScriptLinkedListNode* i = scriptFile->GetFirstInclude(); i != wn_nullptr; i = i->next) {
        wn_char filename[2048];
        WNStrings::WNStrNCpy(filename, folderName, WNStrings::WNStrLen(folderName) + 1);
        WNStrings::WNStrNCat(filename, "/", sizeof(filename) - WNStrings::WNStrLen(filename) - 1);
        WNStrings::WNStrNCat(filename, i->value+1, sizeof(filename) - WNStrings::WNStrLen(filename) - 1);
        filename[strlen(filename) - 1] = '\0';
        WNFileSystem::WNFile::CollapseFolderStructure(filename);
        WNPreprocessedFile* includedFile;
        if(ok != (err = PreprocessFile(filename, _module, includedFile, _toBeLinked))) {
            if(preFile) {
                for(std::vector<WNScriptType>::iterator i = preFile->mExposedTypes.begin(); i != preFile->mExposedTypes.end(); ++i) {
                    mTypeManager->RemoveType(*i);
                }
            }
            std::vector<WNPreprocessedFile*>::iterator toRemove = std::find(mPreprocessedFiles.begin(), mPreprocessedFiles.end(), preFile);
            if(toRemove != mPreprocessedFiles.end())
            {
                mPreprocessedFiles.erase(toRemove);
            }
            delete(preFile);
            _outFile = wn_nullptr;
            return(err);
        }
        preFile->mIncludedFiles.push_back(includedFile);
    }

    if(ok != (err = scriptFile->FinalizePreProcess(*_module, preFile->mFunctionDefinitions, _toBeLinked, *mCompilationLog))) {
        return(err);
    }

    for(std::vector<WNFunctionDefinition*>::iterator i = preFile->mFunctionDefinitions.begin(); i != preFile->mFunctionDefinitions.end(); ++i) {
        (*i)->mContainedFile = preFile;
    }
    for(std::vector<WNScriptType>::iterator i = preFile->mExposedTypes.begin(); i != preFile->mExposedTypes.end(); ++i) {
        (*i)->mContainedFile = preFile;
    }

    return(ok);
}

eWNTypeError WNScriptingEngineImpl::CompileFile(const wn_char* _file) {
    WNCodeModule* module;
    if(GetCompiledModule(_file)){
        return(ok);
    }
    return(CompileFile(_file, module));
}

eWNTypeError WNScriptingEngineImpl::CompileFile(const wn_char* _file, WNCodeModule*& _module) {
    if((_module = GetCompiledModule(_file)) != wn_nullptr) {
        return(ok);
    }
    WNFileSystem::WNFile f;
    if(WNFileSystem::WNFile::ok != f.OpenFile(_file, WNFileSystem::WNFile::eWNFMRead)) {
        return(eWNInvalidFile);
    }
    mInternalLogger.Log(WNLogging::eInfo, 0, "WNScriptingEngine Started Compiling File: ", _file);
    wn_char* c = WNStrings::WNStrNDup(_file, 1024);
    WNFileSystem::WNFile::CollapseFolderStructure(c);

    const wn_char* buff = f.GetDataBuffer();
    WNScriptASTLexer::InputStreamType input(reinterpret_cast<const ANTLR_UINT8*>(buff), ANTLR_ENC_8BIT, static_cast<ANTLR_UINT32>(f.GetFileSize()), reinterpret_cast<ANTLR_UINT8*>(c));

    WNScriptASTLexer lexer(&input);
    WNScriptASTParser::TokenStreamType tStream(ANTLR_SIZE_HINT, lexer.get_tokSource() );
    WNScriptASTParser parser(&tStream);

    WNScriptFile* scriptFile = parser.program();
    if(parser.getNumberOfSyntaxErrors() > 0 ||
        lexer.getNumberOfSyntaxErrors() > 0 ){
        WN_DELETE(scriptFile);
        wn::free(c);
        return(error);
    }

    WNScopedVariableList* variableList = WNScriptingFactoryInternal::CreateScopedVariableList();
    _module = WN_NEW WNCodeModule(*mTypeManager, *variableList, this);
    if(ok != _module->Initialize(eOptimized, *mMemoryManager)) {
        WN_DELETE(scriptFile);
        wn::free(c);
        WN_DELETE(_module);
        return(error);
    }


    eWNTypeError err = ok;
    WNPreprocessedFile* preFile = wn_nullptr;
    std::list<WNStruct*> unlinkedStructs;
    if(ok != (err = PreprocessFile(_file,_module, preFile, unlinkedStructs))) {
        if(preFile) {
            for(std::vector<WNScriptType>::iterator i = preFile->mExposedTypes.begin(); i != preFile->mExposedTypes.end(); ++i) {
                mTypeManager->RemoveType(*i);
            }
        }
        return(err);
    }

    if(ok != (err = LinkStructs(_module, unlinkedStructs))) {
        return(err);
    }

    if(ok != (err = DumpPreprocessedfile(_module, preFile))) {
        if(preFile) {
            for(std::vector<WNScriptType>::iterator i = preFile->mExposedTypes.begin(); i != preFile->mExposedTypes.end(); ++i) {
                mTypeManager->RemoveType(*i);
            }
        }
        return(err);
    }

    for(std::vector<WNFunctionRegistry>::const_iterator i = mRegisteredFunctions.begin(); i != mRegisteredFunctions.end(); ++i) {
        if(ok != (err = _module->AddExternalDefinition(i->mFunctionName, i->mRegisteredFunctionTag, i->mThisType, i->mParams, i->mRetParam))) {
            if(preFile) {
                for(std::vector<WNScriptType>::iterator i = preFile->mExposedTypes.begin(); i != preFile->mExposedTypes.end(); ++i) {
                    mTypeManager->RemoveType(*i);
                }
            }
            wn::free(c);
            return(err);
        }
    }

    if(ok != (err = scriptFile->DumpHeaders(*_module, *mCompilationLog))) {
        if(preFile) {
            for(std::vector<WNScriptType>::iterator i = preFile->mExposedTypes.begin(); i != preFile->mExposedTypes.end(); ++i) {
                mTypeManager->RemoveType(*i);
            }
        }
        wn::free(c);
        WN_DELETE(_module);

        return(err);
    }

    if(ok != (err = scriptFile->GenerateCode(*_module, *mCompilationLog))) {
        if(preFile) {
            for(std::vector<WNScriptType>::iterator i = preFile->mExposedTypes.begin(); i != preFile->mExposedTypes.end(); ++i) {
                mTypeManager->RemoveType(*i);
            }
        }
        wn::free(c);
        WN_DELETE(_module);
        return(err);
    }

    llvm::Module* module = _module->GetModule();
    llvm::FunctionPassManager mFPM(module);

    module->setDataLayout(_module->GetExecutionEngine()->getDataLayout());

    mFPM.add(new llvm::DataLayoutPass(module));
    mFPM.add(llvm::createBasicAliasAnalysisPass());
    mFPM.add(llvm::createConstantPropagationPass());
    mFPM.add(llvm::createPromoteMemoryToRegisterPass());
    mFPM.add(llvm::createInstructionCombiningPass());
    mFPM.add(llvm::createReassociatePass());
    mFPM.add(llvm::createGVNPass());
    mFPM.add(llvm::createCFGSimplificationPass());
    mFPM.add(llvm::createLoopSimplifyPass());

    mFPM.doInitialization();

    for(llvm::Module::iterator i = _module->GetModule()->begin();
        i != _module->GetModule()->end(); ++i){
        mFPM.run(*i);
    }
    llvm::PassManager manager;
    manager.add(llvm::createFunctionAttrsPass());
    manager.add(llvm::createFunctionInliningPass());
    manager.run(*_module->GetModule());

    _module->GetExecutionEngine()->finalizeObject();

    WN_DELETE(variableList);
    WN_DELETE(scriptFile);
    mFileList.push_back(std::pair<wn_char*, WNCodeModule*>(c, _module));

    for(wn_size_t i = 0; i < preFile->mFunctionDefinitions.size(); ++i) {
        preFile->mFunctionDefinitions[i]->mFunctionPointer = _module->GetExecutionEngine()->getPointerToFunction(preFile->mFunctionDefinitions[i]->mFunction);
    }

    for(wn_size_t i = 0; i < preFile->mExposedTypes.size(); ++i) {
        char exposedName[1024];
        exposedName[0] = '\0';
        WNStrings::WNStrCat(exposedName, preFile->mExposedTypes[i]->mName);
        WNStrings::WNStrCat(exposedName, "Destr");
        //Find all the destructors
        std::vector<WNFunctionDefinition*>::iterator func = std::find_if(preFile->mFunctionDefinitions.begin(), preFile->mFunctionDefinitions.end(), [&exposedName](WNFunctionDefinition* f)
            {
                return (WNStrings::WNStrCmp(f->mName, exposedName) == 0);
            }
        );
        if(func == preFile->mFunctionDefinitions.end())
        {
            mInternalLogger.Log(WNLogging::eCritical, 0, "WNScriptingEngine Failed to find destructor that it would have generated in: ", _file);
        }
        reinterpret_cast<wn_void**>(preFile->mExposedTypes[i]->mStoredVTable)[0] = (*func)->mFunctionPointer;
        reinterpret_cast<WNScriptType*>(preFile->mExposedTypes[i]->mStoredVTable)[1] = preFile->mExposedTypes[i];
        std::vector<WNContainedStructType>& st = preFile->mExposedTypes[i]->mStructTypes;
        const llvm::StructLayout* layout = _module->GetExecutionEngine()->getDataLayout()->getStructLayout(preFile->mExposedTypes[i]->mLLVMStructType);
        for(wn_uint32 j = 0; j < static_cast<wn_uint32>(st.size()); ++j) {
            st[j].mCppOffset = static_cast<wn_int32>(layout->getElementOffset(j));
        }
    }
    mInternalLogger.Log(WNLogging::eInfo, 0, "WNScriptingEngine Finished Compiling File: ", _file);
    return(ok);
}

wn_int32 WNScriptingEngineImpl::GetVirtualFunctionIndex(const wn_char* _functionName, WNScriptType& _type, const std::vector<WNScriptType>& _params) {
    return(WNScripting::GetVirtualFunctionIndex(*mTypeManager, _functionName, _params, _type, false));
}

eWNTypeError WNScriptingEngineImpl::GetExistingFunctionPointer(const wn_char* _file, const wn_char* _functionName, WNScriptType& _retParam, const std::vector<WNScriptType>& _params, void*& _ptr) const {

    WNCodeModule* codeModule = wn_nullptr;
    if((codeModule = GetCompiledModule(_file)) != wn_nullptr) {
        return(eWNDoesNotExist);
    }
    WNFunctionDefinition* f = codeModule->GetFunctionDefinition(_functionName, _params);
    if(!f) {
        mInternalLogger.Log(WNLogging::eWarning, 0, "Function ", _functionName, " does not exist in file", _file);
        return(eWNDoesNotExist);
    }
    WNScriptType mObjectType = GetTypeByName("-Object");

    if(f->mReturn->mLLVMStructType && _retParam == mObjectType) {
        _retParam = f->mReturn;
        _ptr = f->mFunctionPointer;
        return(ok);
    }
    if(!(f->mReturn == _retParam)) {
        mInternalLogger.Log(WNLogging::eWarning, 0, "Function ", _functionName, " does not exist in file", _file);
        return(eWNDoesNotExist);
    }

    _ptr = f->mFunctionPointer;

    return(ok);
}


eWNTypeError WNScriptingEngineImpl::GetFunctionPointer(const wn_char* _file, const wn_char* _functionName, WNScriptType& _retParam, const std::vector<WNScriptType>& _params, void*& _ptr) {

    WNCodeModule* codeModule = wn_nullptr;
    eWNTypeError err = CompileFile(_file, codeModule);
    if(err != ok) {
        printf("Could not compile file %s\n", _file);
        return(err);
    }

    WNFunctionDefinition* f = codeModule->GetFunctionDefinition(_functionName, _params);
    if(!f) {
        mInternalLogger.Log(WNLogging::eWarning, 0, "Function ", _functionName, " does not exist in file", _file);
        return(eWNDoesNotExist);
    }
    WNScriptType mObjectType = GetTypeByName("-Object");

    if(f->mReturn->mLLVMStructType && _retParam == mObjectType) {
        _retParam = f->mReturn;
        _ptr = f->mFunctionPointer;
        return(ok);
    }
    if(!(f->mReturn == _retParam)) {
        mInternalLogger.Log(WNLogging::eWarning, 0, "Function ", _functionName, " does not exist in file", _file);
        return(eWNDoesNotExist);
    }

    _ptr = f->mFunctionPointer;

    return(ok);
}

WNScriptType WNScriptingEngineImpl::GetTypeByName(const wn_char* _typeName) const {
    if(!mTypeManager) {
        return(0);
    }
    WNScriptType t;
    if(ok != mTypeManager->GetTypeByName(_typeName, t)) {
        return(0);
    }
    return(t);
}

WNScriptType WNScriptingEngineImpl::GetArrayOf(WNScriptType _type) {
    if(!mTypeManager) {
        return(0);
    }
    WNScriptType t;
    if(ok != mTypeManager->GetArrayOf(_type, t)) {
        return(0);
    }
    return(t);
}

WNScriptType WNScriptingEngineImpl::GetExistingArrayOf(WNScriptType _type) const {
    if(!mTypeManager) {
        return(0);
    }
    WNScriptType t;
    if(ok != mTypeManager->GetExistingArrayOf(_type, t)) {
        return(0);
    }
    return(t);
}

eWNTypeError WNScriptingEngineImpl::RegisterFunction(const wn_char* _functionName, const WNScriptType _retParam, const std::vector<WNScriptType>& _params, void* _ptr) {
    if(!mTypeManager) {
        mInternalLogger.Log(WNLogging::eCritical, 0, "type manager was not correctly created ", _functionName);
        return(error);
    }

    for(std::vector<WNFunctionRegistry>::const_iterator i = mRegisteredFunctions.begin(); i != mRegisteredFunctions.end(); ++i) {
        if(WNStrings::WNStrNCmp(_functionName, i->mFunctionName, 256) == 0) {
            if(i->mParams.size() != _params.size()) {
                continue;
            }
            std::vector<WNScriptType>::const_iterator newIterator = _params.begin();
            std::vector<WNScriptType>::const_iterator oldIterator = i->mParams.begin();
            for(;newIterator != _params.end() && oldIterator != i->mParams.end(); ++newIterator, ++oldIterator) {
                if(*newIterator != *oldIterator) {
                    break;
                }
                if(!mTypeManager->GetCastingOperation(*newIterator, *oldIterator)) {
                    if(!mTypeManager->GetCastingOperation(*oldIterator, *newIterator)) {
                        break;
                    }
                }
            }
            if(newIterator == _params.end() || oldIterator == i->mParams.end()) {
                mInternalLogger.Log(WNLogging::eWarning, 0, "Function already exists ", _functionName);
                return(eWNAlreadyExists);
            }
        }
    }
    mInternalLogger.Log(WNLogging::eDebug, 0, "WNScriptingEngine Function Registered: ", _functionName);
    wn_size_t len = WNStrings::WNStrLen(_functionName);
    wn_char* functionName = wn::malloc<wn_char>(len + 4 * _params.size() + 2);
    functionName[len + 4*_params.size() + 1] = '\0';
    WNMemory::WNMemCpy(functionName, _functionName, len);
    wn_char* loc = functionName + len + 1;
    functionName[len] = '@';
    for(std::vector<WNScriptType>::const_iterator i = _params.begin(); i != _params.end(); ++i) {
        WNMemory::WNMemCpy(loc, (*i)->mTag, 4);
        loc += 4;
    }
    mRegisteredFunctions.push_back(WNFunctionRegistry());
    mRegisteredFunctions.back().functionPointer = _ptr;
    mRegisteredFunctions.back().mFunctionName = WNStrings::WNStrNDup(_functionName, 256);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               ;
    mRegisteredFunctions.back().mRegisteredFunctionTag = functionName;
    mRegisteredFunctions.back().mParams.assign(_params.begin(), _params.end());
    mRegisteredFunctions.back().mRetParam = _retParam;
    mRegisteredFunctions.back().mThisType = wn_nullptr;
    return(ok);
}
eWNTypeError WNScriptingEngineImpl::RegisterMember(const wn_char* _varName, const WNScriptType _thisType, const WNScriptType _varType, wn_size_t _offset) {
    bool err = false;
    std::vector<WNContainedStructType>::iterator i = std::find_if(_thisType->mStructTypes.begin(), _thisType->mStructTypes.end(), [_varName](WNContainedStructType& t) {
        return(WNStrings::WNStrCmp(_varName, t.mName) == 0);
    });
    if(i != _thisType->mStructTypes.end()) {
        return(eWNAlreadyExists);
    }
    WNContainedStructType t = {WNStrings::WNStrNDup(_varName, 1024), _varType, static_cast<wn_int32>(_offset)};
    _thisType->mStructTypes.push_back(t);
    return(ok);
}

eWNTypeError WNScriptingEngineImpl::RegisterMemberFunction(const wn_char* _functionName, const WNScriptType _thisType, const WNScriptType _retParam, const std::vector<WNScriptType>& _params, void* _ptr) {
    if(!mTypeManager) {
        mInternalLogger.Log(WNLogging::eCritical, 0, "type manager was not correctly created ", _functionName);
        return(error);
    }
    if(!_thisType->mCDestructor) {
        mCompilationLog->Log(WNLogging::eError, 0, "Error type is not a registered C++ type");
        return(invalid_parameters);
    }

    for(std::vector<WNFunctionRegistry>::const_iterator i = mRegisteredFunctions.begin(); i != mRegisteredFunctions.end(); ++i) {
        if(!IsAncestor(i->mThisType, _thisType)) {
            continue;
        }
        const char* funcName = i->mFunctionName;
        funcName = strstr(funcName, "$");
        funcName = (funcName == wn_nullptr)? i->mFunctionName: funcName + 1;
        if(WNStrings::WNStrNCmp(_functionName, funcName, 256) == 0) {
            if(i->mParams.size() != _params.size()) {
                continue;
            }
            std::vector<WNScriptType>::const_iterator newIterator = _params.begin();
            std::vector<WNScriptType>::const_iterator oldIterator = i->mParams.begin();
            for(;newIterator != _params.end() && oldIterator != i->mParams.end(); ++newIterator, ++oldIterator) {
                if(*newIterator != *oldIterator) {
                    break;
                }
                if(!mTypeManager->GetCastingOperation(*newIterator, *oldIterator)) {
                    if(!mTypeManager->GetCastingOperation(*oldIterator, *newIterator)) {
                        break;
                    }
                }
            }
            if(newIterator == _params.end() || oldIterator == i->mParams.end()) {
                mInternalLogger.Log(WNLogging::eWarning, 0, "Function already exists ", _functionName);
                return(eWNAlreadyExists);
            }
        }
    }

    wn_size_t len = WNStrings::WNStrLen(_functionName);
    wn_size_t prefixLen = WNStrings::WNStrLen(_thisType->mName);
    wn_char* functionName = wn::malloc<wn_char>(prefixLen + 1 + len + 4 * _params.size() + 2);
    wn_char* exportedfunctionName = wn::malloc<wn_char>(prefixLen + 1 + len + 1);
    functionName[prefixLen + 1 + len + 4*_params.size() + 1] = '\0';
    WNMemory::WNMemCpy(functionName, _thisType->mName, len);
    functionName[prefixLen] = '$';
    WNMemory::WNMemCpy(functionName + prefixLen + 1, _functionName, len);
    functionName[prefixLen + 1 + len] = '\0'; // so we can print out the name so far
    WNMemory::WNMemCpy(exportedfunctionName, functionName, prefixLen + 1 + len + 1);
    mInternalLogger.Log(WNLogging::eDebug, 0, "WNScriptingEngine Function Registered: ", functionName);
    wn_char* loc = functionName + len + 1 + prefixLen + 1;
    functionName[len + prefixLen + 1] = '@';
    for(std::vector<WNScriptType>::const_iterator i = _params.begin(); i != _params.end(); ++i) {
        WNMemory::WNMemCpy(loc, (*i)->mTag, 4);
        loc += 4;
    }

    mRegisteredFunctions.push_back(WNFunctionRegistry());
    mRegisteredFunctions.back().functionPointer = _ptr;
    mRegisteredFunctions.back().mFunctionName = exportedfunctionName;
    mRegisteredFunctions.back().mRegisteredFunctionTag = functionName;
    mRegisteredFunctions.back().mParams.push_back(_thisType);
    mRegisteredFunctions.back().mParams.insert(mRegisteredFunctions.back().mParams.end(), _params.begin(), _params.end());
    mRegisteredFunctions.back().mRetParam = _retParam;
    mRegisteredFunctions.back().mThisType = _thisType;
    return(ok);
}

void* WNScriptingEngineImpl::GetRegisteredFunction(const wn_char* _functionName) const {
    for(std::vector<WNFunctionRegistry>::const_iterator i = mRegisteredFunctions.begin(); i != mRegisteredFunctions.end(); ++i) {
        if(WNStrings::WNStrNCmp(_functionName, i->mRegisteredFunctionTag, 1024) == 0) {
            return(i->functionPointer);
        }
    }
    mCompilationLog->Log(WNLogging::eInfo, 0, "WNScripting: No Explicit function: ", _functionName);
    return(wn_nullptr);
}


wn_void WNScriptingEngineImpl::SetInternalLogLevel(WNLogging::WNLogLevel _level) {
    mInternalLogger.SetLogLevel(_level);
    mLogLevel = _level;
}

wn_void WNScriptingEngineImpl::SetLogLevel(WNLogging::WNLogLevel _level) {
    mCompilationLog->SetLogLevel(_level);
}

wn_void WNScriptingEngineImpl::SetCompilationLog(WNLogging::WNLog* _compilationLog) {
    if(!_compilationLog)
    {
        mCompilationLog = &mInternalLogger;
    }
    else
    {
        mCompilationLog = _compilationLog;
    }
}

wn_void WNScriptingEngineImpl::BufferFlushCallback(wn_void* _context, const wn_char* buffer, wn_size_t bufferLength, const std::vector<WNLogging::WNLogColorElement>& colors) {
    WNScriptingEngineImpl* impl = reinterpret_cast<WNScriptingEngineImpl*>(_context);
    if(!impl->mErrorCallback.IsValid()) {
        return;
    }
    impl->mErrorCallback.Execute(buffer, bufferLength, colors);
}

eWNTypeError WNScriptingEngineImpl::RegisterExternalType(const wn_char* _typeName, void(*ptr)(void*)) {
    WNScriptType outType;
    eWNTypeError err = ok;
    if(ok != (err = mTypeManager->RegisterCStruct(this, _typeName, outType))){
        return(err);
    }
    WNScriptType nullType = NULL;
    if(ok != (err = mTypeManager->GetTypeByName("-Null", nullType))) {
        return(err);
    }
    WNScriptType boolType = NULL;
    if(ok != (err = mTypeManager->GetTypeByName("Bool", boolType))) {
        return(err);
    }
    WNScriptType functionType = NULL;
    if(ok != (err = mTypeManager->GetTypeByName("-Function", functionType))) {
        return(err);
    }
    outType->mCDestructor = ptr;
    mTypeManager->RegisterAllocationOperator(outType, WN_NEW GenerateDefaultAllocation());
    mTypeManager->RegisterAssignmentOperator(outType, AT_EQ, WN_NEW GenerateStructAssignment());
    mTypeManager->RegisterAssignmentOperator(outType, AT_CHOWN, WN_NEW GenerateStructTransfer());
    mTypeManager->RegisterDestructionOperator(outType, WN_NEW GenerateCStructDestruction());
    mTypeManager->RegisterCastingOperator(nullType, outType, WN_NEW GenerateReinterpretCastOperation(outType));
    mTypeManager->RegisterArithmeticOperator(AR_EQ, outType, outType, WN_NEW GenerateStructCompare(boolType, wn_true));
    mTypeManager->RegisterArithmeticOperator(AR_NEQ, outType, outType, WN_NEW GenerateStructCompare(boolType, wn_false));
    mTypeManager->RegisterIDAccessOperator(outType, WN_NEW GenerateCPPStructAccessor(outType, functionType));
    return(ok);
}

eWNTypeError WNScriptingEngineImpl::RegisterExternalType(const wn_char* _typeName, const wn_char* _parentType, void(*ptr)(void*)) {
    WNScriptType outType;
    eWNTypeError err = ok;
    WNScriptType parentType;
    if(ok != (err = mTypeManager->GetTypeByName(_parentType, parentType))) {
        return(eWNDoesNotExist);
    }
    if(ok != (err = mTypeManager->RegisterCStruct(this, _typeName, outType))){
        return(err);
    }
    WNScriptType nullType = NULL;
    if(ok != (err = mTypeManager->GetTypeByName("-Null", nullType))) {
        return(err);
    }
    WNScriptType boolType = NULL;
    if(ok != (err = mTypeManager->GetTypeByName("Bool", boolType))) {
        return(err);
    }
    WNScriptType functionType = NULL;
    if(ok != (err = mTypeManager->GetTypeByName("-Function", functionType))) {
        return(err);
    }
    outType->mCDestructor = ptr;
    outType->mParentClass = parentType;
    mTypeManager->RegisterAllocationOperator(outType, WN_NEW GenerateDefaultAllocation());
    mTypeManager->RegisterAssignmentOperator(outType, AT_EQ, WN_NEW GenerateStructAssignment());
    mTypeManager->RegisterAssignmentOperator(outType, AT_CHOWN, WN_NEW GenerateStructTransfer());
    mTypeManager->RegisterDestructionOperator(outType, WN_NEW GenerateCStructDestruction());
    mTypeManager->RegisterCastingOperator(nullType, outType, WN_NEW GenerateReinterpretCastOperation(outType));
    mTypeManager->RegisterArithmeticOperator(AR_EQ, outType, outType, WN_NEW GenerateStructCompare(boolType, wn_true));
    mTypeManager->RegisterArithmeticOperator(AR_NEQ, outType, outType, WN_NEW GenerateStructCompare(boolType, wn_false));
    mTypeManager->RegisterIDAccessOperator(outType, WN_NEW GenerateCPPStructAccessor(outType, functionType));
    while(parentType) {
        mTypeManager->RegisterCastingOperator(outType, parentType, WN_NEW GenerateReinterpretCastOperation(parentType));
        parentType = parentType->mParentClass;
    }
    return(ok);
}

eWNTypeError WNScriptingEngineImpl::AddExternalLibs(eWNTypeError(*fPtr)(WNTypeManager&, void*), void* context) {
    if(fPtr == wn_nullptr) {
        return(error);
    }
    return(fPtr(*mTypeManager, context));
}

//virtual eWNTypeError GetFunctionPointer(const wn_char* _file, const wn_char* _functionName, WNScriptType& _retParam, const std::vector<WNScriptType>& _params, void*& _ptr);

eWNTypeError WNScriptingEngineImpl::ConstructScriptingObject(WNScriptType _type, wn_void*& _retVal) const {
    StructInternalType* tp = wn::malloc<StructInternalType>();
    WNMemory::WNMemClrT(tp);
    wn_size_t size = 0;
    for(wn_size_t i = 0; i < _type->mStructTypes.size(); ++i) {
        size += _type->mStructTypes[i].mType->mTypeSize;
    }
    void* sType = wn::malloc(size);
    tp->structLoc = sType;
    void* fPtr;
    wn_char strName[1024];
    wn_size_t nameLen = WNStrings::WNStrLen(_type->mName);
    WNMemory::WNMemCpy(strName, _type->mName, nameLen);
    WNMemory::WNMemCpy(strName + nameLen, "Const", 6);
    std::vector<WNScriptType> params;
    params.push_back(_type);
    WNScriptType retType = wn_nullptr;
    eWNTypeError err = ok;
    if(wn_nullptr == (retType = GetTypeByName("Void"))) {
        return(eWNPlatformError);
    }
    if(ok != (err = GetExistingFunctionPointer(_type->mContainedFile->mFileName, strName, retType, params, fPtr))) {
        return(err);
    }
    wn_void(*castedPtr)(void*) = reinterpret_cast<wn_void(*)(void*)>(fPtr);
    castedPtr(tp);
    _retVal = tp;
    return(ok);
}

eWNTypeError WNScriptingEngineImpl::ConstructScriptingArray(WNScriptType _type, wn_size_t _size, wn_void*& _retVal) const {
    StructInternalType* tp = wn::malloc<StructInternalType>();
    WNMemory::WNMemClrT(tp);
    wn_size_t typeSize = _type->mArrayType->mTypeSize;
    wn_void* sType = wn::malloc(sizeof(wn_size_t) * 2 + (_size * typeSize));
    tp->structLoc = sType;
    *reinterpret_cast<wn_size_t*>(sType) = _size;
    reinterpret_cast<wn_void**>(sType)[1] = _type;
    if(_type->mArrayType->mArrayType || _type->mArrayType->mLLVMStructType) {
        wn_void** items = reinterpret_cast<wn_void**>(sType);
        for(wn_size_t i = 0; i < _size; ++i){
            StructInternalType* nt = wn::malloc<StructInternalType>();
            nt->owner = &(items[i+2]);
            nt->refCount++;
            items[i+2] = nt;
        }
    }
    _retVal = tp;
    return(ok);
}

