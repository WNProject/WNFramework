// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#ifndef __WN_STRUCT_ALLOCATION_H__
#define __WN_STRUCT_ALLOCATION_H__
#include "WNScripting/inc/WNExpression.h"

namespace WNScripting {    
    class WNTypeNode;
    class WNStructAllocation : public WNExpression {
    public:
        WNStructAllocation();
        virtual ~WNStructAllocation();
        WN_VOID SetType(WNTypeNode* _type);
        WN_VOID SetCopyInitializer(WNExpression* _expression); 
        eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);
    private:
        WNTypeNode* mType;
        WNExpression* mCopyInitializer;
    };
}
#endif//__WN_STRUCT_ALOCATION_H__
