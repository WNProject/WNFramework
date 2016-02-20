// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_SCOPED_VARIABLE_LIST_IMPL_H__
#define __WN_SCRIPTING_SCOPED_VARIABLE_LIST_IMPL_H__
#include <vector>
#include "WNDeprecatedScripting/inc/WNScopedVariableList.h"
namespace WNLogging {
class WNLog;
};
namespace WNScripting {
class WNScopedVariableListImpl : public WNScopedVariableList {
public:
  virtual ~WNScopedVariableListImpl();
  virtual eWNTypeError PushVariable(WNScriptVariable* _variable);
  virtual void PushScopeBlock(WNCodeModule& _module);
  virtual eWNTypeError PopScopeBlock(WNCodeModule& _module, bool _cleanStack,
      const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);
  virtual eWNTypeError GenerateReturn(WNCodeModule& _module,
      const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);
  virtual const WNScriptVariable* GetVariable(const char* _functionName);
  virtual void ClearScope();

private:
  std::vector<WNScriptVariable*> mScriptVariables;
  std::vector<llvm::Value*> mScopeBlocks;
};
}

#endif  //__WN_SCRIPTING_SCOPED_VARIABLE_LIST_IMPL_H__
