////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

WN_VOID* WNScriptingEngineImpl::CompileLazyFile(WN_VOID* _scriptingEngine, WN_VOID* _requestedFunction) {
    WNScriptingEngine* scriptingEngine = reinterpret_cast<WNScriptingEngine*>(_scriptingEngine);
    WNFunctionDefinition* def = reinterpret_cast<WNFunctionDefinition*>(_requestedFunction);
    WN_VOID* funcPtr = WN_NULL;
    if( eWNOK != scriptingEngine->GetFunctionPointer(def->mContainedFile->mFileName, def->mName, def->mReturn, def->mTypes, funcPtr)) {
        return(WN_NULL);
    }
    return(funcPtr);
}

eWNTypeError WNScriptingEngineImpl::Initialize() {
    mInternalLogger.Log(WNLogging::eInfo, 0, "WNScriptingEngine Initialization Started");
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    mTypeManager = WNScriptingFactoryInternal::CreateTypeManager();
    eWNTypeError err = eWNOK;
    WNScriptType throwawayType;
    mTypeManager->RegisterScalarType("-Function", this, 0, throwawayType, 0, sizeof(WN_SIZE_T));
    mTypeManager->RegisterScalarType("-Ptr", this, 0, throwawayType, llvm::Type::getInt8PtrTy(llvm::getGlobalContext()), sizeof(WN_SIZE_T));
    mTypeManager->RegisterScalarType("-Object", this, 0, throwawayType, llvm::Type::getInt8PtrTy(llvm::getGlobalContext()), sizeof(WN_SIZE_T));
    mTypeManager->RegisterStructType("-Null", this, throwawayType);
    mTypeManager->RegisterScalarType("-Array", this, 0, throwawayType, llvm::Type::getInt8PtrTy(llvm::getGlobalContext()), sizeof(WN_SIZE_T));
    mMemoryManager = WN_NEW WNScriptingMemoryManager(*this);

    if(( err = WNBuiltInInitializer::InitializeIntTypes(this, *mTypeManager)) != eWNOK) return err;
    if(( err = WNBuiltInInitializer::InitializeFloatTypes(this, *mTypeManager)) != eWNOK) return err;
    if(( err = WNBuiltInInitializer::InitializeScriptingCasts(this, *mTypeManager)) != eWNOK) return err;
    if(( err = WNBuiltInInitializer::InitializeFunctions(*this)) != eWNOK) return err;
    
    WNScriptType nullType;
    WNScriptType voidType;
    WNScriptType intType;
    if(eWNOK != mTypeManager->GetTypeByName("-Array", nullType) || eWNOK != mTypeManager->GetTypeByName("Void", voidType) || eWNOK != mTypeManager->GetTypeByName("Int", intType)) {
        return(eWNPlatformError);
    }
    std::vector<WNScriptType> params;
    params.push_back(nullType);
    params.push_back(intType);
    RegisterFunction("$resizeArray", voidType, params, reinterpret_cast<void*>(&Resize));
    if(( err = RegisterCFunction("CompileLazyFunction", CompileLazyFile)) != eWNOK) return err;
    mInternalLogger.Log(WNLogging::eInfo, 0, "WNScriptingEngine Initialization Finished");
    return(eWNOK);
}

WNCodeModule* WNScriptingEngineImpl::GetCompiledModule(const WN_CHAR* _file) {
    WN_CHAR* c = WNStrings::WNStrNDup(_file, 1024);
    WNFileSystem::WNFile::CollapseFolderStructure(c);

    for(std::vector<std::pair<WN_CHAR*, WNCodeModule*> >::iterator i = mFileList.begin(); i != mFileList.end(); ++i) {
        if(WNStrings::WNStrCmp(c, i->first) == 0){
            WNMemory::WNFree(c);
            return(i->second);
        }
    }
    WNMemory::WNFree(c);
    return(WN_NULL);
}

