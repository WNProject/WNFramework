// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_ASSIGNMENT_H__
#define __WN_ASSIGNMENT_H__
#include "WNDeprecatedScripting/inc/WNInstruction.h"
#include "WNDeprecatedScripting/inc/WNScriptingEnums.h"

namespace WNScripting {
class WNLValue;
class WNExpression;
class WNCodeModule;
class WNAssignment : public WNInstruction {
public:
  WNAssignment(WNLValue* _lValue);
  virtual ~WNAssignment();
  void AddValue(WNAssignType _type, WNExpression* value);

  virtual eWNTypeError GenerateCode(WNCodeModule& _module,
      const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);

private:
  WNAssignType mAssignType;
  WNLValue* mLValue;
  WNExpression* mAssignExpression;
};
}
#endif  //__WN_ASSIGNMENT_H__
