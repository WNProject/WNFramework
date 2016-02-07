// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_IF_INSTRUCTION_H__
#define __WN_IF_INSTRUCTION_H__

#include "WNDeprecatedScripting/inc/WNInstruction.h"
#include "WNDeprecatedScripting/inc/WNScriptLinkedList.h"
namespace WNScripting {
class WNExpression;
class WNInstructionList;
class WNElseIf : public WNInstruction {
public:
  WNElseIf(WNExpression* _cond, WNInstructionList* _body);
  virtual ~WNElseIf();

  virtual eWNTypeError GenerateCode(WNCodeModule& _module,
      const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);

  WNExpression* GetCondition();

  WNInstructionList* GetBody();

private:
  WNExpression* mCondition;
  WNInstructionList* mBody;
};

class WNIFInstruction : public WNInstruction {
public:
  WNIFInstruction(WNExpression* _cond, WNInstructionList* _body);
  virtual ~WNIFInstruction();
  void AddElseIf(WNElseIf* _elseif);
  void AddElse(WNInstructionList* _else);

  virtual eWNTypeError GenerateCode(WNCodeModule& _module,
      const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);

private:
  WNExpression* mCondition;
  WNInstructionList* mElse;
  WNInstructionList* mBody;
  WNScriptLinkedList<WNElseIf> mElseIfNodes;
};
}
#endif  //__WN_IF_INSTRUCTION_H__
