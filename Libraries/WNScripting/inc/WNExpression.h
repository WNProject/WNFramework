////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_EXPRESSION_H__
#define __WN_EXPRESSION_H__
#include "WNScripting/inc/WNNode.h"
#include "WNCore/inc/WNTypes.h"
#include "WNScripting/inc/WNScriptingErrors.h"
#include "WNScripting/inc/WNTypeElement.h"
#include "WNLogging/inc/WNLog.h"

namespace llvm {
    class Value;
}
namespace WNScripting {
    class WNCodeModule;
    struct WNFunctionDefinition;
    class WNExpression : public WNNode {
    public:
        WNExpression();
        virtual ~WNExpression();
        llvm::Value* GetValue() const; 

        llvm::Value* GetValueLocation() const; 

        WNScripting::WNScriptType GetType() const; 
        
        WN_BOOL RequiredUse();
        WN_BOOL IsNewlyCreated();

        WNScripting::WNScriptType GetSubType() const;
        llvm::Value* GetSubValue() const;
        
        virtual eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) = 0;
    protected:
        llvm::Value* mValue;
        llvm::Value* mValueLocation;
        llvm::Value* mSubValue;
        WNScriptType mSubValueType;
        WNScriptType mScriptType;
        WN_BOOL mForceUse;
        WN_BOOL mNewlyCreated;
    };
}

#endif//__WN_EXPRESSION_H__

