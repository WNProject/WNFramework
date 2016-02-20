// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_FUNCTION_H__
#define __WN_FUNCTION_H__

#include <vector>
#include "WNCore/inc/WNBase.h"
#include "WNDeprecatedScripting/inc/WNNode.h"
#include "WNDeprecatedScripting/inc/WNScriptingErrors.h"
#include "WNDeprecatedScripting/inc/WNTypeElement.h"
#include "WNLogging/inc/WNLog.h"

namespace WNScripting {
class WNDeclaration;
class WNDeclList;
class WNInstructionList;
class WNCodeModule;
struct WNFunctionDefinition;
class WNFunction : public WNNode {
public:
  WNFunction(
      WNDeclaration* _decl, WNDeclList* _params, WNInstructionList* _body);
  virtual ~WNFunction();
  virtual eWNTypeError GenerateCode(WNCodeModule& _module,
      WNScriptType owningType, WNLogging::WNLog& _compilationLog);

  eWNTypeError ExportFunctions(WNCodeModule& _module, WNScriptType owningType,
      std::vector<WNFunctionDefinition*>& _definitions,
      WNLogging::WNLog& _compilationLog);

  eWNTypeError GenerateHeader(WNCodeModule& _module, WNScriptType owningType,
      WNLogging::WNLog& _compilationLog);

  void SetVirtual();
  void SetOverride();

private:
  WNDeclaration* mDeclaration;
  WNDeclList* mParameters;
  WNInstructionList* mBody;
  WNScriptType mThisType;
  bool mIsOverride;
  bool mIsVirtual;
};
}
#endif  //__WN_FUNCTION_H__
