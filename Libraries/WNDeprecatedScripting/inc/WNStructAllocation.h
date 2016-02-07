// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_STRUCT_ALLOCATION_H__
#define __WN_STRUCT_ALLOCATION_H__
#include "WNDeprecatedScripting/inc/WNExpression.h"

namespace WNScripting {
class WNTypeNode;
class WNStructAllocation : public WNExpression {
public:
  WNStructAllocation();
  virtual ~WNStructAllocation();
  void SetType(WNTypeNode* _type);
  void SetCopyInitializer(WNExpression* _expression);
  eWNTypeError GenerateCode(WNCodeModule& _module,
      const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);

private:
  WNTypeNode* mType;
  WNExpression* mCopyinitializer;
};
}
#endif  //__WN_STRUCT_ALOCATION_H__
