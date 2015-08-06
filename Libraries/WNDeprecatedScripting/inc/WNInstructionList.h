// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_INSTRUCTION_LIST_H__
#define __WN_INSTRUCTION_LIST_H__

#include "WNDeprecatedScripting/inc/WNNode.h"
#include "WNDeprecatedScripting/inc/WNScriptLinkedList.h"
#include "WNDeprecatedScripting/inc/WNScriptingErrors.h"
#include "WNCore/inc/WNTypes.h"
#include "WNLogging/inc/WNLog.h"

namespace WNScripting {
    class WNInstruction;
    class WNCodeModule;
    struct WNFunctionDefinition;
    class WNInstructionList : public WNNode {
    public:
        WNInstructionList();
        WNInstructionList(WNInstruction* inst);
        virtual ~WNInstructionList();
        void AddInstruction(WNInstruction* inst);

        eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);

        wn_bool Returns();
    private:
        WNScriptLinkedList<WNInstruction> mInstructions;
        wn_bool mReturns;
    };
}
#endif//__WN_INSTRUCTION_LIST_H__
