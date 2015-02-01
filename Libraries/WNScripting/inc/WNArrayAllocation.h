// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_ARRAY_ALLOCATION_H__
#define __WN_ARRAY_ALLOCATION_H__
#include "WNScripting/inc/WNExpression.h"
#include "WNScripting/inc/WNScriptLinkedList.h"

namespace WNScripting {
    class WNTypeNode;
    class WNArrayAllocation : public WNExpression {
    public:
        WNArrayAllocation();
        virtual ~WNArrayAllocation();
        wn_void SetType(WNTypeNode* _typeNode);
        wn_void AddExpression(WNExpression* _expr);
        wn_void AddLevel();
        wn_void SetCopyInitializer(WNExpression* _expression);
        eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);

    private:
        WNScriptLinkedList<WNExpression> mArrayInitializers;
        WNExpression* mCopyInitializer;
        WNTypeNode* mType;
        wn_size_t mLevels;
    };
}
#endif//__WN_ARRAY_ALLOCATION_H__
