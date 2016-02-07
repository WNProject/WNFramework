// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_FUNCTION_CALL_EXPR_H__
#define __WN_FUNCTION_CALL_EXPR_H__

#include "WNDeprecatedScripting/inc/WNPostExpression.h"
namespace WNScripting {
class WNArgList;
class WNFunctionCallExpr : public WNPostExpression {
public:
  WNFunctionCallExpr();
  WNFunctionCallExpr(WNArgList* _list);
  virtual ~WNFunctionCallExpr();
  virtual eWNTypeError GenerateCode(WNCodeModule& _module,
      const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);

private:
  WNArgList* mArgs;
};
}
#endif  //__WN_FUNCTION_CALL_EXPR_H__
