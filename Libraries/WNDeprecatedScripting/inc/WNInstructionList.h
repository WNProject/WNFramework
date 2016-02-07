// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_INSTRUCTION_LIST_H__
#define __WN_INSTRUCTION_LIST_H__

#include "WNCore/inc/WNTypes.h"
#include "WNDeprecatedScripting/inc/WNNode.h"
#include "WNDeprecatedScripting/inc/WNScriptLinkedList.h"
#include "WNDeprecatedScripting/inc/WNScriptingErrors.h"
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

  eWNTypeError GenerateCode(WNCodeModule& _module,
      const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);

  bool Returns();

private:
  WNScriptLinkedList<WNInstruction> mInstructions;
  bool mReturns;
};
}
#endif  //__WN_INSTRUCTION_LIST_H__