eWNTypeError WNScriptingEngineImpl::DumpIntoModule(WNCodeModule*& _module, bool mCurrentFile, WNPreprocessedFile* _preprocessedFile, std::deque<WNPreprocessedFile*>& _nextToProcess) {
    if(_preprocessedFile->mMarked) {
        return(eWNOK);
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
        (*i)->mFunction = WN_NULL;
        eWNTypeError err = eWNOK;
        WNFunctionDefinition* def;
        if(eWNOK != (err = _module->AddExternalScriptFunction(*i, def))) {
            if(eWNAlreadyExists == err) {
                mCompilationLog->Log(WNLogging::eError, 0, "Error could not add function ", (*i)->mName, " because it would hide/be hidden by ", def->mName);
            } else {
                mCompilationLog->Log(WNLogging::eError, 0, "Error could not add function ", (*i)->mName);
            }
            return(err);
            
        }
    }

    _preprocessedFile->mMarked = true;
    return(eWNOK);
}

eWNTypeError WNScriptingEngineImpl::DumpPreprocessedfile(WNCodeModule*& _module, WNPreprocessedFile* _preprocessedFile) {
    
    for(std::vector<WNPreprocessedFile*>::iterator i = mPreprocessedFiles.begin(); i != mPreprocessedFiles.end(); ++i) {
        (*i)->mMarked = false;
    }

    std::deque<WNPreprocessedFile*> toProcess;
    
    eWNTypeError err = eWNOK;
    
    if(eWNOK != (err = DumpIntoModule(_module, true, _preprocessedFile, toProcess))) {
        return(err);
    }
    for(std::vector<WNPreprocessedFile*>::iterator i = _preprocessedFile->mIncludedFiles.begin(); i != _preprocessedFile->mIncludedFiles.end(); ++i) {
        toProcess.push_back(*i);
    }
    while(!toProcess.empty()) {
        if(eWNOK != (err = DumpIntoModule(_module, false, *toProcess.begin(), toProcess))) {
            return(err);
        }
        toProcess.pop_front();
    }
    return(eWNOK);
}

