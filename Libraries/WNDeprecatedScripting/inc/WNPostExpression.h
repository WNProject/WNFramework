// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_POST_EXPRESSION_H__
#define __WN_POST_EXPRESSION_H__
#include "WNDeprecatedScripting/inc/WNExpression.h"

namespace WNScripting {
class WNPostExpression : public WNExpression {
public:
  WNPostExpression();
  virtual ~WNPostExpression();
  void AddBaseExpr(WNExpression* _expr);

protected:
  WNExpression* mBaseExpression;
};
}
#endif  //__WN_POST_EXPRESSION_H__
