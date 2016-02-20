// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_WHILE_INSTRUCTION_H__
#define __WN_WHILE_INSTRUCTION_H__

#include "WNDeprecatedScripting/inc/WNInstruction.h"

namespace WNScripting {
class WNExpression;
class WNInstructionList;
class WNWhileInstruction : public WNInstruction {
public:
  WNWhileInstruction(WNExpression* _cond, WNInstructionList* _body);
  virtual ~WNWhileInstruction();
  virtual eWNTypeError GenerateCode(WNCodeModule& _module,
      const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);

private:
  WNExpression* mCondition;
  WNInstructionList* mBody;
};
}
#endif  //__WN_WHILE_INSTRUCTION_H__