eWNTypeError WNScriptingEngineImpl::LinkStructs(WNCodeModule*& _module, std::list<WNStruct*>& _toBeLinked) {
    WN_SIZE_T listSize = _toBeLinked.size();
    WNTypeManager& manager = _module->GetTypeManager();
    eWNTypeError err = eWNOK;
    mCompilationLog->Log(WNLogging::eInfo, 0, "Starting type link ", listSize);
    while(listSize > 0) {
        for(std::list<WNStruct*>::iterator i = _toBeLinked.begin(); i != _toBeLinked.end(); ) { //intentionally not incrementing here (may be removing)
            if(eWNOK == (err = (*i)->FinalizeStruct(manager, *mCompilationLog))) {
                i = _toBeLinked.erase(i);
            }
            else if (err != eWNUnknownResolution) {
                std::vector<WNPreprocessedFile*> toRemoveFiles;
                std::vector<WNScriptType> toRemoveTypes;
                for(std::list<WNStruct*>::iterator i = _toBeLinked.begin(); i != _toBeLinked.end(); ++i) {
                    if((*i)->GetType()->mContainedFile != WN_NULL) {
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
    return(eWNOK);
}

eWNTypeError WNScriptingEngineImpl::PreprocessFile(const WN_CHAR* _file, WNCodeModule*& _module, WNPreprocessedFile*& _outFile, std::list<WNStruct*>& _toBeLinked) {
    char cBuff[2048] ;
    WNStrings::WNStrNCpy(cBuff, _file, 2047);
    WNFileSystem::WNFile::CollapseFolderStructure(cBuff);

    eWNTypeError err = eWNOK;
    
    for(std::vector<WNPreprocessedFile*>::iterator i = mPreprocessedFiles.begin(); i != mPreprocessedFiles.end(); ++i) {
        if(WNStrings::WNStrNCmp((*i)->mFileName, cBuff, 1024) == 0) {
            _outFile = *i;
            return(eWNOK);
        }
    }
    mInternalLogger.Log(WNLogging::eInfo, 0, "WNScriptingEngine Preprocessing File: ", _file);
    WNFileSystem::WNFile f;
    if(WNFileSystem::WNFile::eWNOK != f.OpenFile(cBuff, WNFileSystem::WNFile::eWNFMRead)) {
        return(eWNInvalidFile);
    }
    WN_CHAR* c = WNStrings::WNStrNDup(cBuff, 1024);

    const WN_CHAR* buff = f.GetDataBuffer();
    WNScriptASTLexer::InputStreamType input(reinterpret_cast<const ANTLR_UINT8*>(buff), ANTLR_ENC_8BIT, static_cast<ANTLR_UINT32>(f.GetFileSize()), reinterpret_cast<ANTLR_UINT8*>(c));

    WNScriptASTLexer lexer(&input);
    WNScriptASTParser::TokenStreamType tStream(ANTLR_SIZE_HINT, lexer.get_tokSource() );
    WNScriptASTParser parser(&tStream);

    WNScriptFile* scriptFile = parser.program();
    if(parser.getNumberOfSyntaxErrors() > 0 ||
        lexer.getNumberOfSyntaxErrors() > 0 ){
        WNMemory::WNFree(c);
        return(eWNError);
    }

    std::vector<WNScriptType> containedTypes;
    std::vector<WNFunctionDefinition*> containedFunctions;

    WNPreprocessedFile* preFile = WN_NEW WNPreprocessedFile();
    preFile->mFileName = c;

    if(eWNOK != (err = scriptFile->PreProcess(*_module, preFile->mExposedTypes, preFile->mFunctionDefinitions, *mCompilationLog))) {
        return(err);
    }
    mPreprocessedFiles.push_back(preFile);
    _outFile = preFile;
    WN_CHAR* folderName = WNFileSystem::WNFile::GetFolderName(cBuff);
    for(const WNScriptLinkedList<WN_CHAR>::WNScriptLinkedListNode* i = scriptFile->GetFirstInclude(); i != WN_NULL; i = i->next) {
        WN_CHAR filename[2048];
        WNStrings::WNStrNCpy(filename, folderName, WNStrings::WNStrLen(folderName) + 1);
        WNStrings::WNStrNCat(filename, "/", sizeof(filename) - WNStrings::WNStrLen(filename) - 1);
        WNStrings::WNStrNCat(filename, i->value+1, sizeof(filename) - WNStrings::WNStrLen(filename) - 1);
        filename[strlen(filename) - 1] = '\0';
        WNFileSystem::WNFile::CollapseFolderStructure(filename);
        WNPreprocessedFile* includedFile;
        if(eWNOK != (err = PreprocessFile(filename, _module, includedFile, _toBeLinked))) {
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
            _outFile = WN_NULL;
            return(err);
        }
        preFile->mIncludedFiles.push_back(includedFile);
    }
    
    if(eWNOK != (err = scriptFile->FinalizePreProcess(*_module, preFile->mFunctionDefinitions, _toBeLinked, *mCompilationLog))) {
        return(err);
    }

    for(std::vector<WNFunctionDefinition*>::iterator i = preFile->mFunctionDefinitions.begin(); i != preFile->mFunctionDefinitions.end(); ++i) {
        (*i)->mContainedFile = preFile;
    }
    for(std::vector<WNScriptType>::iterator i = preFile->mExposedTypes.begin(); i != preFile->mExposedTypes.end(); ++i) {
        (*i)->mContainedFile = preFile;
    }

    return(eWNOK);    
}

eWNTypeError WNScriptingEngineImpl::CompileFile(const WN_CHAR* _file) {
    WNCodeModule* module;
    if(GetCompiledModule(_file)){ 
        return(eWNOK);
    }
    return(CompileFile(_file, module));
}

eWNTypeError WNScriptingEngineImpl::CompileFile(const WN_CHAR* _file, WNCodeModule*& _module) {
    if((_module = GetCompiledModule(_file)) != WN_NULL) {
        return(eWNOK);
    }
    WNFileSystem::WNFile f;
    if(WNFileSystem::WNFile::eWNOK != f.OpenFile(_file, WNFileSystem::WNFile::eWNFMRead)) {
        return(eWNInvalidFile);
    }
    mInternalLogger.Log(WNLogging::eInfo, 0, "WNScriptingEngine Started Compiling File: ", _file);
    WN_CHAR* c = WNStrings::WNStrNDup(_file, 1024);
    WNFileSystem::WNFile::CollapseFolderStructure(c);

    const WN_CHAR* buff = f.GetDataBuffer();
    WNScriptASTLexer::InputStreamType input(reinterpret_cast<const ANTLR_UINT8*>(buff), ANTLR_ENC_8BIT, static_cast<ANTLR_UINT32>(f.GetFileSize()), reinterpret_cast<ANTLR_UINT8*>(c));

    WNScriptASTLexer lexer(&input);
    WNScriptASTParser::TokenStreamType tStream(ANTLR_SIZE_HINT, lexer.get_tokSource() );
    WNScriptASTParser parser(&tStream);

    WNScriptFile* scriptFile = parser.program();
    if(parser.getNumberOfSyntaxErrors() > 0 ||
        lexer.getNumberOfSyntaxErrors() > 0 ){
        WN_DELETE(scriptFile);
        WNMemory::WNFree(c);
        return(eWNError);
    }

    WNScopedVariableList* variableList = WNScriptingFactoryInternal::CreateScopedVariableList();
    _module = WN_NEW WNCodeModule(*mTypeManager, *variableList, this);
    if(eWNOK != _module->Initialize(eOptimized, *mMemoryManager)) {
        WN_DELETE(scriptFile);
        WNMemory::WNFree(c);
        WN_DELETE(_module);
        return(eWNError);
    }
    

    eWNTypeError err = eWNOK;
    WNPreprocessedFile* preFile = WN_NULL;
    std::list<WNStruct*> unlinkedStructs;
    if(eWNOK != (err = PreprocessFile(_file,_module, preFile, unlinkedStructs))) {
        if(preFile) {
            for(std::vector<WNScriptType>::iterator i = preFile->mExposedTypes.begin(); i != preFile->mExposedTypes.end(); ++i) {
                mTypeManager->RemoveType(*i);
            }
        }
        return(err);
    }
    
    if(eWNOK != (err = LinkStructs(_module, unlinkedStructs))) {
        return(err);    
    }

    if(eWNOK != (err = DumpPreprocessedfile(_module, preFile))) {
        if(preFile) {
            for(std::vector<WNScriptType>::iterator i = preFile->mExposedTypes.begin(); i != preFile->mExposedTypes.end(); ++i) {
                mTypeManager->RemoveType(*i);
            }
        }
        return(err);
    }

    for(std::vector<WNFunctionRegistry>::const_iterator i = mRegisteredFunctions.begin(); i != mRegisteredFunctions.end(); ++i) {
        if(eWNOK != (err = _module->AddExternalDefinition(i->mFunctionName, i->mRegisteredFunctionTag, i->mThisType, i->mParams, i->mRetParam))) {
            if(preFile) {
                for(std::vector<WNScriptType>::iterator i = preFile->mExposedTypes.begin(); i != preFile->mExposedTypes.end(); ++i) {
                    mTypeManager->RemoveType(*i);
                }
            }
            WNMemory::WNFree(c);
            return(err);
        }
    }
    
    if(eWNOK != (err = scriptFile->DumpHeaders(*_module, *mCompilationLog))) {
        if(preFile) {
            for(std::vector<WNScriptType>::iterator i = preFile->mExposedTypes.begin(); i != preFile->mExposedTypes.end(); ++i) {
                mTypeManager->RemoveType(*i);
            }
        }
        WNMemory::WNFree(c);    
        WN_DELETE(_module);
        
        return(err);
    }

    if(eWNOK != (err = scriptFile->GenerateCode(*_module, *mCompilationLog))) {
        if(preFile) {
            for(std::vector<WNScriptType>::iterator i = preFile->mExposedTypes.begin(); i != preFile->mExposedTypes.end(); ++i) {
                mTypeManager->RemoveType(*i);
            }
        }
        WNMemory::WNFree(c);    
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
    mFileList.push_back(std::pair<WN_CHAR*, WNCodeModule*>(c, _module));
    
    for(WN_SIZE_T i = 0; i < preFile->mFunctionDefinitions.size(); ++i) {
        preFile->mFunctionDefinitions[i]->mFunctionPointer = _module->GetExecutionEngine()->getPointerToFunction(preFile->mFunctionDefinitions[i]->mFunction);
    }
    
    for(WN_SIZE_T i = 0; i < preFile->mExposedTypes.size(); ++i) {
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
        reinterpret_cast<WN_VOID**>(preFile->mExposedTypes[i]->mStoredVTable)[0] = (*func)->mFunctionPointer;
        reinterpret_cast<WNScriptType*>(preFile->mExposedTypes[i]->mStoredVTable)[1] = preFile->mExposedTypes[i]; 
        std::vector<WNContainedStructType>& st = preFile->mExposedTypes[i]->mStructTypes;
        const llvm::StructLayout* layout = _module->GetExecutionEngine()->getDataLayout()->getStructLayout(preFile->mExposedTypes[i]->mLLVMStructType);
        for(WN_UINT32 j = 0; j < static_cast<WN_UINT32>(st.size()); ++j) {
            st[j].mCppOffset = static_cast<WN_INT32>(layout->getElementOffset(j));
        }
    }
    mInternalLogger.Log(WNLogging::eInfo, 0, "WNScriptingEngine Finished Compiling File: ", _file);
    return(eWNOK);
}

WN_INT32 WNScriptingEngineImpl::GetVirtualFunctionIndex(const WN_CHAR* _functionName, WNScriptType& _type, const std::vector<WNScriptType>& _params) {
    return(WNScripting::GetVirtualFunctionIndex(*mTypeManager, _functionName, _params, _type, false));
}

eWNTypeError WNScriptingEngineImpl::GetFunctionPointer(const WN_CHAR* _file, const WN_CHAR* _functionName, WNScriptType& _retParam, const std::vector<WNScriptType>& _params, void*& _ptr) {
    
    WNCodeModule* codeModule = WN_NULL;
    eWNTypeError err = CompileFile(_file, codeModule);
    if(err != eWNOK) {
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
        return(eWNOK);
    }
    if(!(f->mReturn == _retParam)) {
        mInternalLogger.Log(WNLogging::eWarning, 0, "Function ", _functionName, " does not exist in file", _file);
        return(eWNDoesNotExist);
    }
    
    _ptr = f->mFunctionPointer;
   
    return(eWNOK);
}

WNScriptType WNScriptingEngineImpl::GetTypeByName(const WN_CHAR* _typeName) {
    if(!mTypeManager) {
        return(0);
    }
    WNScriptType t;
    if(eWNOK != mTypeManager->GetTypeByName(_typeName, t)) {
        return(0);
    }
    return(t);
}

WNScriptType WNScriptingEngineImpl::GetArrayOf(WNScriptType _type) {
    if(!mTypeManager) {
        return(0);
    }
    WNScriptType t;
    if(eWNOK != mTypeManager->GetArrayOf(_type, t)) {
        return(0);
    }
    return(t);
}

eWNTypeError WNScriptingEngineImpl::RegisterFunction(const WN_CHAR* _functionName, const WNScriptType _retParam, const std::vector<WNScriptType>& _params, void* _ptr) {
    if(!mTypeManager) {
        mInternalLogger.Log(WNLogging::eCritical, 0, "Type manager was not correctly created ", _functionName);
        return(eWNError);
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
    WN_SIZE_T len = WNStrings::WNStrLen(_functionName);
    WN_CHAR* functionName = WNMemory::WNMallocT<WN_CHAR>(len + 4 * _params.size() + 2);
    functionName[len + 4*_params.size() + 1] = '\0';
    WNMemory::WNMemCpy(functionName, _functionName, len);
    WN_CHAR* loc = functionName + len + 1;
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
    mRegisteredFunctions.back().mThisType = WN_NULL;
    return(eWNOK);
}
eWNTypeError WNScriptingEngineImpl::RegisterMember(const WN_CHAR* _varName, const WNScriptType _thisType, const WNScriptType _varType, WN_SIZE_T _offset) {
    bool err = false;
    std::vector<WNContainedStructType>::iterator i = std::find_if(_thisType->mStructTypes.begin(), _thisType->mStructTypes.end(), [_varName](WNContainedStructType& t) {
        return(WNStrings::WNStrCmp(_varName, t.mName) == 0);
    });
    if(i != _thisType->mStructTypes.end()) {
        return(eWNAlreadyExists);
    }
    WNContainedStructType t = {WNStrings::WNStrNDup(_varName, 1024), _varType, static_cast<WN_INT32>(_offset)};
    _thisType->mStructTypes.push_back(t);
    return(eWNOK);
}

eWNTypeError WNScriptingEngineImpl::RegisterMemberFunction(const WN_CHAR* _functionName, const WNScriptType _thisType, const WNScriptType _retParam, const std::vector<WNScriptType>& _params, void* _ptr) {
    if(!mTypeManager) {
        mInternalLogger.Log(WNLogging::eCritical, 0, "Type manager was not correctly created ", _functionName);
        return(eWNError);
    }
    if(!_thisType->mCDestructor) {
        mCompilationLog->Log(WNLogging::eError, 0, "Error type is not a registered C++ type");
        return(eWNInvalidParameters);
    }

    for(std::vector<WNFunctionRegistry>::const_iterator i = mRegisteredFunctions.begin(); i != mRegisteredFunctions.end(); ++i) {
        if(!IsAncestor(i->mThisType, _thisType)) {
            continue;
        }
        const char* funcName = i->mFunctionName;
        funcName = strstr(funcName, "$");
        funcName = (funcName == WN_NULL)? i->mFunctionName: funcName + 1;
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
    
    WN_SIZE_T len = WNStrings::WNStrLen(_functionName);
    WN_SIZE_T prefixLen = WNStrings::WNStrLen(_thisType->mName);
    WN_CHAR* functionName = WNMemory::WNMallocT<WN_CHAR>(prefixLen + 1 + len + 4 * _params.size() + 2);
    WN_CHAR* exportedfunctionName = WNMemory::WNMallocT<WN_CHAR>(prefixLen + 1 + len + 1);
    functionName[prefixLen + 1 + len + 4*_params.size() + 1] = '\0';
    WNMemory::WNMemCpy(functionName, _thisType->mName, len);
    functionName[prefixLen] = '$';
    WNMemory::WNMemCpy(functionName + prefixLen + 1, _functionName, len);
    functionName[prefixLen + 1 + len] = '\0'; // so we can print out the name so far
    WNMemory::WNMemCpy(exportedfunctionName, functionName, prefixLen + 1 + len + 1);
    mInternalLogger.Log(WNLogging::eDebug, 0, "WNScriptingEngine Function Registered: ", functionName);
    WN_CHAR* loc = functionName + len + 1 + prefixLen + 1;
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
    return(eWNOK);
}

void* WNScriptingEngineImpl::GetRegisteredFunction(const WN_CHAR* _functionName) const {
    for(std::vector<WNFunctionRegistry>::const_iterator i = mRegisteredFunctions.begin(); i != mRegisteredFunctions.end(); ++i) {
        if(WNStrings::WNStrNCmp(_functionName, i->mRegisteredFunctionTag, 1024) == 0) {
            return(i->functionPointer);
        }
    }
    mCompilationLog->Log(WNLogging::eInfo, 0, "WNScripting: No Explicit function: ", _functionName);
    return(WN_NULL);
}


WN_VOID WNScriptingEngineImpl::SetInternalLogLevel(WNLogging::WNLogLevel _level) {
    mInternalLogger.SetLogLevel(_level);
    mLogLevel = _level;
}

WN_VOID WNScriptingEngineImpl::SetLogLevel(WNLogging::WNLogLevel _level) {
    mCompilationLog->SetLogLevel(_level);
}

WN_VOID WNScriptingEngineImpl::SetCompilationLog(WNLogging::WNLog* _compilationLog) {
    if(!_compilationLog) 
    {
        mCompilationLog = &mInternalLogger;
    }
    else
    {
        mCompilationLog = _compilationLog;
    }
}

WN_VOID WNScriptingEngineImpl::BufferFlushCallback(WN_VOID* _context, const WN_CHAR* buffer, WN_SIZE_T bufferLength, const std::vector<WNLogging::WNLogColorElement>& colors) {
    WNScriptingEngineImpl* impl = reinterpret_cast<WNScriptingEngineImpl*>(_context);
    if(!impl->mErrorCallback.IsValid()) {
        return;
    }
    impl->mErrorCallback.Execute(buffer, bufferLength, colors);   
}

eWNTypeError WNScriptingEngineImpl::RegisterExternalType(const WN_CHAR* _typeName, void(*ptr)(void*)) {
    WNScriptType outType;
    eWNTypeError err = eWNOK;
    if(eWNOK != (err = mTypeManager->RegisterCStruct(this, _typeName, outType))){
        return(err);
    }
    WNScriptType nullType = NULL;
    if(eWNOK != (err = mTypeManager->GetTypeByName("-Null", nullType))) {
        return(err);
    }
    WNScriptType boolType = NULL;
    if(eWNOK != (err = mTypeManager->GetTypeByName("Bool", boolType))) {
        return(err);
    }
    WNScriptType functionType = NULL;
    if(eWNOK != (err = mTypeManager->GetTypeByName("-Function", functionType))) {
        return(err);
    }
    outType->mCDestructor = ptr;
    mTypeManager->RegisterAllocationOperator(outType, WN_NEW GenerateDefaultAllocation());
    mTypeManager->RegisterAssignmentOperator(outType, AT_EQ, WN_NEW GenerateStructAssignment());
    mTypeManager->RegisterAssignmentOperator(outType, AT_CHOWN, WN_NEW GenerateStructTransfer());
    mTypeManager->RegisterDestructionOperator(outType, WN_NEW GenerateCStructDestruction());
    mTypeManager->RegisterCastingOperator(nullType, outType, WN_NEW GenerateReinterpretCastOperation(outType));
    mTypeManager->RegisterArithmeticOperator(AR_EQ, outType, outType, WN_NEW GenerateStructCompare(boolType, WN_TRUE));
    mTypeManager->RegisterArithmeticOperator(AR_NEQ, outType, outType, WN_NEW GenerateStructCompare(boolType, WN_FALSE));
    mTypeManager->RegisterIDAccessOperator(outType, WN_NEW GenerateCPPStructAccessor(outType, functionType));
    return(eWNOK);
}

eWNTypeError WNScriptingEngineImpl::RegisterExternalType(const WN_CHAR* _typeName, const WN_CHAR* _parentType, void(*ptr)(void*)) {
    WNScriptType outType;
    eWNTypeError err = eWNOK;
    WNScriptType parentType;
    if(eWNOK != (err = mTypeManager->GetTypeByName(_parentType, parentType))) {
        return(eWNDoesNotExist);
    }
    if(eWNOK != (err = mTypeManager->RegisterCStruct(this, _typeName, outType))){
        return(err);
    }
    WNScriptType nullType = NULL;
    if(eWNOK != (err = mTypeManager->GetTypeByName("-Null", nullType))) {
        return(err);
    }
    WNScriptType boolType = NULL;
    if(eWNOK != (err = mTypeManager->GetTypeByName("Bool", boolType))) {
        return(err);
    }
    WNScriptType functionType = NULL;
    if(eWNOK != (err = mTypeManager->GetTypeByName("-Function", functionType))) {
        return(err);
    }
    outType->mCDestructor = ptr;
    outType->mParentClass = parentType;
    mTypeManager->RegisterAllocationOperator(outType, WN_NEW GenerateDefaultAllocation());
    mTypeManager->RegisterAssignmentOperator(outType, AT_EQ, WN_NEW GenerateStructAssignment());
    mTypeManager->RegisterAssignmentOperator(outType, AT_CHOWN, WN_NEW GenerateStructTransfer());
    mTypeManager->RegisterDestructionOperator(outType, WN_NEW GenerateCStructDestruction());
    mTypeManager->RegisterCastingOperator(nullType, outType, WN_NEW GenerateReinterpretCastOperation(outType));
    mTypeManager->RegisterArithmeticOperator(AR_EQ, outType, outType, WN_NEW GenerateStructCompare(boolType, WN_TRUE));
    mTypeManager->RegisterArithmeticOperator(AR_NEQ, outType, outType, WN_NEW GenerateStructCompare(boolType, WN_FALSE));
    mTypeManager->RegisterIDAccessOperator(outType, WN_NEW GenerateCPPStructAccessor(outType, functionType));
    while(parentType) {
        mTypeManager->RegisterCastingOperator(outType, parentType, WN_NEW GenerateReinterpretCastOperation(parentType));
        parentType = parentType->mParentClass;
    }
    return(eWNOK);
}

eWNTypeError WNScriptingEngineImpl::AddExternalLibs(eWNTypeError(*fPtr)(WNTypeManager&, void*), void* context) {
    if(fPtr == WN_NULL) {
        return(eWNError);
    }
    return(fPtr(*mTypeManager, context));
}
