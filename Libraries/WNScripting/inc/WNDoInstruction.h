// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_DO_INSTRUCTION_H__
#define __WN_DO_INSTRUCTION_H__

#include "WNScripting/inc/WNInstruction.h"
namespace WNScripting {
    class WNExpression;
    class WNInstructionList;
    class WNDoInstruction : public WNInstruction {
    public:
        WNDoInstruction(WNExpression* _cond, WNInstructionList* _body);
        virtual ~WNDoInstruction();
        virtual eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);
    private:
        WNExpression* mCondition;
        WNInstructionList* mBody;
    };
}
#endif//__WN_DO_INSTRUCTION_H__


