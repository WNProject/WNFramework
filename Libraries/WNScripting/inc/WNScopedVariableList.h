////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_SCRIPTING_SCOPED_VARIABLE_LIST_H__
#define __WN_SCRIPTING_SCOPED_VARIABLE_LIST_H__
#include "WNCore/inc/WNBase.h"
#include "WNScripting/inc/WNTypeElement.h"
#include "WNScripting/inc/WNScriptingErrors.h"
namespace llvm {
    class Value;
};
namespace WNLogging {
    class WNLog;
};
namespace WNScripting {
    class WNCodeModule;
    struct WNFunctionDefinition;
    class WNScriptVariable {
        public:
        WNScriptVariable(WNScriptType _type, const WN_CHAR* _name, llvm::Value* _alloca, WN_BOOL _dontClean = false):
            mType(_type),
            mLlvmAlloca(_alloca),
            mDontClean(_dontClean) {
                mName = WNStrings::WNStrNDup(_name, 256);
        }
        virtual ~WNScriptVariable() {
            if(mName != WN_NULL) {
                WN_DELETE(mName);
            }
        }
        const WNScriptType GetType() const { return(mType); }
        const WN_CHAR* GetName() const { return(mName); }
        llvm::Value* GetLocation() const { return(mLlvmAlloca); }
        WN_BOOL Clean() const { return(!mDontClean); }
    private:
        WNScriptType mType;
        WN_CHAR* mName;
        llvm::Value* mLlvmAlloca;
        WN_BOOL mDontClean;
    };

    class WNScopedVariableList {
    public:
        virtual ~WNScopedVariableList(){}
        virtual eWNTypeError PushVariable(WNScriptVariable* _variable) = 0;
        virtual WN_VOID PushScopeBlock(WNCodeModule& _module) = 0;
        virtual eWNTypeError PopScopeBlock(WNCodeModule& _module, WN_BOOL _cleanStack, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) = 0;
        virtual eWNTypeError GenerateReturn(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) = 0;
        virtual const WNScriptVariable* GetVariable(const WN_CHAR* _functionName) = 0;
        virtual WN_VOID ClearScope() = 0;
    };
}
#endif // WN_SCOPED_VARIABLE_LIST_H__
