// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNScriptingFactoryInternal.h"
#include "WNScripting/inc/WNTypeManagerImpl.h"
#include "WNMemory/inc/WNAllocation.h"
#include "WNScripting/inc/WNScopedVaraiableListImpl.h"

WNScripting::WNTypeManager* WNScripting::WNScriptingFactoryInternal::CreateTypeManager() {
    return(wn::memory::construct<WNScripting::WNTypeManagerImpl>());
}

WNScripting::WNScopedVariableList* WNScripting::WNScriptingFactoryInternal::CreateScopedVariableList() {
    return(wn::memory::construct<WNScripting::WNScopedVariableListImpl>());
}