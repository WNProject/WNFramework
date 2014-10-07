////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_DECLARATION_H__
#define __WN_DECLARATION_H__
#include "WNScripting/inc/WNInstruction.h"
#include "WNScripting/inc/WNTypeElement.h"
#include "WNScripting/inc/WNScriptLinkedList.h"
namespace WNScripting {
    class WNExpression;
    class WNTypeNode;
    class WNTypeManager;
    class WNDeclaration : public WNInstruction {
    public:    
        WNDeclaration(WNTypeNode* _type, const char* _name);
        virtual ~WNDeclaration();
        const WN_CHAR* GetName(); 

        eWNTypeError GetType(WNTypeManager& _manager, WNScriptType& _scriptType, WNLogging::WNLog& _compilationLog); 

        void AddExpressionInitializer(WNExpression* _expr, bool _assign = false); 

        void AddSizedArrayInitializer(WNExpression* _expr);

        void AddUnsizedArrayInitializer(); 

        virtual eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog); 
        WNExpression* GetExpression();
    private:
        WNTypeNode* mType;
        char* mName;
        WNExpression* mExpr;
        WNTypeNode* mScalarType;
        WNScriptLinkedList<WNExpression> mSizedArrayInitializers;
        WN_SIZE_T mUnsizedArrayInitializers;
        WN_BOOL mInitAssign;
    };
}
#endif//__WN_DECLARATION_H__
