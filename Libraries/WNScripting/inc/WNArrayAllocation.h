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
        WN_VOID SetType(WNTypeNode* _typeNode); 
        WN_VOID AddExpression(WNExpression* _expr); 
        WN_VOID AddLevel(); 
        WN_VOID SetCopyInitializer(WNExpression* _expression);
        eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog); 

    private:
        WNScriptLinkedList<WNExpression> mArrayInitializers;
        WNExpression* mCopyInitializer;
        WNTypeNode* mType;
        WN_SIZE_T mLevels;
    };
}
#endif//__WN_ARRAY_ALLOCATION_H__
