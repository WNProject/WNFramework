// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_ARRAY_ALLOCATION_H__
#define __WN_ARRAY_ALLOCATION_H__
#include "WNDeprecatedScripting/inc/WNExpression.h"
#include "WNDeprecatedScripting/inc/WNScriptLinkedList.h"

namespace WNScripting {
    class WNTypeNode;
    class WNArrayAllocation : public WNExpression {
    public:
        WNArrayAllocation();
        virtual ~WNArrayAllocation();
        void SetType(WNTypeNode* _typeNode);
        void AddExpression(WNExpression* _expr);
        void AddLevel();
        void SetCopyInitializer(WNExpression* _expression);
        eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);

    private:
        WNScriptLinkedList<WNExpression> mArrayinitializers;
        WNExpression* mCopyinitializer;
        WNTypeNode* mType;
        size_t mLevels;
    };
}
#endif//__WN_ARRAY_ALLOCATION_H__
