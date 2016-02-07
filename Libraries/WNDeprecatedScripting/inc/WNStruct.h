// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_STRUCT_H__
#define __WN_STRUCT_H__
#include "WNDeprecatedScripting/inc/WNFunction.h"
#include "WNDeprecatedScripting/inc/WNNode.h"
#include "WNDeprecatedScripting/inc/WNScriptLinkedList.h"
#include "WNDeprecatedScripting/inc/WNScriptingErrors.h"
#include "WNDeprecatedScripting/inc/WNTypeElement.h"

#include <vector>
#include "WNLogging/inc/WNLog.h"

namespace WNScripting {
class WNDeclaration;
class WNCodeModule;
class WNTypeManager;
struct WNFunctionDefinition;
class WNStruct : public WNNode {
public:
  WNStruct(
      const char* _name, bool _isClass = false, const char* _parentType = NULL);
  virtual ~WNStruct();

  void AddStructElem(WNDeclaration* _decl);
  void AddFunction(WNFunction* _func);
  eWNTypeError ExportNewTypes(WNCodeModule& _module,
      std::vector<WNScriptType>& _types,
      std::vector<WNFunctionDefinition*>& _functionDefs,
      WNLogging::WNLog& _compilationLog);
  eWNTypeError FinalizeNewTypes(WNCodeModule& _module,
      std::vector<WNFunctionDefinition*>& _definitions,
      WNLogging::WNLog& _compilationLog);
  eWNTypeError FinalizeStruct(
      WNTypeManager& _manager, WNLogging::WNLog& _compilationLog);
  eWNTypeError GenerateHeader(
      WNCodeModule& _module, WNLogging::WNLog& _compilationLog);
  eWNTypeError GenerateCode(
      WNCodeModule& _module, WNLogging::WNLog& _compilationLog);
  char* GetName() const;
  WNScriptType GetType() const;

private:
  eWNTypeError GenerateConstructor(
      WNCodeModule& _module, WNLogging::WNLog& _compilationLog);
  eWNTypeError GenerateDestructor(
      WNCodeModule& _module, WNLogging::WNLog& _compilationLog);
  eWNTypeError GenerateCopyConstructor(
      WNCodeModule& _module, WNLogging::WNLog& _compilationLog);
  char* mName;
  char* mParentName;
  bool mIsClass;
  WNScriptType mParentType;
  WNScriptType mType;
  WNScriptLinkedList<WNDeclaration> mStructMembers;
  WNScriptLinkedList<WNFunction> mStructFunctions;
};
}
#endif  //__WN_STRUCT_H__
