// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_TYPE_NODE_H__
#define __WN_TYPE_NODE_H__
#include "WNCore/inc/WNTypes.h"
#include "WNDeprecatedScripting/inc/WNNode.h"
#include "WNDeprecatedScripting/inc/WNScriptingEnums.h"
#include "WNDeprecatedScripting/inc/WNScriptingErrors.h"
#include "WNDeprecatedScripting/inc/WNTypeElement.h"

namespace WNScripting {
class WNTypeManager;
class WNTypeNode : public WNNode {
public:
  WNTypeNode(const char* _customType);
  virtual ~WNTypeNode();
  void AddArrayLevel();

  eWNTypeError GetType(WNTypeManager& _module, WNScriptType& _outType,
      WNLogging::WNLog& _compilationLog) const;

private:
  WNScriptTypeName mType;
  char* mCustomType;
  size_t mNumArrayLevels;
};
}
#endif  //__WN_TYPE_NODE_H__
