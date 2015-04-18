// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_FOR_INSTRUCTION_H__
#define __WN_FOR_INSTRUCTION_H__

#include "WNScripting/inc/WNInstruction.h"
namespace WNScripting {
    class WNExpression;
    class WNInstructionList;
    class WNForInstruction : public WNInstruction {
    public:
        WNForInstruction();
        virtual ~WNForInstruction();
        void AddInitializer(WNInstruction* _init);
        void AddCondition(WNExpression* cond);
        void AddPostOp(WNInstruction* _inst);
        void AddBody(WNInstructionList* _body);

        virtual eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);
    private:
        WNInstruction* mInitializer;
        WNExpression* mCondition;
        WNInstruction* mPostOp;
        WNInstructionList* mBody;
    };
}
#endif//__WN_FOR_INSTRUCTION_H__
