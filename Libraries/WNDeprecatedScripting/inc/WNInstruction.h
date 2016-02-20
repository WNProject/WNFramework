// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_INSTRUCTION_H__
#define __WN_INSTRUCTION_H__
#include "WNCore/inc/WNTypes.h"
#include "WNDeprecatedScripting/inc/WNNode.h"
#include "WNDeprecatedScripting/inc/WNScriptingErrors.h"
#include "WNLogging/inc/WNLog.h"

namespace WNScripting {
struct WNFunctionDefinition;
class WNCodeModule;
class WNInstruction : public WNNode {
public:
  WNInstruction();
  virtual ~WNInstruction();
  virtual eWNTypeError GenerateCode(WNCodeModule&, const WNFunctionDefinition*,
      WNLogging::WNLog& _compilationLog) = 0;
  bool Returns();

protected:
  bool mReturns;
};
}
#endif  //__WN_INSTRUCTION_H__
