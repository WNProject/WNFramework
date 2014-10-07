////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_FUNCTION_H__
#define __WN_FUNCTION_H__

#include "WNCore/inc/WNBase.h"
#include "WNScripting/inc/WNNode.h"
#include "WNScripting/inc/WNScriptingErrors.h"
#include "WNScripting/inc/WNTypeElement.h"
#include "WNLogging/inc/WNLog.h"
#include <vector>

namespace WNScripting {
    class WNDeclaration;
    class WNDeclList;
    class WNInstructionList;
    class WNCodeModule;
    struct WNFunctionDefinition;
    class WNFunction : public WNNode {
    public:
        WNFunction(WNDeclaration* _decl, WNDeclList* _params, WNInstructionList* _body);
        virtual ~WNFunction();
        virtual eWNTypeError GenerateCode(WNCodeModule& _module, WNScriptType owningType, WNLogging::WNLog& _compilationLog);
    
        eWNTypeError ExportFunctions(WNCodeModule& _module, WNScriptType owningType, std::vector<WNFunctionDefinition*>& _definitions, WNLogging::WNLog& _compilationLog);

        eWNTypeError GenerateHeader(WNCodeModule& _module, WNScriptType owningType, WNLogging::WNLog& _compilationLog);

        void SetVirtual();
        void SetOverride();
    private:

        WNDeclaration* mDeclaration;
        WNDeclList* mParameters;
        WNInstructionList* mBody;
        WNScriptType mThisType;
        bool mIsOverride;
        bool mIsVirtual;
    };
}
#endif //__WN_FUNCTION_H__ 
