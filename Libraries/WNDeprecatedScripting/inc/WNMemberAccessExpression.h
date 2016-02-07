// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_MEMBER_ACCESS_EXPR_H__
#define __WN_MEMBER_ACCESS_EXPR_H__

#include "WNDeprecatedScripting/inc/WNPostExpression.h"

namespace WNScripting {
class WNMemberAccessExpr : public WNPostExpression {
public:
  WNMemberAccessExpr(const char* _member);
  virtual ~WNMemberAccessExpr();
  virtual eWNTypeError GenerateCode(WNCodeModule& _module,
      const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);

private:
  char* mMember;
};
}
#endif  //__WN_MEMBER_ACCESS_EXPR_H__
