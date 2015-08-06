// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_DECLARATION_H__
#define __WN_DECLARATION_H__
#include "WNDeprecatedScripting/inc/WNInstruction.h"
#include "WNDeprecatedScripting/inc/WNTypeElement.h"
#include "WNDeprecatedScripting/inc/WNScriptLinkedList.h"
namespace WNScripting {
    class WNExpression;
    class WNTypeNode;
    class WNTypeManager;
    class WNDeclaration : public WNInstruction {
    public:
        WNDeclaration(WNTypeNode* _type, const char* _name);
        virtual ~WNDeclaration();
        const wn_char* GetName();

        eWNTypeError GetType(WNTypeManager& _manager, WNScriptType& _scriptType, WNLogging::WNLog& _compilationLog);

        void AddExpressionInitializer(WNExpression* _expr, bool _assign = false);

        void AddSizedArrayinitializer(WNExpression* _expr);

        void AddUnsizedArrayinitializer();

        virtual eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);
        WNExpression* GetExpression();
    private:
        WNTypeNode* mType;
        char* mName;
        WNExpression* mExpr;
        WNTypeNode* mScalarType;
        WNScriptLinkedList<WNExpression> mSizedArrayinitializers;
        wn_size_t mUnsizedArrayinitializers;
        wn_bool mInitAssign;
    };
}
#endif//__WN_DECLARATION_H__
