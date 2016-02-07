// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNDeprecatedScripting/inc/WNScriptingFactoryInternal.h"
#include "WNDeprecatedScripting/inc/WNScopedVaraiableListImpl.h"
#include "WNDeprecatedScripting/inc/WNTypeManagerImpl.h"
#include "WNMemory/inc/WNBasic.h"

WNScripting::WNTypeManager*
WNScripting::WNScriptingFactoryInternal::CreateTypeManager() {
  return (wn::memory::construct<WNScripting::WNTypeManagerImpl>());
}

WNScripting::WNScopedVariableList*
WNScripting::WNScriptingFactoryInternal::CreateScopedVariableList() {
  return (wn::memory::construct<WNScripting::WNScopedVariableListImpl>());
}