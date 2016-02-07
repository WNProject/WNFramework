// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_DECL_LIST_H__
#define __WN_DECL_LIST_H__
#include "WNDeprecatedScripting/inc/WNNode.h"
#include "WNDeprecatedScripting/inc/WNScriptLinkedList.h"
#include "WNDeprecatedScripting/inc/WNScriptingErrors.h"
#include "WNDeprecatedScripting/inc/WNTypeElement.h"

#include <vector>
namespace WNScripting {
class WNDeclaration;
class WNTypeManager;
class WNDeclList : public WNNode {
public:
  WNDeclList(WNDeclaration* _decl);
  virtual ~WNDeclList();
  void AddDeclaration(WNDeclaration* _decl);

  eWNTypeError GetTypeList(WNTypeManager& _manager,
      std::vector<WNScriptType>& _vector, WNLogging::WNLog& _compilationLog);

  eWNTypeError GetDeclarations(
      const WNScriptLinkedList<WNDeclaration>*& _outDeclaration) const;

private:
  WNScriptLinkedList<WNDeclaration> mDeclarations;
};
}
#endif  //__WN_DECL_LIST_H__
