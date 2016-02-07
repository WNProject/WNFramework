// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_FUNCTION_PARAM_H__
#define __WN_FUNCTION_PARAM_H__

#include "WNDeprecatedScripting/inc/WNTypeElement.h"

namespace llvm {
class Value;
}

namespace WNScripting {
struct FunctionParam {
  llvm::Value* mValue;
  WNScriptType mType;
};
}

#endif  //__WN_FUNCTION_PARAM_H__
