// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_FOR_INSTRUCTION_H__
#define __WN_FOR_INSTRUCTION_H__

#include "WNDeprecatedScripting/inc/WNInstruction.h"
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

  virtual eWNTypeError GenerateCode(WNCodeModule& _module,
      const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);

private:
  WNInstruction* minitializer;
  WNExpression* mCondition;
  WNInstruction* mPostOp;
  WNInstructionList* mBody;
};
}
#endif  //__WN_FOR_INSTRUCTION_H__
